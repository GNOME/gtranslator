/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* Here are the header-cutting&-edging-routines
* of gtranslator .
**/

#include "header_stuff.h"

/**
* A count variable
**/

int _count=0;

/**
* Try to catch the header
**/

void cut_the_header_off_it(FILE *mfs,const gchar *cutties)
{
	/**
	* Go to the start
	**/
	fseek(mfs,0L,SEEK_SET);
	/**
	* Some stupid search for the end of the header
	**/
	while((fgets(tmp,256,mfs) != NULL) && (are_we_at_the_end((gchar *)tmp,cutties) == 1))
	{
		_count++;
	}
	/**
	* Have we got any header ?
	**/
	if(_count > 1)
	{
		int i;
		/**
		* Then copy the lines into ´header´
		**/
		for(i=0;i<=_count;++i)
		{
			fgets(tmp,256,mfs);
			strcpy(&header[128][i],tmp);
		}	
	}
}

int are_we_at_the_end(gchar *test,const gchar *ends)
{
	/**
	* Are we at the keylines-end :
	* "Content-Transfer .... \n"
	**/
	if(!strncasecmp(test,ends,strlen(ends)))
	{
		/**
		* Then return 0
		**/
		return 0;
	}	
	/**
	* Or 1 if not ...
	**/
	return 1;
}
