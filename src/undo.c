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
#include "dialogs.h"
#include "gui.h"
#include "parse.h"
#include "undo.h"

/*
 * Internally used variables/structures -- there shouldn't be any need
 *  to "declare" these items publically.
 */

static GList 	*undolist=NULL;

typedef enum
{
	GTR_UNDO_VIEW_CHANGE,
	GTR_UNDO_REPLACES,
	GTR_UNDO_REPLACE,
	GTR_UNDO_INSERT_TEXT,
	GTR_UNDO_DELETE_TEXT,
	GTR_UNDO_TYPE_CHANGE,
	GTR_UNDO_NONE
} GtrUndoType;

/*
 * Return if there are any undo items remaining.
 */
gboolean gtranslator_undo_is_any_item_remaining()
{
	if(undolist && (g_list_length(undolist) >= 1))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
