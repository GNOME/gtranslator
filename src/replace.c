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

#include "replace.h"
#include "find.h"
#include "gui.h"
#include "parse.h"
#include "prefs.h"

#include <string.h>

#include <libgnomeui/libgnomeui.h>

/*
 * Do the replace task for the given data block.
 */
static void replace_msg(gpointer data, gpointer replace);

/*
 * And this is the core function for the replace task.
 */
static void replace_core(gchar **string, GtrReplace *rstuff);

/*
 * Count the replaces.
 */
static gint replaced_count=0;

/*
 * Create a new GtrReplace object.
 */
GtrReplace *gtranslator_replace_new(const gchar *find, const gchar *replace,
	gboolean do_it_for_all)
{
	GtrReplace *newreplace=g_new0(GtrReplace, 1);

	g_return_val_if_fail(find!=NULL, NULL);
	g_return_val_if_fail(replace!=NULL, NULL);
	
	/*
	 * g_strdup the string informations for the new structure.
	 */
	newreplace->string=g_strdup(find);
	newreplace->replace_string=g_strdup(replace);

	if(do_it_for_all)
	{
		newreplace->replace_all=TRUE;
	}
	else
	{
		newreplace->replace_all=FALSE;
	}

	if(newreplace)
	{
		return newreplace;
	}
	else
	{
		return NULL;
	}
}

/*
 * Free up the GtrReplace structure.
 */
void gtranslator_replace_free(GtrReplace **replace)
{
	g_return_if_fail(*replace!=NULL);

	g_free((*replace)->string);
	g_free((*replace)->replace_string);
	g_free(*replace);
}

/*
 * Run the given replace action with the arguments of the
 *  structure.
 */
void gtranslator_replace_run(GtrReplace *replace)
{
	g_return_if_fail(replace!=NULL);

	replaced_count=0;
	
	/*
	 * Don't replace anything when there's no file open or if there's
	 *  no messages list (for whatever reason).
	 */
	if((!file_opened) || (!po->messages))
	{
		return;
	}
	else
	{
		g_list_foreach(po->messages, (GFunc) replace_msg, replace);
	}

	if(replaced_count >= 1)
	{
		enable_actions(ACT_SAVE);
	}
	else
	{
		gnome_app_warning(GNOME_APP(app1), _("No replace made!"));
	}
}

/*
 * Replace in the given message 
 */
static void replace_msg(gpointer data, gpointer replace)
{
	GtrMsg *msg=GTR_MSG(data);
	GtrReplace *l_replace=GTR_REPLACE(replace);

	g_return_if_fail(msg!=NULL);
	g_return_if_fail(l_replace!=NULL);

	/*
	 * Perform the replace actions according to the given action class.
	 */
	switch(wants.find_in)
	{
		case 1:
			replace_core(&msg->msgid, l_replace);
			
			break;

		case 2:
			replace_core(&msg->msgstr, l_replace);
			
			break;

		case 3:
			replace_core(&msg->msgid, l_replace);
			replace_core(&msg->msgstr, l_replace);
			
			break;

		case 4:
			replace_core(&msg->comment, l_replace);

			break;

		case 7:
			replace_core(&msg->msgid, l_replace);
			replace_core(&msg->msgstr, l_replace);
			replace_core(&msg->comment, l_replace);

			break;
	}
}

/*
 * Core kabalak land -- crazy method I know.
 */
static void replace_core(gchar **string, GtrReplace *rstuff)
{
	/*
	 * If any important data is missing, exit from here and don't perform
	 *  any action.
	 */
	if(!(*string) || (!rstuff) ||
	   (!rstuff->string) || (!rstuff->replace_string))
	{
		return;
	}
	else
	{
		/*
		 * Test if the string to replace is even in the original
		 *  string or if we'd do only one replace and we're done with
		 *   it already.
		 */
		if((rstuff->replace_all==FALSE && replaced_count >= 1))
		{
			return;
		}
		else
		{
			if(strstr(*string, rstuff->string))
			{
				gchar **arr;
				GString *yummystring=g_string_new("");
				gint z=0;

				/*
				 * FIXME --> This does not work as the g_strsplit
				 *  function doesn't work as wished by me .-(
				 */
				
				/*
				 * Split the string up for all the replace actions.
				 */
				arr=g_strsplit((*string), rstuff->replace_string, 0);
	
				/*
				 * Test the array elements and use them for building
				 *  up the replacement string.
				 */
				while(arr[z]!=NULL)
				{
					yummystring=g_string_append(yummystring, 
						arr[z]);
	
					yummystring=g_string_append(yummystring,
						rstuff->replace_string);
	
					z++;
				}
	
				g_strfreev(arr);
	
				/*
				 * If we could get a replacement string, free up the
				 *  old content of "*string" and assign the new, fresh
				 *   content of yummystring->str.
				 */
				if(yummystring->len > 0)
				{
					g_free(*string);
					*string=yummystring->str;
				}
				
				/*
				 * Hope we'd make a replace, therefore we do increment
				 *  the replaced_count.
				 */
				replaced_count++;
			}
		}
	}
}
