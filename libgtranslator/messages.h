/**
*
* (C) 2000 Fatih Demir -- kabalak / kabalak@gmx.net
*          Gediminis Paulauskas -- menesis / menesis@delfi.lt
*
* This is distributed under the GNU GPL V 2.0 or higher which can be
*  found in the file COPYING for further studies.
*
* Enjoy this piece of software, brain-crack and other nice things.
*
* WARNING: Trying to decode the source-code may be hazardous for all your
*       future development in direction to better IQ-Test rankings!
*       
**/

#ifndef MESSAGES_H
#define MESSAGES_H 1

#include <glib.h>

/**
* The different stati of the messages.
**/
typedef enum {
	GTR_MSG_STATUS_UNKNOWN		= 0,
	GTR_MSG_STATUS_TRANSLATED	= 1 << 0,
	GTR_MSG_STATUS_FUZZY		= 1 << 1,
	/* If the message shouldn't be translated (web adresses/trade marks) */
	GTR_MSG_STATUS_STICK		= 1 << 2
} GtrMsgStatus;

/**
* Again a try to make a common usable message-struct
**/
typedef struct {
	gchar *comment;
	gchar *msgid;
	gchar *msgstr;
	/* What this pos is used for? */
	gint pos;
	GtrMsgStatus status;
} GtrMsg;

/* Defines for easy casting */
#define GTR_MSG_STATUS(x) ((GtrMsgStatus *)x)
#define GTR_MSG(x) ((GtrMsg *)x)

#endif
