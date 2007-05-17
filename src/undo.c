/*
 * (C) 2001-2007 	Fatih Demir <kabalak@kabalak.net>
 *			Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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

#include <string.h>

#include "actions.h"
#include "comment.h"
#include "dialogs.h"
#include "menus.h"
#include "page.h"
#include "prefs.h"
#include "undo.h"
#include "utils.h"

#include <gtk/gtk.h>

/*
 * Internal undo types -- no, please no questions why so.
 */
typedef struct
{
	gchar *text;
	GtkTextIter *position;
	GtkTextIter *endposition;
	gboolean insertion;
} GtrUndo;

GtrUndo *undo=NULL;

/*
 * Register the given text for an insertion step.
 */
void gtranslator_undo_register_insertion(const gchar *text, const GtkTextIter *position)
{
	g_return_if_fail(position!=NULL);
	g_return_if_fail(text!=NULL);

	if(undo)
	{
		g_free(undo->text);
		g_free(undo->position);
	}
	
	g_free(undo);

	undo=g_new0(GtrUndo, 1);

	undo->text=g_strdup(text);
	undo->insertion=TRUE;
        memcpy(undo->position, position, sizeof(undo->position));

	gtk_widget_set_sensitive(gtranslator_menuitems->undo, TRUE);
}

/*
 * Do the same for the deletion step.
 */
void gtranslator_undo_register_deletion(const gchar *text, const GtkTextIter *position, const GtkTextIter *endposition)
{
	g_return_if_fail(position!=NULL);
	g_return_if_fail(text!=NULL);

	if(undo)
	{
		g_free(undo->text);
		g_free(undo->position);
		g_free(undo->endposition);
	}
	
	g_free(undo);

	undo=g_new0(GtrUndo, 1);

	undo->text=g_strdup(text);
	undo->insertion=FALSE;
        memcpy(undo->position, position, sizeof(undo->position));
        memcpy(undo->endposition, endposition, sizeof(undo->endposition));

	gtk_widget_set_sensitive(gtranslator_menuitems->undo, TRUE);
}

/*
 * Return whether any undo is registered.
 */
gboolean gtranslator_undo_get_if_registered_undo()
{
	return (undo && undo->text);
}

/*
 * Reset the mainly used GtrUndo.
 */
void gtranslator_undo_clean_register()
{
	if(undo)
	{
		g_free(undo->text);
	}

	g_free(undo);
}

/*
 * Undo the last action -- calls the undo stuff with the undo parameters 
 *  for doing the undo -- Undo!
 */
void gtranslator_undo_run_undo()
{
	GtkTextBuffer *buffer;
	GtkTextIter *end = NULL;

	g_return_if_fail(undo!=NULL);
	g_return_if_fail(undo->position >= 0);
	g_return_if_fail(undo->text!=NULL);

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(current_page->trans_msgstr));
	gtk_text_buffer_get_end_iter(buffer, end);

	/*
	 * Is the previous undoable action was a deletion act _this_ way
	 * (with sanity check).
	 */
	if(!undo->insertion && gtk_text_iter_compare(undo->position, end) > 0)
	{
		gtk_text_buffer_insert(buffer, undo->position, undo->text, strlen(undo->text));
	}
	else
	{
		gtk_text_buffer_delete(buffer, undo->position, undo->endposition);
	}
}
