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
#include "preferences.h"
#include "utils.h"

#include <libgnome/gnome-util.h>

/*
 * A simple but very useful macro for more safe g_strdup usage:
 */
#define IF_STRDUP(target, value); \
	if(value) \
	{ \
		target=g_strdup(value); \
	} \
	else \
	{ \
		target=NULL; \
	}

/*
 * This should also be configurable later on.
 */
#define MAX_ID_LIST_LENGTH 10

/*
 * Create and return a GtrID from the current position & po file -- 
 *  if a file is opened yet.
 */
GtrID *gtranslator_id_new()
{
	GtrID *id=g_new0(GtrID, 1);

	g_return_val_if_fail(file_opened==TRUE, NULL);
	g_return_val_if_fail(po->filename!=NULL, NULL);

	id->po_file=g_strdup(po->filename);
	
	IF_STRDUP(id->po_language, GTR_HEADER(po->header)->language);
	IF_STRDUP(id->po_version, GTR_HEADER(po->header)->prj_version);
	IF_STRDUP(id->po_date, GTR_HEADER(po->header)->po_date);
	
	id->po_position=g_list_position(po->messages, po->current);

	return id;
}

/*
 * Creates the id from a given gchar-string formatted by gtranslator.
 */
GtrID *gtranslator_id_new_from_string(const gchar *string)
{
	GtrID 	*id=g_new0(GtrID, 1);
	gchar 	**values;
	gchar 	*filename,
		*encoding_area;

	g_return_val_if_fail(string!=NULL, NULL);

	/*
	 * Check if the given string is a GtrID "URI" at all.
	 */
	if(!nautilus_istr_has_prefix(string, "gtranslator_id:") ||
		!strchr(string, '/') || !strchr(string, '#'))
	{
		return NULL;
	}
	else
	{
		/*
		 * Strip of the file name from the GtrID.
		 */
		gchar *tempzulu;
		
		tempzulu=nautilus_str_get_prefix(string, "#");
		g_strreverse(tempzulu);

		/*
		 * Some reverse-thinking...
		 */
		filename=nautilus_str_get_prefix(tempzulu,
			":di_rotalsnartg");
		
		g_free(tempzulu);
		g_return_val_if_fail(filename!=NULL, NULL);
	}
	
	id->po_file=g_strdup(filename);
	g_free(filename);
	
	/*
	 * Operate on the resting parts of the string-encoded id and split it
	 *  up into its normally 4 parts.
	 */
	encoding_area=nautilus_str_get_after_prefix(string, "#");
	values=g_strsplit(encoding_area, "/", 4);
	g_free(encoding_area);

	IF_STRDUP(id->po_language, values[0]);
	IF_STRDUP(id->po_version, values[1]);
	IF_STRDUP(id->po_date, values[2]);
	
	/*
	 * Always be quite safe about the GtrID values assigned in these routines.
	 */
	if(values[3])
	{
		nautilus_str_to_int(values[3], &(GTR_ID(id)->po_position));
	}
	else
	{
		id->po_position=-1;
	}

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

	string=g_strdup_printf("gtranslator_id:%s#%s/%s/%s/%i", id->po_file, 
		id->po_language, id->po_version, id->po_date, id->po_position);

	return string;
}

/*
 * Encapsulate the calls for creating a new GtrID string.
 */
gchar *gtranslator_id_new_id_string()
{
	GtrID *id;
	gchar *id_string;

	id=gtranslator_id_new();

	g_return_val_if_fail(id!=NULL, NULL);
	
	id_string=gtranslator_id_string_from_id(GTR_ID(id));
	
	gtranslator_id_free(id);
	
	return id_string;
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

	gtranslator_id_free(new);
}

/*
 * Add the given GtrID to the list including checking if it's 
 *  already in the list.
 */
void gtranslator_id_add(GtrID *id)
{
	g_return_if_fail(GTR_ID(id)!=NULL);

	/*
	 * Add the given GtrID to the list but only is our search function
	 *  does show up that it isn't in the list yet.
	 */
	if(!gtranslator_id_search(GTR_ID(id)))
	{
		gtranslator_ids=g_list_prepend(gtranslator_ids, gtranslator_id_copy(id));
		gtranslator_ids=g_list_reverse(gtranslator_ids);
	}
}

/*
 * Remove the given GtrID from our list -- the elements should 
 *  be matching "enough" to apply the removal from the list.
 */
gboolean gtranslator_id_remove(GtrID *id)
{
	g_return_val_if_fail(GTR_ID(id)!=NULL, FALSE);

	if(gtranslator_id_search(id))
	{
		GList *zuper;
		
		zuper=gtranslator_ids;
		
		/*
		 * FIXME:
		 *
		 * This is a quite wrong "adaptance" from the history
		 *  routines to here; there's something surely wrong..
		 */
		while(zuper!=NULL)
		{
			if(gtranslator_id_equal(id, GTR_ID(zuper->data)))
			{
				GList *removeme=zuper;

				zuper=zuper->next;
				g_list_remove_link(gtranslator_ids, removeme);

				gtranslator_id_free(GTR_ID(removeme->data));
				g_list_free_1(removeme);
			}
			else
			{
				zuper=zuper->next;
			}
		}

		return FALSE;
	}
	else
	{
		return FALSE;
	}

	return FALSE;
}

/*
 * Simply search the given GtrID in our IDs list.
 */
gboolean gtranslator_id_search(GtrID *id)
{
	GList 	*checklist=NULL;
	
	g_return_val_if_fail(GTR_ID(id)!=NULL, FALSE);

	/*
	 * If there's no IDs list yet, return FALSE.
	 */
	if(!gtranslator_ids)
	{
		return FALSE;
	}

	checklist=g_list_copy(gtranslator_ids);

	while(checklist!=NULL)
	{
		GtrID 	*check_id=GTR_ID(checklist->data);

		/*
		 * Check if the two GtrIDs are equal -- this should
		 *  be enough.
		 */
		if(gtranslator_id_equal(id, check_id))
		{
			return TRUE;
		}
		
		checklist=checklist->next;
		gtranslator_id_free(GTR_ID(checklist->prev->data));
	}

	g_list_free(checklist);

	return FALSE;
}

/*
 * Load our GtrIDs encoded in string form from the preferences.
 */
void gtranslator_id_load_list()
{
	gchar	*path;
	gint	count, c;

	gtranslator_config_init();
	count=gtranslator_config_get_int("id/length");
	
	if(count > MAX_ID_LIST_LENGTH)
	{
		count=MAX_ID_LIST_LENGTH;
	}

	/*
	 * Read our stored plain-string encoded list of GtrIDs.
	 */
	for(c=0; c < count; c++)
	{
		GtrID	*id=NULL;
		gchar	*content;

		path=g_strdup_printf("id%d/id_string", c);
		content=gtranslator_config_get_string(path);
		g_free(path);
		
		g_return_if_fail(content!=NULL);
		id=gtranslator_id_new_from_string(content);

		gtranslator_ids=g_list_prepend(gtranslator_ids,
			gtranslator_id_copy(id));
		
		gtranslator_id_free(id);
	}

	gtranslator_ids=g_list_reverse(gtranslator_ids);

	gtranslator_config_close();
}

/*
 * Save our GtrIDs list to the preferences. 
 */
void gtranslator_id_save_list()
{
	if(gtranslator_ids)
	{
		GtrID 	*id;
		gchar	*path;
		gchar	*plain_string;
		
		gint	c=0;

		GList	*writelist=g_list_first(gtranslator_ids);

		gtranslator_config_init();

		/*
		 * Write out all GtrIDs in our internal list in string
		 *  form into the preferences.
		 */
		while(writelist!=NULL)
		{
			id=GTR_ID(writelist->data);

			path=g_strdup_printf("id%d/id_string", c);
			plain_string=gtranslator_id_string_from_id(id);
			
			gtranslator_config_set_string(path, plain_string);
			g_free(path);
			
			c++;

			if(c >= MAX_ID_LIST_LENGTH)
			{
				break;
			}
			
			writelist=g_list_next(writelist);
		}

		gtranslator_config_set_int("id/length", c);
		gtranslator_config_close();
	}
}

/*
 * A convenience copy function for our GtrIDs.
 */
GtrID *gtranslator_id_copy(GtrID *id)
{
	GtrID 	*copy=g_new0(GtrID, 1);
	
	g_return_val_if_fail(GTR_ID(id)!=NULL, NULL);

	copy->po_file=g_strdup(GTR_ID(id)->po_file);
	
	/*
	 * Copy the string parts safely or set'em to NULL where needed.
	 */
	IF_STRDUP(copy->po_language, GTR_ID(id)->po_language);
	IF_STRDUP(copy->po_version, GTR_ID(id)->po_version);
	IF_STRDUP(copy->po_date, GTR_ID(id)->po_date);
	
	copy->po_position=GTR_ID(id)->po_position;

	return copy;
}

/*
 * Free the given GtrID.
 */
void gtranslator_id_free(GtrID *id)
{
	if(GTR_ID(id))
	{
		g_free(GTR_ID(id)->po_file);
		g_free(GTR_ID(id)->po_language);
		g_free(GTR_ID(id)->po_version);
		g_free(GTR_ID(id)->po_date);
	}
}
