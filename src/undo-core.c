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

#include "undo-core.h"

/*
 * Creates the new GtrUndoCore structure.
 */
GtrUndoCore *gtranslator_undo_core_new(GFunc function, GFunc reverse_function, 
	const gchar *name, gconstpointer once, gconstpointer twice,
	gconstpointer trice)
{
	GtrUndoCore *core=g_new0(GtrUndoCore, 1);
	
	g_return_val_if_fail(function!=NULL, NULL);
	g_return_val_if_fail(name!=NULL, NULL);

	core->name=g_strdup(name);
	(GFunc )core->function=(GFunc )function;
	(GFunc )core->reverse_function=(GFunc )reverse_function;

	/*
	 * Put up the memory values -- can this be good?
	 */
	core->once=g_memdup(once, sizeof(once));
	core->twice=g_memdup(twice, sizeof(twice));
	core->trice=g_memdup(trice, sizeof(trice));

	return core;
}

/*
 * Free the given GtrUndoCore structure.
 */
void gtranslator_undo_core_free(GtrUndoCore **core)
{
	if(*core)
	{
		#define if_free(x) \
			if(GTR_UNDO_CORE(*core)->x) \
			{ \
				g_free(GTR_UNDO_CORE(*core)->x); \
			}

		if_free(name);
		if_free(once);
		if_free(twice);
		if_free(trice);
		
		#undef if_free
	}
}
