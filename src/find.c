/*
 * (C) 2000 	Gediminas Paulauskas <menesis@delfi.lt>
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

#include "find.h"
#include "prefs.h"
#include "parse.h"
#include "gui.h"

#include <string.h>

static regex_t *target;
static int eflags = 0;
static gchar *pattern = NULL;

static gboolean find_in_msg(GList * msg, gpointer useless);
static gboolean is_fuzzy(GList *msg, gpointer useless);
static gboolean is_untranslated(GList *msg, gpointer useless);

/*
 * Calls function func on each item in list 'begin'. Starts from 
 * item 'begin', loops to first element, and stops at 'begin'.
 * Returns TRUE, if found, FALSE otherwise.
 */
gboolean for_each_msg(GList * begin, FEFunc func, gpointer user_data)
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

/*
 * Searches for target in msg, and on success, displays that message.
 */
static gboolean find_in_msg(GList * msg, gpointer useless)
{
	regmatch_t pos[1];
	if ((wants.find_in != 0) && (GTR_MSG(msg->data)->msgstr == NULL))
		return FALSE;
	if ((wants.find_in != 0) &&
	    (!regexec(target, GTR_MSG(msg->data)->msgstr, 1, pos, 0)))
        {
		/*
		 * We found it!
		 */
		goto_given_msg(msg);
		gtk_editable_select_region(GTK_EDITABLE(trans_box),
					   pos->rm_so, pos->rm_eo);
		return TRUE;
	}
	if ((wants.find_in != 1) &&
	    (!regexec(target, GTR_MSG(msg->data)->msgid, 1, pos, 0)))
        {
		/*
		 * We found it!
		 */
		goto_given_msg(msg);
		gtk_editable_select_region(GTK_EDITABLE(text1),
					   pos->rm_so, pos->rm_eo);
		return TRUE;
	}
        return FALSE;	
}

/*
 * The real search function
 */
void find_do(GtkWidget * widget, gpointer what)
{
	gchar *error;
	GList *begin;
	update_msg();
	if (what) {
		target = gnome_regex_cache_compile(rxc, what, eflags);
		g_free(pattern);
		pattern = what;
	}
	begin = po->current->next;
	if (!begin) 
		begin = po->messages;
	if (for_each_msg(begin, (FEFunc)find_in_msg, NULL) == TRUE)
		return;
	error = g_strdup_printf(_("Could not find\n\"%s\""), pattern);
	gnome_app_message(GNOME_APP(app1), error);
	g_free(error);
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
		goto_given_msg(msg);
		return TRUE;
	} else
		return FALSE;
}

void goto_next_fuzzy(GtkWidget * widget, gpointer useless)
{
	GList *begin;
	
	g_return_if_fail(file_opened == TRUE);
	
 	begin = po->current->next;
	if (!begin)
		begin = po->messages;
	if (for_each_msg(begin, (FEFunc)is_fuzzy, NULL) == TRUE)
		return;
	gnome_app_message(GNOME_APP(app1), 
			  _("There are no fuzzy messages left."));
	disable_actions(ACT_NEXT_FUZZY);
}

static gboolean is_untranslated(GList *msg, gpointer useless)
{
	if (GTR_MSG(msg->data)->status & GTR_MSG_STATUS_TRANSLATED)
		return FALSE;
	goto_given_msg(msg);
	return TRUE;
}

void goto_next_untranslated(GtkWidget * widget, gpointer useless)
{
	GList *begin;
	
	g_return_if_fail(file_opened == TRUE);

 	begin = po->current->next;
	if (!begin)
		begin = po->messages;
	if (for_each_msg(begin, (FEFunc)is_untranslated, NULL))
		return;
	gnome_app_message(GNOME_APP(app1), 
			  _("All messages seem to be translated."));
	disable_actions(ACT_NEXT_UNTRANSLATED);
}

void update_flags(void)
{
	if (wants.match_case)
		eflags &= ~REG_ICASE;
	else
		eflags |= REG_ICASE;
}

