/*
 * (C) 2000-2001 	Gediminas Paulauskas <menesis@gtranslator.org>
 *			Thomas Ziehmer <thomas@gtranslator.org>
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

#include "find.h"
#include "prefs.h"
#include "parse.h"
#include "gui.h"
#include "sidebar.h"

#include <string.h>

#define MAXHITS 10

static regex_t *target;
static int eflags = 0;
static gchar *pattern = NULL;

static gboolean repeat_all(GList * begin, FEFuncR func, gpointer user_data,
			   gboolean first);
static gboolean find_in_msg(GList * msg, gpointer useless, gboolean first);
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

gboolean repeat_all(GList * begin, FEFuncR func, gpointer user_data,
                    gboolean first)
{
	GList *msg;
	int retval;
	gboolean next;

	g_return_val_if_fail(begin != NULL, FALSE);

	msg = begin;
	do {
		next = FALSE;

		retval= func(msg, user_data, first);
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
			g_return_val_if_fail(msg != NULL, TRUE);
		}
	} while (msg != begin || next);
	return FALSE;
}

/*
 * Searches for target in msg, and on success, displays that message.
 */

static GList *get_find_pos(gchar *str)
{
  GList      *pl = NULL;
  regmatch_t *pos;
  gchar      *strend;
  int         offset = 0;
  int         end;

  if (NULL == str) return NULL;
  strend = str + strlen(str);

  pos = (regmatch_t *)g_malloc(sizeof(regmatch_t));
  while (str < strend && 0 == regexec(target, str, 1, pos, 0)) {
    end = pos->rm_eo;

    pos->rm_so += offset;
    pos->rm_eo += offset;
    pl = g_list_append(pl, pos);

    if (end == 0) {
      str++;
      offset++;
    } else {
      str += end;
      offset += end;
    }

    pos = (regmatch_t *)g_malloc(sizeof(regmatch_t));
  }

  return pl;
}

#define SEARCH(str)                                                \
{                                                                  \
	g_list_free(poslist);                                      \
	poslist = get_find_pos(GTR_MSG(msg->data)->str);           \
	hits = g_list_length(poslist);                             \
}

//    while (pos[hits].rm_so != -1 && hits < MAXHITS) hits++;        

static int find_in_msg(GList * msg, gpointer useless, gboolean first)
{
	static int step = 0;
	static int hits = 0, actpos = 0;
	GList *poslist = NULL;
	regmatch_t *pos;

	if (first) step = 0;

	if ((wants.find_in != 0) && (GTR_MSG(msg->data)->msgstr == NULL))
		return -1;

	if ((wants.find_in & findTranslated) && 1 == step) {
		if (hits >= actpos) SEARCH(msgstr);

		if (hits > 0 && actpos < hits) {
			/*
			 * We found it!
			 */
			goto_given_msg(msg);
			pos = (regmatch_t *)g_list_nth_data(poslist, actpos);
			gtk_editable_select_region(GTK_EDITABLE(trans_box),
				pos->rm_so, pos->rm_eo);
			actpos++;

			return 1;
		} else actpos = 0;
	}
	if ((wants.find_in & findEnglish) && 0 == step) {
		if (hits >= actpos) SEARCH(msgid);
		if (hits > 0 && actpos < hits) {
			/*
			 * We found it!
			 */
			goto_given_msg(msg);
			pos = (regmatch_t *)g_list_nth_data(poslist, actpos);
			gtk_editable_select_region(GTK_EDITABLE(text1),
				pos->rm_so, pos->rm_eo);
			actpos++;
		
			return 1;
		} else actpos = 0;
	}
	if((wants.find_in & findComment) && 2 == step) {
		if (hits >= actpos) SEARCH(comment);
		if (hits > 0 && actpos < hits) {
			/*
			 * Hm, we found it in a comment, show it before
			 *  "hightlighting" it.
			 */  
			goto_given_msg(msg);
			show_comment(text1);
			pos = (regmatch_t *)g_list_nth_data(poslist, actpos);
			gtk_editable_select_region(GTK_EDITABLE(text1),
				pos->rm_so, pos->rm_eo);
			actpos++;
		
			return 1;
		} else actpos = 0;
	}
	step++;

	if (3 == step) {
		step = 0;
		return -1;
	}

        return 0;
}
#undef SEARCH

/*
 * The real search function
 */
void find_do(GtkWidget * widget, gpointer what)
{
	gchar *error;
	GList *begin;
	gboolean first = FALSE;
	update_msg();
	if (what) {
		if (strlen(what) == 0) {
			error = g_strdup_printf(_("Please enter a search string"));
			gnome_app_message(GNOME_APP(app1), error);
			g_free(error);
			return;
		}
		target = gnome_regex_cache_compile(rxc, what, eflags);
		g_free(pattern);
		pattern = what;
	}
	begin = po->current;
	if (!begin) {
		begin = po->messages;
		first = TRUE;
	}
	if (repeat_all(begin, (FEFuncR)find_in_msg, NULL, first) == TRUE)
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

