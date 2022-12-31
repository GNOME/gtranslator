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
#include <gtksourceview/gtksource.h>

typedef struct
{
  GtkWidget *main_box;
  GtkWidget *view;
  GtkWidget *filename_label;
} GtrViewerPrivate;

struct _GtrViewer
{
  GtkWindow parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrViewer, gtr_viewer, GTK_TYPE_WINDOW)

static void
gtr_viewer_init (GtrViewer *dlg)
{
  GtkBox *content_area;
  GtkWidget *sw;
  GtkBuilder *builder;
  const gchar *root_objects[] = {
    "main_box",
    NULL
  };
  GtrViewerPrivate *priv = gtr_viewer_get_instance_private (dlg);
  GError *error = NULL;

  gtk_window_set_title (GTK_WINDOW (dlg), _("Source Viewer"));
  gtk_window_set_default_size (GTK_WINDOW (dlg), 800, 600);
  gtk_window_set_resizable (GTK_WINDOW (dlg), TRUE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);

  content_area = GTK_BOX (gtk_box_new (GTK_ORIENTATION_VERTICAL, 6));
  gtk_window_set_child (GTK_WINDOW (dlg), GTK_WIDGET (content_area));

  /*Builder */
  builder = gtk_builder_new ();
  gtk_builder_add_objects_from_resource (
    builder,
    "/org/gnome/gtranslator/plugins/codeview/ui/gtr-viewer.ui",
    root_objects,
    &error
  );

  if (error != NULL)
    {
      g_warning ("Error parsing gtr-viewer.ui: %s", (error)->message);
      g_error_free (error);
    }

  priv->main_box = GTK_WIDGET (gtk_builder_get_object (builder, "main_box"));
  g_object_ref (priv->main_box);
  sw = GTK_WIDGET (gtk_builder_get_object (builder, "scrolledwindow"));
  priv->filename_label = GTK_WIDGET (gtk_builder_get_object (builder, "filename_label"));
  g_object_unref (builder);

  gtk_widget_set_vexpand (priv->main_box, TRUE);
  gtk_box_append (content_area, priv->main_box);

  gtk_widget_set_margin_start (priv->main_box, 6);
  gtk_widget_set_margin_end (priv->main_box, 6);
  gtk_widget_set_margin_top (priv->main_box, 6);
  gtk_widget_set_margin_bottom (priv->main_box, 6);

  /* Source view */
  priv->view = gtk_source_view_new ();
  gtk_text_view_set_editable (GTK_TEXT_VIEW (priv->view), FALSE);
  gtk_widget_set_visible (priv->view, TRUE);
  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (sw), priv->view);

  gtk_source_view_set_highlight_current_line (GTK_SOURCE_VIEW
                                              (priv->view), TRUE);

  gtk_source_view_set_show_line_numbers (GTK_SOURCE_VIEW (priv->view),
                                         TRUE);

  gtk_source_view_set_show_right_margin (GTK_SOURCE_VIEW (priv->view),
                                         TRUE);
}

static void
gtr_viewer_finalize (GObject *object)
{
  G_OBJECT_CLASS (gtr_viewer_parent_class)->finalize (object);
}

static void
gtr_viewer_class_init (GtrViewerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gtr_viewer_finalize;
}

/***************** File loading *****************/

static void
error_dialog (GtkWindow *parent, const gchar *msg, ...)
{
  GtkAlertDialog *dialog;
  va_list ap;
  g_autofree char *tmp = NULL;

  va_start (ap, msg);
  tmp = g_strdup_vprintf (msg, ap);
  va_end (ap);

  dialog = gtk_alert_dialog_new ("%s", tmp);

  gtk_alert_dialog_show (GTK_ALERT_DIALOG (dialog), GTK_WINDOW (parent));
}

static gboolean
gtk_source_buffer_load_file (GtkSourceBuffer *source_buffer,
                             GFile           *file,
                             GError         **error)
{
  GtkTextIter iter;
  gchar *buffer;
  GError *error_here = NULL;
  g_autofree char *filename = NULL;

  g_return_val_if_fail (GTK_SOURCE_IS_BUFFER (source_buffer), FALSE);
  g_return_val_if_fail (G_IS_FILE (file), FALSE);

  filename = g_file_get_basename (file);

  if (!g_file_load_contents (file, NULL, &buffer, NULL, NULL, &error_here))
    {
      error_dialog (NULL, "%s\nFile %s", error_here->message, filename);
      g_propagate_error (error, error_here);
      return FALSE;
    }

  gtk_text_buffer_begin_irreversible_action (GTK_TEXT_BUFFER(source_buffer));
  gtk_text_buffer_set_text (GTK_TEXT_BUFFER (source_buffer), buffer, -1);
  gtk_text_buffer_end_irreversible_action (GTK_TEXT_BUFFER(source_buffer));
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
remove_all_marks (GtkSourceBuffer *buffer)
{
  GtkTextIter s, e;

  gtk_text_buffer_get_bounds (GTK_TEXT_BUFFER (buffer), &s, &e);

  gtk_source_buffer_remove_source_marks (buffer, &s, &e, NULL);
}

/* Note this is wrong for several reasons, e.g. g_pattern_match is broken
 * for glob matching. */
static GtkSourceLanguage *
get_language_for_filename (const gchar *filename)
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
      g_return_val_if_fail (GTK_SOURCE_IS_LANGUAGE (lang), NULL);
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
get_language_for_file (const gchar *filename)
{
  GtkSourceLanguage *language = NULL;

  if (!language)
    language = get_language_for_filename (filename);

  return language;
}

static GtkSourceLanguage *
get_language_by_id (const gchar *id)
{
  GtkSourceLanguageManager *manager;
  manager = gtk_source_language_manager_get_default ();
  return gtk_source_language_manager_get_language (manager, id);
}

static GtkSourceLanguage *
get_language (GtkTextBuffer *buffer, const gchar *filename)
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
open_file (GtkSourceBuffer *buffer, const gchar *filename)
{
  GtkSourceLanguage *language = NULL;
  gchar *freeme = NULL;
  gboolean success = FALSE;
  GFile *file;

  file = g_file_new_for_path (filename);

  remove_all_marks (buffer);

  success = gtk_source_buffer_load_file (buffer, file, NULL);

  g_object_unref (file);

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
jump_to_line (GtkTextView *view, gint line)
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
gtr_show_viewer (GtrWindow *window, const gchar *path, gint line)
{
  GtrViewer *dlg = NULL;
  GtrViewerPrivate *priv;
  GtkSourceBuffer *buffer;
  g_autofree char *label = NULL;

  dlg = g_object_new (GTR_TYPE_VIEWER, "use-header-bar", TRUE, NULL);
  priv = gtr_viewer_get_instance_private (dlg);

  buffer =
    GTK_SOURCE_BUFFER (gtk_text_view_get_buffer
                       (GTK_TEXT_VIEW (priv->view)));

  open_file (buffer, path);
  jump_to_line (GTK_TEXT_VIEW (priv->view), line);

  label = g_strdup_printf ("<b>%s</b>", g_path_get_basename (path));
  gtk_label_set_markup (GTK_LABEL (priv->filename_label), label);

  if (GTK_WINDOW (window) != gtk_window_get_transient_for (GTK_WINDOW (dlg)))
    {
      gtk_window_set_transient_for (GTK_WINDOW (dlg), GTK_WINDOW (window));
    }
  gtk_window_set_modal (GTK_WINDOW (dlg), TRUE);

  gtk_window_present (GTK_WINDOW (dlg));
}
