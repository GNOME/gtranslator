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
#include "window.h"


/*
 * Type checking and casting macros
 */
#define GTR_TYPE_HEADER_DIALOG		(gtranslator_header_dialog_get_type ())
#define GTR_HEADER_DIALOG(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_HEADER_DIALOG, GtranslatorHeaderDialog))
#define GTR_HEADER_DIALOG_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_HEADER_DIALOG, GtranslatorHeaderDialogClass))
#define GTR_IS_HEADER_DIALOG(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_HEADER_DIALOG))
#define GTR_IS_HEADER_DIALOG_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_HEADER_DIALOG))
#define GTR_HEADER_DIALOG_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_HEADER_DIALOG, GtranslatorHeaderDialogClass))

/* Private structure type */
typedef struct _GtranslatorHeaderDialogPrivate GtranslatorHeaderDialogPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorHeaderDialog GtranslatorHeaderDialog;

struct _GtranslatorHeaderDialog
{
  GtkDialog parent_instance;

  /*< private > */
  GtranslatorHeaderDialogPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorHeaderDialogClass GtranslatorHeaderDialogClass;

struct _GtranslatorHeaderDialogClass
{
  GtkDialogClass parent_class;
};

/*
 * Public methods
 */
GType
gtranslator_header_dialog_get_type (void)
  G_GNUC_CONST;

     GType gtranslator_header_dialog_register_type (GTypeModule * module);

     void gtranslator_show_header_dialog (GtranslatorWindow * window);

#endif
