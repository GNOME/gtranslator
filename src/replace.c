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
static void replace_core(gchar **string, gchar *old_string, gchar *new_string);

/*
 * Did any replace succeed? This variable should be responsible for this duty.
 */
static gboolean replaced_anything=FALSE;

/*
 * Create a new GtrReplace object.
 */
GtrReplace *gtranslator_replace_new(const gchar *find, const gchar *replace)
{
	GtrReplace *newreplace=g_new0(GtrReplace, 1);

	g_return_val_if_fail(find!=NULL, NULL);
	g_return_val_if_fail(replace!=NULL, NULL);
	
	/*
	 * Strdup the string informations for out structure.
	 */
	newreplace->string=g_strdup(find);
	newreplace->replace_string=g_strdup(replace);

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

	if(replaced_anything)
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
		case 0:
			replace_core(&msg->msgid, l_replace->string,
				l_replace->replace_string);
			break;

		case 1:
			replace_core(&msg->msgstr, l_replace->string,
				l_replace->replace_string);
			break;

		case 2:
			replace_core(&msg->msgid, l_replace->string,
				l_replace->replace_string);
			
			replace_core(&msg->msgstr, l_replace->string,
				l_replace->replace_string);
			break;
				
	}
}

/*
 * Core kabalak land -- crazy method I know.
 */
static void replace_core(gchar **string, gchar *old_string, gchar *new_string)
{
	/*
	 * If any important data is missing, exit from here and don't perform
	 *  any action.
	 */
	if(!(*string) || (!old_string) || (!new_string))
	{
		return;
	}
	else
	{
		/*
		 * Test if the string to replace is even in the original string.
		 */
		if(!strstr(*string, old_string))
		{
			return;
		}
		else
		{
			g_warning("Replace here!!");
		}
	}
}
