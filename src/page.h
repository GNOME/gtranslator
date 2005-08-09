/*
 * (C) 2000-2004 	Fatih Demir <kabalak@kabalak.net>
 *			Ross Golder <ross@golder.org>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Peeter Vois <peeter@kabalak.net>
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

#ifndef GTR_PAGE_H
#define GTR_PAGE_H 1

#include "gui.h"
#include "parse.h"

/*
 * This class encapsulated all the widgets related to a PO file
 */
typedef struct {
	/* Page number wrt notebook_widget */
	gint num;
	
	GtrPo *po;

	GtkWidget *content_pane;

	GtkWidget *messages_table;
	GtkWidget *messages_tree;
	
	GtkWidget *comment;
	GtkWidget *edit_button;

	GtkWidget *text_vbox;
	GtkWidget *text_msgid;
	GtkWidget *text_msgid_plural;
	GtkWidget *trans_vbox;
	GtkWidget *trans_msgstr[16];

	GtkWidget *table_pane;
} GtrPage;

/*
 * The currently active pages
 */
extern GList *pages;

/*
 * A pointer to the current page
 */
extern GtrPage *current_page;

/*
 * Create a page to display a po file and return the widget
 */
GtrPage* gtranslator_page_new(GtrPo *po);

/*
 * Show/hide the messages table
 */
void gtranslator_page_messages_table_show(GtrPage *page);
void gtranslator_page_messages_table_hide(GtrPage *page);

#endif
