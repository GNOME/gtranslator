/*
*	-> Fatih Demir [ kabalak@gmx.net ]
*	The header for the header-rountines 
*/

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif // HAVE_CONFIG_H

#include "parse.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"

/*
*	Some variables needed for the search-routines .
*/
char tmp[256];
const char *last_id="\n\"";

/*
*	The routines for header_stuff
*/
char *cut_the_header(FILE *,const char *);

/*
*	A function for testing if we're at the end
*	of the header .
*/
int are_we_at_the_end(char,const char *);
