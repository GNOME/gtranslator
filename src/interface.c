/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* The interface file , totally catastrophed by me ...
*	but it does only create the main app ..
* 
* And reformatted ; so it should be much better to read it
*
* -- the source 
**/

#include "interface.h"

/**
* The Gnome-help structure
**/
static GnomeHelpMenuEntry help_me = { 
	"gtranslator", "index.html" 
	};

void call_help_viewer(GtkWidget *widget,gpointer useless)
{
	/**
	* Calls ( hopefully ) the Gnome Helpbrowser
	**/
	gnome_help_pbox_goto(NULL,0,&help_me);
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
**/
void text_has_got_changed(GtkWidget *widget,gpointer useless)
{
	file_changed=TRUE;
}

/**
* The compile function
**/
void compile(GtkWidget *widget,gpointer useless)
{
	if((file_opened!=TRUE))
	{
		gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("No po-file opened which could be compiled."));
	}
	else
	{
		/**
		* Simply call msgfmt with the currently compiled
		*  file
		**/
		/** TODO **/
	}
}

/**
* Disables/greys out the next/... buttons
**/
void disable_buttons()
{
	gtk_widget_set_sensitive(compile_button,FALSE);
	gtk_widget_set_sensitive(save_button,FALSE);
	gtk_widget_set_sensitive(save_as_button,FALSE);
	gtk_widget_set_sensitive(first_button,FALSE);
	gtk_widget_set_sensitive(back_button,FALSE);
	gtk_widget_set_sensitive(next_button,FALSE);
	gtk_widget_set_sensitive(last_button,FALSE);
}

/**
* Enables the buttons from above
**/
void enable_buttons()
{
	gtk_widget_set_sensitive(compile_button,TRUE);
	gtk_widget_set_sensitive(save_button,TRUE);
        gtk_widget_set_sensitive(save_as_button,TRUE);
	gtk_widget_set_sensitive(first_button,TRUE);
        gtk_widget_set_sensitive(back_button,TRUE);
        gtk_widget_set_sensitive(next_button,TRUE);
        gtk_widget_set_sensitive(last_button,TRUE);
}

/**
* The menu-entries
**/
static GnomeUIInfo the_file_menu[] =
{
        {
          GNOME_APP_UI_ITEM, N_("_Compile"),
          NULL,
          compile, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_CONVERT,
          GDK_C, GDK_MOD1_MASK, NULL
        },
        GNOMEUIINFO_MENU_OPEN_ITEM(open_file_show, NULL),
        GNOMEUIINFO_MENU_SAVE_ITEM(NULL, NULL),
        GNOMEUIINFO_MENU_SAVE_AS_ITEM(save_file_as_show, NULL),
        GNOMEUIINFO_SEPARATOR,
        GNOMEUIINFO_MENU_EXIT_ITEM(GTK_SIGNAL_FUNC(gtk_main_quit), NULL),
        GNOMEUIINFO_END
};

static GnomeUIInfo the_edit_menu[] =
{
        GNOMEUIINFO_MENU_CUT_ITEM(cut_clipboard, NULL),
        GNOMEUIINFO_MENU_COPY_ITEM(copy_clipboard, NULL),
        GNOMEUIINFO_MENU_PASTE_ITEM(paste_clipboard, NULL),
        GNOMEUIINFO_MENU_CLEAR_ITEM(clear_selection, NULL),
        GNOMEUIINFO_END
};

static GnomeUIInfo the_view_menu[] =
{
        {
          GNOME_APP_UI_ITEM, N_("Refresh"),
          NULL,
          NULL, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_REFRESH,
          GDK_F5, 0, NULL
        },
        GNOMEUIINFO_END
};

static GnomeUIInfo the_messages_menu[] =
{
        {
          GNOME_APP_UI_ITEM, N_("_First"),
          NULL,
          NULL, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_FIRST,
          GDK_Home, GDK_CONTROL_MASK, NULL
        },
        {
          GNOME_APP_UI_ITEM, N_("_Back"),
          NULL,
          NULL, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BACK,
          GDK_Page_Up, 0, NULL
        },
        GNOMEUIINFO_SEPARATOR,
        {
          GNOME_APP_UI_ITEM, N_("_Next"),
          NULL,
          NULL, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_FORWARD,
          GDK_Page_Down, 0, NULL
        },
        {
          GNOME_APP_UI_ITEM, N_("_Last"),
          NULL,
          NULL, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_LAST,
          GDK_End, GDK_CONTROL_MASK, NULL
        },
        GNOMEUIINFO_END
};

static GnomeUIInfo the_settings_menu[] =
{
        GNOMEUIINFO_MENU_PREFERENCES_ITEM(prefs_box_show, NULL),
        GNOMEUIINFO_END
};

static GnomeUIInfo the_help_menu[] =
{
        GNOMEUIINFO_MENU_ABOUT_ITEM(about_box_show, NULL),
        {
          GNOME_APP_UI_ITEM, N_("_Online help"),
          NULL,
          call_help_viewer, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_OPEN,
          GDK_F1, 0, NULL
        },
        GNOMEUIINFO_END
};

static GnomeUIInfo the_menus[] =
{
        GNOMEUIINFO_MENU_FILE_TREE(the_file_menu),
        GNOMEUIINFO_MENU_EDIT_TREE(the_edit_menu),
        GNOMEUIINFO_MENU_VIEW_TREE(the_view_menu),
        {
          GNOME_APP_UI_SUBTREE, N_("_Messages"),
          NULL,
          the_messages_menu, NULL, NULL,
          GNOME_APP_PIXMAP_NONE, N_("_Messages"),
          0, 0, NULL
        },
        GNOMEUIINFO_MENU_SETTINGS_TREE(the_settings_menu),
        GNOMEUIINFO_MENU_HELP_TREE(the_help_menu),
        GNOMEUIINFO_END
};

GtkWidget*
create_app1 (void)
{
        GtkWidget *dock1;
        GtkWidget *toolbar1;
        GtkWidget *tmp_toolbar_icon;
        GtkWidget *open_button,*cat_button,*po_button;
        GtkWidget *options_button;
        GtkWidget *exit_button;
        GtkWidget *vbox1;
        GtkWidget *scrolledwindow1,*scrolledwindow2;

	/**
	* Create the app	
	**/
        app1=gnome_app_new("gtranslator", _("gtranslator"));
        gtk_window_set_wmclass(GTK_WINDOW(app1), "gtranslator", "gtranslator");
        dock1=GNOME_APP(app1)->dock;
        gtk_widget_show(dock1);
        gnome_app_create_menus(GNOME_APP(app1), the_menus);
        toolbar1=gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
        gtk_widget_show(toolbar1);
	/**
	* Create the search bar
	**/
        search_bar=gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
        gtk_widget_show(search_bar);
        gnome_app_add_toolbar(GNOME_APP(app1), GTK_TOOLBAR(toolbar1), "toolbar1",
                                      GNOME_DOCK_ITEM_BEH_EXCLUSIVE,
                                      GNOME_DOCK_TOP, 1, 0, 0);
        gtk_container_set_border_width(GTK_CONTAINER(toolbar1), 1);
        gtk_toolbar_set_space_size(GTK_TOOLBAR(toolbar1), 16);
        gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar1), GTK_RELIEF_NONE);
	/**
	* Integrate the search_bar
	**/ 
	gnome_app_add_toolbar(GNOME_APP(app1), GTK_TOOLBAR(search_bar), "search_bar",
				GNOME_DOCK_ITEM_BEH_EXCLUSIVE,
				GNOME_DOCK_TOP, 2, 0, 0);
	gtk_container_set_border_width(GTK_CONTAINER(search_bar), 1);
	gtk_toolbar_set_space_size(GTK_TOOLBAR(search_bar), 16);
	gtk_toolbar_set_space_style(GTK_TOOLBAR(search_bar), GTK_TOOLBAR_SPACE_LINE);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(search_bar), GTK_RELIEF_NONE);
	/**
	* Now the "new" icons ...
      	**/
	tmp_toolbar_icon=gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_SEARCH);
	search_button=gtk_toolbar_append_element(GTK_TOOLBAR(search_bar),
				GTK_TOOLBAR_CHILD_BUTTON,
				NULL,
				_("Search"),
				_("Search for a message"),NULL,
				tmp_toolbar_icon , NULL, NULL);
	gtk_widget_show(search_button);	
	tmp_toolbar_icon=gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_REDO);
	search_again_button=gtk_toolbar_append_element(GTK_TOOLBAR(search_bar),
				GTK_TOOLBAR_CHILD_BUTTON,
				NULL,
				_("ReSearch"),
				_("Search again"),NULL,
				tmp_toolbar_icon , NULL, NULL);
	gtk_widget_show(search_again_button);
	tmp_toolbar_icon=gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_JUMP_TO);
	goto_button=gtk_toolbar_append_element(GTK_TOOLBAR(search_bar),
				GTK_TOOLBAR_CHILD_BUTTON,
				NULL,
				_("Goto"),
				_("Goto this message"),NULL,
				tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(goto_button);
	tmp_toolbar_icon=gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_BOOK_RED);
	cat_button=gtk_toolbar_append_element(GTK_TOOLBAR(search_bar),
				GTK_TOOLBAR_CHILD_BUTTON,
				NULL,
				/**
				* Find in the personal catalogue
				**/
				_("F/Cat."),
				_("Find in the personal catalog"),NULL,
				tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(cat_button);
	tmp_toolbar_icon=gnome_stock_pixmap_widget(app1,GNOME_STOCK_PIXMAP_BOOK_OPEN);
	po_button=gtk_toolbar_append_element(GTK_TOOLBAR(search_bar),
				GTK_TOOLBAR_CHILD_BUTTON,
				NULL,
				/**
				* Find in the .po-file
				**/
				_("F/Po."),
				_("Find in the po file "), NULL,
				tmp_toolbar_icon , NULL, NULL);
	gtk_widget_show(po_button);
	/**
	* Add a spacer element ..
	**/
	gtk_toolbar_append_space(GTK_TOOLBAR(search_bar));
	tmp_toolbar_icon=gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_CONVERT);
	compile_button=gtk_toolbar_append_element(GTK_TOOLBAR(toolbar1),
                                      GTK_TOOLBAR_CHILD_BUTTON,
                                      NULL,
                                      _("Compile"),
                                      _("Compile the po-file"), NULL,
                                      tmp_toolbar_icon, NULL, NULL);
       	gtk_widget_show(compile_button);

	tmp_toolbar_icon=gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_OPEN);
	open_button=gtk_toolbar_append_element(GTK_TOOLBAR(toolbar1),
                                      GTK_TOOLBAR_CHILD_BUTTON,
                                      NULL,
                                      _("Open"),
                                      _("Open File"), NULL,
                                      tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(open_button);

	tmp_toolbar_icon=gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_SAVE);
	save_button=gtk_toolbar_append_element(GTK_TOOLBAR(toolbar1),
                                      GTK_TOOLBAR_CHILD_BUTTON,
                                      NULL,
                                      _("Save"),
                                      _("Save File"), NULL,
                                      tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(save_button);

	tmp_toolbar_icon=gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_SAVE_AS);
	save_as_button=gtk_toolbar_append_element(GTK_TOOLBAR(toolbar1),
                                      GTK_TOOLBAR_CHILD_BUTTON,
                                      NULL,
                                      _("Save as"),
                                      _("Save file as"), NULL,
                                      tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(save_as_button);

	tmp_toolbar_icon=gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_FIRST);
	first_button=gtk_toolbar_append_element(GTK_TOOLBAR(toolbar1),
                                      GTK_TOOLBAR_CHILD_BUTTON,
                                      NULL,
                                      _("First"),
                                      _("Go to the first message"), NULL,
                                      tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(first_button);

	tmp_toolbar_icon=gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_BACK);
	back_button=gtk_toolbar_append_element(GTK_TOOLBAR(toolbar1),
                                      GTK_TOOLBAR_CHILD_BUTTON,
                                      NULL,
                                      _("Back"),
                                      _("Go one message back"), NULL,
                                      tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(back_button);

	tmp_toolbar_icon=gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_PREFERENCES);
	options_button=gtk_toolbar_append_element(GTK_TOOLBAR(toolbar1),
                                      GTK_TOOLBAR_CHILD_BUTTON,
                                      NULL,
                                      _("Options"),
                                      _("gtranslator options"), NULL,
                                      tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(options_button);

	tmp_toolbar_icon=gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_FORWARD);
	next_button=gtk_toolbar_append_element(GTK_TOOLBAR(toolbar1),
                                      GTK_TOOLBAR_CHILD_BUTTON,
                                      NULL,
                                      _("Next"),
                                      _("Go one message forward"), NULL,
                                      tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(next_button);

	tmp_toolbar_icon=gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_LAST);
	last_button=gtk_toolbar_append_element(GTK_TOOLBAR(toolbar1),
                                      GTK_TOOLBAR_CHILD_BUTTON,
                                      NULL,
                                      _("Last"),
                                      _("Go to the last message"), NULL,
                                      tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(last_button);

	tmp_toolbar_icon=gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_EXIT);
	exit_button=gtk_toolbar_append_element(GTK_TOOLBAR(toolbar1),
                                      GTK_TOOLBAR_CHILD_BUTTON,
                                      NULL,
                                      _("Exit"),
                                      _("Exit"), NULL,
                                      tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(exit_button);

	vbox1=gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox1);
	gnome_app_set_contents(GNOME_APP(app1), vbox1);

	scrolledwindow1=gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow1);
	gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow1, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

	text1=gtk_text_new(NULL, NULL);
	gtk_widget_show(text1);
	gtk_container_add(GTK_CONTAINER(scrolledwindow1), text1);
	gtk_widget_realize(text1);
	scrolledwindow2=gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow2);
	gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow2, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow2), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

	trans_box=gtk_text_new(NULL, NULL);
	gtk_widget_show(trans_box);
	gtk_container_add(GTK_CONTAINER(scrolledwindow2), trans_box);
	gtk_text_set_editable(GTK_TEXT(trans_box), TRUE);
	gtk_widget_realize(trans_box);

	appbar1=gnome_appbar_new(TRUE, TRUE, GNOME_PREFERENCES_NEVER);
	gtk_widget_show(appbar1);
	gnome_app_set_statusbar(GNOME_APP(app1), appbar1);

	/**
	* The callbacks list
	**/
	gtk_signal_connect(GTK_OBJECT(app1),"delete-event",
		GTK_SIGNAL_FUNC(gtk_main_quit),NULL);	
	gtk_signal_connect(GTK_OBJECT(exit_button),"clicked",
		GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
	gtk_signal_connect(GTK_OBJECT(options_button),"clicked",
		GTK_SIGNAL_FUNC(prefs_box_show),NULL);
	gtk_signal_connect(GTK_OBJECT(compile_button),"clicked",
		GTK_SIGNAL_FUNC(compile),NULL);
	gtk_signal_connect(GTK_OBJECT(search_button),"clicked",
		GTK_SIGNAL_FUNC(s_box),NULL);
	gtk_signal_connect(GTK_OBJECT(open_button),"clicked",
		GTK_SIGNAL_FUNC(open_file_show),NULL);	
	gtk_signal_connect(GTK_OBJECT(save_as_button),"clicked",
		GTK_SIGNAL_FUNC(save_file_as_show),NULL);
	gtk_signal_connect(GTK_OBJECT(trans_box),"changed",
		GTK_SIGNAL_FUNC(text_has_got_changed),NULL);
	/*****************************************************************/
	return app1;
}
