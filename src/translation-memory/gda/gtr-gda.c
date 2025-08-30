/*
 * Copyright (C) 2010  Andrey Kutejko <andy128k@gmail.com>
 *
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sqlite3.h>
#include "gtr-gda.h"
#include "gtr-translation-memory.h"
#include "gtr-dirs.h"
#include "gda-utils.h"

#include <glib.h>
#include <glib-object.h>
#include <string.h>

G_DEFINE_QUARK (gtr_gda_error, gtr_gda_error)

static void
gtr_translation_memory_iface_init (GtrTranslationMemoryInterface * iface);

typedef struct
{
  sqlite3 *db;

  /* prepared statements */
  sqlite3_stmt *stmt_find_orig;
  sqlite3_stmt *stmt_select_trans;
  sqlite3_stmt *stmt_select_word;
  sqlite3_stmt *stmt_find_trans;

  sqlite3_stmt *stmt_insert_orig;
  sqlite3_stmt *stmt_insert_word;
  sqlite3_stmt *stmt_insert_link;
  sqlite3_stmt *stmt_insert_trans;

  sqlite3_stmt *stmt_delete_trans;

  guint max_omits;
  guint max_delta;
  gint max_items;

  GHashTable *lookup_query_cache;
} GtrGdaPrivate;

G_DEFINE_FINAL_TYPE_WITH_CODE (GtrGda,
                               gtr_gda,
                               G_TYPE_OBJECT,
                               G_ADD_PRIVATE (GtrGda)
                               G_IMPLEMENT_INTERFACE (GTR_TYPE_TRANSLATION_MEMORY,
                                                      gtr_translation_memory_iface_init))

static void
bind_int (sqlite3_stmt *stmt, int idx, int value)
{
  int rc = sqlite3_bind_int (stmt, idx, value);
  if (rc != SQLITE_OK)
    g_critical ("Could not bind int %d to statement at idx %d", value, idx);
}

static void
bind_text (sqlite3_stmt *stmt, int idx, const char *txt)
{
  int rc = sqlite3_bind_text (stmt, idx, txt, -1, SQLITE_TRANSIENT);
  if (rc != SQLITE_OK)
    g_critical ("Could not bind text %s to statement at idx %d", txt, idx);
}

static gint
select_integer (sqlite3      *db,
                sqlite3_stmt *stmt,
                GError      **error,
                int           n_args,
                ...)
{
  va_list args;
  int result = 0;

  va_start (args, n_args);

  for (int i = 0; i < n_args; i++)
    {
      GType type = va_arg (args, GType);
      if (type == G_TYPE_STRING)
        {
          const char *charg = va_arg (args, const char *);
          bind_text (stmt, i + 1, charg);
        }
      else if (type == G_TYPE_INT)
        {
          int iarg = va_arg (args, int);
          bind_int (stmt, i + 1, iarg);
        }
      else
        {
          g_critical ("Unknown type when selecting integer");
        }
    }

  va_end (args);

  int rc = sqlite3_step (stmt);
  if (rc == SQLITE_ROW)
    {
      result = sqlite3_column_int (stmt, 0);
    }
  else if (rc != SQLITE_DONE)
    {
      result = -1;
      if (error)
        *error = g_error_new_literal (GTR_GDA_ERROR, rc,
                                      sqlite3_errmsg (db));
    }
  sqlite3_reset (stmt);
  sqlite3_clear_bindings (stmt);

  return result;
}

static int
insert_row (sqlite3      *db,
            sqlite3_stmt *stmt,
            GError      **error,
            int           n_args,
            ...)
{
  va_list args;
  int rc;
  int rowid;

  va_start (args, n_args);

  for (int i = 0; i < n_args; i++)
    {
      GType type = va_arg (args, GType);
      if (type == G_TYPE_STRING)
        {
          const char *charg = va_arg (args, const char *);
          bind_text (stmt, i + 1, charg);
        }
      else if (type == G_TYPE_INT)
        {
          int iarg = va_arg (args, int);
          bind_int (stmt, i + 1, iarg);
        }
      else
        {
          g_critical ("Unknown type when inserting row");
        }
    }

  va_end (args);

  rc = sqlite3_step (stmt);

  sqlite3_reset (stmt);
  sqlite3_clear_bindings (stmt);

  if (rc != SQLITE_DONE)
    {
      if (error)
        *error = g_error_new_literal (GTR_GDA_ERROR, rc,
                                      sqlite3_errmsg (db));

      return 0;
    }

  rowid = sqlite3_last_insert_rowid (db);

  return rowid;
}

static int
execute_non_select_command (sqlite3    *db,
                            const char *sql)
{
  char *errmsg = NULL;
  int rc = sqlite3_exec (db, sql, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK)
    {
      g_warning ("SQLite error: %s", errmsg);
      sqlite3_free (errmsg);
    }
  return rc;
}

static gboolean
begin_transaction (sqlite3 *db, GError **error)
{
  char *errmsg = NULL;
  int rc = sqlite3_exec (db, "BEGIN TRANSACTION", NULL, NULL, &errmsg);
  if (rc != SQLITE_OK)
    {
      if (error)
        *error = g_error_new_literal (GTR_GDA_ERROR, rc,
                                      sqlite3_errmsg (db));
      g_critical ("Could not begin transaction: %s", errmsg);
      sqlite3_free (errmsg);
    }
  return rc == SQLITE_OK;
}

static void
commit_transaction (sqlite3 *db)
{
  char *errmsg = NULL;
  int rc = sqlite3_exec (db, "COMMIT", NULL, NULL, &errmsg);
  if (rc != SQLITE_OK)
    {
      g_critical ("Could not commit transaction: %s", errmsg);
      sqlite3_free (errmsg);
    }
}

static void
rollback_transaction (sqlite3 *db)
{
  char *errmsg = NULL;
  int rc = sqlite3_exec (db, "ROLLBACK", NULL, NULL, &errmsg);
  if (rc != SQLITE_OK)
    {
      g_critical ("Could not rollback transaction: %s", errmsg);
      sqlite3_free (errmsg);
    }
}

static void
execute_non_select (sqlite3      *db,
                    sqlite3_stmt *stmt,
                    int           value1,
                    int           value2,
                    GError      **error)
{
  if (value1 >= 0)
    bind_int (stmt, 1, value1);

  if (value2 >= 0)
    bind_int (stmt, 2, value2);

  int rc = sqlite3_step (stmt);

  sqlite3_reset (stmt);
  sqlite3_clear_bindings (stmt);

  if (rc != SQLITE_DONE && error)
    *error = g_error_new_literal (GTR_GDA_ERROR, rc,
                                  sqlite3_errmsg (db));
}

static int
string_comparator (const void *s1, const void *s2)
{
  return strcmp (*(const gchar **) s1, *(const gchar **) s2);
}

static GStrv
gtr_gda_split_string_in_words (const gchar *phrase)
{
  GStrv words = gtr_gda_utils_split_string_in_words (phrase);
  gsize count = g_strv_length (words);
  gint w;
  gint r;

  if (count <= 1)
    return words;

  qsort (words, count, sizeof (gchar *), string_comparator);

  w = 1;
  r = 1;
  for (; r < count; ++r)
    {
      if (0 == strcmp (words[r], words[w-1]))
        {
          g_free (words[r]);
        }
      else
        {
          words[w] = words[r];
          ++w;
        }
    }
  words[w] = NULL;

  return words;
}

static void
gtr_gda_words_append (GtrGda *self,
                      const gchar * word,
                      gint orig_id,
                      GError **error)
{
  GError *inner_error;
  gint word_id = 0;
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);

  /* look for word */
  {
    inner_error = NULL;
    word_id = select_integer (priv->db, priv->stmt_select_word, &inner_error,
                              1, G_TYPE_STRING, word);
    if (inner_error)
      {
        g_propagate_error (error, inner_error);
        return;
      }
  }

  if (word_id == 0)
    {
      inner_error = NULL;
      word_id = insert_row (priv->db, priv->stmt_insert_word, &inner_error, 1,
                            G_TYPE_STRING, word);
      if (inner_error)
        {
          g_propagate_error (error, inner_error);
          return;
        }
    }

  /* insert link */
  {
    inner_error = NULL;
    execute_non_select (priv->db, priv->stmt_insert_link,
                        word_id, orig_id,
                        &inner_error);

    if (inner_error)
      g_propagate_error (error, inner_error);
  }
}

static gboolean
gtr_gda_store_impl (GtrGda *self,
                    const gchar * original,
                    const gchar * translation,
                    GError **error)
{
  gint orig_id;
  gboolean found_translation = FALSE;
  g_auto(GStrv) words = NULL;
  GError *inner_error;
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);

  inner_error = NULL;
  orig_id = select_integer (priv->db,
                            priv->stmt_find_orig,
                            &inner_error,
                            1, G_TYPE_STRING, original);

  if (inner_error)
    {
      g_propagate_error (error, inner_error);
      return FALSE;
    }

  if (orig_id == 0)
    {
      gsize sz, i;

      words = gtr_gda_split_string_in_words (original);
      sz = g_strv_length (words);

      inner_error = NULL;
      orig_id = insert_row (priv->db,
                  priv->stmt_insert_orig,
                  &inner_error,
                  2,
                  G_TYPE_STRING, original,
                  G_TYPE_INT, sz);
      if (inner_error)
        goto error;

      /* insert words */
      for (i = 0; i < sz; i++)
        {
          inner_error = NULL;
          gtr_gda_words_append (self, words[i], orig_id, &inner_error);
          if (inner_error)
            goto error;
        }
    }
  else
    {
      inner_error = NULL;
      found_translation = select_integer (priv->db,
                                          priv->stmt_find_trans,
                                          &inner_error,
                                          2, G_TYPE_INT, orig_id, G_TYPE_STRING, translation);
      if (inner_error)
        goto error;
    }

  if (!found_translation)
    {
      inner_error = NULL;
      insert_row (priv->db,
                  priv->stmt_insert_trans,
                  &inner_error,
                  2,
                  G_TYPE_INT, orig_id,
                  G_TYPE_STRING, translation);

      if (inner_error)
        goto error;
    }

  return TRUE;

 error:
  g_propagate_error (error, inner_error);
  return FALSE;
}

static gboolean
gtr_gda_store (GtrTranslationMemory * tm, GtrMsg * msg)
{
  GtrGda *self = GTR_GDA (tm);
  gboolean result;
  GError *error;
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);

  g_return_val_if_fail (GTR_IS_GDA (self), FALSE);

  error = NULL;
  if (!begin_transaction (priv->db, &error))
    {
      g_warning ("starting transaction failed: %s", error->message);
      g_error_free (error);
      return FALSE;
    }

  error = NULL;
  result = gtr_gda_store_impl (self,
                               gtr_msg_get_msgid (msg),
                               gtr_msg_get_msgstr (msg),
                               &error);

  if (error)
    {
      g_warning ("storing message failed: %s", error->message);
      g_error_free (error);
    }

  if (result)
    commit_transaction (priv->db);
  else
    rollback_transaction (priv->db);

  return result;
}

static gboolean
gtr_gda_store_list (GtrTranslationMemory * tm, GList * msgs)
{
  GtrGda *self = GTR_GDA (tm);
  gboolean result = TRUE;
  GList *l;
  GError *error;
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);

  g_return_val_if_fail (GTR_IS_GDA (self), FALSE);

  g_debug ("Storing list of messages in translation memory");

  error = NULL;
  if (!begin_transaction (priv->db, &error))
    {
      g_warning ("starting transaction failed: %s", error->message);
      g_error_free (error);
      return FALSE;
    }

  for (l = msgs; l; l = g_list_next (l))
    {
      GtrMsg *msg = GTR_MSG (l->data);

      if (!gtr_msg_is_translated (msg) || gtr_msg_is_fuzzy (msg))
        continue;

      error = NULL;
      result = gtr_gda_store_impl (self,
                                   gtr_msg_get_msgid (msg),
                                   gtr_msg_get_msgstr (msg),
                                   &error);
      if (error)
        {
          g_warning ("storing message failed: %s", error->message);
          g_error_free (error);
          break;
        }
    }

  if (result)
    commit_transaction (priv->db);
  else
    rollback_transaction (priv->db);

  return result;
}

static void
free_match (gpointer data)
{
  GtrTranslationMemoryMatch *match = (GtrTranslationMemoryMatch *) data;

  g_free (match->match);
  g_free (match);
}

static gchar*
build_lookup_query (GtrGda *self, guint word_count)
{
  GString * query = g_string_sized_new (1024);
  guint i;
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);

  g_string_append_printf (query,
                          "select "
                          "    TRANS.VALUE, "
                          "    100 SCORE, "
                          "    TRANS.ID "
                          "from "
                          "     TRANS, ORIG "
                          "where ORIG.ID = TRANS.ORIG_ID "
                          "  and ORIG.VALUE = ?1 "
                          "union "
                          "select "
                          "    TRANS.VALUE, "
                          "    SC SCORE, "
                          "    TRANS.ID "
                          "from TRANS, "
                          "     (select "
                          "          ORIG.ID ORID, "
                          "          cast(count(1) * count(1) * 100 "
                          "               / (%d * ORIG.SENTENCE_SIZE + 1) "
                          "            as integer) SC "
                          "      from "
                          "          WORD, WORD_ORIG_LINK, ORIG "
                          "      where WORD.ID = WORD_ORIG_LINK.WORD_ID "
                          "        and ORIG.ID = WORD_ORIG_LINK.ORIG_ID "
                          "        and ORIG.VALUE <> ?1 "
                          "        and ORIG.SENTENCE_SIZE between %u and %u "
                          "        and WORD.VALUE in (",
                          word_count,
                          word_count,
                          word_count + priv->max_delta);

  for (i = 0; i < word_count; ++i)
    {
      g_string_append_printf (query, "?%u", i + 2);
      if (i != word_count - 1)
        g_string_append (query, ", ");
    }

  g_string_append_printf (query,
                          ") "
                          "     group by ORIG.ID "
                          "     having count(1) >= %d) "
                          "where ORID = TRANS.ORIG_ID "
                          "order by SCORE desc "
                          "limit %d",
                          word_count - priv->max_omits,
                          priv->max_items);

  return g_string_free_and_steal (query);
}

static sqlite3_stmt *
gtr_gda_get_lookup_statement (GtrGda *self, guint word_count, GError **error)
{
  sqlite3_stmt *stmt = NULL;
  g_autofree gchar *query = NULL;
  int rc;
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);

  stmt = g_hash_table_lookup (priv->lookup_query_cache,
                              GUINT_TO_POINTER (word_count));

  if (stmt)
    return stmt;

  query = build_lookup_query (self, word_count);

  rc = sqlite3_prepare_v2 (priv->db, query, -1, &stmt, NULL);

  if (rc != SQLITE_OK)
    {
      if (error)
        *error = g_error_new_literal (GTR_GDA_ERROR, rc,
                                      sqlite3_errmsg (priv->db));

      return NULL;
    }

  // The hashmap takes ownership of the SQlite3 statement
  g_hash_table_insert (priv->lookup_query_cache,
                       GUINT_TO_POINTER (word_count),
                       stmt);

  return stmt;
}

static GList *
gtr_gda_lookup (GtrTranslationMemory * tm, const gchar * phrase)
{
  GtrGda *self = GTR_GDA (tm);
  g_auto(GStrv) words = NULL;
  guint cnt = 0;
  GList *matches = NULL;
  GError *inner_error;
  sqlite3_stmt *stmt = NULL;
  int rc;
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);

  g_return_val_if_fail (GTR_IS_GDA (self), NULL);

  words = gtr_gda_split_string_in_words (phrase);
  if (!words)
    {
      g_warning ("Failed to split string into words");
      return NULL;
    }
  cnt = g_strv_length (words);

  inner_error = NULL;
  stmt = gtr_gda_get_lookup_statement (self, cnt, &inner_error);
  if (inner_error)
    goto end;

  bind_text (stmt, 1, phrase);

  for (int i = 0; i < cnt; ++i)
    bind_text (stmt, i + 2, words[i]);

  while ((rc = sqlite3_step (stmt)) == SQLITE_ROW)
    {
      const unsigned char *val = sqlite3_column_text (stmt, 0);
      int score = sqlite3_column_int (stmt, 1);
      int id = sqlite3_column_int (stmt, 2);

      GtrTranslationMemoryMatch *match = g_new0 (GtrTranslationMemoryMatch, 1);
      match->match = g_strdup ((const char *)val);
      match->level = score;
      match->id = id;
      matches = g_list_prepend (matches, match);
    }

  sqlite3_reset (stmt);
  sqlite3_clear_bindings (stmt);

  if (rc != SQLITE_DONE)
    inner_error = g_error_new_literal (GTR_GDA_ERROR, rc,
                                       sqlite3_errmsg (priv->db));

 end:
  if (stmt)
    {
      sqlite3_reset (stmt);
      sqlite3_clear_bindings (stmt);
    }
  if (inner_error)
    {
      g_list_free_full (matches, free_match);

      g_warning ("%s\n", inner_error->message);

      g_error_free (inner_error);

      return NULL;
    }

  matches = g_list_reverse (matches);
  return matches;
}

static void
gtr_gda_set_max_omits (GtrTranslationMemory * tm, gsize omits)
{
  GtrGda *self = GTR_GDA (tm);
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);

  if (priv->max_omits == omits)
    return;

  g_debug ("Running gtr_gda_set_max_omits: %ld", omits);

  priv->max_omits = omits;
  g_hash_table_remove_all (priv->lookup_query_cache);
}

static void
gtr_gda_set_max_delta (GtrTranslationMemory * tm, gsize delta)
{
  GtrGda *self = GTR_GDA (tm);
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);

  if (priv->max_delta == delta)
    return;

  g_debug ("Running gtr_gda_set_max_delta: %ld", delta);

  priv->max_delta = delta;
  g_hash_table_remove_all (priv->lookup_query_cache);
}

static void
gtr_gda_set_max_items (GtrTranslationMemory * tm, gint items)
{
  GtrGda *self = GTR_GDA (tm);
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);

  if (priv->max_items == items)
    return;

  g_debug ("Running gtr_gda_set_max_items: %d", items);

  priv->max_items = items;
  g_hash_table_remove_all (priv->lookup_query_cache);
}

static void
gtr_translation_memory_iface_init (GtrTranslationMemoryInterface * iface)
{
  iface->store = gtr_gda_store;
  iface->store_list = gtr_gda_store_list;
  iface->lookup = gtr_gda_lookup;
  iface->set_max_omits = gtr_gda_set_max_omits;
  iface->set_max_delta = gtr_gda_set_max_delta;
  iface->set_max_items = gtr_gda_set_max_items;
}

static void
initialize_db (GtrGda *self)
{
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);

  execute_non_select_command (priv->db,
                              "create table if not exists WORD ("
                              "ID integer primary key autoincrement,"
                              "VALUE text unique)");

  execute_non_select_command (priv->db,
                              "create table if not exists WORD_ORIG_LINK ("
                              "WORD_ID integer,"
                              "ORIG_ID integer,"
                              "primary key (WORD_ID, ORIG_ID))");

  execute_non_select_command (priv->db,
                              "create table if not exists ORIG ("
                              "ID integer primary key autoincrement,"
                              "VALUE text unique,"
                              "SENTENCE_SIZE integer)");

  execute_non_select_command (priv->db,
                              "create table if not exists TRANS ("
                              "ID integer primary key autoincrement,"
                              "ORIG_ID integer,"
                              "VALUE text)");

  execute_non_select_command (priv->db,
                              "create index "
                              "if not exists IDX_TRANS_ORIG_ID "
                              "on TRANS (ORIG_ID)");
}

static sqlite3_stmt *
prepare_statement (sqlite3 *db, const gchar *query)
{
  sqlite3_stmt *statement;
  int rc;

  rc = sqlite3_prepare_v2 (db,
                           query,
                           -1,
                           &statement,
                           NULL);

  if (rc != SQLITE_OK)
    {
      g_error ("gtr-gda.c: prepare_statement: "
               "sqlite3_prepare_v2 failed.\n"
               "query: %s\n"
               "error message: %s\n",
               query,
               sqlite3_errmsg (db));
    }
  return statement;
}

static void
gtr_gda_init (GtrGda * self)
{
  g_autofree gchar *connection_string = NULL;
  g_autoptr (GError) error = NULL;
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);
  g_auto (GPathBuf) path;
  g_autofree char *db_filepath = NULL;
  const gchar *config_dir;

  config_dir = gtr_dirs_get_user_config_dir ();
  g_path_buf_init_from_path (&path, config_dir);
  g_path_buf_push (&path, "translation-memory.db");
  db_filepath = g_path_buf_to_path (&path);

  int rc = sqlite3_open_v2 (db_filepath,
                            &priv->db,
                            SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                            NULL);
  if (rc != SQLITE_OK)
    {
      g_warning ("Error creating database: %s", sqlite3_errmsg (priv->db));
      sqlite3_close_v2 (priv->db);
      priv->db = NULL;
    }

  initialize_db (self);

  /* prepare statements */

  priv->stmt_find_orig =
    prepare_statement (priv->db,
                       "select ID from ORIG "
                       "where VALUE=?1");

  priv->stmt_select_word =
    prepare_statement (priv->db,
                       "select ID from WORD "
                       "where VALUE=?1");

  priv->stmt_select_trans =
    prepare_statement (priv->db,
                       "select VALUE from TRANS "
                       "where ORIG_ID=?1");

  priv->stmt_find_trans =
    prepare_statement (priv->db,
                       "select ID from TRANS "
                       "where ORIG_ID=?1 "
                       "and VALUE=?2");

  priv->stmt_insert_orig =
    prepare_statement (priv->db,
                       "insert into "
                       "ORIG (VALUE, SENTENCE_SIZE) "
                       "values "
                       "(?1, ?2)");

  priv->stmt_insert_word =
    prepare_statement (priv->db,
                       "insert into "
                       "WORD (VALUE) "
                       "values "
                       "(?1)");

  priv->stmt_insert_link =
    prepare_statement (priv->db,
                       "insert into "
                       "WORD_ORIG_LINK (WORD_ID, ORIG_ID) "
                       "values "
                       "(?, ?)");

  priv->stmt_insert_trans =
    prepare_statement (priv->db,
                       "insert into "
                       "TRANS (ORIG_ID, VALUE) "
                       "values "
                       "(?1, ?2)");

  priv->stmt_delete_trans =
    prepare_statement (priv->db,
                       "delete from TRANS "
                       "where id = ?1");

  /* GSetting default for max-missing-words */
  priv->max_omits = 2;
  /* GSetting default for max-length-diff */
  priv->max_delta = 2;
  /* Default used at gtr_window_init */
  priv->max_items = 10;

  priv->lookup_query_cache = g_hash_table_new_full (g_direct_hash,
                                                    g_direct_equal,
                                                    NULL,
                                                    (GDestroyNotify)sqlite3_finalize);
}

static void
gtr_gda_dispose (GObject * object)
{
  GtrGda *self = GTR_GDA (object);
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);

  if (priv->stmt_find_orig != NULL)
    {
      sqlite3_finalize (priv->stmt_find_orig);
      priv->stmt_find_orig = NULL;
    }

  if (priv->stmt_select_trans != NULL)
    {
      sqlite3_finalize (priv->stmt_select_trans);
      priv->stmt_select_trans = NULL;
    }

  if (priv->stmt_find_trans != NULL)
    {
      sqlite3_finalize (priv->stmt_find_trans);
      priv->stmt_find_trans = NULL;
    }

  if (priv->stmt_select_word != NULL)
    {
      sqlite3_finalize (priv->stmt_select_word);
      priv->stmt_select_word = NULL;
    }

  if (priv->stmt_insert_orig != NULL)
    {
      sqlite3_finalize (priv->stmt_insert_orig);
      priv->stmt_insert_orig = NULL;
    }

  if (priv->stmt_insert_word != NULL)
    {
      sqlite3_finalize (priv->stmt_insert_word);
      priv->stmt_insert_word = NULL;
    }

  if (priv->stmt_insert_link != NULL)
    {
      sqlite3_finalize (priv->stmt_insert_link);
      priv->stmt_insert_link = NULL;
    }

  if (priv->stmt_insert_trans != NULL)
    {
      sqlite3_finalize (priv->stmt_insert_trans);
      priv->stmt_insert_trans = NULL;
    }

  if (priv->stmt_delete_trans != NULL)
    {
      sqlite3_finalize (priv->stmt_delete_trans);
      priv->stmt_delete_trans = NULL;
    }

  if (priv->lookup_query_cache != NULL)
    {
      g_hash_table_unref (priv->lookup_query_cache);
      priv->lookup_query_cache = NULL;
    }

  if (priv->db != NULL)
    {
      sqlite3_close_v2 (priv->db);
      priv->db = NULL;
    }

  G_OBJECT_CLASS (gtr_gda_parent_class)->dispose (object);
}

static void
gtr_gda_class_init (GtrGdaClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->dispose = gtr_gda_dispose;
}

/**
 * gtr_gda_new:
 *
 * Creates a new #GtrGda object.
 *
 * Returns: a new #GtrGda object
 */
GtrGda *
gtr_gda_new (void)
{
  GtrGda *gda;

  gda = g_object_new (GTR_TYPE_GDA, NULL);

  return gda;
}

