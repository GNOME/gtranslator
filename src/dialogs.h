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

#ifndef GTR_DIALOGS_H
#define GTR_DIALOGS_H 1

#include <stdio.h>
#include <gtk/gtkwidget.h>

#define gtranslator_raise_dialog(dlg)\
	if (dlg) {\
		gdk_window_show(dlg->window);\
		gdk_window_raise(dlg->window);\
		return;\
	}

/*
 * Sets window name/class, icon for it, connects the destroy signal,
 * and shows it. The dialog should be static or global.
 */
void gtranslator_dialog_show(GtkWidget ** dlg, const gchar * wmname);

/*
 * The file open/save (as) file dialogs.
 */
void gtranslator_save_file_as_dialog(GtkWidget * widget, gpointer useless);
void gtranslator_open_file_dialog(GtkWidget * widget, gpointer useless);
gboolean gtranslator_should_the_file_be_saved_dialog(void);

/*
 * This dialog will pop up if a crash recovery file could be found
 *  in ~.
 */
void gtranslator_rescue_file_dialog(void);

/*
 * Set up an query dialog for the query backend -- search for strings
 *  in compiled mo/gmo files in the specific locale directory.
 */
void gtranslator_query_dialog(void);

/*
 * A last questioning dialog before we do accomplish all the missing
 *  strings from the default query domain.
 */
void gtranslator_auto_accomplishment_dialog(void);

/*
 * A replace dialog based onto the find functions.
 */
void gtranslator_replace_dialog(GtkWidget *widget, gpointer useless);

/*
 * The "Go to" dialog.
 */
void gtranslator_go_to_dialog(GtkWidget * widget, gpointer useless);

/*
 * The find dialog function. 
 */
void gtranslator_find_dialog(GtkWidget * widget, gpointer useless);

void gtranslator_compile_error_dialog(FILE * fs);
void gtranslator_file_is_not_changed_dialog(GtkWidget * widget, gpointer useless);

/*
 * Open URI dialog.
 */
void gtranslator_open_uri_dialog(GtkWidget *widget, gpointer useless);

#endif
