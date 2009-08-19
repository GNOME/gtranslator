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

#include "application.h"
#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include "window.h"

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
#define GTR_TYPE_PREFERENCES_DIALOG		(gtranslator_preferences_dialog_get_type ())
#define GTR_PREFERENCES_DIALOG(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_PREFERENCES_DIALOG, GtranslatorPreferencesDialog))
#define GTR_PREFERENCES_DIALOG_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_PREFERENCES_DIALOG, GtranslatorPreferencesDialogClass))
#define GTR_IS_PREFERENCES_DIALOG(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_PREFERENCES_DIALOG))
#define GTR_IS_PREFERENCES_DIALOG_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_PREFERENCES_DIALOG))
#define GTR_PREFERENCES_DIALOG_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_PREFERENCES_DIALOG, GtranslatorPreferencesDialogClass))
/* Private structure type */
typedef struct _GtranslatorPreferencesDialogPrivate
  GtranslatorPreferencesDialogPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorPreferencesDialog GtranslatorPreferencesDialog;

struct _GtranslatorPreferencesDialog
{
  GtkDialog parent_instance;

  /*< private > */
  GtranslatorPreferencesDialogPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorPreferencesDialogClass
  GtranslatorPreferencesDialogClass;

struct _GtranslatorPreferencesDialogClass
{
  GtkDialogClass parent_class;
};

/*
 * Public methods
 */
GType
gtranslator_preferences_dialog_get_type (void)
  G_GNUC_CONST;

     GType gtranslator_preferences_dialog_register_type (GTypeModule *
							 module);

     void gtranslator_show_preferences_dialog (GtranslatorWindow * window);

     GtkWidget
       *gtranslator_preferences_dialog_get_treeview
       (GtranslatorPreferencesDialog * dlg);

     void
       gtranslator_preferences_fill_profile_treeview
       (GtranslatorPreferencesDialog * dlg, GtkTreeModel * model);

G_END_DECLS
#endif /* __PREFERENCES_DIALOG_H__ */
