/*
 * (C) 2001-2003 	Fatih Demir <kabalak@gtranslator.org>
 *			Kevin Vandersloot <kfv101@psu.edu>
 *			Peeter Vois <peeter@gtranslator.org>
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
#include "messages.h"

enum
{
	COL_NUMBER,
	COL_LINE,
	COL_ORIGINAL,
	COL_TRANSLATION,
	COL_COMMENT,
	COL_STATUS,
	COL_BOLD,
	COL_COLOR,
};

/*
 * Returns the ready widget with all the specs.
 */
GtkWidget *gtranslator_messages_table_new(void);

/*
 * Clear the table
 */
void gtranslator_messages_table_clear(void);
 
/*
 * Populate table with the messages
 */
void gtranslator_messages_table_create(void);

/*
 * Update the data in a single row
 */
void gtranslator_messages_table_update_row(GtrMsg *message); 

/*
 * Select given message
 */
void gtranslator_messages_table_select_row(GtrMsg *message);

/*
 * Update the status grouping of a message
 */
void gtranslator_messages_table_update_message_status(GtrMsg *message);

#endif
