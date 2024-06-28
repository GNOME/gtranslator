/*
 * Copyright (C) 2007-2012 Ignacio Casal Quinteiro <icq@gnome.org>
 *               2008  Igalia
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
 *   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *   Pablo Sanxiao <psanxiao@gmail.com>
 */

#pragma once

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gettext-po.h>

G_BEGIN_DECLS

#define GTR_TYPE_MSG (gtr_msg_get_type ())
G_DECLARE_DERIVABLE_TYPE (GtrMsg, gtr_msg, GTR, MSG, GObject)

struct _GtrMsgClass
{
  GObjectClass parent_class;
};


typedef enum
{
  GTR_MSG_STATUS_UNTRANSLATED,
  GTR_MSG_STATUS_FUZZY,
  GTR_MSG_STATUS_TRANSLATED
} GtrMsgStatus;

/* Public methods */
gboolean                   gtr_msg_is_translated            (GtrMsg     *msg);

gboolean                   gtr_msg_is_fuzzy                 (GtrMsg     *msg);
void                       gtr_msg_set_fuzzy                (GtrMsg     *msg,
                                                             gboolean    fuzzy);

GtrMsgStatus               gtr_msg_get_status               (GtrMsg      *msg);
void                       gtr_msg_set_status               (GtrMsg      *msg,
                                                             GtrMsgStatus status);

const gchar               *gtr_msg_get_msgid                (GtrMsg      *msg);

const gchar               *gtr_msg_get_msgid_plural         (GtrMsg      *msg);

const gchar               *gtr_msg_get_msgstr               (GtrMsg      *msg);
void                       gtr_msg_set_msgstr               (GtrMsg      *msg,
                                                             const gchar *msgstr);

const gchar               *gtr_msg_get_msgstr_plural        (GtrMsg      *msg,
                                                             gint         index);
void                       gtr_msg_set_msgstr_plural        (GtrMsg      *msg,
                                                             gint         index,
                                                             const gchar *msgstr);

const gchar               *gtr_msg_get_comment              (GtrMsg      *msg);
void                       gtr_msg_set_comment              (GtrMsg      *msg,
                                                             const gchar *comment);

gint                       gtr_msg_get_po_position          (GtrMsg      *msg);
void                       gtr_msg_set_po_position          (GtrMsg      *msg,
                                                             gint         po_position);

const gchar               *gtr_msg_get_extracted_comments   (GtrMsg      *msg);

const gchar               *gtr_msg_get_filename             (GtrMsg      *msg,
                                                             gint         i);

gint                      *gtr_msg_get_file_line            (GtrMsg      *msg,
                                                             gint         i);

const gchar               *gtr_msg_get_msgctxt              (GtrMsg      *msg);

const gchar               *gtr_msg_get_format               (GtrMsg      *msg);

gchar                     *gtr_msg_check                    (GtrMsg      *msg);

gboolean                  gtr_msg_fix_plurals               (GtrMsg      *msg,
                                                             int plurals);

gboolean                  gtr_msg_compare                   (GtrMsg *first, GtrMsg *second);

/* Semi-private methods */
GtrMsg                   *_gtr_msg_new                      (po_message_iterator_t iter,
                                                             po_message_t          message);

po_message_iterator_t     _gtr_msg_get_iterator             (GtrMsg *msg);
void                      _gtr_msg_set_iterator             (GtrMsg               *msg,
                                                             po_message_iterator_t iter);

po_message_t              _gtr_msg_get_message              (GtrMsg               *msg);

void                      _gtr_msg_set_message              (GtrMsg               *msg,
                                                             po_message_t          message);

G_END_DECLS
