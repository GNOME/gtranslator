/*
*  -> Fatih Demir [ kabalak@gmx.net ] ; GNU GPL V 2.0+-compliant
*	This is the parser header ...
*	Totally written by me ; I think you can see it .
*/

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

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
*	This are the dimensioneers for the *line-array ..
*/
static int count=0;
static int max_count=65000;
char ids_file[256],strs_file[256];

/*
*	A gboolean for the silly question if a file is open ...
*/

gboolean file_opened;

/*
*	A silly question wants a silly answer ...
*/
gboolean file_changed;

/*
*	A simple file check .
*/
void check_file(FILE *);

/*
*	Opens the files "msgids.dat" & "msgstrs.dat" , 
*	created by split_it_with_awk.sh .
*/
int open_the_files(void);

/*
*	Ok , you see I'm not a pure-C-fanatic (as I won't use malloc myself ..)
*/
void parse_this_file_all_through(void);

/*
*	This simply inits the process with a line ...
*/
void init_reading(void);
