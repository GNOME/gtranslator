/*
*	-> Fatih Demir [ kabalak@gmx.net ]
*	A simple parsing header to complete the char[]-array from the 
*	scripts ...	
*/

#include "parse.h"

/*
*	The var's keeping the chars ...
*/
const char *idf="~/.gtranslator/msgids.dat";
const char *strf="~/.gtranslator/msgstrs.dat";
FILE *is,*os;

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
	#ifdef DEBUGGY
	gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Inited reading process ."));
	#endif
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
	max_count=0;
	for(count=1;count<=65000;++count)
	{	
		while(fgets(iline[count],sizeof(iline[count]),input_ids) != NULL)
		{
			#ifdef DEBUGGY
			gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Reading now msgid's ..."));
			#endif
			fgets(iline[count],sizeof(iline[count]),input_ids);
			count++;
			max_count++;
		}
	}
	/*
	if(count < 65000 || count > 0 )
	{
		max_count=count;
	}
	else if(count < 0 )
	{
		max_count=0;
	}
	else
	{*/
		/* 
		*	If the read fails , notify the user
		*/	
	/*	gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Error while parsing the msgid's !"));
	}*/
	/* 2, -> For the msgstr's */
	max_count=0;
	for(count=1;count<=65000;++count)
	{
		while(fgets(sline[count],sizeof(sline[count]),input_strs) != NULL)
		{
			fgets(sline[count],sizeof(sline[count]),input_strs);
			count++;
			max_count++;
		}
	}
	/*
	if(count < 65000 || count >= 0 )
        {
                max_count=count;
		#ifdef DEBUGGY
		gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Reading now the msgstr's ..."));
		#endif
        }
        else if(count < 0 )
        {
                max_count=0;
        }
        else
        {*/
		/*
		*	The same thing as above ...
		*/
           /*     gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Error while parsing the msgstr's !"));
        }*/
}
