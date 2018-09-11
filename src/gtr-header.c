/*
 * (C) 2007 Pablo Sanxiao <psanxiao@gmail.com>
 *          Ignacio Casal Quinteiro <icq@gnome.org>
 *     2008 Igalia
 *
 * gtranslator is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or   
 * (at your option) any later version.
 *
 * gtranslator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Pablo Sanxiao <psanxiao@gmail.com>
 *   Ignacio Casal Quinteiro <icq@gnome.org>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-application.h"
#include "gtr-header.h"
#include "gtr-settings.h"
#include "gtr-profile.h"
#include "gtr-profile-manager.h"
#include "gtr-utils.h"

#include <glib.h>
#include <glib-object.h>
#include <string.h>

#define GTR_HEADER_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
                                         (object), \
                                         GTR_TYPE_HEADER, \
                                         GtrHeaderPrivate))

G_DEFINE_TYPE (GtrHeader, gtr_header, GTR_TYPE_MSG)

struct _GtrHeaderPrivate
{
  GSettings *settings;

  GtrProfileManager *prof_manager;
  GtrProfile *profile;
  gint nplurals;
};

static void
gtr_header_set_field (GtrHeader * header,
                      const gchar * field, const gchar * data)
{
  gchar *msgstr;

  g_return_if_fail (GTR_IS_HEADER (header));
  g_return_if_fail (data != NULL);

  msgstr = po_header_set_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                                field, data);
  gtr_msg_set_msgstr (GTR_MSG (header), msgstr);

  g_free (msgstr);
}

/* Set nplurals variable */
static void
parse_nplurals (GtrHeader * header)
{
  gchar *pointer, *plural_forms;
  gboolean use_profile_values;

  plural_forms = gtr_header_get_plural_forms (header);
  header->priv->nplurals = -1;
  use_profile_values = g_settings_get_boolean (header->priv->settings,
                                               GTR_SETTINGS_USE_PROFILE_VALUES);

  if (use_profile_values || !plural_forms)
    {
      const gchar *plural_form = NULL;
      GtrProfile *profile;

      if (header->priv->profile != NULL)
        profile = header->priv->profile;
      else
        {
          GtrProfileManager *prof_manager;

          prof_manager = gtr_profile_manager_get_default ();
          profile = gtr_profile_manager_get_active_profile (prof_manager);
          g_object_unref (prof_manager);
        }

      if (profile)
        plural_form = gtr_profile_get_plural_forms (profile);
      else if (!plural_forms)
        return;

      if (plural_form)
        {
          g_free (plural_forms);
          plural_forms = g_strdup (plural_form);
        }
      else if (!plural_forms)
        return;
    }

  pointer = g_strrstr (plural_forms, "nplurals");

  if (pointer != NULL)
    {
      while (*pointer != '\0' && *pointer != '=')
        pointer++;

      if (*pointer != '\0')
        {
          pointer++;
          while (*pointer != '\0' && *pointer == ' ')
            pointer++;

          if (*pointer == '\0')
            return;
        }
      else
        return;

      header->priv->nplurals = g_ascii_digit_value (*pointer);
    }

  /*g_message ("nplurals: %d", header->priv->nplurals); */

  g_free (plural_forms);
}

static void
profile_removed_cb (GtrProfileManager *prof_manager,
                    GtrProfile        *profile,
                    GtrHeader         *header)
{
  if (profile == header->priv->profile)
    header->priv->profile = NULL;
}

static void
gtr_header_init (GtrHeader * header)
{
  header->priv = GTR_HEADER_GET_PRIVATE (header);

  header->priv->nplurals = -1;
  header->priv->profile = NULL;

  header->priv->settings = g_settings_new ("org.gnome.gtranslator.preferences.files");
  header->priv->prof_manager = gtr_profile_manager_get_default ();

  g_signal_connect (header->priv->prof_manager, "profile-removed",
                    G_CALLBACK (profile_removed_cb), header);
}

static void
gtr_header_dispose (GObject * object)
{
  GtrHeader *header = GTR_HEADER (object);

  g_clear_object (&header->priv->settings);
  g_clear_object (&header->priv->prof_manager);
  g_clear_object (&header->priv->profile);

  G_OBJECT_CLASS (gtr_header_parent_class)->dispose (object);
}

static void
gtr_header_class_init (GtrHeaderClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrHeaderPrivate));

  object_class->dispose = gtr_header_dispose;
}

/* Public methods */

/**
 * gtr_header_new: (skip)
 * @iter:
 * @message:
 *
 * Return value:
 */
GtrHeader *
gtr_header_new (po_message_iterator_t iter, po_message_t message)
{
  GtrHeader *header;

  header = g_object_new (GTR_TYPE_HEADER, NULL);

  _gtr_msg_set_iterator (GTR_MSG (header), iter);
  _gtr_msg_set_message (GTR_MSG (header), message);

  /* We have to parse the number of plurals */
  parse_nplurals (header);

  return header;
}

/**
 * gtr_header_get_comments:
 * @header: a #GtrHeader
 *
 * Gets the comments of the header.
 *
 * Return value: the comments of the header.
 */
const gchar *
gtr_header_get_comments (GtrHeader * header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_message_comments (_gtr_msg_get_message (GTR_MSG (header)));
}

void
gtr_header_set_comments (GtrHeader * header, const gchar * comments)
{
  g_return_if_fail (GTR_IS_HEADER (header));
  g_return_if_fail (comments != NULL);

  po_message_set_comments (_gtr_msg_get_message (GTR_MSG (header)), comments);
}

gchar *
gtr_header_get_prj_id_version (GtrHeader * header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_header_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                          "Project-Id-Version");
}

void
gtr_header_set_prj_id_version (GtrHeader * header,
                               const gchar * prj_id_version)
{
  g_return_if_fail (GTR_IS_HEADER (header));
  g_return_if_fail (prj_id_version != NULL);

  gtr_header_set_field (header, "Project-Id-Version", prj_id_version);
}

gchar *
gtr_header_get_rmbt (GtrHeader * header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_header_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                          "Report-Msgid-Bugs-To");
}

void
gtr_header_set_rmbt (GtrHeader * header, const gchar * rmbt)
{
  /* FIXME: rmbt is not a good name */
  g_return_if_fail (GTR_IS_HEADER (header));
  g_return_if_fail (rmbt != NULL);

  gtr_header_set_field (header, "Report-Msgid-Bugs-To", rmbt);
}

gchar *
gtr_header_get_pot_date (GtrHeader * header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_header_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                          "POT-Creation-Date");
}

void
gtr_header_set_pot_date (GtrHeader * header, const gchar * pot_date)
{
  g_return_if_fail (GTR_IS_HEADER (header));
  g_return_if_fail (pot_date != NULL);

  gtr_header_set_field (header, "POT-Creation-Date", pot_date);
}

gchar *
gtr_header_get_po_date (GtrHeader * header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_header_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                          "PO-Revision-Date");
}

void
gtr_header_set_po_date (GtrHeader * header, const gchar * po_date)
{
  g_return_if_fail (GTR_IS_HEADER (header));
  g_return_if_fail (po_date != NULL);

  gtr_header_set_field (header, "PO-Revision-Date", po_date);
}

gchar *
gtr_header_get_translator (GtrHeader * header)
{
  gchar *space, *translator_temp, *translator;

  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  translator_temp = po_header_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                                     "Last-Translator");
  space = g_strrstr (translator_temp, " <");

  if (!space)
    translator = g_strdup (translator_temp);
  else
    translator = g_strndup (translator_temp, space - translator_temp);

  g_free (translator_temp);

  return translator;
}

gchar *
gtr_header_get_tr_email (GtrHeader * header)
{
  gchar *space, *email_temp, *email;

  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  email_temp = po_header_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                                "Last-Translator");
  space = g_strrstr (email_temp, " <");

  if (!space)
    email = g_strdup ("");
  else
    email = g_strndup (space + 2, strlen (space) - 3);

  g_free (email_temp);

  return email;
}

void
gtr_header_set_translator (GtrHeader * header,
                           const gchar * name, const gchar * email)
{
  gchar *translator;

  g_return_if_fail (GTR_IS_HEADER (header));

  translator = g_strconcat (name, " <", email, ">", NULL);

  gtr_header_set_field (header, "Last-Translator", translator);

  g_free (translator);
}

gchar *
gtr_header_get_language (GtrHeader * header)
{
  gchar *space, *lang_temp, *language;

  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  lang_temp = po_header_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                               "Language-Team");
  space = g_strrstr (lang_temp, " <");

  if (!space)
    language = g_strdup (lang_temp);
  else
    language = g_strndup (lang_temp, space - lang_temp);

  g_free (lang_temp);

  return language;
}

gchar *
gtr_header_get_lg_email (GtrHeader * header)
{
  gchar *space, *email_temp, *email;

  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  email_temp = po_header_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                                "Language-Team");
  space = g_strrstr (email_temp, " <");

  if (!space)
    email = g_strdup ("");
  else
    email = g_strndup (space + 2, strlen (space) - 3);

  g_free (email_temp);

  return email;
}

void
gtr_header_set_language (GtrHeader * header,
                         const gchar * language, const gchar * email)
{
  gchar *lang_temp;

  g_return_if_fail (GTR_IS_HEADER (header));

  lang_temp = g_strconcat (language, " <", email, ">", NULL);

  gtr_header_set_field (header, "Language-Team", lang_temp);

  g_free (lang_temp);
}

gchar *
gtr_header_get_mime_version (GtrHeader * header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_header_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                          "MIME-Version");
}

void
gtr_header_set_mime_version (GtrHeader * header, const gchar * mime_version)
{
  g_return_if_fail (GTR_IS_HEADER (header));

  gtr_header_set_field (header, "MIME-Version", mime_version);
}

gchar *
gtr_header_get_charset (GtrHeader * header)
{
  gchar *space, *charset_temp, *charset;

  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  charset_temp = po_header_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                                  "Content-Type");

  space = g_strrstr (charset_temp, "=");

  if (!space)
    charset = g_strdup ("");
  else
    charset = g_strdup (space + 1);

  g_free (charset_temp);

  return charset;
}

void
gtr_header_set_charset (GtrHeader * header, const gchar * charset)
{
  gchar *set;

  g_return_if_fail (GTR_IS_HEADER (header));

  set = g_strconcat ("text/plain;", " charset=", charset, NULL);

  gtr_header_set_field (header, "Content-Type", set);

  g_free (set);
}

gchar *
gtr_header_get_encoding (GtrHeader * header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_header_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                          "Content-Transfer-Encoding");
}

void
gtr_header_set_encoding (GtrHeader * header, const gchar * encoding)
{
  g_return_if_fail (GTR_IS_HEADER (header));

  gtr_header_set_field (header, "Content-Transfer-Encoding", encoding);
}

/**
 * gtr_header_get_plural_forms:
 * @header: a #GtrHeader.
 *
 * Return value: a new allocated string with the plural form of the po file.
 */
gchar *
gtr_header_get_plural_forms (GtrHeader * header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_header_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                          "Plural-Forms");
}

/**
 * gtr_header_set_plural_forms:
 * @header: a #GtrHeader
 * @plural_forms: the plural forms string.
 *
 * Sets the plural form string in the @header and it sets the number of plurals.
 */
void
gtr_header_set_plural_forms (GtrHeader * header, const gchar * plural_forms)
{
  g_return_if_fail (GTR_IS_HEADER (header));
  g_return_if_fail (plural_forms != NULL);

  gtr_header_set_field (header, "Plural-Forms", plural_forms);

  /*Now we parse the plural forms to know the number of plurals */
  parse_nplurals (header);
}

/**
 * gtr_header_get_plural:
 * @header: a #GtrHeader
 *
 * Return value: The number of plurals of the po file, if there is not a plural
 * form in the po file it returns the predefined by user number of plurals 
 * or 1 if there is not a plural form string stored.
 */
gint
gtr_header_get_nplurals (GtrHeader * header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), 1);

  if (header->priv->nplurals > -1)
    return header->priv->nplurals;
  else
    return 1;
}

static void
set_profile_values (GtrHeader *header)
{
  GtrProfile *active_profile;
  gboolean use_profile_values;

  if (header->priv->profile != NULL)
    active_profile = header->priv->profile;
  else
    active_profile = gtr_profile_manager_get_active_profile (header->priv->prof_manager);

  use_profile_values = g_settings_get_boolean (header->priv->settings,
                                               GTR_SETTINGS_USE_PROFILE_VALUES);

  if (use_profile_values && active_profile != NULL)
    {
      gtr_header_set_translator (header,
                                 gtr_profile_get_author_name (active_profile),
                                 gtr_profile_get_author_email
                                 (active_profile));
      gtr_header_set_language (header,
                               gtr_profile_get_language_name (active_profile),
                               gtr_profile_get_group_email (active_profile));
      gtr_header_set_charset (header,
                              gtr_profile_get_charset (active_profile));
      gtr_header_set_encoding (header,
                               gtr_profile_get_encoding (active_profile));
      gtr_header_set_plural_forms (header,
                                   gtr_profile_get_plural_forms (active_profile));
    }
}

static void
update_po_date (GtrHeader * header)
{
  gchar *current_date;
  gchar *current_time;
  gchar *new_date;

  current_date = gtr_utils_get_current_date ();
  current_time = gtr_utils_get_current_time ();

  new_date = g_strconcat (current_date, " ", current_time, NULL);

  g_free (current_date);
  g_free (current_time);

  gtr_header_set_po_date (header, new_date);

  g_free (new_date);
}

static void
update_comments (GtrHeader *header, const gchar *comments)
{
  GtrProfile *active_profile;
  GString *new_comments;
  GString *years;
  gchar **comment_lines;
  gchar *translator;
  gchar *email;
  gchar *current_year;
  gboolean use_profile_values;
  gint i;

  if (header->priv->profile != NULL)
    active_profile = header->priv->profile;
  else
    active_profile = gtr_profile_manager_get_active_profile (header->priv->prof_manager);

  current_year = gtr_utils_get_current_year ();

  use_profile_values = g_settings_get_boolean (header->priv->settings,
                                               GTR_SETTINGS_USE_PROFILE_VALUES);

  /* Save the previous translator to update the header's comment */
  if (use_profile_values && active_profile != NULL)
    {
      translator = g_strdup (gtr_profile_get_author_name (active_profile));
      email = g_strdup (gtr_profile_get_author_email (active_profile));
    }
  else
    {
      translator = gtr_header_get_translator (header);
      email = gtr_header_get_tr_email (header);
    }

  comment_lines = g_strsplit (comments, "\n", -1);
  new_comments = g_string_new ("");
  years = g_string_new ("");

  for (i = 0; comment_lines != NULL && comment_lines[i] != NULL; i++)
    {
      if (g_str_has_prefix (comment_lines[i], translator))
        {
          gchar **year_array;
          gint j;

          year_array = g_strsplit (comment_lines[i], ",", -1);

          for (j = 1; year_array != NULL && year_array[j] != NULL; j++)
            {
              gchar *search;

              if (g_str_has_suffix (year_array[j], "."))
                {
                  gint len;

                  len = g_utf8_strlen (year_array[j], -1);
                  search = g_strndup (year_array[j], len - 1);
                }
              else
                search = g_strdup (year_array[j]);

              if ((g_strrstr (years->str, search) == NULL) &&
                  (strcmp (search + 1, current_year) != 0))
                {
                  years = g_string_append (years, search);
                  years = g_string_append_c (years, ',');
                }

              g_free (search);
            }

          g_strfreev (year_array);
        }
      else
        {
          new_comments = g_string_append (new_comments, comment_lines[i]);
          new_comments = g_string_append_c (new_comments, '\n');
        }
    }

  g_strfreev (comment_lines);

  g_string_append_printf (years, " %s.", current_year);

  /* Remove all empty lines at the end */
  while (new_comments->str[new_comments->len - 1] == '\n')
    new_comments = g_string_truncate (new_comments, new_comments->len - 1);

  /* Add \n\n for an extra newline at the end of the comments */
  g_string_append_printf (new_comments, "\n%s <%s>,%s\n\n",
                          translator, email, years->str);

  g_string_free (years, TRUE);

  gtr_header_set_comments (header, new_comments->str);

  g_string_free (new_comments, TRUE);
}

/* FIXME: complete this */
static void
add_default_comments (GtrHeader * header)
{
}

void
gtr_header_update_header (GtrHeader * header)
{
  const gchar *comments;

  /* If needed update the header with the profile values */
  set_profile_values (header);

  /* Update the po date */
  update_po_date (header);

  /* Update the header's comment */
  comments = gtr_header_get_comments (header);

  if (comments != NULL)
    update_comments (header, comments);
  else
    add_default_comments (header);

  /* We need to unfuzzy the header to not produce errors */
  gtr_msg_set_fuzzy (GTR_MSG (header), FALSE);

  /* Update X-Generator field */
  gtr_header_set_field (header, "X-Generator", "Gtranslator "PACKAGE_VERSION);
}

void
gtr_header_set_profile (GtrHeader  *header,
                        GtrProfile *profile)
{
  g_return_if_fail (GTR_IS_HEADER (header));

  header->priv->profile = g_object_ref (profile);
}

/**
 * gtr_header_get_profile:
 * @header: a #GtrHeader
 *
 * Gets the profile of the header
 *
 * Return value: (transfer none): the profile of the header.
 */
GtrProfile *
gtr_header_get_profile (GtrHeader *header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return header->priv->profile;
}
