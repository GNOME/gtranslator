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

#ifndef GTR_MESSAGE_H
#define GTR_MESSAGE_H 1

#include "parse.h"

typedef gboolean (*FEFunc) (gpointer list_item, gpointer user_data);
typedef gboolean (*FEFuncR) (gpointer list_item, gpointer user_data, gboolean first);

/*
 * Execute the given FEFunc for every element.
 */
gboolean gtranslator_message_for_each(GList *first, FEFunc func, gpointer user_data);

/*
 * Finds and shows next fuzzy message.
 */
void gtranslator_message_go_to_next_fuzzy(GtkWidget * widget, gpointer useless);

/*
 * Finds and shows next untranslated message.
 */
void gtranslator_message_go_to_next_untranslated(GtkWidget * widget, gpointer useless);

/*
 * Shows the needed message in text boxes
 */
void gtranslator_message_go_to(GList  * to_go);
void gtranslator_message_go_to_first(GtkWidget  * widget, gpointer useless);
void gtranslator_message_go_to_previous(GtkWidget  * widget, gpointer useless);
void gtranslator_message_go_to_next(GtkWidget  * widget, gpointer useless);
void gtranslator_message_go_to_last(GtkWidget  * widget, gpointer useless);
void gtranslator_message_go_to_no(GtkWidget  * widget, gpointer number);

/*
 * Various GUI/message handling functions.
 */
void gtranslator_message_update(void);
void gtranslator_message_show(GList  * list_item);
void gtranslator_message_change_status(GtkWidget  * widget, gpointer which);

/*
 * Changes message fuzzy status
 */
void gtranslator_message_status_set_fuzzy(GtrMsg  * msg, gboolean fuzzy);

/*
 * Copies msgid to msgstr or blanks msgstr 
 */
void gtranslator_message_status_set_sticky(GtrMsg  *msg, gpointer useless);

/*
 * Frees a GtrMsg.
 */
void gtranslator_message_free(gpointer data, gpointer useless);

#endif
