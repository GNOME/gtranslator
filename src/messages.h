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
typedef enum   _gtr_msg_status gtr_msg_status;
typedef struct _gtr_msg        gtr_msg;
typedef struct _gtr_po         gtr_po;

/**
* The different stati of the translatable
*  messages .
**/
enum _gtr_msg_status
{
	GTRANSLATOR_MSG_STATUS_UNTRANSLATED,
	GTRANSLATOR_MSG_STATUS_TRANSLATED,
	GTRANSLATOR_MSG_STATUS_UNKNOWN,
	/**
 	* If the message shouldn't be translated
 	*  ( -> Web-adresses/trade mark names )
	**/
	GTRANSLATOR_MSG_STATUS_STICK,
	/**
	* If the entry should be translated as
	*  fast as possible .
	**/
	GTRANSLATOR_MSG_STATUS_FIXME,
	/**
	* ?!*S?=
	**/
	GTRANSLATOR_MSG_ASSIMILATED_BY_THE_BORG_OR_BY_MS
};

/**
* Again a try to make a common usable message-struct
**/
struct _gtr_msg
{
	gchar *msgid;
	gchar *msgstr;
	gchar *comment;
	gint pos;
	gtr_msg_status msg_status;
};

/**
* The general po-file structure
**/
struct _gtr_po
{
	gchar *filename;
	gboolean notrans;
};

#endif
