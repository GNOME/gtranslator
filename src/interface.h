/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* This creates only the main app ..
*
* -- the header
**/

#ifndef GTR_INTERFACE_H 
#define GTR_INTERFACE_H 1

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif // HAVE_CONFIG_H

#include <gnome.h>
#include "parse.h"

GtkWidget* create_app1 (void);

/**
* The globally needed widgets
**/

GtkWidget* app1;
GtkWidget* trans_box;
GtkWidget* text1;
GtkWidget* appbar1; 
GtkWidget* search_bar;
GtkWidget* search_button,* search_again_button,* goto_button,* goto_line_button;

/**
* Necessary for the geometry handling :
**/
gchar *gtranslator_geometry;

/**
* For the status messages
**/
gchar status[128];

/**
* Calls the main help for gtranslator
**/ 
void call_help_viewer(GtkWidget *widget,gpointer useless);

/**
* The text-based callbacks
**/
void cut_clipboard(GtkWidget *widget,gpointer useless);
void copy_clipboard(GtkWidget *widget,gpointer useless);
void paste_clipboard(GtkWidget *widget,gpointer useless);
void clear_selection(GtkWidget *widget,gpointer useless);
void text_has_got_changed(GtkWidget *widget,gpointer useless);

#endif // GTR_INTERFACE_H
