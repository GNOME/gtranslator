/**
* Fatih Demir [ kabalak@gmx.net ]
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
#endif // HAVE_CONFIG_H

#include "parse.h"
#include "interface.h"
#include <time.h>

/**
* A typedef for the new structure.
**/
typedef struct _gtr_header gtr_header;

/**
* The header-structure which should hold the
*  header informations
**/
struct _gtr_header {
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
* The general widgets for the edit-dialog.
**/
GtkWidget *gtr_edit_header_dlg,*gtr_edit_header_dlg_cancel,
		*gtr_edit_header_dlg_apply;
GtkWidget *gtr_prj_name,*gtr_prj_version,*gtr_po_date,*gtr_pot_date,
		*gtr_last_translator,*gtr_language_team,
		*gtr_mime_version,*gtr_mime_type,*gtr_encoding;

/**
* Will be useful later on.
**/
void apply_header(gtr_header *the_header);

/**
* This is the current recognition routine.
**/
void get_header(gchar *hline);

/**
* Creates the edit_header-dialog.
**/
void edit_header_create(gtr_header *head);

/**
* Shows the edit_header-dialog.
**/
void edit_header_show();

/**
* And this one hides it.
**/
void edit_header_hide(GtkWidget *widget,gpointer useless);

/**
* Calls the other functions.
**/
void edit_header(GtkWidget *widget,gpointer useless);


#endif // GTR_HEADER_STUFF_H
