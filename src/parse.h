/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
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

#ifndef GTR_PARSE_H
#define GTR_PARSE_H 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#include "gnome-regex.h"

#include "messages.h"
#include "header_stuff.h"

/*
 * The general po-file structure
 */
typedef struct {
	/*
	 * Absolute file name
	 */
	gchar *filename;
	/*
	 * The header 
	 */
	GtrHeader *header;
	/*
	 * All the po->messages are stored here 
	 */
	GList *messages;
	/*
	 * The length of messages list 
	 */
	guint length;
	/*
	 * A pointer to the currently displayed message 
	 */
	GList *current;
	/*
	 * The obsolete messages are stored within this gchar.
	 */
	gchar *obsolete;
	/*
	 * Marks if the file was changed; 
	 */
	guint file_changed : 1;
	/*
	 * Is the file write-permitted?
	 */
	gboolean no_write_perms;
	/*
	 * The translated entries. 
	 */
	guint translated;
	/*
	 * The fuzzy entries. 
	 */
	guint fuzzy;
} GtrPo;

#define GTR_PO(x) ((GtrPo *)x)

/*
 * The main variable for storing info about file 
 */
GtrPo *po;

/*
 * Marks if any file was opened 
 */
gboolean file_opened;

/*
 * Marks if the current message was changed; 
 */
gboolean message_changed;

/*
 * Core backend functions for parsing a po file.
 */ 
gboolean gtranslator_parse_core(void);
void gtranslator_po_free(void);
void gtranslator_parse(const gchar *po);

/*
 * The main granslator parse function which does use the backend
 *  calls from above and also sets up the GUI.
 */
void gtranslator_parse_main(const gchar *po);

/*
 * Callbacks for the widgets
 */
void parse_the_file(GtkWidget  * widget, gpointer of_dlg);
void gtranslator_save_file_dialog(GtkWidget  * widget, gpointer sfa_dlg);
void gtranslator_save_current_file_dialog(GtkWidget  * widget, gpointer useless);
void gtranslator_file_revert(GtkWidget  * widget, gpointer useless);
void gtranslator_file_close(GtkWidget  * widget, gpointer useless);

/*
 * The compile function.
 */
void compile(GtkWidget  * widget, gpointer useless);

/*
 * The update function (now outsources to update.c).
 */
void update(GtkWidget *widget, gpointer useless);

/*
 * Set the progress bar status/determine the po file status.
 */
void gtranslator_set_progress_bar(void);
void gtranslator_get_translated_count(void);

/*
 * A cache for saving regexps 
 */
GnomeRegexCache *rxc;

#endif
