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
 * MERCHANMSGILITY or FITNESS FOR A PARTICULAR PURMSGSE.  See the
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
#define GTR_TYPE_MSG		(gtranslator_msg_get_type ())
#define GTR_MSG(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_MSG, GtranslatorMsg))
#define GTR_MSG_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_MSG, GtranslatorMsgClass))
#define GTR_IS_MSG(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_MSG))
#define GTR_IS_MSG_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_MSG))
#define GTR_MSG_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_MSG, GtranslatorMsgClass))

/* Private structure type */
typedef struct _GtranslatorMsgPrivate	GtranslatorMsgPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorMsg		GtranslatorMsg;

struct _GtranslatorMsg
{
	GObject parent_instance;
	
	/*< private > */
	GtranslatorMsgPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorMsgClass	GtranslatorMsgClass;

struct _GtranslatorMsgClass
{
	GObjectClass parent_class;
};

typedef enum
{
	GTR_MSG_STATUS_UNTRANSLATED,
	GTR_MSG_STATUS_FUZZY,
	GTR_MSG_STATUS_TRANSLATED
} GtranslatorMsgStatus;

/*
 * Public methods
 */
GType		  gtranslator_msg_get_type	       (void) G_GNUC_CONST;

GType		  gtranslator_msg_register_type	       (GTypeModule * module);

GtranslatorMsg   *gtranslator_msg_new                  (po_message_iterator_t iter,
							po_message_t message);

po_message_iterator_t
                  gtranslator_msg_get_iterator         (GtranslatorMsg *msg);

void              gtranslator_msg_set_iterator         (GtranslatorMsg *msg,
							po_message_iterator_t iter);

po_message_t      gtranslator_msg_get_message          (GtranslatorMsg *msg);

void              gtranslator_msg_set_message          (GtranslatorMsg *msg,
							po_message_t message);

GtkTreeRowReference *gtranslator_msg_get_row_reference    (GtranslatorMsg *msg);

void              gtranslator_msg_set_row_reference    (GtranslatorMsg *msg,
							GtkTreeRowReference *row_reference);

gboolean          gtranslator_msg_is_translated        (GtranslatorMsg *msg);

gboolean          gtranslator_msg_is_fuzzy             (GtranslatorMsg *msg);

void              gtranslator_msg_set_fuzzy            (GtranslatorMsg *msg,
							gboolean fuzzy);

void              gtranslator_msg_set_status           (GtranslatorMsg *msg,
							GtranslatorMsgStatus status);

GtranslatorMsgStatus gtranslator_msg_get_status           (GtranslatorMsg *msg);

const gchar      *gtranslator_msg_get_msgid            (GtranslatorMsg *msg);

const gchar      *gtranslator_msg_get_msgid_plural     (GtranslatorMsg *msg);

const gchar      *gtranslator_msg_get_msgstr           (GtranslatorMsg *msg);

void              gtranslator_msg_set_msgstr           (GtranslatorMsg *msg,
							const gchar *msgstr);

const gchar      *gtranslator_msg_get_msgstr_plural    (GtranslatorMsg *msg,
							gint index);

void              gtranslator_msg_set_msgstr_plural    (GtranslatorMsg *msg,
							gint index,
							const gchar *msgstr);

const gchar      *gtranslator_msg_get_comment          (GtranslatorMsg *msg);

void              gtranslator_msg_set_comment          (GtranslatorMsg *msg,
							const gchar *comment);

gint              gtranslator_msg_get_po_position      (GtranslatorMsg *msg);

void              gtranslator_msg_set_po_position      (GtranslatorMsg *msg,
							gint po_position);

const gchar      *gtranslator_msg_get_extracted_comments
                                                       (GtranslatorMsg *msg);
                                                       
const gchar      *gtranslator_msg_get_filename         (GtranslatorMsg *msg,
							gint i);

gint             *gtranslator_msg_get_file_line        (GtranslatorMsg *msg,
							gint i);

const gchar      *gtranslator_msg_get_msgctxt          (GtranslatorMsg *msg);

const gchar      *gtranslator_msg_get_format           (GtranslatorMsg *msg);

gchar            *gtranslator_msg_check                (GtranslatorMsg *msg);

G_END_DECLS

#endif /* __MSG_H__ */
