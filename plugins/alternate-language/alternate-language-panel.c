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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "alternate-language-panel.h"
#include "file-dialogs.h"
#include "msg.h"
#include "plugin.h"
#include "po.h"
#include "tab.h"
#include "view.h"

#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define GTR_ALTERNATE_LANG_PANEL_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),		       \
						 GTR_TYPE_ALTERNATE_LANG_PANEL,     \
						 GtranslatorAlternateLangPanelPrivate))

GTR_PLUGIN_DEFINE_TYPE(GtranslatorAlternateLangPanel, gtranslator_alternate_lang_panel, GTK_TYPE_VBOX)


struct _GtranslatorAlternateLangPanelPrivate
{
	GtkWidget *open_button;
	GtkWidget *close_button;
	GtkWidget *textview;
	
	GtkWidget *translated;
	GtkWidget *untranslated;
	GtkWidget *fuzzy;
	
	GtranslatorPo *po;
	GtranslatorMsg *first;
};

static void
gtranslator_alternate_lang_panel_set_text (GtranslatorAlternateLangPanel *panel,
					   const gchar *text)
{
	GtkTextBuffer *buf;
	
	buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (panel->priv->textview));
	
	gtk_text_buffer_set_text (buf, text, -1);
}

static void
search_message (GtranslatorAlternateLangPanel *panel,
		GtranslatorMsg *msg)
{
	GList *messages;
	GList *l;
	const gchar *msgid = gtranslator_msg_get_msgid (msg);
	const gchar *string;
	GtranslatorMsgStatus status;
	
	messages = gtranslator_po_get_messages (panel->priv->po);
	l = messages;
	do
	{
		string = gtranslator_msg_get_msgid (l->data);
		if (g_utf8_collate(string, msgid) == 0)
		{
			gtranslator_alternate_lang_panel_set_text (panel,
								   gtranslator_msg_get_msgstr (l->data));
			status = gtranslator_msg_get_status (GTR_MSG (l->data));
			switch (status)
			{
				case GTR_MSG_STATUS_TRANSLATED: 
					gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (panel->priv->translated),
								      TRUE);
					break;
				case GTR_MSG_STATUS_FUZZY:
					gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (panel->priv->fuzzy),
								      TRUE);
					break;
				default: break;
			}
			
			return;
		}
	} while ((l = g_list_next (l)));
	
	gtranslator_alternate_lang_panel_set_text (panel,
						   _("Message not found"));
	
	/*
	 * If we are here the status is untranslated
	 */
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (panel->priv->untranslated),
				      TRUE);
}

static void
showed_message_cb (GtranslatorTab *tab,
		   GtranslatorMsg *msg,
		   GtranslatorAlternateLangPanel *panel)
{
	if (panel->priv->po == NULL)
	{
		panel->priv->first = msg;
		return;
	}
	search_message (panel, msg);
}

static void
open_file (GtkWidget *dialog,
	   GtranslatorAlternateLangPanel *panel)
{
	GError *error = NULL;
	gchar *po_file = g_strdup (gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog)));
		  
		  
	panel->priv->po = gtranslator_po_new ();
	gtranslator_po_parse (panel->priv->po, po_file, &error);
	
	if (error != NULL)
	{
		GtkWidget *erdialog;
		/*
		 * FIXME: We have to get the window
		 */
		erdialog = gtk_message_dialog_new (NULL,
						   GTK_DIALOG_DESTROY_WITH_PARENT,
						   GTK_MESSAGE_ERROR,
						   GTK_BUTTONS_CLOSE,
						   error->message);
		gtk_dialog_run (GTK_DIALOG (erdialog));
		gtk_widget_destroy (erdialog);
		g_error_free (error);
	}
	
	search_message (panel, panel->priv->first);
	gtk_widget_set_sensitive (panel->priv->textview, TRUE);
		  
	g_free (po_file);
	gtk_widget_destroy (dialog);
}


static void
gtranslator_file_chooser_analyse (gpointer dialog,
				  GtranslatorAlternateLangPanel *panel)
{	
	gint reply;

	reply = gtk_dialog_run (GTK_DIALOG (dialog));
	switch (reply){
		case GTK_RESPONSE_ACCEPT:
			open_file (GTK_WIDGET (dialog),
				  panel);
			break;
		case GTK_RESPONSE_CANCEL:
			gtk_widget_hide (GTK_WIDGET (dialog));
			break;
		case GTK_RESPONSE_DELETE_EVENT:
			gtk_widget_hide (GTK_WIDGET (dialog));
			break;
		default:
			break;
	}
}

static void
open_button_clicked_cb (GtkWidget *open_button,
			GtranslatorAlternateLangPanel *panel)
{
	GtkWidget *dialog = NULL;
			       
	if(dialog != NULL) {
		gtk_window_present(GTK_WINDOW(dialog));
		return;
	}

	/*
	 * I need a way here to get the window
	 */
	dialog = gtranslator_file_chooser_new (NULL, 
					       FILESEL_OPEN,
					       _("Open file for alternate language"),
					       NULL);
			       
	gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(dialog), TRUE);
	
	gtranslator_file_chooser_analyse((gpointer) dialog, panel);			       
}

static void
close_button_clicked_cb (GtkWidget *close_button,
			 GtranslatorAlternateLangPanel *panel)
{
	if(panel->priv->po != NULL)
	{
		gtranslator_alternate_lang_panel_set_text (panel, _("File closed"));
	
		gtk_widget_set_sensitive (panel->priv->textview, FALSE);
	
		g_object_unref (panel->priv->po);
		
		panel->priv->po = NULL;
	}
}

static void
gtranslator_alternate_lang_panel_draw (GtranslatorAlternateLangPanel *panel)
{
	GtkWidget *hbox;
	GtkWidget *buttonbox;
	GtkWidget *scroll;
	
	/*
	 * Hbox
	 */
	hbox = gtk_hbox_new (FALSE, 6);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (panel), hbox, FALSE, TRUE, 0);
	
	/*
	 * Button box
	 */
	buttonbox = gtk_hbutton_box_new ();
	gtk_button_box_set_layout (GTK_BUTTON_BOX (buttonbox), GTK_BUTTONBOX_START);
	gtk_widget_show (buttonbox);
	
	panel->priv->open_button = gtk_button_new_from_stock (GTK_STOCK_OPEN);
	g_signal_connect (panel->priv->open_button,
			  "clicked",
			  G_CALLBACK (open_button_clicked_cb),
			  panel);
	gtk_widget_show (panel->priv->open_button);
	
	panel->priv->close_button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
	g_signal_connect (panel->priv->close_button,
			  "clicked",
			  G_CALLBACK (close_button_clicked_cb),
			  panel);
	gtk_widget_show (panel->priv->close_button);
	
	gtk_box_pack_start (GTK_BOX (buttonbox),
			    panel->priv->open_button,
			    TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (buttonbox),
			    panel->priv->close_button,
			    TRUE, TRUE, 0);
	
	gtk_box_pack_start (GTK_BOX (hbox), buttonbox, FALSE, TRUE, 0);
	
	/*
	 * Radio buttons
	 */
	panel->priv->translated = gtk_radio_button_new_with_label (NULL,
								   _("Translated"));
	gtk_widget_show (panel->priv->translated);
	gtk_box_pack_start (GTK_BOX (hbox), panel->priv->translated, FALSE, TRUE, 0);
	
	panel->priv->fuzzy = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (panel->priv->translated),
									  _("Fuzzy"));
	gtk_widget_show (panel->priv->fuzzy);
	gtk_box_pack_start (GTK_BOX (hbox), panel->priv->fuzzy, FALSE, TRUE, 0);
	
	panel->priv->untranslated = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (panel->priv->translated),
										 _("Untranslated"));
	gtk_widget_show (panel->priv->untranslated);
	gtk_box_pack_start (GTK_BOX (hbox), panel->priv->untranslated, FALSE, TRUE, 0);
	
	/*
	 * Text view
	 */
	scroll = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scroll);
	
	panel->priv->textview = gtranslator_view_new ();
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (panel->priv->textview),
				     GTK_WRAP_WORD);
	gtk_text_view_set_editable (GTK_TEXT_VIEW (panel->priv->textview),
				    FALSE);
	gtranslator_alternate_lang_panel_set_text (panel, _("There isn't any file loaded"));
	gtk_widget_set_sensitive (panel->priv->textview, FALSE);
	gtk_widget_show (panel->priv->textview);
	
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scroll),
					       panel->priv->textview);
	
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_AUTOMATIC);
	
	gtk_box_pack_start (GTK_BOX (panel), scroll,
			    TRUE, TRUE, 0);
}

static void
gtranslator_alternate_lang_panel_init (GtranslatorAlternateLangPanel *panel)
{
	panel->priv = GTR_ALTERNATE_LANG_PANEL_GET_PRIVATE (panel);
	
	gtranslator_alternate_lang_panel_draw (panel);
	
	panel->priv->po = NULL;
}

static void
gtranslator_alternate_lang_panel_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_alternate_lang_panel_parent_class)->finalize (object);
}

static void
gtranslator_alternate_lang_panel_class_init (GtranslatorAlternateLangPanelClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorAlternateLangPanelPrivate));

	object_class->finalize = gtranslator_alternate_lang_panel_finalize;
}

/***************************** Public funcs ***********************************/

GtkWidget *
gtranslator_alternate_lang_panel_new (GtkWidget *tab)
{
	GtranslatorAlternateLangPanel *panel;
	panel = g_object_new (GTR_TYPE_ALTERNATE_LANG_PANEL, NULL);
	
	g_signal_connect (tab, "showed-message",
			  G_CALLBACK (showed_message_cb),
			  panel);
	
	return GTK_WIDGET (panel);
}
