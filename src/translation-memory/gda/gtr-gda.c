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

#include <libgda.h>
#include <sql-parser/gda-sql-parser.h>
#include "gtr-gda.h"
#include "gtr-translation-memory.h"
#include "gtr-dirs.h"
#include "gda-utils.h"

#include <glib.h>
#include <glib-object.h>
#include <string.h>

static void
gtr_translation_memory_iface_init (GtrTranslationMemoryInterface * iface);

typedef struct
{
  GdaConnection *db;

  GdaSqlParser *parser;

  /* prepared statements */
  GdaStatement *stmt_find_orig;
  GdaStatement *stmt_select_trans;
  GdaStatement *stmt_select_word;
  GdaStatement *stmt_find_trans;

  GdaStatement *stmt_insert_orig;
  GdaStatement *stmt_insert_word;
  GdaStatement *stmt_insert_link;
  GdaStatement *stmt_insert_trans;

  GdaStatement *stmt_delete_trans;

  guint max_omits;
  guint max_delta;
  gint max_items;

  GHashTable *lookup_query_cache;
} GtrGdaPrivate;

G_DEFINE_TYPE_WITH_CODE (GtrGda,
                         gtr_gda,
                         G_TYPE_OBJECT,
                         G_ADD_PRIVATE (GtrGda)
                         G_IMPLEMENT_INTERFACE (GTR_TYPE_TRANSLATION_MEMORY,
                                                gtr_translation_memory_iface_init))

static gint
select_integer (GdaConnection *db,
                GdaStatement *stmt,
                GdaSet *params,
                GError **error)
{
  GdaDataModel *model;
  GError *inner_error;
  gint result = 0;

  inner_error = NULL;
  model = gda_connection_statement_execute_select (db, stmt, params,
                                                   &inner_error);
  g_object_unref (params);

  if (!model)
    {
      g_propagate_error (error, inner_error);
      return 0;
    }

  if (gda_data_model_get_n_rows (model) > 0)
    {
      const GValue * val;

      inner_error = NULL;
      val = gda_data_model_get_typed_value_at (model,
                                               0, 0,
                                               G_TYPE_INT,
                                               FALSE,
                                               &inner_error);
      if (val)
        result = g_value_get_int (val);
      else
        g_propagate_error (error, inner_error);
    }

  g_object_unref (model);

  return result;
}

static gint
insert_row (GdaConnection *db,
            GdaStatement *stmt,
            GdaSet *params,
            GError **error)
{
  GdaSet *last_row;
  GError *inner_error;
  const GValue * val;
  gint result = 0;

  inner_error = NULL;
  if (-1 == gda_connection_statement_execute_non_select (db,
                                                         stmt,
                                                         params,
                                                         &last_row,
                                                         &inner_error))
    {
      g_object_unref (params);
      g_propagate_error (error, inner_error);
      return 0;
    }
  g_object_unref (params);

  g_return_val_if_fail (last_row != NULL, 0);

  val = gda_set_get_holder_value (last_row, "+0");
  if (val)
    result = g_value_get_int (val);

  g_object_unref (last_row);

  return result;
}

static int
string_comparator (const void *s1, const void *s2)
{
  return strcmp (*(const gchar **) s1, *(const gchar **) s2);
}

static gchar **
gtr_gda_split_string_in_words (const gchar *phrase)
{
  gchar **words = gtr_gda_utils_split_string_in_words (phrase);
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
    word_id = select_integer (priv->db,
                              priv->stmt_select_word,
                              gda_set_new_inline (1,
                                                  "value", G_TYPE_STRING,
                                                  word),
                              &inner_error);
    if (inner_error)
      {
        g_propagate_error (error, inner_error);
        return;
      }
  }

  if (word_id == 0)
    {
      inner_error = NULL;
      word_id = insert_row (priv->db,
                            priv->stmt_insert_word,
                            gda_set_new_inline (1,
                                                "value", G_TYPE_STRING, word),
                            &inner_error);
      if (inner_error)
        {
          g_propagate_error (error, inner_error);
          return;
        }
    }

  /* insert link */
  {
    GdaSet *params;

    params = gda_set_new_inline (2,
                                 "word_id", G_TYPE_INT, word_id,
                                 "orig_id", G_TYPE_INT, orig_id);

    inner_error = NULL;
    if (-1 == gda_connection_statement_execute_non_select (priv->db,
                                                           priv->stmt_insert_link,
                                                           params,
                                                           NULL,
                                                           &inner_error))
      g_propagate_error (error, inner_error);

    g_object_unref (params);
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
  gchar **words = NULL;
  GError *inner_error;
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);

  inner_error = NULL;
  orig_id = select_integer (priv->db,
                            priv->stmt_find_orig,
                            gda_set_new_inline (1,
                                                "original", G_TYPE_STRING,
                                                original),
                            &inner_error);
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
                            gda_set_new_inline (2,
                                                "original", G_TYPE_STRING,
                                                original,
                                                "sentence_size", G_TYPE_INT,
                                                (gint) sz),
                            &inner_error);
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

      g_strfreev (words);
    }
  else
    {
      inner_error = NULL;
      found_translation = select_integer (priv->db,
                                          priv->stmt_find_trans,
                                          gda_set_new_inline (2,
                                                              "orig_id",
                                                              G_TYPE_INT,
                                                              orig_id,
                                                              "value",
                                                              G_TYPE_STRING,
                                                              translation),
                                          &inner_error);
      if (inner_error)
        goto error;
    }

  if (!found_translation)
    {
      inner_error = NULL;
      insert_row (priv->db,
                  priv->stmt_insert_trans,
                  gda_set_new_inline (2,
                                      "orig_id", G_TYPE_INT,
                                      orig_id,
                                      "value", G_TYPE_STRING,
                                      translation),
                  &inner_error);
      if (inner_error)
        goto error;
    }

  return TRUE;

 error:
  g_strfreev (words);
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
  if (!gda_connection_begin_transaction (priv->db,
                                         NULL,
                                         GDA_TRANSACTION_ISOLATION_READ_COMMITTED,
                                         &error))
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
    gda_connection_commit_transaction (priv->db, NULL, NULL);
  else
    gda_connection_rollback_transaction (priv->db, NULL, NULL);

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

  error = NULL;
  if (!gda_connection_begin_transaction (priv->db,
                                         NULL,
                                         GDA_TRANSACTION_ISOLATION_READ_COMMITTED,
                                         &error))
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
      if (!result)
        {
          g_warning ("storing message failed: %s", error->message);
          g_error_free (error);
          break;
        }
    }

  if (result)
    gda_connection_commit_transaction (priv->db, NULL, NULL);
  else
    gda_connection_rollback_transaction (priv->db, NULL, NULL);

  return result;
}

static void
gtr_gda_remove (GtrTranslationMemory *tm,
                gint translation_id)
{
  GtrGda *self = GTR_GDA (tm);
  GdaSet *params;
  GError *error;
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);

  params = gda_set_new_inline (1,
                               "id_trans",
                               G_TYPE_INT,
                               translation_id);

  error = NULL;
  gda_connection_statement_execute_non_select (priv->db,
                                               priv->stmt_delete_trans,
                                               params,
                                               NULL,
                                               &error);
  if (error)
    {
      g_warning ("removing translation failed: %s", error->message);
      g_error_free (error);
    }

  g_object_unref (params);
}

static void
free_match (gpointer data)
{
  GtrTranslationMemoryMatch *match = (GtrTranslationMemoryMatch *) data;

  g_free (match->match);
  g_slice_free (GtrTranslationMemoryMatch, match);
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
                          "  and ORIG.VALUE = ##phrase::string "
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
                          "        and ORIG.VALUE <> ##phrase::string "
                          "        and ORIG.SENTENCE_SIZE between %u and %u "
                          "        and WORD.VALUE in (",
                          word_count,
                          word_count,
                          word_count + priv->max_delta);

  for (i = 0; i < word_count; ++i)
    {
      g_string_append_printf (query, "##word%d::string", i);
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

  return g_string_free (query, FALSE);
}

static GdaStatement *
gtr_gda_get_lookup_statement (GtrGda *self, guint word_count, GError **error)
{
  GdaStatement *stmt;
  gchar *query;
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);

  stmt = GDA_STATEMENT (g_hash_table_lookup (priv->lookup_query_cache,
                                             GUINT_TO_POINTER (word_count)));

  if (stmt)
    return stmt;

  query = build_lookup_query (self, word_count);
  stmt = gda_sql_parser_parse_string (priv->parser,
                                      query,
                                      NULL,
                                      error);
  g_free (query);

  g_hash_table_insert (priv->lookup_query_cache,
                       GUINT_TO_POINTER (word_count),
                       stmt);

  return stmt;
}

static GList *
gtr_gda_lookup (GtrTranslationMemory * tm, const gchar * phrase)
{
  GtrGda *self = GTR_GDA (tm);
  gchar **words = NULL;
  guint cnt = 0;
  GList *matches = NULL;
  GError *inner_error;
  GdaStatement *stmt = NULL;
  GdaSet *params = NULL;
  GdaDataModel *model = NULL;
  gint i;
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);

  g_return_val_if_fail (GTR_IS_GDA (self), NULL);

  if (!gda_connection_begin_transaction (priv->db,
                                         NULL,
                                         GDA_TRANSACTION_ISOLATION_READ_COMMITTED,
                                         NULL))
    return NULL;

  words = gtr_gda_split_string_in_words (phrase);
  cnt = g_strv_length (words);

  inner_error = NULL;
  stmt = gtr_gda_get_lookup_statement (self, cnt, &inner_error);
  if (inner_error)
    goto end;

  inner_error = NULL;
  if (!gda_statement_get_parameters (stmt, &params, &inner_error))
    goto end;

  inner_error = NULL;
  gda_set_set_holder_value (params,
                            &inner_error,
                            "phrase", phrase);
  if (inner_error)
    goto end;

  {
    gchar word_id[25];
    for (i = 0; i < cnt; i++)
      {
        sprintf (word_id, "word%d", i);

        inner_error = NULL;
        gda_set_set_holder_value (params,
                                  &inner_error,
                                  word_id, words[i]);
        if (inner_error)
          goto end;
      }
  }

  inner_error = NULL;
  model = gda_connection_statement_execute_select (priv->db,
                                                   stmt,
                                                   params,
                                                   &inner_error);
  if (!model)
    goto end;

  {
    gint count = gda_data_model_get_n_rows (model);
    for (i = 0; i < count; ++i)
      {
        const GValue * val;
        gchar *suggestion;
        gint score;
        gint id;
        GtrTranslationMemoryMatch *match;

        inner_error = NULL;
        val = gda_data_model_get_typed_value_at (model,
                                                 0, i,
                                                 G_TYPE_STRING,
                                                 FALSE,
                                                 &inner_error);
        if (!val)
          goto end;

        suggestion = g_value_dup_string (val);

        inner_error = NULL;
        val = gda_data_model_get_typed_value_at (model,
                                                 1, i,
                                                 G_TYPE_INT,
                                                 FALSE,
                                                 &inner_error);
        if (!val)
          {
            g_free (suggestion);
            goto end;
          }

        score = g_value_get_int (val);

        inner_error = NULL;
        val = gda_data_model_get_typed_value_at (model,
                                                 2, i,
                                                 G_TYPE_INT,
                                                 FALSE,
                                                 &inner_error);
        if (!val)
          {
            g_free (suggestion);
            goto end;
          }

        id = g_value_get_int (val);

        match = g_slice_new (GtrTranslationMemoryMatch);
        match->match = suggestion;
        match->level = score;
        match->id = id;

        matches = g_list_prepend (matches, match);
      }
  }

 end:

  if (model)
    g_object_unref (model);
  if (params)
    g_object_unref (params);

  gda_connection_rollback_transaction (priv->db, NULL, NULL);

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

  priv->max_omits = omits;
  g_hash_table_remove_all (priv->lookup_query_cache);
}

static void
gtr_gda_set_max_delta (GtrTranslationMemory * tm, gsize delta)
{
  GtrGda *self = GTR_GDA (tm);
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);

  priv->max_delta = delta;
  g_hash_table_remove_all (priv->lookup_query_cache);
}

static void
gtr_gda_set_max_items (GtrTranslationMemory * tm, gint items)
{
  GtrGda *self = GTR_GDA (tm);
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);

  priv->max_items = items;
  g_hash_table_remove_all (priv->lookup_query_cache);
}

static void
gtr_translation_memory_iface_init (GtrTranslationMemoryInterface * iface)
{
  iface->store = gtr_gda_store;
  iface->store_list = gtr_gda_store_list;
  iface->remove = gtr_gda_remove;
  iface->lookup = gtr_gda_lookup;
  iface->set_max_omits = gtr_gda_set_max_omits;
  iface->set_max_delta = gtr_gda_set_max_delta;
  iface->set_max_items = gtr_gda_set_max_items;
}

static GdaStatement *
prepare_statement(GdaSqlParser *parser, const gchar *query)
{
  GError *error = NULL;
  GdaStatement *statement = gda_sql_parser_parse_string (parser,
                                                         query,
                                                         NULL,
                                                         &error);

  if (error)
    {
      g_error ("gtr-gda.c: prepare_statement: "
               "gda_sql_parser_parse_string failed.\n"
               "query: %s\n"
               "error message: %s\n",
               query,
               error->message);
    }
  return statement;
}

static void
gtr_gda_init (GtrGda * self)
{
  gchar *connection_string;
  GError *error = NULL;
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);

  gda_init ();

  {
    const gchar *config_dir;
    gchar *encoded_config_dir;

    config_dir = gtr_dirs_get_user_config_dir ();
    encoded_config_dir = gda_rfc1738_encode (config_dir);

    connection_string = g_strdup_printf ("DB_DIR=%s;"
                                         "DB_NAME=translation-memory",
                                         encoded_config_dir);

    g_free (encoded_config_dir);
  }

  priv->db = gda_connection_open_from_string ("Sqlite",
                                                    connection_string,
                                                    NULL,
                                                    GDA_CONNECTION_OPTIONS_NONE,
                                                    &error);
  if (error)
    {
      g_warning ("Error creating database: %s", error->message);
      g_error_free (error);
    }

  gda_connection_execute_non_select_command (priv->db,
                                             "create table WORD ("
                                             "ID integer primary key autoincrement,"
                                             "VALUE text unique)",
                                             NULL);

  gda_connection_execute_non_select_command (priv->db,
                                             "create table WORD_ORIG_LINK ("
                                             "WORD_ID integer,"
                                             "ORIG_ID integer,"
                                             "primary key (WORD_ID, ORIG_ID))",
                                             NULL);

  gda_connection_execute_non_select_command (priv->db,
                                             "create table ORIG ("
                                             "ID integer primary key autoincrement,"
                                             "VALUE text unique,"
                                             "SENTENCE_SIZE integer)",
                                             NULL);

  gda_connection_execute_non_select_command (priv->db,
                                             "create table TRANS ("
                                             "ID integer primary key autoincrement,"
                                             "ORIG_ID integer,"
                                             "VALUE text)",
                                             NULL);

  gda_connection_execute_non_select_command (priv->db,
                                             "create index "
                                             "if not exists IDX_TRANS_ORIG_ID "
                                             "on TRANS (ORIG_ID)",
                                             NULL);

  /* prepare statements */

  priv->parser = gda_connection_create_parser (priv->db);
  if (priv->parser == NULL)
    priv->parser = gda_sql_parser_new ();

  priv->stmt_find_orig =
    prepare_statement (priv->parser,
                       "select ID from ORIG "
                       "where VALUE=##original::string");

  priv->stmt_select_word =
    prepare_statement (priv->parser,
                       "select ID from WORD "
                       "where VALUE=##value::string");

  priv->stmt_select_trans =
    prepare_statement (priv->parser,
                       "select VALUE from TRANS "
                       "where ORIG_ID=##orig_id::int");

  priv->stmt_find_trans =
    prepare_statement (priv->parser,
                       "select ID from TRANS "
                       "where ORIG_ID=##orig_id::int "
                       "and VALUE=##value::string");

  priv->stmt_insert_orig =
    prepare_statement (priv->parser,
                       "insert into "
                       "ORIG (VALUE, SENTENCE_SIZE) "
                       "values "
                       "(##original::string, ##sentence_size::int)");

  priv->stmt_insert_word =
    prepare_statement (priv->parser,
                       "insert into "
                       "WORD (VALUE) "
                       "values "
                       "(##value::string)");

  priv->stmt_insert_link =
    prepare_statement (priv->parser,
                       "insert into "
                       "WORD_ORIG_LINK (WORD_ID, ORIG_ID) "
                       "values "
                       "(##word_id::int, ##orig_id::int)");

  priv->stmt_insert_trans =
    prepare_statement (priv->parser,
                       "insert into "
                       "TRANS (ORIG_ID, VALUE) "
                       "values "
                       "(##orig_id::int, ##value::string)");

  priv->stmt_delete_trans =
    prepare_statement (priv->parser,
                       "delete from TRANS "
                       "where id = ##id_trans::int");

  priv->max_omits = 0;
  priv->max_delta = 0;
  priv->max_items = 0;

  priv->lookup_query_cache = g_hash_table_new_full (g_direct_hash,
                                                          g_direct_equal,
                                                          NULL,
                                                          g_object_unref);
}

static void
gtr_gda_dispose (GObject * object)
{
  GtrGda *self = GTR_GDA (object);
  GtrGdaPrivate *priv = gtr_gda_get_instance_private (self);

  if (priv->stmt_find_orig != NULL)
    {
      g_object_unref (priv->stmt_find_orig);
      priv->stmt_find_orig = NULL;
    }

  if (priv->stmt_select_trans != NULL)
    {
      g_object_unref (priv->stmt_select_trans);
      priv->stmt_select_trans = NULL;
    }

  if (priv->stmt_find_trans != NULL)
    {
      g_object_unref (priv->stmt_find_trans);
      priv->stmt_find_trans = NULL;
    }

  if (priv->stmt_select_word != NULL)
    {
      g_object_unref (priv->stmt_select_word);
      priv->stmt_select_word = NULL;
    }

  if (priv->stmt_insert_orig != NULL)
    {
      g_object_unref (priv->stmt_insert_orig);
      priv->stmt_insert_orig = NULL;
    }

  if (priv->stmt_insert_word != NULL)
    {
      g_object_unref (priv->stmt_insert_word);
      priv->stmt_insert_word = NULL;
    }

  if (priv->stmt_insert_link != NULL)
    {
      g_object_unref (priv->stmt_insert_link);
      priv->stmt_insert_link = NULL;
    }

  if (priv->stmt_insert_trans != NULL)
    {
      g_object_unref (priv->stmt_insert_trans);
      priv->stmt_insert_trans = NULL;
    }

  if (priv->stmt_delete_trans != NULL)
    {
      g_object_unref (priv->stmt_delete_trans);
      priv->stmt_delete_trans = NULL;
    }

  if (priv->parser != NULL)
    {
      g_object_unref (priv->parser);
      priv->parser = NULL;
    }

  if (priv->db != NULL)
    {
      g_object_unref (priv->db);
      priv->db = NULL;
    }

  if (priv->lookup_query_cache != NULL)
    {
      g_hash_table_unref (priv->lookup_query_cache);
      priv->lookup_query_cache = NULL;
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
gtr_gda_new ()
{
  GtrGda *gda;

  gda = g_object_new (GTR_TYPE_GDA, NULL);

  return gda;
}

