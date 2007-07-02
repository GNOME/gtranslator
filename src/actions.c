/*
 * (C) 2001-2007 	Fatih Demir <kabalak@kabalak.net>
 *			Gediminas Paulauskas <menesis@kabalak.net>
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
#include "menus.h"
#include "message.h"
#include "page.h"
#include "prefs.h"
#include "undo.h"

#include <gtk/gtk.h>


typedef struct _GtrAction GtrAction;
#define GTR_ACTION(x) ((GtrAction *)x)

struct _GtrAction {
	GtkWidget *menu;
	GtkWidget *tool;
};

/*
 * An array holds all defined actions.
 */
static GtrAction acts[ACT_END];

/*
 * Routines for actions.
 */
//static void insert_action(gint act_num, GnomeUIInfo mi, GnomeUIInfo ti);


/****
  * Actions stuff goes here
 ** * */
void gtranslator_actions_set_up(gboolean state, ...)
{
	/*va_list ap;
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
	
	guint n_elements;
	gint i;
	GtkToolItem *item;
	n_elements = gtk_toolbar_get_n_items(GTK_TOOLBAR(gtranslator_toolbar));
	for(i = 1; i < n_elements; i++)
	{
		item = gtk_toolbar_get_nth_item(GTK_TOOLBAR(gtranslator_toolbar), i);
		gtk_widget_set_sensitive(GTK_WIDGET(item), state);
	}*/
}

/*static void insert_action(gint act_num, GnomeUIInfo mi, GnomeUIInfo ti)
{
	acts[act_num].menu = mi.widget;
	acts[act_num].tool = ti.widget;
}*/

void gtranslator_actions_set_up_default()
{
	/*
	 * A unused variable for testing if a toolbar element was provided
	 */
/*	GnomeUIInfo NONE;

	NONE.widget = NULL;
	insert_action(ACT_COMPILE, the_file_menu[0], the_toolbar[4]);
	insert_action(ACT_UPDATE, the_file_menu[1], the_toolbar[5]);
	insert_action(ACT_ADD_BOOKMARK, the_file_menu[3], NONE);
	insert_action(ACT_AUTOTRANSLATE, the_file_menu[5], NONE);
	insert_action(ACT_REMOVE_ALL_TRANSLATIONS, the_file_menu[7], NONE);
	insert_action(ACT_SAVE, the_file_menu[11], the_toolbar[1]);
	insert_action(ACT_SAVE_AS, the_file_menu[12], the_toolbar[2]);
	insert_action(ACT_REVERT, the_file_menu[13], NONE);
	insert_action(ACT_CLOSE, the_file_menu[14], NONE);*/
	/*----------------------------------------------------------*/
/*	insert_action(ACT_UNDO, the_edit_menu[0], the_toolbar[8]);
	insert_action(ACT_CUT, the_edit_menu[2], NONE);
	insert_action(ACT_COPY, the_edit_menu[3], NONE);
	insert_action(ACT_PASTE, the_edit_menu[4], NONE);
	insert_action(ACT_CLEAR, the_edit_menu[5], NONE);
	insert_action(ACT_FIND, the_edit_menu[7], the_navibar[8]);
	insert_action(ACT_FIND_AGAIN, the_edit_menu[8], NONE);
	insert_action(ACT_REPLACE, the_edit_menu[9], the_navibar[9]);
	insert_action(ACT_QUERY, the_edit_menu[11], the_navibar[10]);
	insert_action(ACT_HEADER, the_edit_menu[13], the_toolbar[6]);
	insert_action(ACT_COMMENT, the_edit_menu[15], NONE);
	insert_action(ACT_COPY_MSGID2MSGSTR, the_edit_menu[17], NONE);
	insert_action(ACT_FUZZY, the_edit_menu[19], NONE);*/
	/*-----------------------------------------------------------*/
/*	insert_action(ACT_FIRST, the_go_menu[0], the_navibar[0]);
	insert_action(ACT_BACK, the_go_menu[1], the_navibar[1]);
	insert_action(ACT_NEXT, the_go_menu[3], the_navibar[3]);
	insert_action(ACT_LAST, the_go_menu[4], the_navibar[4]);
	insert_action(ACT_GOTO, the_go_menu[6], the_navibar[7]);
	insert_action(ACT_NEXT_FUZZY, the_go_menu[7], the_navibar[6]);
	insert_action(ACT_NEXT_UNTRANSLATED, the_go_menu[8], the_navibar[5]);*/
	/*-----------------------------------------------------------*/
/*	insert_action(ACT_END, NONE, NONE);*/
}

void gtranslator_actions_set_up_state_no_file(void)
{
	//File
	gtk_widget_set_sensitive(gtranslator_menuitems->compile, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->refresh, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->add_bookmark, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->autotranslate, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->remove_translations, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->save, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->save_as, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->revert, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->close, FALSE);
	//Edit
	gtk_widget_set_sensitive(gtranslator_menuitems->undo, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->cut, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->copy, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->paste, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->clear, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->find, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->search_next, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->replace, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->header, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->comment, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->copy_message, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->fuzzy, FALSE);
	//Go
	gtk_widget_set_sensitive(gtranslator_menuitems->first, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->go_back, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->go_forward, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->goto_last, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->jump_to, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->next_fuzzy, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->next_untranslated, FALSE);
	
	//Toolbar
	/*guint n_elements;
	gint i;
	GtkToolItem *item;
	n_elements = gtk_toolbar_get_n_items(GTK_TOOLBAR(gtranslator_toolbar));
	for(i = 1; i < n_elements; i++)
	{
		item = gtk_toolbar_get_nth_item(GTK_TOOLBAR(gtranslator_toolbar), i);
		gtk_widget_set_sensitive(GTK_WIDGET(item), FALSE);
	}*/

#ifdef DONTFORGET
	gtk_text_view_set_editable(GTK_TEXT_VIEW(trans_box), FALSE);
#endif
}

void gtranslator_actions_set_up_file_opened(void)
{	
	//File
	gtk_widget_set_sensitive(gtranslator_menuitems->compile, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->refresh, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->add_bookmark, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->autotranslate, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->remove_translations, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->save, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->save_as, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->revert, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->close, TRUE);
	//Edit
	gtk_widget_set_sensitive(gtranslator_menuitems->undo, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->cut, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->copy, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->paste, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->clear, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->find, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->search_next, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->replace, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->header, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->comment, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->copy_message, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->fuzzy, TRUE);
	//Go
	gtk_widget_set_sensitive(gtranslator_menuitems->first, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->go_back, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->go_forward, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->goto_last, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->jump_to, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->next_fuzzy, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->next_untranslated, TRUE);
	
	//Toolbar
	guint n_elements;
	gint i;
	GtkToolItem *item;
	n_elements = gtk_toolbar_get_n_items(GTK_TOOLBAR(gtranslator_toolbar));
	for(i = 2; i < n_elements; i++)
	{
		//if not undo, first and go_back
		//if(i != 6 && i != 8 && i != 9)
		if(i != 6){
			item = gtk_toolbar_get_nth_item(GTK_TOOLBAR(gtranslator_toolbar), i);
			gtk_widget_set_sensitive(GTK_WIDGET(item), TRUE);
		}
	}
	
	/*
	 * If we'd have the option to use the update function set, enable the
	 *  Update button in the toolbar and in the menu.
	 */	       
	if(GtrPreferences.update_function)
	{
		gtk_widget_set_sensitive(gtranslator_menuitems->refresh, TRUE);
	}

#ifdef DONTFORGET
	/*
	 * Enable the editing of the msgstrs :-)
	 */
	gtk_text_view_set_editable(GTK_TEXT_VIEW(trans_box), TRUE);

	/*
	 * Make it focused initially
	 */
	gtk_window_set_focus(GTK_WINDOW(gtranslator_application), GTK_WIDGET(trans_box));
#endif
}

/*
 * The undo callback/function.
 */
void gtranslator_actions_undo(GtkWidget *widget, gpointer useless)
{
	g_debug("Undo called");
	
	/*
	 * If we have registered any internal "cleverer" Undo, run it -- else
	 *  do what we're doing for more then one year now ,-)
	 */
	if(gtranslator_undo_get_if_registered_undo())
	{
		gtranslator_undo_run_undo();
	}
	else
	{
		gtranslator_message_show(current_page->po->current->data);
	}

	gtk_widget_set_sensitive(gtranslator_menuitems->undo, FALSE);
    	gtk_widget_set_sensitive(gtranslator_menuitems->t_undo, FALSE);
}
