/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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

#include "po.h"
#include "comment-dialog.h"
#include "tab.h"
#include "utils.h"
#include "window.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>


#define GTR_COMMENT_DIALOG_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 	(object),	\
						 	GTR_TYPE_COMMENT_DIALOG,     \
						 	GtranslatorCommentDialogPrivate))


G_DEFINE_TYPE(GtranslatorCommentDialog, gtranslator_comment_dialog, GTK_TYPE_DIALOG)

struct _GtranslatorCommentDialogPrivate
{
	GtkWidget *main_box;
	GtkWidget *comment;
};
		    
static void
comment_changed_cb (GtkTextBuffer *buffer,
		    GtranslatorWindow *window)
{
	gchar *text;
	GtkTextIter start, end;
	GtranslatorTab *tab;
	GtranslatorPo *po;
	GList *msg;
	
	tab = gtranslator_window_get_active_tab(window);
	po = gtranslator_tab_get_po(tab);
	msg = gtranslator_po_get_current_message(po);
	
	gtk_text_buffer_get_bounds(buffer, &start, &end);
	text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
	
	gtranslator_msg_set_comment(msg->data, text);
	gtranslator_po_set_state (po, GTR_PO_STATE_MODIFIED);
}

static void
dialog_response_handler (GtkDialog *dlg, 
			 gint       res_id)
{
	switch (res_id)
	{
		default:
			gtk_widget_destroy (GTK_WIDGET(dlg));
	}
}

static void
gtranslator_comment_dialog_init (GtranslatorCommentDialog *dlg)
{
	gboolean ret;
	GtkWidget *error_widget;
	
	dlg->priv = GTR_COMMENT_DIALOG_GET_PRIVATE (dlg);
	
	gtk_dialog_add_buttons (GTK_DIALOG (dlg),
				GTK_STOCK_CLOSE,
				GTK_RESPONSE_CLOSE,
				NULL);
	
	gtk_window_set_title (GTK_WINDOW (dlg), _("Edit Comment"));
	gtk_window_set_default_size(GTK_WINDOW(dlg), 300, 200);
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
	ret = gtranslator_utils_get_glade_widgets(PKGDATADIR "/comment-dialog.glade",
		"main_box",
		&error_widget,
		
		"main_box", &dlg->priv->main_box,
		"comment", &dlg->priv->comment,
		
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
gtranslator_comment_dialog_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_comment_dialog_parent_class)->finalize (object);
}

static void
gtranslator_comment_dialog_class_init (GtranslatorCommentDialogClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorCommentDialogPrivate));

	object_class->finalize = gtranslator_comment_dialog_finalize;
}

void
gtranslator_show_comment_dialog (GtranslatorWindow *window)
{
	static GtranslatorCommentDialog *dlg = NULL;
	static GtkTextBuffer *buf;
	GtranslatorTab *tab = gtranslator_window_get_active_tab(window);
	GtranslatorPo *po = gtranslator_tab_get_po(tab);
	GList *msg = gtranslator_po_get_current_message(po);
	
	g_return_if_fail(GTR_IS_WINDOW(window));
	
	if(dlg == NULL)
	{
		dlg = g_object_new (GTR_TYPE_COMMENT_DIALOG, NULL);
		buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(dlg->priv->comment));
		g_signal_connect (dlg,
				  "destroy",
				  G_CALLBACK (gtk_widget_destroyed),
				  &dlg);
		g_signal_connect (buf,
				  "changed",
				  G_CALLBACK (comment_changed_cb),
				  window);

		gtk_widget_show (GTK_WIDGET(dlg));
	}
	
	gtk_text_buffer_set_text(buf, gtranslator_msg_get_comment(msg->data), -1); 
	
	if (GTK_WINDOW (window) != gtk_window_get_transient_for (GTK_WINDOW (dlg)))
	{
		gtk_window_set_transient_for (GTK_WINDOW (dlg),
					      GTK_WINDOW (window));
	}

	gtk_window_present (GTK_WINDOW (dlg));
}
