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
* The general header structure used in gtranslator
**/
gtr_header po_header;

/**
* A simple define; ok I'm lazy ..
**/
#define kabalak_str(x) inp=NULL; inp=strstr(hline, ": ");\
po_header.x=g_strchug(strstr(inp, " "));

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
	g_print("orig.>%s",hline);
	if(!g_strncasecmp(hline,"\"Pro",4))
	{
		kabalak_str(project_id);
		g_print("edit.>%s",po_header.project_id);
	}
	if(!g_strncasecmp(hline,"\"POT-",5))
	{
		kabalak_str(pot_date);
		g_print("edit.>%s",po_header.pot_date);
	}
	if(!g_strncasecmp(hline,"\"PO-",4))
	{
		kabalak_str(po_date);
		g_print("edit.>%s",po_header.po_date);
	}
	if(!g_strncasecmp(hline,"\"Las",4))
	{
		kabalak_str(last_translator);
		g_print("edit.>%s",po_header.last_translator);
	}
	if(!g_strncasecmp(hline,"\"Lang",5))
	{
		kabalak_str(language_team);
		g_print("edit.>%s",po_header.language_team);
	}
	if(!g_strncasecmp(hline,"\"MIME",5))
	{
		kabalak_str(mime_version);
		g_print("edit.>%s",po_header.mime_version);
	}
	if(!g_strncasecmp(hline,"\"Content-Ty",11))
	{
		kabalak_str(mime_type);
		g_print("edit.>%s",po_header.mime_type);
	}
	if(!g_strncasecmp(hline,"\"Content-Tr",11))
	{
		kabalak_str(encoding);
		g_print("edit.>%s",po_header.encoding);
	}
	if(!g_strncasecmp(hline,"\"X-G",4))
	{
		kabalak_str(xtranslator);
		g_print("edit.>%s",po_header.xtranslator);
	}
}
