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

#include "preferences.h"

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
/**
* Sorry, monster macros .. 
**/
#define GTR_DB_LIST_MSG(x)  ((GtrMsg *) (GTR_DB_LIST(x)->data))
#define GTR_DB_LIST_MSGSTR_LIST(x) ((GList *) (GTR_DB_LIST_MSG(x)->msgstrlist))
/**
* Ok, ai may be going to protest against this macro ,-)
**/
#define GTR_DB_LIST_MSGSTR_LIST_ITERATE(x) \
	((GList *) (GTR_DB_LIST_MSGSTR_LIST(x)=GTR_DB_LIST_MSGSTR_LIST(x)->next))
#define GTR_DB_HEADER(x) ((GtranslatorDatabaseHeader *) (GTR_DB(x)->header))
#define GTR_DB_FILENAME(x) ((gchar *) (GTR_DB(x)->filename))
/**
* The author/translator macros.
**/
#define GTR_DB_TRANSLATOR(x) ((gchar *) (GTR_DB_HEADER(x)->translator))
#define GTR_DB_TRANSLATOR_EMAIL(x) ((gchar *) (GTR_DB_HEADER(x)->translator_email))
/**
* The rest...
**/
#define GTR_DB_LANG(x) ((gchar *) (GTR_DB_HEADER(x)->language_ename))
#define GTR_DB_LANG_EMAIL(x) ((gchar *) (GTR_DB_HEADER(x)->language_email))
#define GTR_DB_SERIAL(x) ((gint) (GTR_DB_HEADER(x)->serial))

/**
* The facts of a database header.
**/
typedef struct {
	/**
	* The main translator informations.
	*
	* Note: these are optional in UMTF.
	*
	**/
	gchar *translator;
	gchar *translator_email;
	/**
	* The base language name:
	**/
	/**
	* 1) In English.
	**/
	gchar *language_ename;
	/**
	* 2) In the own language.
	**/
	gchar *language_name;
	/**
	* The detailed language version/
	*  specificiers.
	**/
	gchar *language_code;
	/**
	* The language's mailing address.
	**/
	gchar *language_email;
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
