/**
 * Fatih Demir [ kabalak@gmx.net ]
 *
 * (C) 2000 Published under GNU GPL V 2.0+
 *
 * Here will be some useful parts of gtranslator 
 * next periods of time ....
 **/

#include "parse.h"
#include "messages.h"
#include <unistd.h>

/**
 * The file-stream for the file-check
 **/

FILE *fs;

/**
 * A simple stream-check (I love the ifstream.good()-func from C++ ....)
 **/
void check_file(FILE *stream,const char *error)
{
	if(stream == NULL)
	{
		g_error(error);
	}
}

/**
 * The real routine for parsing 
 **/
void parse()
{
	gchar tmp_l[256];
	/** 
	 * Open the file got by the open-dialog
	 **/
	fs=fopen(filename,"r+");
	/**
	 * Check if the stream is OK
	 **/
	check_file(fs,"Couldn't open the po file !");
	msg->po->opened=TRUE;
	msg->po->po_filename=(char *)filename;
	#ifdef DEBUG
	g_print("Got filename  %s \n",filename);
	g_print("Or : %s\n",msg->po->po_filename);
	#endif
	count=0;
	while((fgets(tmp_l,sizeof(tmp_l),fs)) != NULL)
	{
		count++;
		/**
		 * Are we at a msgid ?
		 **/
		if(!strncasecmp(tmp_l,"msgid \"",6))
		{
			/**
			 * Copy the current string to a message 
			 **/
			msg->msgid=tmp_l;
			/**
			 * Define the position
			 **/
			msg->position=count;
			/**
			 * Check if the next line 
			 * is a msgstr 
			 **/
			fgets(tmp_l,sizeof(tmp_l),fs);
			if(!strncasecmp(tmp_l,"msgstr \"",7))
			{	
				/**
				 * If this succeeds , build a msg-block
				 **/
				msg->msgstr=tmp_l;
			}
			/**
			 * If not , read another line & check it  		
			 **/
			fgets(tmp_l,sizeof(tmp_l),fs);
			/**
			 * Again the same
			 **/
			if(!strncasecmp(tmp_l,"msgstr \"",7))
			{
				msg->msgstr=tmp_l;
			}
		}
		
	}
	/**
	 * Now we know where the maximum length has to be
	 **/
	msg->po->file_length=(count - 1);
	max_count=((count - 10 ) / 3);
}
