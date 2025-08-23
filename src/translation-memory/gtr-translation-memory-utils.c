/*
 * Copyright (C) 2001  Fatih Demir <kabalak@kabalak.net>
 *               2012  Ignacio Casal Quinteiro <icq@gnome.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Ignacio Casal Quinteiro <icq@gnome.org>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-translation-memory-utils.h"

#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>


/**
 * gtr_scan_dir:
 * @dir: the dir to parse
 * @list: the list where to store the GFiles
 * @po_name: the name of the specific po file to search or NULL.
 *
 * Scans the directory and subdirectories of @dir looking for filenames remained
 * with .po or files that matches @po_name. @list must be freed with
 * g_slist_free_full (list, g_object_unref).
 */
void
gtr_scan_dir (GFile * dir, GSList ** list, const gchar * po_name)
{
  GFileInfo *info;
  GError *error;
  GFileEnumerator *enumerator;

  error = NULL;
  enumerator = g_file_enumerate_children (dir,
                                          G_FILE_ATTRIBUTE_STANDARD_NAME,
                                          G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                          NULL, &error);
  if (error)
    {
      if (!g_error_matches (error, G_IO_ERROR, G_IO_ERROR_NOT_DIRECTORY))
        g_warning ("Could not enumerate files %s", error->message);
      g_clear_error (&error);
    }
  else
    {
      while ((info =
              g_file_enumerator_next_file (enumerator, NULL, &error)) != NULL)
        {
          g_autoptr (GFile) file = NULL;
          const gchar *name;
          g_autofree char *filename = NULL;

          name = g_file_info_get_name (info);
          file = g_file_get_child (dir, name);

          if (po_name != NULL)
            {
              if (g_str_has_suffix (po_name, ".po"))
                filename = g_strdup (po_name);
              else
                filename = g_strconcat (po_name, ".po", NULL);
            }
          else
            filename = g_strdup (".po");

          if (g_str_has_suffix (name, filename))
            *list = g_slist_prepend (*list, g_steal_pointer (&file));
          else if (g_file_info_get_file_type (info) != G_FILE_TYPE_DIRECTORY)
            gtr_scan_dir (file, list, po_name);

          g_clear_object (&info);
        }
      g_file_enumerator_close (enumerator, NULL, NULL);
      g_object_unref (enumerator);

      if (error)
        {
          g_warning ("%s", error->message);
          g_clear_error (&error);
        }
    }
}

