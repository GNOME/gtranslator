/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* the about-box is now also external-placed
*  from the interface.c file ...
*
* -- the header
**/

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif // HAVE_CONFIG_H

#include "support.h"

/**
* The widget for the about box
**/
GtkWidget *about;

/**
* This creates the about box
**/
void about_box();
void about_box_show(GtkWidget *widget,gpointer useless);
