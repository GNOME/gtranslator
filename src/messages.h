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
* The different stati of the translatable
*  messages .
**/
enum gtr_msg_status
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
	GTRANSLATOR_MSG_ASSIMILATED_BY_THE_BORG
};

#endif // GTR_MESSAGES_H
