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

#ifndef __CHECKOUT_DIALOG_H__
#define __CHECKOUT_DIALOG_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include "window.h"

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_CHECKOUT_DIALOG		(gtranslator_checkout_dialog_get_type ())
#define GTR_CHECKOUT_DIALOG(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_CHECKOUT_DIALOG, GtranslatorCheckoutDialog))
#define GTR_CHECKOUT_DIALOG_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_CHECKOUT_DIALOG, GtranslatorCheckoutDialogClass))
#define GTR_IS_CHECKOUT_DIALOG(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_CHECKOUT_DIALOG))
#define GTR_IS_CHECKOUT_DIALOG_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_CHECKOUT_DIALOG))
#define GTR_CHECKOUT_DIALOG_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_CHECKOUT_DIALOG, GtranslatorCheckoutDialogClass))
/* Private structure type */
typedef struct _GtranslatorCheckoutDialogPrivate
  GtranslatorCheckoutDialogPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorCheckoutDialog GtranslatorCheckoutDialog;

struct _GtranslatorCheckoutDialog
{
  GtkDialog parent_instance;

  /*< private > */
  GtranslatorCheckoutDialogPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorCheckoutDialogClass GtranslatorCheckoutDialogClass;

struct _GtranslatorCheckoutDialogClass
{
  GtkDialogClass parent_class;
};

/*
 * Public methods
 */
GType
gtranslator_checkout_dialog_get_type (void)
  G_GNUC_CONST;

     GType gtranslator_checkout_dialog_register_type (GTypeModule * module);

     void gtranslator_show_checkout_dialog (GtranslatorWindow * window);

G_END_DECLS
#endif /* __CHECKOUT_DIALOG_H__ */
