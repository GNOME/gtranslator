/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
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
#include "gtkspell.h"
#include "gui.h"
#include "menus.h"
#include "message.h"
#include "parse.h"
#include "prefs.h"
#include "syntax.h"
#include "utf8.h"
#include "utils.h"

#include <string.h>
#include <gtk/gtk.h>

#include <libgnomeui/gnome-app.h>
#include <libgnomeui/gnome-app-util.h>

#include <gal/widgets/e-unicode.h>

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
void gtranslator_message_show(GList * list_item)
{
	GtrMsg *msg;
	gchar *ispell_command[5];
	
	g_return_if_fail(list_item!=NULL);

	msg = GTR_MSG(list_item->data);
	nothing_changes = TRUE;
	gtranslator_text_boxes_clean();
	
	/*
	 * Substitute the free spaces in the msgid only if this is wished and
	 *  possible.
	 *
	 * FIXME: this is also done by insert_text_handler. It does not do
	 *  syntax stuff, but should. Then here only gtk_text_insert should be
	 *   left.
	 */ 
	if(wants.dot_char)
	{
		gchar *temp;

		temp = g_strdup(msg->msgid);
		gtranslator_utils_invert_dot(temp);
		
		gtranslator_syntax_insert_text(text_box, temp);
		
		g_free(temp);

		if (msg->msgstr) {
			if(gtranslator_utf8_po_file_is_utf8())
			{
				temp=gtranslator_utf8_get_plain_msgstr(&msg);
			}
			else
			{
				temp=g_strdup(msg->msgstr);
			}
			
			gtranslator_utils_invert_dot(temp);
			
			gtranslator_syntax_insert_text(trans_box, temp);
			
			g_free(temp);
		}
	} else {
		gtranslator_syntax_insert_text(text_box, msg->msgid);

		if(gtranslator_utf8_po_file_is_utf8())
		{
			gchar *text=gtranslator_utf8_get_plain_msgstr(&msg);
			
			gtranslator_syntax_insert_text(trans_box, text);
		}
		else
		{
			gtranslator_syntax_insert_text(trans_box, msg->msgstr);
		}
	}
	
	/*
	 * Use instant spell checking via gtkspell only if the corresponding
	 *  setting in the preferences is set.
	 */
	if(wants.instant_spell_check)
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
			if(wants.use_own_dict && wants.dictionary)
			{
				ispell_command[2]="-d";
				ispell_command[3]=g_strdup_printf("%s",
					wants.dictionary);
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
#define set_active(number,flag) \
	gtk_check_menu_item_set_active(\
		(GtkCheckMenuItem *)(the_msg_status_menu[number].widget),\
		msg->status & flag);
	set_active(0, GTR_MSG_STATUS_TRANSLATED);
	set_active(1, GTR_MSG_STATUS_FUZZY);
	set_active(2, GTR_MSG_STATUS_STICK);
#undef set_active
	nothing_changes = FALSE;
	message_changed = FALSE;
}

void gtranslator_message_update(void)
{
	guint len;
	GtrMsg *msg = GTR_MSG(po->current->data);
	if (!message_changed)
		return;
	len = gtk_text_get_length(GTK_TEXT(trans_box));
	if (len) {
		/* Make both strings end with or without endline */
		if (msg->msgid[strlen(msg->msgid) - 1] == '\n') {
			if (GTK_TEXT_INDEX(GTK_TEXT(trans_box), len - 1)
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
		
		g_free(msg->msgstr);

		if(gtranslator_utf8_po_file_is_utf8())
		{
			msg->msgstr=e_utf8_gtk_editable_get_text(
				GTK_EDITABLE(trans_box));
		}
		else
		{
			msg->msgstr=gtk_editable_get_chars(
				GTK_EDITABLE(trans_box), 0, len);
		}

		/*
		 * If spaces were substituted with dots, replace them back
		 */
		if(wants.dot_char)
			gtranslator_utils_invert_dot(msg->msgstr);
		if (!(msg->status & GTR_MSG_STATUS_TRANSLATED)) {
			msg->status |= GTR_MSG_STATUS_TRANSLATED;
			po->translated++;
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
	} else if (flag == GTR_MSG_STATUS_STICK) {
		gtranslator_message_status_set_sticky(GTR_MSG(po->current->data),
				GTK_CHECK_MENU_ITEM(item)->active);
		gtranslator_message_show(po->current);
		message_changed = TRUE;
	}
	gtranslator_message_update();
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
	gtranslator_message_show(po->current);
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
	regex_t *rex;
	regmatch_t pos[3];
	gchar *comment = msg->comment;
	
	/* 
	 * If fuzzy status is already correct
	 */
	if (((msg->status & GTR_MSG_STATUS_FUZZY) != 0) == fuzzy)
		return;
	if (fuzzy) {
		msg->status |= GTR_MSG_STATUS_FUZZY;
		po->fuzzy++;
		rex = gnome_regex_cache_compile(rxc,
			  "^(#), c-format", 
			  REG_EXTENDED | REG_NEWLINE);
		if (!regexec(rex, comment, 3, pos, 0)) {
			comment[pos[1].rm_so] = '\0';
			msg->comment = g_strdup_printf("%s#, fuzzy%s", comment, 
					    comment+pos[1].rm_eo);
		} else {
			msg->comment = g_strdup_printf("%s#, fuzzy\n", comment);
		}
		g_free(comment);
	} else {
		msg->status &= ~GTR_MSG_STATUS_FUZZY;
		po->fuzzy--;
		rex = gnome_regex_cache_compile(rxc, 
			  "(^#, fuzzy$)|^#, (fuzzy,) c-format",
			  REG_EXTENDED | REG_NEWLINE);
		if (!regexec(rex, comment, 3, pos, 0)) {
			gint i = (pos[1].rm_so == -1) ? 2 : 1;
			strcpy(comment+pos[i].rm_so, comment+pos[i].rm_eo+1);
		}
	}
}

/*
 * Set Sticky status.
 */
void gtranslator_message_status_set_sticky (GtrMsg * msg, gboolean on)
{
	if (on) {
		g_free(msg->msgstr);
		msg->msgstr = g_strdup(msg->msgid);
		
		/*
		 * It is no longer fuzzy
		 */
		gtranslator_message_status_set_fuzzy(msg, FALSE);
		msg->status |= GTR_MSG_STATUS_STICK;
	} else {
		g_free(msg->msgstr);
		msg->msgstr = g_strdup("");
		msg->status &= ~GTR_MSG_STATUS_STICK;
	}
	gtranslator_get_translated_count();
}

/*
 * Free the structure and it's elements.
 */
void gtranslator_message_free(gpointer data, gpointer useless)
{
	g_free(GTR_MSG(data)->comment);
	g_free(GTR_MSG(data)->msgid);
	g_free(GTR_MSG(data)->msgstr);
	g_free(data);
}
