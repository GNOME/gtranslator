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

#include "jump-dialog.h"
#include "tab.h"
#include "utils.h"
#include "window.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>


#define GTR_JUMP_DIALOG_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 	(object),	\
						 	GTR_TYPE_JUMP_DIALOG,     \
						 	GtranslatorJumpDialogPrivate))


G_DEFINE_TYPE(GtranslatorJumpDialog, gtranslator_jump_dialog, GTK_TYPE_DIALOG)

struct _GtranslatorJumpDialogPrivate
{
	GtkWidget *main_box;
	GtkWidget *jump;
	
	GtranslatorWindow *window;
};

static void
dialog_response_handler (GtkDialog *dlg, 
			 gint       res_id)
{
	GtranslatorJumpDialog *dialog = GTR_JUMP_DIALOG (dlg);
	GtranslatorTab *tab;
	gint number;

	switch (res_id)
	{
		case GTK_RESPONSE_OK:
			number = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (dialog->priv->jump));
			tab = gtranslator_window_get_active_tab (dialog->priv->window);
			gtranslator_tab_go_to_number (tab, number - 1);
			gtk_widget_destroy (GTK_WIDGET (dlg));
			break;
	
		default:
			gtk_widget_destroy (GTK_WIDGET (dlg));
	}
}

static void
gtranslator_jump_dialog_init (GtranslatorJumpDialog *dlg)
{
	gboolean ret;
	GtkWidget *error_widget;
	gchar *root_objects [] = {
		"adjustment1",
		"main_box",
		NULL
	};
	
	dlg->priv = GTR_JUMP_DIALOG_GET_PRIVATE (dlg);
	
	gtk_dialog_add_buttons (GTK_DIALOG (dlg),
				GTK_STOCK_OK,
				GTK_RESPONSE_OK,
				GTK_STOCK_CANCEL,
				GTK_RESPONSE_CANCEL,
				NULL);
	
	gtk_window_set_title (GTK_WINDOW (dlg), _("Go to Message"));
	gtk_window_set_default_size(GTK_WINDOW(dlg), 300, 100);
	gtk_window_set_resizable (GTK_WINDOW (dlg), TRUE);
	gtk_dialog_set_has_separator (GTK_DIALOG (dlg), FALSE);
	gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);
	
	/* HIG defaults */
	gtk_container_set_border_width (GTK_CONTAINER (dlg), 5);
	gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (dlg)->vbox), 2); /* 2 * 5 + 2 = 12 */
	gtk_container_set_border_width (GTK_CONTAINER (GTK_DIALOG (dlg)->action_area), 5);
	gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (dlg)->action_area), 4);
	
	gtk_dialog_set_default_response (GTK_DIALOG (dlg), GTK_RESPONSE_OK);
	
	g_signal_connect (dlg,
			  "response",
			  G_CALLBACK (dialog_response_handler),
			  NULL);
	
	/*Glade*/
	ret = gtranslator_utils_get_ui_objects (PKGDATADIR "/jump-dialog.ui",
		root_objects,
		&error_widget,
		
		"main_box", &dlg->priv->main_box,
		"jump", &dlg->priv->jump,
		
		NULL);
	
	if(!ret)
	{
		gtk_widget_show(error_widget);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dlg)->vbox),
				    error_widget,
				    TRUE,
				    TRUE,
				    0);
		
		return;
	}
	
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dlg)->vbox),
			    dlg->priv->main_box, TRUE, TRUE, 0);
	
	gtk_container_set_border_width (GTK_CONTAINER (dlg->priv->main_box), 5);
	
	
}

static void
gtranslator_jump_dialog_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_jump_dialog_parent_class)->finalize (object);
}

static void
gtranslator_jump_dialog_class_init (GtranslatorJumpDialogClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorJumpDialogPrivate));

	object_class->finalize = gtranslator_jump_dialog_finalize;
}

void
gtranslator_show_jump_dialog (GtranslatorWindow *window)
{
	static GtranslatorJumpDialog *dlg = NULL;
	
	g_return_if_fail (GTR_IS_WINDOW (window));
	
	if(dlg == NULL)
	{
		GtranslatorTab *tab;
		GtranslatorPo *po;
		gint messages;
		
		dlg = g_object_new (GTR_TYPE_JUMP_DIALOG, NULL);

		g_signal_connect (dlg,
				  "destroy",
				  G_CALLBACK (gtk_widget_destroyed),
				  &dlg);
		
		dlg->priv->window = window;
		
		/* Set the maximum number of the spin button */
		tab = gtranslator_window_get_active_tab (window);
		po = gtranslator_tab_get_po (tab);
		messages = gtranslator_po_get_messages_count (po);
		gtk_spin_button_set_range (GTK_SPIN_BUTTON (dlg->priv->jump),
					   1.0,
					   (gdouble)messages);

		gtk_widget_show (GTK_WIDGET(dlg));
	}
	
	if (GTK_WINDOW (window) != gtk_window_get_transient_for (GTK_WINDOW (dlg)))
	{
		gtk_window_set_transient_for (GTK_WINDOW (dlg),
					      GTK_WINDOW (window));
	}

	gtk_window_present (GTK_WINDOW (dlg));
}
