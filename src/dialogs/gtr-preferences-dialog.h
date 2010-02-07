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

#ifndef __PREFERENCES_DIALOG_H__
#define __PREFERENCES_DIALOG_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "gtr-application.h"
#include "gtr-window.h"

enum
{
  PROFILE_NAME_COL,
  TOGGLE_COL,
  N_COLUMNS_PROFILES
};

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_PREFERENCES_DIALOG		(gtr_preferences_dialog_get_type ())
#define GTR_PREFERENCES_DIALOG(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_PREFERENCES_DIALOG, GtrPreferencesDialog))
#define GTR_PREFERENCES_DIALOG_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_PREFERENCES_DIALOG, GtrPreferencesDialogClass))
#define GTR_IS_PREFERENCES_DIALOG(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_PREFERENCES_DIALOG))
#define GTR_IS_PREFERENCES_DIALOG_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_PREFERENCES_DIALOG))
#define GTR_PREFERENCES_DIALOG_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_PREFERENCES_DIALOG, GtrPreferencesDialogClass))
/* Private structure type */
typedef struct _GtrPreferencesDialogPrivate GtrPreferencesDialogPrivate;

/*
 * Main object structure
 */
typedef struct _GtrPreferencesDialog GtrPreferencesDialog;

struct _GtrPreferencesDialog
{
  GtkDialog parent_instance;

  /*< private > */
  GtrPreferencesDialogPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtrPreferencesDialogClass GtrPreferencesDialogClass;

struct _GtrPreferencesDialogClass
{
  GtkDialogClass parent_class;
};

/*
 * Public methods
 */
GType
gtr_preferences_dialog_get_type (void)
  G_GNUC_CONST;

     GType gtr_preferences_dialog_register_type (GTypeModule * module);

     void gtr_show_preferences_dialog (GtrWindow * window);

GtkWidget * gtr_preferences_dialog_get_treeview (GtrPreferencesDialog * dlg);

     void
       gtr_preferences_fill_profile_treeview
       (GtrPreferencesDialog * dlg, GtkTreeModel * model);

G_END_DECLS
#endif /* __PREFERENCES_DIALOG_H__ */
