/*
*	-> Fatih Demir [ kabalak@gmx.net ]
*	Here will be some useful parts of gtranslator 
*	next periods of time ....
*/

#include "parse.h"
#include <unistd.h>

/*
*	A simple stream-check (I love the ifstream.good()-func from C++ ....)
*/
void check_file(FILE *stream,const char *error)
{
	if(stream < 0 || stream == NULL)
	{
		g_warning(error);
		/*
		* Too sensitive for gtranslator ?
		*exit(1);
		*/
	}
}

/*
*	The real routine for parsing 
*/
void parse()
{
	FILE *fs;
	gchar tmp_l[256];
	int c;
	/* 
	* Open the file got by the open-dialog
	*/
	fs=fopen(filename,"r+");
	/*
	* Check if the stream is OK
	*/
	check_file(fs,"Couldn't open the po file !");
	count=0;
	/*
	* Just for now , the db-functionability is
	* disabled ..
	*/
	msg->po->db_enabled=FALSE;
	while((fgets(tmp_l,sizeof(tmp_l),fs)) != NULL)
	{
		count++;
		/*
		*Are we at a msgid ?
		*/
		if(!strncmp(tmp_l,"msgid \"",6))
		{
			msg->msgid=tmp_l;
			msg->position=count;
			/*
			* If so , check if the next line 
			* is a msgstr 
			*/
			fgets(tmp_l,sizeof(tmp_l),fs);
			if(!strcmp(tmp_l,"msgstr \"",7))
			{	
				/*
				* If this succeeds , build a msg-block
				*/
				msg->msgstr=tmp_l;
			}
		}
		
	}
	max_count=((count - 10 ) / 3);
	count=1;
}
