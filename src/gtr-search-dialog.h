/*
 * search-dialog.h
 * This file is part of gtranslator based on gedit
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
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef __SEARCH_DIALOG_H__
#define __SEARCH_DIALOG_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_SEARCH_DIALOG              (gtr_search_dialog_get_type())
#define GTR_SEARCH_DIALOG(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), GTR_TYPE_SEARCH_DIALOG, GtrSearchDialog))
#define GTR_SEARCH_DIALOG_CONST(obj)        (G_TYPE_CHECK_INSTANCE_CAST((obj), GTR_TYPE_SEARCH_DIALOG, GtrSearchDialog const))
#define GTR_SEARCH_DIALOG_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GTR_TYPE_SEARCH_DIALOG, GtrSearchDialogClass))
#define GTR_IS_SEARCH_DIALOG(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTR_TYPE_SEARCH_DIALOG))
#define GTR_IS_SEARCH_DIALOG_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_TYPE_SEARCH_DIALOG))
#define GTR_SEARCH_DIALOG_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GTR_TYPE_SEARCH_DIALOG, GtrSearchDialogClass))
/* Private structure type */
typedef struct _GtrSearchDialogPrivate GtrSearchDialogPrivate;

/*
 * Main object structure
 */
typedef struct _GtrSearchDialog GtrSearchDialog;

struct _GtrSearchDialog
{
  GtkDialog dialog;

  /*< private > */
  GtrSearchDialogPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtrSearchDialogClass GtrSearchDialogClass;

struct _GtrSearchDialogClass
{
  GtkDialogClass parent_class;

  /* Key bindings */
    gboolean (*show_replace) (GtrSearchDialog * dlg);
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
gtr_search_dialog_get_type (void)
  G_GNUC_CONST;

     GtkWidget *gtr_search_dialog_new (GtkWindow * parent,
                                       gboolean show_replace);

     void gtr_search_dialog_present_with_time (GtrSearchDialog
                                               * dialog, guint32 timestamp);

gboolean
gtr_search_dialog_get_show_replace (GtrSearchDialog * dialog);

     void gtr_search_dialog_set_show_replace (GtrSearchDialog
                                              * dialog,
                                              gboolean show_replace);

     void gtr_search_dialog_set_search_text (GtrSearchDialog *
                                             dialog, const gchar * text);

     const gchar
       * gtr_search_dialog_get_search_text (GtrSearchDialog * dialog);

     void gtr_search_dialog_set_replace_text (GtrSearchDialog
                                              * dialog, const gchar * text);

     const gchar
       * gtr_search_dialog_get_replace_text (GtrSearchDialog * dialog);

     void gtr_search_dialog_set_original_text (GtrSearchDialog
                                               * dialog, gboolean match_case);

gboolean
gtr_search_dialog_get_original_text (GtrSearchDialog * dialog);

     void
       gtr_search_dialog_set_translated_text (GtrSearchDialog
                                              * dialog, gboolean match_case);

gboolean
gtr_search_dialog_get_translated_text (GtrSearchDialog * dialog);

     void gtr_search_dialog_set_fuzzy (GtrSearchDialog *
                                       dialog, gboolean match_case);

     gboolean gtr_search_dialog_get_fuzzy (GtrSearchDialog * dialog);

     void gtr_search_dialog_set_match_case (GtrSearchDialog *
                                            dialog, gboolean match_case);

gboolean
gtr_search_dialog_get_match_case (GtrSearchDialog * dialog);

     void gtr_search_dialog_set_entire_word (GtrSearchDialog *
                                             dialog, gboolean entire_word);

gboolean
gtr_search_dialog_get_entire_word (GtrSearchDialog * dialog);

     void gtr_search_dialog_set_backwards (GtrSearchDialog *
                                           dialog, gboolean backwards);

     gboolean gtr_search_dialog_get_backwards (GtrSearchDialog * dialog);

     void gtr_search_dialog_set_wrap_around (GtrSearchDialog *
                                             dialog, gboolean wrap_around);

gboolean
gtr_search_dialog_get_wrap_around (GtrSearchDialog * dialog);

G_END_DECLS
#endif /* __SEARCH_DIALOG_H__  */
