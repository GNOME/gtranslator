/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* The dnd-functions of gtranslator.
*       
* -- the source
**/

#include "dnd.h"

void gtranslator_dnd(GtkWidget *widget,GdkDragContext *context,
	int x,int y,GtkSelectionData *seldata, guint info,guint time,
		gpointer data)
{
	GList *fnames, *fnp;
	func=(enum gtr_dnd)data;
	fnames=gnome_uri_list_extract_filenames((char *)seldata->data);
	a_counter=g_list_length(fnames);
	if (a_counter>0)
		for (fnp = fnames; fnp; fnp = fnp->next, a_counter--)
		{
			if (func==GTR_OPEN) 
			{
				parse((char *)(fnp->data));
			}
		}	
	gtk_drag_finish(context,TRUE,FALSE,time);
}
