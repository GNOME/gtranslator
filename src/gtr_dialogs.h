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

#ifndef GTR_GTR_DIALOGS_H
#define GTR_GTR_DIALOGS_H 1

#ifdef HAVE_CONFIG_H	
	#include <config.h>
#endif

#include "interface.h"
#include "support.h"
#include "parse.h"

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
* 6/The search dialog widgets
**/
GtkWidget *s_box_dlg,*s_box_string;
GtkWidget *s_box_ok,*s_box_cancel;

/**
* 7/The GoTo dialog widgets
**/
GtkObject *g_dlg_line_adjustment;
GtkWidget *g_dlg,*g_dlg_line;
GtkWidget *g_dlg_ok,*g_dlg_cancel;

/**
* 8/The basical search dialog widgets
**/
GtkWidget *find_dlg,*find_dlg_cancel,*find_dlg_ok;

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

/**
* The search-box routines
**/
void s_box_create();
void s_box_show();
void s_box_hide(GtkWidget *widget,gpointer useless);
void s_box(GtkWidget *widget,gpointer useless);

/**
* The search-function(s)
**/
void search(GtkWidget *widget,gpointer useless);

/**
* The GoTo line dialog methods
**/
void goto_dlg_create();
void goto_dlg_show();
void goto_dlg_hide(GtkWidget *widget,gpointer useless);
void goto_dlg(GtkWidget *widget,gpointer useless);

/**
* The basical search dialog
**/
void find_dialog(GtkWidget *widget,gpointer title_type);
void find_dialog_hide(GtkWidget *widget,gpointer useless);

#endif
