/*
 * (C) 2000-2003 	Gediminas Paulauskas <menesis@kabalak.net>
 *			Thomas Ziehmer <thomas@kabalak.net>
 *			Fatih Demir <kabalak@kabalak.net>
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

#include "comment.h"
#include "find.h"
#include "message.h"
#include "page.h"
#include "prefs.h"
#include "utils.h"

#include <string.h>
#include <regex.h>
#include <gtk/gtkeditable.h>
#include <gettext-po.h>

#define MAXHITS 10

typedef gboolean (*FEFuncRX) (gpointer list_item, gpointer user_data, gboolean first, gboolean a, gboolean b, gboolean c);

static regex_t target;
static int eflags = 0;
static gchar *pattern = NULL;

static gboolean repeat_all(GList * begin, FEFuncRX func, gpointer user_data,
			   gboolean first, gboolean a, gboolean b, gboolean c);
static gint find_in_msg(GList * msg, gpointer useless, gboolean first,
	gboolean find_in_comments, gboolean find_in_english, gboolean find_in_translation);

gboolean repeat_all(GList * begin, FEFuncRX func, gpointer user_data,
                    gboolean first, gboolean a, gboolean b, gboolean c)
{
	GList *msg;
	int retval;
	gboolean next;

	g_return_val_if_fail(begin != NULL, FALSE);

	msg = begin;
	do {
		next = FALSE;

		retval=func(msg, user_data, first, a, b, c);
		switch (retval) {
		case 1:
			return TRUE;
			break;
		case 0:
			next = TRUE;
			break;
		case -1:
			msg = msg->next;
			break;
		}
		first = FALSE;
		if (msg == NULL) {
			msg = g_list_first(begin);
			g_return_val_if_fail(msg != NULL, FALSE);
		}
	} while (msg != begin || next);
	return FALSE;
}

/*
 * Returns a GList of regmatch_t pointers for matches of a regexp in the
 * given string.
 */
static GList *get_find_pos(gchar *str)
{
	GList *pl = NULL;
	regmatch_t *pos;
	gchar *strend;
	int offset = 0;
	int end;

	g_return_val_if_fail(str != NULL, FALSE);

	strend = str + strlen(str);
	pos = (regmatch_t *)g_malloc(sizeof(regmatch_t));
	while (str < strend && regexec(&target, str, 1, pos, 0) == 0) {
		end = pos->rm_eo;
		pos->rm_so += offset;
		pos->rm_eo += offset;

		/* Add to the list, and allocate a new pos */
		pl = g_list_append(pl, pos);
		pos = (regmatch_t *)g_malloc(sizeof(regmatch_t));

		if (end == 0) {
			str++;
			offset++;
		} else {
			str += end;
			offset += end;
		}
	}

	/* Free the unused pos */
	g_free(pos);

	return pl;
}

/*
 * Searches for target in msg, and on success, displays that message.
 * Returns: 1 if found, 0 if not found
 */
static gint find_in_msg(GList * msg, gpointer useless, gboolean first,
	gboolean find_in_comments, gboolean find_in_english, gboolean find_in_translation)
{
	po_message_t message;
	static int step = 0;
	static int hits = 0, actpos = 0;
	GList *poslist = NULL;
	regmatch_t *pos;
	const char *p;
	int i = 0;

	message = GTR_MSG(msg->data)->message;
	if (first) step = 0;

#define GTR_SEARCH(field, widget)					     \
	if(hits >= actpos) {						     \
		g_list_free(poslist);					     \
		if(field) poslist = get_find_pos((gchar *)field);	     \
		hits = g_list_length(poslist);				     \
	}								     \
	if(hits > 0 && actpos < hits) {					     \
		/* gtranslator_message_go_to(msg); */			     \
		pos = (regmatch_t *)g_list_nth_data(poslist, actpos);	     \
		gtranslator_selection_set(widget, pos->rm_so, pos->rm_eo);   \
		actpos++;						     \
		return 1;						     \
	}								     \
	else actpos = 0;

	if (find_in_english && 0 == step) {
		p = po_message_msgid(message);
		GTR_SEARCH(p, GTK_TEXT_VIEW(current_page->text_msgid));
		p = po_message_msgid_plural(message);
		GTR_SEARCH(p, GTK_TEXT_VIEW(current_page->text_msgid_plural));
	}
	if (find_in_translation && 1 == step) {
		p = po_message_msgstr(message);
		//FIXME
/*		GTR_SEARCH(p, GTK_TEXT_VIEW(current_page->trans_msgstr[0]));
		while((p = po_message_msgstr_plural(message, i))) {
			GTR_SEARCH(p, GTK_TEXT_VIEW(current_page->trans_msgstr[i+1]));
			i++;
		}*/
	}
#ifdef DONTFORGET
	if(find_in_comments && 2 == step) {
		if (hits >= actpos)
		{
			g_list_free(poslist);
			poslist = get_find_pos(GTR_COMMENT(GTR_MSG(msg->data)->comment)->comment);
			hits = g_list_length(poslist);
		}
		if (hits > 0 && actpos < hits) {
			/*
			 * Hm, we found it in a comment, show it before
			 *  "hightlighting" it.
			 */  
			gtranslator_message_go_to(msg);
			actpos++;
		
			return 1;
		} else actpos = 0;
	}
#endif
	step++;

	if (step > 2) {
		step = 0;
		return -1;
	}
        return 0;
}

/*
 * The real search function
 */
void gtranslator_find(GtkWidget * widget, gpointer what, gboolean find_in_comments,
	gboolean find_in_english, gboolean find_in_translation)
{
	GtrPo *po;
	gchar *error;
	GList *begin;
	gboolean first = FALSE;
	po = current_page->po;
	gtranslator_message_update();
	if (what) {
		if (strlen(what) == 0) {
			error = g_strdup_printf(_("Please enter a search string"));
			GtkWidget *dialog;
			dialog = gtk_message_dialog_new (GTK_WINDOW(gtranslator_application),
							 GTK_DIALOG_DESTROY_WITH_PARENT,
							 GTK_MESSAGE_INFO,
							 GTK_BUTTONS_CLOSE,
							 error);
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
			g_free(error);
			return;
		}
		regcomp(&target, what, eflags);
		g_free(pattern);
		pattern = what;
	}
	begin = po->current;
	if (!begin) {
		begin = po->messages;
		first = TRUE;
	}
	if (repeat_all(begin, (FEFuncRX)find_in_msg, NULL, first, find_in_comments, find_in_english, find_in_translation) == TRUE)
		return;
	error = g_strdup_printf(_("Could not find\n\"%s\""), pattern);
	//Message dialog
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new (GTK_WINDOW(gtranslator_application),
					 GTK_DIALOG_DESTROY_WITH_PARENT,
					 GTK_MESSAGE_INFO,
					 GTK_BUTTONS_CLOSE,
					 error);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
	g_free(error);
}

void gtranslator_update_regex_flags(void)
{
	if (GtrPreferences.match_case)
		eflags &= ~REG_ICASE;
	else
		eflags |= REG_ICASE;
}
