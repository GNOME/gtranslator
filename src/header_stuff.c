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

/**
* The general header
**/
gtr_header ph;

/**
* A simple define; ok I'm lazy ..
**/
#define kabalak_str(x) inp=NULL; inp=strstr(hline, ": ");\
ph.x=g_strchug(strstr(inp, " "));

void apply_header(gtr_header the_header)
{
	if((the_header.project_id==NULL)||(strlen(the_header.project_id)<=0))
	{
		g_warning(_("Error while parsing the header!"));
	}
}

void get_header(gchar *hline)
{
	gchar *inp;
	if(!g_strncasecmp(hline,"\"Pro",4))
	{
		kabalak_str(project_id);
	}
	if(!g_strncasecmp(hline,"\"POT-",5))
	{
		kabalak_str(pot_date);
	}
	if(!g_strncasecmp(hline,"\"PO-",4))
	{
		kabalak_str(po_date);
	}
	if(!g_strncasecmp(hline,"\"Las",4))
	{
		kabalak_str(last_translator);
	}
	if(!g_strncasecmp(hline,"\"Lang",5))
	{
		kabalak_str(language_team);
	}
	if(!g_strncasecmp(hline,"\"MIME",5))
	{
		kabalak_str(mime_version);
	}
	if(!g_strncasecmp(hline,"\"Content-Ty",11))
	{
		kabalak_str(mime_type);
	}
	if(!g_strncasecmp(hline,"\"Content-Tr",11))
	{
		kabalak_str(encoding);
	}
	if(!g_strncasecmp(hline,"\"X-G",4))
	{
		kabalak_str(xtranslator);
	}
}

void print_header()
{
	g_print("%s",ph.last_translator);
}
