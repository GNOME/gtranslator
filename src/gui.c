/**
* Fatih Demir [ kabalak@gmx.net ]
*
* Menus, "actions", toolbars, *_msg totally created, and everything else
* changed by
* Gediminas Paulauskas <menesis@delfi.lt>
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* The interface is created here 
*
* -- the source 
**/

#include "gui.h"

/* 
 * Variables and functions, which should be used only here
 */

static gint gtranslator_quit(GtkWidget *widget, GdkEventAny* e, gpointer useless);

typedef struct _GtrAction GtrAction;
#define GTR_ACTION(x) ((GtrAction *)x)

struct _GtrAction
{
	GtkWidget *menu;
	GtkWidget *tool;
};

// A hash table which saves all defined actions
static GHashTable *acts=NULL;

// routines for "actions"
static void destroy_actions(void);
static void create_actions(void);
static void insert_action(gchar *act_name, GnomeUIInfo mi, GnomeUIInfo ti);
static void free_action(gpointer key, gpointer value, gpointer useless);

void call_gtranslator_homepage(GtkWidget *widget,gpointer useless);

// Callbacks for text operations
void cut_clipboard(GtkWidget *widget,gpointer useless);
void copy_clipboard(GtkWidget *widget,gpointer useless);
void paste_clipboard(GtkWidget *widget,gpointer useless);
void clear_selection(GtkWidget *widget,gpointer useless);
void text_has_got_changed(GtkWidget *widget,gpointer useless);

/**
* The menu-entries
**/
static GnomeUIInfo the_file_menu[] =
{
        {
          GNOME_APP_UI_ITEM, N_("_Compile"),
          N_("Compile the po-file"),
          compile, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_CONVERT,
          GDK_C, GDK_MOD1_MASK, NULL
        },
	GNOMEUIINFO_SEPARATOR,
        GNOMEUIINFO_MENU_OPEN_ITEM(open_file, NULL),
        GNOMEUIINFO_MENU_SAVE_ITEM(save_current_file, NULL),
        GNOMEUIINFO_MENU_SAVE_AS_ITEM(save_file_as, NULL),
        GNOMEUIINFO_MENU_REVERT_ITEM(revert_file, NULL),
        GNOMEUIINFO_MENU_CLOSE_ITEM(close_file, NULL),
        GNOMEUIINFO_SEPARATOR,
        GNOMEUIINFO_MENU_EXIT_ITEM(gtranslator_quit, NULL),
        GNOMEUIINFO_END
};

static GnomeUIInfo the_edit_menu[] =
{
        GNOMEUIINFO_MENU_CUT_ITEM(cut_clipboard, NULL),
        GNOMEUIINFO_MENU_COPY_ITEM(copy_clipboard, NULL),
        GNOMEUIINFO_MENU_PASTE_ITEM(paste_clipboard, NULL),
	GNOMEUIINFO_MENU_CLEAR_ITEM(NULL,NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_FIND_ITEM(find_dialog,NULL),
	GNOMEUIINFO_MENU_FIND_AGAIN_ITEM(search_do,NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(
		N_("_Header..."),
		N_("Edit the header"),
		edit_header,
		GNOME_STOCK_MENU_PROP
	),
        GNOMEUIINFO_END
};

static GnomeUIInfo the_messages_menu[] =
{
        {
          GNOME_APP_UI_ITEM, N_("_First"),
          N_("Go to the first message"),
          goto_first_msg, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_FIRST,
          GDK_Home, GDK_CONTROL_MASK, NULL
        },
        {
          GNOME_APP_UI_ITEM, N_("_Back"),
          N_("Go one message back"),
          goto_prev_msg, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BACK,
          GDK_Page_Up, 0, NULL
        },
        {
          GNOME_APP_UI_ITEM, N_("_Next"),
          N_("Go one message forward"),
          goto_next_msg, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_FORWARD,
          GDK_Page_Down, 0, NULL
        },
        {
          GNOME_APP_UI_ITEM, N_("_Last"),
          N_("Go to the Last message"),
          goto_last_msg, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_LAST,
          GDK_End, GDK_CONTROL_MASK, NULL
        },
        GNOMEUIINFO_SEPARATOR,
        {
          GNOME_APP_UI_ITEM, N_("_Go to..."),
          N_("Goto specified message number"),
          goto_dlg, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_JUMP_TO,
          'G', GDK_CONTROL_MASK, NULL
        },
        GNOMEUIINFO_END
};

static GnomeUIInfo the_msg_status_menu [] =
{
	{
		GNOME_APP_UI_TOGGLEITEM, N_("Translated"),
		N_("Toggle translated status of a message"),
		toggle_msg_status, 
		GINT_TO_POINTER(GTRANSLATOR_MSG_STATUS_TRANSLATED), 
		NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_GREEN,
                0, 0, NULL 
	},
	{
		GNOME_APP_UI_TOGGLEITEM, N_("Fuzzy"),
		N_("Toggle fuzzy status of a message"),
		NULL,
		GINT_TO_POINTER(GTRANSLATOR_MSG_STATUS_FUZZY), 
		NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_RED,
                0, 0, NULL 
	},
	{
		GNOME_APP_UI_TOGGLEITEM, N_("Stick"),
		N_("Stick this message"),
		NULL,
		GINT_TO_POINTER(GTRANSLATOR_MSG_STATUS_STICK), 
		NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_STOP,
		0, 0, NULL
	},
	GNOMEUIINFO_END
};

static GnomeUIInfo the_msg_db_menu [] =
{
	GNOMEUIINFO_ITEM_STOCK(
		N_("_Add the current message"),
		N_("Add the current message to the messages DB"),
		append_to_msg_db,
		GNOME_STOCK_MENU_ATTACH
	),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(
		N_("_Query for a message..."),
		N_("Query the messages DB for the current msgid"),
		query_dialog,
		GNOME_STOCK_MENU_SEARCH
	),
	GNOMEUIINFO_END
};

static GnomeUIInfo the_settings_menu[] =
{
        GNOMEUIINFO_MENU_PREFERENCES_ITEM(prefs_box, NULL),
        GNOMEUIINFO_END
};

static GnomeUIInfo the_help_menu[] =
{
	GNOMEUIINFO_HELP("gtranslator"),
        GNOMEUIINFO_MENU_ABOUT_ITEM(about_box, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(
		N_("gtranslator _website"),
		N_("gtranslator's homepage on the web"),
		call_gtranslator_homepage,
		GNOME_STOCK_MENU_HOME
	),
        GNOMEUIINFO_END
};

static GnomeUIInfo the_menus[] =
{
        GNOMEUIINFO_MENU_FILE_TREE(the_file_menu),
        GNOMEUIINFO_MENU_EDIT_TREE(the_edit_menu),
	GNOMEUIINFO_SUBTREE(N_("_Messages"), the_messages_menu),
	GNOMEUIINFO_SUBTREE(N_("Message _status"),the_msg_status_menu),
	GNOMEUIINFO_SUBTREE(N_("Message _db"),the_msg_db_menu),
        GNOMEUIINFO_MENU_SETTINGS_TREE(the_settings_menu),
        GNOMEUIINFO_MENU_HELP_TREE(the_help_menu),
        GNOMEUIINFO_END
};

/*
 * The toolbar buttons
 */

static GnomeUIInfo the_toolbar[] =
{
	GNOMEUIINFO_ITEM_STOCK(
          N_("Compile"),
          N_("Compile the po-file"),
          compile,
          GNOME_STOCK_PIXMAP_CONVERT
	),
	GNOMEUIINFO_ITEM_STOCK(
          N_("Open"),
          N_("Open a po-file"),
          open_file,
          GNOME_STOCK_PIXMAP_OPEN
	),
	GNOMEUIINFO_ITEM_STOCK(
          N_("Save"),
          N_("Save File"),
          save_current_file,
          GNOME_STOCK_PIXMAP_SAVE
	),
	GNOMEUIINFO_ITEM_STOCK(
          N_("Save as"),
          N_("Save file with a different name"),
          save_file_as,
          GNOME_STOCK_PIXMAP_SAVE_AS
	),
	GNOMEUIINFO_ITEM_STOCK(
          N_("Header"),
          N_("Edit the header"),
          edit_header,
          GNOME_STOCK_PIXMAP_PROPERTIES
	),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(
          N_("Options"),
          N_("gtranslator options"),
          prefs_box,
          GNOME_STOCK_PIXMAP_PREFERENCES
	),
	GNOMEUIINFO_ITEM_STOCK(
          N_("Exit"),
          N_("Exit"),
          gtranslator_quit,
          GNOME_STOCK_PIXMAP_EXIT
	),
	GNOMEUIINFO_END
};

static GnomeUIInfo the_searchbar[] =
{
	GNOMEUIINFO_ITEM_STOCK(
          N_("First"),
          N_("Go to the first message"),
          goto_first_msg,
          GNOME_STOCK_PIXMAP_FIRST
	),
	GNOMEUIINFO_ITEM_STOCK(
          N_("Back"),
          N_("Go one message back"),
          goto_prev_msg,
          GNOME_STOCK_PIXMAP_BACK
	),
	GNOMEUIINFO_ITEM_STOCK(
          N_("Next"),
          N_("Go one message forward"),
          goto_next_msg,
          GNOME_STOCK_PIXMAP_FORWARD
	),
	GNOMEUIINFO_ITEM_STOCK(
          N_("Last"),
          N_("Go to the Last message"),
          goto_last_msg,
          GNOME_STOCK_PIXMAP_LAST
	),
	GNOMEUIINFO_ITEM_STOCK(
          N_("Goto"),
          N_("Goto specified message number"),
          goto_dlg,
          GNOME_STOCK_PIXMAP_JUMP_TO
	),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(
          N_("Find"),
          N_("Find string in po-file"),
          find_dialog,
          GNOME_STOCK_PIXMAP_SEARCH
	),
	GNOMEUIINFO_ITEM_STOCK(
          N_("Add"),
          N_("Add to the messages db"),
          append_to_msg_db,
          GNOME_STOCK_PIXMAP_ADD
	),
	GNOMEUIINFO_ITEM_STOCK(
          N_("Query"),
          N_("Query for a message in db"),
          query_dialog,
          GNOME_STOCK_PIXMAP_BOOK_OPEN
	),
	GNOMEUIINFO_END
};

// a unused variable for testing if a toolbar element was provided to
// insert_action
static GnomeUIInfo NONE;

/*
 *  Actions stuff goes here
 */

// This define saves repeating the same twice.
#define change_actions(what)\
	const gchar *act_name=first;\
	va_list ap;\
	GtrAction *act;\
\
	va_start(ap,first);\
	while (act_name!=NULL) {\
		act=GTR_ACTION(g_hash_table_lookup(acts,act_name));\
		if (act) {\
			if (act->menu)\
				gtk_widget_set_sensitive(act->menu,what);\
			if (act->tool)\
				gtk_widget_set_sensitive(act->tool,what);\
		}\
		else g_warning("Action does not exist: %s",act_name);\
		act_name=va_arg(ap,gchar *);\
	}\
	va_end(ap);
	
void enable_actions(const gchar *first, ...)
{
	change_actions(TRUE);
}

void disable_actions(const gchar *first, ...)
{
	change_actions(FALSE);
}

static void insert_action(gchar *act_name, GnomeUIInfo mi, GnomeUIInfo ti)
{
	GtrAction *act;
	act=g_new0(GtrAction,1);
	act->menu=mi.widget;
	if (&ti!=&NONE)
		act->tool=ti.widget;
	g_hash_table_insert(acts,act_name,act);
}

static void create_actions(void)
{
	acts=g_hash_table_new(g_str_hash,g_str_equal);
	insert_action("compile",	the_file_menu[0],	the_toolbar[0]);
	insert_action("save",	the_file_menu[3],	the_toolbar[2]);
	insert_action("save_as",	the_file_menu[4],	the_toolbar[3]);
	insert_action("revert",	the_file_menu[5],	NONE);
	insert_action("close",	the_file_menu[6],	NONE);
	//------------------------------------------------
	insert_action("cut",	the_edit_menu[0],	NONE);
	insert_action("copy",	the_edit_menu[1],	NONE);
	insert_action("paste",	the_edit_menu[2],	NONE);
	insert_action("clear",	the_edit_menu[3],	NONE);
	insert_action("find",	the_edit_menu[5],	the_searchbar[6]);
	insert_action("find_again",	the_edit_menu[6],	NONE);
	insert_action("header",	the_edit_menu[8],	the_toolbar[4]);
	//------------------------------------------------
	insert_action("first",	the_messages_menu[0],	the_searchbar[0]);
	insert_action("back",	the_messages_menu[1],	the_searchbar[1]);
	insert_action("next",	the_messages_menu[2],	the_searchbar[2]);
	insert_action("last",	the_messages_menu[3],	the_searchbar[3]);
	insert_action("goto",	the_messages_menu[5],	the_searchbar[4]);
	//------------------------------------------------
	insert_action("translated",	the_msg_status_menu[0],	NONE);
	insert_action("fuzzy",	the_msg_status_menu[1],	NONE);
	insert_action("stick",	the_msg_status_menu[2],	NONE);
	//------------------------------------------------
	insert_action("add",	the_msg_db_menu[0],	the_searchbar[7]);
	insert_action("query",	the_msg_db_menu[1],	the_searchbar[8]);
}

static void free_action(gpointer key, gpointer value, gpointer useless)
{
	// Destroy only the GtrAction structure,
	// we do not need to destroy widgets
	g_free(value);
}

static void destroy_actions(void)
{
	g_hash_table_foreach(acts,free_action,NULL);
	g_hash_table_destroy(acts);
}

void disable_actions_no_file(void)
{
	disable_actions("compile","save","save_as","revert","close",
		"cut","copy","paste","clear",
		"find","find_again","header",
		"first","back","next","last","goto",
		"translated","fuzzy","stick",
		"add",
		NULL);
}

/*
 * The main function, which creates the application
 */
void create_app1 (void)
{
        GtkWidget *search_bar,*tool_bar;
        GtkWidget *vbox1;
        GtkWidget *scrolledwindow1,*scrolledwindow2;

	/**
	* Create the app	
	**/
        app1=gnome_app_new("gtranslator", _("gtranslator"));
        gnome_app_create_menus(GNOME_APP(app1), the_menus);

	/**
	* Create the tool- and search-bar
	**/
        tool_bar=gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,GTK_TOOLBAR_BOTH);
	gnome_app_fill_toolbar(GTK_TOOLBAR(tool_bar),the_toolbar,NULL);
        gnome_app_add_toolbar(GNOME_APP(app1), GTK_TOOLBAR(tool_bar), 
		"tool_bar", GNOME_DOCK_ITEM_BEH_EXCLUSIVE,
		GNOME_DOCK_TOP, 1, 0, 0);

        search_bar=gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,GTK_TOOLBAR_BOTH);
	gnome_app_fill_toolbar(GTK_TOOLBAR(search_bar),the_searchbar,NULL);
	gnome_app_add_toolbar(GNOME_APP(app1), GTK_TOOLBAR(search_bar), 
		"search_bar", GNOME_DOCK_ITEM_BEH_EXCLUSIVE,
		GNOME_DOCK_TOP, 2, 0, 0);
	
	vbox1=gtk_vbox_new(FALSE, 0);
	gnome_app_set_contents(GNOME_APP(app1), vbox1);

	scrolledwindow1=gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow1, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1),
		GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

	text1=gtk_text_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolledwindow1), text1);
	gtk_text_set_editable(GTK_TEXT(text1), FALSE);

	scrolledwindow2=gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow2, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow2),
		GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

	trans_box=gtk_text_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolledwindow2), trans_box);
	gtk_text_set_editable(GTK_TEXT(trans_box), TRUE);

	appbar1=gnome_appbar_new(FALSE, TRUE, GNOME_PREFERENCES_NEVER);
	gnome_app_set_statusbar(GNOME_APP(app1), appbar1);
        // Make menu hints display on the appbar
	gnome_app_install_menu_hints(GNOME_APP(app1), the_menus);

	// create the actions
	create_actions();

	/**
	* The callbacks list
	**/
	gtk_signal_connect(GTK_OBJECT(app1),"delete_event",
		GTK_SIGNAL_FUNC(gtranslator_quit),NULL);	
	gtk_signal_connect(GTK_OBJECT(trans_box),"changed",
		GTK_SIGNAL_FUNC(text_has_got_changed),NULL);
	/**
	* The D'n'D signals
	**/
	gtk_drag_dest_set(GTK_WIDGET(app1), 
		GTK_DEST_DEFAULT_ALL | GTK_DEST_DEFAULT_HIGHLIGHT, 
		dragtypes, sizeof(dragtypes)/sizeof(dragtypes[0]),
		GDK_ACTION_COPY);
	gtk_signal_connect(GTK_OBJECT(app1), "drag_data_received",
        	GTK_SIGNAL_FUNC(gtranslator_dnd), GINT_TO_POINTER(dnd_type));
}

/**
* Goes to the homepage of gtranslator on the web
**/
void call_gtranslator_homepage(GtkWidget *widget,gpointer useless)
{
	gnome_url_show("http://gtranslator.sourceforge.net/");
}

/**
* The text-callbacks
**/
void cut_clipboard(GtkWidget *widget,gpointer useless)
{
	gtk_editable_cut_clipboard(GTK_EDITABLE(trans_box));
}

void copy_clipboard(GtkWidget *widget,gpointer useless)
{
        gtk_editable_copy_clipboard(GTK_EDITABLE(trans_box));
}

void paste_clipboard(GtkWidget *widget,gpointer useless)
{
        gtk_editable_paste_clipboard(GTK_EDITABLE(trans_box));
}

void clear_selection(GtkWidget *widget,gpointer useless)
{
	gtk_editable_delete_selection(GTK_EDITABLE(trans_box));
}

/**
* Set file_changed to TRUE if the text in the translation box has
*  been updated.
* TODO: if any syntax highlighting will be added, it should be caled from here
**/
void text_has_got_changed(GtkWidget *widget,gpointer useless)
{
	if (nothing_changes) return;
	if (!file_changed) {
		file_changed=TRUE;
		enable_actions("save","revert",NULL);
	}
	message_changed=TRUE;
}

/**
* The own quit-code
**/
static gint 
gtranslator_quit(GtkWidget *widget, GdkEventAny* e, gpointer useless)
{
	// If file was changed, but user pressed Cancel, don't quit
	if (!ask_to_save_file()) return TRUE;
	// Close the file
	close_file(NULL,NULL);
	/**
	* Show a Good Bye!-typo message.
	**/
	gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Bye bye!"));
	if(if_save_geometry==TRUE)
	{
		gchar *gstr;
		gint x,y,w,h;
		gstr=gnome_geometry_string(app1->window);
		if (gnome_parse_geometry(gstr, &x, &y,
			&w, &h))
		{
			gnome_config_push_prefix("/gtranslator/");
			gnome_config_set_int("Geometry/X",x);
			gnome_config_set_int("Geometry/Y",y);
			gnome_config_set_int("Geometry/Width",w);
			gnome_config_set_int("Geometry/Height",h);
			gnome_config_pop_prefix();
			gnome_config_sync();
		}
		else
		{
			g_warning(_("Couldn't store current geometry!"));
		}
	}
	/**
	* If the msg_db has been inited, close it.
	**/
	if (msg_db_inited)
		close_msg_db();
	// Destroy the actions
	destroy_actions();
	// Free allocated strings
	free_prefs();
	/**
	* Call the Gtk+ quit-function
	**/
	gtk_main_quit();
	return FALSE;
}

/*
 * Display the message in text boxes
 * TODO: add syntax highlighting for %s, numbers, symbols, tabs;
 * option to replace spaces with centered dot ( hmm, too much gnopo, I guess ;) )
 */
void display_msg(GList *list_item)
{
	GtrMsg *msg;
	msg=GTR_MSG(list_item->data);
	nothing_changes=TRUE;
	clean_text_boxes();
	gtk_text_insert(GTK_TEXT(text1),NULL,NULL,NULL,msg->msgid,-1);
	gtk_text_insert(GTK_TEXT(trans_box),NULL,NULL,NULL,msg->msgstr,-1);
	nothing_changes=FALSE;
#define set_active(number,flag) \
	gtk_check_menu_item_set_active(\
		(GtkCheckMenuItem *)(the_msg_status_menu[number].widget),\
		msg->status & flag);
	set_active(0, GTRANSLATOR_MSG_STATUS_TRANSLATED);
	set_active(1, GTRANSLATOR_MSG_STATUS_FUZZY);
	set_active(2, GTRANSLATOR_MSG_STATUS_STICK);
}

void update_msg(GList *li)
{
	GtrMsg *msg;
	if (!message_changed) return;
	msg=GTR_MSG(li->data);
	g_free(msg->msgstr);
	msg->msgstr=gtk_editable_get_chars(GTK_EDITABLE(trans_box),0,-1);
	get_msg_status(msg);
	message_changed=FALSE;
}

// TODO make actual changes to messages happen
void toggle_msg_status(GtkWidget *widget,gpointer which)
{
	GtrMsgStatus stat=GTR_MSG(cur_msg->data)->status;
	gint flag=GPOINTER_TO_INT(which);
	stat^=flag;
	message_changed=TRUE;
	if (flag==GTRANSLATOR_MSG_STATUS_STICK)
		gtk_editable_set_editable(GTK_EDITABLE(trans_box),
			!(stat & GTRANSLATOR_MSG_STATUS_STICK));
}

/**
* Cleans up the text boxes.
**/
void clean_text_boxes()
{
	gtk_editable_delete_text(GTK_EDITABLE(text1),0,-1);
	gtk_editable_delete_text(GTK_EDITABLE(trans_box),0,-1);
}

// ------------------------------------
// Callbacks for moving around messages
// ------------------------------------

/**
* Get's the first msg.
**/
void goto_first_msg(GtkWidget *widget,gpointer useless)
{
	update_msg(cur_msg);
	cur_msg=g_list_first(messages);
	display_msg(cur_msg);
	disable_actions("first","back",NULL);
}

/**
* Go to the previous message.
**/
void goto_prev_msg(GtkWidget *widget,gpointer useless)
{
	update_msg(cur_msg);
	if (g_list_position(messages,cur_msg)==(g_list_length(messages)-1))
	{
		enable_actions("next","last",NULL);
		gnome_appbar_refresh(GNOME_APPBAR(appbar1));
	}
	cur_msg=g_list_previous(cur_msg);
	display_msg(cur_msg);
	if (g_list_position(messages,cur_msg)==0)
	{
		disable_actions("first","back",NULL);
		gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("You've reached the first message"));
	}
}

/**
* Go to the next message
**/
void goto_next_msg(GtkWidget *widget,gpointer useless)
{
	update_msg(cur_msg);
	if (g_list_position(messages,cur_msg)==0)
	{
		enable_actions("first","back",NULL);
		gnome_appbar_refresh(GNOME_APPBAR(appbar1));
	}
	cur_msg=g_list_next(cur_msg);
	display_msg(cur_msg);
	if (g_list_position(messages,cur_msg)==(g_list_length(messages)-1))
	{
		disable_actions("next","last",NULL);
		gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("You've reached the last message."));
	}
}

/**
* Go to the last entry.
**/
void goto_last_msg(GtkWidget *widget,gpointer useless)
{
	update_msg(cur_msg);
	cur_msg=g_list_last(messages);
	display_msg(cur_msg);
	disable_actions("next","last",NULL);
}

/**
* Go to the nth entry.
**/
void goto_nth_msg(GtkWidget *widget,gpointer adjustment)
{
	gint pos=g_list_position(messages,cur_msg);
	gint len=g_list_length(messages)-1;
	update_msg(cur_msg);
	if (pos==0)
		enable_actions("first","back",NULL);
	else if (pos==len)
		enable_actions("next","last",NULL);
	cur_msg=g_list_nth(messages,GTK_ADJUSTMENT(adjustment)->value-1);
	display_msg(cur_msg);
	pos=g_list_position(messages,cur_msg);
	if (pos==0)
		disable_actions("first","back",NULL);
	else if (pos==len)
		disable_actions("next","last",NULL);
}

/**
* Go to given entry.
**/
void goto_given_msg(GtkWidget *widget,gpointer to_go)
{
	gint pos=g_list_position(messages,cur_msg);
	gint len=g_list_length(messages)-1;
	update_msg(cur_msg);
	if (pos==0)
		enable_actions("first","back",NULL);
	else if (pos==len)
		enable_actions("next","last",NULL);
	cur_msg=(GList *)to_go;
	display_msg(cur_msg);
	pos=g_list_position(messages,cur_msg);
	if (pos==0)
		disable_actions("first","back",NULL);
	else if (pos==len)
		disable_actions("next","last",NULL);
}

/**
* The real search function
* FIXME: this searches only msgstrs!
* and is always case sensitive!
**/
void search_do(GtkWidget *widget,gpointer lost)
{
	static gchar *search_for=NULL;
	GList *msg;
	gchar *pos;
#define msgstr_from(msg) (GTR_MSG(msg->data)->msgstr)
	if (lost)
	{
		if (search_for) g_free(search_for);
		search_for=lost;
	}
	for (msg=cur_msg->next;msg!=NULL;msg=msg->next)
	{
		pos=strstr(msgstr_from(msg),search_for);
		if (pos)
		{
			// We found it
			gint start=pos-msgstr_from(msg);
			goto_given_msg(NULL,msg);
			gtk_editable_select_region(GTK_EDITABLE(trans_box),
				start,start+strlen(search_for));
			return;
		}
	}
	// It's not nice, just copy from above here and change were needed
	for (msg=messages;msg!=cur_msg->next;msg=msg->next)
	{
		pos=strstr(msgstr_from(msg),search_for);
		if (pos)
		{
			// We found it
			gint start=pos-msgstr_from(msg);
			goto_given_msg(NULL,msg);
			gtk_editable_select_region(GTK_EDITABLE(trans_box),
				start,start+strlen(search_for));
			return;
		}
	}
	pos=g_strdup_printf(_("Could not find\n\"%s\""),search_for);
	gnome_app_message(GNOME_APP(app1),pos);
	g_free(pos);
}

