/*
 * (C) 2000-2003 	Fatih Demir <kabalak@kabalak.net>
 *			Gediminas Paulauskas <menesis@kabalak.net>
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

#include <stdio.h>
#include <gtk/gtkwidget.h>

#include "header_stuff.h"

typedef enum
{
	GTR_PARSER_ERROR_GETTEXT,
	GTR_PARSER_ERROR_FILENAME,
	GTR_PARSER_ERROR_OTHER,
} GtrParserError;

#define GTR_PARSER_ERROR gtranslator_parser_error_quark()
GQuark gtranslator_parser_error_quark (void);


/*
 * The general po-file structure
 */
typedef struct {
	/*
	 * Absolute file name
	 */
	gchar *filename;
	
	/*
	 * Gettext's file handle
	 */
	po_file_t gettext_po_file;

	/*
	 * The message domains in this file
	 */
	GList *domains;
	
	/*
	 * Parsed list of GtrMsgs for the current domains' messagelist
	 */
	GList *messages;
	
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
	 * Is the file write-permitted? (read-only)
	 */
	gboolean no_write_perms;

	/*
	 * Translated entries count
	 */
	guint translated;
	
	/*
	 * Fuzzy entries count
	 */
	guint fuzzy;

} GtrPo;

#define GTR_PO(x) ((GtrPo *)x)

/*
 * The main global PO file reference. Eventually, I hope gtranslator
 * becomes an MDI application, like gedit, and this will either become
 * redundant or will point to the current PO file being editted.
 */
extern GtrPo *po;

/*
 * Parse the given po file, and return a new document handle. Set error
 * on failure.
 */
GtrPo *gtranslator_parse(const gchar *filename, GError **error);

/* We really only want ONE parser function */
#ifdef REDUNDANT
gboolean gtranslator_parse_core(GtrPo *po);
#endif

/*
 * Parse the file, and then trigger the setting up of the GUI.
 */
gboolean gtranslator_parse_main(const gchar *po, GError **error);

/*
 * Save the file with the given filename. Set error on failure.
 */
gboolean gtranslator_save_file(const gchar *name, GError **error);

/*
 * The parts that sets up the GUI after it's been parsed
 */
void gtranslator_parse_set_gui_from(GtrPo *po);

/*
 * Callbacks for the widgets
 */
void gtranslator_parse_the_file_from_file_dialog(GtkWidget  * widget, gpointer of_dlg);
void gtranslator_save_file_dialog(GtkWidget  * widget, gpointer sfa_dlg);
void gtranslator_save_current_file_dialog(GtkWidget  * widget, gpointer useless);
void gtranslator_file_revert(GtkWidget  * widget, gpointer useless);
void gtranslator_file_close(GtkWidget  * widget, gpointer useless);

/*
 * Remove all translations from the po file.
 */
void gtranslator_remove_all_translations(GtrPo *po);

/*
 * Return the number of translated messages in a PO file
 */
void gtranslator_update_translated_count(GtrPo *po);

/*
 * Finished with a po file
 */
void gtranslator_po_free(GtrPo *po);

#endif
