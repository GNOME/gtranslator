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

#ifndef TRANSLATION_DATABASE_H
#define TRANSLATION_DATABASE_H 1

#include <libgtranslator/preferences.h>

/**
* The translation/message database structure definitions and
*  some other nice things are here.
**/

/**
* The facts of a database header.
**/
typedef struct {
	/**
	* The author informations.
	**/
	gchar *author;
	gchar *author_email;
	/**
	* The base language.
	**/
	gchar *language;
	/**
	* The detailed language version/
	*  specificiers.
	**/
	gchar *language_code;
	/**
	* The serial number.
	**/
	gint	serial;
} *GtranslatorDatabaseHeader;

/**
* The main structure for a message database.
**/
typedef struct {
	/**
	* The header
	**/
	GtranslatorDatabaseHeader	header;
	/**
	* The list of messages consisting of GtrMsg's.
	**/
	GList				*messages;
	/**
	* The "real" filename of the database.
	**/
	gchar				filename[256];
} *GtranslatorDatabase;

#endif
