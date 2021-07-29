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

#ifndef __UPLOAD_DIALOG_H__
#define __UPLOAD_DIALOG_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_UPLOAD_DIALOG          (gtr_upload_dialog_get_type ())
#define GTR_UPLOAD_DIALOG(o)            (G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_UPLOAD_DIALOG, GtrUploadDialog))
#define GTR_UPLOAD_DIALOG_CLASS(k)      (G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_UPLOAD_DIALOG, GtrUploadDialogClass))
#define GTR_IS_UPLOAD_DIALOG(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_UPLOAD_DIALOG))
#define GTR_IS_UPLOAD_DIALOG_CLASS(k)   (G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_UPLOAD_DIALOG))
#define GTR_UPLOAD_DIALOG_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_UPLOAD_DIALOG, GtrUploadDialogClass))

/*
 * Main object structure
 */
typedef struct _GtrUploadDialog GtrUploadDialog;

struct _GtrUploadDialog
{
  GtkDialog parent_instance;
};

/*
 * Class definition
 */
typedef struct _GtrUploadDialogClass GtrUploadDialogClass;

struct _GtrUploadDialogClass
{
  GtkDialogClass parent_class;
};

/*
 * Public methods
 */
GType              gtr_upload_dialog_get_type         (void) G_GNUC_CONST;

GtrUploadDialog   *gtr_upload_dialog_new              (GtkWidget *parent);

char              *gtr_upload_dialog_get_comment      (GtrUploadDialog *dlg);
void               gtr_upload_dialog_set_loading      (GtrUploadDialog *dlg,
                                                       gboolean loading);

G_END_DECLS

#endif
