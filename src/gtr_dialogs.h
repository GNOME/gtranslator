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

#include "callbacks.h"
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
 * The funstion prototypes :
 **/
void compile_error_dialog(GtkWidget *widget,gpointer useless);
void no_changed_dialog(GtkWidget *widget,gpointer useless);
