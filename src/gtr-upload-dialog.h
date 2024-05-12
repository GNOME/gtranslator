/*
 * Copyright (C) 2021  Daniel García <danigm@gnome.org>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Daniel García <danigm@gnome.org>
 */

#pragma once

#include <adwaita.h>
#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTR_TYPE_UPLOAD_DIALOG          (gtr_upload_dialog_get_type ())

G_DECLARE_FINAL_TYPE (GtrUploadDialog, gtr_upload_dialog, GTR, UPLOAD_DIALOG,
                      AdwDialog)

/*
 * Public methods
 */
GtrUploadDialog   *gtr_upload_dialog_new              (GtkWidget *parent);

char              *gtr_upload_dialog_get_comment      (GtrUploadDialog *dlg);
void               gtr_upload_dialog_set_loading      (GtrUploadDialog *dlg,
                                                       gboolean loading);
GtkWidget         *gtr_upload_dialog_get_parent       (GtrUploadDialog *dlg);

G_END_DECLS
