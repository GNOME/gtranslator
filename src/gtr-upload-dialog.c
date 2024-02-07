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
#include "gtr-utils.h"

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
  GtkWidget *cancel;
} GtrUploadDialogPrivate;

struct _GtrUploadDialog
{
  AdwDialog parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrUploadDialog, gtr_upload_dialog,
                            ADW_TYPE_DIALOG)

enum
{
  RESPONSE,
  NO_OF_SIGNALS
};

static guint signals[NO_OF_SIGNALS];

static void
on_upload_button_clicked (GtkButton                  *button,
                          GtrUploadDialog            *dlg)
{
  g_signal_emit (GTR_UPLOAD_DIALOG (dlg), signals[RESPONSE], 0);
}

static void
on_cancel_button_clicked (GtkButton                  *button,
                          GtrUploadDialog            *dlg)
{
  adw_dialog_close (ADW_DIALOG (dlg));
}

static void
gtr_upload_dialog_class_init (GtrUploadDialogClass *klass)
{
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (klass),
                                               "/org/gnome/translator/gtr-upload-dialog.ui");

  /* Main layout widgets */
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), GtrUploadDialog, main_box);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), GtrUploadDialog, text_view);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), GtrUploadDialog, label);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), GtrUploadDialog, upload);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), GtrUploadDialog, cancel);

  // Custom signals
  signals[RESPONSE] =
    g_signal_new ("response",
                  G_OBJECT_CLASS_TYPE (klass),
                  G_SIGNAL_RUN_LAST,
                  0, NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
}

static void
gtr_upload_dialog_init (GtrUploadDialog *dlg)
{
  GtrUploadDialogPrivate *priv = gtr_upload_dialog_get_instance_private (dlg);
  gtk_widget_init_template (GTK_WIDGET (dlg));

  g_signal_connect (GTK_BUTTON (priv->upload), "clicked",
                    G_CALLBACK (on_upload_button_clicked), dlg);
  g_signal_connect (GTK_BUTTON (priv->cancel), "clicked",
                    G_CALLBACK (on_cancel_button_clicked), dlg);
}

GtrUploadDialog *
gtr_upload_dialog_new (GtkWidget  *parent)
{
  GtrUploadDialog *dlg;
  GtrUploadDialogPrivate *priv = NULL;

  dlg = g_object_new (GTR_TYPE_UPLOAD_DIALOG, NULL);
  priv = gtr_upload_dialog_get_instance_private (dlg);
  priv->parent = parent;

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
