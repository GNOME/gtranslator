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
* The general macros for the database fields.
**/
#define GTR_DB(x) ((GtranslatorDatabase *) x)
#define GTR_DB_LIST(x) ((GList *) (GTR_DB(x)->messages))
#define GTR_DB_LIST_ITERATE(x) ((GList *) (GTR_DB_LIST(x)=GTR_DB_LIST(x)->next))
#define GTR_DB_LIST_MSG(x)  ((GtrMsg *) (GTR_DB_LIST(x)->data))
#define GTR_DB_LIST_MSGSTR_LIST(x) ((GList *) (GTR_DB_LIST_MSG(x)->msgstrlist))
#define GTR_DB_LIST_MSGSTR_LIST_ITERATE(x) \
	((GList *) (GTR_DB_LIST_MSGSTR_LIST(x)=GTR_DB_LIST_MSGSTR_LIST(x)->next))
#define GTR_DB_HEADER(x) ((GtranslatorDatabaseHeader *) (GTR_DB(x)->header))
#define GTR_DB_FILENAME(x) ((gchar *) (GTR_DB(x)->filename))
/**
* The header part-concerning macros.
**/
#define GTR_DB_AUTHOR(x) ((gchar *) (GTR_DB_HEADER(x)->author))
#define GTR_DB_LANG(x) ((gchar *) (GTR_DB_HEADER(x)->language))
#define GTR_DB_AUTHOR_EMAIL(x) ((gchar *) (GTR_DB_HEADER(x)->author_email))
#define GTR_DB_SERIAL(x) ((gint) (GTR_DB_HEADER(x)->serial))

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
} GtranslatorDatabaseHeader;

/**
* The main structure for a message database.
**/
typedef struct {
	/**
	* The header
	**/
	GtranslatorDatabaseHeader	*header;
	/**
	* The list of messages consisting of GtrMsg's.
	**/
	GList				*messages;
	/**
	* The "real" filename of the database.
	**/
	gchar				*filename;
} GtranslatorDatabase;

#endif
