/*
 * gtranslator-close-confirmation-dialog.h
 * This file is part of gtranslator
 *
 * Copyright (C) 2004-2005 GNOME Foundation 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, 
 * Boston, MA 02111-1307, USA. 
 */

/*
 * Modified by the gtranslator Team, 2004-2005. See the AUTHORS file for a 
 * list of people on the gtranslator Team.  
 * See the ChangeLog files for a list of changes. 
 */

#ifndef __GTR_CLOSE_CONFIRMATION_DIALOG_H__
#define __GTR_CLOSE_CONFIRMATION_DIALOG_H__

#include <glib.h>
#include <gtk/gtk.h>

#include "gtr-po.h"

#define GTR_TYPE_CLOSE_CONFIRMATION_DIALOG		(gtranslator_close_confirmation_dialog_get_type ())
#define GTR_CLOSE_CONFIRMATION_DIALOG(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_CLOSE_CONFIRMATION_DIALOG, GtranslatorCloseConfirmationDialog))
#define GTR_CLOSE_CONFIRMATION_DIALOG_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GTR_TYPE_CLOSE_CONFIRMATION_DIALOG, GtranslatorCloseConfirmationDialogClass))
#define GTR_IS_CLOSE_CONFIRMATION_DIALOG(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTR_TYPE_CLOSE_CONFIRMATION_DIALOG))
#define GTR_IS_CLOSE_CONFIRMATION_DIALOG_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_TYPE_CLOSE_CONFIRMATION_DIALOG))
#define GTR_CLOSE_CONFIRMATION_DIALOG_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj),GTR_TYPE_CLOSE_CONFIRMATION_DIALOG, GtranslatorCloseConfirmationDialogClass))

typedef struct _GtranslatorCloseConfirmationDialog
  GtranslatorCloseConfirmationDialog;
typedef struct _GtranslatorCloseConfirmationDialogClass
  GtranslatorCloseConfirmationDialogClass;
typedef struct _GtranslatorCloseConfirmationDialogPrivate
  GtranslatorCloseConfirmationDialogPrivate;

struct _GtranslatorCloseConfirmationDialog
{
  GtkDialog parent;

  /*< private > */
  GtranslatorCloseConfirmationDialogPrivate *priv;
};

struct _GtranslatorCloseConfirmationDialogClass
{
  GtkDialogClass parent_class;
};

GType
gtranslator_close_confirmation_dialog_get_type (void)
  G_GNUC_CONST;

     GtkWidget *gtranslator_close_confirmation_dialog_new (GtkWindow * parent,
							   GList *
							   unsaved_documents,
							   gboolean
							   logout_mode);
     GtkWidget *gtranslator_close_confirmation_dialog_new_single (GtkWindow *
								  parent,
								  GtranslatorPo
								  * doc,
								  gboolean
								  logout_mode);

     const GList
       *gtranslator_close_confirmation_dialog_get_unsaved_documents
       (GtranslatorCloseConfirmationDialog * dlg);

     GList
       *gtranslator_close_confirmation_dialog_get_selected_documents
       (GtranslatorCloseConfirmationDialog * dlg);

#endif /* __GTR_CLOSE_CONFIRMATION_DIALOG_H__ */
