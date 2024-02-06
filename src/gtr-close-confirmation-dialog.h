/*
 * gtr-close-confirmation-dialog.h
 * This file is part of gtranslator
 *
 * Copyright (C) 2004-2005 GNOME Foundation
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
 */

/*
 * Modified by the gedit Team, 2004-2005. See the AUTHORS file for a
 * list of people on the gtr Team.
 * See the ChangeLog files for a list of changes.
 */

#pragma once

#include <glib.h>
#include <gtk/gtk.h>
#include <adwaita.h>

#include "gtr-po.h"

G_BEGIN_DECLS
#define GTR_TYPE_CLOSE_CONFIRMATION_DIALOG  (gtr_close_confirmation_dialog_get_type ())

G_DECLARE_FINAL_TYPE (GtrCloseConfirmationDialog, gtr_close_confirmation_dialog, GTR, CLOSE_CONFIRMATION_DIALOG, AdwAlertDialog)

GtkWidget *gtr_close_confirmation_dialog_new        (GList * unsaved_documents, gboolean logout_mode);
GtkWidget *gtr_close_confirmation_dialog_new_single (GtrPo * doc, gboolean logout_mode);

const GList * gtr_close_confirmation_dialog_get_unsaved_documents (GtrCloseConfirmationDialog * dlg);
GList * gtr_close_confirmation_dialog_get_selected_documents (GtrCloseConfirmationDialog * dlg);

G_END_DECLS
