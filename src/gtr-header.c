/*
 * Copyright (C) 2007  Pablo Sanxiao <psanxiao@gmail.com>
 *                     Ignacio Casal Quinteiro <icq@gnome.org>
 *               2008  Igalia
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

typedef struct
{
  GSettings *settings;

  GtrProfileManager *prof_manager;
  GtrProfile *profile;
  gint nplurals;
} GtrHeaderPrivate;

struct _GtrHeader
{
  GtrMsg parent_instance;
};

G_DEFINE_FINAL_TYPE_WITH_PRIVATE (GtrHeader, gtr_header, GTR_TYPE_MSG)

void
gtr_header_set_field (GtrHeader * header,
                      const gchar * field, const gchar * data)
{
  gchar *msgstr;

  g_return_if_fail (GTR_IS_HEADER (header));

  if (!data || g_strcmp0 (data, "") == 0)
    return;

  msgstr = po_header_set_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                                field, data);
  gtr_msg_set_msgstr (GTR_MSG (header), msgstr);

  g_free (msgstr);
}

/* Set nplurals variable */
static void
parse_nplurals (GtrHeader * header)
{
  GtrHeaderPrivate *priv = gtr_header_get_instance_private (header);
  g_autofree gchar *plural_forms = NULL;

  plural_forms = gtr_header_get_plural_forms (header);
  priv->nplurals = parse_nplurals_header (plural_forms);
}

static void
profile_removed_cb (GtrProfileManager *prof_manager,
                    GtrProfile        *profile,
                    GtrHeader         *header)
{
  GtrHeaderPrivate *priv = gtr_header_get_instance_private (header);
  if (profile == priv->profile)
    priv->profile = NULL;
}

static void
gtr_header_init (GtrHeader * header)
{
  GtrHeaderPrivate *priv = gtr_header_get_instance_private (header);

  priv->nplurals = -1;
  priv->profile = NULL;

  priv->settings = g_settings_new ("org.gnome.gtranslator.preferences.files");
  priv->prof_manager = gtr_profile_manager_get_default ();

  g_signal_connect (priv->prof_manager, "profile-removed",
                    G_CALLBACK (profile_removed_cb), header);
}

static void
gtr_header_dispose (GObject * object)
{
  GtrHeader *header = GTR_HEADER (object);
  GtrHeaderPrivate *priv = gtr_header_get_instance_private (header);

  g_clear_object (&priv->settings);
  g_clear_object (&priv->prof_manager);
  g_clear_object (&priv->profile);

  G_OBJECT_CLASS (gtr_header_parent_class)->dispose (object);
}

static void
gtr_header_class_init (GtrHeaderClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gtr_header_dispose;
}

static void
gtr_header_set_defaults (GtrHeader *header)
{
  g_autofree gchar* mime_version = gtr_header_get_mime_version (header);
  g_autofree gchar* prj_id_version = gtr_header_get_prj_id_version (header);

  if (!mime_version || *mime_version == '\0')
    gtr_header_set_mime_version (header, "1.0");
  if (!prj_id_version || *prj_id_version == '\0')
    gtr_header_set_prj_id_version (header, "unnamed project");

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

  gtr_header_set_defaults (header);

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
gtr_header_get_language_code (GtrHeader * header)
{
  gchar *language;
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);
  language = po_header_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                              "Language");
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
                         const gchar * language,
                         const gchar * lang_code,
                         const gchar * email)
{
  g_autofree char *lang_team = NULL;

  g_return_if_fail (GTR_IS_HEADER (header));

  if (email && g_strcmp0 (email, "") != 0)
    lang_team = g_strdup_printf ("%s <%s>", language, email);
  else
    lang_team = g_strdup (language);

  gtr_header_set_field (header, "Language-Team", lang_team);
  gtr_header_set_field (header, "Language", lang_code);
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

void
gtr_header_set_dl_info (GtrHeader * header,
                         const gchar * lang,
                         const gchar * module_name,
                         const gchar * branch,
                         const gchar * domain,
                         const char  * vcs_web)
{

  g_return_if_fail (GTR_IS_HEADER (header));

  gtr_header_set_field (header, "X-DL-Lang", lang);
  gtr_header_set_field (header, "X-DL-Module", module_name);
  gtr_header_set_field (header, "X-DL-Branch", branch);
  gtr_header_set_field (header, "X-DL-Domain", domain);
  gtr_header_set_field (header, "X-DL-VCS-Web", vcs_web);
}

/**
 * gtr_header_get_dl_lang:
 * @header: a #GtrHeader
 *
 * Gets the damned lies lang of the header
 *
 * Return value: (transfer full): a newly allocated string representing the
 *                damned lies lang of the header
 */
gchar *
gtr_header_get_dl_lang (GtrHeader * header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_header_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                          "X-DL-Lang");
}

/**
 * gtr_header_get_dl_module:
 * @header: a #GtrHeader
 *
 * Gets the damned lies module of the header
 *
 * Return value: (transfer full): a newly allocated string representing the
 *                damned lies module of the header
 */
gchar *
gtr_header_get_dl_module (GtrHeader * header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_header_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                          "X-DL-Module");
}

/**
 * gtr_header_get_dl_branch:
 * @header: a #GtrHeader
 *
 * Gets the damned lies branch of the header
 *
 * Return value: (transfer full): a newly allocated string representing the
 *                damned lies branch of the header
 */
gchar *
gtr_header_get_dl_branch (GtrHeader * header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_header_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                          "X-DL-Branch");
}

char *
gtr_header_get_dl_vcs_web (GtrHeader * header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_header_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                          "X-DL-VCS-Web");
}

/**
 * gtr_header_get_dl_domain:
 * @header: a #GtrHeader
 *
 * Gets the damned lies domain of the header
 *
 * Return value: (transfer full): a newly allocated string representing the
 *                damned lies domain of the header
 */
gchar *
gtr_header_get_dl_domain (GtrHeader * header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_header_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                          "X-DL-Domain");
}

/**
 * gtr_header_get_dl_state:
 * @header: a #GtrHeader
 *
 * Gets the damned lies module state of the header
 *
 * Return value: (transfer full): a newly allocated string representing the
 *                damned lies module state of the header
 */
gchar *
gtr_header_get_dl_state (GtrHeader * header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_header_field (gtr_msg_get_msgstr (GTR_MSG (header)),
                          "X-DL-State");
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
  GtrHeaderPrivate *priv = gtr_header_get_instance_private (header);
  g_return_val_if_fail (GTR_IS_HEADER (header), 1);

  if (priv->nplurals > -1)
    return priv->nplurals;
  else
    return 1;
}

static void
set_profile_values (GtrHeader *header)
{
  GtrProfile *active_profile;
  gboolean use_profile_values;
  GtrHeaderPrivate *priv = gtr_header_get_instance_private (header);

  if (priv->profile != NULL)
    active_profile = priv->profile;
  else
    active_profile = gtr_profile_manager_get_active_profile (priv->prof_manager);

  use_profile_values = g_settings_get_boolean (priv->settings,
                                               GTR_SETTINGS_USE_PROFILE_VALUES);

  if (use_profile_values && active_profile != NULL)
    {
      gtr_header_set_translator (header,
                                 gtr_profile_get_author_name (active_profile),
                                 gtr_profile_get_author_email
                                 (active_profile));
      gtr_header_set_language (header,
                               gtr_profile_get_language_name (active_profile),
                               gtr_profile_get_language_code (active_profile),
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
  gchar **comment_lines;
  g_autofree char *translator = NULL;
  g_autofree char *email = NULL;
  gchar *current_year;
  gchar *first_year = NULL;
  gchar *years = NULL;
  gboolean use_profile_values;
  gint i;
  GtrHeaderPrivate *priv = gtr_header_get_instance_private (header);

  if (priv->profile != NULL)
    active_profile = priv->profile;
  else
    active_profile = gtr_profile_manager_get_active_profile (priv->prof_manager);

  current_year = gtr_utils_get_current_year ();

  use_profile_values = g_settings_get_boolean (priv->settings,
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

  for (i = 0; comment_lines != NULL && comment_lines[i] != NULL; i++)
    {
      if (g_str_has_prefix (comment_lines[i], translator))
        {
          gchar **year_array;
          gint j;

          year_array = g_strsplit (comment_lines[i], ",", -1);

          // Empty comment
          if (year_array == NULL || year_array[0] == NULL)
            {
              g_strfreev (year_array);
              continue;
            }

          for (j = 1; year_array[j] != NULL; j++)
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

              // removing whitespaces. This will remove ^M chars that can cause
              // problems
              g_strstrip (search);

              // looking for YEAR1-YEAR2.
              if (g_strrstr (search, "-"))
                {
                  gchar **array = g_strsplit (search, "-", 2);
                  if (*array[0] != '\0' && strcmp (array[0], current_year) != 0)
                    first_year = g_strdup (array[0]);

                  g_strfreev (array);
                  g_free (search);
                  break;
                }

              if (*search != '\0' && strcmp (search, current_year) != 0)
                {
                  first_year = g_strdup (search);
                  g_free (search);
                  break;
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

  if (first_year && first_year != current_year)
    {
      years = g_strdup_printf ("%s-%s.", first_year, current_year);
      g_free (first_year);
    }
  else
    years = g_strdup_printf ("%s.", current_year);

  /* Remove all empty lines at the end */
  while (new_comments->str[new_comments->len - 1] == '\n')
    new_comments = g_string_truncate (new_comments, new_comments->len - 1);


  if (!g_settings_get_boolean (priv->settings, GTR_SETTINGS_OMIT_HEADER_CREDIT))
    g_string_append_printf (new_comments, "\n%s <%s>, %s",
                            translator, email, years);

  g_string_append (new_comments, "\n\n");

  g_free (years);
  g_free (current_year);

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
  GtrHeaderPrivate *priv = gtr_header_get_instance_private (header);
  g_return_if_fail (GTR_IS_HEADER (header));

  priv->profile = g_object_ref (profile);
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
  GtrHeaderPrivate *priv = gtr_header_get_instance_private (header);
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return priv->profile;
}
