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

#include "update-dialog.h"
#include "subversion-plugin.h"
#include "svn-update-command.h"
#include "utils.h"
#include "svn-command.h"
#include "statusbar.h"
#include "subversion-utils.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gio/gio.h>


#define GTR_UPDATE_DIALOG_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 	(object),	\
						 	GTR_TYPE_UPDATE_DIALOG,     \
						 	GtranslatorUpdateDialogPrivate))


G_DEFINE_TYPE (GtranslatorUpdateDialog, gtranslator_update_dialog,
	       GTK_TYPE_DIALOG)
     struct _GtranslatorUpdateDialogPrivate
     {
       GtkWidget *main_box;
       GtkWidget *dir_button;

       GtkListStore *store;
       GtkWidget *update_treeview;

       GtranslatorWindow *window;
     };

     enum
     {
       ICON_COLUMN,
       TEXT_COLUMN,
       N_COLUMNS
     };

     static void
       on_update_command_finished (GtranslatorCommand * command,
				   guint return_code,
				   GtranslatorUpdateDialog * dlg)
{
  GtranslatorStatusbar *status;

  status =
    GTR_STATUSBAR (gtranslator_window_get_statusbar (dlg->priv->window));

  gtranslator_statusbar_flash_message (status, 0,
				       _("Subversion: Update complete."));

  subversion_utils_report_errors (dlg->priv->window, command, return_code);

  svn_update_command_destroy (SVN_UPDATE_COMMAND (command));
}

static void
on_update_command_info_arrived (GtranslatorCommand * command,
				GtranslatorUpdateDialog * dlg)
{
  GQueue *info;
  gchar *message;
  GtkTreeIter iter;

  info = svn_command_get_info_queue (SVN_COMMAND (command));

  while (g_queue_peek_head (info))
    {
      message = g_queue_pop_head (info);

      gtk_list_store_append (dlg->priv->store, &iter);
      gtk_list_store_set (dlg->priv->store, &iter, TEXT_COLUMN, message, -1);

      g_free (message);
    }
}

static void
dialog_response_handler (GtranslatorUpdateDialog * dlg, gint res_id)
{
  switch (res_id)
    {
    case GTK_RESPONSE_APPLY:
      {
	gchar *filename;
	SvnUpdateCommand *update_command;

	filename =
	  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER
					 (dlg->priv->dir_button));

	update_command = svn_update_command_new ((gchar *) filename,
						 (gchar *) "", TRUE);
	g_free (filename);

	gtk_list_store_clear (dlg->priv->store);

	g_signal_connect (G_OBJECT (update_command), "command-finished",
			  G_CALLBACK (on_update_command_finished), dlg);

	g_signal_connect (G_OBJECT (update_command), "data-arrived",
			  G_CALLBACK (on_update_command_info_arrived), dlg);

	gtranslator_command_start (GTR_COMMAND (update_command));

	break;
      }
    default:
      gtk_widget_hide (GTK_WIDGET (dlg));
    }
}

static void
setup_treeview (GtranslatorUpdateDialog * dlg)
{
  GtkTreeViewColumn *column;
  GtkCellRenderer *renderer;

  dlg->priv->store = gtk_list_store_new (N_COLUMNS,
					 GDK_TYPE_PIXBUF, G_TYPE_STRING);

  gtk_tree_view_set_model (GTK_TREE_VIEW (dlg->priv->update_treeview),
			   GTK_TREE_MODEL (dlg->priv->store));

  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW
				     (dlg->priv->update_treeview), FALSE);

  column = gtk_tree_view_column_new ();
  /*
   * Icon column
   */
  renderer = gtk_cell_renderer_pixbuf_new ();
  g_object_set (renderer,
		"stock-id", GTK_STOCK_INFO,
		"stock-size", GTK_ICON_SIZE_MENU,
		"xalign", 1.0, "xpad", 6, NULL);

  gtk_tree_view_column_pack_start (column, renderer, FALSE);

  /*
   * Text column
   */

  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_column_pack_start (column, renderer, FALSE);
  gtk_tree_view_column_set_attributes (column,
				       renderer, "text", TEXT_COLUMN, NULL);

  gtk_tree_view_column_set_resizable (column, FALSE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (dlg->priv->update_treeview),
			       column);
}

static void
setup_dir (GtranslatorUpdateDialog * dlg)
{
  GtranslatorTab *tab;
  GtranslatorPo *po;
  gchar *dir;
  GFile *location, *parent, *file;

  tab = gtranslator_window_get_active_tab (dlg->priv->window);
  po = gtranslator_tab_get_po (tab);
  location = gtranslator_po_get_location (po);

  /* Get the directory of the po file */
  parent = g_file_get_parent (location);
  g_object_unref (location);

  /* Get the parent directory */
  file = g_file_get_parent (parent);

  dir = g_file_get_uri (file);

  gtk_file_chooser_set_uri (GTK_FILE_CHOOSER (dlg->priv->dir_button), dir);
  g_free (dir);
  g_object_unref (file);
}

static void
gtranslator_update_dialog_init (GtranslatorUpdateDialog * dlg)
{
  gboolean ret;
  GtkWidget *error_widget;
  gchar *root_objects[] = {
    "upadate_main_box",
    NULL
  };

  dlg->priv = GTR_UPDATE_DIALOG_GET_PRIVATE (dlg);

  gtk_dialog_add_buttons (GTK_DIALOG (dlg),
			  GTK_STOCK_REFRESH,
			  GTK_RESPONSE_APPLY,
			  GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE, NULL);

  gtk_window_set_title (GTK_WINDOW (dlg), _("Update repository"));
  gtk_window_set_default_size (GTK_WINDOW (dlg), 600, 600);
  gtk_window_set_resizable (GTK_WINDOW (dlg), TRUE);
  gtk_dialog_set_has_separator (GTK_DIALOG (dlg), FALSE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);

  /* HIG defaults */
  gtk_container_set_border_width (GTK_CONTAINER (dlg), 5);
  gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (dlg)->vbox), 2);	/* 2 * 5 + 2 = 12 */
  gtk_container_set_border_width (GTK_CONTAINER
				  (GTK_DIALOG (dlg)->action_area), 5);
  gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (dlg)->action_area), 4);

  g_signal_connect (dlg,
		    "response", G_CALLBACK (dialog_response_handler), NULL);

  ret = gtranslator_utils_get_ui_objects (UI_FILE,
					  root_objects,
					  &error_widget,
					  "update_main_box",
					  &dlg->priv->main_box, "dir_button",
					  &dlg->priv->dir_button,
					  "update_treeview",
					  &dlg->priv->update_treeview, NULL);

  if (!ret)
    {
      gtk_widget_show (error_widget);
      gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dlg)->vbox),
			  error_widget, TRUE, TRUE, 0);

      return;
    }

  setup_treeview (dlg);

  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dlg)->vbox),
		      dlg->priv->main_box, TRUE, TRUE, 0);

  gtk_container_set_border_width (GTK_CONTAINER (dlg->priv->main_box), 5);

}

static void
gtranslator_update_dialog_finalize (GObject * object)
{
  G_OBJECT_CLASS (gtranslator_update_dialog_parent_class)->finalize (object);
}

static void
gtranslator_update_dialog_class_init (GtranslatorUpdateDialogClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtranslatorUpdateDialogPrivate));

  object_class->finalize = gtranslator_update_dialog_finalize;
}

void
gtranslator_show_update_dialog (GtranslatorWindow * window)
{
  static GtranslatorUpdateDialog *dlg = NULL;

  g_return_if_fail (GTR_IS_WINDOW (window));

  if (dlg == NULL)
    {
      dlg = g_object_new (GTR_TYPE_UPDATE_DIALOG, NULL);

      gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);

      dlg->priv->window = window;

      setup_dir (dlg);

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
