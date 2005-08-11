/*
 * (C) 2001-2003 	Fatih Demir <kabalak@kabalak.net>
 *			Kevin Vandersloot <kfv101@psu.edu>
 *			Peeter Vois <peeter@kabalak.net>
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

#ifndef GTR_MESSAGES_TABLE_H
#define GTR_MESSAGES_TABLE_H 1

#include <gtk/gtkwidget.h>
#include <gtk/gtktreestore.h>
#include "message.h"

/*
 * Attributes of the messages table
 */
typedef struct {
	GtkWidget *widget;
	GtkTreeStore *store;
	GtkTreeIter untranslated_node;
	GtkTreeIter fuzzy_node;
	GtkTreeIter translated_node;
} GtrMessagesTable;


/*
 * Returns the ready widget with all the specs.
 */
GtrMessagesTable *gtranslator_messages_table_new();

/*
 * Populate a messages table from a given po
 */
void gtranslator_messages_table_populate(GtrMessagesTable *table, GList *messages);

/*
 * Update the data in a single row
 */
void gtranslator_messages_table_update_row(GtrMessagesTable *table, GtrMsg *message); 

/*
 * Select given message
 */
void gtranslator_messages_table_select_row(GtrMessagesTable *table, GtrMsg *message);

/*
 * Frees resources allocated to this messages table
 */
void gtranslator_messages_table_free(GtrMessagesTable *table);
 
#endif
