/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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

#ifndef __JUMP_DIALOG_H__
#define __JUMP_DIALOG_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "gtr-window.h"

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_JUMP_DIALOG		(gtranslator_jump_dialog_get_type ())
#define GTR_JUMP_DIALOG(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_JUMP_DIALOG, GtranslatorJumpDialog))
#define GTR_JUMP_DIALOG_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_JUMP_DIALOG, GtranslatorJumpDialogClass))
#define GTR_IS_JUMP_DIALOG(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_JUMP_DIALOG))
#define GTR_IS_JUMP_DIALOG_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_JUMP_DIALOG))
#define GTR_JUMP_DIALOG_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_JUMP_DIALOG, GtranslatorJumpDialogClass))
/* Private structure type */
typedef struct _GtranslatorJumpDialogPrivate GtranslatorJumpDialogPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorJumpDialog GtranslatorJumpDialog;

struct _GtranslatorJumpDialog
{
  GtkDialog parent_instance;

  /*< private > */
  GtranslatorJumpDialogPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorJumpDialogClass GtranslatorJumpDialogClass;

struct _GtranslatorJumpDialogClass
{
  GtkDialogClass parent_class;
};

/*
 * Public methods
 */
GType
gtranslator_jump_dialog_get_type (void)
  G_GNUC_CONST;

     GType gtranslator_jump_dialog_register_type (GTypeModule * module);

     void gtranslator_show_jump_dialog (GtranslatorWindow * window);

G_END_DECLS
#endif /* __JUMP_DIALOG_H__ */
