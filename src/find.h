/*
 * (C) 2000-2001 	Gediminas Paulauskas <menesis@delfi.lt>
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

#ifndef GTR_FIND_H
#define GTR_FIND_H 1

#include <gtk/gtkwidget.h>

typedef gboolean (*FEFunc) (gpointer list_item, gpointer user_data);

gboolean for_each_msg(GList *first, FEFunc func, gpointer user_data);

/*
 * The generally used find function-
 */
void find_do(GtkWidget *widget, gpointer target);

/*
 * Finds and shows next fuzzy message.
 */
void goto_next_fuzzy(GtkWidget * widget, gpointer useless);

/*
 * Finds and shows next untranslated message.
 */
void goto_next_untranslated(GtkWidget * widget, gpointer useless);

void update_flags(void);

#endif
