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
