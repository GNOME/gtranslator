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

#include <fcntl.h>
#include "semer-csv.h"
#include "semer-utils.h"

G_BEGIN_DECLS

/*
 * Reads in the given file; should be a nicely formatted CSV file.
 */
SemerFile *semer_csv_parse_file(const gchar *filename)
{
	SemerFile *file=g_new0(SemerFile, 1);
	
	g_return_val_if_fail(filename!=NULL, NULL);
	
	if(!semer_utils_test_file(filename))
	{
		return NULL;
	}
	else
	{
		gchar	contents[2048];
		FILE 	*fstream;
	
		fstream=fopen(filename, "r");

		while(fgets(contents, 2048, fstream)!=NULL)
		{
			if(contents && contents[0]!='\0' && contents[0]!='\n')
			{
				/* 
				 * FIXME: Parse the entries. 
				 */
			}
		}
		fclose(fstream);
	}
	
	return file;
}

/*
 * Saves the given SemerFile to a file.
 */
void semer_csv_save_file(SemerFile *file)
{
	FILE 		*fstream;
	
	GList		*entries;
	SemerEntry	*entry;
	gboolean	first_entry;
	
	g_return_if_fail(SEMER_FILE(file)!=NULL);
	g_return_if_fail(SEMER_FILE(file)->filename!=NULL);

	/*
	 * Open our destination file for writing.
	 */
	fstream=fopen(file->filename, "w");
	g_return_if_fail(fstream!=NULL);

	/*
	 * Get the entries and write them just to the output file.
	 */
	entries=SEMER_FILE(file)->entries;
	g_return_if_fail(entries!=NULL);
	first_entry=TRUE;

	while(entries!=NULL)
	{
		entry=SEMER_ENTRY(entries->data);

		/*
		 * We use the '|' delimination here -- others are more common
		 *  I guess.
		 */
		if(first_entry)
		{
			/*
			 * Write the language names as "header lines" into the
			 *  CSV file. 
			 */
			fprintf(fstream, "%s|%s\n", entry->original_language,
				entry->translation_language);
		}
		
		fprintf(fstream, "%s|%s\n", entry->original, entry->translation);
		first_entry=FALSE;
	
		entries=entries->next;
	}

	/*
	 * Close the file stream.
	 */
	fclose(fstream);
}
	
G_END_DECLS
