/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
 *
 * semerkent is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU Library General Public License as published by the
 *   Free Software Foundation; either version 2 of the License, or (at your
 *    option) any later version.
 *    
 * semerkent is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 *    License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * semerkent is being developed around gtranslator, so that any wishes or own
 *  requests should be mailed to me, as I will naturally keep semerkent very
 *   close to gtranslator. This shouldn't avoid any use of semerkent from other
 *    applications/packages.
 *
 * The homepage for semerkent is: http://semerkent.sourceforge.net
 */

#ifndef SEMER_FILE_H
#define SEMER_FILE_H 1

#include "semer-entry.h"
#include "semer-header.h"

G_BEGIN_DECLS

/*
 * The entry writing functions are all SemerEntryWriteFunction's.
 */
typedef void	(*SemerEntryWriteFunction) (SemerEntry *entry, xmlDocPtr doc);

/*
 * The filetypes are supposed to be enum'd here:
 */
typedef enum
{
	SEMER_CSV,
	SEMER_OPENTAG,
	SEMER_TMX,
	SEMER_UMTF,
	SEMER_XLIFF,
	SEMER_NULL
} SemerFileType;

/*
 * The own file structure -- all general operations should be available
 *  in here.
 */
typedef struct
{
	SemerHeader 	*header;
	SemerFileType  	type;
	gchar		*filename;
	
	GList		*entries;
} SemerFile;

#define SEMER_FILE(x) ((SemerFile *) x)
#define IS_SEMER_FILE(x) (sizeof(x) == sizeof(SemerFile *))

/*
 * Create a general SemerFile/delete it.
 */
SemerFile *semer_file_new(void);
void semer_file_free(SemerFile *file);

/*
 * Load the translation memory from the given filename.
 */
SemerFile *semer_file_new_from_file(const gchar *filename);

/* 
 * _set_: Changes the given filename to be the SemerFile's filename.
 *
 * _get_: Returns a copy of the filename.
 */
void semer_file_set_filename(SemerFile *file, const gchar *filename);
gchar *semer_file_get_filename(SemerFile *file);

/*
 * Encapsulates the write calls -- calls the right save function for the
 *  given SemerFile's file(-type).
 */
void semer_file_save(SemerFile *file);

/*
 * Return the file types -- the SemerFile and the gchar based calls.
 */
SemerFileType semer_file_get_type(SemerFile *file);
SemerFileType semer_file_get_type_from_file(const gchar *filename);

/*
 * Return a copy of the given file's header.
 */
SemerHeader *semer_file_get_header(SemerFile *file);

/*
 * Check whether we do support the given file's type.
 */
gboolean semer_file_supported(const gchar *filename);

/*
 * Write all SemerEntry's of the SemerFile using custom writing functions.
 */
void semer_file_write_all_entries(SemerFile *file, xmlDocPtr doc, 
	SemerEntryWriteFunction write_entry_function);

/*
 * Add/remove a SemerEntry to/from the SemerFile; remove returns a usable return
 *  value: -1 in case of non-existing entry "entry" in the file, 1 on error, 
 *   and 0 on success.
 */
void semer_file_add_entry(SemerFile *file, SemerEntry *entry);
gint semer_file_remove_entry(SemerFile *file, SemerEntry *entry);
gint semer_file_remove_entry_all(SemerFile *file, SemerEntry *entry);

G_END_DECLS

#endif /* SEMER_FILE_H */
