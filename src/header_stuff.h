/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* The header for the ´header´-rountines 
*
**/

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif // HAVE_CONFIG_H

#include "parse.h"
#include "interface.h"

/**
* The maximal header lines count 
*  as a simple define .
**/

#define MAX_HLEN 15

/**
* Some variables needed for the search-routines ...
**/

gchar tmp[256];

/**
* The hoped-to-be-maximal-count-for-headers
**/

int max_header_lines_count=MAX_HLEN;

/**
* The char-array for the header-lines :
**/

gchar header[128][MAX_HLEN];

/**
* And some ´delims´ :
*
* The last character at the end should always
*  be a ´\n´ .
**/

const gchar *last_id="\n\"";

/**
* I hope that every .po-file has got
*  this line at the end .
**/

const gchar *cutties="\"Content-Transfer";

/**
* The routines for header_stuff
**/

void cut_the_header_off_it(FILE *,const gchar *);

/**
* A function for testing if we're at the end
* of the header .
**/

int are_we_at_the_end(gchar *,const gchar *);
