/*
 * (C) 2003 	Fatih Demir <kabalak@kabalak.net>
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

#include "actions.h"
#include "comment.h"
#include "find.h"
#include "gui.h"
#include "message.h"
#include "nautilus-string.h"
#include "parse.h"
#include "prefs.h"
#include "replace.h"
#include "utils.h"

#include <libgnomeui/libgnomeui.h>

/*
 * Do the replace task for the given data block.
 */
static void replace_msg(gpointer data, gpointer replace);

#ifdef DONTFORGET
/*
 * And this is the core function for the replace task.
 */
static void replace_core(gchar **string, GtrReplace *rstuff);
#endif

/*
 * Count the replaces.
 */
static gint replaced_count=0;

/*
 * Create a new GtrReplace object.
 */
GtrReplace *gtranslator_replace_new(const gchar *find, const gchar *replace,
	gboolean do_it_for_all, gint start, gboolean replace_in_comments,
	gboolean replace_in_english, gboolean replace_in_translation)
{
	GtrReplace *newreplace=g_new0(GtrReplace, 1);

	g_return_val_if_fail(find!=NULL, NULL);
	g_return_val_if_fail(replace!=NULL, NULL);
	g_return_val_if_fail(start >= 0, NULL);
	
	/*
	 * g_strdup the string informations for the new structure.
	 */
	newreplace->string=g_strdup(find);
	newreplace->replace_string=g_strdup(replace);
	newreplace->start_offset=start;

	newreplace->replace_all=do_it_for_all;

	newreplace->replace_in_comments=replace_in_comments;
	newreplace->replace_in_english=replace_in_english;
	newreplace->replace_in_translation=replace_in_translation;

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
	g_return_if_fail(po!=NULL);
	g_return_if_fail(po->messages!=NULL);

	replaced_count=0;
	
	/* A great opportunity to update po->current->data */
	gtranslator_message_update();
	if(replace->replace_all)
	{
		/*
		 * Perform the replace actions for all messages from the first
		 *  till the last message.
		 */
		g_list_foreach(po->messages, (GFunc) replace_msg, replace);
	}
	else
	{
		GList *theoriginalchoice=NULL;

		/*
		 * Rescue the "current" po->current pointer.
		 */
		theoriginalchoice=po->current;
			
		/*
		 * Replace till we did succeed in doing a replace, then exit this.
		 */
		while((po->current->next) && (replaced_count <= 0))
		{
			g_list_foreach(po->current, (GFunc) replace_msg, replace);
			po->current=po->current->next;
		}
			
		/*
		 * Now we do go back to the status we had before the replace action;
		 *  the po->current pointer is right now.
		 */
		po->current=theoriginalchoice;
	}
	
	/*
	 * Redisplay the current message to get replaces in the current message
	 *  on the screen immediately.
	 */
	gtranslator_message_show(po->current->data);

	if(replaced_count >= 1)
	{
		/*
		 * Enable the save routines and set the changed
		 *  new status of the po file.
		 */
		po->file_changed=TRUE;
		
		gtranslator_actions_enable(ACT_SAVE);
	}
	else
	{
		gnome_app_warning(GNOME_APP(gtranslator_application), _("No replacements made!"));
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
	g_return_if_fail(msg->message!=NULL);
	g_return_if_fail(l_replace!=NULL);


#ifdef DONTFORGET
	/*
	 * Perform the replace actions according to the given action class.
	 */
	if(l_replace->replace_in_english)
	{
		replace_core(&msg->message->msgid, l_replace);
	}

	if(l_replace->replace_in_translation)
	{
		replace_core(&msg->message->msgstr, l_replace);
	}
#endif
}

#ifdef DONTFORGET
/*
 * Core kabalak land -- crazy method I know.
 */
static void replace_core(char **string, GtrReplace *rstuff)
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
				gchar *nstring;
				
				/*
				 * Perform the replace via the nautilus string
				 *  function.
				 */
				nstring=nautilus_str_replace_substring(*string,
					rstuff->string, rstuff->replace_string);

				if(nstring)
				{
					/*
					 * Set the original string to the new form if
					 *  the replace was successful.
					 */
					g_free(*string);
					*string=nstring;
				}
				
				/*
				 * Hope we'd make a replace, therefore we do 
				 *  increment the replaced_count.
				 */
				replaced_count++;
			}
		}
	}
}
#endif
