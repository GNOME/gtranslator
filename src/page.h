/*
 * (C) 2000-2007 	Fatih Demir <kabalak@kabalak.net>
 *			Ross Golder <ross@golder.org>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Peeter Vois <peeter@kabalak.net>
 * 			Ignacio Casal <nacho.resa@gmail.com>
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
#include "messages-table.h"

/*
 * This class encapsulated all the widgets related to a PO file
 */
typedef struct 
{
	GtrPo *po;
	
	GtkWidget *content_pane;

	GtrMessagesTable *messages_table;
	
	GtkWidget *comment;
	GtkWidget *edit_comment;

	/*Original text*/
	GtkWidget *text_notebook;
	GtkWidget *text_msgid;
	GtkWidget *text_msgid_plural;
	
	/*Translated text*/
	GtkWidget *trans_notebook;
	GtkWidget *trans_msgstr;
	GtkWidget *trans_msgstr_plural[6];
	
    	/*Pane widgets*/
	GtkWidget *table_pane;
    	GtkWidget *combobox_pane;
	
	/*Status widgets*/
	GtkWidget *translated;
	GtkWidget *fuzzy;
	GtkWidget *untranslated;
	
} GtrPage;


/*
 * A pointer to the current page
 */
extern GtrPage *current_page;

/*
 * Create a page to display a po file and return the widget
 */
void gtranslator_page_new(GtrPo *po);

/*
 * Show/hide the messages table
 */
void gtranslator_page_show_messages_table(GtrPage *page);
void gtranslator_page_hide_messages_table(GtrPage *page);

/*
 * Called at autosave time
 */
gboolean gtranslator_page_autosave(GtrPage *page);

/*
 * Callback for when a translation is updated
 */
void gtranslator_page_dirty(GtkTextBuffer *textbuffer, gpointer user_data);

#endif
