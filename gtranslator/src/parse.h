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
*	Define the thought-to-be-the-maximal-numbers-of\
*	normal-but-not-gnumeric-po-files define 
*/
#define LINE_LENGTH 2048

char iline[256][LINE_LENGTH],sline[256][LINE_LENGTH];
FILE *input_ids,*input_strs;

/*
*	This are the dimensioneers for the *line-array ..
*/
int count,max_count;
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
