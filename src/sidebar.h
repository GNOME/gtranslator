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

#ifndef GTR_SHORTCUT_BAR_H
#define GTR_SHORTCUT_BAR_H 1

#include "parse.h"
#include <gal/shortcut-bar/e-shortcut-bar.h>

/*
 * The shortcut bar releated widgets/variables/methods.
 */ 

EShortcutModel	*model;

/*
 * Creates the shortcut bar and returns it as a GtkWidget.
 */
GtkWidget *gtranslator_sidebar_new(void);

/*
 * Adds the given po file to our sidebar.
 */ 
void gtranslator_sidebar_add_po(GtrPo *po);

/*
 * Clean the sidebar. Remove all the nice icons for the views.
 */
void gtranslator_sidebar_clear(void);

/*
 * Show the comment of the current message.
 */
void show_comment(GtkWidget *text);

#endif
