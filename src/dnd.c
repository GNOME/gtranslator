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
	gint return_value=0;
	gchar *file;
	GList *fnames, *fnp;
	dnd_type=(gtr_dnd_types)data;
	fnames=gnome_uri_list_extract_filenames((char *)seldata->data);
	a_counter=g_list_length(fnames);
	if (a_counter>0)
		for (fnp = fnames; fnp; fnp = fnp->next, a_counter--)
		{
			if (dnd_type==TARGET_URI_LIST) 
			{
				file=(char *)(fnp->data);
				parse(file);
				return_value=1;
			}
			else
			{
				if(dnd_type==TARGET_NETSCAPE_URL)
				{
					file=(char *)(fnp->data);
					parse(file);
					return_value=1;
				}
				if(dnd_type==TARGET_TEXT_PLAIN)
				{
					file=(char *)(fnp->data);
					gtk_editable_insert_text(GTK_EDITABLE(trans_box),
					file,sizeof(file),0);	
					return_value=1;
				}
			}
		}
	if(return_value==1)
	{
		gtk_drag_finish(context,TRUE,FALSE,time);
	}
	else
	{
		gtk_drag_finish(context,TRUE,TRUE,time);
	}
}
