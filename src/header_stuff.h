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

#ifndef GTR_HEADER_STUFF_H
#define GTR_HEADER_STUFF_H 1

#include <gtk/gtkwidget.h>
#include "message.h"

/*
 * The header-structure which should hold the header information
 */
typedef struct {
	gchar *comment;
	gchar *prj_name;
	gchar *prj_version;
	gchar *report_message_bugs_to;
	gchar *pot_date;
	gchar *po_date;
	gchar *translator;
	gchar *tr_email;
	gchar *language;
	gchar *lg_email;
	gchar *mime_version;
	gchar *charset;
	gchar *encoding;

	gchar *plural_forms;

	/*
	 * Absolutely depreciated by me, but as it's going on my
	 *  nerves, I'm adding this to our supported tags.
	 */
	gchar *generator; 
	
	/*
	 * This atribute is used to save the headers that other
	 * translation aplications can write.
	 */
	gchar *other;
} GtrHeader;

#define GTR_HEADER(x) ((GtrHeader *)x)

/*
 * These converts header between GtrMsg and GtrHeader.
 */
GtrMsg  * gtranslator_header_put(GtrHeader  * h);
GtrHeader  * gtranslator_header_get(GtrMsg  * msg);

/*
 * Updates PO-Revision-Date and Last-Translator fields 
 */
void gtranslator_header_update(GtrHeader  * h);

/*
 * Frees the header structure
 */
void gtranslator_header_free(GtrHeader  * the_header);

/*
 * Creates and shows the dialog for editing the header
 */
void gtranslator_header_edit_dialog(GtkWidget  * widget, gpointer useless);

/*
 * View <-> real form transition.
 */ 
gchar *gtranslator_header_comment_convert_for_view(gchar *comment);
gchar *gtranslator_header_comment_convert_for_save(gchar *comment);

/*
 * Fill up some fields of the header if possible.
 */
gboolean gtranslator_header_fill_up(GtrHeader *header);

/*
 * Create new header with good values
 */
GtrHeader * gtranslator_header_create_from_prefs(void);

#endif
