/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* Here will be the db-type stuff. 
* 
* I'm thinking now about a complete
*  rewrite in order to get it work with XML
*
* -- the central for the stuff-*
**/

#ifndef GTR_MSG_DB_H
#define GTR_MSG_DB_H 1

#include <gtk/gtkwidget.h>

/* The filename of msg_db */
gchar *msg_db;

/* Defines if the msg_db is opened */
gboolean msg_db_inited;

/* Init the msg-db ( open & check the file ) */
int init_msg_db(void);

/* Close the msg-db ( file ) & gzip it */
void close_msg_db(void);

/* A simple callback for the msg_db-adding method. */
void append_to_msg_db(GtkWidget * widget, gpointer useless);

/* Put message pair to the db */
int put_to_msg_db(const gchar * msg_id, const gchar * msg_translation);

/* Get a similar entry from the msg-db */
gchar * get_from_msg_db(const gchar *);

/* Sets the challenge-length. */
void set_challenge_length(int length);

/* Gets the challenge-length. */
unsigned int get_challenge_length(void);

#endif
