/**
 * Fatih Demir [ kabalak@gmx.net ]
 *
 * Here are the routines which could be very
 * useful if you're think of slightly auto-
 * generated translations ..
 **/

#include "msg_db.h"

/**
 * Include the dyndata routines 
 **/
#include <dyndata.h>

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
	/**
	 * After having checked everything create a binary
	 * tree for the translation strings ...
	 **/
	msg_tree=create_bt();
	/**
	 * And a linked list for the single message ids/strs 
	 **/
	msg_list=create_ll();
	msg_db_inited=TRUE;
	/**
	 * Get every message from the msg_db as a new member 
	 * to the linked list while reading the file
	 **/
	while(
	fgets(msg_messages,sizeof(msg_messages),db_stream) != NULL)
	{
		/**
 		 * Add it to the linked list
		 **/
		add_ll(msg_list,&msg_messages[0]);
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

int put_to_msg_db(const gchar *new_message)
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
			g_warning("No message entry got !\n");
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
			/**
			 * FIXME
			 * Add a leaf to the btree ...
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
		 * If there's no LL* msg_list 
		 * print an error message
  		 **/
		g_error("No msg_list available for acting on it !\n");
		 /**
		 * Exit brutally ...
		 **/
	}
}
