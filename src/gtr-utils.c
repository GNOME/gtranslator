/*
 * Copyright (C) 2001-2007  Fatih Demir <kabalak@kabalak.net>
 *                          Ignacio Casal <nacho.resa@gmail.com>
 *                          Paolo Maggi
 *               2008       Igalia
 *
 * Based in gedit utils funcs
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
 *   Fatih Demir <kabalak@kabalak.net>
 *   Pablo Sanxiao <psanxiao@gmail.com>
 *   Ignacio Casal <nacho.resa@gmail.com>
 *   Paolo Maggi 
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-utils.h"
#include "gtr-dirs.h"

#include <string.h>

#include <adwaita.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <libxml/parser.h>

static void
on_uri_launch (GObject *object, GAsyncResult *result, gpointer user_data)
{
  g_autoptr (GError) error = NULL;
  g_app_info_launch_default_for_uri_finish (result, &error);

  if (error)
    g_error ("Could not open help: %s", error->message);
}

xmlDocPtr
gtr_xml_new_doc (const gchar * name)
{
  xmlNodePtr root;
  xmlDocPtr doc;
  doc = xmlNewDoc ((const xmlChar *)"1.0");
  root = xmlNewDocNode (doc, NULL, (const xmlChar *)name, NULL);
  xmlDocSetRootElement (doc, root);
  return doc;
}

xmlDocPtr
gtr_xml_open_file (const gchar * filename)
{
  xmlDocPtr doc;
  g_return_val_if_fail (filename != NULL, NULL);
  doc = xmlParseFile (filename);
  return doc;
}

gchar *
gtr_utils_escape_search_text (const gchar * text)
{
  GString *str;
  gint length;
  const gchar *p;
  const gchar *end;

  if (text == NULL)
    return NULL;

  length = strlen (text);

  /* no escape when typing.
   * The short circuit works only for ascii, but we only
   * care about not escaping a single '\' */
  if (length == 1)
    return g_strdup (text);

  str = g_string_new ("");

  p = text;
  end = text + length;

  while (p != end)
    {
      const gchar *next;
      next = g_utf8_next_char (p);

      switch (*p)
        {
        case '\n':
          g_string_append (str, "\\n");
          break;
        case '\r':
          g_string_append (str, "\\r");
          break;
        case '\t':
          g_string_append (str, "\\t");
          break;
        case '\\':
          g_string_append (str, "\\\\");
          break;
        default:
          g_string_append_len (str, p, next - p);
          break;
        }

      p = next;
    }

  return g_string_free (str, FALSE);
}

gchar *
gtr_utils_unescape_search_text (const gchar * text)
{
  GString *str;
  gint length;
  gboolean drop_prev = FALSE;
  const gchar *cur;
  const gchar *end;
  const gchar *prev;

  if (text == NULL)
    return NULL;

  length = strlen (text);

  str = g_string_new ("");

  cur = text;
  end = text + length;
  prev = NULL;

  while (cur != end)
    {
      const gchar *next;
      next = g_utf8_next_char (cur);

      if (prev && (*prev == '\\'))
        {
          switch (*cur)
            {
            case 'n':
              str = g_string_append (str, "\n");
              break;
            case 'r':
              str = g_string_append (str, "\r");
              break;
            case 't':
              str = g_string_append (str, "\t");
              break;
            case '\\':
              str = g_string_append (str, "\\");
              drop_prev = TRUE;
              break;
            default:
              str = g_string_append (str, "\\");
              str = g_string_append_len (str, cur, next - cur);
              break;
            }
        }
      else if (*cur != '\\')
        {
          str = g_string_append_len (str, cur, next - cur);
        }
      else if ((next == end) && (*cur == '\\'))
        {
          str = g_string_append (str, "\\");
        }

      if (!drop_prev)
        {
          prev = cur;
        }
      else
        {
          prev = NULL;
          drop_prev = FALSE;
        }

      cur = next;
    }

  return g_string_free (str, FALSE);
}

/*
 * n: len of the string in bytes
 */
gboolean
g_utf8_caselessnmatch (const gchar * s1,
                       const gchar * s2, gssize n1, gssize n2)
{
  gchar *casefold;
  gchar *normalized_s1;
  gchar *normalized_s2;
  gint len_s1;
  gint len_s2;
  gboolean ret = FALSE;

  g_return_val_if_fail (s1 != NULL, FALSE);
  g_return_val_if_fail (s2 != NULL, FALSE);
  g_return_val_if_fail (n1 > 0, FALSE);
  g_return_val_if_fail (n2 > 0, FALSE);

  casefold = g_utf8_casefold (s1, n1);
  normalized_s1 = g_utf8_normalize (casefold, -1, G_NORMALIZE_NFD);
  g_free (casefold);

  casefold = g_utf8_casefold (s2, n2);
  normalized_s2 = g_utf8_normalize (casefold, -1, G_NORMALIZE_NFD);
  g_free (casefold);

  len_s1 = strlen (normalized_s1);
  len_s2 = strlen (normalized_s2);

  if (len_s1 < len_s2)
    goto finally_2;

  ret = (strncmp (normalized_s1, normalized_s2, len_s2) == 0);

finally_2:
  g_free (normalized_s1);
  g_free (normalized_s2);

  return ret;
}

/**
 * gtr_utils_help_display:
 * @parent: a #GtkWindow
 *
 * Launches the help viewer on @screen to show gtranslator's help
 */
void
gtr_utils_help_display (GtkWindow * window)
{
  GdkDisplay *display;
  GAppLaunchContext *context;

  display = gtk_widget_get_display (GTK_WIDGET (window));
  if (display != NULL)
    context = G_APP_LAUNCH_CONTEXT (gdk_display_get_app_launch_context (display));
  else
    context = NULL;

  g_app_info_launch_default_for_uri_async ("help:gtranslator", context,
                                           NULL, on_uri_launch, NULL);

  if (context)
    g_object_unref (context);
}

gchar *
gtr_utils_get_current_date (void)
{
  g_autoptr (GDateTime) now = NULL;

  now = g_date_time_new_now_local ();

  return g_date_time_format (now, "%Y-%m-%d");
}

gchar *
gtr_utils_get_current_time (void)
{
  g_autoptr (GDateTime) now = NULL;

  now = g_date_time_new_now_local ();

  return g_date_time_format (now, "%H:%M%z");
}

gchar *
gtr_utils_get_current_year (void)
{
  g_autoptr (GDateTime) now = NULL;

  now = g_date_time_new_now_local ();

  return g_date_time_format (now, "%Y");
}

gchar *
gtr_utils_reduce_path (const gchar * path)
{
  gchar *new_str;
  gchar **array;

  array = g_strsplit (path, "/", -1);

  new_str =
    g_build_filename (array[1], "/../", array[g_strv_length (array) - 1],
                      NULL);

  if (strlen (new_str) >= 30)
    {
      g_free (new_str);
      new_str =
        g_build_filename ("../", array[g_strv_length (array) - 1], NULL);
    }
  return new_str;
}

/*
 * Doubles underscore to avoid spurious menu accels.
 * Got from gedit
 */
gchar *
gtr_utils_escape_underscores (const gchar * text, gssize length)
{
  GString *str;
  const gchar *p;
  const gchar *end;

  g_return_val_if_fail (text != NULL, NULL);

  if (length < 0)
    length = strlen (text);

  str = g_string_sized_new (length);

  p = text;
  end = text + length;

  while (p != end)
    {
      const gchar *next;
      next = g_utf8_next_char (p);

      switch (*p)
        {
        case '_':
          g_string_append (str, "__");
          break;
        default:
          g_string_append_len (str, p, next - p);
          break;
        }

      p = next;
    }

  return g_string_free (str, FALSE);
}

// Removes the extension from a filename
gchar *
gtr_utils_get_filename (const gchar * filename)
{
  gchar *new_str;
  gchar **array;
  int l = 0;

  array = g_strsplit (filename, ".", -1);
  l = g_strv_length (array);

  new_str = g_strdup (array[0]);
  // Remove the extension
  for (int i=1; i < l-1; i++)
    {
      char *tmp = NULL;
      tmp = new_str;
      new_str = g_strjoin (".", tmp, array[i], NULL);
      g_free (tmp);
    }

  g_strfreev (array);

  return new_str;
}

/* Copied from gtkfontbutton.c */
static void
add_css_variations (GString    *s,
                    const char *variations)
{
  const char *p;
  const char *sep = "";

  if (variations == NULL || variations[0] == '\0')
    {
      g_string_append (s, "normal");
      return;
    }

  p = variations;
  while (p && *p)
    {
      const char *start;
      const char *end, *end2;
      double value;
      char name[5];

      while (g_ascii_isspace (*p)) p++;

      start = p;
      end = strchr (p, ',');
      if (end && (end - p < 6))
        goto skip;

      name[0] = p[0];
      name[1] = p[1];
      name[2] = p[2];
      name[3] = p[3];
      name[4] = '\0';

      p += 4;
      while (g_ascii_isspace (*p)) p++;
      if (*p == '=') p++;

      if (p - start < 5)
        goto skip;

      value = g_ascii_strtod (p, (char **) &end2);

      while (end2 && g_ascii_isspace (*end2)) end2++;

      if (end2 && (*end2 != ',' && *end2 != '\0'))
        goto skip;

      g_string_append_printf (s, "%s\"%s\" %g", sep, name, value);
      sep = ", ";

skip:
      p = end ? end + 1 : NULL;
    }
}

/* Copied from gtkfontbutton.c */
/* The only difference is the definition of `s`. */
gchar *
pango_font_description_to_css (PangoFontDescription *desc)
{
  GString *s;
  PangoFontMask set;

  s = g_string_new ("{ ");

  set = pango_font_description_get_set_fields (desc);
  if (set & PANGO_FONT_MASK_FAMILY)
    {
      g_string_append (s, "font-family: \"");
      g_string_append (s, pango_font_description_get_family (desc));
      g_string_append (s, "\"; ");
    }
  if (set & PANGO_FONT_MASK_STYLE)
    {
      switch (pango_font_description_get_style (desc))
        {
        case PANGO_STYLE_NORMAL:
          g_string_append (s, "font-style: normal; ");
          break;
        case PANGO_STYLE_OBLIQUE:
          g_string_append (s, "font-style: oblique; ");
          break;
        case PANGO_STYLE_ITALIC:
          g_string_append (s, "font-style: italic; ");
          break;
        default:
          break;
        }
    }
  if (set & PANGO_FONT_MASK_VARIANT)
    {
      switch (pango_font_description_get_variant (desc))
        {
        case PANGO_VARIANT_NORMAL:
          g_string_append (s, "font-variant: normal; ");
          break;
        case PANGO_VARIANT_SMALL_CAPS:
          g_string_append (s, "font-variant: small-caps; ");
          break;
        case PANGO_VARIANT_ALL_SMALL_CAPS:
          g_string_append (s, "font-variant: all-small-caps; ");
          break;
        case PANGO_VARIANT_PETITE_CAPS:
          g_string_append (s, "font-variant: petite-caps; ");
          break;
        case PANGO_VARIANT_ALL_PETITE_CAPS:
          g_string_append (s, "font-variant: all-petite-caps; ");
          break;
        case PANGO_VARIANT_UNICASE:
          g_string_append (s, "font-variant: unicase; ");
          break;
        case PANGO_VARIANT_TITLE_CAPS:
          g_string_append (s, "font-variant: titling-caps; ");
          break;
        default:
          break;
        }
    }
  if (set & PANGO_FONT_MASK_WEIGHT)
    {
      switch (pango_font_description_get_weight (desc))
        {
        case PANGO_WEIGHT_THIN:
          g_string_append (s, "font-weight: 100; ");
          break;
        case PANGO_WEIGHT_ULTRALIGHT:
          g_string_append (s, "font-weight: 200; ");
          break;
        case PANGO_WEIGHT_LIGHT:
        case PANGO_WEIGHT_SEMILIGHT:
          g_string_append (s, "font-weight: 300; ");
          break;
        case PANGO_WEIGHT_BOOK:
        case PANGO_WEIGHT_NORMAL:
          g_string_append (s, "font-weight: 400; ");
          break;
        case PANGO_WEIGHT_MEDIUM:
          g_string_append (s, "font-weight: 500; ");
          break;
        case PANGO_WEIGHT_SEMIBOLD:
          g_string_append (s, "font-weight: 600; ");
          break;
        case PANGO_WEIGHT_BOLD:
          g_string_append (s, "font-weight: 700; ");
          break;
        case PANGO_WEIGHT_ULTRABOLD:
          g_string_append (s, "font-weight: 800; ");
          break;
        case PANGO_WEIGHT_HEAVY:
        case PANGO_WEIGHT_ULTRAHEAVY:
          g_string_append (s, "font-weight: 900; ");
          break;
        default:
          break;
        }
    }
  if (set & PANGO_FONT_MASK_STRETCH)
    {
      switch (pango_font_description_get_stretch (desc))
        {
        case PANGO_STRETCH_ULTRA_CONDENSED:
          g_string_append (s, "font-stretch: ultra-condensed; ");
          break;
        case PANGO_STRETCH_EXTRA_CONDENSED:
          g_string_append (s, "font-stretch: extra-condensed; ");
          break;
        case PANGO_STRETCH_CONDENSED:
          g_string_append (s, "font-stretch: condensed; ");
          break;
        case PANGO_STRETCH_SEMI_CONDENSED:
          g_string_append (s, "font-stretch: semi-condensed; ");
          break;
        case PANGO_STRETCH_NORMAL:
          g_string_append (s, "font-stretch: normal; ");
          break;
        case PANGO_STRETCH_SEMI_EXPANDED:
          g_string_append (s, "font-stretch: semi-expanded; ");
          break;
        case PANGO_STRETCH_EXPANDED:
          g_string_append (s, "font-stretch: expanded; ");
          break;
        case PANGO_STRETCH_EXTRA_EXPANDED:
          break;
        case PANGO_STRETCH_ULTRA_EXPANDED:
          g_string_append (s, "font-stretch: ultra-expanded; ");
          break;
        default:
          break;
        }
    }
  if (set & PANGO_FONT_MASK_SIZE)
    {
      g_string_append_printf (s, "font-size: %dpt; ", pango_font_description_get_size (desc) / PANGO_SCALE);
    }

  if (set & PANGO_FONT_MASK_VARIATIONS)
    {
      const char *variations;

      g_string_append (s, "font-variation-settings: ");
      variations = pango_font_description_get_variations (desc);
      add_css_variations (s, variations);
      g_string_append (s, "; ");
    }

  g_string_append (s, "}");

  return g_string_free (s, FALSE);
}

// TODO: Improve this parser, this string parsing is weak
// It could be better to use GRegex: https://docs.gtk.org/glib/method.Regex.match.html
int
parse_nplurals_header (const gchar * plurals_header)
{
  if (plurals_header == NULL)
    return -1;

  gchar * pointer = g_strrstr (plurals_header, "nplurals");

  if (!pointer)
    return -1;

  while (*pointer != '\0' && *pointer != '=')
    pointer++;

  if (*pointer != '\0')
    {
      pointer++;
      while (*pointer != '\0' && *pointer == ' ')
        pointer++;

      if (*pointer == '\0')
        return -1;
    }
  else
    return -1;

  return g_ascii_digit_value (*pointer);
}
