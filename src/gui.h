/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gtranslator.org>
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

#ifndef GTR_GUI_H
#define GTR_GUI_H 1

#include <gtk/gtkwidget.h>

#include <libgnome/gnome-defs.h>
#include <libgnome/gnome-i18n.h>

/*
 * The globally needed widgets
 */

GtkWidget *gtranslator_application;
GtkWidget *trans_box;
GtkWidget *text_box;
GtkWidget *gtranslator_application_bar;

void gtranslator_create_main_window(void);

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
	ACT_ACCOMPLISH,
	ACT_SAVE_AS,
	ACT_REVERT,
	ACT_CLOSE,
	/**********/
	ACT_VIEW_MESSAGE,
	ACT_VIEW_COMMENTS,
	ACT_VIEW_NUMBER,
	ACT_VIEW_C_FORMAT,
	ACT_VIEW_HOTKEY,
	/**********/
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

void gtranslator_actions_set_up(gboolean state, ...);
#define gtranslator_actions_enable(args...) gtranslator_actions_set_up(TRUE, args, ACT_END)
#define gtranslator_actions_disable(args...) gtranslator_actions_set_up(FALSE, args, ACT_END)
void gtranslator_actions_setup_state_no_file(void);
void gtranslator_actions_setup_file_opened(void);

/*
 * Various functions for displaying messages 
 */
void gtranslator_message_update(void);
void gtranslator_application_bar_update(gint pos);
void gtranslator_message_show(GList  * list_item);
void gtranslator_text_boxes_clean(void);
void gtranslator_message_change_status(GtkWidget  * widget, gpointer which);

/*
 * Shows the needed message in text boxes
 */
void gtranslator_message_go_to(GList  * to_go);
void gtranslator_message_go_to_first(GtkWidget  * widget, gpointer useless);
void gtranslator_message_go_to_previous(GtkWidget  * widget, gpointer useless);
void gtranslator_message_go_to_next(GtkWidget  * widget, gpointer useless);
void gtranslator_message_go_to_last(GtkWidget  * widget, gpointer useless);
void gtranslator_message_go_to_no(GtkWidget  * widget, gpointer number);

/*
 * A status defining callback -- now generaly defined for instance.
 */
void gtranslator_translation_changed(GtkWidget  * widget, gpointer useless);

/*
 * If TRUE, means that trans_box is being changed by program, not user
 */
gboolean nothing_changes;

#endif
