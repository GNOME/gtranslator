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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dialogs.h"
#include "gui.h"
#include "prefs.h"
#include "parse.h"
#include "header_stuff.h"
#include "find.h"
#include "dnd.h"
#include "about.h"
#include "history.h"
#include "sidebar.h"
#include "stylistics.h"
#include "syntax.h"
#include "gtkspell.h"
#include "color-schemes.h"

#include "pixmaps/untrans.xpm"

#include <libgnomevfs/gnome-vfs-init.h>
#include <gal/e-paned/e-hpaned.h>

typedef struct _GtrAction GtrAction;
#define GTR_ACTION(x) ((GtrAction *)x)

struct _GtrAction {
	GtkWidget *menu;
	GtkWidget *tool;
};

/*
 * An array holds all defined actions
 */
static GtrAction acts[ACT_END];

/*
 * routines for actions
 */
static void create_actions(void);
static void insert_action(gint act_num, GnomeUIInfo mi, GnomeUIInfo ti);

/*
 * Callbacks for text operations
 */
static void cut_clipboard(GtkWidget  * widget, gpointer useless);
static void copy_clipboard(GtkWidget  * widget, gpointer useless);
static void paste_clipboard(GtkWidget  * widget, gpointer useless);
static void clear_selection(GtkWidget  * widget, gpointer useless);
static void undo_changes(GtkWidget  * widget, gpointer useless);

/*
 * Pops up a menu if needed
 */
static gint create_popup_menu(GtkText *widget, GdkEventButton *event, gpointer d);
	
static void invert_dot(gchar *str);
static void update_appbar(gint pos);
static void call_gtranslator_homepage(GtkWidget  * widget, gpointer useless);
static gint gtranslator_quit(GtkWidget  * widget, GdkEventAny  * e,
			     gpointer useless);

/*
 * To get the left/right moves from the cursor.
 */ 
static gint gtranslator_keyhandler(GtkWidget *widget, GdkEventKey *event);

/*
 * The target formats
 */
static  GtkTargetEntry dragtypes[] = {
	{ "text/uri-list", 0, TARGET_URI_LIST },
	{ "text/plain", 0, TARGET_NETSCAPE_URL },
	{ "text/plain", 0, TARGET_TEXT_PLAIN }
};

/*
 * The menu-entries
 */

/*
 * The recenlty used menu in a little bit different manner ( this is just
 *  a placeholder.
 */
static GnomeUIInfo the_last_files_menus[] = {
        GNOMEUIINFO_END
};

/*
 * The File menu.
 */
static GnomeUIInfo the_file_menu[] = {
	{
		GNOME_APP_UI_ITEM, N_("_Compile"),
		N_("Compile the po-file"),
		compile, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_CONVERT,
		GDK_C, GDK_MOD1_MASK, NULL
	},
	{
		GNOME_APP_UI_ITEM, N_("_Update"),
		N_("Update the po-file"),
		update, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_REFRESH,
		GDK_F5, 0, NULL
	},
	GNOMEUIINFO_SEPARATOR,
	{
		GNOME_APP_UI_ITEM, N_("Autoaccomplish"),
		N_("Automatically fill missing translations from the default query domain"),
		accomplish_dialog, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_INDEX,
		GDK_F10, 0, NULL
	},
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_OPEN_ITEM(open_file, NULL),
	{
		GNOME_APP_UI_ITEM, N_("Open from _URI"),
		N_("Open a po file from a given URI"),
		open_uri_dialog, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_OPEN,
		GDK_F3, GDK_MOD1_MASK, NULL
	},
	GNOMEUIINFO_MENU_SAVE_ITEM(save_current_file, NULL),
	GNOMEUIINFO_MENU_SAVE_AS_ITEM(save_file_as, NULL),
	GNOMEUIINFO_MENU_REVERT_ITEM(revert_file, NULL),
	GNOMEUIINFO_MENU_CLOSE_ITEM(close_file, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_SUBTREE( N_("Recen_t files"), the_last_files_menus),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_EXIT_ITEM(gtranslator_quit, NULL),
	GNOMEUIINFO_END
};

static GnomeUIInfo the_edit_menu[] = {
	GNOMEUIINFO_MENU_UNDO_ITEM(undo_changes, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_CUT_ITEM(cut_clipboard, NULL),
	GNOMEUIINFO_MENU_COPY_ITEM(copy_clipboard, NULL),
	GNOMEUIINFO_MENU_PASTE_ITEM(paste_clipboard, NULL),
	GNOMEUIINFO_MENU_CLEAR_ITEM(clear_selection, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_FIND_ITEM(find_dialog, NULL),
	GNOMEUIINFO_MENU_FIND_AGAIN_ITEM(find_do, NULL),
	{
		GNOME_APP_UI_ITEM, N_("_Query"),
		N_("Query for a string"),
		query_dialog, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_JUMP_TO,
		GDK_F7, 0, NULL
	},
	GNOMEUIINFO_SEPARATOR,
	{
		GNOME_APP_UI_ITEM, N_("_Header..."),
		N_("Edit the header"),
		edit_header, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_PROP,
		GDK_F8, 0, NULL
	},
	GNOMEUIINFO_END
};

static GnomeUIInfo the_messages_menu[] = {
	{
	 GNOME_APP_UI_ITEM, N_("_First"),
	 N_("Go to the first message"),
	 goto_first_msg, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_FIRST,
	 GDK_Up, GDK_CONTROL_MASK, NULL
	},
	{
	 GNOME_APP_UI_ITEM, N_("_Back"),
	 N_("Go one message back"),
	 goto_prev_msg, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BACK,
	 GDK_Left, GDK_CONTROL_MASK, NULL
	},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("_Next"),
	 N_("Go one message forward"),
	 goto_next_msg, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_FORWARD,
	 GDK_Right, GDK_CONTROL_MASK, NULL
	},
	{
	 GNOME_APP_UI_ITEM, N_("_Last"),
	 N_("Go to the last message"),
	 goto_last_msg, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_LAST,
	 GDK_Down, GDK_CONTROL_MASK, NULL
	},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("_Go to..."),
	 N_("Goto specified message number"),
	 goto_dlg, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_JUMP_TO,
	 'G', GDK_CONTROL_MASK, NULL
	},
	{
	 GNOME_APP_UI_ITEM, N_("Next fuz_zy"),
	 N_("Go to next fuzzy message"),
	 goto_next_fuzzy, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_RED,
	 'Z', GDK_MOD1_MASK, NULL
	},
	{
	 GNOME_APP_UI_ITEM, N_("Next _untranslated"),
	 N_("Go to next untranslated message"),
	 goto_next_untranslated, NULL, NULL,
	 GNOME_APP_PIXMAP_DATA, untrans_xpm,
	 'U', GDK_MOD1_MASK, NULL
	},
	GNOMEUIINFO_END
};

static GnomeUIInfo the_msg_status_menu[] = {
	{
		GNOME_APP_UI_TOGGLEITEM, N_("_Translated"),
		N_("Toggle translated status of a message"),
		toggle_msg_status,
		GINT_TO_POINTER(GTR_MSG_STATUS_TRANSLATED),
		NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_GREEN,
		GDK_1, GDK_MOD1_MASK, NULL
	},
	{
		GNOME_APP_UI_TOGGLEITEM, N_("_Fuzzy"),
		N_("Toggle fuzzy status of a message"),
		toggle_msg_status,
		GINT_TO_POINTER(GTR_MSG_STATUS_FUZZY),
		NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_RED,
		GDK_2, GDK_MOD1_MASK, NULL
	},
	{
		GNOME_APP_UI_TOGGLEITEM, N_("_Stick"),
		N_("Stick this message"),
		toggle_msg_status,
		GINT_TO_POINTER(GTR_MSG_STATUS_STICK),
		NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_COPY,
		GDK_3, GDK_MOD1_MASK, NULL
	},
	GNOMEUIINFO_END
};

static GnomeUIInfo the_settings_menu[] = {
	GNOMEUIINFO_MENU_PREFERENCES_ITEM(prefs_box, NULL),
	GNOMEUIINFO_END
};

static GnomeUIInfo the_help_menu[] = {
	GNOMEUIINFO_HELP("gtranslator"),
	GNOMEUIINFO_MENU_ABOUT_ITEM(about_box, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(N_("gtranslator _website"),
			       N_("gtranslator's homepage on the web"),
			       call_gtranslator_homepage,
			       GNOME_STOCK_MENU_HOME),
	GNOMEUIINFO_END
};

static GnomeUIInfo the_menus[] = {
	GNOMEUIINFO_MENU_FILE_TREE(the_file_menu),
	GNOMEUIINFO_MENU_EDIT_TREE(the_edit_menu),
	GNOMEUIINFO_SUBTREE(N_("_Messages"), the_messages_menu),
	GNOMEUIINFO_SUBTREE(N_("Mess_age status"), the_msg_status_menu),
	GNOMEUIINFO_MENU_SETTINGS_TREE(the_settings_menu),
	GNOMEUIINFO_MENU_HELP_TREE(the_help_menu),
	GNOMEUIINFO_END
};

/* 
 * The toolbar buttons
 */
static GnomeUIInfo the_toolbar[] = {
	GNOMEUIINFO_ITEM_STOCK(N_("Open"),
			       N_("Open a po-file"),
			       open_file,
			       GNOME_STOCK_PIXMAP_OPEN),
	GNOMEUIINFO_ITEM_STOCK(N_("Save"),
			       N_("Save File"),
			       save_current_file,
			       GNOME_STOCK_PIXMAP_SAVE),
	GNOMEUIINFO_ITEM_STOCK(N_("Save as"),
			       N_("Save file with a different name"),
			       save_file_as,
			       GNOME_STOCK_PIXMAP_SAVE_AS),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(N_("Compile"),
			       N_("Compile the po-file"),
			       compile,
			       GNOME_STOCK_PIXMAP_CONVERT),
	GNOMEUIINFO_ITEM_STOCK(N_("Update"),
			       N_("Update the po-file"),
			       update,
			       GNOME_STOCK_PIXMAP_REFRESH),		       
	GNOMEUIINFO_ITEM_STOCK(N_("Header"),
			       N_("Edit the header"),
			       edit_header,
			       GNOME_STOCK_PIXMAP_PROPERTIES),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(N_("Options"),
			       N_("gtranslator options"),
			       prefs_box,
			       GNOME_STOCK_PIXMAP_PREFERENCES),
	GNOMEUIINFO_ITEM_STOCK(N_("Exit"),
			       N_("Exit"),
			       gtranslator_quit,
			       GNOME_STOCK_PIXMAP_EXIT),
	GNOMEUIINFO_END
};

static GnomeUIInfo the_navibar[] = {
	GNOMEUIINFO_ITEM_STOCK(N_("First"),
			       N_("Go to the first message"),
			       goto_first_msg,
			       GNOME_STOCK_PIXMAP_FIRST),
	GNOMEUIINFO_ITEM_STOCK(N_("Back"),
			       N_("Go one message back"),
			       goto_prev_msg,
			       GNOME_STOCK_PIXMAP_BACK),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(N_("Next"),
			       N_("Go one message forward"),
			       goto_next_msg,
			       GNOME_STOCK_PIXMAP_FORWARD),
	GNOMEUIINFO_ITEM_STOCK(N_("Last"),
			       N_("Go to the last message"),
			       goto_last_msg,
			       GNOME_STOCK_PIXMAP_LAST),
	GNOMEUIINFO_ITEM(N_("Missing"),
			       N_("Go to next untranslated message"),
			       goto_next_untranslated,
			       untrans_xpm),
	GNOMEUIINFO_ITEM_STOCK(N_("Fuzzy"),
			       N_("Go to the next fuzzy translation"),
			       goto_next_fuzzy,
			       GNOME_STOCK_PIXMAP_BOOK_RED),
	GNOMEUIINFO_ITEM_STOCK(N_("Go to"),
			       N_("Go to specified message number"),
			       goto_dlg,
			       GNOME_STOCK_PIXMAP_JUMP_TO),
	GNOMEUIINFO_ITEM_STOCK(N_("Find"),
			       N_("Find string in po-file"),
			       find_dialog,
			       GNOME_STOCK_PIXMAP_SEARCH),
	GNOMEUIINFO_ITEM_STOCK(N_("Query"),
			       N_("Query for a string"),
			       query_dialog,
			       GNOME_STOCK_PIXMAP_JUMP_TO),
	GNOMEUIINFO_END
};

/*
 * The popup-menu.
 */
static GnomeUIInfo the_popup_menu[] = {
	GNOMEUIINFO_MENU_OPEN_ITEM(open_file, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_CUT_ITEM(cut_clipboard, NULL),
	GNOMEUIINFO_MENU_COPY_ITEM(copy_clipboard, NULL),
	GNOMEUIINFO_MENU_PASTE_ITEM(paste_clipboard, NULL),
	GNOMEUIINFO_MENU_CLEAR_ITEM(clear_selection, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_CLOSE_ITEM(close_file, NULL),
	GNOMEUIINFO_MENU_SAVE_AS_ITEM(save_file_as, NULL),
	GNOMEUIINFO_END
};

/*
 * Pop's up the curious popup-menu.
 */
static gint create_popup_menu(GtkText *widget, GdkEventButton *event, gpointer d)
{
	/*
	 * Only react on rightclick.
	 */
	if(event->button==3)
	{
		/*
		 * Only respond if a file has been present/opened and if the
		 * corresponding option is set.
		 */
		if((wants.popup_menu) && (file_opened==TRUE))
		{
			GtkWidget *popup_menu;
			popup_menu=gnome_popup_menu_new(the_popup_menu);
			gnome_popup_menu_do_popup_modal(popup_menu,
				NULL, NULL, NULL, event);
			/*
			 * Destroy the menu after creation.
			 */
			gtk_widget_destroy(popup_menu);
			return TRUE;
		}
	}
	return FALSE;
}

/****
  * Actions stuff goes here
 ** * */
void change_actions(gboolean state, ...)
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
		if (id==0)
			g_warning("id==0, it shouldn't be!");
	} 
	va_end(ap);
}

static void insert_action(gint act_num, GnomeUIInfo mi, GnomeUIInfo ti)
{
	acts[act_num].menu = mi.widget;
	acts[act_num].tool = ti.widget;
}

static void create_actions(void)
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
	/*------------------------------------------------*/
	insert_action(ACT_UNDO, the_edit_menu[0], NONE);
	insert_action(ACT_CUT, the_edit_menu[2], NONE);
	insert_action(ACT_COPY, the_edit_menu[3], NONE);
	insert_action(ACT_PASTE, the_edit_menu[4], NONE);
	insert_action(ACT_CLEAR, the_edit_menu[5], NONE);
	insert_action(ACT_FIND, the_edit_menu[7], the_navibar[8]);
	insert_action(ACT_FIND_AGAIN, the_edit_menu[8], NONE);
	insert_action(ACT_QUERY, the_edit_menu[9], the_navibar[9]);
	insert_action(ACT_HEADER, the_edit_menu[11], the_toolbar[6]);
	/*------------------------------------------------*/
	insert_action(ACT_FIRST, the_messages_menu[0], the_navibar[0]);
	insert_action(ACT_BACK, the_messages_menu[1], the_navibar[1]);
	insert_action(ACT_NEXT, the_messages_menu[3], the_navibar[3]);
	insert_action(ACT_LAST, the_messages_menu[4], the_navibar[4]);
	insert_action(ACT_GOTO, the_messages_menu[6], the_navibar[7]);
	insert_action(ACT_NEXT_FUZZY, the_messages_menu[7], the_navibar[6]);
	insert_action(ACT_NEXT_UNTRANSLATED, the_messages_menu[8], the_navibar[5]);
	/*------------------------------------------------*/
	insert_action(ACT_TRANSLATED, the_msg_status_menu[0], NONE);
	insert_action(ACT_FUZZY, the_msg_status_menu[1], NONE);
	insert_action(ACT_STICK, the_msg_status_menu[2], NONE);
	/*------------------------------------------------*/
	insert_action(ACT_END, NONE, NONE);
}

void disable_actions_no_file(void)
{
	disable_actions(ACT_COMPILE, ACT_UPDATE, ACT_ACCOMPLISH,
			ACT_SAVE, ACT_SAVE_AS, ACT_REVERT, ACT_CLOSE,
			ACT_UNDO, ACT_CUT, ACT_COPY, ACT_PASTE, ACT_CLEAR,
			ACT_FIND, ACT_FIND_AGAIN, ACT_HEADER, ACT_QUERY,
			ACT_FIRST, ACT_BACK, ACT_NEXT, ACT_LAST,
			ACT_GOTO, ACT_NEXT_FUZZY, ACT_NEXT_UNTRANSLATED,
			ACT_FUZZY, ACT_TRANSLATED, ACT_STICK);
	gtk_text_set_editable(GTK_TEXT(trans_box), FALSE);
}

void enable_actions_just_opened(void)
{
	enable_actions( ACT_COMPILE, ACT_SAVE_AS, ACT_CLOSE, ACT_ACCOMPLISH,
			ACT_CUT, ACT_COPY, ACT_PASTE, ACT_CLEAR,
			ACT_FIND, ACT_HEADER, ACT_NEXT, ACT_LAST, ACT_QUERY,
			ACT_GOTO, ACT_FUZZY, ACT_TRANSLATED, ACT_STICK);
	/*
	 * If we'd have the option to use the update function set, enable the
	 *  Update button in the toolbar and in the menu.
	 */	       
	if(wants.update_function)
	{
		enable_actions(ACT_UPDATE);	
	}  
	/*
	 * Enable the editing of the msgstrs :-)
	 */
	gtk_text_set_editable(GTK_TEXT(trans_box), TRUE);
	/*
	 * Make it focused initially
	 */
	gtk_window_set_focus(GTK_WINDOW(app1), trans_box);
}

/*
  * The main function, which creates the application
 */
void create_app1(void)
{
	GtkWidget *search_bar, *tool_bar;
	GtkWidget *vbox1;
	GtkWidget *scrolledwindow1, *scrolledwindow2;
	GtkWidget *pane;
	GtkWidget *filebox;
	
	/*
	 * Create the app	
	 */
	app1 = gnome_app_new("gtranslator", _("gtranslator"));
	gnome_app_create_menus(GNOME_APP(app1), the_menus);

	pane=e_hpaned_new();
	filebox=gtranslator_sidebar_new();
	
	e_paned_pack1(E_PANED(pane), filebox, TRUE, FALSE);
	e_paned_set_position(E_PANED(pane), 75);

	/*
	 * Create the tool- and search-bar
	 */
	tool_bar =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
	gnome_app_fill_toolbar(GTK_TOOLBAR(tool_bar), the_toolbar, NULL);
	gnome_app_add_toolbar(GNOME_APP(app1), GTK_TOOLBAR(tool_bar),
			      "tool_bar", GNOME_DOCK_ITEM_BEH_EXCLUSIVE,
			      GNOME_DOCK_TOP, 1, 0, 0);

	search_bar = gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
				     GTK_TOOLBAR_BOTH);
	gnome_app_fill_toolbar(GTK_TOOLBAR(search_bar), the_navibar, NULL);
	gnome_app_add_toolbar(GNOME_APP(app1), GTK_TOOLBAR(search_bar),
			      "search_bar", GNOME_DOCK_ITEM_BEH_EXCLUSIVE,
			      GNOME_DOCK_TOP, 2, 0, 0);

	vbox1 = gtk_vbox_new(FALSE, 0);
	
	e_paned_pack2(E_PANED(pane), vbox1, TRUE, FALSE);
	gnome_app_set_contents(GNOME_APP(app1), pane);

	scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow1, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	text1=gtk_text_new(NULL,NULL);
	
	gtk_container_add(GTK_CONTAINER(scrolledwindow1), text1);
	
	gtk_text_set_editable(GTK_TEXT(text1), FALSE);

	scrolledwindow2 = gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow2, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow2),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	trans_box = gtk_text_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolledwindow2), trans_box);

	appbar1 = gnome_appbar_new(TRUE, TRUE, GNOME_PREFERENCES_NEVER);
	gnome_app_set_statusbar(GNOME_APP(app1), appbar1);
	
	/*
	 * Make menu hints display on the appbar
	 */
	gnome_app_install_menu_hints(GNOME_APP(app1), the_menus);

	create_actions();

	gtranslator_history_show();

	/*
	 * Check if we'd to use special styles.
	 */
	if(wants.use_own_specs)
	{
		/*
		 * Set the own specs for colors and for the font.
		 */
		gtranslator_config_init();
		gtranslator_set_style(text1);
		gtranslator_set_style(trans_box);
		gtranslator_config_close();
	}
	
	/*
	 * The callbacks list
	 */
	gtk_signal_connect(GTK_OBJECT(app1), "delete_event",
			   GTK_SIGNAL_FUNC(gtranslator_quit), NULL);
	gtk_signal_connect(GTK_OBJECT(trans_box), "changed",
			   GTK_SIGNAL_FUNC(text_has_got_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(text1), "button_press_event",
			   GTK_SIGNAL_FUNC(create_popup_menu), NULL);
	gtk_signal_connect(GTK_OBJECT(trans_box), "button_press_event",
			   GTK_SIGNAL_FUNC(create_popup_menu), NULL);

	gtk_signal_connect(GTK_OBJECT(app1), "key_press_event",
			   GTK_SIGNAL_FUNC(gtranslator_keyhandler), NULL);
	/*
	 * The D'n'D signals
	 */
	gtk_drag_dest_set(GTK_WIDGET(app1),
			  GTK_DEST_DEFAULT_ALL | GTK_DEST_DEFAULT_HIGHLIGHT,
			  dragtypes, sizeof(dragtypes) / sizeof(dragtypes[0]),
			  GDK_ACTION_COPY);
	gtk_signal_connect(GTK_OBJECT(app1), "drag_data_received",
			   GTK_SIGNAL_FUNC(gtranslator_dnd),
			   GUINT_TO_POINTER(dnd_type));
}

/*
 * The own quit-code
 */
static gint gtranslator_quit(GtkWidget  * widget, GdkEventAny  * e,
			     gpointer useless)
{
	/*
	 * If file was changed, but user pressed Cancel, don't quit
	 */
	if (!ask_to_save_file())
		return TRUE;
	close_file(NULL, NULL);
	save_geometry();
	/*
	 * Free the preferences stuff.
	 */
	free_prefs();
	gnome_regex_cache_destroy(rxc);
	/*
	 * Store the current date.
	 */
	gtranslator_config_init();
	gtranslator_config_set_last_run_date();
	gtranslator_config_close();
	
	if(gnome_vfs_initialized())
	{
		gnome_vfs_shutdown();
	}

	/*
	 * Free up our used GtrColorScheme "theme".
	 */
	free_color_scheme(&theme);
	
	/*
	 * Quit with the normal Gtk+ quit.
	 */
	gtk_main_quit();
	return FALSE;
}

/*
 * Go through the characters and search for free spaces
 * and replace them with '·''s.
 */
static void invert_dot(gchar *str)
{
	guint i;
	g_return_if_fail(str != NULL);

	for(i=0; str[i] != '\0'; i++) {
		if(str[i]==' ')
			str[i]=(_("·"))[0];
		else if(str[i]==(_("·"))[0])
			str[i]=' ';
	}
}

/* 
 * Display the message in text boxes
 */
void display_msg(GList  * list_item)
{
	GtrMsg *msg;
	gchar *ispell_command[5];
	msg = GTR_MSG(list_item->data);
	nothing_changes = TRUE;
	clean_text_boxes();
	/*
	 * Substitute the free spaces in the msgid only if this is wished and
	 *  possible.
	 */ 
	if(wants.dot_char)
	{
		gchar *temp;

		temp = g_strdup(msg->msgid);
		invert_dot(temp);
		
		gtranslator_syntax_insert_text(text1, temp);
		
		g_free(temp);

		if (msg->msgstr) {
			temp = g_strdup(msg->msgstr);
			invert_dot(temp);
			
			gtranslator_syntax_insert_text(trans_box, temp);
			
			g_free(temp);
		}
	} else {
		gtranslator_syntax_insert_text(text1, msg->msgid);
		gtranslator_syntax_insert_text(trans_box, msg->msgstr);
	}
	
	/*
	 * Use instant spell checking via gtkspell only if the corresponding
	 *  setting in the preferences is set.
	 */
	if(wants.instant_spell_check)
	{
		/*
		 * Start up gtkspell if not already done.
		 */ 
		if(!gtkspell_running())
		{
			ispell_command[0]="ispell";
			ispell_command[1]="-a";
			
			/*
			 * Should we use special dictionary settings?
			 */ 
			if(wants.use_own_dict && wants.dictionary)
			{
				ispell_command[2]="-d";
				ispell_command[3]=g_strdup_printf("%s",
					wants.dictionary);
				ispell_command[4]=NULL;
			} else {
				ispell_command[2]=NULL;
			}
		
			/*
			 * Start the gtkspell process.
			 */ 
			gtkspell_start(NULL, ispell_command);
		}

		/*
		 * Attach it to the translation box for instant spell checking.
		 */ 
		gtkspell_attach(GTK_TEXT(trans_box));
	}
#define set_active(number,flag) \
	gtk_check_menu_item_set_active(\
		(GtkCheckMenuItem *)(the_msg_status_menu[number].widget),\
		msg->status & flag);
	set_active(0, GTR_MSG_STATUS_TRANSLATED);
	set_active(1, GTR_MSG_STATUS_FUZZY);
	set_active(2, GTR_MSG_STATUS_STICK);
#undef set_active
	nothing_changes = FALSE;
	message_changed = FALSE;
}

void update_msg(void)
{
	guint len;
	GtrMsg *msg = GTR_MSG(po->current->data);
	if (!message_changed)
		return;
	len = gtk_text_get_length(GTK_TEXT(trans_box));
	if (len) {
		if (msg->msgid[strlen(msg->msgid) - 1] == '\n') {
			if (GTK_TEXT_INDEX(GTK_TEXT(trans_box), len - 1)
			    != '\n')
				gtk_editable_insert_text(
				    GTK_EDITABLE(trans_box), "\n", 1, &len);
		} else {
			if (GTK_TEXT_INDEX(GTK_TEXT(trans_box), len - 1)
			    == '\n') {
				gtk_editable_delete_text(
				    GTK_EDITABLE(trans_box), len-1, len);
				len--;
			}
		}
		g_free(msg->msgstr);
		msg->msgstr = gtk_editable_get_chars(GTK_EDITABLE(trans_box),
						     0, len);

		/*
		 * If spaces were substituted with dots, replace them back
		 */
		if(wants.dot_char)
			invert_dot(msg->msgstr);
		if (!(msg->status & GTR_MSG_STATUS_TRANSLATED)) {
			msg->status |= GTR_MSG_STATUS_TRANSLATED;
			po->translated++;
		}
	} else {
		msg->msgstr = NULL;
		msg->status &= ~GTR_MSG_STATUS_TRANSLATED;
		po->translated--;
	}
	message_changed = FALSE;
	/*
	 * Update the statusbar informations.
	 */
	update_appbar(g_list_position(po->messages, po->current));
}

void toggle_msg_status(GtkWidget  * item, gpointer which)
{
	gint flag = GPOINTER_TO_INT(which);
	if (nothing_changes)
		return;
	text_has_got_changed(NULL, NULL);
	if (flag == GTR_MSG_STATUS_FUZZY) {
		mark_msg_fuzzy(GTR_MSG(po->current->data),
			       GTK_CHECK_MENU_ITEM(item)->active);
	} else if (flag == GTR_MSG_STATUS_STICK) {
		mark_msg_sticky(GTR_MSG(po->current->data),
				GTK_CHECK_MENU_ITEM(item)->active);
		display_msg(po->current);
		message_changed = TRUE;
	}
	update_msg();
}
	 
/*
 * Cleans up the text boxes.
 */
void clean_text_boxes()
{
	gtk_editable_delete_text(GTK_EDITABLE(text1), 0, -1);
	gtk_editable_delete_text(GTK_EDITABLE(trans_box), 0, -1);
}

static void update_appbar(gint pos)
{
	gchar *str, *status;
	GtrMsg *msg;
	gnome_appbar_pop(GNOME_APPBAR(appbar1));
	/*
	 * Get the message.
	 */
	msg=GTR_MSG(po->current->data);
	/*
	 * And append according to the message status the status name.
	 */
	if(msg->status & GTR_MSG_STATUS_FUZZY)
	{
		if(po->fuzzy>1)
		{
			status=g_strdup_printf(_("%s [ %i Fuzzy left ]"), _("Fuzzy"), po->fuzzy);
		}	
		else
		{
			status=g_strdup_printf(_("%s [ No fuzzy left ]"), _("Fuzzy"));
			/*
			 * Also disable the corresponding button.
			 */
			disable_actions(ACT_NEXT_FUZZY);
		}
	} else if(msg->status & GTR_MSG_STATUS_STICK) {
		status=g_strdup(_("Stick"));
	} else if(msg->status & GTR_MSG_STATUS_TRANSLATED) {
		status=g_strdup(_("Translated"));
	} else { 
		/*
		 * Message is untranslated 
		 */
		if ((po->length - po->translated)>1)
		{
			guint missya;
			missya = po->length - po->translated;
			status=g_strdup_printf(_("%s [ %i Untranslated left ]"), _("Untranslated"), missya);
		} else {
			status=g_strdup_printf(_("%s [ No untranslated left ]"), _("Untranslated"));
			/*
			 * Also disable the coressponding buttons for the
			 *  next untranslated message/accomplish function.
			 */
			disable_actions(ACT_NEXT_UNTRANSLATED, ACT_ACCOMPLISH);
		}	
	}
	/*
	 * Assign the first part.
	 */
	str=g_strdup_printf(_("Message %d / %d / Status: %s"), pos + 1, po->length, status);
	/*
	 * Set the appbar text.
	 */
	gnome_appbar_push(GNOME_APPBAR(appbar1), str);
	/*
	 * Update the progressbar.
	 */
	gtranslator_set_progress_bar();

	/*
	 * And free the allocated string.
	 */
	g_free(str);

	if(status)
	{
		g_free(status);
	}
}

/*
 * Updates current msg, and shows to_go msg instead, also adjusts actions
 */
void goto_given_msg(GList  * to_go)
{
	static gint pos = 0;
	update_msg();
	if (pos == 0)
	{
		enable_actions(ACT_FIRST, ACT_BACK);
	}	
	else if (pos == po->length - 1)
	{
		enable_actions(ACT_NEXT, ACT_LAST);
	}	
	po->current = to_go;
	display_msg(po->current);
	pos = g_list_position(po->messages, po->current);
	if (pos == 0)
	{
		disable_actions(ACT_FIRST, ACT_BACK);
	}	
	else if (pos == po->length - 1)
	{
		disable_actions(ACT_NEXT, ACT_LAST);
	}	
	update_appbar(pos);
}

/*
 * Callbacks for moving around messages 
 */
void goto_first_msg(GtkWidget  * widget, gpointer useless)
{
	goto_given_msg(g_list_first(po->messages));
}

void goto_prev_msg(GtkWidget  * widget, gpointer useless)
{
	goto_given_msg(g_list_previous(po->current));
}

void goto_next_msg(GtkWidget  * widget, gpointer useless)
{
	goto_given_msg(g_list_next(po->current));
}

void goto_last_msg(GtkWidget  * widget, gpointer useless)
{
	goto_given_msg(g_list_last(po->messages));
}

void goto_nth_msg(GtkWidget  * widget, gpointer number)
{
	goto_given_msg(g_list_nth(po->messages, GPOINTER_TO_UINT(number)));
}

/*
 * Goes to the homepage of gtranslator on the web
 */
static void call_gtranslator_homepage(GtkWidget  * widget, gpointer useless)
{
	gnome_url_show("http://www.gtranslator.org");
}

/*
 * The text oriented callbacks
 */
static void cut_clipboard(GtkWidget  * widget, gpointer useless)
{
	gtk_editable_cut_clipboard(GTK_EDITABLE(trans_box));
}

static void copy_clipboard(GtkWidget  * widget, gpointer useless)
{
	gtk_editable_copy_clipboard(GTK_EDITABLE(trans_box));
}

static void paste_clipboard(GtkWidget  * widget, gpointer useless)
{
	gtk_editable_paste_clipboard(GTK_EDITABLE(trans_box));

	/*
	 * Rehighlight the text widget after a paste.
	 */ 
	gtranslator_syntax_update_text(trans_box);
}

static void clear_selection(GtkWidget  * widget, gpointer useless)
{
	gtk_editable_delete_selection(GTK_EDITABLE(trans_box));
}

static void undo_changes(GtkWidget  * widget, gpointer useless)
{
	display_msg(po->current);
	disable_actions(ACT_UNDO);
}

/*
 * Set po->file_changed to TRUE if the text in the translation box has been
 * updated.
 */
void text_has_got_changed(GtkWidget  * widget, gpointer useless)
{
	if (nothing_changes)
		return;
	if (!po->file_changed)
	{
		po->file_changed = TRUE;
		if(po->no_write_perms==FALSE||strstr(po->filename, "/.gtranslator-"))
		{
			enable_actions(ACT_SAVE, ACT_REVERT, ACT_UNDO);
		}
		else
		{
			enable_actions(ACT_REVERT, ACT_UNDO);
		}
	}
	if (!message_changed)
	{
		GtrMsg *msg = GTR_MSG(po->current->data);
		message_changed = TRUE;
		enable_actions(ACT_UNDO);
		if ((wants.unmark_fuzzy) 
		     && (msg->status & GTR_MSG_STATUS_FUZZY))
		{
		     	mark_msg_fuzzy(msg, FALSE);
			gtk_check_menu_item_set_active(
			    (GtkCheckMenuItem *) acts[ACT_FUZZY].menu, FALSE);
		}
	}

	/*
	 * Do all these steps only if the option to use the '·' is set.
	 */
	if(wants.dot_char)
	{
		gchar *newstr;
		guint len, index;

		nothing_changes = TRUE;
		/*
		 * Freeze the translation box.
		 */
		gtk_text_freeze(GTK_TEXT(trans_box));
		
		/*
		 * Get the current pointer position.
		 */
		index=gtk_editable_get_position(GTK_EDITABLE(trans_box));
		/*
		 * Get the text from the translation box.
		 */
		newstr=gtk_editable_get_chars(GTK_EDITABLE(trans_box), 0, -1);
		/*
		 * Parse the characters for a free space and replace
		 *  them with the '·'.
		 */
		for(len=0; newstr[len]!='\0'; len++) {
			if(newstr[len]==' ') {
				static gint pos;
				pos = len;

				/*
				 * Clean the textbox.
				 */
				gtk_editable_delete_text(
					GTK_EDITABLE(trans_box), pos, pos+1);
				/*
				 * Insert the changed text with the '·''s.
				 */
				gtk_editable_insert_text(
					GTK_EDITABLE(trans_box), _("·"), 1, &pos);
			}
		}
		
		g_free(newstr);

		
		/*
		 * Go to the old text index.
		 */
		if(index > 0 &&
			index < gtk_text_get_length(GTK_TEXT(trans_box)))
		{
			gtk_editable_set_position(
				GTK_EDITABLE(trans_box), index);
		}
		else
		{
			if(index >= gtk_text_get_length(GTK_TEXT(trans_box)))
			{
				gtk_editable_set_position(
					GTK_EDITABLE(trans_box), 
					gtk_text_get_length(
						GTK_TEXT(trans_box)));
			}
		}

		/*
                 * Thaw up the translation box to avoid the reverse writing
                 *  feature.
                 */
		gtk_text_thaw(GTK_TEXT(trans_box));
		nothing_changes = FALSE;
	}

	gtranslator_syntax_update_text(trans_box);
}

/*
 * The own keyhandler to get the left/right actions.
 */ 
static gint gtranslator_keyhandler(GtkWidget *widget, GdkEventKey *event)
{
	g_return_val_if_fail(widget!=NULL, FALSE);
	g_return_val_if_fail(event!=NULL, FALSE);
	#define IfGood(x) \
	if(GTK_WIDGET_SENSITIVE(GTK_WIDGET(((GnomeUIInfo)(x)).widget)))

	if(file_opened)
	{
		if(event->state & GDK_CONTROL_MASK)
		{
			switch(event->keyval)
			{
				case GDK_Left:
				case GDK_Down:
					IfGood(the_navibar[1])
					{
						goto_prev_msg(NULL, NULL);
					}
					break;
				
				case GDK_Right:
				case GDK_Up:
					IfGood(the_navibar[3])
					{
						goto_next_msg(NULL, NULL);
					}
					break;

				case GDK_Page_Up:
					IfGood(the_navibar[0])
					{
						goto_first_msg(NULL, NULL);
					}
					break;
					
				case GDK_Page_Down:
					IfGood(the_navibar[4])
					{
						goto_last_msg(NULL, NULL);
					}
					break;
					
				default:
					break;
			}
		}
	}
	
	return TRUE;
}
