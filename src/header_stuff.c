/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* Completely reorganized header-functions 
*
* -- source
**/

#include "header_stuff.h"

void apply_header(gtr_header the_header)
{
	if((the_header.project_id==NULL)||(strlen(the_header.project_id)<=0))
	{
		g_warning(_("Error while parsing the header!"));
	}
}
