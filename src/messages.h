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

/**
* Typedefs for the following structures
**/
typedef struct _po_file gtr_po;
typedef struct _message gtr_msg;

/**
* The structure with general 
*  informations ...
**/
struct _po_file 
{
	gchar *po_filename;
	unsigned int file_length;
	gboolean opened;
};

/**
* Every message should be filled
*  in here .
**/
struct _message
{
	gtr_po *po;
	gchar *msgid;
	gchar *msgstr;
	unsigned int position;
	gtr_msg_status *status;
};

/**
* The different stati of the translatable
*  messages .
**/
typedef enum
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
} gtr_msg_status ;
