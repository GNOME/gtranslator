/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANMSGILITY or FITNESS FOR A PARTICULAR PURMSGSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __MESSAGE_TABLE_MODEL_H__
#define __MESSAGE_TABLE_MODEL_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtktreemodel.h>
#include "msg.h"

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_MESSAGE_TABLE_MODEL		(gtranslator_message_table_model_get_type ())
#define GTR_MESSAGE_TABLE_MODEL(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_MESSAGE_TABLE_MODEL, GtranslatorMessageTableModel))
#define GTR_MESSAGE_TABLE_MODEL_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_MESSAGE_TABLE_MODEL, GtranslatorMessageTableModelClass))
#define GTR_IS_MESSAGE_TABLE_MODEL(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_MESSAGE_TABLE_MODEL))
#define GTR_IS_MESSAGE_TABLE_MODEL_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_MESSAGE_TABLE_MODEL))
#define GTR_MESSAGE_TABLE_MODEL_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_MESSAGE_TABLE_MODEL, GtranslatorMessageTableModelClass))

/* Private structure type */
typedef struct _GtranslatorMessageTableModelPrivate	GtranslatorMessageTableModelPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorMessageTableModel		GtranslatorMessageTableModel;

struct _GtranslatorMessageTableModel
{
	GObject parent_instance;
	
	GList *values;
	gint stamp;
	gint length;
};

/*
 * Class definition
 */
typedef struct _GtranslatorMessageTableModelClass	GtranslatorMessageTableModelClass;

struct _GtranslatorMessageTableModelClass
{
	GObjectClass parent_class;
};

typedef enum _GtranslatorMessageTableModelColumn GtranslatorMessageTableModelColumn;

enum _GtranslatorMessageTableModelColumn
{
	GTR_MESSAGE_TABLE_MODEL_ICON_COLUMN,
	GTR_MESSAGE_TABLE_MODEL_ID_COLUMN,
	GTR_MESSAGE_TABLE_MODEL_ORIGINAL_COLUMN,
	GTR_MESSAGE_TABLE_MODEL_TRANSLATION_COLUMN,
	GTR_MESSAGE_TABLE_MODEL_STATUS_COLUMN,
	GTR_MESSAGE_TABLE_MODEL_POINTER_COLUMN,
	GTR_MESSAGE_TABLE_MODEL_N_COLUMNS
};

/*
 * Public methods
 */
GType		  gtranslator_message_table_model_get_type	       (void) G_GNUC_CONST;

GType		  gtranslator_message_table_model_register_type	       (GTypeModule * module);

GtranslatorMessageTableModel *gtranslator_message_table_model_new                  (void);
                 
void              gtranslator_message_table_model_append             (GtranslatorMessageTableModel *model,
								      GtranslatorMsg *msg,
								      GtkTreeIter *iter);

void              gtranslator_message_table_model_update_row           (GtranslatorMessageTableModel *model,
									GtkTreePath *path);

G_END_DECLS

#endif /* __MESSAGE_TABLE_MODEL_H__ */
