/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "file-dialogs.h"
#include "tab.h"
#include "po.h"
#include "window.h"

/*
  * Store the given filename's directory for our file dialogs completion-routine.
 */
void
gtranslator_file_dialogs_store_directory(const gchar *filename)
{
	gchar *directory;

	g_return_if_fail(filename!=NULL);
	
	directory=g_path_get_dirname(filename);
	
//	gtranslator_config_set_string("informations/last_directory", directory);

	g_free(directory);
}

/*
 * File chooser dialog
 */
GtkWindow *
gtranslator_file_chooser_new (GtkWindow *parent,
			      FileselMode mode,
			      gchar *title)
{
	GtkWidget *dialog;
	GtkFileFilter *filter;
	
	dialog = gtk_file_chooser_dialog_new(title,
					     parent,
					     (mode == FILESEL_SAVE) ? GTK_FILE_CHOOSER_ACTION_SAVE : GTK_FILE_CHOOSER_ACTION_OPEN,
					     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					     (mode == FILESEL_SAVE) ? GTK_STOCK_SAVE : GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
					     NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog),GTK_RESPONSE_ACCEPT);
	if (mode != FILESEL_SAVE)
		{
			filter = gtk_file_filter_new();
			gtk_file_filter_set_name(filter,_("Gettext translation"));
			gtk_file_filter_add_mime_type(filter,"text/x-gettext-translation");
			gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog),filter);
	
			filter = gtk_file_filter_new();
			gtk_file_filter_set_name(filter,_("Gettext translation template"));
			gtk_file_filter_add_mime_type(filter,"text/x-gettext-translation-template");
			gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog),filter);
			
			filter = gtk_file_filter_new();
			gtk_file_filter_set_name(filter,_("All files"));
			gtk_file_filter_add_pattern(filter,"*");
			gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog),filter);
			
//			gtranslator_file_dialogs_set_directory(&dialog);
		} 
		
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent));
	gtk_widget_show_all(GTK_WIDGET(dialog));
	return GTK_WINDOW(dialog);
}
