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

#ifndef GTR_PREFS_DIALOG_H
#define GTR_PREFS_DIALOG_H 1

#include <gtk/gtkwidget.h>

/*
 * This is the new gtranslator own preferences dialog type. Encapsulates some
 *  neat things about GnomePropertyBox/EPaned etc.
 */
typedef struct
{
	GtkWidget	*dialog;
	GtkWidget	*pane;

	GtkWidget	*sections_list;

	GList		*sections;
	GList		*widgets;

	gboolean	 changed;
	gboolean	 shown;
	
	gint		 changes;

	gpointer	 content;

	GVoidFunc	 read_all_options_function;
} GtrPrefsDialog;

#define GTR_PREFS_DIALOG(x) ((GtrPrefsDialog *) x)

/*
 * Creates a new preferences dialog with all the nice contents and values from
 *  the preferences -- the supplied "read_all_options_func" should read all of
 *   the options from the widget and should perform further steps.
 */
GtrPrefsDialog *gtranslator_prefs_dialog_new(GVoidFunc read_all_options_func);

/*
 * Append the given preferences page to the preferences dialog.
 */
void gtranslator_prefs_dialog_append_page(GtrPrefsDialog *dialog,
	gchar *section_name, GtkWidget *section_table);

/*
 * Shows/hides the given preferences dialog -- simple handlers for now to wrap
 *  the enormous macro length calls .-)
 */
void gtranslator_prefs_dialog_show(GtrPrefsDialog *dialog);
void gtranslator_prefs_dialog_hide(GtrPrefsDialog *dialog);

/*
 * Handles/set the changed status -- plays with the buttons, variables are
 *  also handled here.
 */
void gtranslator_prefs_dialog_changed(GtrPrefsDialog *dialog);
gboolean gtranslator_prefs_dialog_is_changed(GtrPrefsDialog *dialog);

/*
 * Closes and free's the prefs dialog.
 */
void gtranslator_prefs_dialog_close(GtrPrefsDialog *dialog);

#endif
