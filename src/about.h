/**
* Fatih Demir <kabalak@gmx.net>
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* the about-box is now also external-placed
*  from the interface.c file ...
*
* -- the header
**/

#ifndef GTR_ABOUT_H
#define GTR_ABOUT_H 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gui.h"
#include "dialogs.h"

void about_box(GtkWidget * widget, gpointer useless);

#endif
