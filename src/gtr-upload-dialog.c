/*
 * Copyright (C) 2021  Daniel García <danigm@gnome.org>
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
 *
 * Authors:
 *   Daniel García <danigm@gnome.org>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-upload-dialog.h"

#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

typedef struct
{
  GtkWidget *main_box;
  GtkWidget *text_view;
  GtkWidget *label;
  GtkWidget *parent;
  GtkWidget *upload;
} GtrUploadDialogPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GtrUploadDialog, gtr_upload_dialog, GTK_TYPE_DIALOG)

static void
gtr_upload_dialog_class_init (GtrUploadDialogClass *klass)
{
}

static void
gtr_upload_dialog_init (GtrUploadDialog *dlg)
{
  GtrUploadDialogPrivate *priv = gtr_upload_dialog_get_instance_private (dlg);
  GtkBox *content_area;
  GtkBuilder *builder;
  gchar *root_objects[] = {
    "main_box",
    NULL
  };

  priv->upload = gtk_dialog_add_button (GTK_DIALOG (dlg),
                                        _("_Upload"), GTK_RESPONSE_ACCEPT);

  gtk_window_set_title (GTK_WINDOW (dlg), _("Upload to Damned Lies"));
  gtk_window_set_resizable (GTK_WINDOW (dlg), FALSE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);
  gtk_window_set_modal (GTK_WINDOW (dlg), TRUE);

  content_area = GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dlg)));

  /* HIG defaults */
  gtk_container_set_border_width (GTK_CONTAINER (dlg), 5);
  gtk_box_set_spacing (content_area, 2); /* 2 * 5 + 2 = 12 */

  builder = gtk_builder_new ();
  GError *error = NULL;
  gtk_builder_add_objects_from_resource (builder, "/org/gnome/translator/gtr-upload-dialog.ui",
                                         root_objects, &error);
  if (error)
  {
    g_message("%s", error->message);
  }
  priv->main_box = GTK_WIDGET (gtk_builder_get_object (builder, "main_box"));
  g_object_ref (priv->main_box);
  priv->text_view = GTK_WIDGET (gtk_builder_get_object (builder, "text_view"));
  priv->label = GTK_WIDGET (gtk_builder_get_object (builder, "label"));
  g_object_unref (builder);

  gtk_box_pack_start (content_area, priv->main_box, FALSE, FALSE, 0);
}

GtrUploadDialog *
gtr_upload_dialog_new (GtkWidget  *parent)
{
  GtrUploadDialog *dlg;
  GtrUploadDialogPrivate *priv = NULL;

  dlg = g_object_new (GTR_TYPE_UPLOAD_DIALOG, NULL);
  priv = gtr_upload_dialog_get_instance_private (dlg);
  priv->parent = parent;

  if (GTK_WINDOW (parent) != gtk_window_get_transient_for (GTK_WINDOW (dlg)))
    {
      gtk_window_set_transient_for (GTK_WINDOW (dlg), GTK_WINDOW (parent));
    }

  return dlg;
}

char *
gtr_upload_dialog_get_comment (GtrUploadDialog *dlg)
{
  GtrUploadDialogPrivate *priv = gtr_upload_dialog_get_instance_private (dlg);
  GtkTextView *view = GTK_TEXT_VIEW (priv->text_view);
  GtkTextBuffer *buffer = gtk_text_view_get_buffer (view);
  char * comment = NULL;

  GtkTextIter start, end;
  gtk_text_buffer_get_start_iter (buffer, &start);
  gtk_text_buffer_get_end_iter (buffer, &end);
  comment = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

  return comment;
}

void
gtr_upload_dialog_set_loading (GtrUploadDialog *dlg,
                               gboolean         loading)
{
  GtrUploadDialogPrivate *priv = gtr_upload_dialog_get_instance_private (dlg);

  if (loading)
    {
      gtk_widget_set_sensitive (priv->text_view, FALSE);
      gtk_widget_set_sensitive (priv->upload, FALSE);
      gtk_button_set_label (GTK_BUTTON (priv->upload), _("Uploading..."));
    }
  else
    {
      gtk_widget_set_sensitive (priv->text_view, TRUE);
      gtk_widget_set_sensitive (priv->upload, TRUE);
      gtk_button_set_label (GTK_BUTTON (priv->upload), _("Upload"));
    }
}

GtkWidget *
gtr_upload_dialog_get_parent (GtrUploadDialog *dlg)
{
  GtrUploadDialogPrivate *priv = gtr_upload_dialog_get_instance_private (dlg);
  return priv->parent;
}
