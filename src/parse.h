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

#ifndef GTR_PARSE_H
#define GTR_PARSE_H 1

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
* The file's name
**/
static gchar *po_file=NULL;

/**
* A gboolean for the silly question if a file is open ...
**/
gboolean file_opened;

/**
* A silly question wants a silly answer ...
**/
gboolean file_changed;

/**
* If we're at the last/first entries ...
**/
gboolean first_entry,last_entry;

/**
* Center of interest are these lists -- at least for me
**/
GList *msgids,*msgstrs,*head,*temp;

/**
* A simple file check .
**/
void check_file(FILE *);

/**
* The internally used parse-function
**/
void parse(gchar *po);

/**
* The new method for the widgets
**/
void parse_the_file(GtkWidget *widget,gpointer filename);

/**
* General functions are following -- these do operate on the
*  global lists where they get the first/last/next/previous
*   msgid & mggstr's ...
**/

/**
* Get first msgid, msgstr
**/
gchar *get_first_msgid();
gchar *get_first_msgstr();

/**
* Get previous msgid, msgstr 
**/
gchar *get_prev_msgid();
gchar *get_prev_msgstr();

/**
* Get next msgid, msgstr
**/
gchar *get_next_msgid();
gchar *get_next_msgstr();

/**
* Get last msgid, msgstr
**/
gchar *get_last_msgid();
gchar *get_last_msgstr();

#endif // GTR_PARSE_H
