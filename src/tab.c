/*
 * Copyright (C) 2007   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * 			Fatih Demir <kabalak@kabalak.net>
 * 			Ross Golder <ross@golder.org>
 * 			Gediminas Paulauskas <menesis@kabalak.net>
 * 			Thomas Ziehmer <thomas@kabalak.net>
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

#include "application.h"
#include "context.h"
#include "io-error-message-area.h"
#include "message-area.h"
#include "message-table.h"
#include "msg.h"
#include "tab.h"
#include "po.h"
#include "prefs-manager.h"
#include "view.h"
#include "translation-memory.h"
#include "translation-memory-ui.h"
#include "window.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#define GTR_TAB_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_TAB,     \
					 GtranslatorTabPrivate))

#define MAX_PLURALS 6

#define GTR_TAB_KEY "GtranslatorTabFromDocument"

G_DEFINE_TYPE(GtranslatorTab, gtranslator_tab, GTK_TYPE_VBOX)

struct _GtranslatorTabPrivate
{
	GtranslatorPo *po;
	
	GtkWidget *table_pane;
	GtkWidget *content_pane;
	GtkWidget *panel;
	GtkWidget *message_table;
	GtkWidget *lateral_panel; //TM, Context, etc.

	GtkWidget *comment_pane;
	GtkWidget *context;
        GtkWidget *translation_memory;
	
	/*Comment button*/
	GtkWidget *comment_button;
	
	/*Message area*/
	GtkWidget *message_area;
	
	/*Original text*/
        GtkWidget *msgid_hbox;
	GtkWidget *text_vbox;
	GtkWidget *text_msgid;
	GtkWidget *text_plural_scroll;
	GtkWidget *text_msgid_plural;
	
	/*Translated text*/
        GtkWidget *msgstr_hbox;
	GtkWidget *trans_notebook;
	GtkWidget *trans_msgstr[MAX_PLURALS];
	
	/*Status widgets*/
	GtkWidget *translated;
	GtkWidget *fuzzy;
	GtkWidget *untranslated;
	
	/* Autosave */
	GTimer *timer;
	gint autosave_interval;
	guint autosave_timeout;
	gint autosave : 1;
	
	/*Blocking movement*/
	gboolean blocking;
};

enum
{
	SHOWED_MESSAGE,
	MESSAGE_CHANGED,
	MESSAGE_EDITION_FINISHED,
	LAST_SIGNAL
};

enum
{
	PROP_0,
	PROP_AUTOSAVE,
	PROP_AUTOSAVE_INTERVAL
};

static guint signals[LAST_SIGNAL];

static gboolean gtranslator_tab_autosave (GtranslatorTab *tab);

static void
install_autosave_timeout (GtranslatorTab *tab)
{
	gint timeout;

	g_return_if_fail (tab->priv->autosave_timeout <= 0);
	g_return_if_fail (tab->priv->autosave);
	g_return_if_fail (tab->priv->autosave_interval > 0);
	
	/* Add a new timeout */
	timeout = g_timeout_add (tab->priv->autosave_interval * 1000 * 60,
				 (GSourceFunc) gtranslator_tab_autosave,
				 tab);
	
	tab->priv->autosave_timeout = timeout;
}

static gboolean
install_autosave_timeout_if_needed (GtranslatorTab *tab)
{	
	g_return_val_if_fail (tab->priv->autosave_timeout <= 0, FALSE);

 	if (tab->priv->autosave)
 	{
 		install_autosave_timeout (tab);
 		
 		return TRUE;
 	}
 	
 	return FALSE;
}

static gboolean
gtranslator_tab_autosave (GtranslatorTab *tab)
{
	GError *error = NULL;
	
	if (!gtranslator_po_get_state (tab->priv->po) == GTR_PO_STATE_MODIFIED)
		return TRUE;
	
	gtranslator_po_save_file (tab->priv->po, &error);
	if (error)
	{
		g_warning (error->message);
		g_error_free (error);
	}
	
	return TRUE;
}

static void
remove_autosave_timeout (GtranslatorTab *tab)
{
	g_return_if_fail (tab->priv->autosave_timeout > 0);
	
	g_source_remove (tab->priv->autosave_timeout);
	tab->priv->autosave_timeout = 0;
}

static void
gtranslator_tab_showed_message (GtranslatorTab *tab,
				GtranslatorMsg *msg)
{
	if (strcmp (gtranslator_msg_get_comment (msg), "") != 0)
		gtk_widget_show (tab->priv->comment_button);
	else gtk_widget_hide (tab->priv->comment_button);
}

static void
gtranslator_tab_edition_finished (GtranslatorTab *tab,
				  GtranslatorMsg *msg)
{
	GtranslatorTranslationMemory *tm;
	gchar *message_error;
	GtkWidget *message_area;
	
	tm = GTR_TRANSLATION_MEMORY (gtranslator_application_get_translation_memory (GTR_APP));
	
	if (gtranslator_msg_is_translated (msg) && !gtranslator_msg_is_fuzzy (msg))
		gtranslator_translation_memory_store (tm,
						      gtranslator_msg_get_msgid (msg),
						      gtranslator_msg_get_msgstr (msg));
	
	/*
	 * Checking message
	 */
	message_error = gtranslator_msg_check (msg);
	
	if (message_error != NULL)
	{
		gtranslator_tab_block_movement (tab);
		
		message_area = create_error_message_area (_("There is an error in the message:"),
							  message_error);
		gtranslator_tab_set_message_area (tab, message_area);
		g_free (message_error);
	}
	else
	{
		gtranslator_tab_unblock_movement (tab);
		gtranslator_tab_set_message_area (tab, NULL);
	}
}

/*
 * Write the change back to the gettext PO instance in memory and
 * mark the page dirty
 */
static void 
gtranslator_message_translation_update(GtkTextBuffer *textbuffer,
				       GtranslatorTab *tab)
{
	GtranslatorHeader *header;
	GtkTextIter start, end;
	GtkTextBuffer *buf;
	GList *msg_aux;
	GtranslatorMsg *msg;
	const gchar *check;
	gchar *translation;
	gint i;
		
	/* Work out which message this is associated with */
	
	header = gtranslator_po_get_header (tab->priv->po);
	
	msg_aux = gtranslator_po_get_current_message(tab->priv->po);
	msg = msg_aux->data;
	buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tab->priv->trans_msgstr[0]));
	
	if(gtranslator_msg_is_fuzzy(msg) && gtranslator_prefs_manager_get_unmark_fuzzy())
		gtranslator_msg_set_fuzzy(msg, FALSE);
	
	if(textbuffer == buf)
	{
		/* Get message as UTF-8 buffer */
		gtk_text_buffer_get_bounds(textbuffer, &start, &end);
		translation = gtk_text_buffer_get_text(textbuffer, &start, &end, TRUE);
		
		/* TODO: convert to file's own encoding if not UTF-8 */
		
		/* Write back to PO file in memory */
		if(!(check = gtranslator_msg_get_msgid_plural(msg))) {
			gtranslator_msg_set_msgstr(msg, translation);
		}
		else {
			gtranslator_msg_set_msgstr_plural(msg, 0, translation);
			//free(check);
		}
		g_free(translation);
		return;
	}
	i=1;
	while(i < gtranslator_header_get_nplurals (header))
	{
		/* Know when to break out of the loop */
		if(!tab->priv->trans_msgstr[i]) {
			break;
		}
		
		/* Have we reached the one we want yet? */
		buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tab->priv->trans_msgstr[i]));
		if(textbuffer != buf) {
			i++;
			continue;
		}
		
		/* Get message as UTF-8 buffer */
		gtk_text_buffer_get_bounds(textbuffer, &start, &end);
		translation = gtk_text_buffer_get_text(textbuffer, &start, &end, TRUE);
		
		/* TODO: convert to file's own encoding if not UTF-8 */
		
		/* Write back to PO file in memory */
		gtranslator_msg_set_msgstr_plural(msg, i, translation);
		return;
	}

	/* Shouldn't get here */
	g_return_if_reached();
}




static GtkWidget *
gtranslator_tab_append_msgstr_page (const gchar *tab_label,
				    GtkWidget *box,
				    gboolean spellcheck)
{
	GtkWidget *scroll;
	GtkWidget *label;
	GtkWidget *widget;
	
	label = gtk_label_new(tab_label);
	
	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show (scroll);
	
	widget = gtranslator_view_new();
	gtk_widget_show (widget);
	
	if(spellcheck && gtranslator_prefs_manager_get_spellcheck())
		gtranslator_view_enable_spellcheck(GTR_VIEW(widget),
						    spellcheck);
	
	gtk_container_add (GTK_CONTAINER (scroll),
			   widget);
	
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_AUTOMATIC);
	
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroll),
					     GTK_SHADOW_IN);
	
	gtk_notebook_append_page (GTK_NOTEBOOK (box), scroll, label);

	return widget;
}

static void
gtranslator_message_plural_forms (GtranslatorTab *tab,
				  GtranslatorMsg *msg)
{
	GtranslatorHeader *header;
	GtkTextBuffer *buf;
	const gchar *msgstr_plural;
	gint i;

	g_return_if_fail (tab != NULL);
	g_return_if_fail (msg != NULL);
	
	header = gtranslator_po_get_header (tab->priv->po);
	
	for (i = 0; i < gtranslator_header_get_nplurals (header); i++)
	{
		msgstr_plural = gtranslator_msg_get_msgstr_plural (msg, i);
		if (msgstr_plural)
		{
			buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tab->priv->trans_msgstr[i]));
			gtk_source_buffer_begin_not_undoable_action (GTK_SOURCE_BUFFER (buf));
			gtk_text_buffer_set_text (buf, (gchar*)msgstr_plural, -1);
			gtk_source_buffer_end_not_undoable_action (GTK_SOURCE_BUFFER (buf));
		}
	}
}

static GtkWidget *
gtranslator_tab_create_comment_button ()
{
	GtkWidget *button;
	GtkWidget *image;
	
	/* setup close button */
	button = gtk_button_new ();
	gtk_button_set_relief (GTK_BUTTON (button),
			       GTK_RELIEF_NONE);
	/* don't allow focus on the close button */
	gtk_button_set_focus_on_click (GTK_BUTTON (button), FALSE);

	image = gtk_image_new_from_stock (GTK_STOCK_INDEX,
					  GTK_ICON_SIZE_MENU);
	gtk_widget_show (image);
	gtk_container_add (GTK_CONTAINER (button), image);

	gtk_widget_set_tooltip_text (button, _("Open comment dialog"));
	
	return button;
}

static void
on_comment_button_clicked (GtkButton *button,
			   gpointer useless)
{
	GtranslatorWindow *window = gtranslator_application_get_active_window (GTR_APP);
	
	gtranslator_show_comment_dialog (window);
}

/*
 * gtranslator_tab_show_message:
 * @tab: a #GtranslationTab
 * @msg: a #GtranslatorMsg
 * 
 * Shows the @msg in the @tab TextViews
 *
 */
static void
gtranslator_tab_show_message(GtranslatorTab *tab,
			     GtranslatorMsg *msg)
{
	GtranslatorTabPrivate *priv = tab->priv;
	GtranslatorPo *po;
	GtkTextBuffer *buf;
	const gchar *msgid, *msgid_plural;
	const gchar *msgstr, *msgstr_plural;
	
	g_return_if_fail(GTR_IS_TAB(tab));
	
	po = priv->po;
	gtranslator_po_update_current_message(po, msg);
	msgid = gtranslator_msg_get_msgid(msg);
	if(msgid) 
	{
		buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->text_msgid));
		gtk_source_buffer_begin_not_undoable_action(GTK_SOURCE_BUFFER(buf));
		gtk_text_buffer_set_text(buf, (gchar*)msgid, -1);
		gtk_source_buffer_end_not_undoable_action(GTK_SOURCE_BUFFER(buf));
	}
	msgid_plural = gtranslator_msg_get_msgid_plural(msg);
	if(!msgid_plural) 
	{
		msgstr = gtranslator_msg_get_msgstr(msg);
		/*
		 * Disable notebook tabs and hide widgets
		 */
		gtk_widget_hide (priv->text_plural_scroll);
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(priv->trans_notebook), FALSE);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->trans_notebook), 0);
		if(msgstr) 
		{
			buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->trans_msgstr[0]));
			gtk_source_buffer_begin_not_undoable_action(GTK_SOURCE_BUFFER(buf));
			gtk_text_buffer_set_text(buf, (gchar*)msgstr, -1);
			gtk_source_buffer_end_not_undoable_action(GTK_SOURCE_BUFFER(buf));
		}
	}
	else {
		gtk_widget_show (priv->text_plural_scroll);
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(tab->priv->trans_notebook), TRUE);
		buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tab->priv->text_msgid_plural));
		gtk_text_buffer_set_text(buf, (gchar*)msgid_plural, -1);
		gtranslator_message_plural_forms(tab, msg);
	}
}

static void
emit_message_changed_signal(GtkTextBuffer *buf,
			    GtranslatorTab *tab)
{
	GList *msg; 
	
	msg = gtranslator_po_get_current_message(tab->priv->po);
	
	g_signal_emit(G_OBJECT(tab), signals[MESSAGE_CHANGED], 0, msg->data); 
}

static void
update_status(GtranslatorTab *tab,
	      GtranslatorMsg *msg,
	      gpointer useless)
{
	GtranslatorMsgStatus status;
	GtranslatorPoState po_state;
	
	status = gtranslator_msg_get_status (msg);
	po_state = gtranslator_po_get_state (tab->priv->po);

	if((status == GTR_MSG_STATUS_FUZZY) && !gtranslator_msg_is_fuzzy(msg))
	{
		_gtranslator_po_increase_decrease_fuzzy(tab->priv->po, FALSE);
		if(gtranslator_msg_is_translated(msg))
		{
			status = GTR_MSG_STATUS_TRANSLATED;
			_gtranslator_po_increase_decrease_translated(tab->priv->po, TRUE);
		}
		else {
			status = GTR_MSG_STATUS_UNTRANSLATED;
		}
	}
	else if((status == GTR_MSG_STATUS_TRANSLATED) && !gtranslator_msg_is_translated(msg))
	{
		status = GTR_MSG_STATUS_UNTRANSLATED;
		_gtranslator_po_increase_decrease_translated(tab->priv->po, FALSE);
	}
	else if((status == GTR_MSG_STATUS_TRANSLATED) && gtranslator_msg_is_fuzzy(msg))
	{
		status = GTR_MSG_STATUS_FUZZY;
		_gtranslator_po_increase_decrease_translated(tab->priv->po, FALSE);
		_gtranslator_po_increase_decrease_fuzzy(tab->priv->po, TRUE);
	}
	else if((status == GTR_MSG_STATUS_UNTRANSLATED) && gtranslator_msg_is_translated(msg))
	{
		if(gtranslator_msg_is_fuzzy(msg))
		{
			status = GTR_MSG_STATUS_FUZZY;
			_gtranslator_po_increase_decrease_fuzzy(tab->priv->po, TRUE);
		}
		else {
			status = GTR_MSG_STATUS_TRANSLATED;
			_gtranslator_po_increase_decrease_translated(tab->priv->po, TRUE);
		}
	}

	gtranslator_msg_set_status (msg, status);
	
	/* We need to update the tab state too if is neccessary*/
	if (po_state != GTR_PO_STATE_MODIFIED)
		gtranslator_po_set_state(tab->priv->po, GTR_PO_STATE_MODIFIED);
}

static void
comment_pane_position_changed (GObject		*tab_gobject,
			       GParamSpec	*arg1,
			       GtranslatorTab	*tab)
{
	gtranslator_prefs_manager_set_comment_pane_pos(gtk_paned_get_position(GTK_PANED(tab_gobject)));
}

static void
content_pane_position_changed (GObject		*tab_gobject,
			       GParamSpec	*arg1,
			       GtranslatorTab	*tab)
{
	gtranslator_prefs_manager_set_content_pane_pos(gtk_paned_get_position(GTK_PANED(tab_gobject)));
}

static void
gtranslator_tab_add_msgstr_tabs (GtranslatorTab *tab)
{
	GtranslatorHeader *header;
	GtranslatorTabPrivate *priv = tab->priv;
	gchar *label;
	GtkTextBuffer *buf;
	gint i = 0;
	
	/*
	 * We get the header of the po file
	 */
	header = gtranslator_po_get_header (tab->priv->po);
	
	do{
		label = g_strdup_printf (_("Plural %d"), i+1);
		priv->trans_msgstr[i] = gtranslator_tab_append_msgstr_page (label,
									    priv->trans_notebook,
									    TRUE);
		buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (priv->trans_msgstr[i]));
		g_signal_connect (buf, "end-user-action",
				  G_CALLBACK (gtranslator_message_translation_update),
				  tab);
		
		g_signal_connect_after (buf, "end_user_action",
					G_CALLBACK (emit_message_changed_signal),
					tab);
		i++;
		g_free (label);
	}while (i < gtranslator_header_get_nplurals (header));
}

static void
gtranslator_tab_draw (GtranslatorTab *tab)
{
        gint current_page_num;
	GtkWidget *image;
	GtkWidget *vertical_box;
	GtkWidget *label_widget;
	GtkWidget *msgid_label;
	GtkWidget *msgstr_label;
	GtkWidget *current_page;
	GtkWidget *notebook, *tm_layout, *tm, *comments_label, *tm_label, *scroll;
	GtkWidget *hbox;
	GtkWidget *label;
	GtranslatorTabPrivate *priv = tab->priv;
	
	/*
	 * Panel
	 */
	priv->panel = gtk_notebook_new ();
	gtk_notebook_set_tab_pos (GTK_NOTEBOOK (priv->panel),
				  GTK_POS_BOTTOM);
	gtk_widget_show (priv->panel);

	/*
	 * Message table
	 */
	priv->message_table = gtranslator_message_table_new(GTK_WIDGET(tab));
	gtk_widget_show (priv->message_table);
	
	label_widget = gtk_label_new (_("Message Table"));
	
	gtk_notebook_append_page (GTK_NOTEBOOK (priv->panel),
				  priv->message_table,
				  label_widget);

	gtk_notebook_set_show_tabs(GTK_NOTEBOOK (priv->panel),
				   FALSE);
	
	/*
	 * Comment pane
	 */
	priv->comment_pane = gtk_hpaned_new();
	gtk_paned_set_position(GTK_PANED(priv->comment_pane), gtranslator_prefs_manager_get_comment_pane_pos());
	g_signal_connect (priv->comment_pane,
			  "notify::position",
			  G_CALLBACK (comment_pane_position_changed),
			  tab);
	gtk_widget_show (priv->comment_pane);
	
	/*
	 * Lateral panel
	 */
	tab->priv->lateral_panel = gtk_notebook_new ();
	gtk_widget_show (tab->priv->lateral_panel);

	gtk_paned_pack2(GTK_PANED(priv->comment_pane), tab->priv->lateral_panel,
			TRUE, TRUE);
	
	/*
	 * Context
	 */
	priv->context = gtranslator_context_panel_new (GTK_WIDGET (tab));
	gtk_widget_show (priv->context);
	gtranslator_tab_add_widget_to_lateral_panel (tab, priv->context,
						     _("Context"));

	/*
	 * TM
	 */
	priv->translation_memory = gtranslator_translation_memory_ui_new (GTK_WIDGET (tab));
	gtk_widget_show (priv->translation_memory);
	gtranslator_tab_add_widget_to_lateral_panel (tab, priv->translation_memory,
						     _("Translation Memory"));
	
	/*
	 * Content pane; this is where the message table and message area go
	 */
	priv->content_pane = gtk_vpaned_new();
	gtk_paned_set_position(GTK_PANED(priv->content_pane),
			       gtranslator_prefs_manager_get_content_pane_pos());
	g_signal_connect (priv->content_pane,
			  "notify::position",
			  G_CALLBACK (content_pane_position_changed),
			  tab);
	gtk_widget_show (priv->content_pane);

	/*
	 * Pack the comments pane and the main content
	 */
	vertical_box=gtk_vbox_new(FALSE, 0);
	gtk_paned_pack1(GTK_PANED(priv->content_pane), GTK_WIDGET(priv->panel), TRUE, FALSE);
	gtk_paned_pack2(GTK_PANED(priv->content_pane), priv->comment_pane, FALSE, TRUE);
	gtk_widget_show (vertical_box);
	
	/*
	 * Orignal text widgets
	 */
	priv->msgid_hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (priv->msgid_hbox);
	
	msgid_label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (msgid_label),
			      "<b>Original Text:</b>");
	gtk_misc_set_padding (GTK_MISC (msgid_label), 0, 5);
	gtk_widget_show (msgid_label);

	gtk_box_pack_start(GTK_BOX(priv->msgid_hbox), msgid_label, FALSE, FALSE, 0);

	priv->text_vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (priv->text_vbox);
	
	/* Singular */
	scroll = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scroll);
	
	priv->text_msgid = gtranslator_view_new ();
	gtk_text_view_set_editable (GTK_TEXT_VIEW (priv->text_msgid), FALSE);
	gtk_widget_show (priv->text_msgid);
	
	gtk_container_add (GTK_CONTAINER (scroll),
			   priv->text_msgid);
	
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
				        GTK_POLICY_AUTOMATIC,
				        GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroll),
					     GTK_SHADOW_IN);
	
	gtk_box_pack_start (GTK_BOX (priv->text_vbox), scroll, TRUE, TRUE, 0);
	
	/* Plural */
	priv->text_plural_scroll = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (priv->text_plural_scroll);
	
	priv->text_msgid_plural = gtranslator_view_new ();
	gtk_text_view_set_editable (GTK_TEXT_VIEW (priv->text_msgid_plural),
				    FALSE);
	gtk_widget_show (priv->text_msgid_plural);
	
	gtk_container_add (GTK_CONTAINER (priv->text_plural_scroll),
			   priv->text_msgid_plural);
	
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (priv->text_plural_scroll),
				        GTK_POLICY_AUTOMATIC,
				        GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (priv->text_plural_scroll),
					     GTK_SHADOW_IN);
	
	gtk_box_pack_start (GTK_BOX (priv->text_vbox), priv->text_plural_scroll,
			    TRUE, TRUE, 0);

	gtk_box_pack_start (GTK_BOX (vertical_box), priv->msgid_hbox, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vertical_box), priv->text_vbox, TRUE, TRUE, 0);

	
	/*
	 * Translation widgets
	 */
	priv->msgstr_hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (priv->msgstr_hbox);
	
	msgstr_label = gtk_label_new (NULL);
	gtk_label_set_markup_with_mnemonic (GTK_LABEL (msgstr_label),
					    "<b>Tran_slated Text:</b>");
	gtk_misc_set_padding (GTK_MISC (msgstr_label), 0, 5);
	gtk_misc_set_alignment (GTK_MISC (msgstr_label), 0, 0.5);
	gtk_widget_show (msgstr_label);

	gtk_box_pack_start(GTK_BOX(priv->msgstr_hbox), msgstr_label, TRUE, TRUE, 0);
	
	priv->comment_button = gtranslator_tab_create_comment_button ();
	gtk_box_pack_start (GTK_BOX (priv->msgstr_hbox), priv->comment_button,
			    FALSE, FALSE, 0);
	g_signal_connect (priv->comment_button, "clicked",
			  G_CALLBACK (on_comment_button_clicked), NULL);

	priv->trans_notebook = gtk_notebook_new();
	gtk_notebook_set_show_border(GTK_NOTEBOOK(priv->trans_notebook), FALSE);
	gtk_widget_show (priv->trans_notebook);

	gtk_box_pack_start(GTK_BOX(vertical_box), priv->msgstr_hbox, FALSE, FALSE, 0);	
	gtk_box_pack_start(GTK_BOX(vertical_box), priv->trans_notebook, TRUE, TRUE, 0);	

	gtk_paned_pack1(GTK_PANED(priv->comment_pane), vertical_box, FALSE, FALSE);
	
	gtk_box_pack_start(GTK_BOX(tab), priv->content_pane, TRUE, TRUE, 0);
}

static void
gtranslator_tab_init (GtranslatorTab *tab)
{
	tab->priv = GTR_TAB_GET_PRIVATE (tab);
	
	g_signal_connect(tab, "message-changed",
			 G_CALLBACK(update_status), NULL);
	
	gtranslator_tab_draw(tab);
	
	/* Manage auto save data */
	tab->priv->autosave = gtranslator_prefs_manager_get_autosave ();
	tab->priv->autosave = (tab->priv->autosave != FALSE);

	tab->priv->autosave_interval = gtranslator_prefs_manager_get_autosave_interval ();
	if (tab->priv->autosave_interval <= 0)
		tab->priv->autosave_interval = GPM_DEFAULT_AUTOSAVE_INTERVAL;
}

static void
gtranslator_tab_finalize (GObject *object)
{
	GtranslatorTab *tab = GTR_TAB (object);
	
	if (tab->priv->po)
		g_object_unref (tab->priv->po);
		
	if (tab->priv->timer != NULL)
		g_timer_destroy (tab->priv->timer);
	
	if (tab->priv->autosave_timeout > 0)
		remove_autosave_timeout (tab);
	
	G_OBJECT_CLASS (gtranslator_tab_parent_class)->finalize (object);
}

static void
gtranslator_tab_get_property (GObject    *object,
			      guint       prop_id,
			      GValue     *value,
			      GParamSpec *pspec)
{
	GtranslatorTab *tab = GTR_TAB (object);

	switch (prop_id)
	{
		case PROP_AUTOSAVE:
			g_value_set_boolean (value,
					     gtranslator_tab_get_autosave_enabled (tab));
			break;
		case PROP_AUTOSAVE_INTERVAL:
			g_value_set_int (value,
					 gtranslator_tab_get_autosave_interval (tab));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gtranslator_tab_set_property (GObject      *object,
			      guint         prop_id,
			      const GValue *value,
			      GParamSpec   *pspec)
{
	GtranslatorTab *tab = GTR_TAB (object);

	switch (prop_id)
	{
		case PROP_AUTOSAVE:
			gtranslator_tab_set_autosave_enabled (tab,
							      g_value_get_boolean (value));
			break;
		case PROP_AUTOSAVE_INTERVAL:
			gtranslator_tab_set_autosave_interval (tab,
							       g_value_get_int (value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;			
	}
}

static void
gtranslator_tab_class_init (GtranslatorTabClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorTabPrivate));

	object_class->finalize = gtranslator_tab_finalize;
	object_class->set_property = gtranslator_tab_set_property;
	object_class->get_property = gtranslator_tab_get_property;
	klass->showed_message = gtranslator_tab_showed_message;
	klass->message_edition_finished = gtranslator_tab_edition_finished;

	/* Signals */
	signals[SHOWED_MESSAGE] = 
		g_signal_new("showed-message",
			     G_OBJECT_CLASS_TYPE (klass),
			     G_SIGNAL_RUN_LAST,
			     G_STRUCT_OFFSET (GtranslatorTabClass, showed_message),
			     NULL, NULL,
			     g_cclosure_marshal_VOID__POINTER,
			     G_TYPE_NONE, 1,
			     G_TYPE_POINTER);
	
	signals[MESSAGE_CHANGED] =
		g_signal_new("message-changed",
			     G_OBJECT_CLASS_TYPE (klass),
			     G_SIGNAL_RUN_LAST,
			     G_STRUCT_OFFSET (GtranslatorTabClass, message_changed),
			     NULL, NULL,
			     g_cclosure_marshal_VOID__POINTER,
			     G_TYPE_NONE, 1,
			     G_TYPE_POINTER);
	
	signals[MESSAGE_EDITION_FINISHED] = 
		g_signal_new("message-edition-finished",
			     G_OBJECT_CLASS_TYPE (klass),
			     G_SIGNAL_RUN_LAST,
			     G_STRUCT_OFFSET (GtranslatorTabClass, message_edition_finished),
			     NULL, NULL,
			     g_cclosure_marshal_VOID__POINTER,
			     G_TYPE_NONE, 1,
			     G_TYPE_POINTER);
			     
	/* Properties */
	g_object_class_install_property (object_class,
					 PROP_AUTOSAVE,
					 g_param_spec_boolean ("autosave",
							       "Autosave",
							       "Autosave feature",
							       TRUE,
							       G_PARAM_READWRITE |
							       G_PARAM_STATIC_STRINGS));

	g_object_class_install_property (object_class,
					 PROP_AUTOSAVE_INTERVAL,
					 g_param_spec_int ("autosave-interval",
							   "AutosaveInterval",
							   "Time between two autosaves",
							   0,
							   G_MAXINT,
							   0,
							   G_PARAM_READWRITE |
							   G_PARAM_STATIC_STRINGS));
}

/***************************** Public funcs ***********************************/

/**
 * gtranslator_tab_new:
 * @po: a #GtranslatorPo
 * 
 * Creates a new #GtranslatorTab.
 * 
 * Return value: a new #GtranslatorTab object
 **/
GtranslatorTab *
gtranslator_tab_new (GtranslatorPo *po)
{
	GtranslatorTab *tab;
	
	g_return_val_if_fail(po != NULL, NULL);
	
	tab = g_object_new (GTR_TYPE_TAB, NULL);
	
	tab->priv->po = po;
	g_object_set_data (G_OBJECT (po), GTR_TAB_KEY, tab);
	install_autosave_timeout_if_needed (tab);
	
	/*
	 * Now we have to initialize the number of msgstr tabs
	 */
	gtranslator_tab_add_msgstr_tabs (tab);
	
	gtranslator_message_table_populate(GTR_MESSAGE_TABLE(tab->priv->message_table),
					   gtranslator_po_get_messages(tab->priv->po));
	
	gtk_widget_show (GTK_WIDGET(tab));
	return tab;
}

/**
 * gtranslator_tab_get_po:
 * @tab: a #GtranslatorTab
 *
 * Return value: the #GtranslatorPo stored in the #GtranslatorTab
**/
GtranslatorPo *
gtranslator_tab_get_po(GtranslatorTab *tab)
{
	return tab->priv->po;
}

/**
 * gtranslator_tab_get_panel:
 * @tab: a #GtranslationTab
 * 
 * Return value: the horizontal notebook of the #GtranslationTab
**/
GtkWidget *
gtranslator_tab_get_panel(GtranslatorTab *tab)
{
	g_return_val_if_fail(tab != NULL, NULL);
	
	return tab->priv->panel;
}

/**
 * gtranslator_tab_get_active_trans_tab:
 * @tab: a #GtranslationTab
 * 
 * Return value: the number of the active translation notebook.
 **/
gint
gtranslator_tab_get_active_trans_tab(GtranslatorTab *tab)
{
	return gtk_notebook_get_current_page(GTK_NOTEBOOK(tab->priv->trans_notebook));
}

/**
 * gtranslator_tab_get_context_panel:
 * @tab: a #GtranslatorTab
 *
 * Return value: the #GtranslaorContextPanel
 */
GtranslatorContextPanel *
gtranslator_tab_get_context_panel(GtranslatorTab *tab)
{
	return GTR_CONTEXT_PANEL(tab->priv->context);
}

/**
 * gtranslator_tab_get_translation_memory_ui:
 * @tab: a #GtranslatorTab
 *
 * Returns: the #GtranslatorTranslationMemoryUi panel.
 */
GtkWidget *
gtranslator_tab_get_translation_memory_ui (GtranslatorTab *tab)
{
	g_return_val_if_fail (GTR_IS_TAB (tab), NULL);
	
	return tab->priv->translation_memory;
}

/**
 * gtranslator_tab_get_active_view:
 * @tab: a #GtranslationTab
 *
 * Return value: the active page of the translation notebook.
**/
GtranslatorView *
gtranslator_tab_get_active_view(GtranslatorTab *tab)
{
	gint num;
	
	num = gtk_notebook_get_current_page(GTK_NOTEBOOK(tab->priv->trans_notebook));
	return GTR_VIEW(tab->priv->trans_msgstr[num]);
}

/**
 * gtranslator_tab_get_all_views:
 * @tab: the #GtranslationTab
 * @original: TRUE if you want original TextViews.
 * @translated: TRUE if you want tranlated TextViews.
 *
 * Returns all the views currently present in #GtranslationTab
 *
 * Return value: a newly allocated list of #GtranslationTab objects
 */
GList *
gtranslator_tab_get_all_views(GtranslatorTab *tab,
			      gboolean original,
			      gboolean translated)
{
	GList *ret = NULL;
	gint i = 0;
	
	g_return_if_fail(GTR_IS_TAB(tab));
	
	if(original)
	{
		ret = g_list_append(ret, tab->priv->text_msgid);
		ret = g_list_append(ret, tab->priv->text_msgid_plural);
	}
	
	if(translated)
	{
		while(i < MAX_PLURALS)
		{
			if(tab->priv->trans_msgstr[i])
				ret = g_list_append(ret, tab->priv->trans_msgstr[i]);
			else break;
			i++;
		}
	}
	
	return ret;
}

/**
 * gtranslator_tab_get_name:
 * @tab: a #GtranslatorTab 
 * 
 * Return value: a new allocated string with the name of the @tab.
 */
gchar *
gtranslator_tab_get_name (GtranslatorTab *tab)
{
	GtranslatorHeader *header;
	GtranslatorPoState state;
	gchar *str;
	gchar *tab_name;

	header = gtranslator_po_get_header (tab->priv->po);
	state = gtranslator_po_get_state (tab->priv->po);

	str = gtranslator_header_get_prj_id_version (header);

	if (state == GTR_PO_STATE_MODIFIED)
	{
		tab_name = g_strdup_printf ("*%s", str);
		return tab_name;
	}
	
	return g_strdup (str);
}

/**
 * gtranslator_tab_message_go_to:
 * @tab: a #GtranslatorTab
 * @to_go: the #GtranslatorMsg you want to jump
 * @searching: TRUE if we are searching in the message list
 *
 * Jumps to the specific @to_go pointer message and show the message
 * in the #GtranslatorView.
**/
void 
gtranslator_tab_message_go_to (GtranslatorTab *tab,
			       GList * to_go,
			       gboolean searching,
			       GtranslatorTabMove move)
{
	GtranslatorPo *po;
	GList *current_msg;
	static gboolean first_msg = TRUE;
 
	g_return_if_fail (tab != NULL);
	g_return_if_fail (to_go != NULL);
	g_return_if_fail (GTR_IS_MSG (to_go->data));
		
	po = tab->priv->po;
	
	current_msg = gtranslator_po_get_current_message (po);
	
	/*
	 * Emitting message-edition-finished signal
	 */
	if (!searching)
		g_signal_emit (G_OBJECT (tab), signals[MESSAGE_EDITION_FINISHED],
			       0, GTR_MSG (current_msg->data));
	
	if (!tab->priv->blocking || first_msg)
	{
		gboolean plurals;
		gint current_page, n_pages;
		/*
		 * If the current message is plural and we press next/prev
		 * we have to change to the next/prev plural tab in case is not
		 * the last
		 * To implement that:
		 * if the tabs are showed then we check if we want prev or 
		 * next and then if we need to change the tab we change it 
		 * in other case we show the message
		 * 
		 * I don't like too much this implementation so if anybody can
		 * rewrite this is a better way would be great.
		 */
		plurals = gtk_notebook_get_show_tabs (GTK_NOTEBOOK (tab->priv->trans_notebook));
		current_page = gtk_notebook_get_current_page (GTK_NOTEBOOK (tab->priv->trans_notebook));
		n_pages = gtk_notebook_get_n_pages (GTK_NOTEBOOK (tab->priv->trans_notebook));
		if ((plurals == TRUE) && (move != GTR_TAB_MOVE_NONE))
		{
			if ((n_pages - 1) == current_page && move == GTR_TAB_MOVE_NEXT)
			{
				gtk_notebook_set_current_page (GTK_NOTEBOOK (tab->priv->trans_notebook),
							       0);
				gtranslator_tab_show_message (tab, to_go->data);
			}
			else if (current_page == 0 && move == GTR_TAB_MOVE_PREV)
			{
				gtk_notebook_set_current_page (GTK_NOTEBOOK (tab->priv->trans_notebook),
							       n_pages - 1);
				gtranslator_tab_show_message (tab, to_go->data);
			}
			else
			{
				if (move == GTR_TAB_MOVE_NEXT)
					gtk_notebook_set_current_page (GTK_NOTEBOOK (tab->priv->trans_notebook),
								       current_page + 1);
				else
					gtk_notebook_set_current_page (GTK_NOTEBOOK (tab->priv->trans_notebook),
								       current_page - 1);
				return;
			}
		}
		else
			gtranslator_tab_show_message (tab, to_go->data);
		first_msg = FALSE;
	}
	else
		return;
	
	/*
	 * Emitting showed-message signal
	 */
	if (!searching)
		g_signal_emit (G_OBJECT (tab), signals[SHOWED_MESSAGE], 0,
			       GTR_MSG (to_go->data)); 
}

/**
 * _gtranslator_tab_can_close:
 * @tab: a #GtranslatorTab
 *
 * Whether a #GtranslatorTab can be closed.
 *
 * Returns: TRUE if the #GtranslatorPo of the @tab is already saved
 */
gboolean
_gtranslator_tab_can_close (GtranslatorTab *tab)
{
	return gtranslator_po_get_state (tab->priv->po) == GTR_PO_STATE_SAVED;
}

/**
 * gtranslator_tab_get_from_document:
 * @po: a #GtranslatorPo
 *
 * Returns the #GtranslatorTab for a specific #GtranslatorPo.
 *
 * Returns: the #GtranslatorTab for a specific #GtranslatorPo
 */
GtranslatorTab *
gtranslator_tab_get_from_document (GtranslatorPo *po)
{
	gpointer res;
	
	g_return_val_if_fail (GTR_IS_PO (po), NULL);
	
	res = g_object_get_data (G_OBJECT (po), GTR_TAB_KEY);
	
	return (res != NULL) ? GTR_TAB (res) : NULL;
}

/**
 * gtranslator_tab_get_autosave_enabled:
 * @tab: a #GtranslatorTab
 * 
 * Gets the current state for the autosave feature
 * 
 * Return value: TRUE if the autosave is enabled, else FALSE
 **/
gboolean
gtranslator_tab_get_autosave_enabled (GtranslatorTab *tab)
{
	g_return_val_if_fail (GTR_IS_TAB (tab), FALSE);

	return tab->priv->autosave;
}

/**
 * gtranslator_tab_set_autosave_enabled:
 * @tab: a #GtranslatorTab
 * @enable: enable (TRUE) or disable (FALSE) auto save
 * 
 * Enables or disables the autosave feature. It does not install an
 * autosave timeout if the document is new or is read-only
 **/
void
gtranslator_tab_set_autosave_enabled (GtranslatorTab *tab, 
				      gboolean enable)
{
	g_return_if_fail (GTR_IS_TAB (tab));

	if (tab->priv->autosave == enable)
		return;

	tab->priv->autosave = enable;

 	if (enable && 
 	    (tab->priv->autosave_timeout <= 0))
 	{
		install_autosave_timeout (tab);
		
		return;
	}
 		
 	if (!enable && (tab->priv->autosave_timeout > 0))
 	{
		remove_autosave_timeout (tab);
		
 		return; 
 	} 

 	g_return_if_fail (!enable && (tab->priv->autosave_timeout <= 0)); 
}

/**
 * gtranslator_tab_get_autosave_interval:
 * @tab: a #GtranslatorTab
 * 
 * Gets the current interval for the autosaves
 * 
 * Return value: the value of the autosave
 **/
gint 
gtranslator_tab_get_autosave_interval (GtranslatorTab *tab)
{
	g_return_val_if_fail (GTR_IS_TAB (tab), 0);

	return tab->priv->autosave_interval;
}

/**
 * gtranslator_tab_set_autosave_interval:
 * @tab: a #GtranslatorTab
 * @interval: the new interval
 * 
 * Sets the interval for the autosave feature. It does nothing if the
 * interval is the same as the one already present. It removes the old
 * interval timeout and adds a new one with the autosave passed as
 * argument.
 **/
void 
gtranslator_tab_set_autosave_interval (GtranslatorTab *tab, 
				       gint interval)
{
	g_return_if_fail (GTR_IS_TAB (tab));
	g_return_if_fail (interval > 0);

	if (tab->priv->autosave_interval == interval)
		return;

	tab->priv->autosave_interval = interval;
		
	if (!tab->priv->autosave)
		return;

	if (tab->priv->autosave_timeout > 0)
	{
		remove_autosave_timeout (tab);

		install_autosave_timeout (tab);
	}
}

/**
 * gtranslator_tab_add_widget_to_lateral_panel:
 * @tab: a #GtranslatorTab
 * @widget: a #GtkWidget
 * @tab_name: the tab name in the notebook
 *
 * Adds a new widget to the laberal panel notebook.
 */
void
gtranslator_tab_add_widget_to_lateral_panel (GtranslatorTab *tab,
					     GtkWidget *widget,
					     const gchar *tab_name)
{
	GtkWidget *label;
	
	g_return_if_fail (GTR_IS_TAB (tab));
	g_return_if_fail (GTK_IS_WIDGET (widget));
	
	label = gtk_label_new (tab_name);
	
	gtk_notebook_append_page (GTK_NOTEBOOK (tab->priv->lateral_panel),
				  widget, label);
}

/**
 * gtranslator_tab_remove_widget_from_lateral_panel:
 * @tab: a #GtranslatorTab
 * @widget: a #GtkWidget
 *
 * Removes the @widget from the lateral panel notebook of @tab.
 */
void
gtranslator_tab_remove_widget_from_lateral_panel (GtranslatorTab *tab,
						  GtkWidget *widget)
{
	gint page;
	
	g_return_if_fail (GTR_IS_TAB (tab));
	g_return_if_fail (GTK_IS_WIDGET (widget));
	
	page = gtk_notebook_page_num (GTK_NOTEBOOK (tab->priv->lateral_panel),
				      widget);
	
	gtk_notebook_remove_page (GTK_NOTEBOOK (tab->priv->lateral_panel),
				  page);
}

/**
 * gtranslator_tab_show_lateral_panel_widget:
 * @tab: a #GtranslatorTab
 * @widget: the widget to be shown.
 *
 * Shows the notebook page of the @widget.
 */
void
gtranslator_tab_show_lateral_panel_widget (GtranslatorTab *tab,
					   GtkWidget *widget)
{
	gint page;
	
	page = gtk_notebook_page_num (GTK_NOTEBOOK (tab->priv->lateral_panel),
				      widget);
	gtk_notebook_set_current_page (GTK_NOTEBOOK (tab->priv->lateral_panel),
				       page);
}

/**
 * gtranslator_tab_clear_msgstr_views:
 * @tab: a #GtranslatorTab
 * 
 * Clears all text from msgstr text views.
 */
void
gtranslator_tab_clear_msgstr_views (GtranslatorTab *tab)
{
	gint i = 0;
	GtranslatorHeader *header;
	GtkTextBuffer *buf;
	
	g_return_if_fail (GTR_IS_TAB (tab));
	
	header = gtranslator_po_get_header (tab->priv->po);
	
	do {
		buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tab->priv->trans_msgstr[i]));
		gtk_text_buffer_begin_user_action (buf);
		gtk_text_buffer_set_text (buf, "", -1);
		gtk_text_buffer_end_user_action (buf);
		i++;
	}while (i < gtranslator_header_get_nplurals (header));
}

/**
 * gtranslator_tab_block_movement:
 * @tab: a #GtranslatorTab
 *
 * Blocks the movement to the next/prev message.
 */
void
gtranslator_tab_block_movement (GtranslatorTab *tab)
{
	g_return_if_fail (GTR_IS_TAB (tab));
	
	tab->priv->blocking = TRUE;
}

/**
 * gtranslator_tab_unblock_movement:
 * @tab: a #GtranslatorTab
 * 
 * Unblocks the movement to the next/prev message.
 */
void
gtranslator_tab_unblock_movement (GtranslatorTab *tab)
{
	g_return_if_fail (GTR_IS_TAB (tab));
	
	tab->priv->blocking = FALSE;
}

/**
 * gtranslator_tab_go_to_next:
 * @tab: a #GtranslatorTab
 *
 * Moves to the next message or plural tab in case the message has plurals.
 */
void
gtranslator_tab_go_to_next (GtranslatorTab *tab)
{
	GtranslatorPo *po;
	
	po = gtranslator_tab_get_po (tab);
	
	gtranslator_tab_message_go_to (tab,
				       g_list_next (gtranslator_po_get_current_message (po)),
				       FALSE,
				       GTR_TAB_MOVE_NEXT);
}

/**
 * gtranslator_tab_go_to_prev:
 * @tab: a #GtranslatorTab
 *
 * Moves to the previous message or plural tab in case the message has plurals.
 */
void
gtranslator_tab_go_to_prev (GtranslatorTab *tab)
{
	GtranslatorPo *po;
	
	po = gtranslator_tab_get_po (tab);
	
	gtranslator_tab_message_go_to (tab,
				       g_list_previous (gtranslator_po_get_current_message (po)),
				       FALSE,
				       GTR_TAB_MOVE_PREV);
}

/**
 * gtranslator_tab_go_to_first:
 * @tab: a #GtranslatorTab
 *
 * Jumps to the first message.
 */
void
gtranslator_tab_go_to_first (GtranslatorTab *tab)
{
	GtranslatorPo *po;
	
	po = gtranslator_tab_get_po (tab);
	
	gtranslator_tab_message_go_to (tab,
				       g_list_first (gtranslator_po_get_current_message (po)),
				       FALSE,
				       GTR_TAB_MOVE_NONE);
}

/**
 * gtranslator_tab_go_to_last:
 * @tab: a #GtranslatorTab 
 *
 * Jumps to the last message.
 */
void
gtranslator_tab_go_to_last (GtranslatorTab *tab)
{
	GtranslatorPo *po;
	
	po = gtranslator_tab_get_po (tab);
	
	gtranslator_tab_message_go_to (tab,
				       g_list_last (gtranslator_po_get_current_message (po)),
				       FALSE,
				       GTR_TAB_MOVE_NONE);
}

/**
 * gtranslator_tab_go_to_next_fuzzy:
 * @tab: a #GtranslatorTab
 *
 * If there is a next fuzzy message it jumps to it.
 *
 * Returns: TRUE if there is a next fuzzy message.
 */
gboolean
gtranslator_tab_go_to_next_fuzzy (GtranslatorTab *tab)
{
	GtranslatorPo *po;
	GList *msg;
	
	po = gtranslator_tab_get_po (tab);
	
	msg = gtranslator_po_get_next_fuzzy (po);
	if(msg != NULL)
	{
		gtranslator_tab_message_go_to (tab, msg, FALSE,
					       GTR_TAB_MOVE_NONE);
		return TRUE;
	}
	
	return FALSE;
}

/**
 * gtranslator_tab_go_to_prev_fuzzy:
 * @tab: a #GtranslatorTab
 *
 * If there is a prev fuzzy message it jumps to it.
 *
 * Returns: TRUE if there is a prev fuzzy message.
 */
gboolean
gtranslator_tab_go_to_prev_fuzzy (GtranslatorTab *tab)
{
	GtranslatorPo *po;
	GList *msg;
	
	po = gtranslator_tab_get_po (tab);
	
	msg = gtranslator_po_get_prev_fuzzy (po);
	if(msg != NULL)
	{
		gtranslator_tab_message_go_to (tab, msg, FALSE,
					       GTR_TAB_MOVE_NONE);
		return TRUE;
	}
	
	return FALSE;
}

/**
 * gtranslator_tab_go_to_next_untrans:
 * @tab: a #GtranslatorTab
 *
 * If there is a next untranslated message it jumps to it.
 *
 * Returns: TRUE if there is a next untranslated message.
 */
gboolean
gtranslator_tab_go_to_next_untrans (GtranslatorTab *tab)
{
	GtranslatorPo *po;
	GList *msg;
	
	po = gtranslator_tab_get_po (tab);
	
	msg = gtranslator_po_get_next_untrans (po);
	if(msg != NULL)
	{
		gtranslator_tab_message_go_to (tab, msg, FALSE,
					       GTR_TAB_MOVE_NONE);
		return TRUE;
	}
	
	return FALSE;
}

/**
 * gtranslator_tab_go_to_prev_untrans:
 * @tab: a #GtranslatorTab
 *
 * If there is a prev untranslated message it jumps to it.
 *
 * Returns: TRUE if there is a prev untranslated message.
 */
gboolean
gtranslator_tab_go_to_prev_untrans (GtranslatorTab *tab)
{
	GtranslatorPo *po;
	GList *msg;
	
	po = gtranslator_tab_get_po (tab);
	
	msg = gtranslator_po_get_prev_untrans (po);
	if(msg != NULL)
	{
		gtranslator_tab_message_go_to (tab, msg, FALSE,
					       GTR_TAB_MOVE_NONE);
		return TRUE;
	}
	
	return FALSE;
}

/**
 * gtranslator_tab_go_to_next_fuzzy_or_untrans:
 * @tab: a #GtranslatorTab
 *
 * If there is a next fuzzy or untranslated message it jumps to it.
 *
 * Returns: TRUE if there is a next fuzzy or untranslated message.
 */
gboolean
gtranslator_tab_go_to_next_fuzzy_or_untrans (GtranslatorTab *tab)
{
	GtranslatorPo *po;
	GList *msg;
	
	po = gtranslator_tab_get_po (tab);
	
	msg = gtranslator_po_get_next_fuzzy_or_untrans (po);
	if(msg != NULL)
	{
		gtranslator_tab_message_go_to (tab, msg, FALSE,
					       GTR_TAB_MOVE_NONE);
		return TRUE;
	}
	
	return FALSE;
}

/**
 * gtranslator_tab_go_to_prev_fuzzy_or_untrans:
 * @tab: a #GtranslatorTab
 *
 * If there is a prev fuzzy or untranslated message it jumps to it.
 *
 * Returns: TRUE if there is a prev fuzzy or untranslated message.
 */
gboolean
gtranslator_tab_go_to_prev_fuzzy_or_untrans (GtranslatorTab *tab)
{
	GtranslatorPo *po;
	GList *msg;
	
	po = gtranslator_tab_get_po (tab);
	
	msg = gtranslator_po_get_prev_fuzzy_or_untrans (po);
	if(msg != NULL)
	{
		gtranslator_tab_message_go_to (tab, msg, FALSE,
					       GTR_TAB_MOVE_NONE);
		return TRUE;
	}
	
	return FALSE;
}

/**
 * gtranslator_tab_go_to_number:
 * @tab: a #GtranslatorTab
 * @number: the message number you want to jump
 *
 * Jumps to the message with the @number in the list, if the message does not
 * exists it does not jump.
 */
void
gtranslator_tab_go_to_number (GtranslatorTab *tab,
			      gint number)
{
	GtranslatorPo *po;
	GList *msg;
	
	po = gtranslator_tab_get_po (tab);
	
	msg = gtranslator_po_get_msg_from_number (po, number);
	if(msg != NULL)
	{
		gtranslator_tab_message_go_to (tab, msg, FALSE,
					       GTR_TAB_MOVE_NONE);
	}
}

/**
 * gtranslator_tab_set_message_area:
 * @tab: a #GtranslatorTab
 * @message_area: a #GtranslatorMessageArea
 *
 * Sets the @message_area to be shown in the @tab.
 */
void
gtranslator_tab_set_message_area (GtranslatorTab  *tab,
				  GtkWidget *message_area)
{
	g_return_if_fail (GTR_IS_TAB (tab));
	
        if (tab->priv->message_area == message_area)
                return;

        if (tab->priv->message_area != NULL)
                gtk_widget_destroy (tab->priv->message_area);

        tab->priv->message_area = message_area;

        if (message_area == NULL)
                return;

        gtk_box_pack_start (GTK_BOX (tab),
                            tab->priv->message_area,
                            FALSE,
                            FALSE,
                            0);

        g_object_add_weak_pointer (G_OBJECT (tab->priv->message_area), 
                                   (gpointer *)&tab->priv->message_area);
}