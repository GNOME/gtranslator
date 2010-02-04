/*
 * search-dialog.h
 * This file is part of gtranslator
 *
 * Copyright (C) 2005 Paolo Maggi
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


#ifndef __SEARCH_DIALOG_H__
#define __SEARCH_DIALOG_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_SEARCH_DIALOG              (gtranslator_search_dialog_get_type())
#define GTR_SEARCH_DIALOG(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), GTR_TYPE_SEARCH_DIALOG, GtranslatorSearchDialog))
#define GTR_SEARCH_DIALOG_CONST(obj)        (G_TYPE_CHECK_INSTANCE_CAST((obj), GTR_TYPE_SEARCH_DIALOG, GtranslatorSearchDialog const))
#define GTR_SEARCH_DIALOG_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GTR_TYPE_SEARCH_DIALOG, GtranslatorSearchDialogClass))
#define GTR_IS_SEARCH_DIALOG(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTR_TYPE_SEARCH_DIALOG))
#define GTR_IS_SEARCH_DIALOG_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_TYPE_SEARCH_DIALOG))
#define GTR_SEARCH_DIALOG_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GTR_TYPE_SEARCH_DIALOG, GtranslatorSearchDialogClass))
/* Private structure type */
typedef struct _GtranslatorSearchDialogPrivate GtranslatorSearchDialogPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorSearchDialog GtranslatorSearchDialog;

struct _GtranslatorSearchDialog
{
  GtkDialog dialog;

  /*< private > */
  GtranslatorSearchDialogPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorSearchDialogClass GtranslatorSearchDialogClass;

struct _GtranslatorSearchDialogClass
{
  GtkDialogClass parent_class;

  /* Key bindings */
    gboolean (*show_replace) (GtranslatorSearchDialog * dlg);
};

enum
{
  GTR_SEARCH_DIALOG_FIND_RESPONSE = 100,
  GTR_SEARCH_DIALOG_REPLACE_RESPONSE,
  GTR_SEARCH_DIALOG_REPLACE_ALL_RESPONSE
};

/*
 * Public methods
 */
GType
gtranslator_search_dialog_get_type (void)
  G_GNUC_CONST;

     GtkWidget *gtranslator_search_dialog_new (GtkWindow * parent,
					       gboolean show_replace);

     void gtranslator_search_dialog_present_with_time (GtranslatorSearchDialog
						       * dialog,
						       guint32 timestamp);

     gboolean
       gtranslator_search_dialog_get_show_replace (GtranslatorSearchDialog *
						   dialog);

     void gtranslator_search_dialog_set_show_replace (GtranslatorSearchDialog
						      * dialog,
						      gboolean show_replace);

     void gtranslator_search_dialog_set_search_text (GtranslatorSearchDialog *
						     dialog,
						     const gchar * text);

     const gchar
       *gtranslator_search_dialog_get_search_text (GtranslatorSearchDialog *
						   dialog);

     void gtranslator_search_dialog_set_replace_text (GtranslatorSearchDialog
						      * dialog,
						      const gchar * text);

     const gchar
       *gtranslator_search_dialog_get_replace_text (GtranslatorSearchDialog *
						    dialog);

     void gtranslator_search_dialog_set_original_text (GtranslatorSearchDialog
						       * dialog,
						       gboolean match_case);

     gboolean
       gtranslator_search_dialog_get_original_text (GtranslatorSearchDialog *
						    dialog);

     void
       gtranslator_search_dialog_set_translated_text (GtranslatorSearchDialog
						      * dialog,
						      gboolean match_case);

     gboolean
       gtranslator_search_dialog_get_translated_text (GtranslatorSearchDialog
						      * dialog);

     void gtranslator_search_dialog_set_fuzzy (GtranslatorSearchDialog *
					       dialog, gboolean match_case);

     gboolean gtranslator_search_dialog_get_fuzzy (GtranslatorSearchDialog *
						   dialog);

     void gtranslator_search_dialog_set_match_case (GtranslatorSearchDialog *
						    dialog,
						    gboolean match_case);

     gboolean
       gtranslator_search_dialog_get_match_case (GtranslatorSearchDialog *
						 dialog);

     void gtranslator_search_dialog_set_entire_word (GtranslatorSearchDialog *
						     dialog,
						     gboolean entire_word);

     gboolean
       gtranslator_search_dialog_get_entire_word (GtranslatorSearchDialog *
						  dialog);

     void gtranslator_search_dialog_set_backwards (GtranslatorSearchDialog *
						   dialog,
						   gboolean backwards);

     gboolean gtranslator_search_dialog_get_backwards (GtranslatorSearchDialog
						       * dialog);

     void gtranslator_search_dialog_set_wrap_around (GtranslatorSearchDialog *
						     dialog,
						     gboolean wrap_around);

     gboolean
       gtranslator_search_dialog_get_wrap_around (GtranslatorSearchDialog *
						  dialog);

G_END_DECLS
#endif /* __SEARCH_DIALOG_H__  */
