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

/*
 * Create a new GtrReplace object.
 */
GtrReplace *gtranslator_replace_new(const gchar *find, const gchar *replace,
	gboolean replace_all)
{
	GtrReplace *newreplace=g_new0(GtrReplace, 1);

	g_return_val_if_fail(find!=NULL, NULL);
	g_return_val_if_fail(replace!=NULL, NULL);
	
	/*
	 * Strdup the string informations for out structure.
	 */
	newreplace->string=g_strdup(find);
	newreplace->replace_string=g_strdup(replace);

	if(replace_all)
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

	if(!replace->replace_all)
	{
		g_warning("WRITE CODE FOR REPLACE A SINGLE STRING HERE: replace.c +86");
	}
	else
	{
		g_warning("WRITE CODE FOR REPLACING ALL STRINGS HERE: replace.c +90");
	}
}
