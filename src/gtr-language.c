/*
 * gtr-language.c
 * This file is part of gtranslator
 *
 * Copyright (C) 2003, 2004 - Christian Persch
 * Copyright (C) 2006 - Paolo Maggi
 * Copyright (C) 2011 - Ignacio Casal Quinteiro
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib/gi18n.h>
#include <libxml/xmlreader.h>

#include "gtr-language.h"
#include "gtr-dirs.h"

#define ISO_639_DOMAIN	"iso_639"

#define ISOCODESLOCALEDIR		ISO_CODES_PREFIX "/share/locale"
#define GTR_PLURAL_FORMS_FILENAME	"gtr-plural-forms.ini"

struct _GtrLanguage
{
  gchar *code;
  gchar *name;
  gchar *plural_form;
};

static gboolean initialized = FALSE;
static GKeyFile *plurals_file = NULL;
static GSList *languages = NULL;

G_DEFINE_BOXED_TYPE (GtrLanguage, gtr_language,
                     gtr_language_copy,
                     gtr_language_free)

/* These are to make language bindings happy. Since Languages are
 * const, copy() just returns the same pointer and frees() doesn't
 * do nothing */

GtrLanguage *
gtr_language_copy (const GtrLanguage *lang)
{
	g_return_val_if_fail (lang != NULL, NULL);

	return (GtrLanguage *) lang;
}

void 
gtr_language_free (GtrLanguage *lang)
{
	g_return_if_fail (lang != NULL);
}

static void
bind_iso_domains (void)
{
  static gboolean bound = FALSE;

  if (bound == FALSE)
    {
      bindtextdomain (ISO_639_DOMAIN, ISOCODESLOCALEDIR);
      bind_textdomain_codeset (ISO_639_DOMAIN, "UTF-8");

      bound = TRUE;
    }
}

static void
load_plural_form (GtrLanguage *lang)
{
  gchar *plural_form;

  plural_form = g_key_file_get_string (plurals_file, "Plural Forms", lang->code, NULL);

  if (plural_form != NULL && *plural_form != '\0')
    {
      lang->plural_form = plural_form;
    }
  else
    {
      lang->plural_form = NULL;
    }
}

static void
read_iso_639_entry (xmlTextReaderPtr reader,
                    GSList **langs)
{
  xmlChar *code, *name;

  code = xmlTextReaderGetAttribute (reader, (const xmlChar *) "iso_639_1_code");
  name = xmlTextReaderGetAttribute (reader, (const xmlChar *) "name");

  if (code != NULL && code[0] != '\0' && name != NULL && name[0] != '\0')
    {
      GtrLanguage *lang = g_slice_new (GtrLanguage);
      lang->code = (gchar *)code;
      lang->name = g_strdup (dgettext (ISO_639_DOMAIN, (gchar *)name));
      xmlFree (name);

      /* set the plural form */
      load_plural_form (lang);

      *langs = g_slist_prepend (*langs, lang);
    }
  else
    {
      xmlFree (code);
      xmlFree (name);
    }
}

typedef enum
{
  STATE_START,
  STATE_STOP,
  STATE_ENTRIES
} ParserState;

static void
load_iso_entries (int iso,
                  GFunc read_entry_func,
                  gpointer user_data)
{
  xmlTextReaderPtr reader;
  ParserState state = STATE_START;
  xmlChar iso_entries[32], iso_entry[32];
  char *filename;
  int ret = -1;

  filename = g_strdup_printf (ISO_CODES_PREFIX "/share/xml/iso-codes/iso_%d.xml", iso);
  reader = xmlNewTextReaderFilename (filename);
  if (reader == NULL)
    goto out;

  xmlStrPrintf (iso_entries, sizeof (iso_entries), (const xmlChar *)"iso_%d_entries", iso);
  xmlStrPrintf (iso_entry, sizeof (iso_entry), (const xmlChar *)"iso_%d_entry", iso);

  ret = xmlTextReaderRead (reader);

  while (ret == 1)
    {
      const xmlChar *tag;
      xmlReaderTypes type;

      tag = xmlTextReaderConstName (reader);
      type = xmlTextReaderNodeType (reader);

      if (state == STATE_ENTRIES &&
          type == XML_READER_TYPE_ELEMENT &&
          xmlStrEqual (tag, iso_entry))
        {
          read_entry_func (reader, user_data);
        }
      else if (state == STATE_START &&
               type == XML_READER_TYPE_ELEMENT &&
               xmlStrEqual (tag, iso_entries))
        {
          state = STATE_ENTRIES;
        }
      else if (state == STATE_ENTRIES &&
               type == XML_READER_TYPE_END_ELEMENT &&
               xmlStrEqual (tag, iso_entries))
        {
          state = STATE_STOP;
        }
      else if (type == XML_READER_TYPE_SIGNIFICANT_WHITESPACE ||
               type == XML_READER_TYPE_WHITESPACE ||
               type == XML_READER_TYPE_TEXT ||
               type == XML_READER_TYPE_COMMENT)
        {
          /* eat it */
        }
      else
        {
          /* ignore it */
        }

      ret = xmlTextReaderRead (reader);
    }

  xmlFreeTextReader (reader);

out:
  if (ret < 0 || state != STATE_STOP)
    {
      g_warning ("Failed to load ISO-%d codes from %s!\n",
                 iso, filename);
    }

  g_free (filename);
}

static void
gtr_language_lazy_init (void)
{
  gchar *filename;

  if (initialized)
    return;

  plurals_file = g_key_file_new ();
  filename = gtr_dirs_get_ui_file (GTR_PLURAL_FORMS_FILENAME);
  if (!g_key_file_load_from_file (plurals_file, filename, G_KEY_FILE_NONE, NULL))
    {
      g_warning ("Bad plugin file: '%s'", filename);
      g_free (filename);
      return;
    }

  g_free (filename);

  bind_iso_domains ();
  load_iso_entries (639, (GFunc) read_iso_639_entry, &languages);
  languages = g_slist_reverse (languages);

  /* free the file, not needed anymore */
  g_key_file_free (plurals_file);

  initialized = TRUE;
}

const GSList *
gtr_language_get_languages ()
{
  gtr_language_lazy_init ();

  return languages;
}

const gchar *
gtr_language_get_code (const GtrLanguage *lang)
{
  g_return_val_if_fail (lang != NULL, NULL);

  gtr_language_lazy_init ();

  return lang->code;
}

const gchar *
gtr_language_get_name (const GtrLanguage *lang)
{
  g_return_val_if_fail (lang != NULL, NULL);

  gtr_language_lazy_init ();

  return lang->name;
}

const gchar *
gtr_language_get_plural_form (const GtrLanguage *lang)
{
  g_return_val_if_fail (lang != NULL, NULL);

  gtr_language_lazy_init ();

  return lang->plural_form;
}
