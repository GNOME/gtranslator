/*
 * (C) 2001-2002 	Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
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

#ifndef GTR_ACTIONS_H
#define GTR_ACTIONS_H 1

#include <glib.h>
#include <gtk/gtkwidget.h>

/*
 * ACT_* enum values and handling of gui activation/disabling.
 */

enum {
	ACT_NULL,
	ACT_COMPILE,
	ACT_SAVE,
	ACT_UPDATE,
	ACT_AUTOTRANSLATE,
	ACT_REMOVE_ALL_TRANSLATIONS,
	ACT_SAVE_AS,
	ACT_REVERT,
	ACT_CLOSE,
	/***********************/
	ACT_IMPORT_UTF8,
	ACT_EXPORT_UTF8,
	/***********************/
	ACT_UNDO,
	ACT_CUT,
	ACT_COPY,
	ACT_PASTE,
	ACT_CLEAR,
	ACT_FIND,
	ACT_FIND_AGAIN,
	ACT_REPLACE,
	ACT_QUERY,
	ACT_HEADER,
	ACT_COMMENT,
	/***********************/
	ACT_FIRST,
	ACT_BACK,
	ACT_NEXT,
	ACT_LAST,
	ACT_GOTO,
	ACT_NEXT_FUZZY,
	ACT_NEXT_UNTRANSLATED,
	/***********************/
	ACT_TRANSLATED,
	ACT_FUZZY,
	ACT_STICK,
	
	/*
	 * This must always be the last, add new entries above
	 */
	ACT_END
};

/*
 * Defines for convenient handling of the state changes.
 */
#define gtranslator_actions_enable(args...) gtranslator_actions_set_up(TRUE, args, ACT_END)
#define gtranslator_actions_disable(args...) gtranslator_actions_set_up(FALSE, args, ACT_END)

/*
 * Set up the general binding at startup time.
 */
void gtranslator_actions_set_up_default(void);

/*
 * The real backend function for the actions.
 */
void gtranslator_actions_set_up(gboolean state, ...);

/*
 * Easy calls for the defined states to easify work.
 */
void gtranslator_actions_set_up_state_no_file(void);
void gtranslator_actions_set_up_file_opened(void);

/*
 * The GUI Undo/Redo callbacks/functions.
 */
void gtranslator_actions_undo(GtkWidget *widget, gpointer useless);

/*
 * A wrapper function for disabling the Fuzzy menu item.
 */
void gtranslator_actions_disable_fuzzy_menu_item(void);

#endif
