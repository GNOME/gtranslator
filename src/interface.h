/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* This creates only the main app ..
*
* -- the header
**/

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif // HAVE_CONFIG_H

#include <gnome.h>

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
* Calls the main help for gtranslator
**/ 
void call_help_viewer(GtkWidget *widget,gpointer useless);
