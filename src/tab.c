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

#include "comment.h"
#include "io-error-message-area.h"
#include "message-area.h"
#include "message-table.h"
#include "msg.h"
#include "tab.h"
#include "po.h"
#include "prefs-manager.h"
#include "view.h"

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

	GtkWidget *comment_pane;
	GtkWidget *comment;
        GtkWidget *translation_memory;
	
	/*Message area*/
	GtkWidget *message_area;
	
	/*Original text*/
	GtkWidget *text_notebook;
	GtkWidget *text_msgid;
	GtkWidget *text_msgid_plural;
	
	/*Translated text*/
	GtkWidget *trans_notebook;
	GtkWidget *trans_msgstr[MAX_PLURALS];
	
	/*Status widgets*/
	GtkWidget *translated;
	GtkWidget *fuzzy;
	GtkWidget *untranslated;
};

enum
{
	SHOWED_MESSAGE,
	MESSAGE_CHANGED,
	MESSAGE_EDITION_FINISHED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];

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
gtranslator_tab_append_page(const gchar *tab_label,
			    GtkWidget *notebook,
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
	
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll),
					      widget);
	
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
		
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll, label);
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
		 * Disable notebook tabs
		 */
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(priv->text_notebook), FALSE);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->text_notebook), 0);
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
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(tab->priv->text_notebook), TRUE);
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
			_gtranslator_po_increase_decrease_translated(tab->priv->po, FALSE);
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
set_message_area (GtranslatorTab  *tab,
                  GtkWidget *message_area)
{
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
		priv->trans_msgstr[i] = gtranslator_tab_append_page (label,
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
	GtkWidget *image;
	GtkWidget *vertical_box;
	GtkWidget *label_widget;
	GtkWidget *notebook, *tm_layout, *tm, *comments_label, *tm_label, *scroll;
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
	 * Comment
	 */	
	comments_label = gtk_label_new ("Comments");
	tm_label = gtk_label_new ("Translation Memory");

	priv->comment = gtranslator_comment_panel_new (GTK_WIDGET (tab));
	gtk_widget_show (priv->comment);

	priv->translation_memory = gtranslator_translation_memory_ui_new (GTK_WIDGET (tab));
	gtk_widget_show (priv->translation_memory);

	notebook = gtk_notebook_new ();
	gtk_widget_show (notebook);

	gtk_paned_pack2(GTK_PANED(priv->comment_pane), notebook, TRUE, TRUE);
	
	gtk_notebook_append_page (GTK_NOTEBOOK (notebook), priv->comment, comments_label);
	gtk_notebook_append_page (GTK_NOTEBOOK (notebook), priv->translation_memory, tm_label);
	
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
	priv->text_notebook = gtk_notebook_new();
	gtk_notebook_set_show_border(GTK_NOTEBOOK(priv->text_notebook), FALSE);
	gtk_widget_show (priv->text_notebook);
	priv->text_msgid = gtranslator_tab_append_page(_("Singular"),
						       priv->text_notebook,
						       FALSE);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(priv->text_msgid), FALSE);
	priv->text_msgid_plural = gtranslator_tab_append_page(_("Plural"),
							      priv->text_notebook,
							      FALSE);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(priv->text_msgid_plural), FALSE);
	gtk_box_pack_start(GTK_BOX(vertical_box), priv->text_notebook, TRUE, TRUE, 0);

	
	/*
	 * Translation widgets
	 */
	priv->trans_notebook = gtk_notebook_new();
	gtk_notebook_set_show_border(GTK_NOTEBOOK(priv->trans_notebook), FALSE);
	gtk_widget_show (priv->trans_notebook);

	gtk_box_pack_start(GTK_BOX(vertical_box), priv->trans_notebook, TRUE, TRUE, 0);	

	gtk_paned_pack1(GTK_PANED(priv->comment_pane), vertical_box, FALSE, FALSE);
	
	gtk_box_pack_start(GTK_BOX(tab), priv->content_pane, TRUE, TRUE, 0);
}

static void
gtranslator_tab_init (GtranslatorTab *tab)
{
	GtkWidget *image;
	
	tab->priv = GTR_TAB_GET_PRIVATE (tab);
	
	g_signal_connect(tab, "message-changed",
			 G_CALLBACK(update_status), NULL);
	
	gtranslator_tab_draw(tab);
}

static void
gtranslator_tab_finalize (GObject *object)
{
	GtranslatorTab *tab = GTR_TAB(object);
	gint i;
	
	if(tab->priv->po)
		g_object_unref(tab->priv->po);
	
	G_OBJECT_CLASS (gtranslator_tab_parent_class)->finalize (object);
}

static void
gtranslator_tab_class_init (GtranslatorTabClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorTabPrivate));

	object_class->finalize = gtranslator_tab_finalize;
	
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
 * gtranslator_tab_get_active_text_tab:
 * @tab: a #GtranslationTab
 * 
 * Return value: the number of the active original text notebook.
 **/
gint
gtranslator_tab_get_active_text_tab(GtranslatorTab *tab)
{
	return gtk_notebook_get_current_page(GTK_NOTEBOOK(tab->priv->text_notebook));
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
 * gtranslator_tab_get_comment_panel:
 * @tab: a #GtranslatorTab
 *
 * Return value: the #GtranslaorCommentPanel
 */
GtranslatorCommentPanel *
gtranslator_tab_get_comment_panel(GtranslatorTab *tab)
{
	return GTR_COMMENT_PANEL(tab->priv->comment);
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
gtranslator_tab_message_go_to(GtranslatorTab *tab,
			      GList * to_go,
			      gboolean searching)
{
	GtranslatorPo *po;
	static gint pos = 0;
	GList *current_msg;
	gchar *message_error;
	GtkWidget *message_area;
 
	g_return_if_fail (tab != NULL);
	g_return_if_fail (to_go != NULL);
	g_return_if_fail (GTR_IS_MSG (to_go->data));
		
	po = tab->priv->po;
	
	current_msg = gtranslator_po_get_current_message(po);
	message_error = gtranslator_msg_check(current_msg->data);
	if(message_error == NULL)
	{
		/*
		 * Emitting message-edition-finished signal
		 */
		g_signal_emit (G_OBJECT (tab), signals[MESSAGE_EDITION_FINISHED],
			       0, GTR_MSG (current_msg->data));
		
		gtranslator_tab_show_message(tab, to_go->data);
		set_message_area(tab, NULL);
	}
	else
	{
		message_area = create_error_message_area(_("There is an error in the message:"),
							 message_error);
		set_message_area(tab, message_area);
		g_free (message_error);
		return;
	}
	
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
