/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* The dnd-functions of gtranslator.
*	
* -- header
**/

#ifndef GTR_DND_H
#define GTR_DND_H 1

#include <gnome.h>
#include <gtk/gtk.h>
#include <glib.h>

/**
* The D'n'D types
**/
enum gtr_dnd
{
        GTR_OPEN
};

/**
* The target formats
**/
static GtkTargetEntry dragtypes[] =
{
        { "text/uri-list",0,0 }
};

/**
* Another variable
**/
gint a_counter;

/**
* The gtranslator's dnd-types enumeration.
**/
enum gtr_dnd func;

/**
* The D'n'D function
**/
void gtranslator_dnd(GtkWidget *widget,GdkDragContext *context,
        int x,int y,GtkSelectionData *seldata, guint info,guint time,
                gpointer data);

#endif // GTR_DND_H 
