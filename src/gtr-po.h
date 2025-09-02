/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *               2008  Pablo Sanxiao <psanxiao@gmail.com>
 *                     Igalia
 *      
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Pablo Sanxiao <psanxiao@gmail.com>
 *   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 */

#ifndef __PO_H__
#define __PO_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gettext-po.h>
#include <gio/gio.h>

#include "gtr-header.h"

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_PO		(gtr_po_get_type ())

G_DECLARE_FINAL_TYPE (GtrPo, gtr_po, GTR, PO, GObject)

struct _GtrPo
{
  GObject parent_instance;
};

struct _GtrPoClass
{
  GObjectClass parent_class;
};

#define GTR_PO_ERROR gtr_po_error_quark()

enum
{
  GTR_PO_ERROR_GETTEXT,
  GTR_PO_ERROR_FILENAME,
  GTR_PO_ERROR_RECOVERY,
  GTR_PO_ERROR_FILE_EMPTY,
  GTR_PO_ERROR_READONLY,
  GTR_PO_ERROR_ENCODING,
  GTR_PO_ERROR_OTHER,
};

typedef enum
{
  GTR_PO_STATE_SAVED,
  GTR_PO_STATE_MODIFIED
} GtrPoState;

/*
 * Public methods
 */
GQuark gtr_po_error_quark (void);

     GtrPo *gtr_po_new_from_file (GFile   *location,
                                  GError **error) G_GNUC_WARN_UNUSED_RESULT;

     void gtr_po_save_header_in_msg (GtrPo * po, GtrHeader * header);

     void gtr_po_save_file (GtrPo * po, GError ** error);

     GtrPoState gtr_po_get_state (GtrPo * po);

     void gtr_po_set_state (GtrPo * po, GtrPoState state);

     void gtr_po_set_dl_info (GtrPo * po, const gchar * lang, const gchar * module_name,
                              const gchar * branch, const gchar * domain, const gchar * module_state,
                              const gchar *vcs_web);

     const gchar *gtr_po_get_dl_lang (GtrPo * po);

     const gchar *gtr_po_get_dl_module (GtrPo * po);

     const gchar *gtr_po_get_dl_branch (GtrPo * po);

     const gchar *gtr_po_get_dl_domain (GtrPo * po);

     const gchar *gtr_po_get_dl_module_state (GtrPo * po);

     const gchar *gtr_po_get_dl_vcs_web (GtrPo * po);

     gboolean gtr_po_can_dl_upload (GtrPo * po);

     GFile *gtr_po_get_location (GtrPo * po);

     void gtr_po_set_location (GtrPo * po, GFile * location);

     GList *gtr_po_get_messages (GtrPo * po);

     GList *gtr_po_get_current_message (GtrPo * po);

     void gtr_po_update_current_message (GtrPo * po, GtrMsg * msg);

     GList *gtr_po_get_domains (GtrPo * po);

     po_file_t gtr_po_get_po_file (GtrPo * po);

     GList *gtr_po_get_next_fuzzy (GtrPo * po);

     GList *gtr_po_get_prev_fuzzy (GtrPo * po);

     GList *gtr_po_get_next_untrans (GtrPo * po);

     GList *gtr_po_get_prev_untrans (GtrPo * po);

     GList *gtr_po_get_next_fuzzy_or_untrans (GtrPo * po);

     GList *gtr_po_get_prev_fuzzy_or_untrans (GtrPo * po);

     GList *gtr_po_get_msg_from_number (GtrPo * po, gint number);

     GtrHeader *gtr_po_get_header (GtrPo * po);

gint
gtr_po_get_translated_count (GtrPo * po);

gint
gtr_po_get_fuzzy_count (GtrPo * po);

gint
gtr_po_get_untranslated_count (GtrPo * po);

gint
gtr_po_get_messages_count (GtrPo * po);

gint
gtr_po_get_message_position (GtrPo * po);

     gchar *gtr_po_check_po_file (GtrPo * po);

/* Unexported funcs */
     void
       _gtr_po_increase_decrease_translated (GtrPo * po, gboolean increase);

     void _gtr_po_increase_decrease_fuzzy (GtrPo * po, gboolean increase);

G_END_DECLS
#endif /* __PO_H__ */
