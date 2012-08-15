/*
 * (C) 2001-2007 Fatih Demir <kabalak@kabalak.net>
 *               Ignacio Casal <nacho.resa@gmail.com>
 * 		 Paolo Maggi
 *     2008      Igalia
 *
 * Based in gedit utils funcs
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

#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

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

/**
 * gtr_gtk_button_new_with_stock_icon:
 * @label: the label of the button
 * @stock_id: the id of the stock image
 * 
 * Convenience function to create a #GtkButton with a stock image.
 * 
 * Returns: a new #GtkButton
 */
GtkWidget *
gtr_gtk_button_new_with_stock_icon (const gchar * label,
                                    const gchar * stock_id)
{
  GtkWidget *button;

  button = gtk_button_new_with_mnemonic (label);
  gtk_button_set_image (GTK_BUTTON (button),
                        gtk_image_new_from_stock (stock_id,
                                                  GTK_ICON_SIZE_BUTTON));

  return button;
}

/**
 * gtr_utils_menu_position_under_widget:
 * @menu: a #GtkMenu
 * @x: the x position of the widget
 * @y: the y position of the widget
 * @push_in: 
 * @user_data: the widget to get the position
 * 
 * It returns the position to popup a menu in a specific widget.
 */
void
gtr_utils_menu_position_under_widget (GtkMenu * menu,
                                      gint * x,
                                      gint * y,
                                      gboolean * push_in, gpointer user_data)
{
  GtkWidget *w = GTK_WIDGET (user_data);
  GtkRequisition requisition;
  GtkAllocation allocation;

  gdk_window_get_origin (gtk_widget_get_window (w), x, y);
  gtk_widget_get_preferred_size (GTK_WIDGET (menu), &requisition,
                                 NULL);
  gtk_widget_get_allocation (w, &allocation);

  if (gtk_widget_get_direction (w) == GTK_TEXT_DIR_RTL)
    {
      *x += allocation.x + allocation.width - requisition.width;
    }
  else
    {
      *x += allocation.x;
    }

  *y += allocation.y + allocation.height;

  *push_in = TRUE;
}

/**
 * gtr_utils_menu_position_under_tree_view:
 * @menu: a #GtkMenu
 * @x: the x position of the widget
 * @y: the y position of the widget
 * @push_in: 
 * @user_data: the widget to get the position
 * 
 * It returns the position to popup a menu in a TreeView.
 */
void
gtr_utils_menu_position_under_tree_view (GtkMenu * menu,
                                         gint * x,
                                         gint * y,
                                         gboolean * push_in,
                                         gpointer user_data)
{
  GtkTreeView *tree = GTK_TREE_VIEW (user_data);
  GtkTreeModel *model;
  GtkTreeSelection *selection;
  GtkTreeIter iter;

  model = gtk_tree_view_get_model (tree);
  g_return_if_fail (model != NULL);

  selection = gtk_tree_view_get_selection (tree);
  g_return_if_fail (selection != NULL);

  if (gtk_tree_selection_get_selected (selection, NULL, &iter))
    {
      GtkTreePath *path;
      GdkRectangle rect;

      gdk_window_get_origin (gtk_widget_get_window (GTK_WIDGET (tree)), x, y);

      path = gtk_tree_model_get_path (model, &iter);
      gtk_tree_view_get_cell_area (tree, path, gtk_tree_view_get_column (tree, 0),      /* FIXME 0 for RTL ? */
                                   &rect);
      gtk_tree_path_free (path);

      *x += rect.x;
      *y += rect.y + rect.height;

      if (gtk_widget_get_direction (GTK_WIDGET (tree)) == GTK_TEXT_DIR_RTL)
        {
          GtkRequisition requisition;
          gtk_widget_get_preferred_size (GTK_WIDGET (menu),
                                         &requisition, NULL);
          *x += rect.width - requisition.width;
        }
    }
  else
    {
      /* no selection -> regular "under widget" positioning */
      gtr_utils_menu_position_under_widget (menu, x, y, push_in, tree);
    }
}


static gboolean
is_valid_scheme_character (gchar c)
{
  return g_ascii_isalnum (c) || c == '+' || c == '-' || c == '.';
}

static gboolean
has_valid_scheme (const gchar * uri)
{
  const gchar *p;

  p = uri;

  if (!is_valid_scheme_character (*p))
    {
      return FALSE;
    }

  do
    {
      p++;
    }
  while (is_valid_scheme_character (*p));

  return *p == ':';
}

static gboolean
gtr_utils_is_valid_uri (const gchar * uri)
{
  const guchar *p;

  if (uri == NULL)
    return FALSE;

  if (!has_valid_scheme (uri))
    return FALSE;

  /* We expect to have a fully valid set of characters */
  for (p = (const guchar *) uri; *p; p++)
    {
      if (*p == '%')
        {
          ++p;
          if (!g_ascii_isxdigit (*p))
            return FALSE;

          ++p;
          if (!g_ascii_isxdigit (*p))
            return FALSE;
        }
      else
        {
          if (*p <= 32 || *p >= 128)
            return FALSE;
        }
    }

  return TRUE;
}

/**
 * gtr_utils_drop_get_uris:
 * @selection_data: the #GtkSelectionData from drag_data_received
 *
 * Create a list of valid uri's from a uri-list drop.
 * 
 * Returns: a string array which will hold the uris or NULL if there 
 *		 were no valid uris. g_strfreev should be used when the 
 *		 string array is no longer used
 */
GSList *
gtr_utils_drop_get_locations (GtkSelectionData * selection_data)
{
  gchar **uris;
  gint i;
  GSList *locations = NULL;

  uris = g_uri_list_extract_uris ((gchar *) gtk_selection_data_get_data (selection_data));

  for (i = 0; uris[i] != NULL; i++)
    {
      GFile *file;
      /* Silently ignore malformed URI/filename */
      if (gtr_utils_is_valid_uri (uris[i]))
        {
          file = g_file_new_for_uri (uris[i]);
          locations = g_slist_prepend (locations, file);
        }
    }

  return locations;
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
 * @doc_id: the name of the type of doc
 * @file_name: the name of the doc
 * 
 * Shows the help for an specific document in the default help browser.
 */
void
gtr_utils_help_display (GtkWindow * parent,
                        const gchar * doc_id, const gchar * file_name)
{

  GError *error = NULL;
  gchar *command;
  const gchar *lang;
  const gchar *const *langs;
  const gchar *path;
  gchar *uri = NULL;
  gint i;


  /* FIXME: How to display help on windows. Gedit opens a browser and displays
     a url with the contents of the help 
     if (uri == NULL)
     {
     GtkWidget *dialog;
     dialog = gtk_message_dialog_new (parent,
     GTK_DIALOG_DESTROY_WITH_PARENT,
     GTK_MESSAGE_ERROR,
     GTK_BUTTONS_CLOSE,
     _("Sorry, Gtranslator for windows is unable to display help yet."));
     gtk_dialog_run (GTK_DIALOG (dialog));
     gtk_widget_destroy (dialog);

     return;
     } End of FIXME: How to display help on windows. */


  g_return_if_fail (file_name != NULL);

  langs = g_get_language_names ();
  for (i = 0; langs[i]; i++)
    {
      lang = langs[i];
      if (strchr (lang, '.'))
        continue;

      path = gtr_dirs_get_gtr_help_dir ();
      uri = g_build_filename (path, doc_id,
                              lang, file_name, NULL);

      if (g_file_test (uri, G_FILE_TEST_EXISTS))
        {
          break;
        }
      g_free (uri);
      uri = NULL;
    }


  if (uri == NULL)
    {
      GtkWidget *dialog;
      dialog = gtk_message_dialog_new (parent,
                                       GTK_DIALOG_DESTROY_WITH_PARENT,
                                       GTK_MESSAGE_ERROR,
                                       GTK_BUTTONS_CLOSE,
                                       _("Unable to display help. "
                                         "Please make sure the Gtranslator "
                                         "documentation package is installed."));
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);

      return;
    }

  command = g_strconcat ("gnome-help help://", uri, NULL);
  g_free (uri);

  g_spawn_command_line_async (command, &error);


  if (error != NULL)
    {
      g_warning ("Error executing help application: %s", error->message);
      g_error_free (error);

      return;
    }
  g_free (command);
}

gchar *
gtr_utils_get_current_date (void)
{
  time_t now;
  struct tm *now_here;
  gchar *date = g_malloc (11);

  now = time (NULL);
  now_here = localtime (&now);
  strftime (date, 11, "%Y-%m-%d", now_here);

  return date;
}

gchar *
gtr_utils_get_current_time (void)
{
  time_t now;
  struct tm *now_here;
  gchar *t = g_malloc (11);

  now = time (NULL);
  now_here = localtime (&now);
  strftime (t, 11, "%H:%M%z", now_here);

  return t;
}

gchar *
gtr_utils_get_current_year (void)
{
  time_t now;
  struct tm *now_here;
  gchar *year = g_malloc (5);

  now = time (NULL);
  now_here = localtime (&now);
  strftime (year, 5, "%Y", now_here);

  return year;
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
