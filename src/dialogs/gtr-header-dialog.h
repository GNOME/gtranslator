/*
 * Copyright (C) 2007  Pablo Sanxiao <psanxiao@gmail.com>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 2 of the License, or
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

#ifndef __HEADER_DIALOG_H__
#define __HEADER_DIALOG_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "gtr-window.h"


/*
 * Type checking and casting macros
 */
#define GTR_TYPE_HEADER_DIALOG		(gtr_header_dialog_get_type ())
#define GTR_HEADER_DIALOG(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_HEADER_DIALOG, GtrHeaderDialog))
#define GTR_HEADER_DIALOG_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_HEADER_DIALOG, GtrHeaderDialogClass))
#define GTR_IS_HEADER_DIALOG(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_HEADER_DIALOG))
#define GTR_IS_HEADER_DIALOG_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_HEADER_DIALOG))
#define GTR_HEADER_DIALOG_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_HEADER_DIALOG, GtrHeaderDialogClass))

/* Private structure type */
typedef struct _GtrHeaderDialogPrivate GtrHeaderDialogPrivate;

/*
 * Main object structure
 */
typedef struct _GtrHeaderDialog GtrHeaderDialog;

struct _GtrHeaderDialog
{
  GtkDialog parent_instance;

  /*< private > */
  GtrHeaderDialogPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtrHeaderDialogClass GtrHeaderDialogClass;

struct _GtrHeaderDialogClass
{
  GtkDialogClass parent_class;
};

/*
 * Public methods
 */
GType
gtr_header_dialog_get_type (void)
  G_GNUC_CONST;

     GType gtr_header_dialog_register_type (GTypeModule * module);

     void gtr_show_header_dialog (GtrWindow * window);

#endif
