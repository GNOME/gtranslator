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
	gchar *project_id;
	gchar *pot_date;
	gchar *po_date;
	gchar *last_translator;
	gchar *language_team;
	gchar *mime_version;
	gchar *mime_type;
	gchar *encoding;
	/**
	* Is this a standard header part?
	**/
	gchar *xtranslator;
};

void apply_header(gtr_header the_header);

#endif // GTR_HEADER_STUFF_H
