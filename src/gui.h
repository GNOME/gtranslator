/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gmx.net>
 *			Gediminas Paulauskas <menesis@delfi.lt>
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

#ifndef GTR_GUI_H
#define GTR_GUI_H 1

#include <gnome.h>

#ifdef USE_WINDOW_ICON
#include <libgnomeui/gnome-window-icon.h>
#endif

/*
 * The generally used EShortcutModel.
 */ 
#ifdef USE_GAL_GUI
#include <gal/shortcut-bar/e-shortcut-bar.h>

EShortcutModel *model;
#endif

/*
 * The globally needed widgets
 */

GtkWidget *app1;
GtkWidget *trans_box;
GtkWidget *text1;
GtkWidget *appbar1;

void create_app1(void);

/*
 * For the status messages
 */
gchar status[128];

/*
 * Actions IDs
 */
enum {
	/*
	 * FIXME: some time this is disabled
	 */
	ACT_NULL,
	ACT_COMPILE,
	ACT_SAVE,
	ACT_UPDATE,
	ACT_SAVE_AS,
	ACT_REVERT,
	ACT_CLOSE,
	/**********/
	ACT_UNDO,
	ACT_CUT,
	ACT_COPY,
	ACT_PASTE,
	ACT_CLEAR,
	ACT_FIND,
	ACT_FIND_AGAIN,
	ACT_HEADER,
	/**********/
	ACT_FIRST,
	ACT_BACK,
	ACT_NEXT,
	ACT_LAST,
	ACT_GOTO,
	ACT_NEXT_FUZZY,
	ACT_NEXT_UNTRANSLATED,
	/**********/
	ACT_TRANSLATED,
	ACT_FUZZY,
	ACT_STICK,
	/*
	 * This must always be the last, add new entries above
	 */
	ACT_END
};

void change_actions(gboolean state, ...);
#define enable_actions(args...) change_actions(TRUE, args, ACT_END)
#define disable_actions(args...) change_actions(FALSE, args, ACT_END)
void disable_actions_no_file(void);
void enable_actions_just_opened(void);

/*
 * Various functions for displaying messages 
 */
void update_msg(void);
void display_msg(GList  * list_item);
void clean_text_boxes(void);
void toggle_msg_status(GtkWidget  * widget, gpointer which);

/*
 * Shows the needed message in text boxes
 */
void goto_given_msg(GList  * to_go);
void goto_first_msg(GtkWidget  * widget, gpointer useless);
void goto_prev_msg(GtkWidget  * widget, gpointer useless);
void goto_next_msg(GtkWidget  * widget, gpointer useless);
void goto_last_msg(GtkWidget  * widget, gpointer useless);
void goto_nth_msg(GtkWidget  * widget, gpointer number);

/*
 * If TRUE, means that trans_box is being changed by program, not user
 */
gboolean nothing_changes;

#endif
