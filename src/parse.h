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
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "gui.h"
#include "messages.h"
#include "header_stuff.h"
#include "gtr_dialogs.h"

/**
* The new filename
**/
static gchar *i_love_this_file=NULL;

/**
* The counting variable for the messages.
**/
static gint msg_pair;

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
void parse_the_file(GtkWidget *widget,gpointer useless);

/**
* General functions are following -- these do operate on the
*  global lists where they get the first/last/next/previous
*   msgid & msgstr's ...
**/

/**
* Get first msgid, msgstr
**/
void get_first_msg(GtkWidget *widget,gpointer useless);

/**
* Get previous msgid, msgstr 
**/
void get_prev_msg(GtkWidget *widget,gpointer useless);

/**
* Get next msgid, msgstr
**/
void get_next_msg(GtkWidget *widget,gpointer useless);

/**
* Get last msgid, msgstr
**/
void get_last_msg(GtkWidget *widget,gpointer useless);

/**
* The search function
**/
gchar *search_do(GtkWidget *widget,gpointer wherefrom);

/**
* Two lists for the messages.
**/
GList *messages;

#endif
