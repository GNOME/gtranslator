/**
* Fatih Demir <kabalak@gmx.net>
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

#include <gtk/gtkwidget.h>
#include "messages.h"

/**
* The header-structure which should hold the header information
**/
typedef struct {
	gchar *comment;
	gchar *prj_name;
	gchar *prj_version;
	gchar *pot_date;
	gchar *po_date;
	gchar *translator;
	gchar *tr_email;
	gchar *language;
	gchar *lg_email;
	gchar *mime_version;
	gchar *charset;
	gchar *encoding;
} GtrHeader;

#define GTR_HEADER(x) ((GtrHeader *)x)

/**
* These converts header between GtrMsg and GtrHeader.
**/
GtrMsg * put_header(GtrHeader * h);
GtrHeader * get_header(GtrMsg * msg);

/* Updates PO-Revision-Date and Last-Translator fields */
void update_header(GtrHeader * h);

/**
* Frees the header structure
**/
void free_header(GtrHeader * the_header);

/**
* Creates and shows the dialog for editing the header
**/
void edit_header(GtkWidget * widget, gpointer useless);

#endif
