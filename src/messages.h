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

#include "languages.h"

/**
* The structure with general 
*  informations ...
**/
struct po_file 
{
	gchar *po_filename;
	unsigned int file_length;
	gboolean opened;
};

/**
* Every message should be filled
*  in here .
**/
struct message
{
	struct po_file *po;
	gchar *msgid;
	gchar *msgstr;
	unsigned int position;
	gboolean translated;
};

/**
* The used message structure pointer 
**/
struct message *msg;

/**
* The typedef for the message stati
**/
typedef enum _gtranslator_msg_status gtranslator_msg_status;

/**
* The different stati of the translatable
*  messages .
**/
enum _gtranslator_msg_status 
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
	GTRANSLATOR_MSG_STATUS_FIXME
};
