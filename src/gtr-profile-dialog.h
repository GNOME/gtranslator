/*
 * Copyright (C) 2007  Pablo Sanxiao <psanxiao@gmail.com>
 *               2008  Igalia 
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
 *   Pablo Sanxiao <psanxiao@gmail.com> 
 */

#ifndef __PROFILE_DIALOG_H__
#define __PROFILE_DIALOG_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "gtr-profile.h"

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_PROFILE_DIALOG		(gtr_profile_dialog_get_type ())
#define GTR_PROFILE_DIALOG(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_PROFILE_DIALOG, GtrProfileDialog))
#define GTR_PROFILE_DIALOG_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_PROFILE_DIALOG, GtrProfileDialogClass))
#define GTR_IS_PROFILE_DIALOG(o)	(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_PROFILE_DIALOG))
#define GTR_IS_PROFILE_DIALOG_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_PROFILE_DIALOG))
#define GTR_PROFILE_DIALOG_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_PROFILE_DIALOG, GtrProfileDialogClass))

/* Private structure type */
typedef struct _GtrProfileDialogPrivate GtrProfileDialogPrivate;

/*
 * Main object structure
 */
typedef struct _GtrProfileDialog GtrProfileDialog;

struct _GtrProfileDialog
{
  GtkDialog parent_instance;

  /*< private > */
  GtrProfileDialogPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtrProfileDialogClass GtrProfileDialogClass;

struct _GtrProfileDialogClass
{
  GtkDialogClass parent_class;
};

/*
 * Public methods
 */
GType               gtr_profile_dialog_get_type         (void) G_GNUC_CONST;

GtrProfileDialog   *gtr_profile_dialog_new              (GtkWidget  *parent,
                                                         GtrProfile *profile);

GtrProfile         *gtr_profile_dialog_get_profile      (GtrProfileDialog *dlg);

G_END_DECLS

#endif
