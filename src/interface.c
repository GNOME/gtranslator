/**
 * Fatih Demir [ kabalak@gmx.net ]
 *
 * (C) 2000 Published under GNU GPL V 2.0+
 *
 * The interface file , totally catastrophed by me ...
 * 
 * And reformatted ; so it should be much better to read it
 *	                                  ^^^^^^  
 * Where does the word ´better´ (->German ´besser´) come from ?
 **/

#ifdef HAVE_CONFIG_H
#        include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>

#include <gnome.h>

#include "callbacks.h"
#include "interface.h"

static GnomeUIInfo file1_menu_uiinfo[] =
{
        {
          GNOME_APP_UI_ITEM, N_("_Compile"),
          NULL,
          on_compile1_activate, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_CONVERT,
          GDK_C, GDK_MOD1_MASK, NULL
        },
        GNOMEUIINFO_MENU_OPEN_ITEM (on_open1_activate, NULL),
        GNOMEUIINFO_MENU_SAVE_ITEM (on_save1_activate, NULL),
        GNOMEUIINFO_MENU_SAVE_AS_ITEM (on_save_as1_activate, NULL),
        GNOMEUIINFO_SEPARATOR,
        GNOMEUIINFO_MENU_EXIT_ITEM (on_exit1_activate, NULL),
        GNOMEUIINFO_END
};

static GnomeUIInfo edit1_menu_uiinfo[] =
{
        GNOMEUIINFO_MENU_CUT_ITEM (on_cut1_activate, NULL),
        GNOMEUIINFO_MENU_COPY_ITEM (on_copy1_activate, NULL),
        GNOMEUIINFO_MENU_PASTE_ITEM (on_paste1_activate, NULL),
        GNOMEUIINFO_MENU_CLEAR_ITEM (on_clear1_activate, NULL),
        GNOMEUIINFO_END
};

static GnomeUIInfo view1_menu_uiinfo[] =
{
        {
          GNOME_APP_UI_ITEM, N_("Refresh"),
          NULL,
          on_refresh1_activate, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_REFRESH,
          GDK_F5, 0, NULL
        },
        GNOMEUIINFO_END
};

static GnomeUIInfo messages1_menu_uiinfo[] =
{
        {
          GNOME_APP_UI_ITEM, N_("_First"),
          NULL,
          on_first1_activate, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_FIRST,
          GDK_Home, GDK_CONTROL_MASK, NULL
        },
        {
          GNOME_APP_UI_ITEM, N_("_Back"),
          NULL,
          on_back1_activate, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BACK,
          GDK_Page_Up, 0, NULL
        },
        GNOMEUIINFO_SEPARATOR,
        {
          GNOME_APP_UI_ITEM, N_("_Next"),
          NULL,
          on_next1_activate, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_FORWARD,
          GDK_Page_Down, 0, NULL
        },
        {
          GNOME_APP_UI_ITEM, N_("_Last"),
          NULL,
          on_last1_activate, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_LAST,
          GDK_End, GDK_CONTROL_MASK, NULL
        },
        GNOMEUIINFO_END
};

static GnomeUIInfo settings1_menu_uiinfo[] =
{
        GNOMEUIINFO_MENU_PREFERENCES_ITEM (on_preferences1_activate, NULL),
        GNOMEUIINFO_END
};

static GnomeUIInfo help1_menu_uiinfo[] =
{
        GNOMEUIINFO_MENU_ABOUT_ITEM (on_about1_activate, NULL),
        {
          GNOME_APP_UI_ITEM, N_("_Online help"),
          NULL,
          on_online_help1_activate, NULL, NULL,
          GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_OPEN,
          GDK_F1, 0, NULL
        },
        GNOMEUIINFO_END
};

static GnomeUIInfo menubar1_uiinfo[] =
{
        GNOMEUIINFO_MENU_FILE_TREE (file1_menu_uiinfo),
        GNOMEUIINFO_MENU_EDIT_TREE (edit1_menu_uiinfo),
        GNOMEUIINFO_MENU_VIEW_TREE (view1_menu_uiinfo),
        {
          GNOME_APP_UI_SUBTREE, N_("_Messages"),
          NULL,
          messages1_menu_uiinfo, NULL, NULL,
          GNOME_APP_PIXMAP_NONE, N_("_Messages"),
          0, 0, NULL
        },
        GNOMEUIINFO_MENU_SETTINGS_TREE (settings1_menu_uiinfo),
        GNOMEUIINFO_MENU_HELP_TREE (help1_menu_uiinfo),
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

        gnome_app_create_menus (GNOME_APP (app1), menubar1_uiinfo);

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
        gtk_toolbar_set_space_style (GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_SPACE_LINE);
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
				_("F/Cat."),
				_("Find in the personal catalog"),NULL,
				tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(cat_button);
	tmp_toolbar_icon = gnome_stock_pixmap_widget(app1,GNOME_STOCK_PIXMAP_BOOK_OPEN);
	po_button = gtk_toolbar_append_element(GTK_TOOLBAR(search_bar),
				GTK_TOOLBAR_CHILD_BUTTON,
				NULL,
				_("F/Po."),
				_("Find in the po file "), NULL,
				tmp_toolbar_icon , NULL, NULL);
	gtk_widget_show(po_button);
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

	appbar1 = gnome_appbar_new (TRUE ,FALSE , GNOME_PREFERENCES_NEVER);
	gtk_widget_show (appbar1);
	gnome_app_set_statusbar (GNOME_APP (app1), appbar1);

        gtk_signal_connect (GTK_OBJECT (compile_button), "pressed",
                            GTK_SIGNAL_FUNC (on_compile_button_pressed),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (search_button), "pressed",
		            GTK_SIGNAL_FUNC (on_search_button_pressed) ,
		            NULL);
        gtk_signal_connect (GTK_OBJECT (open_button), "pressed",
                            GTK_SIGNAL_FUNC (on_open_button_pressed),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (save_button), "pressed",
                            GTK_SIGNAL_FUNC (on_save_button_pressed),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (save_as_button), "pressed",
                            GTK_SIGNAL_FUNC (on_save_as_button_pressed),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (first_button), "pressed",
                            GTK_SIGNAL_FUNC (on_first_button_pressed),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (back_button), "pressed",
                            GTK_SIGNAL_FUNC (on_back_button_pressed),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (options_button), "pressed",
                            GTK_SIGNAL_FUNC (on_options_button_pressed),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (next_button), "pressed",
                            GTK_SIGNAL_FUNC (on_next_button_pressed),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (last_button), "pressed",
                            GTK_SIGNAL_FUNC (on_last_button_pressed),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (exit_button), "pressed",
                            GTK_SIGNAL_FUNC (on_exit_button_pressed),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (trans_box), "changed",
                            GTK_SIGNAL_FUNC (on_trans_box_changed),
                            GTK_TEXT(trans_box));
        gtk_signal_connect (GTK_OBJECT (trans_box), "copy_clipboard",
                            GTK_SIGNAL_FUNC (on_trans_box_copy_clipboard),
                            GTK_TEXT(trans_box));
        gtk_signal_connect (GTK_OBJECT (trans_box), "cut_clipboard",
                            GTK_SIGNAL_FUNC (on_trans_box_cut_clipboard),
                            GTK_TEXT(trans_box));
        gtk_signal_connect (GTK_OBJECT (trans_box), "paste_clipboard",
                            GTK_SIGNAL_FUNC (on_trans_box_paste_clipboard),
                            GTK_TEXT(trans_box));
        gtk_signal_connect (GTK_OBJECT (trans_box), "activate",
                            GTK_SIGNAL_FUNC (on_trans_box_activate),
                            GTK_TEXT(trans_box));

        return app1;
}

GtkWidget*
create_about1 (void)
{
        const gchar *authors[] = {
          "Fatih Demir",
          NULL
        };
        GtkWidget *about1;

        about1 = gnome_about_new ("gtranslator", VERSION,
                              _("(C) 1999-2000 Fatih Demir ( kabalak@gmx.net )"),
                              authors,
                              _("gtranslator is a GnomeApplication for translating .po-files more comfortable than in (X)Emacs .\nTill now you can't see much , but that will change !"),
                              NULL);
        gtk_widget_set_name (about1, "about1");
        gtk_object_set_data (GTK_OBJECT (about1), "about1", about1);
        gtk_window_set_modal (GTK_WINDOW (about1), TRUE);
        gtk_window_set_wmclass (GTK_WINDOW (about1), "gtranslator", "gtranslator");

        return about1;
}

GtkWidget*
create_propertybox1 (void)
{
        GtkWidget *propertybox1;
        GtkWidget *notebook1;
        GtkWidget *vbox3;
        GtkWidget *warn_for_errors;
        GtkWidget *saving_unchanged;
        GtkWidget *start_at_last_file;
        GtkWidget *label1;
        GtkWidget *vbox2;
        GSList *verbose_group = NULL;
        GtkWidget *verbose_yes;
        GtkWidget *verbose_no;
        GtkWidget *label2;

	propertybox1 = gnome_property_box_new ();

	notebook1 = GNOME_PROPERTY_BOX (propertybox1)->notebook;
	gtk_widget_show (notebook1);

	vbox3 = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox3);
	gtk_container_add (GTK_CONTAINER (notebook1), vbox3);

	warn_for_errors = gtk_check_button_new_with_label (_("Warn me if there are errors in the .po-file"));
	gtk_widget_show (warn_for_errors);
	gtk_box_pack_start (GTK_BOX (vbox3), warn_for_errors, FALSE, FALSE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (warn_for_errors), TRUE);

	saving_unchanged = gtk_check_button_new_with_label (_("Ask for saving unchanged .po-files "));
	gtk_widget_show (saving_unchanged);
	gtk_box_pack_start (GTK_BOX (vbox3), saving_unchanged, FALSE, FALSE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (saving_unchanged), TRUE);

	start_at_last_file = gtk_check_button_new_with_label (_("gtranslator should start at the last edited file"));
	gtk_widget_show (start_at_last_file);
	gtk_box_pack_start (GTK_BOX (vbox3), start_at_last_file, FALSE, FALSE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (start_at_last_file), TRUE);

	label1 = gtk_label_new (_("Common settings "));
	gtk_widget_show (label1);
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label1);

	vbox2 = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox2);
	gtk_container_add (GTK_CONTAINER (notebook1), vbox2);

	verbose_yes = gtk_radio_button_new_with_label (verbose_group, _("Show me the output of msgfmt --statistics"));
	verbose_group = gtk_radio_button_group (GTK_RADIO_BUTTON (verbose_yes));
	gtk_widget_show (verbose_yes);
	gtk_box_pack_start (GTK_BOX (vbox2), verbose_yes, FALSE, FALSE, 0);

        verbose_no = gtk_radio_button_new_with_label (verbose_group, _("Don't  show me any messages of msgfmt "));
        verbose_group = gtk_radio_button_group (GTK_RADIO_BUTTON (verbose_no));
        gtk_widget_show (verbose_no);
        gtk_box_pack_start (GTK_BOX (vbox2), verbose_no, FALSE, FALSE, 0);
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (verbose_no), TRUE);

        label2 = gtk_label_new (_("Compiling  points ..."));
        gtk_widget_show (label2);
        gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label2);

	gtk_signal_connect (GTK_OBJECT (propertybox1), "apply",
                            GTK_SIGNAL_FUNC (on_propertybox1_apply),
                            NULL);
	gtk_signal_connect (GTK_OBJECT (propertybox1), "help",
                            GTK_SIGNAL_FUNC (on_propertybox1_help),
                            propertybox1);
	gtk_signal_connect (GTK_OBJECT (propertybox1), "close",
                            GTK_SIGNAL_FUNC (on_propertybox1_close),
                            propertybox1);
	gtk_signal_connect (GTK_OBJECT (warn_for_errors), "toggled",
                            GTK_SIGNAL_FUNC (on_warn_for_errors_toggled),
                            propertybox1);
	gtk_signal_connect (GTK_OBJECT (saving_unchanged), "toggled",
                            GTK_SIGNAL_FUNC (on_saving_unchanged_toggled),
                            propertybox1);
	gtk_signal_connect (GTK_OBJECT (start_at_last_file), "toggled",
                            GTK_SIGNAL_FUNC (on_start_at_last_file_toggled),
                            propertybox1);
	gtk_signal_connect (GTK_OBJECT (verbose_yes), "toggled",
                            GTK_SIGNAL_FUNC (on_verbose_yes_toggled),
                            propertybox1);
	gtk_signal_connect (GTK_OBJECT (verbose_no), "toggled",
                            GTK_SIGNAL_FUNC (on_verbose_no_toggled),
                            propertybox1);

	return propertybox1;
}

GtkWidget*
create_open_file (void)
{
	GtkWidget *open_file;
	GtkWidget *ok_button_of_open_file;
	GtkWidget *cancel_button_of_open_file;

	open_file = gtk_file_selection_new (_("Select your .po-file to open"));
	gtk_container_set_border_width (GTK_CONTAINER (open_file), 10);
	gtk_window_set_wmclass (GTK_WINDOW (open_file), "gtranslator", "gtranslator");
	gtk_file_selection_hide_fileop_buttons (GTK_FILE_SELECTION (open_file));

        ok_button_of_open_file = GTK_FILE_SELECTION (open_file)->ok_button;
        gtk_widget_show (ok_button_of_open_file);
        GTK_WIDGET_SET_FLAGS (ok_button_of_open_file, GTK_CAN_DEFAULT);

        cancel_button_of_open_file = GTK_FILE_SELECTION (open_file)->cancel_button;
        gtk_widget_show (cancel_button_of_open_file);
        GTK_WIDGET_SET_FLAGS (cancel_button_of_open_file, GTK_CAN_DEFAULT);

        gtk_signal_connect (GTK_OBJECT (ok_button_of_open_file), "pressed",
                            GTK_SIGNAL_FUNC (on_ok_button_of_open_file_pressed),
                            open_file);
        gtk_signal_connect (GTK_OBJECT (cancel_button_of_open_file), "pressed",
                            GTK_SIGNAL_FUNC (on_cancel_button_of_open_file_pressed),
                            open_file);

        return open_file;
}

GtkWidget*
create_save_file_as (void)
{
        GtkWidget *save_file_as;
        GtkWidget *ok_button_of_save_as_file;
        GtkWidget *cancel_button_of_save_as_file;

        save_file_as = gtk_file_selection_new (_("Select your file to save to"));
        gtk_container_set_border_width (GTK_CONTAINER (save_file_as), 10);
        gtk_window_set_wmclass (GTK_WINDOW (save_file_as), "gtranslator", "gtranslator");

        ok_button_of_save_as_file = GTK_FILE_SELECTION (save_file_as)->ok_button;
        gtk_widget_show (ok_button_of_save_as_file);
        GTK_WIDGET_SET_FLAGS (ok_button_of_save_as_file, GTK_CAN_DEFAULT);

        cancel_button_of_save_as_file = GTK_FILE_SELECTION (save_file_as)->cancel_button;
        gtk_widget_show (cancel_button_of_save_as_file);
        GTK_WIDGET_SET_FLAGS (cancel_button_of_save_as_file, GTK_CAN_DEFAULT);

        gtk_signal_connect (GTK_OBJECT (ok_button_of_save_as_file), "pressed",
                            GTK_SIGNAL_FUNC (on_ok_button_of_save_as_file_pressed),
                            save_file_as);
        gtk_signal_connect (GTK_OBJECT (cancel_button_of_save_as_file), "pressed",
                            GTK_SIGNAL_FUNC (on_cancel_button_of_save_as_file_pressed),
                            save_file_as);

        return save_file_as;
}

GtkWidget*
create_errors_in_the_po (void)
{
        GtkWidget *errors_in_the_po;
        GtkWidget *dialog_vbox1;
        GtkWidget *button11;
        GtkWidget *dialog_action_area1;

        errors_in_the_po = gnome_message_box_new (_("An error occured in your .po-file as it \ncouldn't be compiled by mfgfmt !"),
                                    GNOME_MESSAGE_BOX_ERROR, NULL);
        GTK_WINDOW (errors_in_the_po)->type = GTK_WINDOW_DIALOG;
        gtk_window_set_policy (GTK_WINDOW (errors_in_the_po), FALSE, FALSE, FALSE);
        gtk_window_set_wmclass (GTK_WINDOW (errors_in_the_po), "gtranslator", "gtranslator");
        gnome_dialog_set_close (GNOME_DIALOG (errors_in_the_po), TRUE);
        gnome_dialog_close_hides (GNOME_DIALOG (errors_in_the_po), TRUE);

        dialog_vbox1 = GNOME_DIALOG (errors_in_the_po)->vbox;
        gtk_widget_show (dialog_vbox1);

        gnome_dialog_append_button (GNOME_DIALOG (errors_in_the_po), GNOME_STOCK_BUTTON_OK);
        button11 = g_list_last (GNOME_DIALOG (errors_in_the_po)->buttons)->data;
        gtk_widget_show (button11);
        GTK_WIDGET_SET_FLAGS (button11, GTK_CAN_DEFAULT);

        dialog_action_area1 = GNOME_DIALOG (errors_in_the_po)->action_area;
        if (dialog_action_area1 != NULL)
        return errors_in_the_po;
}

GtkWidget*
create_save_unchanged_po (void)
{
        GtkWidget *save_unchanged_po;
        GtkWidget *dialog_vbox2;
        GtkWidget *button12;
        GtkWidget *dialog_action_area2;

        save_unchanged_po = gnome_message_box_new (_("You try to save an unchanged .po-file !\nHow useless ."),
                                    GNOME_MESSAGE_BOX_WARNING, NULL);
        gtk_widget_set_name (save_unchanged_po, "save_unchanged_po");
        gtk_object_set_data (GTK_OBJECT (save_unchanged_po), "save_unchanged_po", save_unchanged_po);
        GTK_WINDOW (save_unchanged_po)->type = GTK_WINDOW_DIALOG;
        gtk_window_set_policy (GTK_WINDOW (save_unchanged_po), FALSE, FALSE, FALSE);
        gtk_window_set_wmclass (GTK_WINDOW (save_unchanged_po), "gtranslator", "gtranslator");
        gnome_dialog_set_close (GNOME_DIALOG (save_unchanged_po), TRUE);
        gnome_dialog_close_hides (GNOME_DIALOG (save_unchanged_po), TRUE);

        dialog_vbox2 = GNOME_DIALOG (save_unchanged_po)->vbox;
        gtk_widget_set_name (dialog_vbox2, "dialog_vbox2");
        gtk_object_set_data (GTK_OBJECT (save_unchanged_po), "dialog_vbox2", dialog_vbox2);
        gtk_widget_show (dialog_vbox2);

        gnome_dialog_append_button (GNOME_DIALOG (save_unchanged_po), GNOME_STOCK_BUTTON_OK);
        button12 = g_list_last (GNOME_DIALOG (save_unchanged_po)->buttons)->data;
        gtk_widget_set_name (button12, "button12");
        gtk_widget_ref (button12);
        gtk_object_set_data_full (GTK_OBJECT (save_unchanged_po), "button12", button12,
                                  (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show (button12);
        GTK_WIDGET_SET_FLAGS (button12, GTK_CAN_DEFAULT);

        dialog_action_area2 = GNOME_DIALOG (save_unchanged_po)->action_area;
        if (dialog_action_area2 != NULL)
          gtk_widget_set_name (dialog_action_area2, "dialog_action_area2");
        gtk_widget_ref (dialog_action_area2);
        gtk_object_set_data_full (GTK_OBJECT (save_unchanged_po), "dialog_action_area2", dialog_action_area2,
                                  (GtkDestroyNotify) gtk_widget_unref);

        return save_unchanged_po;
}

GtkWidget*
create_compiling_po (void)
{
        GtkWidget *compiling_po;
        GtkWidget *dialog_vbox3;
        GtkWidget *button13;
        GtkWidget *dialog_action_area3;

        compiling_po = gnome_message_box_new (_("Your .po-file is being compiled by \nmsgfmt now ...."),
                                    GNOME_MESSAGE_BOX_INFO, NULL);
        gtk_widget_set_name (compiling_po, "compiling_po");
        gtk_object_set_data (GTK_OBJECT (compiling_po), "compiling_po", compiling_po);
        GTK_WINDOW (compiling_po)->type = GTK_WINDOW_DIALOG;
        gtk_window_set_policy (GTK_WINDOW (compiling_po), FALSE, FALSE, FALSE);
        gtk_window_set_wmclass (GTK_WINDOW (compiling_po), "gtranslator", "gtranslator");
        gnome_dialog_set_close (GNOME_DIALOG (compiling_po), TRUE);
        gnome_dialog_close_hides (GNOME_DIALOG (compiling_po), TRUE);

        dialog_vbox3 = GNOME_DIALOG (compiling_po)->vbox;
        gtk_widget_set_name (dialog_vbox3, "dialog_vbox3");
        gtk_object_set_data (GTK_OBJECT (compiling_po), "dialog_vbox3", dialog_vbox3);
        gtk_widget_show (dialog_vbox3);

        gnome_dialog_append_button (GNOME_DIALOG (compiling_po), GNOME_STOCK_BUTTON_OK);
        button13 = g_list_last (GNOME_DIALOG (compiling_po)->buttons)->data;
        gtk_widget_set_name (button13, "button13");
        gtk_widget_ref (button13);
        gtk_object_set_data_full (GTK_OBJECT (compiling_po), "button13", button13,
                                  (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show (button13);
        GTK_WIDGET_SET_FLAGS (button13, GTK_CAN_DEFAULT);

        dialog_action_area3 = GNOME_DIALOG (compiling_po)->action_area;
        if (dialog_action_area3 != NULL)
          gtk_widget_set_name (dialog_action_area3, "dialog_action_area3");
        gtk_widget_ref (dialog_action_area3);
        gtk_object_set_data_full (GTK_OBJECT (compiling_po), "dialog_action_area3", dialog_action_area3,
                                  (GtkDestroyNotify) gtk_widget_unref);
        gnome_execute_shell(NULL,"msgfmt tr.po");
        return compiling_po;
}

GtkWidget*
create_help_for_the_propbox1 (void)
{
        GtkWidget *help_for_the_propbox1;
        GtkWidget *dialog_vbox5;
        GtkWidget *button15;
        GtkWidget *dialog_action_area5;

        help_for_the_propbox1 = gnome_message_box_new (_("Here you can select various options of\ngtranslator which will make gtranslator\nmore work like you want it to ."),
                                    GNOME_MESSAGE_BOX_INFO, NULL);
        gtk_widget_set_name (help_for_the_propbox1, "help_for_the_propbox1");
        gtk_object_set_data (GTK_OBJECT (help_for_the_propbox1), "help_for_the_propbox1", help_for_the_propbox1);
        GTK_WINDOW (help_for_the_propbox1)->type = GTK_WINDOW_DIALOG;
        gtk_window_set_policy (GTK_WINDOW (help_for_the_propbox1), FALSE, FALSE, FALSE);
        gtk_window_set_wmclass (GTK_WINDOW (help_for_the_propbox1), "gtranslator", "gtranslator");
        gnome_dialog_set_close (GNOME_DIALOG (help_for_the_propbox1), TRUE);
        gnome_dialog_close_hides (GNOME_DIALOG (help_for_the_propbox1), TRUE);

        dialog_vbox5 = GNOME_DIALOG (help_for_the_propbox1)->vbox;
        gtk_widget_set_name (dialog_vbox5, "dialog_vbox5");
        gtk_object_set_data (GTK_OBJECT (help_for_the_propbox1), "dialog_vbox5", dialog_vbox5);
        gtk_widget_show (dialog_vbox5);

        gnome_dialog_append_button (GNOME_DIALOG (help_for_the_propbox1), GNOME_STOCK_BUTTON_OK);
        button15 = g_list_last (GNOME_DIALOG (help_for_the_propbox1)->buttons)->data;
        gtk_widget_set_name (button15, "button15");
        gtk_widget_ref (button15);
        gtk_object_set_data_full (GTK_OBJECT (help_for_the_propbox1), "button15", button15,
                                  (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show (button15);
        GTK_WIDGET_SET_FLAGS (button15, GTK_CAN_DEFAULT);

        dialog_action_area5 = GNOME_DIALOG (help_for_the_propbox1)->action_area;
        if (dialog_action_area5 != NULL)
          gtk_widget_set_name (dialog_action_area5, "dialog_action_area5");
        gtk_widget_ref (dialog_action_area5);
        gtk_object_set_data_full (GTK_OBJECT (help_for_the_propbox1), "dialog_action_area5", dialog_action_area5,
                                  (GtkDestroyNotify) gtk_widget_unref);

        return help_for_the_propbox1;
}

