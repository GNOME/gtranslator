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
GtkFileDialog *
gtr_file_chooser_new (GtkWindow * parent,
                      FileselMode mode,
                      const gchar * title, const gchar * dir)
{
  GtkFileDialog *dialog;

  dialog = gtk_file_dialog_new ();
  gtk_file_dialog_set_title (dialog, title);
  gtk_file_dialog_set_modal (dialog, TRUE);

  if (dir)
  {
    g_autoptr (GFile) file = g_file_new_for_uri (dir);
    gtk_file_dialog_set_initial_folder (dialog, file);
  }

  if (mode != FILESEL_SAVE)
    {
      g_autoptr (GListStore) filters = g_list_store_new (GTK_TYPE_FILE_FILTER);
      g_autoptr (GtkFileFilter) po = NULL;
      g_autoptr (GtkFileFilter) pot = NULL;
      g_autoptr (GtkFileFilter) all = NULL;
      /* Now we set the filters */
      // FIXME: review if we need to free the filters... maybe there's a memory leak
      po = gtk_file_filter_new ();
      gtk_file_filter_set_name (po, _("Gettext translation"));
#ifndef G_OS_WIN32
      gtk_file_filter_add_mime_type (po, "text/x-gettext-translation");
#else
      gtk_file_filter_add_pattern (po, "*.po");
#endif

      pot = gtk_file_filter_new ();
      gtk_file_filter_set_name (pot, _("Gettext translation template"));
      gtk_file_filter_add_pattern (pot, "*.pot");

      all = gtk_file_filter_new ();
      gtk_file_filter_set_name (all, _("All files"));
      gtk_file_filter_add_pattern (all, "*");

      g_list_store_append (filters, po);
      g_list_store_append (filters, pot);
      g_list_store_append (filters, all);
      gtk_file_dialog_set_filters (dialog, G_LIST_MODEL (filters));
    }

  return dialog;
}
