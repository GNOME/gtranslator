/**
* Fatih Demir [ kabalak@gmx.net ]
* Gediminas Paulauskas <menesis@delfi.lt>
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* The header for the ´header´-rountines 
*
* -- header
**/

#ifndef GTR_HEADER_STUFF_H
#define GTR_HEADER_STUFF_H 1

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "gtk/gtk.h"

/**
* A typedef for the new structure.
**/
typedef struct _GtrHeader GtrHeader;
#define GTR_HEADER(x) ((GtrHeader *)x)

/**
* The header-structure which should hold the
*  header informations
**/
struct _GtrHeader {
	gchar *prj_name;
	gchar *prj_version;
	gchar *pot_date;
	gchar *po_date;
	gchar *last_translator;
	gchar *language_team;
	gchar *mime_version;
	gchar *mime_type;
	gchar *encoding;
};

/**
* A structure which will be hopefully used now...
**/
GtrHeader *ph;

/**
* This is the current recognition routine.
**/
gboolean get_header(GtrMsg *msg);

/**
* Frees the header structure
**/
void free_header(GtrHeader *the_header);

/**
* Creates and show the dialog for editing the header
**/
void edit_header(GtkWidget *widget,gpointer useless);

#endif 

