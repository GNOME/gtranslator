/*
 * (C) 2001-2004 	Fatih Demir <kabalak@kabalak.net>
 * 			Ross Golder <ross@golder.org>
 * 			Gediminas Paulauskas <menesis@kabalak.net>
 * 			Thomas Ziehmer <thomas@kabalak.net>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *    
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "actions.h"
#include "comment.h"
#include "dialogs.h"
#include "gui.h"
#include "learn.h"
#include "menus.h"
#include "message.h"
#include "messages-table.h"
#include "parse.h"
#include "prefs.h"
#include "undo.h"
#include "utils.h"
#include "utils_gui.h"

#include <string.h>
#include <regex.h>
#include <gtk/gtk.h>
#include <gtkspell/gtkspell.h>
#include <libgnomeui/gnome-app.h>
#include <libgnomeui/gnome-app-util.h>

static gboolean is_fuzzy(GList *msg, gpointer useless);
static gboolean is_untranslated(GList *msg, gpointer useless);

GtkSpell *gtrans_spell = NULL;

/*
 * Calls function func on each item in list 'begin'. Starts from 
 * item 'begin', loops to first element, and stops at 'begin'.
 * Returns TRUE, if found, FALSE otherwise.
 */
gboolean gtranslator_message_for_each(GList * begin, FEFunc func, gpointer user_data)
{
	GList *msg;

	g_return_val_if_fail(begin != NULL, FALSE);

	msg = begin;
	do {
		if (msg == NULL) {
			msg = g_list_first(begin);
			g_return_val_if_fail(msg != NULL, TRUE);
		}
		if (func(msg, user_data))
			return TRUE;
		msg = msg->next;
	} while (msg != begin);
	return FALSE;
}

static gboolean is_fuzzy(GList *msg, gpointer useless)
{
	/* 
	 * Control if there's any message data.
	 */
	if(!(GTR_MSG(msg->data)))
	{
		g_warning(_("Couldn't get the message!"));
		return FALSE;
	}
	if (GTR_MSG(msg->data)->status & GTR_MSG_STATUS_FUZZY) {
		gtranslator_message_go_to(msg);
		return TRUE;
	} else
		return FALSE;
}

void gtranslator_message_go_to_next_fuzzy(GtkWidget * widget, gpointer useless)
{
	GList *begin;
	
	g_return_if_fail(po != NULL);
	
 	begin = po->current->next;
	if (!begin)
		begin = po->messages;
	if (gtranslator_message_for_each(begin, (FEFunc)is_fuzzy, NULL) == TRUE)
		return;
	gnome_app_message(GNOME_APP(gtranslator_application), 
			  _("There are no fuzzy messages left."));
	gtranslator_actions_disable(ACT_NEXT_FUZZY);
}

static gboolean is_untranslated(GList *msg, gpointer useless)
{
	if (GTR_MSG(msg->data)->status & GTR_MSG_STATUS_TRANSLATED)
		return FALSE;
	gtranslator_message_go_to(msg);
	return TRUE;
}

void gtranslator_message_go_to_next_untranslated(GtkWidget * widget, gpointer useless)
{
	GList *begin;
	
	g_return_if_fail(po != NULL);

 	begin = po->current->next;
	if (!begin)
		begin = po->messages;
	if (gtranslator_message_for_each(begin, (FEFunc)is_untranslated, NULL))
		return;
	gnome_app_message(GNOME_APP(gtranslator_application), 
			  _("All messages seem to be translated."));
	gtranslator_actions_disable(ACT_NEXT_UNTRANSLATED);
}

/* 
 * Display the message in text boxes
 */
void gtranslator_message_show(GtrMsg *msg)
{
	GtkTextBuffer *buf;
	const char *msgid, *msgid_plural, *msgstr[15];

	g_assert(po != NULL);		
	g_assert(msg != NULL);

	/*
	 * Clear up previous message widgets from the original/translated
	 * fields
	 */
	while(g_list_length(GTK_BOX(document_view->text_vbox)->children) > 0) {
		gpointer widget = GTK_BOX(document_view->text_vbox)->children->data;
		gtk_widget_destroy(GTK_WIDGET(widget));
		GTK_BOX(document_view->text_vbox)->children = GTK_BOX(document_view->text_vbox)->children->next;
	}
	while(g_list_length(GTK_BOX(document_view->trans_vbox)->children) > 0) {
		gpointer widget = GTK_BOX(document_view->trans_vbox)->children->data;
		gtk_widget_destroy(GTK_WIDGET(widget));
		GTK_BOX(document_view->text_vbox)->children = GTK_BOX(document_view->trans_vbox)->children->next;
	}
	
	/*
	 * Set up new widgets
	 */
	msgid = po_message_msgid(msg->message);
	if(msgid) {
		buf = gtk_text_buffer_new(NULL);
		if(GtrPreferences.dot_char) {
			gchar *temp = gtranslator_utils_invert_dot((gchar*)msgid);
			gtk_text_buffer_set_text(buf, temp, -1);
			g_free(temp);
		}
		else {
			gtk_text_buffer_set_text(buf, (gchar*)msgid, -1);
		}
		document_view->text_msgid = gtk_text_view_new_with_buffer(buf);
		gtk_box_pack_end(GTK_BOX(document_view->text_vbox), document_view->text_msgid, FALSE, FALSE, 0);
	}
	msgid_plural = po_message_msgid_plural(msg->message);
	if(msgid_plural) {
		buf = gtk_text_buffer_new(NULL);
		if(GtrPreferences.dot_char) {
			gchar *temp = gtranslator_utils_invert_dot((gchar*)msgid);
			gtk_text_buffer_set_text(buf, temp, -1);
			g_free(temp);
		}
		else {
			gtk_text_buffer_set_text(buf, (gchar*)msgid, -1);
		}
		document_view->text_msgid = gtk_text_view_new_with_buffer(buf);
		gtk_box_pack_end(GTK_BOX(document_view->text_vbox), document_view->text_msgid, FALSE, FALSE, 0);
	}
	msgstr[0] = po_message_msgstr(msg->message);
	
	/*
	 * Set up the comment display.
	 */
	gtranslator_comment_display(GTR_COMMENT(msg->comment));

#ifdef NOTYET
	/*
	 * Use instant spell checking via gtkspell only if the corresponding
	 *  setting in the preferences is set.
	 */
	if(GtrPreferences.instant_spell_check)
	{
		/*
		 * Start up gtkspell if not already done.
		 */ 
		GError *error = NULL;
		char *errortext = NULL;
		
		if (gtrans_spell == NULL && trans_vbox != NULL) {
		    gtrans_spell = gtkspell_new_attach(trans_box, NULL, &error);
		    if (gtrans_spell == NULL) {
			g_print(_("gtkspell error: %s\n"), error->message);
			errortext = g_strdup_printf(_("GtkSpell was unable to initialize.\n %s"), error->message);
			g_error_free(error);
		    }
		} 		
	} else {
	    if (gtrans_spell != NULL) {
		gtkspell_detach(gtrans_spell);
		gtrans_spell = NULL;
	    }
	}
#endif

	/* Enable/disable 'toggle fuzzy status' widgets */
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(the_edit_menu[17].widget),
		msg->status & GTR_MSG_STATUS_FUZZY
	);

	nothing_changes = FALSE;
	msg->changed = FALSE;
}

void gtranslator_message_update(void)
{
	GtrMsg *msg = GTR_MSG(po->current->data);
	if (!msg->changed)
		return;
	
	/*
	 * Update the statusbar informations.
	 */
	gtranslator_application_bar_update(g_list_position(po->messages, 
		po->current));

	/*
	 * Go to the corresponding row in the messages table.
	 */
	if(GtrPreferences.show_messages_table)
	{
		gtranslator_messages_table_update_row(msg);
	}

	/*
	 * Enable the remove all translations function if wished and if any
	 *  translated entries have already landed.
	 */
	if(GtrPreferences.rambo_function && (po->translated > 0))
	{
		gtranslator_actions_enable(ACT_REMOVE_ALL_TRANSLATIONS);
	}
	//	gtk_text_thaw( GTK_TEXT(trans_box) );
	//	gtk_editable_set_position( GTK_EDITABLE(trans_box), pos );

	msg->changed = FALSE;
}

void gtranslator_message_change_status(GtkWidget  * item, gpointer which)
{
	gint flag = GPOINTER_TO_INT(which);
	if (nothing_changes)
		return;
	gtranslator_translation_changed(NULL, NULL);
	if (flag == GTR_MSG_STATUS_FUZZY) {
		gtranslator_message_status_set_fuzzy(GTR_MSG(po->current->data),
			       GTK_CHECK_MENU_ITEM(item)->active);
		if(GTK_CHECK_MENU_ITEM(item)->active)
			po->fuzzy++;
		else
			po->fuzzy--;
	}
	
	gtranslator_message_update();
	if(GtrPreferences.show_messages_table)
	{
		gtranslator_messages_table_update_row(GTR_MSG(po->current->data));
	}
}

/*
 * Updates current msg, and shows to_go msg instead, also adjusts actions
 */
void gtranslator_message_go_to(GList * to_go)
{
	static gint pos = 0;
 
	g_return_if_fail (to_go!=NULL);
	
	gtranslator_message_update();
	
	if (pos == 0)
	{
		gtranslator_actions_enable(ACT_FIRST, ACT_BACK);
	}	
	else if (pos == g_list_length(po->messages) - 1)
	{
		gtranslator_actions_enable(ACT_NEXT, ACT_LAST);
	}

	po->current = to_go;
	gtranslator_message_show(po->current->data);

	if(GtrPreferences.show_messages_table)
	{
		gtranslator_messages_table_select_row(GTR_MSG(po->current->data));
	}
	
	pos = g_list_position(po->messages, po->current);
	
	if (pos == 0)
	{
		gtranslator_actions_disable(ACT_FIRST, ACT_BACK);
	}	
	else if (pos == g_list_length(po->messages) - 1)
	{
		gtranslator_actions_disable(ACT_NEXT, ACT_LAST);
	}
	
	gtranslator_application_bar_update(pos);

	/*
	 * Clean up any Undo stuff lying 'round.
	 */
	gtranslator_undo_clean_register();
	gtranslator_actions_disable(ACT_UNDO);
}

/*
 * Callbacks for moving around messages 
 */
void gtranslator_message_go_to_first(GtkWidget  * widget, gpointer useless)
{
	gtranslator_message_go_to(g_list_first(po->messages));
}

void gtranslator_message_go_to_previous(GtkWidget  * widget, gpointer useless)
{
	gtranslator_message_go_to(g_list_previous(po->current));
}

void gtranslator_message_go_to_next(GtkWidget  * widget, gpointer useless)
{
	gtranslator_message_go_to(g_list_next(po->current));
}

void gtranslator_message_go_to_last(GtkWidget  * widget, gpointer useless)
{
	gtranslator_message_go_to(g_list_last(po->messages));
}

void gtranslator_message_go_to_no(GtkWidget  * widget, gpointer number)
{
	gtranslator_message_go_to(g_list_nth(po->messages, GPOINTER_TO_UINT(number)));
}

/*
 * Set Fuzzy status.
 */
void gtranslator_message_status_set_fuzzy(GtrMsg * msg, gboolean fuzzy)
{
	static int compiled = FALSE;
	static regex_t rexf, rexc;
	regmatch_t pos[3];
	gchar *comment;

	g_return_if_fail(msg!=NULL);
	g_return_if_fail(msg->comment!=NULL);
	g_return_if_fail(GTR_COMMENT(msg->comment)->comment!=NULL);

	comment=gtranslator_comment_get_comment_contents(msg->comment);
	
	if (!compiled) {
		regcomp(&rexf, "^(#), c-format", REG_EXTENDED | REG_NEWLINE);
		regcomp(&rexc, "(^#, fuzzy$)|^#, (fuzzy,) .*$", REG_EXTENDED | REG_NEWLINE);
		compiled = TRUE;
	}

	/* 
	 * If fuzzy status is already correct
	 */
	if (((msg->status & GTR_MSG_STATUS_FUZZY) != 0) == fuzzy)
		return;
	if (fuzzy) {
		gchar *comchar;
		
		msg->status |= GTR_MSG_STATUS_FUZZY;

		if (!regexec(&rexf, comment, 3, pos, 0)) {
			comment[pos[1].rm_so] = '\0';
			comchar = g_strdup_printf("%s#, fuzzy%s", comment, 
					    comment+pos[1].rm_eo);
		} else {
			comchar = g_strdup_printf("%s#, fuzzy\n", comment);
		}

		gtranslator_comment_update(&msg->comment, comchar);

		/* No need to free comment. Fixes several crashes and file corruption bugs */
		/* g_free(comment); */
		
		g_free(comchar);
	} else {
		msg->status &= ~GTR_MSG_STATUS_FUZZY;
		if (!regexec(&rexc, comment, 3, pos, 0)) {
			gint i = (pos[1].rm_so == -1) ? 2 : 1;
			strcpy(comment+pos[i].rm_so, comment+pos[i].rm_eo+1);
		}
	}
}

/*
 * Set Sticky status.
 */
void gtranslator_message_status_set_sticky (GtrMsg * msg, gpointer useless)
{
	g_return_if_fail(po != NULL);

	msg=GTR_MSG(GTR_PO(po)->current->data);
	g_return_if_fail(msg!=NULL);

	/*
	 * It is no longer fuzzy.
	 */
	gtranslator_message_status_set_fuzzy(msg, FALSE);
	po->fuzzy--;
	msg->status |= GTR_MSG_STATUS_STICK;
	msg->status |= GTR_MSG_STATUS_TRANSLATED;
	msg->changed = TRUE;
	gtranslator_message_show(po->current->data);
	gtranslator_update_translated_count(po);
	gtranslator_actions_enable(ACT_REVERT, ACT_SAVE);
	gtranslator_translation_changed(NULL, NULL);
	gtranslator_update_progress_bar();
}

/*
 * Clear the translation
 */
void gtranslator_message_clear_translation(GtrMsg *msg, gpointer useless)
{
	const char *emptystr = {0x0};
	po_message_t message = GTR_MSG(msg)->message;

	g_return_if_fail(message!=NULL);
	
	/*
	 * Clear any existing translation for this message
	 */
	 
	po_message_set_msgstr(message, emptystr);
}

/*
 * Free the structure and it's elements.
 */
void gtranslator_message_free(gpointer data, gpointer useless)
{
	g_return_if_fail(data!=NULL);
	gtranslator_comment_free(&GTR_MSG(data)->comment);

	g_free(data);
}

/*
 * Use the untranslated message as the translation.
 */
void gtranslator_message_copy_to_translation(void)
{
	const char *msgid;

	po_message_t message = GTR_MSG(po->current->data)->message;

	msgid = po_message_msgid(message);
	po_message_set_msgstr(message, msgid);
	free((void *)msgid);
}

/*
 * Toggle the sticky status
 */
void gtranslator_message_status_toggle_fuzzy(GtkWidget *widget, gpointer useless) {
	GtrMsg *msg = GTR_MSG(po->current->data);
	if(msg->is_fuzzy) {
		msg->is_fuzzy = FALSE;
		po->fuzzy--;
	}
	else
	{
		msg->is_fuzzy = TRUE;
		po->fuzzy++;
	}
}
