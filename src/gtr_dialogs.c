/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* This is the general place for all dialogs
* used in gtranslator ...
*
* -- the source ...
**/

#include "gtr_dialogs.h"

void compile_error_dialog(GtkWidget *widget,gpointer useless)
{
	/**
	* Should be shown if there is a non-zero return value
	*  from msgfmt FILE -o TEST_OUTPUT
	**/
	compile_error=gnome_app_error(GNOME_APP(app1),
		_("An error occured while msgfmt was executed.\nPlease check your .po file again."));
	gtk_widget_show(compile_error);
}

void no_changes_dialog(GtkWidget *widget,gpointer useless)
{
	/**
	* Should appear when there weren't made any changes 
	*  to the .po-file ( no changed-callback has been
	*   called ... )
	**/
	no_changes=gnome_app_warning(GNOME_APP(app1),
		_("You didn't change anything in the .po-file!\n"));
	gtk_widget_show(no_changes);
}

/**
* The preferences box help
**/
void prefs_box_help(GtkWidget *widget,gpointer useless)
{
	prefs_box_help_help=_("With the Preferences box you can define some variables\n\
with which you can make gtranslator make more work\nlike YOU want it to work!");
	prefs_box_help_dlg=gnome_app_message(GNOME_APP(app1),prefs_box_help_help);
}

/**
* The Open file routines ..
**/
void open_file_create()
{
	of_dlg=gtk_file_selection_new(_("gtranslator -- open a po-file"));
	of_dlg_ok=GTK_FILE_SELECTION(of_dlg)->ok_button;
	of_dlg_cancel=GTK_FILE_SELECTION(of_dlg)->cancel_button;
	gtk_file_selection_show_fileop_buttons(GTK_FILE_SELECTION(of_dlg));
	gtk_signal_connect(GTK_OBJECT(of_dlg_ok),"clicked",
		GTK_SIGNAL_FUNC(parse_the_file),NULL);
	gtk_signal_connect(GTK_OBJECT(of_dlg_cancel),"clicked",
		GTK_SIGNAL_FUNC(open_file_hide),NULL);
}

void open_file_show()
{
	open_file_create();
	/**
        * Set the icon.
        **/
	#ifdef USE_WINDOW_ICON
        gnome_window_icon_set_from_file(GTK_WINDOW(of_dlg),WINDOW_ICON);
	#endif
	gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Select your desired po-file to open"));
	gtk_widget_show(of_dlg);
}

void open_file_hide(GtkWidget *widget,gpointer useless)
{
	gtk_widget_hide(of_dlg);
}

void open_file(GtkWidget *widget,gpointer useless)
{
	open_file_show();
}

/**
* Save file as-dialogs
**/
void save_file_as_create()
{
	sfa_dlg=gtk_file_selection_new(_("gtranslator -- save file as .."));
	sfa_dlg_ok=GTK_FILE_SELECTION(sfa_dlg)->ok_button;
	sfa_dlg_cancel=GTK_FILE_SELECTION(sfa_dlg)->cancel_button;
	gtk_file_selection_show_fileop_buttons(GTK_FILE_SELECTION(sfa_dlg));
	gtk_signal_connect(GTK_OBJECT(sfa_dlg_ok),"clicked",
		NULL,NULL);
	gtk_signal_connect(GTK_OBJECT(sfa_dlg_cancel),"clicked",
		GTK_SIGNAL_FUNC(save_file_as_hide),NULL);
}

void save_file_as_show()
{
	save_file_as_create();
	/**
        * Set the icon.
        **/
	#ifdef USE_WINDOW_ICON
        gnome_window_icon_set_from_file(GTK_WINDOW(sfa_dlg),WINDOW_ICON);
	#endif
	gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Select your filename to which the po-file should be saved to."));
	gtk_widget_show(sfa_dlg);
}

void save_file_as_hide(GtkWidget *widget,gpointer useless)
{
	gtk_widget_hide(sfa_dlg);
}

void save_file_as(GtkWidget *widget,gpointer useless)
{
	save_file_as_show();
}

/**
* The search-box routines
**/
void s_box_create()
{
	GtkWidget *s_box_label;
	/**
	* Create a gtk_dialog. This will be used as a 
	*  search-box-dialog.	
	**/
	s_box_dlg=gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(s_box_dlg),_("gtranslator -- search-box"));
	gtk_window_set_wmclass(GTK_WINDOW(s_box_dlg),"gtranslator -- search-box","gtranslator -- search-box");
	gtk_widget_set_usize(s_box_dlg,220,90);
	/**
	* Set up the information label 
	**/
	
	s_box_label=gtk_label_new(_("Enter your desired search string: "));
	s_box_string=gnome_entry_new("SEARCH");
	s_box_ok=gtk_button_new_with_label(_("Search"));
	s_box_cancel=gnome_stock_button(GNOME_STOCK_BUTTON_CANCEL);
	/**
	* Add the widgets to the dialog.
	**/
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(s_box_dlg)->action_area),s_box_ok);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(s_box_dlg)->action_area),s_box_cancel);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(s_box_dlg)->vbox),s_box_label);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(s_box_dlg)->vbox),s_box_string);
	/**
        * Set the icon.
        **/
        gnome_window_icon_set_from_file(GTK_WINDOW(s_box_dlg),WINDOW_ICON);
	/**
	* Show the inserted widgets first
	**/
	gtk_widget_show(s_box_ok);
	gtk_widget_show(s_box_cancel);
	gtk_widget_show(s_box_label);
	gtk_widget_show(s_box_string);
	/**
	* Connect the widgets to the signals
	**/
	gtk_signal_connect(GTK_OBJECT(s_box_cancel),"clicked",
		GTK_SIGNAL_FUNC(s_box_hide),NULL);
	gtk_signal_connect(GTK_OBJECT(s_box_ok),"clicked",
		GTK_SIGNAL_FUNC(search),NULL);
}

void s_box_show()
{
	s_box_create();
	gnome_entry_load_history(GNOME_ENTRY(s_box_string));
	gtk_widget_show(s_box_dlg);
}

void s_box_hide(GtkWidget *widget,gpointer useless)
{
	gnome_entry_save_history(GNOME_ENTRY(s_box_string));
	gtk_widget_hide(s_box_dlg);
}

void s_box(GtkWidget *widget,gpointer useless)
{
	s_box_show();
}

/**
* The search function
**/
void search(GtkWidget *widget,gpointer useless)
{
	gchar *zoom_into_my_brain;
	gtk_widget_set_sensitive(search_again_button,TRUE);
	zoom_into_my_brain=search_do(widget,(gpointer)1);
}

/**
* The GoTo methods
**/
void goto_dlg_create(gint lines)
{
	GtkWidget *g_dlg_label;
	gint nb=1;
	/**
	* Again create a Gtk+ dialog
	**/
	g_dlg=gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(g_dlg),_("GoTo message entry"));
	gtk_window_set_wmclass(GTK_WINDOW(g_dlg),"gtranslator -- goto","gtranslator -- goto");
	gtk_widget_set_usize(g_dlg,180,90);
	/**
	* Create the buttons, the entry & the label
	**/
	g_dlg_ok=gtk_button_new_with_label(_("Go!"));
	g_dlg_cancel=gnome_stock_button(GNOME_STOCK_BUTTON_CANCEL);
	g_dlg_line_adjustment=gtk_adjustment_new(nb, 1, msg_pair, 1, 2, 2);
	g_dlg_line=gtk_spin_button_new(GTK_ADJUSTMENT(g_dlg_line_adjustment), 1, 0);
	g_dlg_label=gtk_label_new(_("Goto the specified message entry."));
	/**
	* Add'em to the dialog
	**/
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(g_dlg)->action_area),g_dlg_ok);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(g_dlg)->action_area),g_dlg_cancel);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(g_dlg)->vbox),g_dlg_label);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(g_dlg)->vbox),g_dlg_line);
	/**
        * Set the icon.
        **/
	#ifdef USE_WINDOW_ICON
        gnome_window_icon_set_from_file(GTK_WINDOW(g_dlg),WINDOW_ICON);
	#endif
	/**
	* Show the widgets
	**/
	gtk_widget_show(g_dlg_ok);
	gtk_widget_show(g_dlg_cancel);
	gtk_widget_show(g_dlg_label);
	gtk_widget_show(g_dlg_line);
	/**
	* Connect the signals
	**/
	gtk_signal_connect(GTK_OBJECT(g_dlg_cancel),"clicked",
		GTK_SIGNAL_FUNC(goto_dlg_hide),NULL);
}

void goto_dlg_show()
{
	goto_dlg_create((gint)g_list_length(messages));
	gtk_widget_show(g_dlg);
}

void goto_dlg_hide(GtkWidget *widget,gpointer useless)
{
	gtk_widget_hide(g_dlg);
}

void goto_dlg(GtkWidget *widget,gpointer useless)
{
	goto_dlg_show();
}

void find_dialog(GtkWidget *widget,gpointer title_type)
{
	GtkWidget *findy,*ignore_case,*t;
	gchar *title;
	/**
	* Simply define the title via 
	*  a given argument to the callback.
	**/
	switch((int)title_type)
	{
		case 10:
			title=_("gtranslator -- find in the catalogue");
			break;
		case 11:
			title=_("gtranslator -- find in the po-file");
			break;
		default:
			title=_("gtranslator -- find");
			break;	
	}
	/**
	* Create the widgets.
	**/
	find_dlg_cancel=gnome_stock_button(GNOME_STOCK_BUTTON_CANCEL);
	find_dlg_ok=gtk_button_new_with_label(_("Find"));
	findy=gnome_entry_new("FINDY");
	ignore_case=gtk_check_button_new_with_label(_("Ignore the case"));
	t=gtk_table_new(2,3,FALSE);
	/**
	* Create the dialog.
	**/
	find_dlg=gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(find_dlg),title);
	gtk_window_set_wmclass(GTK_WINDOW(find_dlg),"gtranslator","gtranslator");
	gtk_widget_set_usize(find_dlg,270,110);
	/**
	* Add the elements to the table.
	**/
	gtk_table_attach_defaults(GTK_TABLE(t),findy,1,2,0,1);
	gtk_table_attach_defaults(GTK_TABLE(t),ignore_case,1,2,1,2);
	/**
	* .. and add the buttons.
	**/
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(find_dlg)->vbox),t);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(find_dlg)->action_area),find_dlg_ok);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(find_dlg)->action_area),find_dlg_cancel);
	/**
	* Connect the dialogs.
	**/
	gtk_signal_connect(GTK_OBJECT(find_dlg),"delete-event",
		GTK_SIGNAL_FUNC(find_dialog_hide),NULL);
	gtk_signal_connect(GTK_OBJECT(find_dlg_cancel),"clicked",
		GTK_SIGNAL_FUNC(find_dialog_hide),NULL);
	gtk_signal_connect(GTK_OBJECT(find_dlg_ok),"clicked",
		NULL,NULL);
	/**
	* Set the icon.
	**/
	#ifdef USE_WINDOW_ICON
	gnome_window_icon_set_from_file(GTK_WINDOW(find_dlg),WINDOW_ICON);
	#endif
	/**
	* Show the widgtes and finally the box.
	**/
	gtk_widget_show_all(t);
	gtk_widget_show(find_dlg_ok);
	gtk_widget_show(find_dlg_cancel);
	gtk_widget_show(find_dlg);
}

void find_dialog_hide(GtkWidget *widget,gpointer useless)
{
	gtk_widget_hide(find_dlg);
}
