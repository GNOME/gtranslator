/*
 * (C) 2000-2003 	Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
 *			Ross Golder <ross@gtranslator.org>
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

#ifndef GTR_MESSAGES_H
#define GTR_MESSAGES_H 1

#include <glib.h>
#include <gtk/gtktreemodel.h>
#include "comment.h"

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

	gchar 		*msgid;
	gchar 		*msgstr;
	
	gint		no;
	gint 		pos;

	/* Cursor position in translation text box*/
	gint		cursor_offset;
	
	gchar		*msgid_plural;

	/*
	 * The plural forms msgstrs - the first "msgstr[0]" is stored in the
	 *  1st message translation equivalent "msgstr" as it should be logical.
	 */
	gchar		*msgstr_1;
	gchar		*msgstr_2;
	
	GtrMsgStatus 	status;

	/*
	 * GtkTreeIter for when using the messages table
	 */
	GtkTreeIter     iter;
} GtrMsg;

/*
 * Defines for easy casting 
 */
#define GTR_MSG_STATUS(x) ((GtrMsgStatus *)x)
#define GTR_MSG(x) ((GtrMsg *)x)

#endif
