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

#include "gui.h"

/**
* The Gnome-help structure
**/
static GnomeHelpMenuEntry help_me = { 
	"gtranslator", "index.html" 
	};

/**
* Calls the general help for gtranslator
**/
void call_help_viewer(GtkWidget *widget,gpointer useless)
{
	/**
	* Calls ( hopefully ) the Gnome Helpbrowser
	**/
	gnome_help_pbox_goto(NULL,0,&help_me);
}

/**
* Goes to the homepage of gtranslator on the web
**/
void call_gtranslator_homepage(GtkWidget *widget,gpointer useless)
{
	/**
	* Calls your selected webpages-viewer for GNOME.
	**/
	gnome_url_show("http://gtranslator.sourceforge.net");
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
		* Simply compile the .po-file
		**/
		gchar cmd[256];
		gint res=1;
		/**
		* If a filename has been set yet, then we can try to compile it.
		**/
		if(po->filename)
		{
			sprintf(cmd,"%s %s","msgfmt",(gchar *)(po->filename));
			res=system(cmd);
			/**
			* If there has been an error show an error-box
			**/
			if(res!=0)
			{
				compile_error_dialog(NULL,NULL);
			}
			else
			{
				gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Po-file has been compiled successfully."));
			}
		}
	}
}

/**
* The own quit-code
**/
gint gtranslator_quit(GtkWidget *widget,gpointer useless)
{
	if(if_save_geometry==TRUE)
	{
		gchar *gstr;
		gint x,y,w,h;
		gstr=gnome_geometry_string(app1->window);
		if(gnome_parse_geometry(gstr, &x, &y,
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
	if(msg_db_inited==TRUE)
	{
		close_msg_db();
	}    
	/**
	* Show a Good Bye!-typo message.
	**/
	gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Bye bye!"));
	/**
	* Call the Gtk+ quit-function
	**/
	gtk_main_quit();
}

/**
* Saves the entry in the msg_db.
**/
void append_to_msg_db(GtkWidget *widget,gpointer useless)
{
	/**
	* Build two temporary gchar's.
	**/
	gchar *tc1=g_new(gchar,1);
	gchar *tc2=g_new(gchar,1);
	/**
	* Get the text-entries.
	**/
	tc1=gtk_editable_get_chars(GTK_EDITABLE(trans_box), 0, gtk_text_get_length(GTK_TEXT(trans_box)));
	tc2=gtk_editable_get_chars(GTK_EDITABLE(text1), 0, gtk_text_get_length(GTK_TEXT(text1)));
	/**
	* Call the msg_db-function for adding new entries.
	**/
	put_to_msg_db(tc2,tc1);
	/**
	* ... and free the gchar's, if possible.
	**/
	if(tc1)
	{
		g_free(tc1);
	}
	if(tc2)
	{
		g_free(tc2);
	}	
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
	GNOMEUIINFO_SEPARATOR,
        GNOMEUIINFO_MENU_OPEN_ITEM(open_file, NULL),
        GNOMEUIINFO_MENU_SAVE_ITEM(NULL, NULL),
        GNOMEUIINFO_MENU_SAVE_AS_ITEM(save_file_as, NULL),
        GNOMEUIINFO_SEPARATOR,
        GNOMEUIINFO_MENU_EXIT_ITEM(GTK_SIGNAL_FUNC(gtranslator_quit), NULL),
        GNOMEUIINFO_END
};

static GnomeUIInfo the_edit_menu[] =
{
        GNOMEUIINFO_MENU_CUT_ITEM(cut_clipboard, NULL),
        GNOMEUIINFO_MENU_COPY_ITEM(copy_clipboard, NULL),
        GNOMEUIINFO_MENU_PASTE_ITEM(paste_clipboard, NULL),
        {
		GNOME_APP_UI_ITEM, N_("C_lear"),
		N_("Clears the current selection"),
		NULL, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_TRASH,
		'K', GDK_CONTROL_MASK, NULL
	},
	GNOMEUIINFO_SEPARATOR,
	{
		GNOME_APP_UI_ITEM, N_("Search"),
		N_("Search"),
		s_box, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_SEARCH,
		'S', GDK_CONTROL_MASK, NULL	
	},
	{
		GNOME_APP_UI_ITEM, N_("ReSearch"),
		N_("Search again"),
		search_do, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_REDO,
		'R', GDK_CONTROL_MASK, NULL
	},
	GNOMEUIINFO_SEPARATOR,
	{
		GNOME_APP_UI_ITEM, N_("Find"),
		N_("Find"),
		find_dialog, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_SEARCH,
		'F', GDK_CONTROL_MASK, NULL

	},
	GNOMEUIINFO_SEPARATOR,
	{
		GNOME_APP_UI_ITEM, N_("_Header"),
		N_("Edit the header"),
		edit_header, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_INDEX,
		0, 0, NULL
	},
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
          get_first_msg, NULL, NULL,
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
          get_last_msg, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_LAST,
          GDK_End, GDK_CONTROL_MASK, NULL
        },
        GNOMEUIINFO_END
};

static GnomeUIInfo the_msg_status_menu [] =
{
	{
		GNOME_APP_UI_ITEM, N_("Translated"),
		NULL,
		NULL, NULL, N_("Translated"),
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_GREEN,
		0, 0, NULL
	},
	GNOMEUIINFO_SEPARATOR,
	{
		GNOME_APP_UI_ITEM, N_("Untranslated"),
                NULL,
                NULL, NULL, N_("Untranslated"),
                GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_RED,
                0, 0, NULL
	},
	{
		GNOME_APP_UI_ITEM, N_("FIXME"),
		NULL,
		NULL, NULL, N_("FIXME"),
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_OPEN,
                0, 0, NULL 
	},
	GNOMEUIINFO_SEPARATOR,
	{
		GNOME_APP_UI_ITEM, N_("Stick"),
		NULL,
		NULL, NULL, N_("Stick this message"),
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_INDEX,
		0, 0, NULL
	},
	GNOMEUIINFO_END
};

static GnomeUIInfo the_msg_db_menu [] =
{
	{
		GNOME_APP_UI_ITEM, N_("_Add the current message"),
		N_("Add the current message to the messages DB."),
		append_to_msg_db, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_FORWARD,
		0, 0, NULL
	},
	GNOMEUIINFO_SEPARATOR,
	{
		GNOME_APP_UI_ITEM, N_("_Query for a message"),
		N_("Query the messages DB for the current msgid."),
		query_dialog, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_JUMP_TO,
		0, 0, NULL
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
	GNOMEUIINFO_SEPARATOR,
	{
		GNOME_APP_UI_ITEM, N_("gtranslator _website"),
		N_("gtranslator's homepage on the web"),
		call_gtranslator_homepage, NULL, NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_HOME,
		0, 0, NULL
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
	{
		GNOME_APP_UI_SUBTREE, N_("Message _status"),
		NULL,
		the_msg_status_menu, NULL, NULL,
		GNOME_APP_PIXMAP_NONE, N_("Message _status"),
		0, 0, NULL
	},
	{
		GNOME_APP_UI_SUBTREE, N_("Message _db"),
		NULL,
		the_msg_db_menu, NULL, NULL,
		GNOME_APP_PIXMAP_NONE, N_("Message _db"),
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
	 /**
        * Add a spacer element ..
        **/
        gtk_toolbar_append_space(GTK_TOOLBAR(search_bar));
	/**
	* Add an `Add'-button to the search-bar.
	**/
	tmp_toolbar_icon=gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_ADD);
	add_button=gtk_toolbar_append_element(GTK_TOOLBAR(search_bar),
				GTK_TOOLBAR_CHILD_BUTTON,
				NULL,
				_("Add"),
				_("Add to the messages db."),NULL,
				tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(add_button);
	 /**
        * Add a spacer element ..
        **/
        gtk_toolbar_append_space(GTK_TOOLBAR(search_bar));
	tmp_toolbar_icon=gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_BOOK_RED);
	cat_button=gtk_toolbar_append_element(GTK_TOOLBAR(search_bar),
				GTK_TOOLBAR_CHILD_BUTTON,
				NULL,
				/**
				* Find in the personal catalogue
				**/
				_("F/DB"),
				_("Find in the personal messages db"),NULL,
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
	tmp_toolbar_icon=gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_INDEX);
	header_button=gtk_toolbar_append_element(GTK_TOOLBAR(search_bar),
					GTK_TOOLBAR_CHILD_BUTTON,
					NULL,
					_("Header"),
					_("Edit the header informations"),NULL,
					tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(header_button);

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
	* Use the window-icon feature of gnome-libs, if available.
	**/
	#ifdef USE_WINDOW_ICON
	gnome_window_icon_set_from_file(GTK_WINDOW(app1),WINDOW_ICON);
	#endif
	/**
	* The callbacks list
	**/
	gtk_signal_connect(GTK_OBJECT(app1),"delete-event",
		GTK_SIGNAL_FUNC(gtranslator_quit),NULL);	
	gtk_signal_connect(GTK_OBJECT(exit_button),"clicked",
		GTK_SIGNAL_FUNC(gtranslator_quit),NULL);
	gtk_signal_connect(GTK_OBJECT(first_button),"clicked",
		GTK_SIGNAL_FUNC(get_first_msg),NULL);
	gtk_signal_connect(GTK_OBJECT(back_button),"clicked",
		GTK_SIGNAL_FUNC(get_prev_msg),NULL);
	gtk_signal_connect(GTK_OBJECT(next_button),"clicked",
		GTK_SIGNAL_FUNC(get_next_msg),NULL);
	gtk_signal_connect(GTK_OBJECT(last_button),"clicked",
		GTK_SIGNAL_FUNC(get_last_msg),NULL);
	gtk_signal_connect(GTK_OBJECT(options_button),"clicked",
		GTK_SIGNAL_FUNC(prefs_box_show),NULL);
	gtk_signal_connect(GTK_OBJECT(compile_button),"clicked",
		GTK_SIGNAL_FUNC(compile),NULL);
	gtk_signal_connect(GTK_OBJECT(search_button),"clicked",
		GTK_SIGNAL_FUNC(s_box),NULL);
	gtk_signal_connect(GTK_OBJECT(search_again_button),"clicked",
		GTK_SIGNAL_FUNC(search_do),(gpointer)2);
	gtk_signal_connect(GTK_OBJECT(goto_button),"clicked",
		GTK_SIGNAL_FUNC(goto_dlg),NULL);
	gtk_signal_connect(GTK_OBJECT(add_button),"clicked",
		GTK_SIGNAL_FUNC(append_to_msg_db),NULL);	
	gtk_signal_connect(GTK_OBJECT(cat_button),"clicked",
		GTK_SIGNAL_FUNC(find_dialog),(gpointer)10);
	gtk_signal_connect(GTK_OBJECT(po_button),"clicked",
		GTK_SIGNAL_FUNC(find_dialog),(gpointer)11);
	gtk_signal_connect(GTK_OBJECT(header_button),"clicked",
		GTK_SIGNAL_FUNC(edit_header),NULL);
	gtk_signal_connect(GTK_OBJECT(open_button),"clicked",
		GTK_SIGNAL_FUNC(open_file),NULL);	
	gtk_signal_connect(GTK_OBJECT(save_as_button),"clicked",
		GTK_SIGNAL_FUNC(save_file_as),NULL);
	gtk_signal_connect(GTK_OBJECT(trans_box),"changed",
		GTK_SIGNAL_FUNC(text_has_got_changed),NULL);
	/*****************************************************************/
	/**     
	* The D'n'D signals
	**/
	/**     
	* Get the drag-sets
	**/
	gtk_drag_dest_set(GTK_WIDGET(app1), GTK_DEST_DEFAULT_ALL | GTK_DEST_DEFAULT_HIGHLIGHT, dragtypes,
	        sizeof(dragtypes)/sizeof(dragtypes[0]),GDK_ACTION_COPY);
	/**     
	* Connecting the function
	**/
	gtk_signal_connect(GTK_OBJECT(app1), "drag_data_received",
        	GTK_SIGNAL_FUNC(gtranslator_dnd), (gpointer)dnd_type);
	/*****************************************************************/
	return app1;
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
        gtk_widget_set_sensitive(goto_button,FALSE);
        gtk_widget_set_sensitive(add_button,FALSE);
        gtk_widget_set_sensitive(header_button,FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(the_file_menu[0].widget),FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(the_messages_menu[0].widget),FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(the_messages_menu[1].widget),FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(the_messages_menu[3].widget),FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(the_messages_menu[4].widget),FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(the_edit_menu[8].widget),FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(the_edit_menu[10].widget),FALSE);
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
        gtk_widget_set_sensitive(goto_button,TRUE);
        gtk_widget_set_sensitive(add_button,TRUE);
        gtk_widget_set_sensitive(header_button,TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(the_file_menu[0].widget),TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(the_messages_menu[0].widget),TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(the_messages_menu[1].widget),TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(the_messages_menu[3].widget),TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(the_messages_menu[4].widget),TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(the_edit_menu[8].widget),TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(the_edit_menu[10].widget),TRUE);
}
