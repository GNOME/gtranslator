/**
* Fatih Demir [ kabalak@gmx.net ] 
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* This is the parser header ...
* Totally written by me ; I think you can see it .
*
* -- the header
**/

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

#include "interface.h"

/**
* This are the global count variables  
**/
int count,max_count;

/**
* A gboolean for the silly question if a file is open ...
**/

gboolean file_opened;

/**
* A silly question wants a silly answer ...
**/
gboolean file_changed;

/**
* For a simple check if we're at the first string 
**/
gboolean at_the_first;

/**
* ... and if we're at the last string ....
**/
gboolean at_the_last;

/**
* A simple file check .
**/
void check_file(FILE *);

/**
* The previous parsing routine
* ; will be removed soon 
**/
void parse(char *);

/**
* Get next msgid 
**/
int get_next_msgid(FILE *,char *);

/**
* Get next msgstr
**/
int get_next_msgstr(FILE *,char *);
