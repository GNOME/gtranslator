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

#include "about.h"
#include "actions.h"
#include "defines.include"
#include "dialogs.h"
#include "find.h"
#include "gui.h"
#include "menus.h"
#include "message.h"
#include "parse.h"
#include "prefs.h"
#include "utils_gui.h"

#include "pixmaps/auto_translation.xpm"
#include "pixmaps/copy_msgid2msgstr.xpm"
#include "pixmaps/edit_comment.xpm"
#include "pixmaps/query.xpm"
#include "pixmaps/query_big.xpm"

#include <gdk/gdkkeysyms.h>

#include <libgnomeui/gnome-app.h>
#include <libgnomeui/gnome-app-helper.h>
#include <libgnomeui/gnome-stock.h>

GnomeUIInfo the_menus[] = {
	GNOMEUIINFO_MENU_FILE_TREE(the_file_menu),
	GNOMEUIINFO_MENU_EDIT_TREE(the_edit_menu),
	GNOMEUIINFO_SUBTREE(N_("_Messages"), the_messages_menu),
	GNOMEUIINFO_SUBTREE(N_("Mess_age status"), the_msg_status_menu),
	GNOMEUIINFO_MENU_SETTINGS_TREE(the_settings_menu),
	GNOMEUIINFO_MENU_HELP_TREE(the_help_menu),
	GNOMEUIINFO_END
};

/*
 * The File menu.
 */
GnomeUIInfo the_file_menu[] = {
	{
		GNOME_APP_UI_ITEM, N_("_Compile"),
		N_("Compile the po file"),
		compile, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_CONVERT,
		GDK_C, GDK_MOD1_MASK, NULL
	},
	{
		GNOME_APP_UI_ITEM, N_("_Update"),
		N_("Update the po file"),
		update, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_REFRESH,
		GDK_F5, 0, NULL
	},
	GNOMEUIINFO_SEPARATOR,
	{
		GNOME_APP_UI_ITEM, N_("Aut_otranslate..."),
		N_("Autotranslate the file with information from the default query domain/learn buffer"),
		gtranslator_auto_translation_dialog, NULL, NULL,
		GNOME_APP_PIXMAP_DATA, auto_translation_xpm,
		GDK_F10, 0, NULL
	},
	GNOMEUIINFO_SEPARATOR,
	{
		GNOME_APP_UI_ITEM, N_("Remove all translations..."),
		N_("Remove all existing translations from the po file"),
		gtranslator_remove_all_translations_dialog, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_TRASH,
		0, 0, NULL
	},
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_OPEN_ITEM(gtranslator_open_file_dialog, NULL),
	{
		GNOME_APP_UI_ITEM, N_("Open from _URI..."),
		N_("Open a po file from a given URI"),
		gtranslator_open_uri_dialog, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_OPEN,
		GDK_F3, GDK_MOD1_MASK, NULL
	},
	GNOMEUIINFO_MENU_SAVE_ITEM(gtranslator_save_current_file_dialog, NULL),
	GNOMEUIINFO_MENU_SAVE_AS_ITEM(gtranslator_save_file_as_dialog, NULL),
	GNOMEUIINFO_MENU_REVERT_ITEM(gtranslator_file_revert, NULL),
	GNOMEUIINFO_MENU_CLOSE_ITEM(gtranslator_file_close, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_SUBTREE(N_("Recen_t files"), the_last_files_menus),
	GNOMEUIINFO_SEPARATOR,
	{
		GNOME_APP_UI_ITEM, N_("_Export to UTF-8..."),
		N_("Export the current po file to UTF-8"),
		gtranslator_export_dialog, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_YELLOW,
		GDK_E, GDK_MOD1_MASK, NULL
	},
	{
		GNOME_APP_UI_ITEM, N_("_Import from UTF-8..."),
		N_("Import a UTF-8 encoded po file into it's corresponding plain encoding"),
		gtranslator_import_dialog, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_RED,
		GDK_I, GDK_MOD1_MASK, NULL
	},
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_EXIT_ITEM(gtranslator_quit, NULL),
	GNOMEUIINFO_END
};

/*
 * The recenlty used menu in a little bit different manner ( this is just
 *  a placeholder.
 */
GnomeUIInfo the_last_files_menus[] = {
        GNOMEUIINFO_END
};

GnomeUIInfo the_edit_menu[] = {
	GNOMEUIINFO_MENU_UNDO_ITEM(gtranslator_actions_undo, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_CUT_ITEM(gtranslator_clipboard_cut, NULL),
	GNOMEUIINFO_MENU_COPY_ITEM(gtranslator_clipboard_copy, NULL),
	GNOMEUIINFO_MENU_PASTE_ITEM(gtranslator_clipboard_paste, NULL),
	GNOMEUIINFO_MENU_CLEAR_ITEM(gtranslator_selection_clear, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_FIND_ITEM(gtranslator_find_dialog, NULL),
	GNOMEUIINFO_MENU_FIND_AGAIN_ITEM(gtranslator_find, NULL),
	GNOMEUIINFO_MENU_REPLACE_ITEM(gtranslator_replace_dialog, NULL),
	{
		GNOME_APP_UI_ITEM, N_("_Query..."),
		N_("Query for a string"),
		gtranslator_query_dialog, NULL, NULL,
		GNOME_APP_PIXMAP_DATA, query_xpm,
		GDK_F7, 0, NULL
	},
	GNOMEUIINFO_SEPARATOR,
	{
		GNOME_APP_UI_ITEM, N_("_Header..."),
		N_("Edit po file header"),
		gtranslator_header_edit_dialog, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_PROP,
		GDK_F8, 0, NULL
	},
	GNOMEUIINFO_SEPARATOR,
	{
		GNOME_APP_UI_ITEM, N_("C_omment..."),
		N_("Edit message comment"),
		gtranslator_edit_comment_dialog, NULL, NULL,
		GNOME_APP_PIXMAP_DATA, edit_comment_xpm,
		GDK_F9, 0, NULL
	},
	GNOMEUIINFO_SEPARATOR,
	{
		GNOME_APP_UI_ITEM, N_("Copy _message -> translation"),
		N_("Copy the original message contents and paste them as translation"),
		gtranslator_message_status_set_sticky, NULL, NULL,
		GNOME_APP_PIXMAP_DATA, copy_msgid2msgstr_xpm,
		GDK_F11, 0, NULL
	},
	GNOMEUIINFO_END
};

GnomeUIInfo the_messages_menu[] = {
	{
	 GNOME_APP_UI_ITEM, N_("_First"),
	 N_("Go to the first message"),
	 gtranslator_message_go_to_first, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_FIRST,
	 GDK_Up, GDK_CONTROL_MASK, NULL
	},
	{
	 GNOME_APP_UI_ITEM, N_("_Back"),
	 N_("Move back one message"),
	 gtranslator_message_go_to_previous, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BACK,
	 GDK_Left, GDK_CONTROL_MASK, NULL
	},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("_Next"),
	 N_("Move forward one message"),
	 gtranslator_message_go_to_next, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_FORWARD,
	 GDK_Right, GDK_CONTROL_MASK, NULL
	},
	{
	 GNOME_APP_UI_ITEM, N_("_Last"),
	 N_("Go to the last message"),
	 gtranslator_message_go_to_last, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_LAST,
	 GDK_Down, GDK_CONTROL_MASK, NULL
	},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("_Go to..."),
	 N_("Goto specified message number"),
	 gtranslator_go_to_dialog, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_JUMP_TO,
	 'G', GDK_CONTROL_MASK, NULL
	},
	{
	 GNOME_APP_UI_ITEM, N_("Next fuz_zy"),
	 N_("Go to next fuzzy message"),
	 gtranslator_message_go_to_next_fuzzy, NULL, NULL,
	 GNOME_APP_PIXMAP_FILENAME, "gtranslator/fuzzy_small.png",
	 'Z', GDK_MOD1_MASK, NULL
	},
	{
	 GNOME_APP_UI_ITEM, N_("Next _untranslated"),
	 N_("Go to next untranslated message"),
	 gtranslator_message_go_to_next_untranslated, NULL, NULL,
	 GNOME_APP_PIXMAP_FILENAME, "gtranslator/untranslated_small.png",
	 'U', GDK_MOD1_MASK, NULL
	},
	GNOMEUIINFO_END
};

GnomeUIInfo the_msg_status_menu[] = {
	{
		GNOME_APP_UI_TOGGLEITEM, N_("_Translated"),
		N_("Toggle translated status of a message"),
		gtranslator_message_change_status,
		GINT_TO_POINTER(GTR_MSG_STATUS_TRANSLATED),
		NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_GREEN,
		GDK_1, GDK_MOD1_MASK, NULL
	},
	{
		GNOME_APP_UI_TOGGLEITEM, N_("_Fuzzy"),
		N_("Toggle fuzzy status of a message"),
		gtranslator_message_change_status,
		GINT_TO_POINTER(GTR_MSG_STATUS_FUZZY),
		NULL,
		GNOME_APP_PIXMAP_FILENAME, "gtranslator/fuzzy_small.png",
		GDK_2, GDK_MOD1_MASK, NULL
	},
	GNOMEUIINFO_END
};

GnomeUIInfo the_settings_menu[] = {
	GNOMEUIINFO_MENU_PREFERENCES_ITEM(gtranslator_preferences_dialog_create, NULL),
	GNOMEUIINFO_END
};

GnomeUIInfo the_help_menu[] = {
	GNOMEUIINFO_HELP("gtranslator"),
	GNOMEUIINFO_ITEM_STOCK(N_("gtranslator _website"),
			       N_("gtranslator's homepage on the web"),
			       gtranslator_utils_show_home_page,
			       GNOME_STOCK_MENU_HOME),
	GNOMEUIINFO_MENU_ABOUT_ITEM(gtranslator_about_dialog, NULL),
	GNOMEUIINFO_END
};

/*
 * The toolbar buttons
 */
GnomeUIInfo the_toolbar[] = {
	GNOMEUIINFO_ITEM_STOCK(N_("Open"),
			       N_("Open a po file"),
			       gtranslator_open_file_dialog,
			       GNOME_STOCK_PIXMAP_OPEN),
	GNOMEUIINFO_ITEM_STOCK(N_("Save"),
			       N_("Save File"),
			       gtranslator_save_current_file_dialog,
			       GNOME_STOCK_PIXMAP_SAVE),
	GNOMEUIINFO_ITEM_STOCK(N_("Save as"),
			       N_("Save file with a different name"),
			       gtranslator_save_file_as_dialog,
			       GNOME_STOCK_PIXMAP_SAVE_AS),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(N_("Compile"),
			       N_("Compile the po file"),
			       compile,
			       GNOME_STOCK_PIXMAP_CONVERT),
	GNOMEUIINFO_ITEM_STOCK(N_("Update"),
			       N_("Update the po file"),
			       update,
			       GNOME_STOCK_PIXMAP_REFRESH),
	GNOMEUIINFO_ITEM_STOCK(N_("Header"),
			       N_("Edit the header"),
			       gtranslator_header_edit_dialog,
			       GNOME_STOCK_PIXMAP_PROPERTIES),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(N_("Undo"),
			       N_("Undo the last performed action"),
			       gtranslator_actions_undo,
			       GNOME_STOCK_PIXMAP_UNDO),
	GNOMEUIINFO_END
};

GnomeUIInfo the_navibar[] = {
	GNOMEUIINFO_ITEM_STOCK(N_("First"),
			       N_("Go to the first message"),
			       gtranslator_message_go_to_first,
			       GNOME_STOCK_PIXMAP_FIRST),
	GNOMEUIINFO_ITEM_STOCK(N_("Back"),
			       N_("Move back one message"),
			       gtranslator_message_go_to_previous,
			       GNOME_STOCK_PIXMAP_BACK),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(N_("Next"),
			       N_("Move forward one message"),
			       gtranslator_message_go_to_next,
			       GNOME_STOCK_PIXMAP_FORWARD),
	GNOMEUIINFO_ITEM_STOCK(N_("Last"),
			       N_("Go to the last message"),
			       gtranslator_message_go_to_last,
			       GNOME_STOCK_PIXMAP_LAST),
	{
		GNOME_APP_UI_ITEM, N_("Missing"),
		N_("Go to next untranslated message"),
		gtranslator_message_go_to_next_untranslated, NULL, NULL,
		GNOME_APP_PIXMAP_FILENAME, "gtranslator/untranslated.png",
		0, 0, NULL
	},
	{
		GNOME_APP_UI_ITEM, N_("Fuzzy"),
		N_("Go to the next fuzzy translation"),
		gtranslator_message_go_to_next_fuzzy, NULL, NULL,
		GNOME_APP_PIXMAP_FILENAME, "gtranslator/fuzzy.png",
		0, 0, NULL
	},
	GNOMEUIINFO_ITEM_STOCK(N_("Go to"),
			       N_("Go to specified message number"),
			       gtranslator_go_to_dialog,
			       GNOME_STOCK_PIXMAP_JUMP_TO),
	GNOMEUIINFO_ITEM_STOCK(N_("Find"),
			       N_("Find string in po file"),
			       gtranslator_find_dialog,
			       GNOME_STOCK_PIXMAP_SEARCH),
	GNOMEUIINFO_ITEM_STOCK(N_("Replace"),
			       N_("Replace string in po file"),
			       gtranslator_replace_dialog,
			       GNOME_STOCK_PIXMAP_SRCHRPL),
	{
		GNOME_APP_UI_ITEM, N_("Query"),
		N_("Query for a string"),
		gtranslator_query_dialog, NULL, NULL,
		GNOME_APP_PIXMAP_DATA, query_big_xpm,
		0, 0, NULL
	},
	GNOMEUIINFO_END
};

/*
 * The popup-menu for the main GUI.
 */
GnomeUIInfo the_main_popup_menu[] = {
	GNOMEUIINFO_MENU_OPEN_ITEM(gtranslator_open_file_dialog, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_CUT_ITEM(gtranslator_clipboard_cut, NULL),
	GNOMEUIINFO_MENU_COPY_ITEM(gtranslator_clipboard_copy, NULL),
	GNOMEUIINFO_MENU_PASTE_ITEM(gtranslator_clipboard_paste, NULL),
	GNOMEUIINFO_MENU_CLEAR_ITEM(gtranslator_selection_clear, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_CLOSE_ITEM(gtranslator_file_close, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_SAVE_AS_ITEM(gtranslator_save_file_as_dialog, NULL),
	GNOMEUIINFO_END
};
