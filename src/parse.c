/**
 * Fatih Demir [ kabalak@gmx.net ]
 *
 * Here will be some useful parts of gtranslator 
 * next periods of time ....
 **/

#include "parse.h"
#include <unistd.h>

/**
 * A simple stream-check (I love the ifstream.good()-func from C++ ....)
 **/
void check_file(FILE *stream,const char *error)
{
	if(stream < 0 || stream == NULL)
	{
		g_error(error);
		/**
		 * FIXME :
		 * Too sensitive for gtranslator ?
		 **/
		/***exit(1);***/
	}
}

/**
 * The real routine for parsing 
 **/
void parse()
{
	FILE *fs;
	gchar tmp_l[256];
	int c;
	/** 
	 * Open the file got by the open-dialog
	 **/
	fs=fopen(filename,"r+");
	/**
	 * Check if the stream is OK
	 **/
	check_file(fs,"Couldn't open the po file !");
	count=0;
	/**
	 * Just for now , the db-functionability is
	 * disabled ..
	 **/
	msg->po.db_enabled=FALSE;
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
	msg->po_file->maximal_position=(count - 1);
	max_count=((count - 10 ) / 3);
}
