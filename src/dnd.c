/*
 * (C) 2000 	Fatih Demir <kabalak@gmx.net>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *    
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "dnd.h"
#include "parse.h"
#include "gui.h"

/*
 * The general D'n'D function.
 */ 
void gtranslator_dnd(GtkWidget * widget, GdkDragContext * context, int x,
		     int y, GtkSelectionData * seldata, guint info,
		     guint time, gpointer data)
{
	gint return_value = 0;
	gchar *file;
	GList *fnames, *fnp;

	dnd_type = GPOINTER_TO_UINT(data);
	fnames = gnome_uri_list_extract_filenames((char *) seldata->data);
	/*
         * First we do obtain, that we even did get any filenames list.
         */
	if (g_list_length(fnames) > 0) {
		/*
                 * Check the list entries for our supported D'n'D types.
                 */
		for (fnp = fnames; fnp; fnp = fnp->next) {
			if (dnd_type == TARGET_URI_LIST) {
				file = (char *) (fnp->data);
				parse(file);
				return_value = 1;
			} else {
				if (dnd_type == TARGET_NETSCAPE_URL) {
					file = (char *) (fnp->data);
					parse(file);
					return_value = 1;
				}
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
	
	/*
         * Displays if the Drop was successfull.
         */
	if (return_value == 1) {
		gtk_drag_finish(context, TRUE, FALSE, time);
	} else {
		gtk_drag_finish(context, FALSE, TRUE, time);
	}
}

