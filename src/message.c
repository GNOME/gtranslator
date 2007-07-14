/*
 * (C) 2001-2007 	Fatih Demir <kabalak@kabalak.net>
 * 			Ross Golder <ross@golder.org>
 * 			Gediminas Paulauskas <menesis@kabalak.net>
 * 			Thomas Ziehmer <thomas@kabalak.net>
 *			Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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
#include "learn.h"
#include "menus.h"
#include "message.h"
#include "messages-table.h"
#include "page.h"
#include "prefs.h"
#include "translator.h"
#include "undo.h"
#include "utils.h"
#include "utils_gui.h"

#include <string.h>
#include <regex.h>
#include <gtk/gtk.h>

#ifdef HAVE_GTKSPELL
#include <gtkspell/gtkspell.h>
#endif

static gboolean is_fuzzy(GList *msg, gpointer useless);
static gboolean is_untranslated(GList *msg, gpointer useless);

/*
 * Write the change back to the gettext PO instance in memory and
 * mark the page dirty
 */
void gtranslator_message_translation_update(GtkTextBuffer *textbuffer, gpointer *user_data);

#ifdef HAVE_GTKSPELL
GtkSpell *gtrans_spell = NULL;
#endif



/*
 * Returns TRUE if the message is translated
 */
gboolean
po_message_is_translated (po_message_t message)
{
	if (po_message_msgid_plural(message) == NULL)
		return po_message_msgstr(message)[0] != '\0';	
	else
	{
		int i;

		for (i = 0; ; i++)
		{
			const gchar *str_i = po_message_msgstr_plural(message, i);
			if (str_i == NULL)
				break;
			if (str_i[0] == '\0')
				return FALSE;
		}
		
	return TRUE;
	}
}

/*
 * Calls function func on each item in list 'begin'. Starts from 
 * item 'begin', loops to first element, and stops at 'begin'.
 * Returns TRUE, if found, FALSE otherwise.
 */
gboolean 
gtranslator_message_for_each(GList * begin,
			     FEFunc func,
			     gpointer user_data)
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

static gboolean 
is_fuzzy(GList *msg, gpointer useless)
{
	/* 
	 * Control if there's any message data.
	 */
	if(!(GTR_MSG(msg->data)))
	{
		g_warning(_("Couldn't get the message!"));
		return FALSE;
	}
	if (po_message_is_fuzzy(GTR_MSG(msg->data)->message)) {
		gtranslator_message_go_to(msg);
		return TRUE;
	} else
		return FALSE;
}

void 
gtranslator_message_go_to_next_fuzzy(GtkWidget * widget,
				     gpointer useless)
{
	GList *begin;
	
	g_assert(current_page != NULL);
	
 	begin = current_page->po->current->next;
	if (!begin)
		begin = current_page->po->messages;
	if (gtranslator_message_for_each(begin, (FEFunc)is_fuzzy, NULL) == TRUE)
		return;
	//Message dialog
    	gtranslator_show_message(_("There are no fuzzy messages left."), NULL);
	//Disable next_fuzzy menu_item
	gtk_widget_set_sensitive(gtranslator_menuitems->next_fuzzy, FALSE);
}

static gboolean 
is_untranslated(GList *msg, gpointer useless)
{
	if(!po_message_is_translated(GTR_MSG(msg->data)->message))
	{
		gtranslator_message_go_to(msg);
		return TRUE;
	}
	return FALSE;
}

void 
gtranslator_message_go_to_next_untranslated(GtkWidget * widget, gpointer useless)
{
	GList *begin;
    
	g_assert(current_page != NULL);

 	begin = current_page->po->current->next;
	if (!begin)
		begin = current_page->po->messages;
	if (gtranslator_message_for_each(begin, (FEFunc)is_untranslated, NULL))
		return;
	//Message dialog
    	gtranslator_show_message(_("All messages seems to be translated."), NULL);
	//Disable next_untranslated menuitem
	gtk_widget_set_sensitive(gtranslator_menuitems->next_untranslated, FALSE);
}

#ifdef HAVE_GTKSPELL
void
gtranslator_attach_gskspell()
{
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
		
		if (gtrans_spell == NULL && current_page->trans_msgstr != NULL) {
		    gtrans_spell = gtkspell_new_attach(GTK_TEXT_VIEW(current_page->trans_msgstr), NULL, &error);
		    if (gtrans_spell == NULL) {
			g_print(_("gtkspell error: %s\n"), error->message);
			errortext = g_strdup_printf(_("GtkSpell was unable to initialize.\n %s"), error->message);
			g_error_free(error);
		    }
		}
		/*guint nplural = languages[].plural;
		if( nplural >= 2 )
		{
			guint i;
			GtkSpell *gtkspell[nplural-1];
			for(i = 0; i < nplural; i++) 
			{*/
				/*
				 * Doing this i think there are memory leaks
				 * becouse i can't detach it
				 */
			/*	gtkspell[i] = NULL;
				gtkspell[i] = 
					gtkspell_new_attach(GTK_TEXT_VIEW(current_page->trans_msgstr_plural[i]), NULL, &error);
				if (gtkspell[i] == NULL) {
					g_print(_("gtkspell error: %s\n"), error->message);
					errortext = g_strdup_printf(_("GtkSpell was unable to initialize.\n %s"), error->message);
					g_error_free(error);
		    		}
			}
		}*/
	} else {
	    if (gtrans_spell != NULL) {
		gtkspell_detach(gtrans_spell);
		gtrans_spell = NULL;
	    }
	}
}
#endif

/*
 * Cleans up the text boxes.
 */
void gtranslator_text_boxes_clean()
{
	GtkTextIter start, end;
	GtkTextBuffer *buffer;

	buffer = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(current_page->text_msgid)));
	gtk_text_buffer_get_bounds(buffer, &start, &end);
	gtk_text_buffer_delete(buffer, &start, &end);

	buffer = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(current_page->trans_msgstr)));
	gtk_text_buffer_get_bounds(buffer, &start, &end);
	gtk_text_buffer_delete(buffer, &start, &end);
}

/* 
 * Display the message in text boxes
 */
void 
gtranslator_message_show(GtrMsg *msg)
{
	GtkTextBuffer *buf;
	const gchar *msgid, *msgid_plural, *msgstr_plural;
	const gchar *msgstr;
	int i;
	
	g_assert(current_page != NULL);
	
	/*
	 * Update current_page->po->current
	 */
	i = g_list_index(current_page->po->messages, msg);
	current_page->po->current = g_list_nth(current_page->po->messages, i);
	
	/*
	 * Clear up previous message widgets from the original/translated
	 * fields
	 */
	gtranslator_text_boxes_clean();
	
	/*
	 * Set up new widgets
	 */
	msgid = po_message_msgid(msg->message);
	if(msgid) {
		buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(current_page->text_msgid));
		if(GtrPreferences.dot_char) {
			gchar *temp = gtranslator_utils_invert_dot((gchar*)msgid);
			gtk_text_buffer_set_text(buf, temp, -1);
			g_free(temp);
		}
		else {
			gtk_text_buffer_set_text(buf, (gchar*)msgid, -1);
		}
	}
	msgid_plural = po_message_msgid_plural(msg->message);
	if(!msgid_plural) {
		msgstr = po_message_msgstr(msg->message);
		/*
		 * Disable notebook tabs
		 */
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(current_page->text_notebook), FALSE);
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(current_page->trans_notebook), FALSE);
		if(msgstr) 
		{
			buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(current_page->trans_msgstr));
			if(GtrPreferences.dot_char) {
				gchar *temp = gtranslator_utils_invert_dot((gchar*)msgstr);
				gtk_text_buffer_set_text(buf, temp, -1);
				g_free(temp);
			}
			else {
				gtk_text_buffer_set_text(buf, (gchar*)msgstr, -1);
			}
			g_signal_connect(buf, "end-user-action",
					 G_CALLBACK(gtranslator_message_translation_update),
					 current_page);
		}
	}
	else {
		/*
		 * Enable notebook tabs
		 * FIXME: Ata 4 formas de plural pode haber según a páxina de gettext 
		 * http://www.gnu.org/software/gettext/manual/html_node/Plural-forms.html#Plural-forms
		 * Débese comprobar a configuración para saber cantas solapas se deben poñer.
		 * Se non hai nada na configuración deberase pensar que o número é 2 (Galego)
		 * Alomenos así é como está configurado no kbabel
		 * Pois vai ser que hai 6 formas do plural.
		 * 
		 */
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(current_page->text_notebook), TRUE);
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(current_page->trans_notebook), TRUE);
		buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(current_page->text_msgid_plural));
		if(GtrPreferences.dot_char) {
			gchar *temp = gtranslator_utils_invert_dot((gchar*)msgid_plural);
			gtk_text_buffer_set_text(buf, temp, -1);
			g_free(temp);
		}
		else {
			gtk_text_buffer_set_text(buf, (gchar*)msgid_plural, -1);
		}
		msgstr_plural = po_message_msgstr_plural(msg->message, 0);
		if(msgstr_plural)
		{
			buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(current_page->trans_msgstr));
			if(GtrPreferences.dot_char) {
				gchar *temp = gtranslator_utils_invert_dot((gchar*)msgstr_plural);
				gtk_text_buffer_set_text(buf, temp, -1);
				g_free(temp);
			}
			else gtk_text_buffer_set_text(buf, (gchar*)msgstr_plural, -1);
			
			g_signal_connect(buf, "end-user-action",
					 G_CALLBACK(gtranslator_message_translation_update),
					 current_page);
		}
	}
	/*i=2;
	while(msgid_plural && (msgstr[i] = po_message_msgstr_plural(msg->message, i - 1))) {
		buf = gtk_text_buffer_new(NULL);
		if(GtrPreferences.dot_char) {
			gchar *temp = gtranslator_utils_invert_dot((gchar*)msgstr[i]);
			gtk_text_buffer_set_text(buf, temp, -1);
			g_free(temp);
		}
		else {
			gtk_text_buffer_set_text(buf, (gchar*)msgstr[i], -1);
		}
		g_signal_connect(buf, "changed", G_CALLBACK(gtranslator_message_translation_update), current_page);
		current_page->trans_msgstr[i] = gtk_text_view_new_with_buffer(buf);
		gtk_box_pack_start(trans_vbox, current_page->trans_msgstr[i], TRUE, TRUE, 0);
		gtk_widget_show(current_page->trans_msgstr[i]);
		i++;
	}*/
	
	/*
	 * Set up the comment display.
	 */
	gtranslator_comment_display(GTR_COMMENT(msg->comment));

#ifdef HAVE_GTKSPELL
	gtranslator_attach_gskspell();
#endif

	/*
	 * Status widgets and fuzzy menuitem
	 */
	if(po_message_is_fuzzy(msg->message))
	{
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtranslator_menuitems->fuzzy),
							       TRUE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(current_page->fuzzy), TRUE);
	}
	else if(po_message_is_translated(msg->message))
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(current_page->translated), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(current_page->untranslated), TRUE);

	nothing_changes = FALSE;
	msg->changed = FALSE;
}

void 
gtranslator_message_update(void)
{
	GtrMsg *msg;
	GtrPo *po;
	
	g_assert(current_page != NULL);
	
	po = current_page->po;
	if(!(msg = GTR_MSG(po->current->data)))
		return;
	
	/*
	 * Update the statusbar informations.
	 */
	gtranslator_application_bar_update(g_list_position(po->messages, 
		po->current));

	/*
	 * Go to the corresponding row in the messages table.
	 */
	if(current_page->messages_table)
	{
		gtranslator_messages_table_update_row(current_page->messages_table, msg);
	}

	/*
	 * Enable the remove all translations function if wished and if any
	 *  translated entries have already landed.
	 */
	if(GtrPreferences.rambo_function && (po->translated > 0))
	{
		gtk_widget_set_sensitive(gtranslator_menuitems->remove_translations, TRUE);
	}
	//	gtk_text_thaw( GTK_TEXT(trans_box) );
	//	gtk_editable_set_position( GTK_EDITABLE(trans_box), pos );

	msg->changed = FALSE;
}

void 
gtranslator_message_change_status(GtkWidget  * item, gpointer which)
{
	GtrPo *po;
	gint flag;
	
	g_assert(current_page != NULL);
	po = current_page->po;
	flag = GPOINTER_TO_INT(which);
	if (nothing_changes)
		return;
	gtranslator_translation_changed(NULL, NULL);
	if (flag == GTR_MSG_STATUS_FUZZY) {
		gtranslator_message_status_set_fuzzy(GTR_MSG(po->current->data),
			       gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(item)));
		if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(item)))
			po->fuzzy++;
		else
			po->fuzzy--;
	}
	
	gtranslator_message_update();
	if(current_page->messages_table)
	{
		gtranslator_messages_table_update_row(current_page->messages_table,
						      GTR_MSG(po->current->data));
	}
}

/*
 * Updates current msg, and shows to_go msg instead, also adjusts actions
 */
void 
gtranslator_message_go_to(GList * to_go)
{
	GtrPo *po;
	static gint pos = 0;
 
	g_return_if_fail (to_go!=NULL);
	
	po = current_page->po;
	
	gtranslator_message_update();
	
	if (pos == 0)
	{
		gtk_widget_set_sensitive(gtranslator_menuitems->first, TRUE);
	    	gtk_widget_set_sensitive(gtranslator_menuitems->t_first, TRUE);
		gtk_widget_set_sensitive(gtranslator_menuitems->go_back, TRUE);
	    	gtk_widget_set_sensitive(gtranslator_menuitems->t_go_back, TRUE);
	}	
	else if (pos == g_list_length(po->messages) - 1)
	{
		gtk_widget_set_sensitive(gtranslator_menuitems->go_forward, TRUE);
	    	gtk_widget_set_sensitive(gtranslator_menuitems->t_go_forward, TRUE);
		gtk_widget_set_sensitive(gtranslator_menuitems->goto_last, TRUE);
	    	gtk_widget_set_sensitive(gtranslator_menuitems->t_goto_last, TRUE);
	}

	gtranslator_message_show(to_go->data);

	if(current_page->messages_table)
	{
		/*Only work with move forward, with move backward don't work*/
		gtranslator_messages_table_select_row(current_page->messages_table,
						      GTR_MSG(po->current->data));
	}
	
	pos = g_list_position(po->messages, po->current);
	
	if (pos == 0)
	{
	    	//First items
		gtk_widget_set_sensitive(gtranslator_menuitems->first, FALSE);
	    	gtk_widget_set_sensitive(gtranslator_menuitems->t_first, FALSE);
	    	//go_back items
		gtk_widget_set_sensitive(gtranslator_menuitems->go_back, FALSE);
	    	gtk_widget_set_sensitive(gtranslator_menuitems->t_go_back, FALSE);
	    	
	}	
	else if (pos == g_list_length(po->messages) - 1)
	{
	    	//Go_forward items
		gtk_widget_set_sensitive(gtranslator_menuitems->go_forward, FALSE);
	    	gtk_widget_set_sensitive(gtranslator_menuitems->t_go_forward, FALSE);
	    	//Goto_last items
		gtk_widget_set_sensitive(gtranslator_menuitems->goto_last, FALSE);
	 	gtk_widget_set_sensitive(gtranslator_menuitems->t_goto_last, FALSE);
	}
	
	gtranslator_application_bar_update(pos);

	/*
	 * Clean up any Undo stuff lying 'round.
	 */
	gtranslator_undo_clean_register();
	gtk_widget_set_sensitive(gtranslator_menuitems->undo, FALSE);
    	gtk_widget_set_sensitive(gtranslator_menuitems->t_undo, FALSE);
}

/*
 * Callbacks for moving around messages 
 */
void 
gtranslator_message_go_to_first(GtkWidget  * widget,
				gpointer useless)
{
	gtranslator_message_go_to(g_list_first(current_page->po->messages));
}

void 
gtranslator_message_go_to_previous(GtkWidget  * widget,
				   gpointer useless)
{
	gtranslator_message_go_to(g_list_previous(current_page->po->current));
}

void 
gtranslator_message_go_to_next(GtkWidget  * widget,
			       gpointer useless)
{
	gtranslator_message_go_to(g_list_next(current_page->po->current));
}

void 
gtranslator_message_go_to_last(GtkWidget  * widget,
			       gpointer useless)
{
	gtranslator_message_go_to(g_list_last(current_page->po->messages));
}

void 
gtranslator_message_go_to_no(GtkWidget  * widget,
			     gpointer number)
{
	gtranslator_message_go_to(g_list_nth(current_page->po->messages,
					     GPOINTER_TO_UINT(number)));
}

/*
 * Set Fuzzy status.
 */
void 
gtranslator_message_status_set_fuzzy(GtrMsg * msg,
				     gboolean fuzzy)
{
	/*
	 * I think this can do it with void po_message_set_fuzzy (po_message_t message, int fuzzy);
	 * but is neccessary save the file.
	 */
/*	static int compiled = FALSE;
	static regex_t rexf, rexc;
	regmatch_t pos[3];
	gchar *comment;*/

	g_return_if_fail(msg!=NULL);
	/*g_return_if_fail(msg->comment!=NULL);
	g_return_if_fail(GTR_COMMENT(msg->comment)->comment!=NULL);

	comment=gtranslator_comment_get_comment_contents(msg->comment);
	
	if (!compiled) {
		regcomp(&rexf, "^(#), c-format", REG_EXTENDED | REG_NEWLINE);
		regcomp(&rexc, "(^#, fuzzy$)|^#, (fuzzy,) .*$", REG_EXTENDED | REG_NEWLINE);
		compiled = TRUE;
	}
*/
	/* 
	 * If fuzzy status is already correct
	 */
/*	if (((msg->status & GTR_MSG_STATUS_FUZZY) != 0) == fuzzy)
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
*/
		/* No need to free comment. Fixes several crashes and file corruption bugs */
		/* g_free(comment); */
		
	/*	g_free(comchar);
	} else {
		msg->status &= ~GTR_MSG_STATUS_FUZZY;
		if (!regexec(&rexc, comment, 3, pos, 0)) {
			gint i = (pos[1].rm_so == -1) ? 2 : 1;
			strcpy(comment+pos[i].rm_so, comment+pos[i].rm_eo+1);
		}
	}*/
	
	
	po_message_set_fuzzy(msg->message, fuzzy);

	//Status widget
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(current_page->fuzzy), fuzzy);
}

/*
 * Set Sticky status.
 */
void 
gtranslator_message_status_set_sticky (GtrMsg * msg,
				       gpointer useless)
{
	g_assert(current_page != NULL);

	msg=GTR_MSG(GTR_PO(current_page->po)->current->data);
	g_return_if_fail(msg!=NULL);

	/*
	 * It is no longer fuzzy.
	 */
	gtranslator_message_status_set_fuzzy(msg, FALSE);
	current_page->po->fuzzy--;
	msg->status |= GTR_MSG_STATUS_STICK;
	msg->status |= GTR_MSG_STATUS_TRANSLATED;
	msg->changed = TRUE;
	gtranslator_message_show(current_page->po->current->data);
	gtranslator_update_translated_count(current_page->po);
	//Enable revert and save menuitems 
	gtk_widget_set_sensitive(gtranslator_menuitems->revert, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->save, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->t_save, TRUE);
	gtranslator_translation_changed(NULL, NULL);
	gtranslator_update_progress_bar();
}

/*
 * Clear the translation
 */
void 
gtranslator_message_clear_translation(GtrMsg *msg,
				      gpointer useless)
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
void 
gtranslator_message_free(gpointer data,
			 gpointer useless)
{
	g_return_if_fail(data!=NULL);
	gtranslator_comment_free(&GTR_MSG(data)->comment);

	g_free(data);
}

/*
 * Use the untranslated message as the translation.
 */
void 
gtranslator_message_copy_to_translation(void)
{
	const gchar *msgid;

	po_message_t message = GTR_MSG(current_page->po->current->data)->message;

	/*We need to know if we are in a plural textview*/
	msgid = po_message_msgid(message);
	po_message_set_msgstr(message, msgid);
	free((void *)msgid);
}

/*
 * Toggle the sticky status
 */
void 
gtranslator_message_status_toggle_fuzzy(GtkWidget *widget,
					gpointer useless)
{
	GtrMsg *msg = GTR_MSG(current_page->po->current->data);
	if(msg->is_fuzzy) {
		msg->is_fuzzy = FALSE;
		po_message_set_fuzzy(msg->message, FALSE);
		current_page->po->fuzzy--;
	}
	else
	{
		msg->is_fuzzy = TRUE;
		po_message_set_fuzzy(msg->message, TRUE);
		current_page->po->fuzzy++;
	}
}

/*
 * Write the change back to the gettext PO instance in memory and
 * mark the page dirty
 */
void 
gtranslator_message_translation_update(GtkTextBuffer *textbuffer,
				       gpointer *user_data)
{
	GtkTextIter start, end;
	GtkTextBuffer *buf;
	GtrMsg *msg;
	const char *check;
	char *translation;
	int i;
	
	/* Work out which message this is associated with */
	msg = current_page->po->current->data;
	buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(current_page->trans_msgstr));
	if(textbuffer == buf) {
		/* Get message as UTF-8 buffer */
		gtk_text_buffer_get_bounds(textbuffer, &start, &end);
		translation = gtk_text_buffer_get_text(textbuffer, &start, &end, TRUE);
		
		/* TODO: convert to file's own encoding if not UTF-8 */
		
		/* Write back to PO file in memory */
		if(!(check = po_message_msgid_plural(msg->message))) {
			po_message_set_msgstr(msg->message, translation);
		}
		else {
			po_message_set_msgstr_plural(msg->message, 0, translation);
			//free(check);
		}
		g_free(translation);
		
		/* Activate 'save', 'revert' etc. */
		gtranslator_page_dirty(textbuffer, user_data);
		return;
	}
	i=2;
	//while(i < 16) {
		/* Know when to break out of the loop */
		/*if(!current_page->trans_msgstr) {
			break;
		}*/
		
		/* Have we reached the one we want yet? */
		buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(current_page->trans_msgstr));
		/*if(textbuffer != buf) {
			i++;
			continue;
		}*/
		
		/* Get message as UTF-8 buffer */
		gtk_text_buffer_get_bounds(textbuffer, &start, &end);
		translation = gtk_text_buffer_get_text(textbuffer, &start, &end, TRUE);
		
		/* TODO: convert to file's own encoding if not UTF-8 */
		
		/* Write back to PO file in memory */
		po_message_set_msgstr_plural(msg->message, i - 1, translation);

		/* Activate 'save', 'revert' etc. */
		gtranslator_page_dirty(textbuffer, user_data);
		return;
		
	//}

	/* Shouldn't get here */
	g_return_if_reached();
}
