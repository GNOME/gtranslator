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

#ifndef HANDLE_DB_H
#define HANDLE_DB_H 1

#include <libgtranslator/parse-db.h>
#include <libgtranslator/translation-database.h>
#include <libgtranslator/messages.h>

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
GtranslatorDatabase gtranslator_open_db(gchar *filename);

/**
* Saves the message database.
**/
void gtranslator_save_db(GtranslatorDatabase database);

/**
* Dumps the given database to the given file.
**/
void gtranslator_dump_db(GtranslatorDatabase database,
	gchar *filename);

/**
* "Moves" the given database to the file specified by 
*  "moveto".
**/
void gtranslator_move_db(GtranslatorDatabase database,
	char *moveto);

#endif
