/**
 * Fatih Demir [ kabalak@gmx.net ]
 *
 * (C) 2000 Published under GNU GPL V 2.0+
 *
 * This is the general place for all dialogs
 * used in gtranslator ...
 *
 * the source ...
 **/

#include "gtr_dialogs.h"

void compile_error_dialog(GtkWidget *widget,gpointer useless)
{
	/**
	 * Should be shown if there is a non-zero return value
	 *  from msgfmt FILE -o TEST_OUTPUT
	 **/
	compile_error=gnome_app_error(GNOME_APP(app1),
		_("An error occured while msgfmt was executed .\nPlease check your .po file again ."));
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
		_("You didn't change anything in the .po-file !\n"));
	gtk_widget_show(no_changes);
}

/**
 * The preferences box help
**/
void prefs_box_help(GtkWidget *widget,gpointer useless)
{
	prefs_box_help_help=_("With the Preferences box you can define some variables\n
with which you can make gtranslator make more work\n like YOU want it to work !");
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
	gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(of_dlg));
	gtk_signal_connect(GTK_OBJECT(of_dlg_ok),"clicked",
	GTK_SIGNAL_FUNC(parse),NULL);
	gtk_signal_connect(GTK_OBJECT(of_dlg_cancel),"clicked",
	GTK_SIGNAL_FUNC(open_file_hide),NULL);
}

void open_file_show()
{
	open_file_create();
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
	sfa_dlg=gtk_file_selection_new(_("gtranslator -- save file as .. "));
	sfa_dlg_ok=GTK_FILE_SELECTION(sfa_dlg)->ok_button;
	sfa_dlg_cancel=GTK_FILE_SELECTION(sfa_dlg)->cancel_button;
	gtk_file_selection_show_fileop_button(GTK_FILE_SELECTION(sfa_dlg));
	/** FIXME FIXME FIXME NOW ! **/
	gtk_signal_connect(GTK_OBJECT(sfa_dlg_ok),"clicked",
	GTK_SIGNAL_FUNC(save_file_as_hide),NULL);
	gtk_signal_connect(GTK_OBJECT(sfa_dlg_cancel),"clicked",
	GTK_SIGNAL_FUNC(save_file_as_hide),NULL);
}

void save_file_as_show()
{
	save_file_as_create();
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
