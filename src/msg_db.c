/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* Here are the routines which could be very
*  useful if you're think of slightly auto-
*   generated translations ..
* 
* Or of globally accesable strings in every 
*  translation.
*
* -- the central-of-interest
**/

#include "msg_db.h"
#include "gui.h"
#include "find.h"

#include <stdio.h>

/**
* The db file-stream
**/
static FILE *db_stream;

/**
* The message char
**/
static gchar msg_messages[128];

/**
* The linked lists :
**/
static GList *msg_list = NULL;

/* The challenge length. */
static guint challen;

int init_msg_db(void)
{
	/**
	* Use the default location if no other msg_db has been defined.
	**/
	if (!msg_db)
		msg_db = g_strdup_printf("%s/%s", g_get_home_dir(), "msg.db");
	db_stream = fopen(msg_db, "r+");
	if (db_stream == NULL) {
		g_message(_("No message db found! Creating one in `%s'."),
			  msg_db);
		db_stream = fopen(msg_db, "a+");
	}
	/**
	* Get every message from the msg_db as a new member 
	*  to the linked list while reading the file ; 
	**/
	while ((fgets(msg_messages, sizeof(msg_messages), db_stream) != NULL)) {
		/**
 		* 2) Parse the msg_db and add every entry to the
		*     the linked lists 
		**/
		msg_list = g_list_append(msg_list, (gpointer) msg_messages);
	}
	/**
	* 3) Check if we had lost the list 
	**/
	if (!msg_list) {
		/**
		* 3.1) No list, no fun 
		**/
		g_error(_("gtranslator lost the list of translations!\n"));
	}
	msg_db_inited = TRUE;
	return 0;
}

void close_msg_db(void)
{
	if (!msg_db_inited)
		return;
	fclose(db_stream);
	msg_db_inited = FALSE;
	g_free(msg_db);
}

int put_to_msg_db(const gchar * msg_id, const gchar * msg_translation)
{
	/**
	* Check if we've got a working ( inited ) msg_db ?
	**/
	if (msg_db_inited != TRUE) {
		/**
		* Show a little warning ...
		**/
		g_warning(_("The message db `%s' hasn't been initted yet!\n"),
			  msg_db);
			return 1;
	}
	if (!msg_translation) {
			/**
			* Warn if there isn't any message which could be added
			*  to the msg_db .
			**/
			g_warning(_("Got no message entry!\n"));
			return 1;	
		}
	/* Go to the end of the file */
	fseek(db_stream, 0L, SEEK_END);
			/**
			* Add the given parameters to the msg_db .
			**/
	fputs("\n", db_stream);
	fputs(msg_id, db_stream);
			/**
			* These ';;' are used as separators and shouldn't
			*  appear in normal context ...
			**/
	fputs(";;", db_stream);
	fputs(msg_translation, db_stream);
	return 0;
}

/**
* Saves the entry in the msg_db.
**/
void append_to_msg_db(GtkWidget * widget, gpointer useless)
{
	gchar *tc1;
	gchar *tc2;
	/**
	* Get the text-entries.
	**/
	tc1 = gtk_editable_get_chars(GTK_EDITABLE(trans_box), 0, -1);
	tc2 = gtk_editable_get_chars(GTK_EDITABLE(text1), 0, -1);
	/**
	* Call the msg_db-function for adding new entries.
	**/
	put_to_msg_db(tc2, tc1);
		g_free(tc1);
		g_free(tc2);
}

static gchar *result = NULL;
					
static gboolean find_similar(gchar * db_entry, gchar * query)
{
	if (!g_strncasecmp(db_entry, query, challen)) {
		gchar *emp = strstr(db_entry, ";;");
		if (emp) {
			result = g_strdup(emp+2);
			return TRUE;
		}	
	}
	return FALSE;
}

gchar * get_from_msg_db(const gchar * query)
{
	if (msg_db_inited != TRUE) {
		g_warning(_("The message db seems not to be initted!\n"));
		return _("No message db available!");
	}
	if (!msg_list)
		return _("The message db is empty!");
	if (for_each_msg(msg_list, (FEFunc) find_similar, (gpointer) query))
		return result;
	return _("No entry found");
}

/**
* Sets the challenge-length ( oh, we're doing checks, yeah ..)
**/
void set_challenge_length(int length)
{
	if (length <= 1) {
		g_warning(_("Trying to set an irregular challange length!"));
		challen = 0;
	} else {
		challen = length;
	}
}

/**
* Gets the challenge-length.
**/
unsigned int get_challenge_length(void)
{
		return challen;
}
