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
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __MESSAGE_TABLE_H__
#define __MESSAGE_TABLE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "msg.h"

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_MESSAGE_TABLE		(gtranslator_message_table_get_type ())
#define GTR_MESSAGE_TABLE(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_MESSAGE_TABLE, GtranslatorMessageTable))
#define GTR_MESSAGE_TABLE_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_MESSAGE_TABLE, GtranslatorMessageTableClass))
#define GTR_IS_MESSAGE_TABLE(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_MESSAGE_TABLE))
#define GTR_IS_MESSAGE_TABLE_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_MESSAGE_TABLE))
#define GTR_MESSAGE_TABLE_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_MESSAGE_TABLE, GtranslatorMessageTableClass))

/* Private structure type */
typedef struct _GtranslatorMessageTablePrivate	GtranslatorMessageTablePrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorMessageTable		GtranslatorMessageTable;

struct _GtranslatorMessageTable
{
	GtkVBox parent_instance;
	
	/* < private > */
	GtranslatorMessageTablePrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorMessageTableClass	GtranslatorMessageTableClass;

struct _GtranslatorMessageTableClass
{
	GtkVBoxClass parent_class;
};

/*
 * Public methods
 */
GType		gtranslator_message_table_get_type		(void) G_GNUC_CONST;

GType		gtranslator_message_table_register_type		(GTypeModule * module);

GtkWidget	*gtranslator_message_table_new			(GtkWidget *tab);

void		gtranslator_message_table_populate		(GtranslatorMessageTable *table, 
								 GList *messages);

void		gtranslator_message_table_update_translation	(GtranslatorMessageTable *table,
								 GtranslatorMsg *msg,
								 gchar *translation);

G_END_DECLS

#endif /* __MESSAGE_TABLE_H__ */
