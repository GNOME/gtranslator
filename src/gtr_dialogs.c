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
	g_dlg_line_adjustment=gtk_adjustment_new(nb, 1, msg_pair_count, 1, 2, 2);
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

void query_dialog_create()
{
	GtkWidget *q_dlg_label;
	/**
	* The default challenge length.
	**/
	gint default_challen;
	/**
	* The GtkAdjustment for the spinbutton.
	**/
	GtkObject *fot;
	/**
	* The default challen is 2.
	**/
	default_challen=2;
	/**
	* Create the dialog.
	**/
	q_dlg=gtk_dialog_new();
	/**
	* Create a helping label.
	**/
	q_dlg_label=gtk_label_new(_("With this query window you can query\n\
the message db for an existing entry\n\
with your query string."));
	/**
	* Get the adjustment.
	**/
	fot=gtk_adjustment_new(default_challen, 1, 10, 1, 2, 2);
	/**
	* Create the spinbutton.
	**/
	q_dlg_spin_button=gtk_spin_button_new(GTK_ADJUSTMENT(fot),1,0);
	/**
	* Add another GNOME entry.
	**/
	q_entry=gnome_entry_new("QUERY");
	/**
	* Create the buttons.
	**/
	q_dlg_cancel=gnome_stock_button(GNOME_STOCK_BUTTON_CANCEL);
	q_dlg_query=gtk_button_new_with_label(_("Query"));
	/**
	* Set the size.
	**/
	gtk_widget_set_usize(q_dlg,260,150);
	/**
	* Add the widgets to the dialog.
	**/
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(q_dlg)->vbox),q_dlg_label);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(q_dlg)->vbox),q_entry);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(q_dlg)->vbox),q_dlg_spin_button);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(q_dlg)->action_area),q_dlg_query);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(q_dlg)->action_area),q_dlg_cancel);
	/**
	* Connect the signals.
	**/
	gtk_signal_connect(GTK_OBJECT(q_dlg_query),"clicked",
		GTK_SIGNAL_FUNC(r_window),NULL);
	gtk_signal_connect(GTK_OBJECT(q_dlg_cancel),"clicked",
		GTK_SIGNAL_FUNC(query_dialog_hide),NULL);
	/**
	* Show the inner-widgets.
	**/
	gtk_widget_show(q_dlg_label);
	gtk_widget_show(q_dlg_spin_button);
	gtk_widget_show(q_entry);
	gtk_widget_show(q_dlg_cancel);
	gtk_widget_show(q_dlg_query);
}

void query_dialog(GtkWidget *widget,gpointer useless)
{
	/**
	* Create the dialog.
	**/
	query_dialog_create();
	/**
	* Set the window-icon.
	**/
	#ifdef USE_WINDOW_ICON
        gnome_window_icon_set_from_file(GTK_WINDOW(q_dlg),WINDOW_ICON);
        #endif
	/**
	* Set the window-properties.
	**/
	gtk_window_set_title(GTK_WINDOW(q_dlg),_("gtranslator -- query the message db"));
	gtk_window_set_wmclass(GTK_WINDOW(q_dlg),"gtranslator","gtranslator");
	gtk_widget_show(q_dlg);
}

void query_dialog_hide(GtkWidget *widget,gpointer useless)
{
	/**
	* Hide the dialog.
	**/
	gtk_widget_hide(q_dlg);
}

void r_window(GtkWidget *widget,gpointer useless)
{
	GtkWidget *r_label,*r_result;
	 /**
        * The gchar holding the result.
        **/
        gchar *result;
	gchar result_string[256];
	result_string[0]='\0';
	/**
	* Set the challenge-length as specified in the query-dialog.
	**/
	set_challenge_length(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(q_dlg_spin_button)));
	/**
	* Get the result.
	*
	* Note: This is really ugly but it avoids sparing many lines for this.
	**/
	result=get_from_msg_db(gtk_entry_get_text(GTK_ENTRY(gnome_entry_gtk_entry(GNOME_ENTRY(q_entry)))));
	sprintf(result_string,_("Result: `%s'"),result);
	/**
	* Hide the previous query dialog.
	**/
	gtk_widget_hide(q_dlg);
	/**
	* Create the dialog.
	**/
	r_window_dlg=gtk_dialog_new();
	/**
	* An explaining label in front of the result.
	**/
	r_label=gtk_label_new(result_string);
	/**
	* Create the buttons.
	**/
	r_window_cancel=gnome_stock_button(GNOME_STOCK_BUTTON_CANCEL);
	r_window_apply=gtk_button_new_with_label(_("Insert"));
	/**
	* Set the window properties.
	**/
	gtk_window_set_title(GTK_WINDOW(r_window_dlg),_("gtranslator -- result for your query"));
	gtk_window_set_wmclass(GTK_WINDOW(r_window_dlg),"gtranslator","gtranslator");	
	/**
	* Place the other widgets.
	**/
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(r_window_dlg)->vbox),r_label);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(r_window_dlg)->action_area),r_window_apply);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(r_window_dlg)->action_area),r_window_cancel);
	/**
	* Show again the inner widgte first.
	**/
	gtk_widget_show(r_label);
	gtk_widget_show(r_window_apply);
	gtk_widget_show(r_window_cancel);
	/**
        * Set the window-icon.
        **/
        #ifdef USE_WINDOW_ICON
        gnome_window_icon_set_from_file(GTK_WINDOW(q_dlg),WINDOW_ICON);
        #endif
	/**
	* Set the signals.
	**/
	gtk_signal_connect(GTK_OBJECT(r_window_cancel),"clicked",
		GTK_SIGNAL_FUNC(r_window_hide),NULL);
	/**
	* Show it now.
	**/
	gtk_widget_show(r_window_dlg);
}

void r_window_hide(GtkWidget *widget,gpointer useless)
{
	/**
	* Hide the result window.
	**/
	gtk_widget_hide(r_window_dlg);
}
