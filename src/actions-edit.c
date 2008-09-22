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
 *     MERCHANMSGILITY or FITNESS FOR A PARTICULAR PURMSGSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gi18n.h>

#include "actions.h"
#include "dialogs/header-dialog.h"
#include "dialogs/comment-dialog.h"
#include "msg.h"
#include "po.h"
#include "tab.h"
#include "window.h"

void
gtranslator_actions_edit_undo (GtkAction   *action,
			       GtranslatorWindow *window)
{
	GtranslatorView *active_view;
	GtkSourceBuffer *active_document;

	active_view = gtranslator_window_get_active_view (window);
	g_return_if_fail (active_view);

	active_document = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (active_view)));

	gtk_text_buffer_begin_user_action(GTK_TEXT_BUFFER(active_document));
	gtk_source_buffer_undo (active_document);
	gtk_text_buffer_end_user_action(GTK_TEXT_BUFFER(active_document));

	gtk_widget_grab_focus (GTK_WIDGET (active_view));
}

void
gtranslator_actions_edit_redo (GtkAction   *action,
			       GtranslatorWindow *window)
{
	GtranslatorView *active_view;
	GtkSourceBuffer *active_document;

	active_view = gtranslator_window_get_active_view (window);
	g_return_if_fail (active_view);

	active_document = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (active_view)));

	gtk_text_buffer_begin_user_action(GTK_TEXT_BUFFER(active_document));
	gtk_source_buffer_redo (active_document);
	gtk_text_buffer_end_user_action(GTK_TEXT_BUFFER(active_document));

	gtk_widget_grab_focus (GTK_WIDGET (active_view));
}

void
gtranslator_actions_edit_cut (GtkAction   *action,
			      GtranslatorWindow *window)
{
	GtranslatorView *active_view;

	active_view = gtranslator_window_get_active_view (window);
	g_return_if_fail (active_view);

	gtranslator_view_cut_clipboard (active_view);

	gtk_widget_grab_focus (GTK_WIDGET (active_view));
}

void
gtranslator_actions_edit_copy (GtkAction   *action,
			       GtranslatorWindow *window)
{
	GtranslatorView *active_view;

	active_view = gtranslator_window_get_active_view (window);
	g_return_if_fail (active_view);

	gtranslator_view_copy_clipboard (active_view);

	gtk_widget_grab_focus (GTK_WIDGET (active_view));
}

void
gtranslator_actions_edit_paste (GtkAction   *action,
				GtranslatorWindow *window)
{
	GtranslatorView *active_view;

	active_view = gtranslator_window_get_active_view (window);
	g_return_if_fail (active_view);

	gtranslator_view_paste_clipboard (active_view);

	gtk_widget_grab_focus (GTK_WIDGET (active_view));
}


/*
 * Use the untranslated message as the translation.
 */
void 
gtranslator_message_copy_to_translation(GtkAction *action,
					GtranslatorWindow *window)
{
	const gchar *msgid;
	GtranslatorTab *current;
	GtranslatorPo *po;
	GList *msg;
	gint page_index;
	
	current = gtranslator_window_get_active_tab(window);
	po = gtranslator_tab_get_po(current);
	msg = gtranslator_po_get_current_message(po);
	
	page_index = gtranslator_tab_get_active_text_tab(current);
	
	if(page_index == 0)
		msgid = gtranslator_msg_get_msgid(msg->data);
	else msgid = gtranslator_msg_get_msgid_plural(msg->data);
	
	if(msgid)
	{
		page_index = gtranslator_tab_get_active_trans_tab(current);
		
		if(page_index == 0)
			gtranslator_msg_set_msgstr(msg->data, msgid);
		else
			gtranslator_msg_set_msgstr_plural(msg->data, page_index, msgid);
	}
	
	/*
	 * should we change the state of the message?
	 * if we have then put the message as translated
	 */
	if(gtranslator_msg_is_fuzzy(msg->data) && gtranslator_prefs_manager_get_unmark_fuzzy())
		gtranslator_msg_set_fuzzy(msg->data, FALSE);
		
	gtranslator_tab_message_go_to(current, msg, FALSE);
	
	/*
	 * Emit that message was changed.
	 */
	g_signal_emit_by_name(current, "message_changed", msg->data);
}

/*
 * Toggle the sticky status
 */
void 
gtranslator_message_status_toggle_fuzzy(GtkAction *action,
					GtranslatorWindow *window)
{
	GtranslatorTab *current;
	GtranslatorPo *po;
	GList *msg;
	
	current = gtranslator_window_get_active_tab(window);
	po = gtranslator_tab_get_po(current);
	msg = gtranslator_po_get_current_message(po);
	
	if(gtranslator_msg_is_fuzzy(msg->data))
		gtranslator_msg_set_fuzzy(msg->data, FALSE);
	else
		gtranslator_msg_set_fuzzy(msg->data, TRUE);
	
	/*
	 * Emit that message was changed.
	 */
	g_signal_emit_by_name(current, "message_changed", msg->data);					
}

void
gtranslator_actions_edit_preferences(GtkAction *action,
				     GtranslatorWindow *window)
{
	gtranslator_show_preferences_dialog(window);
}

void
gtranslator_actions_edit_header(GtkAction *action,
				GtranslatorWindow *window)
{
	gtranslator_show_header_dialog(window);
}

void
gtranslator_edit_message_comment(GtkAction *action,
				 GtranslatorWindow *window)
{	
	gtranslator_show_comment_dialog(window);
}
