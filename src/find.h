/*
 * (C) 2000-2003 	Gediminas Paulauskas <menesis@kabalak.net>
 * 			Fatih Demir <kabalak@kabalak.net>
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

/*
 * The generally used find function - now also with the same syntax like the
 *  replace functionality though separate.
 */
void gtranslator_find(GtkWidget *widget, gpointer target, gboolean find_in_comments,
	gboolean find_in_english, gboolean find_in_translation);

void gtranslator_update_regex_flags(void);

#endif
