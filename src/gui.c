/**
* Fatih Demir <kabalak@gmx.net>
* Gediminas Paulauskas <menesis@delfi.lt>
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* The interface is created here 
*
* -- the source 
**/

#include "gui.h"
#include "prefs.h"
#include "dialogs.h"
#include "parse.h"
#include "header_stuff.h"
#include "find.h"
#include "spell.h"
#include "dnd.h"
#include "about.h"

#include <libgtranslator/preferences.h>
#include <libgtranslator/stylistics.h>

typedef struct _GtrAction GtrAction;
#define GTR_ACTION(x) ((GtrAction *)x)

struct _GtrAction {
	GtkWidget *menu;
	GtkWidget *tool;
};

/* An array holds all defined actions */
static GtrAction acts[ACT_END];

/* routines for actions */
static void create_actions(void);
static void insert_action(gint act_num, GnomeUIInfo mi, GnomeUIInfo ti);

/* Callbacks for text operations */
static void cut_clipboard(GtkWidget * widget, gpointer useless);
static void copy_clipboard(GtkWidget * widget, gpointer useless);
static void paste_clipboard(GtkWidget * widget, gpointer useless);
static void clear_selection(GtkWidget * widget, gpointer useless);
static void undo_changes(GtkWidget * widget, gpointer useless);
static void text_has_got_changed(GtkWidget * widget, gpointer useless);

static void update_appbar(gint pos);
static void call_gtranslator_homepage(GtkWidget * widget, gpointer useless);
static gint gtranslator_quit(GtkWidget * widget, GdkEventAny * e,
			     gpointer useless);

/**
* The target formats
**/
static  GtkTargetEntry dragtypes[] = {
	{ "text/uri-list", 0, TARGET_URI_LIST },
	{ "text/plain", 0, TARGET_NETSCAPE_URL },
	{ "text/plain", 0, TARGET_TEXT_PLAIN }
};

/**
* The menu-entries
**/

/**
* The recenlty used menu in a little bit different manner ( this is just
*  a placeholder.
**/
static GnomeUIInfo the_last_files_menus[] = {
        GNOMEUIINFO_END
};

/**
* The File menu.
**/
static GnomeUIInfo the_file_menu[] = {
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
	{
	 GNOME_APP_UI_ITEM, N_("Update po-file"),
	 N_("Update the po-file"),
	 update, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_REFRESH,
	 GDK_F5, 0, NULL
	},
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
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(N_("_Header..."),
			       N_("Edit the header"),
			       edit_header,
			       GNOME_STOCK_MENU_PROP),
	GNOMEUIINFO_ITEM_STOCK(N_("_Spell check..."),
			       N_("Check the spelling of translation"),
			       check_spelling,
			       GNOME_STOCK_MENU_SPELLCHECK),
	GNOMEUIINFO_END
};

static GnomeUIInfo the_messages_menu[] = {
	{
	 GNOME_APP_UI_ITEM, N_("_First"),
	 N_("Go to the first message"),
	 goto_first_msg, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_FIRST,
	 GDK_Home, GDK_CONTROL_MASK, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("_Back"),
	 N_("Go one message back"),
	 goto_prev_msg, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BACK,
	 GDK_Page_Up, 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("_Next"),
	 N_("Go one message forward"),
	 goto_next_msg, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_FORWARD,
	 GDK_Page_Down, 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("_Last"),
	 N_("Go to the last message"),
	 goto_last_msg, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_LAST,
	 GDK_End, GDK_CONTROL_MASK, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("_Go to..."),
	 N_("Goto specified message number"),
	 goto_dlg, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_JUMP_TO,
	 'G', GDK_CONTROL_MASK, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Next fuz_zy"),
	 N_("Go to next fuzzy message"),
	 goto_next_fuzzy, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_RED,
	 'Z', GDK_MOD1_MASK, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Next _untranslated"),
	 N_("Go to next untranslated message"),
	 goto_next_untranslated, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_OPEN,
	 'U', GDK_MOD1_MASK, NULL},
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
	 0, 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("_Fuzzy"),
	 N_("Toggle fuzzy status of a message"),
	 toggle_msg_status,
	 GINT_TO_POINTER(GTR_MSG_STATUS_FUZZY),
	 NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_RED,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("_Stick"),
	 N_("Stick this message"),
	 toggle_msg_status,
	 GINT_TO_POINTER(GTR_MSG_STATUS_STICK),
	 NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_STOP,
	 0, 0, NULL},
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
	GNOMEUIINFO_SUBTREE(N_("Message _status"), the_msg_status_menu),
	GNOMEUIINFO_MENU_SETTINGS_TREE(the_settings_menu),
	GNOMEUIINFO_MENU_HELP_TREE(the_help_menu),
	GNOMEUIINFO_END
};

/** 
* The toolbar buttons
**/
static GnomeUIInfo the_toolbar[] = {
	GNOMEUIINFO_ITEM_STOCK(N_("Compile"),
			       N_("Compile the po-file"),
			       compile,
			       GNOME_STOCK_PIXMAP_CONVERT),
	GNOMEUIINFO_ITEM_STOCK(N_("Open"),
			       N_("Open a po-file"),
			       open_file,
			       GNOME_STOCK_PIXMAP_OPEN),
	GNOMEUIINFO_ITEM_STOCK(N_("Update"),
			       N_("Update the po-file"),
			       update,
			       GNOME_STOCK_PIXMAP_REFRESH),		       
	GNOMEUIINFO_ITEM_STOCK(N_("Save"),
			       N_("Save File"),
			       save_current_file,
			       GNOME_STOCK_PIXMAP_SAVE),
	GNOMEUIINFO_ITEM_STOCK(N_("Save as"),
			       N_("Save file with a different name"),
			       save_file_as,
			       GNOME_STOCK_PIXMAP_SAVE_AS),
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

static GnomeUIInfo the_searchbar[] = {
	GNOMEUIINFO_ITEM_STOCK(N_("First"),
			       N_("Go to the first message"),
			       goto_first_msg,
			       GNOME_STOCK_PIXMAP_FIRST),
	GNOMEUIINFO_ITEM_STOCK(N_("Back"),
			       N_("Go one message back"),
			       goto_prev_msg,
			       GNOME_STOCK_PIXMAP_BACK),
	GNOMEUIINFO_ITEM_STOCK(N_("Next"),
			       N_("Go one message forward"),
			       goto_next_msg,
			       GNOME_STOCK_PIXMAP_FORWARD),
	GNOMEUIINFO_ITEM_STOCK(N_("Last"),
			       N_("Go to the last message"),
			       goto_last_msg,
			       GNOME_STOCK_PIXMAP_LAST),
	GNOMEUIINFO_ITEM_STOCK(N_("Missing"),
			       N_("Go to next untranslated message"),
			       goto_next_untranslated,
			       GNOME_STOCK_PIXMAP_BOOK_OPEN),
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
	GNOMEUIINFO_END
};

/**
* The popup-menu.
**/
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

/**
* Pop's up the curious popup-menu.
**/
void create_popup_menu(GtkWidget *widget, GdkEventButton *event)
{
	GtkWidget *popup_menu;
	/**
	* Only react on rightclick.
	**/
	if(event->button==3)
	{
		popup_menu=gnome_popup_menu_new(the_popup_menu);
		/**
		* Only respond if a file has been present/opened and if the corresponding
		*  option is set.
		**/
		if((file_opened==TRUE) && (wants.popup_menu))
		{
			gnome_popup_menu_do_popup_modal(popup_menu, NULL, NULL, NULL, event);
			/**
			* Destroy the menu after creation.
			**/
			gtk_widget_destroy(popup_menu);
		}
	}	
}

/*****
 * Actions stuff goes here
 *****/
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
		id++; 
	} 
	va_end(ap);
}

/* a unused variable for testing if a toolbar element was provided */
static GnomeUIInfo NONE;

static void insert_action(gint act_num, GnomeUIInfo mi, GnomeUIInfo ti)
{
	acts[act_num].menu = mi.widget;
	acts[act_num].tool = ti.widget;
}

static void create_actions(void)
{
	NONE.widget = NULL;
	insert_action(ACT_COMPILE, the_file_menu[0], the_toolbar[0]);
	insert_action(ACT_SAVE, the_file_menu[3], the_toolbar[3]);
	insert_action(ACT_SAVE_AS, the_file_menu[4], the_toolbar[4]);
	insert_action(ACT_REVERT, the_file_menu[5], NONE);
	insert_action(ACT_CLOSE, the_file_menu[6], NONE);
	insert_action(ACT_UPDATE, the_file_menu[8], the_toolbar[2]);
	/*------------------------------------------------ */
	insert_action(ACT_UNDO, the_edit_menu[0], NONE);
	insert_action(ACT_CUT, the_edit_menu[2], NONE);
	insert_action(ACT_COPY, the_edit_menu[3], NONE);
	insert_action(ACT_PASTE, the_edit_menu[4], NONE);
	insert_action(ACT_CLEAR, the_edit_menu[5], NONE);
	insert_action(ACT_FIND, the_edit_menu[7], the_searchbar[7]);
	insert_action(ACT_FIND_AGAIN, the_edit_menu[8], NONE);
	insert_action(ACT_HEADER, the_edit_menu[10], the_toolbar[5]);
	insert_action(ACT_SPELL, the_edit_menu[11], NONE);
	/*------------------------------------------------ */
	insert_action(ACT_FIRST, the_messages_menu[0], the_searchbar[0]);
	insert_action(ACT_BACK, the_messages_menu[1], the_searchbar[1]);
	insert_action(ACT_NEXT, the_messages_menu[2], the_searchbar[2]);
	insert_action(ACT_LAST, the_messages_menu[3], the_searchbar[3]);
	insert_action(ACT_GOTO, the_messages_menu[5], the_searchbar[6]);
	insert_action(ACT_NEXT_FUZZY, the_messages_menu[6], the_searchbar[5]);
	insert_action(ACT_NEXT_UNTRANSLATED, the_messages_menu[7], the_searchbar[4]);
	/*------------------------------------------------ */
	insert_action(ACT_TRANSLATED, the_msg_status_menu[0], NONE);
	insert_action(ACT_FUZZY, the_msg_status_menu[1], NONE);
	insert_action(ACT_STICK, the_msg_status_menu[2], NONE);
	/*------------------------------------------------ */
	insert_action(ACT_END, NONE, NONE);
}

void disable_actions_no_file(void)
{
	disable_actions(ACT_COMPILE, ACT_SAVE, ACT_SAVE_AS, ACT_REVERT,
			ACT_CLOSE, ACT_CUT, ACT_COPY, ACT_PASTE, ACT_CLEAR,
			ACT_FIND, ACT_FIND_AGAIN, ACT_HEADER, ACT_FIRST,
			ACT_BACK, ACT_NEXT, ACT_LAST, ACT_GOTO, ACT_UNDO,
			ACT_NEXT_FUZZY,  ACT_UPDATE, ACT_STICK, ACT_FUZZY,
			ACT_TRANSLATED, ACT_NEXT_UNTRANSLATED);
	gtk_text_set_editable(GTK_TEXT(trans_box), FALSE);
}

void enable_actions_just_opened(void)
{
	enable_actions(ACT_COMPILE, ACT_SAVE_AS, ACT_CLOSE, ACT_CUT, ACT_COPY,
		       ACT_PASTE, ACT_CLEAR, ACT_FIND, ACT_HEADER, ACT_NEXT,
		       ACT_LAST, ACT_GOTO, ACT_TRANSLATED,
		       ACT_STICK);
	/**
	* If we'd have the option to use the update function set, enable the
	*  Update button in the toolbar and in the menu.
	**/	       
	if(wants.update_function)
	{
		enable_actions(ACT_UPDATE);	
	}  
	/**
	* Enable the editing of the msgstrs :-)
	**/
	gtk_text_set_editable(GTK_TEXT(trans_box), TRUE);
}

/*
 * The main function, which creates the application
 */
void create_app1(void)
{
	GtkWidget *search_bar, *tool_bar;
	GtkWidget *vbox1;
	GtkWidget *scrolledwindow1, *scrolledwindow2;

	/**
	* Create the app	
	**/
	app1 = gnome_app_new("gtranslator", _("gtranslator"));
	gnome_app_create_menus(GNOME_APP(app1), the_menus);

	/**
	* Create the tool- and search-bar
	**/
	tool_bar =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
	gnome_app_fill_toolbar(GTK_TOOLBAR(tool_bar), the_toolbar, NULL);
	gnome_app_add_toolbar(GNOME_APP(app1), GTK_TOOLBAR(tool_bar),
			      "tool_bar", GNOME_DOCK_ITEM_BEH_EXCLUSIVE,
			      GNOME_DOCK_TOP, 1, 0, 0);

	search_bar = gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
				     GTK_TOOLBAR_BOTH);
	gnome_app_fill_toolbar(GTK_TOOLBAR(search_bar), the_searchbar, NULL);
	gnome_app_add_toolbar(GNOME_APP(app1), GTK_TOOLBAR(search_bar),
			      "search_bar", GNOME_DOCK_ITEM_BEH_EXCLUSIVE,
			      GNOME_DOCK_TOP, 2, 0, 0);

	vbox1 = gtk_vbox_new(FALSE, 0);
	gnome_app_set_contents(GNOME_APP(app1), vbox1);

	scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow1, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	text1 = gtk_text_new(NULL, NULL);
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
	/* Make menu hints display on the appbar */
	gnome_app_install_menu_hints(GNOME_APP(app1), the_menus);

	create_actions();

	gtranslator_display_recent();

	/**
	* Check if we'd to use special styles.
	**/
	if(wants.use_own_specs)
	{
		gtranslator_set_style(text1, trans_box);
	}
	
	/**
	* The callbacks list
	**/
	gtk_signal_connect(GTK_OBJECT(app1), "delete_event",
			   GTK_SIGNAL_FUNC(gtranslator_quit), NULL);
	gtk_signal_connect(GTK_OBJECT(trans_box), "changed",
			   GTK_SIGNAL_FUNC(text_has_got_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(text1), "button_press_event",
			   GTK_SIGNAL_FUNC(create_popup_menu), NULL);
	gtk_signal_connect(GTK_OBJECT(trans_box), "button_press_event",
			   GTK_SIGNAL_FUNC(create_popup_menu), NULL);
	/**
	* The D'n'D signals
	**/
	gtk_drag_dest_set(GTK_WIDGET(app1),
			  GTK_DEST_DEFAULT_ALL | GTK_DEST_DEFAULT_HIGHLIGHT,
			  dragtypes, sizeof(dragtypes) / sizeof(dragtypes[0]),
			  GDK_ACTION_COPY);
	gtk_signal_connect(GTK_OBJECT(app1), "drag_data_received",
			   GTK_SIGNAL_FUNC(gtranslator_dnd),
			   GUINT_TO_POINTER(dnd_type));
}

/**
* The own quit-code
**/
static gint gtranslator_quit(GtkWidget * widget, GdkEventAny * e,
			     gpointer useless)
{
	/* If file was changed, but user pressed Cancel, don't quit */
	if (!ask_to_save_file())
		return TRUE;
	close_file(NULL, NULL);
	gnome_appbar_set_status(GNOME_APPBAR(appbar1), _("Bye bye!"));
	save_geometry();
	/**
	* Free the preferences stuff.
	**/
	free_prefs();
	gnome_regex_cache_destroy(rxc);
	/**
	* Store the current date.
	**/
	gtranslator_config_set_last_run_date();
	gtk_main_quit();
	return FALSE;
}

/* 
 * Display the message in text boxes
 * TODO: add syntax highlighting for %s, numbers, symbols, tabs; option to 
 * replace spaces with centered dot; and append a char at the end of message
 */
void display_msg(GList * list_item)
{
	GtrMsg *msg;
	msg = GTR_MSG(list_item->data);
	nothing_changes = TRUE;
	clean_text_boxes();
	/**
	* Substitute the free spaces in the msgid only if this is wished and
	*  possible.
	**/ 
	if(wants.dot_char)
	{
		/**
		* A variable for the length of the strings.
		**/
		guint len;
		if(msg->msgid)
		{
			/**
			* Go through the characters and search for free spaces and replace them
			*  with '·''s.
			**/
			for(len=0;len<strlen(msg->msgid);++len)
			{
				/**
				* Do we have got a free space ?
				**/
				if(msg->msgid[len]==' ')
				{
					/**
					* Then substitute it with a '·'.
					**/
					msg->msgid[len]='·';
				}
			}
		}
		/**
		* And operate in the same way for the msgstr.
		**/
		if(msg->msgstr)
		{	
			for(len=0;len<strlen(msg->msgstr);++len)
			{
				if(msg->msgstr[len]==' ')
				{
					msg->msgstr[len]='·';
				}
			}
		}
	}	
	/**
	* Insert the changed text into the two text-boxes.
	**/
	gtk_text_insert(GTK_TEXT(text1), NULL, NULL, NULL, msg->msgid, -1);
	gtk_text_insert(GTK_TEXT(trans_box), NULL, NULL, NULL, msg->msgstr, -1);
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
		/**
		* Do all this only if a STRING is in the boxes, don't operate on
		*  blank boxes ...
		**/
		if(msg->msgid)
		{
			if (msg->msgid[strlen(msg->msgid) - 1] == '\n') {
				if (GTK_TEXT_INDEX(GTK_TEXT(trans_box), len -1 )
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
		}
		msg->msgstr = gtk_editable_get_chars(GTK_EDITABLE(trans_box),
						     0, len);
		msg->status |= GTR_MSG_STATUS_TRANSLATED;
	} else {
		msg->msgstr = NULL;
		msg->status &= ~GTR_MSG_STATUS_TRANSLATED;
	}
	message_changed = FALSE;
	/**
	* Update the statusbar informations.
	**/
	update_appbar(g_list_position(po->messages, po->current));
}

void toggle_msg_status(GtkWidget * item, gpointer which)
{
	GtrMsgStatus *stat = &(GTR_MSG(po->current->data)->status);
	gint flag = GPOINTER_TO_INT(which);
	if (nothing_changes)
		return;
	text_has_got_changed(NULL, NULL);
	/* We've got only one case done -- fuzzy */
	if (flag == GTR_MSG_STATUS_FUZZY) {
		mark_msg_fuzzy(GTR_MSG(po->current->data),
			       GTK_CHECK_MENU_ITEM(item)->active);
		/**
		* Test if the fuzzy item is acive.
		**/	       
		if(GTK_CHECK_MENU_ITEM(item)->active==TRUE)
		{
			/**
			* And set the fuzzy count.
			**/
			po->fuzzy--;
			/**
			* And step up with the translated count.
			**/
			po->translated++;
		}	       
		/**
		* Also update the status information in the statusbar.
		**/	       
		update_appbar(g_list_position(po->messages, po->current));
	} else {
		if (GTK_CHECK_MENU_ITEM(item)->active)
		{
			*stat |= flag;
		}	
		else
		{
			*stat &= ~flag;
		}
		/**
		* Again the appbar update.
		**/
		update_appbar(g_list_position(po->messages, po->current));
	}
	update_msg();
}

/**
* Cleans up the text boxes.
**/
void clean_text_boxes()
{
	gtk_editable_delete_text(GTK_EDITABLE(text1), 0, -1);
	gtk_editable_delete_text(GTK_EDITABLE(trans_box), 0, -1);
}

static void update_appbar(gint pos)
{
	gchar *str;
	GtrMsg *msg=g_new0(GtrMsg,1);
	gnome_appbar_pop(GNOME_APPBAR(appbar1));
	/**
	* Assign the first part.
	**/
	str=g_strdup_printf(_("Message %d / %d / Status:"), pos + 1, po->length);
	/**
	* Get the message.
	**/
	msg=GTR_MSG(po->current->data);
	/**
	* And append according to the message status the status name.
	**/
	if(msg->status & GTR_MSG_STATUS_FUZZY)
	{
		if(po->fuzzy>1)
		{
			str=g_strdup_printf(_("%s %s [ %i Fuzzy left ]"), str, _("Fuzzy"), po->fuzzy);
		}	
		else
		{
			str=g_strdup_printf(_("%s %s [ No fuzzy left ]"), str, _("Fuzzy"));
		}
	}
	if(msg->status & GTR_MSG_STATUS_TRANSLATED)
	{
		str=g_strdup_printf("%s %s", str, _("Translated"));
	}
	else
	{
		if(msg->status & GTR_MSG_STATUS_STICK)
		{
			str=g_strdup_printf("%s %s", str, _("Stick"));
		}
		else
		{
			if(((g_list_length(po->messages))-po->translated)>1)
			{
				guint missya;
				missya=((g_list_length(po->messages))-po->translated);
				str=g_strdup_printf(_("%s %s [ %i Untranslated left ]"), str, _("Untranslated"), missya);
			}
			else
			{
				str=g_strdup_printf(_("%s %s [ No untranslated left ]"), str, _("Untranslated"));
			}	
		}
	}
	/**
	* Set the appbar text.
	**/
	gnome_appbar_push(GNOME_APPBAR(appbar1), str);
	/**
	* Update the progressbar.
	**/
	gtranslator_set_progress_bar();
	/**
	* And free the allocated string.
	**/
	g_free(str);
}

/* Updates current msg, and shows to_go msg instead, also adjusts actions */
void goto_given_msg(GList * to_go)
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

/**
 * Callbacks for moving around messages 
 **/
void goto_first_msg(GtkWidget * widget, gpointer useless)
{
	goto_given_msg(g_list_first(po->messages));
}

void goto_prev_msg(GtkWidget * widget, gpointer useless)
{
	goto_given_msg(g_list_previous(po->current));
}

void goto_next_msg(GtkWidget * widget, gpointer useless)
{
	goto_given_msg(g_list_next(po->current));
}

void goto_last_msg(GtkWidget * widget, gpointer useless)
{
	goto_given_msg(g_list_last(po->messages));
}

void goto_nth_msg(GtkWidget * widget, gpointer number)
{
	goto_given_msg(g_list_nth(po->messages, GPOINTER_TO_UINT(number)));
}

/* Goes to the homepage of gtranslator on the web */
static void call_gtranslator_homepage(GtkWidget * widget, gpointer useless)
{
	gnome_url_show("http://gtranslator.sourceforge.net/");
}

/**
* The text oriented callbacks
**/
static void cut_clipboard(GtkWidget * widget, gpointer useless)
{
	gtk_editable_cut_clipboard(GTK_EDITABLE(trans_box));
}

static void copy_clipboard(GtkWidget * widget, gpointer useless)
{
	gtk_editable_copy_clipboard(GTK_EDITABLE(trans_box));
}

static void paste_clipboard(GtkWidget * widget, gpointer useless)
{
	gtk_editable_paste_clipboard(GTK_EDITABLE(trans_box));
}

static void clear_selection(GtkWidget * widget, gpointer useless)
{
	gtk_editable_delete_selection(GTK_EDITABLE(trans_box));
}

static void undo_changes(GtkWidget * widget, gpointer useless)
{
	display_msg(po->current);
	disable_actions(ACT_UNDO);
}

/**
* Set po->file_changed to TRUE if the text in the translation box has been
* updated.
* TODO if any syntax highlighting will be added, it should be caled from here
**/
static void text_has_got_changed(GtkWidget * widget, gpointer useless)
{
	if (nothing_changes)
		return;
	if (!po->file_changed) {
		po->file_changed = TRUE;
		enable_actions(ACT_SAVE, ACT_REVERT, ACT_UNDO);
		if((GTR_MSG(po->current->data)->status | GTR_MSG_STATUS_FUZZY)||
			GTR_MSG(po->current->data)->status | GTR_MSG_STATUS_TRANSLATED)
		{
			/**
			* Increment the translated count.
			**/
			po->translated++;
			/**
			* Set the translated flag.
			**/
			GTR_MSG(po->current->data)->status &= ~GTR_MSG_STATUS_TRANSLATED;
			/**
			* Update the appbar.
			**/
			update_appbar(g_list_position(po->messages, po->current));
		}
	}
	if (!message_changed) {
		GtrMsg *msg = GTR_MSG(po->current->data);
		message_changed = TRUE;
		enable_actions(ACT_UNDO);
		if ((wants.unmark_fuzzy) 
		     && (msg->status & GTR_MSG_STATUS_FUZZY)) {
		     	mark_msg_fuzzy(msg, FALSE);
			gtk_check_menu_item_set_active(
			    (GtkCheckMenuItem *) acts[ACT_FUZZY].menu, FALSE);
			/**
			* Decrement the fuzzy count.
			**/
			po->fuzzy--;
			/**
			* And increment the translated count.
			**/ 
			po->translated++;
		}
		/**
		* Also update the status information in the statusbar.
		**/    
		update_appbar(g_list_position(po->messages,
			po->current));
	}
	/**
	* Do all these steps only if the option to use the '·' is set.
	**/
	if(wants.dot_char)
	{
		/**
		* The gchar for the text in the translation box which can be changed by
		*  the user and a guint variable for the length of this text.
		**/
		gchar *newstr=g_new0(gchar,1);
		guint len, index=1;
		/**
		* Get the current pointer poasition.
		**/
		index=gtk_editable_get_position(GTK_EDITABLE(trans_box));
		/**
		* Freeze the translation box.
		**/
		gtk_text_freeze(GTK_TEXT(trans_box));
		/**
		* Get the text from the translation box.
		**/
		newstr=gtk_editable_get_chars(GTK_EDITABLE(trans_box), 0, -1);
		/**
		* Parse the characters for a free space and replace
		*  them with the '·'.
		**/
		for(len=0;len<=(strlen(newstr));++len)
		{
			if(newstr[len]==' ')
			{
				newstr[len]='·';
			}
		}
		/**
		* Go to the first index in the box.
		**/
		gtk_text_set_point(GTK_TEXT(trans_box), 0); 
		/**
		* Clean up the translation box.
		**/
		gtk_text_forward_delete(GTK_TEXT(trans_box),
			gtk_text_get_length(GTK_TEXT(trans_box)));
		/**
		* Insert the changed text with the '·''s.
		**/
		gtk_text_insert(GTK_TEXT(trans_box), NULL, NULL, NULL,
			newstr, -1);
		/**
		* Go to the old text index.
		**/
		gtk_editable_set_position(GTK_EDITABLE(trans_box), index);
		/**
                * Thaw up the translation box to avoid the reverse writing
                *  feature.
                **/
		gtk_text_thaw(GTK_TEXT(trans_box));
	}
}
