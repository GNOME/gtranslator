/**
*
* (C) 2000 Fatih Demir -- kabalak / kabalak@gmx.net
*
* This is distributed under the GNU GPL V 2.0 or higher which can be
*  found in the file COPYING for further studies.
*
* Enjoy this piece of software, brain-crack and other nice things.
*
* WARNING: Trying to decode the source-code may be hazardous for all your
*	future development in direction to better IQ-Test rankings!
*
**/

#ifndef LIBGTRANSLATOR_HANDLE_DB_H
#define LIBGTRANSLATOR_HANDLE_DB_H 1

#include "parse-db.h"
#include "translation-database.h"
#include "messages.h"

/**
* General functions of libgtranslator for the message databases
*  handling.
**/

/**
* A "logic-encapsulator" for the gtranslator_dump_db function.
**/
#define gtranslator_copy_db(db, name) gtranslator_dump_db(db, name)

/**
* Opens the database from the specified file ( this needs
*  to be an absolute path).
**/
GtranslatorDatabase *gtranslator_open_db(gchar *filename);

/**
* Saves the message database.
**/
void gtranslator_save_db(GtranslatorDatabase *database);

/**
* Dumps the given database to the given file.
**/
void gtranslator_dump_db(GtranslatorDatabase *database,
	gchar *filename);

/**
* "Moves" the given database to the file specified by 
*  "moveto".
**/
void gtranslator_move_db(GtranslatorDatabase *database,
	char *moveto);

/**
* Adds the given GtrMsg to the GtranslatorDatabase.
**/
void gtranslator_add_to_db(GtranslatorDatabase *database,
	GtrMsg *message);

/**
* Delete the given GtrMsg (the msgid and it's translations)
*  from the database.
**/
void gtranslator_delete_from_db(GtranslatorDatabase *database,
	GtrMsg *message);

/**
* Delete the given msgid from the database.
**/
void gtranslator_delete_msgid_from_db(GtranslatorDatabase *database,
	gchar *msgid);

/**
* Delete the given msgstr from the database.
**/
void gtranslator_delete_msgstr_from_db(GtranslatorDatabase *database,
	gchar *msgstr);

/**
* Search the database for the given searchitem and return a GList
*  consisting of GtrMsgs.
**/
GList *gtranslator_lookup_db(GtranslatorDatabase *database,
	gchar *searchitem);

#endif
