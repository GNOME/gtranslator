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

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>

#include <gnome.h>

#include "interface.h"

/**
 * The Gnome-help structure
**/
static GnomeHelpMenuEntry help_me = { "gtranslator", "index.html" };

void call_help_viewer(GtkWidget *widget,gpointer useless)
{
	/**
	 * Calls ( hopefully ) the Gnome Helpbrowser
	**/
	gnome_help_pbox_goto(NULL,0,&help_me);
}

static GnomeUIInfo the_file_menu[] =
{
        {
          GNOME_APP_UI_ITEM, N_("_Compile"),
          NULL,
          NULL, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_CONVERT,
          GDK_C, GDK_MOD1_MASK, NULL
        },
        GNOMEUIINFO_MENU_OPEN_ITEM (NULL, NULL),
        GNOMEUIINFO_MENU_SAVE_ITEM (NULL, NULL),
        GNOMEUIINFO_MENU_SAVE_AS_ITEM (NULL, NULL),
        GNOMEUIINFO_SEPARATOR,
        GNOMEUIINFO_MENU_EXIT_ITEM (GTK_SIGNAL_FUNC(gtk_main_quit), NULL),
        GNOMEUIINFO_END
};

static GnomeUIInfo the_edit_menu[] =
{
        GNOMEUIINFO_MENU_CUT_ITEM (NULL, NULL),
        GNOMEUIINFO_MENU_COPY_ITEM (NULL, NULL),
        GNOMEUIINFO_MENU_PASTE_ITEM (NULL, NULL),
        GNOMEUIINFO_MENU_CLEAR_ITEM (NULL, NULL),
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
        GNOMEUIINFO_MENU_PREFERENCES_ITEM (NULL, NULL),
        GNOMEUIINFO_END
};

static GnomeUIInfo the_help_menu[] =
{
        GNOMEUIINFO_MENU_ABOUT_ITEM (NULL, NULL),
        {
          GNOME_APP_UI_ITEM, N_("_Online help"),
          NULL,
          NULL, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_OPEN,
          GDK_F1, 0, NULL
        },
        GNOMEUIINFO_END
};

static GnomeUIInfo the_menus[] =
{
        GNOMEUIINFO_MENU_FILE_TREE (the_file_menu),
        GNOMEUIINFO_MENU_EDIT_TREE (the_edit_menu),
        GNOMEUIINFO_MENU_VIEW_TREE (the_view_menu),
        {
          GNOME_APP_UI_SUBTREE, N_("_Messages"),
          NULL,
          the_messages_menu, NULL, NULL,
          GNOME_APP_PIXMAP_NONE, N_("_Messages"),
          0, 0, NULL
        },
        GNOMEUIINFO_MENU_SETTINGS_TREE (the_settings_menu),
        GNOMEUIINFO_MENU_HELP_TREE (the_help_menu),
        GNOMEUIINFO_END
};

GtkWidget*
create_app1 (void)
{
        GtkWidget *dock1;
        GtkWidget *toolbar1;
        GtkWidget *tmp_toolbar_icon;
        GtkWidget *compile_button;
        GtkWidget *open_button,*cat_button,*po_button;
        GtkWidget *save_button,*save_as_button;
        GtkWidget *first_button,*last_button;
        GtkWidget *back_button,*next_button;
        GtkWidget *options_button;
        GtkWidget *exit_button;
        GtkWidget *vbox1;
        GtkWidget *scrolledwindow1,*scrolledwindow2;

        app1 = gnome_app_new ("gtranslator", _("gtranslator"));
        gtk_window_set_wmclass (GTK_WINDOW (app1), "gtranslator", "gtranslator");

        dock1 = GNOME_APP (app1)->dock;
        gtk_widget_show (dock1);

        gnome_app_create_menus (GNOME_APP (app1), the_menus);

        toolbar1 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
        gtk_widget_set_name (toolbar1, "toolbar1");
        gtk_widget_ref (toolbar1);
        gtk_object_set_data_full (GTK_OBJECT (app1), "toolbar1", toolbar1,
                                  (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show (toolbar1);
	/**
	 * Create the search bar
	 **/
        search_bar = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
        gtk_widget_show (search_bar);
        gnome_app_add_toolbar (GNOME_APP (app1), GTK_TOOLBAR (toolbar1), "toolbar1",
                                      GNOME_DOCK_ITEM_BEH_EXCLUSIVE,
                                      GNOME_DOCK_TOP, 1, 0, 0);
        gtk_container_set_border_width (GTK_CONTAINER (toolbar1), 1);
        gtk_toolbar_set_space_size (GTK_TOOLBAR (toolbar1), 16);
        gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbar1), GTK_RELIEF_NONE);
	/**
	 * Integrate the search_bar
	 **/ 
	gnome_app_add_toolbar (GNOME_APP (app1), GTK_TOOLBAR(search_bar), "search_bar",
				GNOME_DOCK_ITEM_BEH_EXCLUSIVE,
				GNOME_DOCK_TOP, 2, 0, 0);
	gtk_container_set_border_width (GTK_CONTAINER(search_bar), 1);
	gtk_toolbar_set_space_size (GTK_TOOLBAR(search_bar), 16);
	gtk_toolbar_set_space_style (GTK_TOOLBAR(search_bar), GTK_TOOLBAR_SPACE_LINE);
	gtk_toolbar_set_button_relief (GTK_TOOLBAR(search_bar), GTK_RELIEF_NONE);
	/**
	 * Now the "new" icons ...
      	 **/
	tmp_toolbar_icon = gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_SEARCH);
	search_button = gtk_toolbar_append_element (GTK_TOOLBAR(search_bar),
				GTK_TOOLBAR_CHILD_BUTTON,
				NULL,
				_("Search"),
				_("Search for a message"),NULL,
				tmp_toolbar_icon , NULL, NULL);
	gtk_widget_show(search_button);	
	tmp_toolbar_icon = gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_REDO);
	search_again_button = gtk_toolbar_append_element(GTK_TOOLBAR(search_bar),
				GTK_TOOLBAR_CHILD_BUTTON,
				NULL,
				_("ReSearch"),
				_("Search again"),NULL,
				tmp_toolbar_icon , NULL, NULL);
	gtk_widget_show(search_again_button);
	tmp_toolbar_icon = gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_JUMP_TO);
	goto_button = gtk_toolbar_append_element(GTK_TOOLBAR(search_bar),
				GTK_TOOLBAR_CHILD_BUTTON,
				NULL,
				_("Goto"),
				_("Goto this message"),NULL,
				tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(goto_button);
	tmp_toolbar_icon = gnome_stock_pixmap_widget(app1, GNOME_STOCK_PIXMAP_BOOK_RED);
	cat_button = gtk_toolbar_append_element(GTK_TOOLBAR(search_bar),
				GTK_TOOLBAR_CHILD_BUTTON,
				NULL,
				/**
				 * Find in the personal catalogue
				 **/
				_("F/Cat."),
				_("Find in the personal catalog"),NULL,
				tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(cat_button);
	tmp_toolbar_icon = gnome_stock_pixmap_widget(app1,GNOME_STOCK_PIXMAP_BOOK_OPEN);
	po_button = gtk_toolbar_append_element(GTK_TOOLBAR(search_bar),
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
	/**
	 * Now the old ones ( from GLADE-times .. ) :
	 **/
	tmp_toolbar_icon = gnome_stock_pixmap_widget (app1, GNOME_STOCK_PIXMAP_CONVERT);
	compile_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                      GTK_TOOLBAR_CHILD_BUTTON,
                                      NULL,
                                      _("Compile"),
                                      _("New File"), NULL,
                                      tmp_toolbar_icon, NULL, NULL);
       	gtk_widget_show (compile_button);

	tmp_toolbar_icon = gnome_stock_pixmap_widget (app1, GNOME_STOCK_PIXMAP_OPEN);
	open_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                      GTK_TOOLBAR_CHILD_BUTTON,
                                      NULL,
                                      _("Open"),
                                      _("Open File"), NULL,
                                      tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show (open_button);

	tmp_toolbar_icon = gnome_stock_pixmap_widget (app1, GNOME_STOCK_PIXMAP_SAVE);
	save_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                      GTK_TOOLBAR_CHILD_BUTTON,
                                      NULL,
                                      _("Save"),
                                      _("Save File"), NULL,
                                      tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show (save_button);

	tmp_toolbar_icon = gnome_stock_pixmap_widget (app1, GNOME_STOCK_PIXMAP_SAVE_AS);
	save_as_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                      GTK_TOOLBAR_CHILD_BUTTON,
                                      NULL,
                                      _("Save as"),
                                      NULL, NULL,
                                      tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show (save_as_button);

	tmp_toolbar_icon = gnome_stock_pixmap_widget (app1, GNOME_STOCK_PIXMAP_FIRST);
	first_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                      GTK_TOOLBAR_CHILD_BUTTON,
                                      NULL,
                                      _("First"),
                                      NULL, NULL,
                                      tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show (first_button);

	tmp_toolbar_icon = gnome_stock_pixmap_widget (app1, GNOME_STOCK_PIXMAP_BACK);
	back_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                      GTK_TOOLBAR_CHILD_BUTTON,
                                      NULL,
                                      _("Back"),
                                      NULL, NULL,
                                      tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show (back_button);

	tmp_toolbar_icon = gnome_stock_pixmap_widget (app1, GNOME_STOCK_PIXMAP_PREFERENCES);
	options_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                      GTK_TOOLBAR_CHILD_BUTTON,
                                      NULL,
                                      _("Options"),
                                      NULL, NULL,
                                      tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show (options_button);

	tmp_toolbar_icon = gnome_stock_pixmap_widget (app1, GNOME_STOCK_PIXMAP_FORWARD);
	next_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                      GTK_TOOLBAR_CHILD_BUTTON,
                                      NULL,
                                      _("Next"),
                                      NULL, NULL,
                                      tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show (next_button);

	tmp_toolbar_icon = gnome_stock_pixmap_widget (app1, GNOME_STOCK_PIXMAP_LAST);
	last_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                      GTK_TOOLBAR_CHILD_BUTTON,
                                      NULL,
                                      _("Last"),
                                      NULL, NULL,
                                      tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show (last_button);

	tmp_toolbar_icon = gnome_stock_pixmap_widget (app1, GNOME_STOCK_PIXMAP_EXIT);
	exit_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                      GTK_TOOLBAR_CHILD_BUTTON,
                                      NULL,
                                      _("Exit"),
                                      NULL, NULL,
                                      tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show (exit_button);

	vbox1 = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox1);
	gnome_app_set_contents (GNOME_APP (app1), vbox1);

	scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow1);
	gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow1, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

	text1 = gtk_text_new (NULL, NULL);
	gtk_widget_show (text1);
	gtk_container_add (GTK_CONTAINER (scrolledwindow1), text1);
	gtk_widget_realize (text1);
	scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow2);
	gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow2, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

	trans_box = gtk_text_new (NULL, NULL);
	gtk_widget_show (trans_box);
	gtk_container_add (GTK_CONTAINER (scrolledwindow2), trans_box);
	gtk_text_set_editable (GTK_TEXT (trans_box), TRUE);
	gtk_widget_realize (trans_box);

	appbar1 = gnome_appbar_new (TRUE ,TRUE , GNOME_PREFERENCES_NEVER);
	gtk_widget_show (appbar1);
	gnome_app_set_statusbar (GNOME_APP (app1), appbar1);

	/**
	 * The callbacks list
	 **/
	gtk_signal_connect(GTK_OBJECT(app1),"delete-event",
	GTK_SIGNAL_FUNC(gtk_main_quit),NULL);	
	gtk_signal_connect(GTK_OBJECT(exit_button),"clicked",
	GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
	
	return app1;
}
