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

#ifndef GTR_PO_H
#define GTR_PO_H 1

#include "header_stuff.h"

/*
 * The general function type for all direct file operations.
 */
typedef void	(* GtrFileOpFunction)	(gchar *filename);

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

	/*
	 * Are we editing an UTF-8 po file?
	 */
	gboolean utf8;
	
	/*
	 * Determine whether we've got a really opened GtrPo file.
	 */
	gboolean file_opened;
	
	/*
	 * Another internal helping variable for determining whether we've got
	 *  changed something internally.
	 */
	gboolean current_message_changed;

	/*
	 * The locale charset name.
	 */
	gchar *locale_charset;

	/*
	 * The now GtrPo binded saving, opening etc. functions.
	 */
	GtrFileOpFunction open; 
	GtrFileOpFunction save;
	GtrFileOpFunction save_as;
	
	GVoidFunc compile;
	GVoidFunc close;
} GtrPo;

#define GTR_PO(x) ((GtrPo *) x)

/*
 * Create a quite "empty" GtrPo for data-coverage issues.
 */
GtrPo *gtranslator_po_new();

/*
 * Read the given po file really into the GtrPo structure.
 */
GtrPo *gtranslator_po_new_from_file(gchar *filename);

/*
 * Utility functions which are really only existing for more usefulness and
 *  sanity throughout the code.
 */
void gtranslator_po_compile(GtrPo *po);
void gtranslator_po_open(GtrPo *po, gchar *filename);
void gtranslator_po_save(GtrPo *po);
void gtranslator_po_save_as(GtrPo *po, gchar *filename);
void gtranslator_po_close(GtrPo *po);

/*
 * Free the GtrPo structure.
 */
void gtranslator_po_free(GtrPo *po);

#endif
