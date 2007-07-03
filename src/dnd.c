/*
 * (C) 2000-2007 	Fatih Demir <kabalak@kabalak.net>
 *			Ignacio Casal <nacho.resa@gmail.com>
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

#include "config.h"
#include "dialogs.h"
#include "dnd.h"
#include "gui.h"
#include "parse.h"

#include <gtk/gtk.h>
#include <string.h>

/*
 * The general D'n'D function.
 */ 
void gtranslator_dnd(GtkWidget * widget, GdkDragContext * context, int x,
		     int y, GtkSelectionData * seldata, guint info,
		     guint32 time, gpointer data)
{
	gchar *file;
	const gchar *file_aux;
	dnd_type = GPOINTER_TO_UINT(data);
	file=((gchar *) (seldata->data));
	file_aux = g_strip_context(file,"file://");
	if(dnd_type==TARGET_URI_LIST || dnd_type==TARGET_NETSCAPE_URL)
	{
		GError *error = NULL;
		if(!gtranslator_open(file_aux, &error)) {
			if(error) {
			    	gtranslator_show_message(error->message, NULL);
				g_error_free(error);
			}
			return;
		}
		gtk_drag_finish(context, TRUE, FALSE, time);
	}
	else
	{
		gtk_drag_finish(context, FALSE, TRUE, time);
	}
	//g_free(file_aux);
}
