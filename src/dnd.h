/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gmx.net>
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

#ifndef GTR_DND_H
#define GTR_DND_H 1

#include <gtk/gtkselection.h>

/*
 * The target types for gtranslator.
 */
enum {
	TARGET_URI_LIST,
	TARGET_NETSCAPE_URL,
	TARGET_TEXT_PLAIN
};

/*
 * The used enumeration.
 */ 
guint dnd_type;

/*
 * The receiving D'n'D function for gtranslator.
 */
void gtranslator_dnd(GtkWidget * widget, GdkDragContext * context, int x,
		     int y, GtkSelectionData * seldata, guint info,
		     guint time, gpointer data);

#endif
