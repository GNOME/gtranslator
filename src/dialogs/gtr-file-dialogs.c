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

#include "gtr-file-dialogs.h"
#include "gtr-tab.h"
#include "gtr-po.h"
#include "gtr-window.h"

/*
 * File chooser dialog
 */
GtkWidget *
gtranslator_file_chooser_new (GtkWindow * parent,
			      FileselMode mode,
			      const gchar * title, const gchar * dir)
{
  GtkWidget *dialog;
  GtkFileFilter *filter;

  dialog = gtk_file_chooser_dialog_new (title,
					parent,
					(mode ==
					 FILESEL_SAVE) ?
					GTK_FILE_CHOOSER_ACTION_SAVE :
					GTK_FILE_CHOOSER_ACTION_OPEN,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					(mode ==
					 FILESEL_SAVE) ? GTK_STOCK_SAVE :
					GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
					NULL);
  gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_ACCEPT);

  if (dir)
    gtk_file_chooser_set_current_folder_uri (GTK_FILE_CHOOSER (dialog), dir);

  if (mode != FILESEL_SAVE)
    {
      /* We set a multi selection dialog */
      gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (dialog), TRUE);

      /* Now we set the filters */
      filter = gtk_file_filter_new ();
      gtk_file_filter_set_name (filter, _("Gettext translation"));
#ifndef G_OS_WIN32
      gtk_file_filter_add_mime_type (filter, "text/x-gettext-translation");
#else
      gtk_file_filter_add_pattern (filter, "*.po");
#endif
      gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filter);

      filter = gtk_file_filter_new ();
      gtk_file_filter_set_name (filter, _("Gettext translation template"));
      gtk_file_filter_add_pattern (filter, "*.pot");
      gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filter);

      filter = gtk_file_filter_new ();
      gtk_file_filter_set_name (filter, _("All files"));
      gtk_file_filter_add_pattern (filter, "*");
      gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filter);
    }

  gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (parent));

  return dialog;
}
