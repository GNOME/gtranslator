/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
 *		Kevin Vandersloot <kfv101@psu.edu>
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
	COL_NUM,
	COL_ORIG,
	COL_TRANS,
	COL_COMMENT,
	COL_STATUS,
};



/*
 * Returns the ready widget with all the specs.
 */
GtkWidget *gtranslator_messages_table_new(void);
 
/*
 * Clear the table and populate with the messages
 */
void gtranslator_messages_table_create(void);

/*
 * Update the data in a single row
 */
void gtranslator_messages_table_update_row(GtrMsg *message); 

/*
 * Save the e-tree state
 */
void gtranslator_messages_table_save_state(void);

/*
 * Show/hide and toggle routines accordingly to the 
 *  existing sidebar routines.
 */
gboolean gtranslator_messages_table_show(void);
gboolean gtranslator_messages_table_hide(void);

void gtranslator_messages_table_toggle(void);

#endif
