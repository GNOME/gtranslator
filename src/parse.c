/*
*	-> Fatih Demir [ kabalak@gmx.net ]
*	Here will be some useful parts of gtranslator 
*	next periods of time ....
*/

#include "parse.h"
#include <unistd.h>
/*
*	The var's keeping the file names ...
*/
const char *idf,*strf;
FILE *is,*os;

/*
*	A simple stream-check (I love the ifstream.good()-func from C++ ....)
*/
void check_file(FILE *stream)
{
	if(stream < 0 || stream == NULL)
	{
		g_warning("\nError opening the file stream !\n");
		#ifdef SINIR_ET_BENI
		exit(1);
		#endif
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
	check_file(fs);
	count=0;
	while((fgets(tmp_l,sizeof(tmp_l),fs)) != NULL)
	{
		count++;
		/*
		*Are we at a msgid ?
		*/
		if(!strncmp(tmp_l,"msgid \"",6))
		{
			/*
			* If so , copy the line to iline
			*/
			strcpy(iline,tmp_l);
		}
	}
	max_count=((count - 10 ) / 3);
	old_count=count;
	count=1;
	
}
