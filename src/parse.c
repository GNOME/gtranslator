/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* Here will be some useful parts of gtranslator 
* next periods of time ....
*
* -- the source
**/

#include "parse.h"
#include "messages.h"
#include <errno.h>
#include <unistd.h>

/**
* The file-stream for the file-check
**/

FILE *fs;

/**
* A simple stream-check (I love the ifstream.good()-func from C++ ....)
**/
void check_file(FILE *stream)
{
	if(stream == NULL)
	{
		/**
		* If there are any problems , give a
		*  message ..
		**/
		g_error(_("\nThe file stream is lost !\nError No. %i .\n"),errno);
	}
}

/**
* This have to be renamed and used in the
*  msg_*-getting routines ...
**/
void parse(char *filename)
{
	gchar tmp_l[256];
	gtr_msg *msg;	

	/** 
	* Open the file got by the open-dialog
	**/
	fs=fopen(filename,"r+");
	/**
	* Check if the stream is OK
	**/
	check_file(fs);
	msg->po->opened=TRUE;
	msg->po->po_filename=(char *)filename;
	g_print (_("Got filename  %s \n"),(char *)filename);
	count=0;
	while((fgets(tmp_l,sizeof(tmp_l),fs)) != NULL)
	{
		count++;
		/**
		* Are we at a msgid ?
		**/
		if(strncasecmp("msgid \"",tmp_l,7))
		{
			/**
			* Copy the current string to a message 
			**/
			msg->msgid=(char *)tmp_l;
			/**
			* Define the position
			**/
			msg->position=count;
			/**
			* Check if the next line 
			* is a msgstr 
			**/
			fgets(tmp_l,sizeof(tmp_l),fs);
			/**
			* While there's no msgstr ...
			* add it to the msgid ...
			**/
			while(!strncasecmp("msgstr \"",tmp_l,8))
			{
				/**
				* Add it , add it , ... 
				**/
				fgets(tmp_l,sizeof(tmp_l),fs);
				strncat(msg->msgid,tmp_l,sizeof(tmp_l));
			}
		} else {
			/**
			* Now we're hopefully at a msgstr
			**/
			msg->msgstr=(char *)tmp_l;
			while(!strncasecmp("msgid \"",tmp_l,7))
			{
				/**
				* Add it , add it , ... 
				**/
				fgets(tmp_l,sizeof(tmp_l),fs);
				strncat(msg->msgstr,tmp_l,sizeof(tmp_l));
			}
		}
		
	}
	/**
	* Now we know where the maximum length has to be
	**/
	msg->po->file_length=(count - 1);
	max_count=((count - 10 ) / 3);
}
