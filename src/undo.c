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

#include "actions.h"
#include "dialogs.h"
#include "gui.h"
#include "parse.h"
#include "undo.h"

/*
 * The generally used lists for Redo/Undo.
 */
GList 	*undolist=NULL;
GList 	*redolist=NULL;

/*
 * The Undo elements count.
 */
gint 	actions=0;

/*
 * Free the given GtrUndo consistent list.
 */
void gtranslator_undo_free_list(GList *list);

/*
 * Creation/registering inclusive creation of undo/redo steps
 *  and their deletion.
 */
GtrUndo *gtranslator_undo_new(const gchar *description,
	const gchar *actionname, GFunc function, gpointer data);
void gtranslator_undo_free(GtrUndo **undo);

GtrUndo *gtranslator_undo_new_register(const gchar *description, 
	const gchar *actionname, GFunc function, gpointer data);
GtrUndo *gtranslator_redo_new_register(const gchar *description,
	const gchar *actionname, GFunc function, gpointer data);

/*
 * Registering functions for interactions with the lists.
 */
void gtranslator_undo_register(GtrUndo *undo);
void gtranslator_redo_register(GtrUndo *redo);


/*
 * Create a GtrUndo structure.
 */
GtrUndo *gtranslator_undo_new(const gchar *description, 
	const gchar *actionname, GFunc function, gpointer data)
{
	GtrUndo *undo=g_new0(GtrUndo, 1);

	g_return_val_if_fail(description!=NULL, NULL);

	/*
	 * Use the GtrUndoCore stuff for the core elements.
	 */
	GTR_UNDO_CORE(undo->core)=gtranslator_undo_core_new(
		actionname, function);
	
	undo->description=g_strdup(description);

	if(data)
	{
		gtranslator_undo_core_set_data(&GTR_UNDO_CORE(undo->core), data);
	}

	if(undo)
	{
		return undo;
	}
	else
	{
		return NULL;
	}
}

/*
 * Create & register the Undo/Redo step.
 */
GtrUndo *gtranslator_undo_new_register(const gchar *description, 
	const gchar *actionname, GFunc function, gpointer data)
{
	GtrUndo *undo=gtranslator_undo_new(description, actionname,
		function, data);
	
	if(undo)
	{
		gtranslator_undo_register(undo);
		return undo;
	}
	else
	{
		return NULL;
	}
}

GtrUndo *gtranslator_redo_new_register(const gchar *description,
	const gchar *actionname, GFunc function, gpointer data)
{
	GtrUndo *redo=gtranslator_undo_new(description, actionname,
		function, data);

	if(redo)
	{
		gtranslator_redo_register(redo);
		return redo;
	}
	else
	{
		return NULL;
	}
}

/*
 * Fully setup a GtrUndo without the need to declare/handle around
 *  with them at the calling time.
 */
void gtranslator_undo_add(const gchar *description,
	const gchar *actionname, GFunc function, gpointer data)
{
	GtrUndo *undo;

	undo=gtranslator_undo_new_register(description, actionname,
		function, data);

	g_return_if_fail(undo!=NULL);

	gtranslator_actions_enable(ACT_UNDO);
}

/*
 * And also give the same functionality for the redo steps.
 */
void gtranslator_redo_add(const gchar *description,
	const gchar *actionname, GFunc function, gpointer data)
{
	GtrUndo *redo;

	redo=gtranslator_redo_new_register(description, actionname,
		function, data);

	g_return_if_fail(redo!=NULL);
}

/*
 * Free up one GtrUndo structure.
 */
void gtranslator_undo_free(GtrUndo **undo)
{
	gtranslator_undo_core_free(&GTR_UNDO_CORE(GTR_UNDO(*undo)->core));
	g_free((GTR_UNDO(*undo)->description));
}

/*
 * Register the undo action within a context.
 */
void gtranslator_undo_register(GtrUndo *undo)
{
	g_return_if_fail(undo!=NULL);

	undolist=g_list_prepend(undolist, GTR_UNDO(undo));

	actions++;
}

/*
 * Register the GtrUndo as a _redo_ step for the given GtrUndo.
 */
void gtranslator_redo_register(GtrUndo *redo)
{
	g_return_if_fail(redo!=NULL);

	redolist=g_list_prepend(redolist, GTR_UNDO(redo));

	actions++;
}

/*
 * All the GtrUndo actions are kept right here.
 */
void gtranslator_undo_get_undo_list()
{
	if(undolist)
	{
		/*
		 * Print out every element for now -- this should be a
		 *  menu in the nearer future.
		 */
		while(undolist->data)
		{
			g_message("Undo: %s>>", 
				GTR_UNDO(undolist->data)->description);

			undolist=undolist->next;

			if(!undolist)
			{
				return;
			}
		}
	}
}

/*
 * Also build up the Redo list via this function.
 */
void gtranslator_redo_get_redo_list()
{
	if(redolist)
	{
		while(redolist->data)
		{
			g_message("Redo: %s>>",
				GTR_UNDO(redolist->data)->description);

			redolist=redolist->next;

			if(!redolist)
			{
				return;
			}
		}
	}
}

/*
 * Free up the internally kept list of our GtrUndo datas.
 */
void gtranslator_undo_free_list(GList *list)
{
	if(list)
	{
		/*
		 * Free up every single element with our own functions
		 *  before setting the list to NULL.
		 */
		while(list->data)
		{
			if(GTR_UNDO(list->data))
			{
				gtranslator_undo_free(list->data);
				actions--;
			}
			
			list=list->next;
		}

		list=NULL;
	}
}

/*
 * Simply free both of the lists via the "gtranslator_undo_free_list" function.
 */
void gtranslator_undo_free_lists()
{
	gtranslator_undo_free_list(undolist);
	gtranslator_undo_free_list(redolist);
}

/*
 * Undo the last registered action.
 */
void gtranslator_undo()
{
	if(undolist)
	{
		GtrUndoCore *core;
		core=GTR_UNDO_CORE(GTR_UNDO(undolist->data)->core);

		if(core)
		{
			gtranslator_undo_core_run(&core, NULL);
			gtranslator_redo_add("Redo an Undo", 
				"redo_add", 
				(GFunc) core->function, core->data);
			
		}
	}
}

/*
 * Redo the last registered, undone action.
 */
void gtranslator_redo()
{
	if(redolist)
	{
		GtrUndoCore *core;
		core=GTR_UNDO_CORE(GTR_UNDO(redolist->data)->core);

		if(core)
		{
			gtranslator_undo_core_run(&core, NULL);
		}
	}
}
