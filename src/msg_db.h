/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* Here will be the db-type stuff. 
* 
* I'm thinking now about a complete
*  rewrite in order to get it work on top
*   of GConf.
*
* -- the central for the stuff-*
**/

#ifndef GTR_MSG_DB_H
#define GTR_MSG_DB_H 1

#include "gui.h"
#include "parse.h"

/**
* Some static variables for the poptTable in the main
*  routines.
**/
static gchar *file_to_open=NULL;
static gchar *query_string=NULL;

/**
* The default msg_db location .
**/
static gchar *msg_db;

/**
* Defines if the msg_db is opened 
**/
gboolean msg_db_inited;

/**
* Init the msg-db ( open & 
* check the file )
**/
int init_msg_db();

/**
* Close the msg-db ( file )
* & gzip it 
**/
void close_msg_db();

/**
* Put this to the db
**/
int put_to_msg_db(const gchar *,const gchar *);

/**
* Get a similar entry from 
* the msg-db & return a ´gchar *´
**/
gchar *get_from_msg_db(const gchar *);

/**
* Returns the length of the msg-db
* in bytes :
**/
unsigned int get_msg_db_size();

/**
* The linked lists :
**/
GList *msg_list, *cur_list;

#endif
