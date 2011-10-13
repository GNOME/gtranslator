/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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

#ifndef __MSG_H__
#define __MSG_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gettext-po.h>

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_MSG		(gtr_msg_get_type ())
#define GTR_MSG(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_MSG, GtrMsg))
#define GTR_MSG_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_MSG, GtrMsgClass))
#define GTR_IS_MSG(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_MSG))
#define GTR_IS_MSG_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_MSG))
#define GTR_MSG_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_MSG, GtrMsgClass))
/* Private structure type */
typedef struct _GtrMsgPrivate GtrMsgPrivate;

/*
 * Main object structure
 */
typedef struct _GtrMsg GtrMsg;

struct _GtrMsg
{
  GObject parent_instance;

  /*< private > */
  GtrMsgPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtrMsgClass GtrMsgClass;

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

/*
 * Public methods
 */
GType
gtr_msg_get_type (void)
  G_GNUC_CONST;

     GType gtr_msg_register_type (GTypeModule * module);

     GtrMsg *gtr_msg_new (po_message_iterator_t iter, po_message_t message);

po_message_iterator_t
gtr_msg_get_iterator (GtrMsg * msg);

     void gtr_msg_set_iterator (GtrMsg * msg, po_message_iterator_t iter);

     po_message_t gtr_msg_get_message (GtrMsg * msg);

     void gtr_msg_set_message (GtrMsg * msg, po_message_t message);

     GtkTreeRowReference *gtr_msg_get_row_reference (GtrMsg * msg);

     void gtr_msg_set_row_reference (GtrMsg * msg,
                                     GtkTreeRowReference * row_reference);

     gboolean gtr_msg_is_translated (GtrMsg * msg);

     gboolean gtr_msg_is_fuzzy (GtrMsg * msg);

     void gtr_msg_set_fuzzy (GtrMsg * msg, gboolean fuzzy);

     void gtr_msg_set_status (GtrMsg * msg, GtrMsgStatus status);

     GtrMsgStatus gtr_msg_get_status (GtrMsg * msg);

     const gchar *gtr_msg_get_msgid (GtrMsg * msg);

     const gchar *gtr_msg_get_msgid_plural (GtrMsg * msg);

     const gchar *gtr_msg_get_msgstr (GtrMsg * msg);

     void gtr_msg_set_msgstr (GtrMsg * msg, const gchar * msgstr);

     const gchar *gtr_msg_get_msgstr_plural (GtrMsg * msg, gint index);

     void gtr_msg_set_msgstr_plural (GtrMsg * msg,
                                     gint index, const gchar * msgstr);

     const gchar *gtr_msg_get_comment (GtrMsg * msg);

     void gtr_msg_set_comment (GtrMsg * msg, const gchar * comment);

     gint gtr_msg_get_po_position (GtrMsg * msg);

     void gtr_msg_set_po_position (GtrMsg * msg, gint po_position);

     const gchar *gtr_msg_get_extracted_comments (GtrMsg * msg);

     const gchar *gtr_msg_get_filename (GtrMsg * msg, gint i);

     gint *gtr_msg_get_file_line (GtrMsg * msg, gint i);

     const gchar *gtr_msg_get_msgctxt (GtrMsg * msg);

     const gchar *gtr_msg_get_format (GtrMsg * msg);

     gchar *gtr_msg_check (GtrMsg * msg);

G_END_DECLS
#endif /* __MSG_H__ */
