/*
 * (C) 2000-2001	Fatih Demir <kabalak@gmx.net>
 *
 * libgtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 * libgtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef LIBGTRANSLATOR_TRANSLATION_DATABASE_H
#define LIBGTRANSLATOR_TRANSLATION_DATABASE_H 1

/*
 * The message database format used by gtranslator in former times.
 */

#include "preferences.h"

/*
 * Generally useful defines for the database fields.
 */
#define GTR_DB(x) ((GtranslatorDatabase *) x)
#define GTR_DB_LIST(x) ((GList *) (GTR_DB(x)->messages))
#define GTR_DB_LIST_ITERATE(x) ((GList *) (GTR_DB_LIST(x)=GTR_DB_LIST(x)->next))
#define GTR_DB_LIST_MSG(x)  ((GtrMsg *) (GTR_DB_LIST(x)->data))
#define GTR_DB_LIST_MSGSTR_LIST(x) ((GList *) (GTR_DB_LIST_MSG(x)->msgstrlist))
/*
 * Ehem, some kind of big-grown macros..
 */
#define GTR_DB_LIST_MSGSTR_LIST_ITERATE(x) \
	((GList *) (GTR_DB_LIST_MSGSTR_LIST(x)=GTR_DB_LIST_MSGSTR_LIST(x)->next))
#define GTR_DB_HEADER(x) ((GtranslatorDatabaseHeader *) (GTR_DB(x)->header))
#define GTR_DB_FILENAME(x) ((gchar *) (GTR_DB(x)->filename))
/*
 * Some defines for easier access to the header fields of a message database.
 */
#define GTR_DB_TRANSLATOR(x) ((gchar *) (GTR_DB_HEADER(x)->translator))
#define GTR_DB_TRANSLATOR_EMAIL(x) ((gchar *) (GTR_DB_HEADER(x)->translator_email))
#define GTR_DB_LANG(x) ((gchar *) (GTR_DB_HEADER(x)->language_ename))
#define GTR_DB_LANG_EMAIL(x) ((gchar *) (GTR_DB_HEADER(x)->language_email))
#define GTR_DB_SERIAL(x) ((gint) (GTR_DB_HEADER(x)->serial))

/*
 * The database header structure.
 */
typedef struct {
	/*
	 * The main translator informations.
	 *
	 * Note: these are optional in UMTF.
	 *
	 */
	gchar *translator;
	gchar *translator_email;
	/*
	 * The base language name:
	 *
	 * 1 - In English.
	 *
	 */
	gchar *language_ename;
	/*
	 * 2 - In the own language.
	 */
	gchar *language_name;
	/*
	 * The detailed language specificiers.
	 */
	gchar *language_code;
	/*
	 * The language's mailing address.
	 */
	gchar *language_email;
	/*
	 * The serial number.
	 */
	gint	serial;
} GtranslatorDatabaseHeader;

/*
 * The main database structure for.
 */
typedef struct {
	/*
	 * The header
	 */
	GtranslatorDatabaseHeader	*header;
	/*
	 * The list of messages (consisting of GtrMsg's).
	 */
	GList				*messages;
	/*
	 * The "real" filename.
	 */
	gchar				*filename;
} GtranslatorDatabase;

#endif
