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
	/**
	* Use the default location if no other msg_db
	*  has been defined.
	**/
	if(!msg_db)
	{
		gchar temp_db[128];
		sprintf(temp_db,"%s/%s",g_get_home_dir(),"msg.db");
		msg_db=temp_db;
	}
	if(fopen(msg_db,"r+")==NULL)
	{
		g_message(_("No msg_db found! Creating one in `%s'."),msg_db);
		db_stream=fopen(msg_db,"a+");
	}
	else
	{
		db_stream=fopen(msg_db,"r+");
	}
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
	if(msg_db_inited!=TRUE)
	{
		/**
		* Show a little warning ...
		**/
		g_warning(_("The msg_db `%s' hasn't been inited yet!\n"),msg_db);
			return 1;
	}
	else
	{
		if((! msg_translation )||( strlen(msg_translation) < 0) )
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
		if(( strlen(msg_translation) < 0 ) || ( ! msg_translation ))
		{
			g_warning(_("New message entry has wrong/non-logical length.\n"));
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
			* These ';;' are used as separators and shouldn't
			*  appear in normal context ...
			**/
			fputs(";;",db_stream);
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
			if(!g_strncasecmp(msg_messages,get_similar,3))
			{
				if(strstr(msg_messages,";;"))
				{
					gchar *emp;
					gint emp_i=0,i;
					emp[0]='\0';
					while(msg_messages[emp_i]!=';')
					{
						emp_i++;
					}
					emp_i++;emp_i++;
					for(i=0;i<((strlen(msg_messages))-emp_i-1);++i)
					{
						emp[i]=msg_messages[emp_i];
						emp_i++;
					}
					return (gchar *)emp;
				}
			}
		}	
		return _("No entry found in the msg_db.");
	}
}
