/*
 * (C) 2001-2003 	Fatih Demir <kabalak@gtranslator.org>
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

#include "bookmark.h"
#include "message.h"
#include "nautilus-string.h"
#include "open.h"
#include "parse.h"
#include "preferences.h"
#include "utils.h"

/*
 * For the moment 10 bookmarks should be the upper limit.
 */
#define MAX_BOOKMARKS 10

/*
 * The used GList for the GtrBookmark's -- a general way to handle with this
 *  list is supplied by the gtranslator_bookmark_* methods.
 */
GList *gtranslator_bookmarks=NULL;

/*
 * Create and return a GtrBookmark from the current position & po file -- 
 *  if a file is opened yet.
 */
GtrBookmark *gtranslator_bookmark_new()
{
	GtrBookmark *bookmark=g_new0(GtrBookmark, 1);

	g_return_val_if_fail(file_opened==TRUE, NULL);
	g_return_val_if_fail(po->filename!=NULL, NULL);

	bookmark->file=g_strdup(po->filename);
	bookmark->version=g_strdup(po->header->prj_version);
	
	bookmark->position=g_list_position(po->messages, po->current);

	if(bookmark->position < 0)
	{
		bookmark->position=0;
	}

	return bookmark;
}

/*
 * Creates the bookmark from a given gchar-string formatted by gtranslator.
 */
GtrBookmark *gtranslator_bookmark_new_from_string(const gchar *string)
{
	GtrBookmark 	*bookmark=g_new0(GtrBookmark, 1);
	gchar 		**values;
	gchar 		*filename, *encoding_area;

	g_return_val_if_fail(string!=NULL, NULL);

	/*
	 * Check if the given string is a GtrBookmark "URI" at all.
	 */
	if(!nautilus_istr_has_prefix(string, "gtranslator_bookmark:") ||
		!strchr(string, '/') || !strchr(string, '#'))
	{
		return NULL;
	}
	else
	{
		/*
		 * Strip of the file name from the GtrBookmark.
		 */
		gchar *tempzulu;
		
		tempzulu=nautilus_str_get_prefix(string, "#");
		filename=nautilus_str_get_after_prefix(tempzulu, ":");
		filename++;
		
		g_free(tempzulu);
		g_return_val_if_fail(filename!=NULL, NULL);
	}
	
	bookmark->file=g_strdup(filename);
	/*
	 * FIXME: Doesn't work due to the filename++ above, fix needed!
	 * 
	 * g_free(filename);
	 */
	
	/*
	 * Operate on the resting parts of the string-encoded bookmark and
	 *  split it up into its normally 4 parts.
	 */
	encoding_area=nautilus_str_get_after_prefix(string, "#");
	encoding_area++;

	values=g_strsplit(encoding_area, "/", 2);

	/*
	 * FIXME: Again the str++ problem like above with filename++, aaaarg!
	 * 
	 * g_free(encoding_area);
	 */

	bookmark->version = g_strdup(values[0]);
	
	/*
	 * Always be quite safe about the GtrBookmark values assigned here.
	 */
	if(values[1])
	{
		nautilus_str_to_int(values[1], 
			&(bookmark->position));
	}
	else
	{
		bookmark->position=-1;
	}

	g_strfreev(values);
	return bookmark;
}

/*
 * The otherway conversion: convert a GtrBookmark into a plain string.
 */
gchar *gtranslator_bookmark_string_from_bookmark(GtrBookmark *bookmark)
{
	gchar *string;

	g_return_val_if_fail(bookmark!=NULL, NULL);

	string=g_strdup_printf("gtranslator_bookmark:%s#%s/%i",
		bookmark->file, bookmark->version, bookmark->position);

	return string;
}

/*
 * Encapsulate the calls for creating a new GtrBookmark string.
 */
gchar *gtranslator_bookmark_new_bookmark_string()
{
	GtrBookmark *bookmark;
	gchar *bookmark_string;

	bookmark=gtranslator_bookmark_new();

	g_return_val_if_fail(bookmark!=NULL, NULL);
	
	bookmark_string=gtranslator_bookmark_string_from_bookmark(bookmark);
	
	gtranslator_bookmark_free(bookmark);
	
	return bookmark_string;
}

/*
 * Open the given bookmark.
 */
void gtranslator_bookmark_open(GtrBookmark *bookmark)
{
	g_return_if_fail(bookmark!=NULL);
	g_return_if_fail(bookmark->file!=NULL);

	/*
	 * Open the po file.
	 */
	gtranslator_open_file(bookmark->file);

	/*
	 * Only re-setup the bookmark if the po file could be opened.
	 */
	if(po && bookmark->position!=-1 &&
	   po->length >= bookmark->position)
	{
		gtranslator_message_go_to_no(NULL, 
			GINT_TO_POINTER(bookmark->position));
	}
}

/*
 * Is the given GtrBookmark resolvable at all -- is the file present and do the
 *  other specs also match?!
 */
gboolean gtranslator_bookmark_resolvable(GtrBookmark *bookmark)
{
	GtrPo *current_po;
	
	g_return_val_if_fail(bookmark!=NULL, FALSE);

	/*
	 * First check if there's a file with that name.
	 */
	if(!g_file_test(bookmark->file, G_FILE_TEST_EXISTS))
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
				
		gtranslator_parse_main(bookmark->file);

		/*
		 * Check the header parts for equality.
		 */
		CHECK_HEADER_PART(prj_version);
		
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
		 * At least 2 equalities must have been occured to let the
		 *  GtrBookmark be resolvable.
		 */
		if(potcom >= 2)
		{
			/*
			 * Free the "new-old" po and reassign the saved
			 *  original po variable.
			 */
			gtranslator_po_free(po);
			po=current_po;

			return TRUE;
		}
	}

	return FALSE;
}

/*
 * Return whether the two GtrBookmark's are equal.
 */
gboolean gtranslator_bookmark_equal(GtrBookmark *one, GtrBookmark *two)
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
			if(!nautilus_strcasecmp(GTR_BOOKMARK(one)->x, GTR_BOOKMARK(two)->x)) \
			{ \
				potcom++; \
			}

		/*
		 * Check the single string parts for common values and increase
		 *  the potcom variable in this case.
		 */
		CHECK_CASE(file);
		CHECK_CASE(version);

		/*
		 * The position is checked here extra as it's not a string .-)
		 */
		if(GTR_BOOKMARK(one)->position==GTR_BOOKMARK(two)->position)
		{
			potcom++;
		}

		#undef CHECK_CASE

		/*
		 * Have we got more then/equal to 2 common values? 
		 *  Then these bookmarks are equal.
		 */
		if(potcom >= 2)
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
 * Smally different: compare the GtrBookmark and the bookmark string.
 */
gboolean gtranslator_bookmark_string_equal(GtrBookmark *bookmark, const gchar *string)
{
	GtrBookmark *new=gtranslator_bookmark_new_from_string(string);
	gboolean equal=gtranslator_bookmark_equal(bookmark, new);
	gtranslator_bookmark_free(new);
	return equal;
}

/*
 * Add the given GtrBookmark to the list including checking if it's 
 *  already in the list.
 */
void gtranslator_bookmark_add(GtrBookmark *bookmark)
{
	g_return_if_fail(bookmark!=NULL);

	/*
	 * Add the given GtrBookmark to the list but only is our search function
	 *  does show up that it isn't in the list yet.
	 */
	if(!gtranslator_bookmark_search(bookmark))
	{
		gtranslator_bookmarks=g_list_append(gtranslator_bookmarks, gtranslator_bookmark_copy(bookmark));
	}
}

/*
 * Adding the bookmark directly without any feedback and such but easyness.
 */
void gtranslator_bookmark_add_direct()
{
	GtrBookmark *bookmark=NULL;

	bookmark=gtranslator_bookmark_new();
	g_return_if_fail(bookmark!=NULL);

	gtranslator_bookmark_add(GTR_BOOKMARK(bookmark));
	gtranslator_bookmark_free(bookmark);
}

/*
 * Remove the given GtrBookmark from our list -- the elements should 
 *  be matching "enough" to apply the removal from the list.
 */
gboolean gtranslator_bookmark_remove(GtrBookmark *bookmark)
{
	g_return_val_if_fail(bookmark!=NULL, FALSE);

	if(gtranslator_bookmark_search(bookmark))
	{
		GList *zuper;
		
		zuper=gtranslator_bookmarks;
		
		/*
		 * FIXME:
		 *
		 * This is a quite wrong "adaptance" from the history
		 *  routines to here; there's something surely wrong..
		 */
		while(zuper!=NULL)
		{
			if(gtranslator_bookmark_equal(bookmark, GTR_BOOKMARK(zuper->data)))
			{
				GList *removeme=zuper;

				GTR_ITER(zuper);
				g_list_remove_link(gtranslator_bookmarks, removeme);

				gtranslator_bookmark_free(GTR_BOOKMARK(removeme->data));
				g_list_free_1(removeme);
			}
			else
			{
				GTR_ITER(zuper);
			}
		}
	}

	return FALSE;
}

/*
 * Simply search the given GtrBookmark in our IDs list.
 */
gboolean gtranslator_bookmark_search(GtrBookmark *bookmark)
{
	GList 	*checklist=NULL;
	
	g_return_val_if_fail(bookmark!=NULL, FALSE);

	/*
	 * If there's no IDs list yet, return FALSE.
	 */
	if(!gtranslator_bookmarks)
	{
		return FALSE;
	}

	checklist=g_list_copy(gtranslator_bookmarks);

	while(checklist!=NULL)
	{
		GtrBookmark 	*check_bookmark=GTR_BOOKMARK(checklist->data);

		/*
		 * Check if the two GtrBookmarks are equal -- this should
		 *  be enough.
		 */
		if(gtranslator_bookmark_equal(bookmark, check_bookmark))
		{
			return TRUE;
		}
		
		GTR_ITER(checklist);
		gtranslator_bookmark_free(GTR_BOOKMARK(checklist->prev->data));
	}

	g_list_free(checklist);

	return FALSE;
}

/*
 * Load our GtrBookmarks encoded in string form from the preferences.
 */
void gtranslator_bookmark_load_list()
{
	gchar	*path;
	gint	count, c;

	count=gtranslator_config_get_int("bookmark/length");
	
	if(count > MAX_BOOKMARKS)
	{
		count=MAX_BOOKMARKS;
	}

	/*
	 * Read our stored plain-string encoded list of GtrBookmarks.
	 */
	for(c=0; c < count; c++)
	{
		GtrBookmark	*bookmark=NULL;
		gchar	*content;

		path=g_strdup_printf("bookmark%d/bookmark_string", c);
		content=gtranslator_config_get_string(path);
		g_free(path);
		
		g_return_if_fail(content!=NULL);
		bookmark=gtranslator_bookmark_new_from_string(content);

		gtranslator_bookmarks=g_list_prepend(gtranslator_bookmarks,
			gtranslator_bookmark_copy(bookmark));
		
		g_message("Bookmark #%i: File %s,\n\tVersion: %s, message %i\n",
				(c+1), bookmark->file, bookmark->version, bookmark->position);

		gtranslator_bookmark_free(bookmark);
	}

	gtranslator_bookmarks=g_list_reverse(gtranslator_bookmarks);
}

/*
 * Save our GtrBookmarks list to the preferences. 
 */
void gtranslator_bookmark_save_list()
{
	if(gtranslator_bookmarks)
	{
		GtrBookmark 	*bookmark;
		gchar	*path;
		gchar	*plain_string;
		
		gint	c=0;

		GList	*writelist=g_list_first(gtranslator_bookmarks);

		/*
		 * Write out all GtrBookmarks in our internal list in string
		 *  form into the preferences.
		 */
		while(writelist!=NULL)
		{
			bookmark=GTR_BOOKMARK(writelist->data);

			path=g_strdup_printf("bookmark%d/bookmark_string", c);
			plain_string=gtranslator_bookmark_string_from_bookmark(bookmark);
			
			gtranslator_config_set_string(path, plain_string);
			g_free(path);
			
			c++;

			if(c >= MAX_BOOKMARKS)
			{
				break;
			}
			
			GTR_ITER(writelist);
		}

		gtranslator_config_set_int("bookmark/length", c);
	}
}

/*
 * Return a copy of our internally used bookmarks' list -- or NULL.
 */
GList *gtranslator_bookmark_get_list()
{
	if(!gtranslator_bookmarks || g_list_length(gtranslator_bookmarks) <= 0)
	{
		return NULL;
	}
	else
	{
		return (g_list_copy(gtranslator_bookmarks));
	}
}

/*
 * A convenience copy function for our GtrBookmarks.
 */
GtrBookmark *gtranslator_bookmark_copy(GtrBookmark *bookmark)
{
	GtrBookmark 	*copy=g_new0(GtrBookmark, 1);
	
	g_return_val_if_fail(bookmark!=NULL, NULL);

	copy->file=g_strdup(bookmark->file);
	copy->version = g_strdup(bookmark->version);
	copy->position=bookmark->position;

	return copy;
}

/*
 * Free the given GtrBookmark.
 */
void gtranslator_bookmark_free(GtrBookmark *bookmark)
{
	if(bookmark)
	{
		g_free(bookmark->file);
		g_free(bookmark->version);
		g_free(bookmark);
	}
}
