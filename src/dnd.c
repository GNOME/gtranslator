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
int ret=1;
GList *fnames, *fnp;
enum gtr_dnd func;
func=(enum gtr_dnd)data;

fnames=gnome_uri_list_extract_filenames((char *)seldata->data);
a_counter=g_list_length(fnames);

if (a_counter>0)
	for (fnp = fnames; fnp; fnp = fnp->next, count--)
	{
		if (func==GTR_OPEN) ret=parse((char *)(fnp->data));
	}

	gnome_uri_list_free_strings(fnames);

if (ret==0)
{
	gtk_drag_finish(context,TRUE,FALSE,time);
}
else
{
	gtk_drag_finish(context,FALSE,FALSE,time);
}

	/**
	* Get the drag-sets
	**/
	gtk_drag_dest_set(widget, GTK_DEST_DEFAULT_ALL, dragtypes,
		sizeof(dragtypes)/sizeof(dragtypes[0]),GDK_ACTION_COPY);
	/**
	* Connecting the function(s)
	**/
	gtk_signal_connect(GTK_OBJECT(widget), "drag_data_received",
		GTK_SIGNAL_FUNC(gtranslator_dnd), (gpointer)func);
}
