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

/*
*	The hoped-to-be-maximal-count-for-headers
*/

int max_header_lines_count=15;

/*
*	The char-array for the header-lines :
*/

char header[128][max_header_lines_count];

/*
*	And some ´delims´ 
*/

const char *last_id="\n\"";
const char *cutties="\"Content-Transfer";

/*
*	The routines for header_stuff
*/
char *cut_the_header(FILE *,const char *);

/*
*	A function for testing if we're at the end
*	of the header .
*/
int are_we_at_the_end(char,const char *);
