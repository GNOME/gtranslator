/*
* Fatih Demir [ kabalak@gmx.net ]
*
* Here are the header-cutting&-edging-routines
* of gtranslator .
*/

#include "header_stuff.h"

/*
* A count variable
*/

int _count=0;

/*
* Try to catch the header
*/

void cut_the_header_off_it(FILE *mfs,const char *cutties)
{
	/*
	* Go to the start
	*/
	fseek(mfs,0L,SEEK_SET);
	/*
	* Some stupid search for the end of the header ?
	*/
	while((fgets(tmp,256,mfs) != NULL) && (are_we_at_the_end(tmp,cutties) == 1))
	{
		#ifdef DEBUG
		g_print("Searching for this cutties : %s \n",cutties);
		#endif DEBUG
		_count++;
	}
	#ifdef DEBUG 
	g_print("Finished at %i .\n",_count);
	#endif
	/*
	* Have we got any header ?
	*/
	if(_count > 1)
	{
		int i;
		/*
		* Then copy the lines into ´header´
		*/
		#ifdef DEBUG
		g_print("Got a header !\n");
		g_print("Copying the lines into the header-array ...\n");
		#endif
		for(i=0;i<=_count;++i)
		{
			fgets(tmp,256,mfs);
			strcpy(header[128][i],tmp);
		}	
	}
}

int are_we_at_the_end(char test,const char *ends)
{
	/*
	* Are we at the keylines-end :
	* "Content-Transfer .... \n"
	*/
	if(!strncasecmp(test,ends,strlen(ends)))
	{
		/*
		* Then return 0
		*/
		return 0;
	}	
	/*
	* Or 1 if not ...
	*/
	return 1;
}
