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

#include "gtr-utils.h"
#include "gtr-dirs.h"

#include <string.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

static const gchar *badwords[] = {
  "a",
  //"all",
  "an",
  //"are",
  //"can",
  //"for",
  //"from",
  "have",
  //"it",
  //"may",
  //"not",
  "of",
  //"that",
  "the",
  //"this",
  //"was",
  "will",
  //"with",
  //"you",
  //"your",
  NULL
};

static gboolean
check_good_word (const gchar * word, gchar ** badwords)
{
  gboolean check = TRUE;
  gchar *lower = g_utf8_strdown (word, -1);
  gint i = 0;

  while (badwords[i] != NULL)
    {
      gchar *lower_collate = g_utf8_collate_key (lower, -1);

      if (strcmp (lower_collate, badwords[i]) == 0)
        {
          check = FALSE;
          g_free (lower_collate);
          break;
        }
      i++;
      g_free (lower_collate);
    }
  return check;
}

/**
 * gtr_utils_split_string_in_words:
 * @string: the text to process
 *
 * Process a text and split it in words using pango.
 * 
 * Returns: an array of words of the processed text
 */
gchar **
gtr_utils_split_string_in_words (const gchar * string)
{
  PangoLanguage *lang = pango_language_from_string ("en");
  PangoLogAttr *attrs;
  GPtrArray *array;
  gint char_len;
  gint i = 0;
  gchar *s;
  gchar *start = NULL;
  static gchar **badwords_collate = NULL;

  if (badwords_collate == NULL)
    {
      gint words_size = g_strv_length ((gchar **) badwords);
      gint x = 0;

      badwords_collate = g_new0 (gchar *, words_size + 1);

      while (badwords[x] != NULL)
        {
          badwords_collate[x] = g_utf8_collate_key (badwords[x], -1);
          x++;
        }
      badwords_collate[x] = NULL;
    }

  char_len = g_utf8_strlen (string, -1);
  attrs = g_new (PangoLogAttr, char_len + 1);

  pango_get_log_attrs (string,
                       strlen (string), -1, lang, attrs, char_len + 1);

  array = g_ptr_array_new ();

  s = (gchar *) string;
  while (i <= char_len)
    {
      gchar *end;

      if (attrs[i].is_word_start)
        start = s;
      if (attrs[i].is_word_end)
        {
          gchar *word;

          end = s;
          word = g_strndup (start, end - start);

          if (check_good_word (word, badwords_collate))
            g_ptr_array_add (array, word);
        }

      i++;
      s = g_utf8_next_char (s);
    }

  g_free (attrs);
  g_ptr_array_add (array, NULL);

  return (gchar **) g_ptr_array_free (array, FALSE);
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

  gdk_window_get_origin (w->window, x, y);
  gtk_widget_size_request (GTK_WIDGET (menu), &requisition);

  if (gtk_widget_get_direction (w) == GTK_TEXT_DIR_RTL)
    {
      *x += w->allocation.x + w->allocation.width - requisition.width;
    }
  else
    {
      *x += w->allocation.x;
    }

  *y += w->allocation.y + w->allocation.height;

  *push_in = TRUE;
}

/**
 * gtr_utils_menu_position_under_widget:
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

      gdk_window_get_origin (GTK_WIDGET (tree)->window, x, y);

      path = gtk_tree_model_get_path (model, &iter);
      gtk_tree_view_get_cell_area (tree, path, gtk_tree_view_get_column (tree, 0),      /* FIXME 0 for RTL ? */
                                   &rect);
      gtk_tree_path_free (path);

      *x += rect.x;
      *y += rect.y + rect.height;

      if (gtk_widget_get_direction (GTK_WIDGET (tree)) == GTK_TEXT_DIR_RTL)
        {
          GtkRequisition requisition;
          gtk_widget_size_request (GTK_WIDGET (menu), &requisition);
          *x += rect.width - requisition.width;
        }
    }
  else
    {
      /* no selection -> regular "under widget" positioning */
      gtr_utils_menu_position_under_widget (menu, x, y, push_in, tree);
    }
}

static GtkWidget *
handle_builder_error (const gchar * message, ...)
{
  GtkWidget *label;
  gchar *msg;
  gchar *msg_plain;
  va_list args;

  va_start (args, message);
  msg_plain = g_strdup_vprintf (message, args);

  msg = g_strconcat ("<span size=\"large\" weight=\"bold\">",
                     msg_plain, "</span>\n\n",
                     _("Please check your installation."), NULL);
  label = gtk_label_new (msg);

  gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);

  g_free (msg_plain);
  g_free (msg);

  gtk_misc_set_padding (GTK_MISC (label), 5, 5);

  return label;
}

/**
 * gtr_utils_get_ui_objects:
 * @filename: the path to the gtk builder file
 * @root_objects: a NULL terminated list of root objects to load or NULL to
 *                load all objects
 * @error_widget: a pointer were a #GtkLabel
 * @object_name: the name of the first object
 * @...: a pointer were the first object is returned, followed by more
 *       name / object pairs and terminated by NULL.
 *
 * This function gets the requested objects from a GtkBuilder ui file. In case
 * of error it returns FALSE and sets error_widget to a GtkLabel containing
 * the error message to display.
 *
 * Returns FALSE if an error occurs, TRUE on success.
 */
gboolean
gtr_utils_get_ui_objects (const gchar * filename,
                          gchar ** root_objects,
                          GtkWidget ** error_widget,
                          const gchar * object_name, ...)
{
  GtkBuilder *builder;
  va_list args;
  const gchar *name;
  GError *error = NULL;
  gchar *filename_markup;
  gboolean ret = TRUE;

  g_return_val_if_fail (filename != NULL, FALSE);
  g_return_val_if_fail (error_widget != NULL, FALSE);
  g_return_val_if_fail (object_name != NULL, FALSE);

  filename_markup = g_markup_printf_escaped ("<i>%s</i>", filename);
  *error_widget = NULL;

  builder = gtk_builder_new ();

  if (root_objects != NULL)
    gtk_builder_add_objects_from_file (builder,
                                       filename, root_objects, &error);
  else
    gtk_builder_add_from_file (builder, filename, &error);

  if (error != NULL)
    {
      *error_widget =
        handle_builder_error (_("Unable to open ui file %s. Error: %s"),
                              filename_markup, error->message);
      g_error_free (error);
      g_free (filename_markup);

      return FALSE;
    }

  va_start (args, object_name);
  for (name = object_name; name; name = va_arg (args, const gchar *))
    {
      GObject **gobj;

      gobj = va_arg (args, GObject **);
      *gobj = gtk_builder_get_object (builder, name);

      if (!*gobj)
        {
          *error_widget =
            handle_builder_error (_
                                  ("Unable to find the object '%s' inside file %s."),
                                  name, filename_markup), ret = FALSE;
          break;
        }

      /* we return a new ref for the root objects,
       * the others are already reffed by their parent root object */
      if (root_objects != NULL)
        {
          gint i;

          for (i = 0; root_objects[i] != NULL; ++i)
            {
              if ((strcmp (name, root_objects[i]) == 0))
                {
                  g_object_ref (*gobj);
                }
            }
        }
    }
  va_end (args);

  g_free (filename_markup);
  g_object_unref (builder);

  return ret;
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

  uris = g_uri_list_extract_uris ((gchar *) selection_data->data);

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
 * gtr_utils_activate_url:
 * @dialog: a #GtkAboutDialog
 * @url: the url to show
 * @data: useless data variable
 * 
 * Shows the corresponding @url in the default browser.
 */
void
gtr_utils_activate_url (GtkAboutDialog * dialog,
                        const gchar * url, gpointer data)
{
  //FIXME: gtk_url_show deprecates this func.
  gchar *open[3];

  if (g_find_program_in_path ("xdg-open"))
    {
      open[0] = "xdg-open";
    }
  else
    return;

  open[1] = (gchar *) url;
  open[2] = NULL;

  gdk_spawn_on_screen (gdk_screen_get_default (),
                       NULL,
                       open,
                       NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, NULL);
}

/**
 * gtr_utils_activate_email:
 * @dialog: a #GtkAboutDialog
 * @email: the email to show
 * @data: useless data variable
 * 
 * Shows the corresponding @email in the default mailer.
 */
void
gtr_utils_activate_email (GtkAboutDialog * dialog,
                          const gchar * email, gpointer data)
{
  //FIXME: gtk_url_show deprecates this func.
  gchar *open[3];

  if (g_find_program_in_path ("xdg-email"))
    {
      open[0] = "xdg-email";
    }
  else
    return;

  open[1] = (gchar *) email;
  open[2] = NULL;

  gdk_spawn_on_screen (gdk_screen_get_default (),
                       NULL,
                       open,
                       NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, NULL);
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
  GdkScreen *screen;
  gchar *command;
  const gchar *lang;
  const gchar *const *langs;
  gchar *uri = NULL;
  gchar *path;
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

      path = gtr_dirs_get_gtr_data_dir ();
      uri = g_build_filename (path, "gnome", "help", doc_id,
                              lang, file_name, NULL);
      g_free (path);

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

  command = g_strconcat ("gnome-help ghelp://", uri, NULL);
  g_free (uri);

  screen = gtk_widget_get_screen (GTK_WIDGET (parent));
  gdk_spawn_command_line_on_screen (screen, command, &error);


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

/**
 * gtr_utils_scan_dir:
 * @dir: the dir to parse
 * @list: the list where to store the GFiles
 * @po_name: the name of the specific po file to search or NULL.
 *
 * Scans the directory and subdirectories of @dir looking for filenames remained
 * with .po or files that matches @po_name. The contents of @list must be freed with
 * g_slist_foreach (list, (GFunc)g_object_unref, NULL).
 */
void
gtr_utils_scan_dir (GFile * dir, GSList ** list, const gchar * po_name)
{
  GFileInfo *info;
  GError *error;
  GFile *file;
  GFileEnumerator *enumerator;

  error = NULL;
  enumerator = g_file_enumerate_children (dir,
                                          G_FILE_ATTRIBUTE_STANDARD_NAME,
                                          G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                          NULL, &error);
  if (enumerator)
    {
      error = NULL;

      while ((info =
              g_file_enumerator_next_file (enumerator, NULL, &error)) != NULL)
        {
          const gchar *name;
          gchar *filename;

          name = g_file_info_get_name (info);
          file = g_file_get_child (dir, name);

          if (po_name != NULL)
            {
              if (g_str_has_suffix (po_name, ".po"))
                filename = g_strdup (po_name);
              else
                filename = g_strconcat (po_name, ".po", NULL);
            }
          else
            filename = g_strdup (".po");

          if (g_str_has_suffix (name, filename))
            *list = g_slist_prepend (*list, file);
          g_free (filename);

          gtr_utils_scan_dir (file, list, po_name);
          g_object_unref (info);
        }
      g_file_enumerator_close (enumerator, NULL, NULL);
      g_object_unref (enumerator);

      if (error)
        {
          g_warning ("%s", error->message);
        }
    }
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
