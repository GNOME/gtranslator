/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *    
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "id.h"
#include "nautilus-string.h"
#include "parse.h"
#include "utils.h"

#include <libgnome/gnome-util.h>

/*
 * Create and return a GtrID from the current position & po file -- 
 *  if a file is opened yet.
 */
GtrID *gtranslator_id_new()
{
	GtrID *id=g_new0(GtrID, 1);

	g_return_val_if_fail(file_opened==TRUE, NULL);

	id->po_file=g_strdup(po->filename);
	id->po_language=g_strdup(GTR_HEADER(po->header)->language);
	id->po_version=g_strdup(GTR_HEADER(po->header)->prj_version);
	id->po_date=g_strdup(GTR_HEADER(po->header)->po_date);
	id->po_position=g_list_position(po->messages, po->current);

	return id;
}

/*
 * Creates the id from a given gchar-string formatted by gtranslator.
 */
GtrID *gtranslator_id_new_from_string(const gchar *string)
{
	GtrID *id=g_new0(GtrID, 1);
	gchar **values;
	gchar *filename;

	g_return_val_if_fail(string!=NULL, NULL);

	/*
	 * Strip off the filename which is used as the "prefix".
	 */
	filename=nautilus_str_get_prefix(string, ":");
	g_return_val_if_fail(filename!=NULL, NULL);
	
	id->po_file=g_strdup(filename);
	g_free(filename);
	
	/*
	 * Operate on the resting parts of the string-encoded id and split it
	 *  up into its normally 4 parts.
	 */
	filename=nautilus_str_get_after_prefix(string, ":");
	values=g_strsplit(filename, "/", 4);

	/*
	 * Assign the "parsed" values.
	 */
	id->po_language=g_strdup(values[0]);
	id->po_version=g_strdup(values[1]);
	id->po_date=g_strdup(values[2]);
	nautilus_str_to_int(values[3], &(GTR_ID(id)->po_position));

	/*
	 * Free the used variables.
	 */
	g_free(filename);
	g_strfreev(values);

	return id;
}

/*
 * The otherway conversion: convert a GtrID into a plain string.
 */
gchar *gtranslator_id_string_from_id(GtrID *id)
{
	gchar *string;

	g_return_val_if_fail(id!=NULL, NULL);

	string=g_strdup_printf("%s:%s/%s/%s/%i", id->po_file, id->po_language,
		id->po_version, id->po_date, id->po_position);

	return string;
}

/*
 * Is the given GtrID resolvable at all -- is the file present and do the
 *  other specs also match?!
 */
gboolean gtranslator_id_resolvable(GtrID *id)
{
	GtrPo *current_po;
	
	g_return_val_if_fail(id!=NULL, FALSE);

	/*
	 * First check if there's a file with that name.
	 */
	if(!g_file_exists(GTR_ID(id)->po_file)) 
	{
		return FALSE;
	}
	else
	{
		gint potcom=0;
		
		current_po=po;
		
		#define CHECK_HEADER_PART(x); \
			if(!nautilus_strcmp(GTR_HEADER(po->header)->x, \
				GTR_HEADER(current_po->header)->x)) \
				{ \
					potcom++; \
				}
				
		gtranslator_parse_main(GTR_ID(id)->po_file);

		/*
		 * Check the header parts for equality.
		 */
		CHECK_HEADER_PART(language);
		CHECK_HEADER_PART(prj_version);
		CHECK_HEADER_PART(po_date);
		
		#undef CHECK_HEADER_PART

		/*
		 * Are the filenames (somehow) equal?
		 */
		if(!nautilus_strcmp(
			gtranslator_utils_get_raw_file_name(po->filename),
			gtranslator_utils_get_raw_file_name(current_po->filename)))
		{
			potcom++;
		}
		
		/*
		 * At least 3 equalities must have been occured to let the
		 *  GtrID be resolvable.
		 */
		if(potcom >= 3)
		{
			/*
			 * Free the "new-old" po and reassign the saved
			 *  original po variable.
			 */
			gtranslator_po_free();
			po=current_po;

			return TRUE;
		}
	}

	return FALSE;
}

/*
 * Return whether the two GtrID's are equal.
 */
gboolean gtranslator_id_equal(GtrID *one, GtrID *two)
{
	g_return_val_if_fail(one!=NULL, FALSE);
	g_return_val_if_fail(two!=NULL, FALSE);

	if(one==two)
	{
		return TRUE;
	}
	else
	{
		gint potcom=0;

		#define CHECK_CASE(x); \
			if(!nautilus_strcasecmp(GTR_ID(one)->x, GTR_ID(two)->x)) \
			{ \
				potcom++; \
			}

		/*
		 * Check the single string parts for common values and increase
		 *  the potcom variable in this case.
		 */
		CHECK_CASE(po_file);
		CHECK_CASE(po_language);
		CHECK_CASE(po_version);
		CHECK_CASE(po_date);

		/*
		 * The position is checked here extra as it's not a string .-)
		 */
		if(GTR_ID(one)->po_position==GTR_ID(two)->po_position)
		{
			potcom++;
		}

		#undef CHECK_CASE

		/*
		 * Have we got more then/equal to 3 common values? Then the id's are equal.
		 */
		if(potcom >= 3)
		{
			return TRUE;
		}
	}

	/*
	 * On all other cases we were unsuccessful.
	 */
	return FALSE;
}

/*
 * Smally different: compare the GtrID and the id string.
 */
gboolean gtranslator_id_string_equal(GtrID *id, const gchar *string)
{
	GtrID *new=gtranslator_id_new_from_string(string);
	return (gtranslator_id_equal(id, new));

	gtranslator_id_free(&new);
}

/*
 * Free the given GtrID.
 */
void gtranslator_id_free(GtrID **id)
{
	if(GTR_ID(*id))
	{
		g_free(GTR_ID(*id)->po_file);
		g_free(GTR_ID(*id)->po_language);
		g_free(GTR_ID(*id)->po_version);
		g_free(GTR_ID(*id)->po_date);
	}
}
