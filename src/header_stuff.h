/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gtranslator.net>
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

#ifndef GTR_HEADER_STUFF_H
#define GTR_HEADER_STUFF_H 1

#include <gtk/gtkwidget.h>
#include "messages.h"

/*
 * The header-structure which should hold the header information
 */
typedef struct {
	gchar *comment;
	gchar *prj_name;
	gchar *prj_version;
	gchar *pot_date;
	gchar *po_date;
	gchar *translator;
	gchar *tr_email;
	gchar *language;
	gchar *lg_email;
	gchar *mime_version;
	gchar *charset;
	gchar *encoding;
} GtrHeader;

#define GTR_HEADER(x) ((GtrHeader *)x)

/*
 * These converts header between GtrMsg and GtrHeader.
 */
GtrMsg  * put_header(GtrHeader  * h);
GtrHeader  * get_header(GtrMsg  * msg);

/*
 * Updates PO-Revision-Date and Last-Translator fields 
 */
void update_header(GtrHeader  * h);

/*
 * Frees the header structure
 */
void free_header(GtrHeader  * the_header);

/*
 * Creates and shows the dialog for editing the header
 */
void edit_header(GtkWidget  * widget, gpointer useless);

#endif
