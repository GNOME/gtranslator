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

#ifndef GTR_GTR_DIALOGS_H
#define GTR_GTR_DIALOGS_H 1

#include "prefs.h"
#include "gui.h"
#include "parse.h"

#define raise_and_return_if_exists(dlg)\
	if (dlg) {\
		gdk_window_show(dlg->window);\
		gdk_window_raise(dlg->window);\
		return;\
	}

/*
 * Sets window name/class, icon for it, connects the destroy signal,
 * and shows it. The dialog should be static or global.
 */
void show_nice_dialog(GtkWidget ** dlg, const gchar * wmname);

/*
 * The file open/save (as) file dialogs.
 */
void save_file_as(GtkWidget * widget, gpointer useless);
void open_file(GtkWidget * widget, gpointer useless);
gboolean ask_to_save_file(void);

/*
 * The "Go to" dialog.
 */
void goto_dlg(GtkWidget * widget, gpointer useless);

/*
 * The find dialog function. 
 */
void find_dialog(GtkWidget * widget, gpointer useless);

void compile_error_dialog(FILE * fs);
void no_changed_dialog(GtkWidget * widget, gpointer useless);

/*
 * Open URI dialog.
 */
void open_uri_dialog(GtkWidget *widget, gpointer useless);

#endif
