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
*	A structure for the po-files :
*/
struct po_file {
	const char *filename;
	unsigned int maximal_position;
	gboolean db_enabled;	
};

/*
*	The structure for the transations :	
*/
struct message  {
	struct po_file *po-file;
	char *msgid;
	char *msgstr;
	unsigned int position;
};

/*
*	This are the global count variables  
*/
int count,max_count;

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

/*
*	Get next msgid 
*/
int get_next_msgid(FILE *,char *);

/*
*	Get next msgstr
*/
int get_next_msgstr(FILE *,char *);
