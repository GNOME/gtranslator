/**
* Fatih Demir <kabalak@gmx.net>
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* The dnd-functions of gtranslator.
*       
* -- the source
**/

#include "dnd.h"
#include "parse.h"
#include "gui.h"

void gtranslator_dnd(GtkWidget * widget, GdkDragContext * context, int x,
		     int y, GtkSelectionData * seldata, guint info,
		     guint time, gpointer data)
{
	gint return_value = 0;
	gchar *file;
	GList *fnames, *fnp;

	dnd_type = GPOINTER_TO_UINT(data);
	fnames = gnome_uri_list_extract_filenames((char *) seldata->data);
	/**
        * Have we got a filename to check?
        **/
	if (g_list_length(fnames) > 0) {
		/**
                * Check the types of the entries of the list
                *  with the filenames.
                **/
		for (fnp = fnames; fnp; fnp = fnp->next) {
			/**
                        * Is it a filename?
                        **/
			if (dnd_type == TARGET_URI_LIST) {
				file = (char *) (fnp->data);
				parse(file);
				return_value = 1;
			} else {
				/**
                                * .. or a dropped URL to a po-file from Netscape.
                                **/
				if (dnd_type == TARGET_NETSCAPE_URL) {
					file = (char *) (fnp->data);
					parse(file);
					return_value = 1;
				}
				/**
                                * .. or simply plain text.
                                **/
				if (dnd_type == TARGET_TEXT_PLAIN) {
					file = (char *) (fnp->data);
					gtk_editable_insert_text(GTK_EDITABLE
								 (trans_box),
								 file,
								 sizeof(file),
								 0);
					return_value = 1;
				}
			}
		}
	}
	gnome_uri_list_free_strings(fnames);
	/**
        * Check if the Drop was successfull.
        **/
	if (return_value == 1) {
		gtk_drag_finish(context, TRUE, FALSE, time);
	} else {
		gtk_drag_finish(context, FALSE, TRUE, time);
	}
}

