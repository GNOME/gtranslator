/*
*  -> Fatih Demir [ kabalak@gmx.net ] 
*	This is the parser header ...
*	Totally written by me ; I think you can see it .
*/

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif // HAVE_CONFIG_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h" 

/*
*	two-dimensional array for
*	the char's of msgid & msgstr :
*/
char iline[256][256],sline[256][256],;

/*
*	This are the dimensioneers for the *line-array ..
*/
int count,max_count,old_count;

/*
*	A gboolean for the silly question if a file is open ...
*/

gboolean file_opened;

/*
*	A silly question wants a silly answer ...
*/
gboolean file_changed;

/*
*	For a simple check if we're at the first string 
*/
gboolean at_the_first;

/*
*	... and if we're at the last string ....
*/
gboolean at_the_last;

/*
*	A simple file check .
*/
void check_file(FILE *);

/*
*	The proto for the real parse routine ..
*/
void parse(void);
