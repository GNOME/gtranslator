/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* This is the general place for all dialogs
* used in gtranslator ...
*
* the header ...
**/

#ifdef HAVE_CONFIG_H	
	#include <config.h>
#endif // HAVE_CONFIG_H

#include "interface.h"
#include "support.h"

/**
* The widgets for the dialogs & the functions to call them .
**/

/**
* 1/errors have occured while msgfmt was executed .. 
**/
GtkWidget *compile_error;

/**
* 2/no changes were made to the .po-file
**/
GtkWidget *no_changes;

/**
* 3/Open file dialog and the buttons of it
**/
GtkWidget *of_dlg;
GtkWidget *of_dlg_ok,*of_dlg_cancel;

/**
* 4/Save file as dialog and the buttons for this
**/
GtkWidget *sfa_dlg;
GtkWidget *sfa_dlg_ok,*sfa_dlg_cancel;

/**
* 5/Help dialog for the preferences box & the gchar *
**/
GtkWidget *prefs_box_help_dlg;
gchar *prefs_box_help_help;

/**
* The function prototypes :
**/
void compile_error_dialog(GtkWidget *widget,gpointer useless);
void no_changed_dialog(GtkWidget *widget,gpointer useless);

/**
* The file open/save_as file selections and their methods .
**/
void save_file_as_create();
void save_file_as_show();
void save_file_as_hide(GtkWidget *widget,gpointer useless);
void save_file_as(GtkWidget *widget,gpointer useless);
void open_file_create();
void open_file_show();
void open_file_hide(GtkWidget *widget,gpointer useless);
void open_file(GtkWidget *widget,gpointer useless);

/**
* The help-box for the preferences box
**/
void prefs_box_help(GtkWidget *widget,gpointer useless);
