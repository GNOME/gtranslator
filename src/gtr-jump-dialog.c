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
#include "gtr-jump-dialog.h"
#include "gtr-tab.h"
#include "gtr-utils.h"
#include "gtr-window.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>


#define GTR_JUMP_DIALOG_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 	(object),	\
						 	GTR_TYPE_JUMP_DIALOG,     \
						 	GtrJumpDialogPrivate))


G_DEFINE_TYPE (GtrJumpDialog, gtr_jump_dialog, GTK_TYPE_DIALOG)
     struct _GtrJumpDialogPrivate
     {
       GtkWidget *main_box;
       GtkWidget *jump;

       GtrWindow *window;
     };

     static void dialog_response_handler (GtkDialog * dlg, gint res_id)
{
  GtrJumpDialog *dialog = GTR_JUMP_DIALOG (dlg);
  GtrTab *tab;
  gint number;

  switch (res_id)
    {
    case GTK_RESPONSE_OK:
      number =
        gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON
                                          (dialog->priv->jump));
      tab = gtr_window_get_active_tab (dialog->priv->window);
      gtr_tab_go_to_number (tab, number - 1);
      gtk_widget_destroy (GTK_WIDGET (dlg));
      break;

    default:
      gtk_widget_destroy (GTK_WIDGET (dlg));
    }
}

static void
gtr_jump_dialog_init (GtrJumpDialog * dlg)
{
  GtrJumpDialogPrivate *priv;
  GtkWidget *action_area;
  GtkBox *content_area;
  GtkBuilder *builder;
  gchar *root_objects[] = {
    "adjustment1",
    "main_box",
    NULL
  };

  dlg->priv = GTR_JUMP_DIALOG_GET_PRIVATE (dlg);
  priv = dlg->priv;

  gtk_dialog_add_buttons (GTK_DIALOG (dlg),
                          GTK_STOCK_OK,
                          GTK_RESPONSE_OK,
                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

  gtk_window_set_title (GTK_WINDOW (dlg), _("Go to Message"));
  gtk_window_set_default_size (GTK_WINDOW (dlg), 300, 100);
  gtk_window_set_resizable (GTK_WINDOW (dlg), TRUE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);

  action_area = gtk_dialog_get_action_area (GTK_DIALOG (dlg));
  content_area = GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dlg)));

  /* HIG defaults */
  gtk_container_set_border_width (GTK_CONTAINER (dlg), 5);
  gtk_box_set_spacing (content_area, 2);    /* 2 * 5 + 2 = 12 */
  gtk_container_set_border_width (GTK_CONTAINER (action_area), 5);
  gtk_box_set_spacing (GTK_BOX (action_area), 4);

  gtk_dialog_set_default_response (GTK_DIALOG (dlg), GTK_RESPONSE_OK);

  g_signal_connect (dlg,
                    "response", G_CALLBACK (dialog_response_handler), NULL);

  builder = gtk_builder_new ();
  gtk_builder_add_objects_from_resource (builder, "/org/gnome/gtranslator/ui/gtr-jump-dialog.ui",
                                         root_objects, NULL);
  priv->main_box = GTK_WIDGET (gtk_builder_get_object (builder, "main_box"));
  g_object_ref (priv->main_box);
  priv->jump = GTK_WIDGET (gtk_builder_get_object (builder, "jump"));
  g_object_unref (builder);

  gtk_box_pack_start (content_area, priv->main_box, TRUE, TRUE, 0);

  gtk_container_set_border_width (GTK_CONTAINER (priv->main_box), 5);
}

static void
gtr_jump_dialog_finalize (GObject * object)
{
  G_OBJECT_CLASS (gtr_jump_dialog_parent_class)->finalize (object);
}

static void
gtr_jump_dialog_class_init (GtrJumpDialogClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrJumpDialogPrivate));

  object_class->finalize = gtr_jump_dialog_finalize;
}

void
gtr_show_jump_dialog (GtrWindow * window)
{
  static GtrJumpDialog *dlg = NULL;

  g_return_if_fail (GTR_IS_WINDOW (window));

  if (dlg == NULL)
    {
      GtrTab *tab;
      GtrPo *po;
      gint messages;

      dlg = g_object_new (GTR_TYPE_JUMP_DIALOG, NULL);

      g_signal_connect (dlg,
                        "destroy", G_CALLBACK (gtk_widget_destroyed), &dlg);

      dlg->priv->window = window;

      /* Set the maximum number of the spin button */
      tab = gtr_window_get_active_tab (window);
      po = gtr_tab_get_po (tab);
      messages = gtr_po_get_messages_count (po);
      gtk_spin_button_set_range (GTK_SPIN_BUTTON (dlg->priv->jump),
                                 1.0, (gdouble) messages);

      gtk_widget_show (GTK_WIDGET (dlg));
    }

  if (GTK_WINDOW (window) != gtk_window_get_transient_for (GTK_WINDOW (dlg)))
    {
      gtk_window_set_transient_for (GTK_WINDOW (dlg), GTK_WINDOW (window));
    }

  gtk_window_present (GTK_WINDOW (dlg));
}
