/**
* Fatih Demir [ kabalak@gmx.net ]
* 
* (C) 2000 Published under GNU GPL V 2.0+
*
* The messages-general stuff will be stored 
* here .
*
* -- messages are@home->*this;
**/

#ifndef GTR_MESSAGES_H
#define GTR_MESSAGES_H 1

#include <glib.h>

/**
* Typedefs for the `new' data-types
**/
typedef enum   _GtrMsgStatus	GtrMsgStatus;
typedef struct _GtrMsg	GtrMsg;
typedef struct _GtrPo	GtrPo;

// defines for easy casting
#define GTR_MSG_STATUS(x) ((GtrMsgStatus *)x)
#define GTR_MSG(x) ((GtrMsg *)x)
#define GTR_PO(x) ((GtrPo *)x)

/**
* The different stati of the translatable
*  messages .
**/
enum _GtrMsgStatus
{
	GTRANSLATOR_MSG_STATUS_UNKNOWN		= 0,
	GTRANSLATOR_MSG_STATUS_TRANSLATED	= 1 << 0,
	GTRANSLATOR_MSG_STATUS_FUZZY		= 1 << 1,
 	// If the message shouldn't be translated 
	// (Web-adresses/trade mark names)
	GTRANSLATOR_MSG_STATUS_STICK		= 1 << 2
};

/**
* Again a try to make a common usable message-struct
**/
struct _GtrMsg
{
	gchar *msgid;
	gchar *msgstr;
	gchar *comment;
// What this pos is used for?
	gint pos;
	GtrMsgStatus status;
};

/**
* The general po-file structure
**/
struct _GtrPo
{
	/**
	* The filename
	**/
	gchar *filename;
	/**
	* If no translated entry has been found this'd set to TRUE.
	**/
	gboolean notrans;
	/**
	* A pointer to the first message
	**/
	GtrMsg *first;
	/**
	* A pointer to the last message
	**/
	GtrMsg *last;
};

#endif
