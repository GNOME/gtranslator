/**
 * Fatih Demir [ kabalak@gmx.net ]
 * 
 * (C) 2000 Published under GNU GPL V 2.0+
 *
 * The messages-general stuff will be stored 
 * here .
 **/

#include "callbacks.h"

/**
 * The structure with general 
 *  informations ...
 **/
struct po_file 
{
	char *po_filename;
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
	char *msgid;
	char *msgstr;
	unsigned int position;
	gboolean translated;
};

/**
 * The used message structure pointer 
 **/
struct message *msg;
