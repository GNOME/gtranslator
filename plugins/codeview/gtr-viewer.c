/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-dirs.h"
#include "gtr-utils.h"
#include "gtr-viewer.h"
#include "gtr-window.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <string.h>
#include <gtksourceview/gtksourcelanguagemanager.h>

#define GTR_VIEWER_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 	(object),	\
						 	GTR_TYPE_VIEWER,     \
						 	GtrViewerPrivate))


G_DEFINE_TYPE (GtrViewer, gtr_viewer, GTK_TYPE_DIALOG)

struct _GtrViewerPrivate
{
  GtkWidget *main_box;
  GtkWidget *view;
  GtkWidget *filename_label;
};

static void
dialog_response_handler (GtkDialog * dlg, gint res_id)
{
  switch (res_id)
    {
    default:
      gtk_widget_destroy (GTK_WIDGET (dlg));
    }
}

static void
gtr_viewer_init (GtrViewer * dlg)
{
  gboolean ret;
  GtkWidget *error_widget, *action_area;
  GtkBox *content_area;
  GtkWidget *sw;
  gchar *path;
  gchar *root_objects[] = {
    "main_box",
    NULL
  };

  dlg->priv = GTR_VIEWER_GET_PRIVATE (dlg);

  gtk_dialog_add_buttons (GTK_DIALOG (dlg),
                          GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE, NULL);

  gtk_window_set_title (GTK_WINDOW (dlg), _("Source Viewer"));
  gtk_window_set_default_size (GTK_WINDOW (dlg), 800, 600);
  gtk_window_set_resizable (GTK_WINDOW (dlg), TRUE);
  gtk_dialog_set_has_separator (GTK_DIALOG (dlg), FALSE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);

  action_area = gtk_dialog_get_action_area (GTK_DIALOG (dlg));
  content_area = GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dlg)));

  /* HIG defaults */
  gtk_container_set_border_width (GTK_CONTAINER (dlg), 5);
  gtk_box_set_spacing (content_area, 2);    /* 2 * 5 + 2 = 12 */
  gtk_container_set_border_width (GTK_CONTAINER (action_area), 5);
  gtk_box_set_spacing (GTK_BOX (action_area), 4);

  g_signal_connect (dlg,
                    "response", G_CALLBACK (dialog_response_handler), NULL);

  /*Builder */
  path = gtr_dirs_get_ui_file ("gtr-viewer.ui");
  ret = gtr_utils_get_ui_objects (path,
                                  root_objects,
                                  &error_widget,
                                  "main_box", &dlg->priv->main_box,
                                  "scrolledwindow", &sw,
                                  "filename_label",
                                  &dlg->priv->filename_label, NULL);
  g_free (path);

  if (!ret)
    {
      gtk_widget_show (error_widget);
      gtk_box_pack_start (content_area, error_widget, TRUE, TRUE, 0);

      return;
    }

  gtk_box_pack_start (content_area, dlg->priv->main_box, TRUE, TRUE, 0);

  gtk_container_set_border_width (GTK_CONTAINER (dlg->priv->main_box), 5);

  /* Source view */
  dlg->priv->view = gtk_source_view_new ();
  gtk_text_view_set_editable (GTK_TEXT_VIEW (dlg->priv->view), FALSE);
  gtk_widget_show (dlg->priv->view);
  gtk_container_add (GTK_CONTAINER (sw), dlg->priv->view);

  gtk_source_view_set_highlight_current_line (GTK_SOURCE_VIEW
                                              (dlg->priv->view), TRUE);

  gtk_source_view_set_show_line_numbers (GTK_SOURCE_VIEW (dlg->priv->view),
                                         TRUE);

  gtk_source_view_set_show_right_margin (GTK_SOURCE_VIEW (dlg->priv->view),
                                         TRUE);
}

static void
gtr_viewer_finalize (GObject * object)
{
  G_OBJECT_CLASS (gtr_viewer_parent_class)->finalize (object);
}

static void
gtr_viewer_class_init (GtrViewerClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrViewerPrivate));

  object_class->finalize = gtr_viewer_finalize;
}

/***************** File loading *****************/

static void
error_dialog (GtkWindow * parent, const gchar * msg, ...)
{
  va_list ap;
  gchar *tmp;
  GtkWidget *dialog;

  va_start (ap, msg);
  tmp = g_strdup_vprintf (msg, ap);
  va_end (ap);

  dialog = gtk_message_dialog_new (parent,
                                   GTK_DIALOG_DESTROY_WITH_PARENT,
                                   GTK_MESSAGE_ERROR,
                                   GTK_BUTTONS_OK, "%s", tmp);
  g_free (tmp);

  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

static gboolean
gtk_source_buffer_load_file (GtkSourceBuffer * source_buffer,
                             const gchar * filename, GError ** error)
{
  GtkTextIter iter;
  gchar *buffer;
  GError *error_here = NULL;

  g_return_val_if_fail (GTK_IS_SOURCE_BUFFER (source_buffer), FALSE);
  g_return_val_if_fail (filename != NULL, FALSE);

  if (!g_file_get_contents (filename, &buffer, NULL, &error_here))
    {
      error_dialog (NULL, "%s\nFile %s", error_here->message, filename);
      g_propagate_error (error, error_here);
      return FALSE;
    }

  gtk_source_buffer_begin_not_undoable_action (source_buffer);
  gtk_text_buffer_set_text (GTK_TEXT_BUFFER (source_buffer), buffer, -1);
  gtk_source_buffer_end_not_undoable_action (source_buffer);
  gtk_text_buffer_set_modified (GTK_TEXT_BUFFER (source_buffer), FALSE);

  /* move cursor to the beginning */
  gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (source_buffer), &iter);
  gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER (source_buffer), &iter);

  {
    GtkTextIter start, end;
    char *text;
    gtk_text_buffer_get_bounds (GTK_TEXT_BUFFER (source_buffer), &start,
                                &end);
    text =
      gtk_text_buffer_get_text (GTK_TEXT_BUFFER (source_buffer), &start, &end,
                                TRUE);
    g_assert (!strcmp (text, buffer));
    g_free (text);
  }

  g_free (buffer);
  return TRUE;
}

static void
remove_all_marks (GtkSourceBuffer * buffer)
{
  GtkTextIter s, e;

  gtk_text_buffer_get_bounds (GTK_TEXT_BUFFER (buffer), &s, &e);

  gtk_source_buffer_remove_source_marks (buffer, &s, &e, NULL);
}

/* Note this is wrong for several reasons, e.g. g_pattern_match is broken
 * for glob matching. */
static GtkSourceLanguage *
get_language_for_filename (const gchar * filename)
{
  const gchar *const *languages;
  gchar *filename_utf8;
  GtkSourceLanguageManager *manager;

  filename_utf8 = g_filename_to_utf8 (filename, -1, NULL, NULL, NULL);
  g_return_val_if_fail (filename_utf8 != NULL, NULL);

  manager = gtk_source_language_manager_get_default ();
  languages = gtk_source_language_manager_get_language_ids (manager);

  while (*languages != NULL)
    {
      GtkSourceLanguage *lang;
      gchar **globs, **p;

      lang = gtk_source_language_manager_get_language (manager, *languages);
      g_return_val_if_fail (GTK_IS_SOURCE_LANGUAGE (lang), NULL);
      ++languages;

      globs = gtk_source_language_get_globs (lang);
      if (globs == NULL)
        continue;

      for (p = globs; *p != NULL; p++)
        {
          if (g_pattern_match_simple (*p, filename_utf8))
            {
              g_strfreev (globs);
              g_free (filename_utf8);

              return lang;
            }
        }

      g_strfreev (globs);
    }

  g_free (filename_utf8);
  return NULL;
}

static GtkSourceLanguage *
get_language_for_file (const gchar * filename)
{
  GtkSourceLanguage *language = NULL;

  if (!language)
    language = get_language_for_filename (filename);

  return language;
}

static GtkSourceLanguage *
get_language_by_id (const gchar * id)
{
  GtkSourceLanguageManager *manager;
  manager = gtk_source_language_manager_get_default ();
  return gtk_source_language_manager_get_language (manager, id);
}

static GtkSourceLanguage *
get_language (GtkTextBuffer * buffer, const gchar * filename)
{
  GtkSourceLanguage *language = NULL;
  GtkTextIter start, end;
  gchar *text;
  gchar *lang_string;

  gtk_text_buffer_get_start_iter (buffer, &start);
  end = start;
  gtk_text_iter_forward_line (&end);

#define LANG_STRING "gtk-source-lang:"
  text = gtk_text_iter_get_slice (&start, &end);
  lang_string = strstr (text, LANG_STRING);
  if (lang_string != NULL)
    {
      gchar **tokens;

      lang_string += strlen (LANG_STRING);
      g_strchug (lang_string);

      tokens = g_strsplit_set (lang_string, " \t\n", 2);

      if (tokens != NULL && tokens[0] != NULL)
        language = get_language_by_id (tokens[0]);

      g_strfreev (tokens);
    }

  if (!language)
    language = get_language_for_file (filename);

  g_free (text);
  return language;
}

static gboolean
open_file (GtkSourceBuffer * buffer, const gchar * filename)
{
  GtkSourceLanguage *language = NULL;
  gchar *freeme = NULL;
  gboolean success = FALSE;
  GFile *file;
  gchar *path;

  file = g_file_new_for_path (filename);
  path = g_file_get_path (file);
  g_object_unref (file);

  remove_all_marks (buffer);

  success = gtk_source_buffer_load_file (buffer, path, NULL);

  if (!success)
    goto out;

  language = get_language (GTK_TEXT_BUFFER (buffer), filename);

  if (language == NULL)
    g_print ("No language found for file `%s'\n", filename);

  gtk_source_buffer_set_language (buffer, language);
  g_object_set_data_full (G_OBJECT (buffer),
                          "filename", g_strdup (filename),
                          (GDestroyNotify) g_free);

  if (language != NULL)
    {
      gchar **styles;

      styles = gtk_source_language_get_style_ids (language);

      if (styles == NULL)
        g_print ("No styles in language '%s'\n",
                 gtk_source_language_get_name (language));
      else
        {
          gchar **ids;
          g_print ("Styles in in language '%s':\n",
                   gtk_source_language_get_name (language));

          ids = styles;

          while (*ids != NULL)
            {
              const gchar *name;

              name = gtk_source_language_get_style_name (language, *ids);

              g_print ("- %s (name: '%s')\n", *ids, name);

              ++ids;
            }

          g_strfreev (styles);
        }

      g_print ("\n");
    }
out:
  g_free (freeme);
  return success;
}

static void
jump_to_line (GtkTextView * view, gint line)
{
  GtkTextBuffer *buffer;
  GtkTextIter iter;
  gint line_count;

  buffer = gtk_text_view_get_buffer (view);

  line_count = gtk_text_buffer_get_line_count (buffer);

  if (line >= line_count)
    gtk_text_buffer_get_end_iter (buffer, &iter);
  else
    gtk_text_buffer_get_iter_at_line (buffer, &iter, line - 1);

  gtk_text_buffer_place_cursor (buffer, &iter);

  gtk_text_view_scroll_to_mark (view,
                                gtk_text_buffer_get_insert (buffer),
                                0.25, FALSE, 0.0, 0.0);
}

void
gtr_show_viewer (GtrWindow * window, const gchar * path, gint line)
{
  static GtrViewer *dlg = NULL;

  g_return_if_fail (GTR_IS_WINDOW (window));

  if (dlg == NULL)
    {
      GtkSourceBuffer *buffer;
      gchar *label;

      dlg = g_object_new (GTR_TYPE_VIEWER, NULL);

      buffer =
        GTK_SOURCE_BUFFER (gtk_text_view_get_buffer
                           (GTK_TEXT_VIEW (dlg->priv->view)));

      open_file (buffer, path);
      jump_to_line (GTK_TEXT_VIEW (dlg->priv->view), line);

      label = g_strdup_printf ("<b>%s</b>", g_path_get_basename (path));
      gtk_label_set_markup (GTK_LABEL (dlg->priv->filename_label), label);
      g_free (label);

      g_signal_connect (dlg,
                        "destroy", G_CALLBACK (gtk_widget_destroyed), &dlg);
      gtk_widget_show (GTK_WIDGET (dlg));
    }

  if (GTK_WINDOW (window) != gtk_window_get_transient_for (GTK_WINDOW (dlg)))
    {
      gtk_window_set_transient_for (GTK_WINDOW (dlg), GTK_WINDOW (window));
    }

  gtk_window_present (GTK_WINDOW (dlg));
}
