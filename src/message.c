/*
 * (C) 2001-2003 	Fatih Demir <kabalak@gtranslator.org>
 * 			Thomas Ziehmer <thomas@gtranslator.org>
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
#ifdef NOT_PORTED
# include "gtkspell.h"
#endif
#include "gui.h"
#include "learn.h"
#include "menus.h"
#include "message.h"
#include "messages-table.h"
#include "parse.h"
#include "prefs.h"
#include "syntax.h"
#include "undo.h"
#ifdef UTF8_CODE
# include "utf8.h"
#endif
#include "utils.h"
#include "utils_gui.h"

#include <string.h>
#include <regex.h>
#include <gtk/gtk.h>

#include <libgnomeui/gnome-app.h>
#include <libgnomeui/gnome-app-util.h>

static gboolean is_fuzzy(GList *msg, gpointer useless);
static gboolean is_untranslated(GList *msg, gpointer useless);

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
	
	g_return_if_fail(file_opened == TRUE);
	
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
	
	g_return_if_fail(file_opened == TRUE);

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
#ifdef NOT_PORTED
	gchar *ispell_command[5];
#endif

	if(!file_opened)
	{
		return;
	}
	
	g_return_if_fail(msg!=NULL);

	nothing_changes = TRUE;
	gtranslator_text_boxes_clean();

	/*
	 * Set up the comment display.
	 */
	gtranslator_comment_display(GTR_COMMENT(msg->comment));
	
	/*
	 * Substitute the free spaces in the msgid only if this is wished and
	 *  possible.
	 *
	 * FIXME: this is also done by insert_text_handler. It does not do
	 *  syntax stuff, but should. Then here only gtk_text_insert should be
	 *   left.
	 */ 
	if(GtrPreferences.dot_char)
	{
		gchar *temp;

		g_return_if_fail(msg!=NULL);
		g_return_if_fail(msg->msgid!=NULL);
		
		temp = gtranslator_utils_invert_dot(msg->msgid);
		gtranslator_insert_text(text_box, temp);
		
		GTR_FREE(temp);

		if (msg->msgstr) {
			temp = gtranslator_utils_invert_dot(msg->msgstr);
			
			gtranslator_insert_text(trans_box, temp);
			
			GTR_FREE(temp);
		}
	} else {
		gtranslator_insert_text(text_box, msg->msgid);
		gtranslator_insert_text(trans_box, msg->msgstr);
	}
#ifdef NOT_PORTED	
	/*
	 * Use instant spell checking via gtkspell only if the corresponding
	 *  setting in the preferences is set.
	 */
	if(GtrPreferences.instant_spell_check)
	{
		/*
		 * Start up gtkspell if not already done.
		 */ 
		if(!gtkspell_running())
		{
			ispell_command[0]="ispell";
			ispell_command[1]="-a";
			
			/*
			 * Should we use special dictionary settings?
			 */ 
			if(GtrPreferences.use_own_dict && GtrPreferences.dictionary)
			{
				ispell_command[2]="-d";
				ispell_command[3]=g_strdup_printf("%s",
					GtrPreferences.dictionary);
				ispell_command[4]=NULL;
			} else {
				ispell_command[2]=NULL;
			}
		
			/*
			 * Start the gtkspell process.
			 */ 
			gtkspell_start(NULL, ispell_command);
		}

		/*
		 * Attach it to the translation box for instant spell checking.
		 */ 
		gtkspell_attach(GTK_TEXT(trans_box));
	}
#endif //NOT_PORTED

	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(the_edit_menu[19].widget),
		msg->status & GTR_MSG_STATUS_FUZZY
	);
	nothing_changes = FALSE;
	message_changed = FALSE;
}

void gtranslator_message_update(void)
{
  guint len; 
  //		pos;
	GtkTextIter start, end;
	GtrMsg 
		*msg = GTR_MSG(po->current->data);
		
	if (!message_changed)
		return;
	//	pos = gtk_editable_get_position( GTK_EDITABLE(trans_box) );
	// gtk_text_freeze( GTK_TEXT(trans_box) );
	len = gtk_text_buffer_get_char_count(gtk_text_view_get_buffer(trans_box));
	if (len) {
		gtk_text_buffer_get_bounds(gtk_text_view_get_buffer(trans_box), &start, &end);

		/* Make both strings end with or without endline */
		// XXX fix it
		/*
		if (msg->msgid[strlen(msg->msgid) - 1] == '\n') {
			if (GTK_TEXT_INDEX(trans_box), len - 1)
			    != '\n')
				gtk_editable_insert_text(
				    GTK_EDITABLE(trans_box), "\n", 1, &len);
		} else {
			if (GTK_TEXT_INDEX(GTK_TEXT(trans_box), len - 1)
			    == '\n') {
				gtk_editable_delete_text(
				    GTK_EDITABLE(trans_box), len-1, len);
				len--;
			}
		}
		*/

		GTR_FREE(msg->msgstr);

#ifdef UTF8_CODE
		if(po->utf8)
		{
			msg->msgstr=gtranslator_utf8_get_gtk_text_as_utf8_string(GTK_WIDGET(trans_box));
		}
		else
		{
			msg->msgstr=gtk_editable_get_chars(
				GTK_EDITABLE(trans_box), 0, len);
		}
		*/
		// XXX we always work with utf-8
		// YYY that's why I'm preparing to carve out all
		//     the utf-8 stuff except and make sure there
		//     are g_converts on load/save (for non-utf8
		//     users).
#endif

		msg->msgstr = gtk_text_buffer_get_text(gtk_text_view_get_buffer(trans_box), &start, &end, FALSE);
		
		/*
		 * If spaces were substituted with dots, replace them back
		 */
		if(GtrPreferences.dot_char) {
			gchar *old;
			old = msg->msgstr;
			msg->msgstr = gtranslator_utils_invert_dot(old);
			g_free(old);
		}
		if (!(msg->status & GTR_MSG_STATUS_TRANSLATED)) {
			msg->status |= GTR_MSG_STATUS_TRANSLATED;
			po->translated++;
		}

		/*
		 * Learn the msgstr as a string.
		 */
		if(GtrPreferences.auto_learn)
		{
			gtranslator_learn_string(msg->msgid, msg->msgstr);
		}
		
	} else {
		msg->msgstr = NULL;
		msg->status &= ~GTR_MSG_STATUS_TRANSLATED;
		po->translated--;
	}
	
	message_changed = FALSE;
	
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
		gtranslator_messages_table_update_message_status(GTR_MSG(po->current->data));
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
	else if (pos == po->length - 1)
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
	else if (pos == po->length - 1)
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
		
		/*
		 * FIXME: Crashes currently...
		 * FIXED?
		 */
		GTR_FREE(comment);
		GTR_FREE(comchar);
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
	g_return_if_fail(file_opened==TRUE);

	msg=GTR_MSG(GTR_PO(po)->current->data);
	g_return_if_fail(msg!=NULL);

	GTR_FREE(msg->msgstr);
	msg->msgstr = g_strdup(msg->msgid);
	
	/*
	 * It is no longer fuzzy.
	 */
	gtranslator_message_status_set_fuzzy(msg, FALSE);
	po->fuzzy--;
	msg->status |= GTR_MSG_STATUS_STICK;
	msg->status |= GTR_MSG_STATUS_TRANSLATED;

	message_changed = TRUE;
	gtranslator_message_show(po->current->data);
	gtranslator_get_translated_count();
	gtranslator_actions_enable(ACT_REVERT, ACT_SAVE);
}

/*
 * Free the structure and it's elements.
 */
void gtranslator_message_free(gpointer data, gpointer useless)
{
	g_return_if_fail(data!=NULL);
	gtranslator_comment_free(&GTR_MSG(data)->comment);
	GTR_FREE(GTR_MSG(data)->msgid);
	GTR_FREE(GTR_MSG(data)->msgstr);

	GTR_FREE(GTR_MSG(data)->msgid_plural);

	GTR_FREE(data);
}
