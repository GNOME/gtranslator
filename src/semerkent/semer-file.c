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

#include "semer-csv.h"
#include "semer-file.h"
#include "semer-opentag.h"
#include "semer-tmx.h"
#include "semer-umtf.h"
#include "semer-utils.h"
#include "semer-xliff.h"

#include <string.h>

G_BEGIN_DECLS

/*
 * The internally used entry removal function which is used as background
 *  functions for the entry removal function.
 */
static gint semer_file_remove_entry_internal(SemerFile *file, SemerEntry *entry,
       gboolean remove_all_or_not);

/*
 * Create/delete the SemerFile's.
 */
SemerFile *semer_file_new(void)
{
	SemerFile *file=g_new0(SemerFile, 1);

	file->header=semer_header_new_with_defaults();
	file->filename=NULL;
	file->type=SEMER_NULL;
	file->entries=NULL;

	return file;
}

void semer_file_free(SemerFile *file)
{
	g_return_if_fail(SEMER_FILE(file)!=NULL);

	semer_header_free(SEMER_FILE(file)->header);
	semer_utils_free(SEMER_FILE(file)->filename);
	
	while(SEMER_FILE(file)->entries)
	{
		semer_entry_free(SEMER_FILE(file)->entries->data);
		
		SEMER_FILE(file)->entries=SEMER_FILE(file)->entries->next;
	}
	
	g_list_free(SEMER_FILE(file)->entries);
	
	semer_utils_free(SEMER_FILE(file));
}

/*
 * Set the filename part of the SemerFile sanely.
 */
void semer_file_set_filename(SemerFile *file, const gchar *filename)
{
	g_return_if_fail(SEMER_FILE(file)!=NULL);
	g_return_if_fail(filename!=NULL);

	semer_utils_strset(&SEMER_FILE(file)->filename, filename, NULL);
}

/*
 * Return a copy of the SemerFile's filename.
 */
gchar *semer_file_get_filename(SemerFile *file)
{
	/*
	 * First, select all "mad" cases before g_strdup'ing the filename.
	 */
	g_return_val_if_fail(SEMER_FILE(file)!=NULL, NULL);
	g_return_val_if_fail(SEMER_FILE(file)->filename!=NULL, NULL);

	return g_strdup(SEMER_FILE(file)->filename);
}

/*
 * Calls the corresponding save function call for the given SemerFile's
 *  file(-type).
 */
void semer_file_save(SemerFile *file)
{
	g_return_if_fail(SEMER_FILE(file)!=NULL);
	g_return_if_fail(SEMER_FILE(file)->type!=SEMER_NULL);

	switch(SEMER_FILE(file)->type)
	{
		case SEMER_CSV:
			semer_csv_save_file(SEMER_FILE(file));
				break;

		case SEMER_OPENTAG:
			semer_opentag_save_file(SEMER_FILE(file));
				break;

		case SEMER_TMX:
			semer_tmx_save_file(SEMER_FILE(file));
				break;

		case SEMER_UMTF:
			semer_umtf_save_file(SEMER_FILE(file));
				break;

		case SEMER_XLIFF:
			semer_xliff_save_file(SEMER_FILE(file));
				break;

		default:
			break;
	}
}

/*
 * Determine the support state.
 */
gboolean semer_file_supported(const gchar *filename)
{
	xmlDocPtr doc;
	
	g_return_val_if_fail(filename!=NULL, FALSE);

	/*
	 * If the file is a po file, we shouldn't try to do it the xml-way.
	 */
	if(strstr(filename, ".po"))
	{
		return TRUE;
	}

	/*
	 * We can handle CSV files, so get it right here .-)
	 */
	if(strstr(filename, ".csv"))
	{
		return TRUE;
	}
	
	/*
	 * XML type checks.
	 */
	doc=xmlParseFile(filename);
	g_return_val_if_fail(doc!=NULL, FALSE);
	
	if(semer_xml_doctype_cmp(doc, "tmx"))
	{
		xmlFreeDoc(doc);
		return TRUE;
	}
	else if(semer_xml_doctype_cmp(doc, "opentag"))
	{
		xmlFreeDoc(doc);
		return TRUE;
	}
	else if(semer_xml_doctype_cmp(doc, "salt"))
	{
		xmlFreeDoc(doc);
		return TRUE;
	}
	else if(semer_xml_doctype_cmp(doc, "umtf"))
	{
		xmlFreeDoc(doc);
		return TRUE;
	}
	else if(semer_xml_doctype_cmp(doc, "xliff"))
	{
		xmlFreeDoc(doc);
		return TRUE;
	}
	else
	{
		xmlFreeDoc(doc);
		return FALSE;
	}
}

/*
 * Return a copy of the given file's header.
 */
SemerHeader *semer_file_get_header(SemerFile *file)
{
	g_return_val_if_fail(SEMER_FILE(file)!=NULL, NULL);
	return SEMER_HEADER(semer_header_copy(SEMER_FILE(file)->header));
}

/*
 * Get the file type for an existing SemerFile.
 */
SemerFileType semer_file_get_type(SemerFile *file)
{
	g_return_val_if_fail(SEMER_FILE(file)->filename!=NULL, SEMER_NULL);
	
	return (semer_file_get_type_from_file(SEMER_FILE(file)->filename));
}

/*
 * Get the file type from the filename & file-content.
 */
SemerFileType semer_file_get_type_from_file(const gchar *filename)
{
	xmlDocPtr doc;

	g_return_val_if_fail(filename!=NULL, SEMER_NULL);
		
	doc=xmlParseFile(filename);
	g_return_val_if_fail(doc!=NULL, SEMER_NULL);

	/*
	 * Detect the xml file's filetype.
	 */
	if(semer_xml_doctype_cmp(doc, "tmx"))
	{
		xmlFreeDoc(doc);
		return SEMER_TMX;
	}
	else if(semer_xml_doctype_cmp(doc, "opentag"))
	{
		xmlFreeDoc(doc);
		return SEMER_OPENTAG;
	}
	else if(semer_xml_doctype_cmp(doc, "umtf"))
	{
		xmlFreeDoc(doc);
		return SEMER_UMTF;
	}
	else if(semer_xml_doctype_cmp(doc, "xliff"))
	{
		xmlFreeDoc(doc);
		return SEMER_XLIFF;
	}

	xmlFreeDoc(doc);
	return SEMER_NULL;
}

/*
 * Load the translation memory from the given filename.
 */
SemerFile *semer_file_new_from_file(const gchar *filename)
{
	g_return_val_if_fail(filename!=NULL, NULL);
	
	if(semer_file_supported(filename))
	{
		SemerFile *file=g_new0(SemerFile, 1);
		
		/*
		 * Initialize the SemerFile from the given file.
		 */
		file->filename=g_strdup(filename);
		file->header=NULL;
		file->type=SEMER_NULL;
		file->entries=NULL;
		
		return file;
	}
	else
	{
		return NULL;
	}
}

/*
 * A general wrapping function for writing the SemerEntry's via custom 
 *  write functions.
 */
void semer_file_write_all_entries(SemerFile *file, xmlDocPtr doc,
	SemerEntryWriteFunction write_entry_function)
{
	g_return_if_fail(SEMER_FILE(file)!=NULL);
	g_return_if_fail(doc!=NULL);
	g_return_if_fail(write_entry_function!=NULL);

	/*
	 * Operate only if there are any SemerEntry's for the given SemerFile.
	 */
	if(SEMER_FILE(file)->entries)
	{
		GList		*entries_list;
		SemerEntry	*entry;

		/*
		 * Get the SemerEntry's list.
		 */
		entries_list=SEMER_FILE(file)->entries;

		while(entries_list!=NULL)
		{
			/*
			 * Get the entry data from the list and use the supplied
			 *  function to write every entry.
			 */
			entry=SEMER_ENTRY(entries_list->data);
			write_entry_function(entry, doc);
			
			entries_list=entries_list->next;
		}
	}
}

/*
 * Add the given SemerEntry to the SemerFile's entries list.
 */
void semer_file_add_entry(SemerFile *file, SemerEntry *entry)
{
	g_return_if_fail(SEMER_FILE(file)!=NULL);
	g_return_if_fail(SEMER_ENTRY(entry)!=NULL);

	/*
	 * Only insert the given entry if it's not already contained in the
	 *  entries list of the SemerFile.
	 */
	if(!g_list_find(SEMER_FILE(file)->entries, (gpointer) entry))
	{
		/*
		 * Prepend a copy of the given SemerEntry to the entries list
		 *  of the SemerFile.
		 */
		SEMER_FILE(file)->entries=g_list_prepend(
			SEMER_FILE(file)->entries,
				semer_entry_copy(entry));

		SEMER_FILE(file)->entries=g_list_reverse(
			SEMER_FILE(file)->entries);
	}
}

/*
 * Remove the given SemerEntry from the SemerFile's entries list -- the gboolean
 *  argument defines whether to remove every occurence or not.
 */
static gint semer_file_remove_entry_internal(SemerFile *file, SemerEntry *entry, 
	gboolean remove_all_or_not)
{
	GList 		*nice_entries=NULL;
	gboolean	already_removed=FALSE;
	
	g_return_val_if_fail(SEMER_FILE(file)!=NULL, 1);
	g_return_val_if_fail(SEMER_FILE(file)->entries!=NULL, -1);

	g_return_val_if_fail(SEMER_ENTRY(entry)!=NULL, 1);

	/*
	 * Get over with the entries list from the SemerFile.
	 */
	nice_entries=SEMER_FILE(file)->entries;

	while(nice_entries!=NULL)
	{
		/*
		 * Check whether the removal is already performed
		 *  once and we should only operate once; return 0 in this
		 *   case.
		 */
		if(!remove_all_or_not && already_removed)
		{
			return 0;
		}
		
		/*
		 * The entry is found if original & translation are the same.
		 */
		if(!g_ascii_strcasecmp(entry->original, 
				SEMER_ENTRY(nice_entries->data)->original) &&
			!g_ascii_strcasecmp(entry->translation, 
				SEMER_ENTRY(nice_entries->data)->translation))
		{
			GList 	*removelist=NULL;
			
			removelist=nice_entries;
			nice_entries=nice_entries->next;
			
			/*
			 * Remove the entry data/link.
			 */
			g_list_remove_link(SEMER_FILE(file)->entries, 
				removelist);

			/*
			 * Cleanup the used variables/data.
			 */
			semer_entry_free(SEMER_ENTRY(removelist->data));
			g_list_free_1(removelist);
			
			already_removed=TRUE;
		}
		else
		{
			nice_entries=nice_entries->next;
		}
	}

	return 0;
}

/*
 * Remove only the first occurence of the SemerEntry -- if any occurence
 *  is found at all.
 */
gint semer_file_remove_entry(SemerFile *file, SemerEntry *entry)
{
	return (semer_file_remove_entry_internal(file, entry, FALSE));
}

/*
 * Remove all occurences of the SemerEntry.
 */
gint semer_file_remove_entry_all(SemerFile *file, SemerEntry *entry)
{
	return (semer_file_remove_entry_internal(file, entry, TRUE));
}

G_END_DECLS
