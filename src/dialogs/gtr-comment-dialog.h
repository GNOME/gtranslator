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

#ifndef __COMMENT_DIALOG_H__
#define __COMMENT_DIALOG_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "gtr-window.h"
#include "gtr-msg.h"

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_COMMENT_DIALOG		(gtranslator_comment_dialog_get_type ())
#define GTR_COMMENT_DIALOG(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_COMMENT_DIALOG, GtrCommentDialog))
#define GTR_COMMENT_DIALOG_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_COMMENT_DIALOG, GtrCommentDialogClass))
#define GTR_IS_COMMENT_DIALOG(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_COMMENT_DIALOG))
#define GTR_IS_COMMENT_DIALOG_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_COMMENT_DIALOG))
#define GTR_COMMENT_DIALOG_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_COMMENT_DIALOG, GtrCommentDialogClass))
/* Private structure type */
typedef struct _GtrCommentDialogPrivate
  GtrCommentDialogPrivate;

/*
 * Main object structure
 */
typedef struct _GtrCommentDialog GtrCommentDialog;

struct _GtrCommentDialog
{
  GtkDialog parent_instance;

  /*< private > */
  GtrCommentDialogPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtrCommentDialogClass GtrCommentDialogClass;

struct _GtrCommentDialogClass
{
  GtkDialogClass parent_class;
};

/*
 * Public methods
 */
GType
gtranslator_comment_dialog_get_type (void)
  G_GNUC_CONST;

     GType gtranslator_comment_dialog_register_type (GTypeModule * module);

     void gtranslator_show_comment_dialog (GtrWindow * window);

G_END_DECLS
#endif /* __COMMENT_DIALOG_H__ */
