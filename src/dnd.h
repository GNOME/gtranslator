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

#include "interface.h"
#include "parse.h"
#include "msg_db.h"
#include "messages.h"

/**
* The gtranslator's dnd-types enumeration.
**/
typedef enum _gtr_dnd_types gtr_dnd_types;

/**
* The used enumeration
**/
gtr_dnd_types dnd_type;

/**
* The D'n'D types
**/
enum _gtr_dnd_types
{
       	TARGET_URI_LIST,
	TARGET_NETSCAPE_URL,
	TARGET_TEXT_PLAIN
};

/**
* The target formats
**/
static GtkTargetEntry dragtypes[] =
{
        {
		"text/uri-list", 0, TARGET_URI_LIST
	},
	{
		"text/plain", 0, TARGET_NETSCAPE_URL
	},
	{
		"text/plain", 0, TARGET_TEXT_PLAIN
	}
};

/**
* Another variable
**/
gint a_counter;

/**
* The D'n'D function
**/
void gtranslator_dnd(GtkWidget *widget,GdkDragContext *context,
        int x,int y,GtkSelectionData *seldata, guint info,guint time,
                gpointer data);

#endif // GTR_DND_H 
