/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* Completely reorganized header-functions 
*
* -- source
**/

#include "header_stuff.h"

/**     
* To test if the 
*       msgid "" 
*       msgstr ""
*  introduction part has been already passed
*
**/
gboolean nh=FALSE;

void get_header(gpointer a)
{
	if((!g_strncasecmp(a,"msgid \"",7)) && (nh==FALSE))
	{
		head=g_list_append(head,(gpointer)a);
		g_print("-> %s",(gchar *)a);
		nh=TRUE;	
	}
}
