/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
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

#include "comment.h"

/*
 * The different stati of the messages.
 */
typedef enum {
	GTR_MSG_STATUS_UNKNOWN		= 0,
	GTR_MSG_STATUS_TRANSLATED	= 1 << 0,
	GTR_MSG_STATUS_FUZZY		= 1 << 1,
	/*
	 * If the message shouldn't be translated (web adresses/trade marks) 
	 */
	GTR_MSG_STATUS_STICK		= 1 << 2
} GtrMsgStatus;

/*
 * The generally used message structure in (lib-)gtranslator.
 */ 
typedef struct {
	GtrComment 	*comment;

	gchar 		*msgid;
	gchar 		*msgstr;
	
	gint 		pos;
	
	GtrMsgStatus 	status;
} GtrMsg;

/*
 * Defines for easy casting 
 */
#define GTR_MSG_STATUS(x) ((GtrMsgStatus *)x)
#define GTR_MSG(x) ((GtrMsg *)x)

#endif
