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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "actions.h"
#include "comment.h"
#include "dialogs.h"
#include "gui.h"
#include "parse.h"
#include "prefs.h"
#include "syntax.h"
#include "undo.h"
#include "utils.h"

#include <gtk/gtkeditable.h>
#include <gtk/gtktext.h>

/*
 * Internal undo types -- no, please no questions why so.
 */
typedef struct
{
	gchar	*text;
	gint	 position;
	gboolean insertion;
} GtrUndo;

GtrUndo *undo=NULL;

/*
 * Register the given text for an insertion step.
 */
void gtranslator_undo_register_insertion(const gchar *text, const gint position)
{
	g_return_if_fail(position!=-1);
	g_return_if_fail(text!=NULL);

	if(undo)
	{
		GTR_FREE(undo->text);
	}
	
	GTR_FREE(undo);

	undo=g_new0(GtrUndo, 1);

	undo->text=g_strdup(text);
	undo->position=position;
	undo->insertion=TRUE;

	gtranslator_actions_enable(ACT_UNDO);
}

/*
 * Do the same for the deletion step.
 */
void gtranslator_undo_register_deletion(const gchar *text, const gint position)
{
	g_return_if_fail(position!=-1);

	if(!text)
	{
		return;
	}

	if(undo)
	{
		GTR_FREE(undo->text);
	}
	
	GTR_FREE(undo);

	undo=g_new0(GtrUndo, 1);

	undo->text=g_strdup(text);
	undo->position=position;
	undo->insertion=FALSE;

	gtranslator_actions_enable(ACT_UNDO);
}

/*
 * Return whether any undo is registered.
 */
gboolean gtranslator_undo_get_if_registered_undo()
{
	if(undo && undo->text)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*
 * Reset the mainly used GtrUndo.
 */
void gtranslator_undo_clean_register()
{
	if(undo)
	{
		GTR_FREE(undo->text);
	}

	GTR_FREE(undo);
}

/*
 * Undo the last action -- calls the undo stuff with the undo parameters 
 *  for doing the undo -- Undo!
 */
void gtranslator_undo_run_undo()
{
	g_return_if_fail(undo!=NULL);
	g_return_if_fail(undo->position >= 0);
	g_return_if_fail(undo->text!=NULL);

	/*
	 * Is the previous undoable action was a deletion act _this_ way.
	 */
	if(!undo->insertion)
	{
		/*
		 * Check if the undo position is still appliable.
		 */
		if(undo->position <= gtk_text_get_length(GTK_TEXT(trans_box)))
		{
			gtk_editable_insert_text(GTK_EDITABLE(trans_box),
				undo->text, strlen(undo->text), &undo->position);
		}
	}
	else
	{
		/*
		 * Again, check for sanity.
		 */
		if(undo->position <= gtk_text_get_length(GTK_TEXT(trans_box)))
		{
			gtk_editable_delete_text(GTK_EDITABLE(trans_box),
				undo->position, 
				(undo->position + strlen(undo->text)));
		}
	}

	/*
	 * Now update the syntax highlighting.
	 */
	gtranslator_syntax_update_text(trans_box);
}
