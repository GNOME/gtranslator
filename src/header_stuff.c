/*
*	-> Fatih Demir [ kabalak@gmx.net ]
*	Here are the header-cutting&-edging-routines
*	of gtranslator .
*/

#include "header_stuff.h"

void cut_the_header_off_it(FILE *mfs,const char *cutties)
{
	fseek(mfs,0L,SEEK_SET);
	while((fgets(tmp,256,mfs) != NULL) && (are_we_at_the_end(tmp,cutties) == 1));
}

int are_we_at_the_end(char test,const char *ends)
{
	if(!strncmp(test,ends,strlen(ends)))
	{
		return 0;
	}	
	return 1;
}
