/**
* Fatih Demir <kabalak@gmx.net>
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* The dnd-functions of gtranslator.
*	
* -- header
**/

#ifndef GTR_DND_H
#define GTR_DND_H 1

#include <gtk/gtkselection.h>

/**
* The gtranslator's dnd-types enumeration.
**/
enum {
	TARGET_URI_LIST,
	TARGET_NETSCAPE_URL,
	TARGET_TEXT_PLAIN
};

/**
* The used enumeration
**/
guint dnd_type;

/**
* The D'n'D function
**/
void gtranslator_dnd(GtkWidget * widget, GdkDragContext * context, int x,
		     int y, GtkSelectionData * seldata, guint info,
		     guint time, gpointer data);

#endif
