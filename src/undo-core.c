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
GtrUndoCore *gtranslator_undo_core_new(const gchar *name, GFunc func)
{
	GtrUndoCore *core=g_new0(GtrUndoCore, 1);
	
	g_return_val_if_fail(name!=NULL, NULL);
	g_return_val_if_fail(func!=NULL, NULL);

	core->name=g_strdup(name);
	(GFunc) core->function=(GFunc) func;
	core->data=NULL;

	if(core)
	{
		return core;
	}
	else
	{
		return NULL;
	}
}

/*
 * Free the given GtrUndoCore structure.
 */
void gtranslator_undo_core_free(GtrUndoCore **core)
{
	if(*core)
	{
		g_free((*core)->name);
		
		if((*core)->data)
		{
			g_free((*core)->data);
		}
	}
}

/*
 * Set/get the internal data field of the GtrUndoCore.
 */
void gtranslator_undo_core_set_data(GtrUndoCore **core, gpointer data)
{
	g_return_if_fail(*core!=NULL);
	g_return_if_fail(data!=NULL);

	/*
	 * Free the text data and assign it or does purely assign it.
	 */
	if(GTR_UNDO_CORE(*core)->data)
	{
		g_free(GTR_UNDO_CORE(*core)->data);
		GTR_UNDO_CORE(*core)->data=data;
	}
	else
	{
		GTR_UNDO_CORE(*core)->data=data;
	}
}

gpointer gtranslator_undo_core_get_data(GtrUndoCore **core)
{
	g_return_val_if_fail(*core!=NULL, NULL);
	
	return (GTR_UNDO_CORE(*core)->data);
}

/*
 * Perform the real Undo/Redo step through the function & data.
 */
void gtranslator_undo_core_run(GtrUndoCore **core, gpointer additional_data)
{
	g_return_if_fail(*core!=NULL);
	
	if(GTR_UNDO_CORE(*core)->data)
	{
		if(additional_data)
		{
			GTR_UNDO_CORE(*core)->function(additional_data, GTR_UNDO_CORE(*core)->data);
		}
		else
		{
			GTR_UNDO_CORE(*core)->function(NULL, GTR_UNDO_CORE(*core)->data);
		}
	}
}
