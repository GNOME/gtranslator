/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* The find-routines for gtranslator.
*
* -- the source.
**/

#include "find.h"

/**
* Returns the first result from the list as a
*  gchar.
**/
gchar *gtr_find(GList *list,gchar *query)
{
	results=g_list_find((GList *) list,(gpointer) query);
	if(!((gchar *)(results->data)))
	{
		return _("Nothing appropriate found!");
	}
	else
	{
		return (gchar *)(results->data);
	}
}

/**
* This returns a useless and somethimes funny gchar for
*  instance.
**/
gchar *gtr_find_get_hastalavista(const gchar *query)
{
	if(!query)
	{
		return _("No query, no fun.");
	}
	else
	{
		return "FUN!";
	}
}
