/**
 * Fatih Demir [ kabalak@gmx.net ]
 *
 * (C) 2000 Published under GNU GPL V 2.0+
 *
 * Here are the routines which could be very
 * useful if you're think of slightly auto-
 * generated translations ..
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
	check_file(db_stream,"Couldn't open the msg-db !");
	msg_db_inited=TRUE;
	/**
	 * Get every message from the msg_db as a new member 
	 * to the linked list while reading the file ; 
  	 *
	 * 1) Allocate the linked lists
	 *
	 **/
	msg_list = g_list_alloc();
	cur_list = g_list_alloc();
	while(
	(fgets(msg_messages,sizeof(msg_messages),db_stream) != NULL)
	)
	{
		/**
 		 * 2) Parse the msg_db and add every entry to the
		 *     the linked lists 
		 **/
		msg_list = g_list_append(cur_list,&msg_messages[0]);
	}
}

void close_msg_db()
{
	/**
	 * Have we lost the file stream  
	 **/
	check_file(db_stream,"Couldn't close the msg-db !");
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

int put_to_msg_db(const gchar *msg_id,const gchar *new_message)
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
		g_warning("The msg_db : %s seems not to be inited !\n",msg_db);
		return 1;
	}
	else
	{
		gint new_message_length;
		if(!new_message)
		{
			/**
			 * Warn if there isn't any message
			 **/
			g_warning("Got no message entry !\n");
			return 1;	
		}
		/**
		 * Get the length of the new entry
		 **/
		new_message_length=strlen(new_message);
		if(( new_message_length < 0 ) || ( ! new_message ))
		{
			g_warning("New message entry has wrong/non-logical length : %i\n",new_message_length);
			return 1;
		}
		else
		{
			/** FIXME 
			 * Where to add this ?
			 **/	
		}
	}
	return 0;
}

gchar *get_from_msg_db(const gchar *get_similar)
{
	if(msg_db_inited==TRUE)
	{
		 /**
                 * Show a little warning ...
                 **/
                g_warning("The msg_db : %s seems not to be inited !\n",msg_db);
		 /**
		 * Return an explaining char* ...
		 **/
                return "Not available";
	}
	if(!msg_list)
	{
		 /**
		 * If there's no msg_list 
		 *  print an error message
  		 **/
		g_error("No msg_list available for acting on it !\n");
		 /**
		 * Exit brutally ...
		 **/
	}
}
