/**
* Fatih Demir [ kabalak@gmx.net ] 
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* This is the parser header ...
* Totally written by me ; I think you can see it .
* 
* Yes, of course ;) I even could replace half of it's contents...
* Gediminas Paulauskas <menesis@delfi.lt>
*
* -- the header
**/

#ifndef GTR_PARSE_H
#define GTR_PARSE_H 1

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "messages.h"
#include "header_stuff.h"
#include "gui.h"

/**
* The new filename
**/
gchar *i_love_this_file;

/**
* A list for the messages.
**/
GList *messages;
void free_messages(void);

/**
* The currently displayed message
**/
GList *cur_msg;

/**
* A gboolean for the silly question if a file is open ...
**/
gboolean file_opened;

/**
* A silly question wants a silly answer ...
**/
gboolean file_changed;

// Marks if the current message was changed;
gboolean message_changed;

/**
* A simple file check .
**/
void check_file(FILE *);

// Updates and returns message status.
GtrMsgStatus get_msg_status(GtrMsg *msg);

/**
* The internally used parse-function
**/
void parse(const char *po);

/**
* Callbacks for the widgets
**/
void parse_the_file(GtkWidget *widget,gpointer of_dlg);
void save_the_file(GtkWidget *widget,gpointer sfa_dlg);
void save_current_file(GtkWidget *widget,gpointer useless);
void revert_file(GtkWidget *widget, gpointer useless);
void close_file(GtkWidget *widget, gpointer useless);

void compile(GtkWidget *widget,gpointer useless);

#endif
