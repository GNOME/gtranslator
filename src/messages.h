/**
* Fatih Demir [ kabalak@gmx.net ]
* Gediminas Paulauskas <menesis@delfi.lt>
* 
* (C) 2000 Published under GNU GPL V 2.0+
*
* The messages-general stuff will be stored here .
*
**/

#ifndef GTR_MESSAGES_H
#define GTR_MESSAGES_H 1

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
