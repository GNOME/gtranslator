/*
 * (C) 2007 Pablo Sanxiao <psanxiao@gmail.com>
 *          Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *   Pablo Sanxiao <psanxiao@gmail.com>
 *   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 */

#include "gtr-application.h"
#include "gtr-header.h"
#include "gtr-prefs-manager.h"
#include "gtr-profile.h"
#include "gtr-utils.h"

#include <glib.h>
#include <glib-object.h>
#include <string.h>

#define GTR_HEADER_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
                                         (object), \
                                         GTR_TYPE_HEADER, \
                                         GtranslatorHeaderPrivate))

G_DEFINE_TYPE (GtranslatorHeader, gtranslator_header, GTR_TYPE_MSG)

struct _GtranslatorHeaderPrivate
{
  gint   nplurals;
};

static void
gtranslator_header_set_field (GtranslatorHeader *header,
                              const gchar       *field,
                              const gchar       *data)
{
  gchar *msgstr;

  g_return_if_fail (GTR_IS_HEADER (header));
  g_return_if_fail (data != NULL);

  msgstr = po_header_set_field (gtranslator_msg_get_msgstr (GTR_MSG (header)),
                                field, data);
  gtranslator_msg_set_msgstr (GTR_MSG (header), msgstr);

  g_free (msgstr);
}

/* Set nplurals variable */
static void
parse_nplurals (GtranslatorHeader * header)
{
  gchar *pointer, *plural_forms;

  plural_forms = g_strdup (gtranslator_header_get_plural_forms (header));

  if (!plural_forms)
    {
      const gchar *plural_form;
      GtranslatorProfile *profile;

      header->priv->nplurals = -1;

      profile = gtranslator_application_get_active_profile (GTR_APP);

      if (profile)
        plural_form = gtranslator_profile_get_plurals (profile);
      else
        return;

      if (plural_form)
        {
          gtranslator_header_set_plural_forms (header, plural_form);
          plural_forms = g_strdup (plural_form);
        }
    }

  pointer = plural_forms;

  while (*pointer != '=')
    pointer++;
  pointer++;

  //if there are any space between '=' and nplural number pointer++
  while (*pointer == ' ')
    pointer++;

  header->priv->nplurals = g_ascii_digit_value (*pointer);

  g_free (plural_forms);
}

static void
gtranslator_header_init (GtranslatorHeader * header)
{
  header->priv = GTR_HEADER_GET_PRIVATE (header);

  header->priv->nplurals = -1;
}

static void
gtranslator_header_finalize (GObject * object)
{
  G_OBJECT_CLASS (gtranslator_header_parent_class)->finalize (object);
}

static void
gtranslator_header_class_init (GtranslatorHeaderClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtranslatorHeaderPrivate));

  object_class->finalize = gtranslator_header_finalize;
}

/* Public methods */

GtranslatorHeader *
gtranslator_header_new (po_message_iterator_t iter,
                        po_message_t          message)
{
  GtranslatorHeader *header;

  header = g_object_new (GTR_TYPE_HEADER, "gettext-iter", iter,
                         "gettext-msg", message, NULL);

  /* We have to parse the number of plurals */
  parse_nplurals (header);

  return header;
}

/**
 * gtranslator_header_get_comments:
 * @header: a #GtranslatorHeader
 *
 * Gets the comments of the header.
 *
 * Return value: the comments of the header.
 */
const gchar *
gtranslator_header_get_comments (GtranslatorHeader *header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_message_comments (gtranslator_msg_get_message (GTR_MSG (header)));
}

void
gtranslator_header_set_comments (GtranslatorHeader *header,
                                 const gchar *comments)
{
  g_return_if_fail (GTR_IS_HEADER (header));
  g_return_if_fail (comments != NULL);
 
  po_message_set_comments (gtranslator_msg_get_message (GTR_MSG (header)),
                           comments);
}

gchar *
gtranslator_header_get_prj_id_version (GtranslatorHeader *header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_header_field (gtranslator_msg_get_msgstr (GTR_MSG (header)),
                          "Project-Id-Version");
}

void
gtranslator_header_set_prj_id_version (GtranslatorHeader *header,
                                       const gchar *prj_id_version)
{
  g_return_if_fail (GTR_IS_HEADER (header));
  g_return_if_fail (prj_id_version != NULL);

  gtranslator_header_set_field (header, "Project-Id-Version",
                                prj_id_version);
}

gchar *
gtranslator_header_get_rmbt (GtranslatorHeader *header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_header_field (gtranslator_msg_get_msgstr (GTR_MSG (header)),
                          "Report-Msgid-Bugs-To");
}

void
gtranslator_header_set_rmbt (GtranslatorHeader *header,
                             const gchar *rmbt)
{
  /* FIXME: rmbt is not a good name */
  g_return_if_fail (GTR_IS_HEADER (header));
  g_return_if_fail (rmbt != NULL);

  gtranslator_header_set_field (header, "Report-Msgid-Bugs-To",
                                rmbt);
}

gchar *
gtranslator_header_get_pot_date (GtranslatorHeader *header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_header_field (gtranslator_msg_get_msgstr (GTR_MSG (header)),
                          "POT-Creation-Date");
}

void
gtranslator_header_set_pot_date (GtranslatorHeader *header,
                                 const gchar *pot_date)
{
  g_return_if_fail (GTR_IS_HEADER (header));
  g_return_if_fail (pot_date != NULL);

  gtranslator_header_set_field (header, "POT-Creation-Date",
                                pot_date);
}

gchar *
gtranslator_header_get_po_date (GtranslatorHeader *header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_header_field (gtranslator_msg_get_msgstr (GTR_MSG (header)),
                          "PO-Revision-Date");
}

void
gtranslator_header_set_po_date (GtranslatorHeader *header,
                                const gchar *po_date)
{
  g_return_if_fail (GTR_IS_HEADER (header));
  g_return_if_fail (po_date != NULL);

  gtranslator_header_set_field (header, "PO-Revision-Date",
                                po_date);
}

gchar *
gtranslator_header_get_translator (GtranslatorHeader *header)
{
  gchar *space, *translator_temp, *translator;

  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  translator_temp = po_header_field (gtranslator_msg_get_msgstr (GTR_MSG (header)),
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
gtranslator_header_get_tr_email (GtranslatorHeader *header)
{
  gchar *space, *email_temp, *email;

  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  email_temp = po_header_field (gtranslator_msg_get_msgstr (GTR_MSG (header)),
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
gtranslator_header_set_translator (GtranslatorHeader *header,
                                   const gchar *name,
                                   const gchar *email)
{
  gchar *translator;

  g_return_if_fail (GTR_IS_HEADER (header));

  translator = g_strconcat (name, " <", email, ">", NULL);

  gtranslator_header_set_field (header, "Last-Translator",
                                translator);

  g_free (translator);
}

gchar *
gtranslator_header_get_language (GtranslatorHeader *header)
{
  gchar *space, *lang_temp, *language;

  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  lang_temp = po_header_field (gtranslator_msg_get_msgstr (GTR_MSG (header)),
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
gtranslator_header_get_lg_email (GtranslatorHeader *header)
{
  gchar *space, *email_temp, *email;

  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  email_temp = po_header_field (gtranslator_msg_get_msgstr (GTR_MSG (header)),
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
gtranslator_header_set_language (GtranslatorHeader *header,
                                 const gchar *language,
                                 const gchar *email)
{
  gchar *lang_temp;

  g_return_if_fail (GTR_IS_HEADER (header));

  lang_temp = g_strconcat (language, " <", email, ">", NULL);

  gtranslator_header_set_field (header, "Language-Team",
                                lang_temp);

  g_free (lang_temp);
}

gchar *
gtranslator_header_get_mime_version (GtranslatorHeader *header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_header_field (gtranslator_msg_get_msgstr (GTR_MSG (header)),
                          "MIME-Version");
}

void
gtranslator_header_set_mime_version (GtranslatorHeader *header,
                                     const gchar *mime_version)
{
  g_return_if_fail (GTR_IS_HEADER (header));

  gtranslator_header_set_field (header, "MIME-Version",
                                mime_version);
}

gchar *
gtranslator_header_get_charset (GtranslatorHeader *header)
{
  gchar *space, *charset_temp, *charset;

  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  charset_temp = po_header_field (gtranslator_msg_get_msgstr (GTR_MSG (header)),
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
gtranslator_header_set_charset (GtranslatorHeader *header,
                                const gchar *charset)
{
  gchar *set;

  g_return_if_fail (GTR_IS_HEADER (header));

  set = g_strconcat("text/plain;", " charset=",
                    charset, NULL);

  gtranslator_header_set_field (header, "Content-Type",
                                set);

  g_free (set);
}

gchar *
gtranslator_header_get_encoding (GtranslatorHeader *header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_header_field (gtranslator_msg_get_msgstr (GTR_MSG (header)),
                          "Content-Transfer-Encoding");
}

void
gtranslator_header_set_encoding (GtranslatorHeader *header,
                                 const gchar *encoding)
{
  g_return_if_fail (GTR_IS_HEADER (header));

  gtranslator_header_set_field (header, "Content-Transfer-Encoding",
                                encoding);
}

/**
 * gtranslator_header_get_plural_forms:
 * @header: a #GtranslatorHeader.
 *
 * Return value: a new allocated string with the plural form of the po file.
 */
gchar *
gtranslator_header_get_plural_forms (GtranslatorHeader *header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), NULL);

  return po_header_field (gtranslator_msg_get_msgstr (GTR_MSG (header)),
                          "Plural-Forms");
}

/**
 * gtranslator_header_set_plural_forms:
 * @header: a #GtranslatorHeader
 * @plural_forms: the plural forms string.
 *
 * Sets the plural form string in the @header and it sets the number of plurals.
 */
void
gtranslator_header_set_plural_forms (GtranslatorHeader * header,
                                     const gchar * plural_forms)
{
  g_return_if_fail (GTR_IS_HEADER (header));
  g_return_if_fail (plural_forms != NULL);

  gtranslator_header_set_field (header, "Plural-Forms",
                                plural_forms);

  /*Now we parse the plural forms to know the number of plurals */
  parse_nplurals (header);
}

/**
 * gtranslator_header_get_plural:
 * @header: a #GtranslatorHeader
 *
 * Return value: The number of plurals of the po file, if there is not a plural
 * form in the po file it returns the predefined by user number of plurals 
 * or 1 if there is not a plural form string stored.
 */
gint
gtranslator_header_get_nplurals (GtranslatorHeader * header)
{
  const gchar *plural_form;

  g_return_val_if_fail (GTR_IS_HEADER (header), 1);

  if (header->priv->nplurals > -1)
    return header->priv->nplurals;
  else
    return 1;
}

static void
set_profile_values (GtranslatorHeader *header)
{
  if (gtranslator_prefs_manager_get_use_profile_values ())
    {
      GtranslatorProfile *active_profile;

      active_profile = gtranslator_application_get_active_profile (GTR_APP);

      gtranslator_header_set_translator (header, gtranslator_profile_get_author_name (active_profile),
                                         gtranslator_profile_get_author_email (active_profile));
      gtranslator_header_set_language (header, gtranslator_profile_get_language_name (active_profile),
                                       gtranslator_profile_get_group_email (active_profile));
      gtranslator_header_set_charset (header, gtranslator_profile_get_charset (active_profile));
      gtranslator_header_set_encoding (header, gtranslator_profile_get_encoding (active_profile));
      gtranslator_header_set_plural_forms (header, gtranslator_profile_get_plurals (active_profile));
    }
}

static void
update_po_date (GtranslatorHeader *header)
{
  gchar *current_date;
  gchar *current_time;
  gchar *new_date;

  current_date = gtranslator_utils_get_current_date ();
  current_time = gtranslator_utils_get_current_time ();

  new_date = g_strconcat (current_date, " ", current_time, NULL);

  g_free (current_date);
  g_free (current_time);

  gtranslator_header_set_po_date (header, new_date);

  g_free (new_date);
}

static void
update_comments (GtranslatorHeader *header,
                 const gchar       *comments)
{
  GString *new_comments;
  GString *years;
  gchar **comment_lines;
  gchar *translator;
  gchar *email;
  gchar *current_year;
  gint i;

  current_year = gtranslator_utils_get_current_year ();

  /* Save the previous translator to update the header's comment */
  if (gtranslator_prefs_manager_get_use_profile_values ())
    {
      GtranslatorProfile *active_profile;

      active_profile = gtranslator_application_get_active_profile (GTR_APP);

      translator = g_strdup (gtranslator_profile_get_author_name (active_profile));
      email = g_strdup (gtranslator_profile_get_author_email (active_profile));
    }
  else
    {
      translator = gtranslator_header_get_translator (header);
      email = gtranslator_header_get_tr_email (header);
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

  g_string_append_printf (new_comments, "\n%s <%s>,%s",
                          translator, email, years->str);

  g_string_free (years, TRUE);

  gtranslator_header_set_comments (header, new_comments->str);

  g_string_free (new_comments, TRUE);
}

/* FIXME: complete this */
static void
add_default_comments (GtranslatorHeader *header)
{
}

void
gtranslator_header_update_header (GtranslatorHeader *header)
{
  const gchar *comments;

  /* If needed update the header with the profile values */
  set_profile_values (header);

  /* Update the po date */
  update_po_date (header);

  /* Update the header's comment */
  comments = gtranslator_header_get_comments (header);

  if (comments != NULL)
    update_comments (header, comments);
  else
    add_default_comments (header);
}