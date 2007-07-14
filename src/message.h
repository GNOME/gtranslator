/*
 * (C) 2001-2003		Gediminas Paulauskas <menesis@kabalak.net>
 * 				Fatih Demir <kabalak@kabalak.net>
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

#include "comment.h"

#include <gtk/gtktreemodel.h>
#include <gtk/gtkwidget.h>

#include <gettext-po.h>

/*
 * The different stati of the messages.
 */
typedef enum {
	GTR_MSG_STATUS_UNKNOWN		= 0,
	GTR_MSG_STATUS_TRANSLATED	= 1 << 0,
	GTR_MSG_STATUS_FUZZY		= 1 << 1,

	/*
	 * "Sticky" is just a virtual state for messages where the
	 *   original and the translation should be the same.
	 *
	 * Currently this is only internally useful but when using extended
	 *  message comments this status might also get "persistent".
	 */
	GTR_MSG_STATUS_STICK		= 1 << 2,

	/*
	 * "Review" could be another virtual/semi-conservative message status
	 *   with which a translator could mark a message for review via this
	 *    tag in the comment of the message for example.
	 */
	GTR_MSG_STATUS_REVIEW		= 1 << 3
} GtrMsgStatus;

/*
 * The generally used message structure in gtranslator.
 */ 
typedef struct {
	GtrComment 	*comment;

	po_message_t message;
	
	gint		no;
	gint 		pos;

	GtrMsgStatus 	status;

	gboolean	is_fuzzy;
	
	/*
	 * GtkTreeIter for when using the messages table
	 */
	GtkTreeIter     iter;
	
	/*
	 * Flag to indicate it has changed since being loaded
	 */
	gboolean	changed;
} GtrMsg;

/*
 * Defines for easy casting 
 */
#define GTR_MSG_STATUS(x) ((GtrMsgStatus *)x)
#define GTR_MSG(x) ((GtrMsg *)x)

typedef gboolean (*FEFunc) (gpointer list_item, gpointer user_data);
typedef gboolean (*FEFuncR) (gpointer list_item, gpointer user_data, gboolean first);

/*
 * Returns TRUE if the message is translated
 */
gboolean po_message_is_translated (po_message_t message);

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
void gtranslator_message_show(GtrMsg  * msg);
void gtranslator_message_change_status(GtkWidget  * widget, gpointer which);
void gtranslator_message_copy_to_translation(void);

/*
 * Changes message fuzzy status
 */
void gtranslator_message_status_set_fuzzy(GtrMsg  * msg, gboolean fuzzy);
void gtranslator_message_status_toggle_fuzzy(GtkWidget *widget, gpointer useless);

/*
 * Copies msgid to msgstr or blanks msgstr 
 */
void gtranslator_message_status_set_sticky(GtrMsg  *msg, gpointer useless);

/*
 * Clears the translation (sets msgstr to an empty string)
 */
void gtranslator_message_clear_translation(GtrMsg  *msg, gpointer useless);

/*
 * Frees a GtrMsg.
 */
void gtranslator_message_free(gpointer data, gpointer useless);

#endif
