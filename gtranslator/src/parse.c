/*
*	-> Fatih Demir [ kabalak@gmx.net ]
*	A simple parsing header to complete the char[]-array from the 
*	scripts ...	
*/

#include "parse.h"
#include <unistd.h>
/*
*	The var's keeping the file names ...
*/
const char *idf,*strf,*homedir;
FILE *is,*os;


void home_maker()
{
	homedir = getenv("HOME");
}
/*
*	A simple stream-check (I love the ifstream.good()-func from C++ ....)
*/
void check_file(FILE *stream)
{
	if(stream < 0 || stream == NULL)
	{
		g_warning("\nError opening a file stream !\n");
		/*
		*exit(1);
		*/
	}
}

/*
*	Opens the files at startup
*/
int open_the_files()
{
	is = fopen(idf,"r");
	check_file(is);
	os = fopen(strf,"r+");
	check_file(os);
	return 0;
}

/*
*	The initial function for reading the files ..
*/
void init_reading(void)
{
	input_ids=fopen(ids_file,"r+");
        fgets(iline[0],255,input_ids);
        input_strs=fopen(strs_file,"r+");
        fgets(sline[0],255,input_strs);
        gtk_text_insert(GTK_TEXT(text1),NULL,NULL,NULL,iline,-1);
        gtk_text_insert(GTK_TEXT(trans_box),NULL,NULL,NULL,sline,-1);
	at_the_first=TRUE;
	max_count=0;
	count=1;
	#ifdef DEBUGGY
	gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Inited reading process ."));
	#endif
	gnome_appbar_set_progress(GNOME_APPBAR(appbar1),15);
}

/*
*	The monster thing :
*/
void parse_this_file_all_through()
{
	/*
	*	Parse the prepared files to the char[]-arrays 
	*/ 
	/* 1. -> For the msgid's */
	at_the_first=FALSE;
	gnome_appbar_set_status(GNOME_APPBAR(appbar1),20);
		while(fgets(iline,LINE_LENGTH,input_ids) != NULL)
		{
			#ifdef DEBUGGY
			gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Reading now msgid's ..."));
			#endif
			fgets(iline,LINE_LENGTH,input_ids);
			count++;
		}
	max_count=count;
	gnome_appbar_set_status(GNOME_APPBAR(appbar1),60);
	/* 2, -> For the msgstr's */
	at_the_first=FALSE;
	
		while(fgets(sline,LINE_LENGTH,input_strs) != NULL)
		{
			fgets(sline,LINE_LENGTH,input_strs);
			count++;
		}
	gnome_appbar_set_status(GNOME_APPBAR(appbar1),100);
}
