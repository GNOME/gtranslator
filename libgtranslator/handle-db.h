/*
 * (C) 2000 	Fatih Demir <kabalak@gmx.net>
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

#ifndef LIBGTRANSLATOR_HANDLE_DB_H
#define LIBGTRANSLATOR_HANDLE_DB_H 1

/*
 * The message database handling routines of libgtranslator.
 */ 

#include "parse-db.h"
#include "translation-database.h"
#include "messages.h"

/*
 * Opens the database from the specified file (this needs
 *  to be an absolute path).
 */
GtranslatorDatabase 	*gtranslator_open_db(gchar *filename);

/*
 * Saves the given message database.
 */
void 			gtranslator_save_db(GtranslatorDatabase *database);

/*
 * Dumps the given database to the file.
 */
void 			gtranslator_dump_db(GtranslatorDatabase *database,
			gchar *filename);

/*
 * "Moves" the given database to the specified file.
 */
void 			gtranslator_move_db(GtranslatorDatabase *database,
			char *moveto);

/*
 * Adds the given GtrMsg to the database.
 */
void 			gtranslator_add_to_db(GtranslatorDatabase *database,
			GtrMsg *message);

/*
 * Deletes the given GtrMsg (the msgid and it's translations)
 *  from the database.
 */
void 			gtranslator_delete_from_db(GtranslatorDatabase *database,
			GtrMsg *message);

/*
 * Deletes the given msgid from the database.
 */
void 			gtranslator_delete_msgid_from_db(GtranslatorDatabase *database,
			gchar *msgid);

/*
 * Deletes the given msgstr from the database.
 */
void 			gtranslator_delete_msgstr_from_db(GtranslatorDatabase *database,
			gchar *msgstr);

/*
 * Search the database for the given searchitem and return a GList
 *  consisting of GtrMsgs which do include this searchitem.
 */
GList 			*gtranslator_lookup_db(GtranslatorDatabase *database,
			gchar *searchitem);

#endif
