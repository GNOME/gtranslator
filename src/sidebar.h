/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
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

#ifndef GTR_SIDEBAR_H
#define GTR_SIDEBAR_H 1

#include "parse.h"

/*
 * Creates the shortcut bar and returns it as a GtkWidget.
 */
GtkWidget *gtranslator_sidebar_new(void);

/*
 * Adds the given po file to our sidebar.
 */ 
void gtranslator_sidebar_activate_views(void);

/*
 * Clean the sidebar. Remove all the nice icons for the views.
 */
void gtranslator_sidebar_clear_views(void);

/*
 * Hide/Show the sidebar and return if the action was successful.
 */
gboolean gtranslator_sidebar_hide(void);
gboolean gtranslator_sidebar_show(void);

/*
 * Should avtivate the hide/show functions for the sidebar.
 */
void gtranslator_sidebar_toggle(void);

#endif
