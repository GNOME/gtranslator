/*
 * (C) 2000-2003 	Fatih Demir <kabalak@gtranslator.org>
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
#include "dnd.h"
#include "gui.h"
#include "parse.h"
#include "syntax.h"

#include <gtk/gtkdnd.h>
#include <gtk/gtktext.h>

/*
 * The general D'n'D function.
 */ 
void gtranslator_dnd(GtkWidget * widget, GdkDragContext * context, int x,
		     int y, GtkSelectionData * seldata, guint info,
		     guint time, gpointer data)
{
	gint return_value = 0;
	gchar *file;

	dnd_type = GPOINTER_TO_UINT(data);
	file=((gchar *) (seldata->data));
	
	if(dnd_type==TARGET_URI_LIST || dnd_type==TARGET_NETSCAPE_URL)
	{
		gtranslator_parse_main(file);
		gtk_drag_finish(context, TRUE, FALSE, time);
	}
	else if(dnd_type==TARGET_TEXT_PLAIN)
	{
		gtranslator_insert_text(trans_box, file);
		gtk_drag_finish(context, TRUE, FALSE, time);
	}
	else
	{
		gtk_drag_finish(context, FALSE, TRUE, time);
	}
}

