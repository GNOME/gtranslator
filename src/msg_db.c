/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* Here are the routines which could be very
*  useful if you're think of slightly auto-
*   generated translations ..
* 
* Or of globally accesable strings in every 
*  translation.
*
* -- the central-of-interest
**/

#include "msg_db.h"

/**
* The db file-stream
**/
FILE *db_stream;

/**
* The message char
**/
gchar msg_messages[128];

int init_msg_db()
{
	db_stream=fopen(msg_db,"r+");
	/**
	* Have we got a file stream ?
	**/
	check_file(db_stream);
	msg_db_inited=TRUE;
	/**
	* Get every message from the msg_db as a new member 
	* to the linked list while reading the file ; 
  	*
	* 1) Allocate the linked lists
	*
	**/
	msg_list=g_list_alloc();
	cur_list=g_list_alloc();
	while(
	(fgets(msg_messages,sizeof(msg_messages),db_stream) != NULL)
	)
	{
		/**
 		* 2) Parse the msg_db and add every entry to the
		*     the linked lists 
		**/
		msg_list=g_list_append(cur_list,(gpointer)msg_messages);
	}
	/**
	* 3) Check if we had lost the list 
	**/
	if(!msg_list)
	{
		/**
		* 3.1) No list, no fun 
		**/
		g_error(_("gtranslator lost the list of translations!\n"));
	}
	return 0;
}

void close_msg_db()
{
	/**
	* Have we lost the file stream  
	**/
	check_file(db_stream);
	/**
	* Go to the end of the file ..	
	**/
	fseek(db_stream,0L,SEEK_END);
	/**
	* Close the msg-db file(stream)
	**/
	fclose(db_stream);
	/**
	* Make the closure of the msg_db known
	*  to the other routines .
	**/
	msg_db_inited=FALSE;
}

int put_to_msg_db(const gchar *msg_id,const gchar *msg_translation)
{
	/**
	* Check if we've got a working 
	* ( inited ) msg_db ?
	**/
	if(msg_db_inited==FALSE)
	{
		/**
		* Show a little warning ...
		**/
		g_warning(_("The msg_db `%s' seems not to be inited!\n"),msg_db);
		return 1;
	}
	else
	{
		gint msg_translation_length;
		if(!msg_translation)
		{
			/**
			* Warn if there isn't any message which could be added
			*  to the msg_db .
			**/
			g_warning(_("Got no message entry!\n"));
			return 1;	
		}
		/**
		* Get the length of the new entry
		**/
		msg_translation_length=strlen(msg_translation);
		if(( msg_translation_length < 0 ) || ( ! msg_translation ))
		{
			g_warning(_("New message entry has wrong/non-logical length : %i\n"),msg_translation_length);
			return 1;
		}
		else
		{
			/**
			* Go to the end of the file
			**/
			fseek(db_stream,0L,SEEK_END);
			/**
			* Add the given parameters to the msg_db .
			**/
			fputs("\n",db_stream);
			fputs(msg_id,db_stream);
			/**
			* These ';;;' are used as separators and shouldn't
			*  appear in normal context ...
			**/
			fputs(";;;",db_stream);
			fputs(msg_translation,db_stream);
			/**
			* After all this , the story should be at a happy ending 
			**/
				return 0;
		}
	}
	return 0;
}

gchar *get_from_msg_db(const gchar *get_similar)
{
	if(msg_db_inited!=TRUE)
	{
		 /**
                 * Show a little warning ...
                 **/
                g_warning(_("The msg_db `%s' seems not to be inited!\n"),msg_db);
		 /**
		 * Return a very "explaining" char* ...
		 **/
                return _("Not available");
	}
	if(!msg_list)
	{
		 /**
		 * If there's no msg_list 
		 *  print an error message
  		 **/
		g_error(_("No msg_list available for acting on it!\n"));
		 /**
		 * Exit brutally ...
		 **/
	}
	else
	{
		/**
		* Go to the beginning of the file
		**/
		fseek(db_stream,0L,SEEK_SET);
		while(fgets(msg_messages,sizeof(msg_messages),db_stream) != NULL)
		{
			/**
			* Only challenge the first 3 characters
			**/
			if(!g_strncasecmp(msg_messages,get_similar,4))
			{
				if(strstr(msg_messages,";;;"))	
				{
					/** TODO : CUT THEM OFF */
				}
			}
		}	
		return _("No entry found in the msg_db.");
	}
}
