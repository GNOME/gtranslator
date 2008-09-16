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

#include "checkout-dialog.h"
#include "subversion-plugin.h"
#include "svn-checkout-command.h"
#include "utils.h"
#include "svn-command.h"
#include "statusbar.h"
#include "subversion-utils.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gio/gio.h>


#define GTR_CHECKOUT_DIALOG_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 	(object),	\
						 	GTR_TYPE_CHECKOUT_DIALOG,     \
						 	GtranslatorCheckoutDialogPrivate))


G_DEFINE_TYPE(GtranslatorCheckoutDialog, gtranslator_checkout_dialog, GTK_TYPE_DIALOG)

struct _GtranslatorCheckoutDialogPrivate
{
	GtkWidget *main_box;
	GtkWidget *path_entry;
	GtkWidget *dir_find_button;
	GtkWidget *url_entry;
	
	GtkListStore *store;
	GtkWidget *checkout_treeview;
	
	GtranslatorWindow *window;
};

enum 
{
	ICON_COLUMN,
	TEXT_COLUMN,
	N_COLUMNS
};

static void
on_checkout_command_finished (GtranslatorCommand *command,
			    guint return_code,
			    GtranslatorCheckoutDialog *dlg)
{
	GtranslatorStatusbar *status;
	
	status = GTR_STATUSBAR (gtranslator_window_get_statusbar (dlg->priv->window));
	
	gtranslator_statusbar_flash_message (status, 0,
					     _("Subversion: Checkout complete."));
	
	subversion_utils_report_errors (dlg->priv->window, 
					command, return_code);
	
	svn_checkout_command_destroy (SVN_CHECKOUT_COMMAND (command));	
}

static void
on_checkout_command_info_arrived (GtranslatorCommand *command,
				GtranslatorCheckoutDialog *dlg)
{
	GQueue *info;
	gchar *message;
	GtkTreeIter iter;
	
	info = svn_command_get_info_queue (SVN_COMMAND (command));
	
	while (g_queue_peek_head (info))
	{
		message = g_queue_pop_head (info);
		
		gtk_list_store_append (dlg->priv->store, &iter);
		gtk_list_store_set (dlg->priv->store, &iter,
				    TEXT_COLUMN, message,
				    -1);
		
		g_free (message);
	}
}

static void
dialog_response_handler (GtranslatorCheckoutDialog *dlg, 
			 gint       res_id)
{
	switch (res_id)
	{
		case GTK_RESPONSE_APPLY:
		{
			const gchar *path, *url;
			SvnCheckoutCommand *checkout_command;
			
			path = gtk_entry_get_text (GTK_ENTRY (dlg->priv->path_entry));
			if (strcmp ("", path) == 0)
			{
				GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW (dlg->priv->window),
									    GTK_DIALOG_DESTROY_WITH_PARENT,
									    GTK_MESSAGE_WARNING,
									    GTK_BUTTONS_CLOSE,
									    _("Please, add a directory path to make the checkout"));
				gtk_dialog_run (GTK_DIALOG (dialog));
				gtk_widget_destroy (dialog);
				break;
			}
			
			url = gtk_entry_get_text (GTK_ENTRY (dlg->priv->url_entry));
			if (strcmp ("", url) == 0)
			{
				GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW (dlg->priv->window),
									    GTK_DIALOG_DESTROY_WITH_PARENT,
									    GTK_MESSAGE_WARNING,
									    GTK_BUTTONS_CLOSE,
									    _("Please, add a Subversion URL"));
				gtk_dialog_run (GTK_DIALOG (dialog));
				gtk_widget_destroy (dialog);
				break;
			}

			checkout_command = svn_checkout_command_new (url, path);
			
			gtk_list_store_clear (dlg->priv->store);
			
			g_signal_connect (G_OBJECT (checkout_command), "command-finished",
					  G_CALLBACK (on_checkout_command_finished),
					  dlg);
			
			g_signal_connect (G_OBJECT (checkout_command), "data-arrived",
					  G_CALLBACK (on_checkout_command_info_arrived),
					  dlg);

			gtranslator_command_start (GTR_COMMAND (checkout_command));

			break;
		}
		default:
			gtk_widget_hide (GTK_WIDGET (dlg));
	}
}

static void
on_dir_find_button_clicked (GtkButton *button,
			    GtranslatorCheckoutDialog *dlg)
{
	GtkWidget *dialog;
	gint res;
	
	dialog = gtk_file_chooser_dialog_new (_("Checkout directory"),
					      GTK_WINDOW (dlg->priv->window),
					      GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER,
					      GTK_STOCK_CANCEL,
					      GTK_RESPONSE_CANCEL,
					      GTK_STOCK_OK,
					      GTK_RESPONSE_OK,
					      NULL);
	res = gtk_dialog_run (GTK_DIALOG (dialog));
	switch (res)
	{
		case GTK_RESPONSE_OK: 
		{
			gchar *filename;
			
			filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
			gtk_entry_set_text (GTK_ENTRY (dlg->priv->path_entry),
					    filename);
			g_free (filename);
			break;
		}
		default:
			break;
	}
	gtk_widget_destroy (dialog);
}

static void
setup_treeview (GtranslatorCheckoutDialog *dlg)
{
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;
	
	dlg->priv->store = gtk_list_store_new (N_COLUMNS,
					       GDK_TYPE_PIXBUF,
					       G_TYPE_STRING);
	
	gtk_tree_view_set_model (GTK_TREE_VIEW (dlg->priv->checkout_treeview),
				 GTK_TREE_MODEL (dlg->priv->store));
	
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (dlg->priv->checkout_treeview),
					   FALSE);
	
	column = gtk_tree_view_column_new ();
	/*
	 * Icon column
	 */
	renderer = gtk_cell_renderer_pixbuf_new ();
	g_object_set (renderer,
		      "stock-id", GTK_STOCK_INFO,
		      "stock-size", GTK_ICON_SIZE_MENU,
		      "xalign", 1.0,
		      "xpad", 6,
		      NULL);

	gtk_tree_view_column_pack_start (column, renderer, FALSE);
	
	/*
	 * Text column
	 */
	
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_column_pack_start (column, renderer, FALSE);
	gtk_tree_view_column_set_attributes (column,
					     renderer,
					     "text", TEXT_COLUMN,
					     NULL);

	gtk_tree_view_column_set_resizable (column, FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW (dlg->priv->checkout_treeview),
				     column);
}

static void
gtranslator_checkout_dialog_init (GtranslatorCheckoutDialog *dlg)
{
	gboolean ret;
	GtkWidget *error_widget;
	
	dlg->priv = GTR_CHECKOUT_DIALOG_GET_PRIVATE (dlg);
	
	gtk_dialog_add_buttons (GTK_DIALOG (dlg),
				GTK_STOCK_REFRESH,
				GTK_RESPONSE_APPLY,
				GTK_STOCK_CLOSE,
				GTK_RESPONSE_CLOSE,
				NULL);
	
	gtk_window_set_title (GTK_WINDOW (dlg), _("Checkout repository"));
	gtk_window_set_default_size (GTK_WINDOW (dlg), 600, 600);
	gtk_window_set_resizable (GTK_WINDOW (dlg), TRUE);
	gtk_dialog_set_has_separator (GTK_DIALOG (dlg), FALSE);
	gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);
	
	/* HIG defaults */
	gtk_container_set_border_width (GTK_CONTAINER (dlg), 5);
	gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (dlg)->vbox), 2); /* 2 * 5 + 2 = 12 */
	gtk_container_set_border_width (GTK_CONTAINER (GTK_DIALOG (dlg)->action_area), 5);
	gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (dlg)->action_area), 4);
	
	g_signal_connect (dlg,
			  "response",
			  G_CALLBACK (dialog_response_handler),
			  NULL);
	
	/*Glade*/
	ret = gtranslator_utils_get_glade_widgets (GLADE_FILE,
		"checkout_main_box",
		&error_widget,
		
		"checkout_main_box", &dlg->priv->main_box,
		"path_entry", &dlg->priv->path_entry,
		"dir_find_button", &dlg->priv->dir_find_button,
		"url_entry", &dlg->priv->url_entry,
		"checkout_treeview", &dlg->priv->checkout_treeview,
		
		NULL);
	
	if(!ret)
	{
		gtk_widget_show (error_widget);
		gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dlg)->vbox),
					     error_widget);
		
		return;
	}
	
	setup_treeview (dlg);
	
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dlg)->vbox),
			    dlg->priv->main_box, TRUE, TRUE, 0);
	
	gtk_container_set_border_width (GTK_CONTAINER (dlg->priv->main_box), 5);
	
	g_signal_connect (dlg->priv->dir_find_button, "clicked",
			  G_CALLBACK (on_dir_find_button_clicked), dlg);
}

static void
gtranslator_checkout_dialog_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_checkout_dialog_parent_class)->finalize (object);
}

static void
gtranslator_checkout_dialog_class_init (GtranslatorCheckoutDialogClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorCheckoutDialogPrivate));

	object_class->finalize = gtranslator_checkout_dialog_finalize;
}

void
gtranslator_show_checkout_dialog (GtranslatorWindow *window)
{
	static GtranslatorCheckoutDialog *dlg = NULL;
	
	g_return_if_fail (GTR_IS_WINDOW (window));
	
	if(dlg == NULL)
	{
		dlg = g_object_new (GTR_TYPE_CHECKOUT_DIALOG, NULL);
		
		gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg),
						    TRUE);
		
		dlg->priv->window = window;
		
		g_signal_connect (dlg,
				  "destroy",
				  G_CALLBACK (gtk_widget_destroyed),
				  &dlg);

		gtk_widget_show (GTK_WIDGET (dlg));
	}
	
	if (GTK_WINDOW (window) != gtk_window_get_transient_for (GTK_WINDOW (dlg)))
	{
		gtk_window_set_transient_for (GTK_WINDOW (dlg),
					      GTK_WINDOW (window));
	}

	gtk_window_present (GTK_WINDOW (dlg));
}
