/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
 *		Gediminas Paulauskas <menesis@gtranslator.org>
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
#include "gui.h"
#include "menus.h"
#include "message.h"
#include "prefs.h"
#include "undo.h"
#include "utf8.h"

#include <gtk/gtk.h>
#include <libgnomeui/gnome-app.h>
#include <libgnomeui/gnome-app-helper.h>

typedef struct _GtrAction GtrAction;
#define GTR_ACTION(x) ((GtrAction *)x)

struct _GtrAction {
	GtkWidget *menu;
	GtkWidget *tool;
};

/*
 * "Swaps" the given actions; changes the state correspondingly.
 */
#define gtranslator_actions_swap(x, y); \
		gtranslator_actions_disable(x); \
		gtranslator_actions_enable(y);

/*
 * An array holds all defined actions.
 */
static GtrAction acts[ACT_END];

/*
 * Routines for actions.
 */
static void insert_action(gint act_num, GnomeUIInfo mi, GnomeUIInfo ti);


/****
  * Actions stuff goes here
 ** * */
void gtranslator_actions_set_up(gboolean state, ...)
{
	va_list ap;
	gint id;
	va_start(ap,state);
	while (TRUE) {
		id=va_arg(ap,gint);
		if (id==ACT_END)
			break;
		if (acts[id].menu)
			gtk_widget_set_sensitive(acts[id].menu,state); 
		if (acts[id].tool)
			gtk_widget_set_sensitive(acts[id].tool,state);
	} 
	va_end(ap);
}

static void insert_action(gint act_num, GnomeUIInfo mi, GnomeUIInfo ti)
{
	acts[act_num].menu = mi.widget;
	acts[act_num].tool = ti.widget;
}

void gtranslator_actions_set_up_default()
{
	/*
	 * A unused variable for testing if a toolbar element was provided
	 */
	GnomeUIInfo NONE;

	NONE.widget = NULL;
	insert_action(ACT_COMPILE, the_file_menu[0], the_toolbar[4]);
	insert_action(ACT_UPDATE, the_file_menu[1], the_toolbar[5]);
	insert_action(ACT_ACCOMPLISH, the_file_menu[3], NONE);
	insert_action(ACT_SAVE, the_file_menu[7], the_toolbar[1]);
	insert_action(ACT_SAVE_AS, the_file_menu[8], the_toolbar[2]);
	insert_action(ACT_REVERT, the_file_menu[9], NONE);
	insert_action(ACT_CLOSE, the_file_menu[10], NONE);
	/*----------------------------------------------------------*/
	insert_action(ACT_EXPORT_UTF8, the_file_menu[14], NONE);
	insert_action(ACT_IMPORT_UTF8, the_file_menu[15], NONE);
	/*----------------------------------------------------------*/
	insert_action(ACT_VIEW_MESSAGE, the_views_menu[0], NONE);
	insert_action(ACT_VIEW_COMMENTS, the_views_menu[1], NONE);
	insert_action(ACT_VIEW_NUMBER, the_views_menu[2], NONE);
	insert_action(ACT_VIEW_C_FORMAT, the_views_menu[3], NONE);
	insert_action(ACT_VIEW_HOTKEY, the_views_menu[4], NONE);
	/*----------------------------------------------------------*/
	insert_action(ACT_UNDO, the_edit_menu[0], the_toolbar[8]);
	insert_action(ACT_REDO, the_edit_menu[1], the_toolbar[9]);
	insert_action(ACT_CUT, the_edit_menu[3], NONE);
	insert_action(ACT_COPY, the_edit_menu[4], NONE);
	insert_action(ACT_PASTE, the_edit_menu[5], NONE);
	insert_action(ACT_CLEAR, the_edit_menu[6], NONE);
	insert_action(ACT_FIND, the_edit_menu[8], the_navibar[8]);
	insert_action(ACT_FIND_AGAIN, the_edit_menu[9], NONE);
	insert_action(ACT_REPLACE, the_edit_menu[10], the_navibar[9]);
	insert_action(ACT_QUERY, the_edit_menu[11], the_navibar[10]);
	insert_action(ACT_HEADER, the_edit_menu[13], the_toolbar[6]);
	/*-----------------------------------------------------------*/
	insert_action(ACT_FIRST, the_messages_menu[0], the_navibar[0]);
	insert_action(ACT_BACK, the_messages_menu[1], the_navibar[1]);
	insert_action(ACT_NEXT, the_messages_menu[3], the_navibar[3]);
	insert_action(ACT_LAST, the_messages_menu[4], the_navibar[4]);
	insert_action(ACT_GOTO, the_messages_menu[6], the_navibar[7]);
	insert_action(ACT_NEXT_FUZZY, the_messages_menu[7], the_navibar[6]);
	insert_action(ACT_NEXT_UNTRANSLATED, the_messages_menu[8], the_navibar[5]);
	/*-----------------------------------------------------------*/
	insert_action(ACT_TRANSLATED, the_msg_status_menu[0], NONE);
	insert_action(ACT_FUZZY, the_msg_status_menu[1], NONE);
	insert_action(ACT_STICK, the_msg_status_menu[2], NONE);
	/*-----------------------------------------------------------*/
	insert_action(ACT_END, NONE, NONE);
}

void gtranslator_actions_set_up_state_no_file(void)
{
	gtranslator_actions_disable(ACT_COMPILE, ACT_UPDATE, ACT_ACCOMPLISH,
			ACT_SAVE, ACT_SAVE_AS, ACT_REVERT, ACT_CLOSE, ACT_UNDO,
			ACT_REDO, ACT_CUT, ACT_COPY, ACT_PASTE, ACT_CLEAR,
			ACT_FIND, ACT_FIND_AGAIN, ACT_HEADER, ACT_QUERY,
			ACT_FIRST, ACT_BACK, ACT_NEXT, ACT_LAST, ACT_REPLACE,
			ACT_GOTO, ACT_NEXT_FUZZY, ACT_NEXT_UNTRANSLATED,
			ACT_FUZZY, ACT_TRANSLATED, ACT_STICK, ACT_VIEW_MESSAGE, 
			ACT_VIEW_COMMENTS, ACT_VIEW_NUMBER, ACT_VIEW_C_FORMAT,
			ACT_VIEW_HOTKEY, ACT_EXPORT_UTF8);

	gtk_text_set_editable(GTK_TEXT(trans_box), FALSE);
}

void gtranslator_actions_set_up_file_opened(void)
{
	gtranslator_actions_enable(ACT_COMPILE, ACT_SAVE_AS, ACT_CLOSE,
		ACT_ACCOMPLISH, ACT_CUT, ACT_COPY, ACT_PASTE, ACT_CLEAR,
		ACT_REPLACE, ACT_FIND, ACT_HEADER, ACT_NEXT, ACT_LAST,
		ACT_QUERY, ACT_GOTO, ACT_FUZZY, ACT_TRANSLATED, ACT_STICK,
		ACT_VIEW_MESSAGE, ACT_VIEW_COMMENTS, ACT_VIEW_NUMBER, 
		ACT_VIEW_C_FORMAT, ACT_VIEW_HOTKEY,
		ACT_IMPORT_UTF8);

	gtranslator_actions_disable(ACT_SAVE, ACT_UNDO, ACT_REDO);
	
	/*
	 * If we'd have the option to use the update function set, enable the
	 *  Update button in the toolbar and in the menu.
	 */	       
	if(GtrPreferences.update_function)
	{
		gtranslator_actions_enable(ACT_UPDATE);	
	}

	/*
	 * Check if the current file is UTF-8 -- then disable the export
	 *  menu entry; it's already in UTF-8.
	 */
	if(po->utf8)
	{
		gtranslator_actions_disable(ACT_EXPORT_UTF8);
	}
	else
	{
		gtranslator_actions_enable(ACT_EXPORT_UTF8);
	}

	/*
	 * Enable the editing of the msgstrs :-)
	 */
	gtk_text_set_editable(GTK_TEXT(trans_box), TRUE);

	/*
	 * Make it focused initially
	 */
	gtk_window_set_focus(GTK_WINDOW(gtranslator_application), trans_box);
}

/*
 * The undo callback/function.
 */
void gtranslator_actions_undo(GtkWidget *widget, gpointer useless)
{
	gtranslator_message_show(po->current);
	gtranslator_actions_swap(ACT_UNDO, ACT_REDO);
}

/*
 * The redo callback/function.
 */
void gtranslator_actions_redo(GtkWidget *widget, gpointer useless)
{
	gtranslator_actions_swap(ACT_REDO, ACT_UNDO);
}

/*
 * Disable the corresponding menu item.
 */
void gtranslator_actions_disable_fuzzy_menu_item(void)
{
	gtk_check_menu_item_set_active((GtkCheckMenuItem *) 
		acts[ACT_FUZZY].menu, FALSE);
}

