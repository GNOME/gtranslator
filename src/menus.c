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

#include "about.h"
#include "actions.h"
#include "compile.h"
#include "dialogs.h"
#include "find.h"
#include "gui.h"
#include "header_stuff.h"
#include "menus.h"
#include "message.h"
#include "parse.h"
#include "prefs.h"
#include "utils_gui.h"

#include <gdk/gdkkeysyms.h>
#include <glade/glade-xml.h>

//Struct with all menuitem widgets
GtrMenuItems *gtranslator_menuitems;

/*
 * This func is only to store all widgets to use it later
 * to set the sensitive (gtk_widget_set_sensitive)
 */
void
gtranslator_menuitems_set_up()
{
	GtrMenuItems *this;
	this = g_new0(GtrMenuItems, 1);
	
	//File
	this->open = glade_xml_get_widget(glade, "open");
	this->open_uri = glade_xml_get_widget(glade, "open_uri");
	this->save = glade_xml_get_widget(glade, "save");
	this->save_as = glade_xml_get_widget(glade, "save_as");
	this->revert = glade_xml_get_widget(glade, "revert");
	this->close = glade_xml_get_widget(glade, "close");
	this->recent_files = glade_xml_get_widget(glade, "recent_files");
	this->quit = glade_xml_get_widget(glade, "quit");
	
	//Edit
	this->undo = glade_xml_get_widget(glade, "undo");
	this->cut = glade_xml_get_widget(glade, "cut");
	this->copy = glade_xml_get_widget(glade, "copy");
	this->paste = glade_xml_get_widget(glade, "paste");
	this->clear = glade_xml_get_widget(glade, "clear");
	this->find = glade_xml_get_widget(glade, "find");
	this->search_next = glade_xml_get_widget(glade, "search_next");
	this->replace = glade_xml_get_widget(glade, "replace");
	this->header = glade_xml_get_widget(glade, "header");
	this->comment = glade_xml_get_widget(glade, "comment_item");
	this->copy_message = glade_xml_get_widget(glade, "copy_message");
	this->fuzzy = glade_xml_get_widget(glade, "fuzzy");
	this->preferences = glade_xml_get_widget(glade, "preferences");
	
	//Actions
	this->compile = glade_xml_get_widget(glade, "compile");
	this->refresh = glade_xml_get_widget(glade, "refresh");
	this->add_bookmark = glade_xml_get_widget(glade, "add_bookmark");
	this->autotranslate = glade_xml_get_widget(glade, "autotranslate");
	this->remove_translations = glade_xml_get_widget(glade, "remove_translations");
	
	//View
	this->bookmarks = glade_xml_get_widget(glade, "bookmarks");
	this->colorschemes = glade_xml_get_widget(glade, "colorschemes");
	
	//Go
	this->first = glade_xml_get_widget(glade, "first");
	this->go_back = glade_xml_get_widget(glade, "go_back");
	this->go_forward = glade_xml_get_widget(glade, "go_forward");
	this->goto_last = glade_xml_get_widget(glade, "goto_last");
	this->jump_to = glade_xml_get_widget(glade, "jump_to");
	this->next_fuzzy = glade_xml_get_widget(glade, "next_fuzzy");
	this->previously_fuzzy = glade_xml_get_widget(glade, "previously_fuzzy");
	this->next_untranslated = glade_xml_get_widget(glade, "next_untranslated");
	this->previously_untranslated = glade_xml_get_widget(glade, "previously_untranslated");
	
	//About
	this->contents = glade_xml_get_widget(glade, "contents");
	this->website = glade_xml_get_widget(glade, "website");
	this->about = glade_xml_get_widget(glade, "about");
	
	//Toolbar
	this->t_save = glade_xml_get_widget(glade, "save_toolbar");
	this->t_undo = glade_xml_get_widget(glade, "undo_toolbar");
    	this->t_first = glade_xml_get_widget(glade, "goto_first_toolbar");
    	this->t_go_back = glade_xml_get_widget(glade, "go_back_toolbar");
    	this->t_go_forward = glade_xml_get_widget(glade, "go_forward_toolbar");
    	this->t_goto_last = glade_xml_get_widget(glade, "goto_last_toolbar");
	
	gtranslator_menuitems = this;
}

void
connect_menu_signals()
{
	/*********************** File menu **************************/
	
	//Open item
	glade_xml_signal_connect_data(  glade, "on_open_activate",
					G_CALLBACK(gtranslator_open_file_dialog), NULL );
	glade_xml_signal_connect_data(  glade, "on_open_select",
					G_CALLBACK(push_statusbar_data), 
				      _("Open a po file") );
	glade_xml_signal_connect_data(  glade, "on_open_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
					
	//Open from URI item
	glade_xml_signal_connect_data(  glade, "on_open_uri_activate",
					G_CALLBACK(gtranslator_open_uri_dialog), NULL );
	glade_xml_signal_connect_data(  glade, "on_open_uri_select",
					G_CALLBACK(push_statusbar_data),
				      _("Open a po file from a given URI") );
	glade_xml_signal_connect_data(  glade, "on_open_uri_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	//Save item
	glade_xml_signal_connect_data(  glade, "on_save_activate",
					G_CALLBACK(gtranslator_save_current_file_dialog), NULL );
	glade_xml_signal_connect_data(  glade, "on_save_select",
					G_CALLBACK(push_statusbar_data), 
				      _("Save the file") );
	glade_xml_signal_connect_data(  glade, "on_save_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
					
	//Save as item
	glade_xml_signal_connect_data(  glade, "on_save_as_activate",
					G_CALLBACK(gtranslator_save_file_as_dialog), NULL );
	glade_xml_signal_connect_data(  glade, "on_save_as_select",
					G_CALLBACK(push_statusbar_data),
				      _("Save the file with another name") );
	glade_xml_signal_connect_data(  glade, "on_save_as_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
					
	//Revert item
	glade_xml_signal_connect_data(  glade, "on_revert_activate",
					G_CALLBACK(gtranslator_file_revert), NULL );
	glade_xml_signal_connect_data(  glade, "on_revert_select",
					G_CALLBACK(push_statusbar_data), _("...") );
	glade_xml_signal_connect_data(  glade, "on_revert_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
					
	//Close item
	glade_xml_signal_connect_data(  glade, "on_close_activate",
					G_CALLBACK(gtranslator_file_close), NULL );
	glade_xml_signal_connect_data(  glade, "on_close_select",
					G_CALLBACK(push_statusbar_data), 
				      _("Close the current file") );
	glade_xml_signal_connect_data(  glade, "on_close_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
					
	//Quit item
	glade_xml_signal_connect_data(  glade, "on_quit_activate",
					G_CALLBACK(gtranslator_menu_quit_cb), NULL );
	glade_xml_signal_connect_data(  glade, "on_quit_select",
					G_CALLBACK(push_statusbar_data),
				      _("Quit the program") );
	glade_xml_signal_connect_data(  glade, "on_quit_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
					
	/*********************** Edit menu **************************/
	
	//Undo item
	glade_xml_signal_connect_data(  glade, "on_undo_activate",
					G_CALLBACK(gtranslator_actions_undo), NULL );
	glade_xml_signal_connect_data(  glade, "on_undo_select",
					G_CALLBACK(push_statusbar_data),
				      _("Undo last operation") );
	glade_xml_signal_connect_data(  glade, "on_undo_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	//Cut item
	glade_xml_signal_connect_data(  glade, "on_cut_activate",
					G_CALLBACK(gtranslator_clipboard_cut), NULL );
	glade_xml_signal_connect_data(  glade, "on_cut_select",
					G_CALLBACK(push_statusbar_data),
				      _("Cut the selected text") );
	glade_xml_signal_connect_data(  glade, "on_cut_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );

	//Copy item
	glade_xml_signal_connect_data(  glade, "on_copy_activate",
					G_CALLBACK(gtranslator_clipboard_copy), NULL );
	glade_xml_signal_connect_data(  glade, "on_copy_select",
					G_CALLBACK(push_statusbar_data),
				      _("Copy the selected text") );
	glade_xml_signal_connect_data(  glade, "on_copy_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	//Paste item
	glade_xml_signal_connect_data(  glade, "on_paste_activate",
					G_CALLBACK(gtranslator_clipboard_paste), NULL );
	glade_xml_signal_connect_data(  glade, "on_paste_select",
					G_CALLBACK(push_statusbar_data),
				      _("") );
	glade_xml_signal_connect_data(  glade, "on_paste_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	//Clear item
	glade_xml_signal_connect_data(  glade, "on_clear_activate",
					G_CALLBACK(gtranslator_selection_clear), NULL );
	glade_xml_signal_connect_data(  glade, "on_clear_select",
					G_CALLBACK(push_statusbar_data),
				      _("Clear the translation text box") );
	glade_xml_signal_connect_data(  glade, "on_clear_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
			
	//Find item		
	glade_xml_signal_connect_data(  glade, "on_find_activate",
					G_CALLBACK(gtranslator_find_dialog), NULL );
	glade_xml_signal_connect_data(  glade, "on_find_select",
					G_CALLBACK(push_statusbar_data),
				      _("") );
	glade_xml_signal_connect_data(  glade, "on_find_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	//Search next item			
	glade_xml_signal_connect_data(  glade, "on_search_next_activate",
					G_CALLBACK(gtranslator_find), NULL );
	glade_xml_signal_connect_data(  glade, "on_search_next_select",
					G_CALLBACK(push_statusbar_data),
				      _("") );
	glade_xml_signal_connect_data(  glade, "on_search_next_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	//Find and replace item
	glade_xml_signal_connect_data(  glade, "on_replace_activate",
					G_CALLBACK(gtranslator_replace_dialog), NULL );
	glade_xml_signal_connect_data(  glade, "on_replace_select",
					G_CALLBACK(push_statusbar_data),
				      _("") );
	glade_xml_signal_connect_data(  glade, "on_replace_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	//Header item	
	glade_xml_signal_connect_data(  glade, "on_header_activate",
					G_CALLBACK(gtranslator_header_edit_dialog), NULL );
	glade_xml_signal_connect_data(  glade, "on_header_select",
					G_CALLBACK(push_statusbar_data),
				      _("Edit po file header") );
	glade_xml_signal_connect_data(  glade, "on_header_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	//Comment item		
	glade_xml_signal_connect_data(  glade, "on_comment_item_activate",
					G_CALLBACK(gtranslator_edit_comment_dialog), NULL );
	glade_xml_signal_connect_data(  glade, "on_comment_item_select",
					G_CALLBACK(push_statusbar_data),
				      _("Edit message comment") );
	glade_xml_signal_connect_data(  glade, "on_comment_item_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	//Copy _message -> translation item
	glade_xml_signal_connect_data(  glade, "on_copy_message_activate",
					G_CALLBACK(gtranslator_message_copy_to_translation), NULL );
	glade_xml_signal_connect_data(  glade, "on_copy_message_select",
					G_CALLBACK(push_statusbar_data),
				      _("Copy the original message contents and paste them as translation") );
	glade_xml_signal_connect_data(  glade, "on_copy_message_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
					
	//Fuzzy item
	glade_xml_signal_connect_data(  glade, "on_fuzzy_activate",
					G_CALLBACK(gtranslator_message_status_toggle_fuzzy), NULL );
	glade_xml_signal_connect_data(  glade, "on_fuzzy_select",
					G_CALLBACK(push_statusbar_data),
				      _("Toggle fuzzy status of a message") );
	glade_xml_signal_connect_data(  glade, "on_fuzzy_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	//Preferences item
	glade_xml_signal_connect_data(  glade, "on_preferences_activate",
					G_CALLBACK(gtranslator_preferences_dialog_create), NULL );
	glade_xml_signal_connect_data(  glade, "on_preferences_select",
					G_CALLBACK(push_statusbar_data),
				      _("Edit gtranslator preferences") );
	glade_xml_signal_connect_data(  glade, "on_preferences_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
					
	/*********************** Actions menu **************************/
	
	//Compile item
	glade_xml_signal_connect_data(  glade, "on_compile_activate",
					G_CALLBACK(compile), NULL );
	glade_xml_signal_connect_data(  glade, "on_compile_select",
					G_CALLBACK(push_statusbar_data),
				      _("Compile the po file") );
	glade_xml_signal_connect_data(  glade, "on_compile_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	//Refresh item
	//glade_xml_signal_connect_data(  glade, "on_refresh_activate",
	//				G_CALLBACK(), NULL );
	
	//Add bookmark item
	glade_xml_signal_connect_data(  glade, "on_add_bookmark_activate",
					G_CALLBACK(gtranslator_bookmark_adding_dialog), NULL );
	glade_xml_signal_connect_data(  glade, "on_add_bookmark_select",
					G_CALLBACK(push_statusbar_data),
				      _("Add a bookmark for this message in this po file") );
	glade_xml_signal_connect_data(  glade, "on_add_bookmark_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
					
	//Autotranslate item
	glade_xml_signal_connect_data(  glade, "on_autotranslate_activate",
					G_CALLBACK(gtranslator_auto_translation_dialog), NULL );
	glade_xml_signal_connect_data(  glade, "on_autotranslate_select",
					G_CALLBACK(push_statusbar_data),
				      _("Autotranslate the file with information from your learn buffer") );
	glade_xml_signal_connect_data(  glade, "on_autotranslate_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
					
	//Remove all translations item
	glade_xml_signal_connect_data(  glade, "on_remove_translations_activate",
					G_CALLBACK(gtranslator_remove_all_translations_dialog), NULL );
	glade_xml_signal_connect_data(  glade, "on_remove_translations_select",
					G_CALLBACK(push_statusbar_data),
				      _("Remove all existing translations from the po file") );
	glade_xml_signal_connect_data(  glade, "on_remove_translations_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	/************************ Go menu ***************************/
	
	//First item
	glade_xml_signal_connect_data(  glade, "on_first_activate",
					G_CALLBACK(gtranslator_message_go_to_first), NULL );
	glade_xml_signal_connect_data(  glade, "on_first_select",
					G_CALLBACK(push_statusbar_data),
				      _("Go to first message") );
	glade_xml_signal_connect_data(  glade, "on_first_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	//Go back item
	glade_xml_signal_connect_data(  glade, "on_go_back_activate",
					G_CALLBACK(gtranslator_message_go_to_previous), NULL );
	glade_xml_signal_connect_data(  glade, "on_go_back_select",
					G_CALLBACK(push_statusbar_data),
				      _("Move back one message") );
	glade_xml_signal_connect_data(  glade, "on_go_back_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	//Go forward item
	glade_xml_signal_connect_data(  glade, "on_go_forward_activate",
					G_CALLBACK(gtranslator_message_go_to_next), NULL );
	glade_xml_signal_connect_data(  glade, "on_go_forward_select",
					G_CALLBACK(push_statusbar_data),
				      _("Move forward one message") );
	glade_xml_signal_connect_data(  glade, "on_go_forward_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	//Goto last item
	glade_xml_signal_connect_data(  glade, "on_goto_last_activate",
					G_CALLBACK(gtranslator_message_go_to_last), NULL );
	glade_xml_signal_connect_data(  glade, "on_goto_last_select",
					G_CALLBACK(push_statusbar_data),
				      _("Go to the last message") );
	glade_xml_signal_connect_data(  glade, "on_goto_last_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	//Jump to item
	glade_xml_signal_connect_data(  glade, "on_jump_to_activate",
					G_CALLBACK(gtranslator_go_to_dialog), NULL );
	glade_xml_signal_connect_data(  glade, "on_jump_to_select",
					G_CALLBACK(push_statusbar_data),
				      _("Go to especified message number") );
	glade_xml_signal_connect_data(  glade, "on_jump_to_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	//Next fuzzy item
	glade_xml_signal_connect_data(  glade, "on_next_fuzzy_activate",
					G_CALLBACK(gtranslator_message_go_to_next_fuzzy), NULL );
	glade_xml_signal_connect_data(  glade, "on_next_fuzzy_select",
					G_CALLBACK(push_statusbar_data),
				      _("Go to next fuzzy message") );
	glade_xml_signal_connect_data(  glade, "on_next_fuzzy_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	//Previously fuzzy item
	glade_xml_signal_connect_data(  glade, "on_previously_fuzzy_activate",
					G_CALLBACK(gtranslator_message_go_to_next_fuzzy),
				      NULL );
	glade_xml_signal_connect_data(  glade, "on_previously_fuzzy_select",
					G_CALLBACK(push_statusbar_data),
				      _("Go to previously fuzzy message") );
	glade_xml_signal_connect_data(  glade, "on_previously_fuzzy_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	//Next untranslated item
	glade_xml_signal_connect_data(  glade, "on_next_untranslated_activate",
					G_CALLBACK(gtranslator_message_go_to_next_untranslated), NULL );
	glade_xml_signal_connect_data(  glade, "on_next_untranslated_select",
					G_CALLBACK(push_statusbar_data),
				      _("Go to next untranslated message") );
	glade_xml_signal_connect_data(  glade, "on_next_untranslated_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	//Previously untranslated item
	glade_xml_signal_connect_data(  glade, "on_previously_untranslated_activate",
					G_CALLBACK(gtranslator_message_go_to_next_untranslated), NULL );
	glade_xml_signal_connect_data(  glade, "on_previously_untranslated_select",
					G_CALLBACK(push_statusbar_data),
				      _("Go to previously untranslated message") );
	glade_xml_signal_connect_data(  glade, "on_previously_untranslated_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
	
	/*********************** Help menu **************************/
	
	//Contents item
/*	glade_xml_signal_connect_data(  glade, "on_contents_activate",
					G_CALLBACK(), NULL );*/
					
	//Website item
	glade_xml_signal_connect_data(  glade, "on_website_activate",
					G_CALLBACK(gtranslator_utils_show_home_page), NULL );
	glade_xml_signal_connect_data(  glade, "on_website_select",
					G_CALLBACK(push_statusbar_data),
				      _("gtranslator's homepage on the web") );
	glade_xml_signal_connect_data(  glade, "on_website_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
					
	//About item
	glade_xml_signal_connect_data(  glade, "on_about_activate",
					G_CALLBACK(gtranslator_about_dialog), NULL );
	glade_xml_signal_connect_data(  glade, "on_about_select",
					G_CALLBACK(push_statusbar_data),
				      _(" ") );
	glade_xml_signal_connect_data(  glade, "on_about_deselect",
					G_CALLBACK(pop_statusbar_data), NULL );
	
}
