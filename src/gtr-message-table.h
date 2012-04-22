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

#include "gtr-message-container.h"
#include "gtr-msg.h"

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_MESSAGE_TABLE		(gtr_message_table_get_type ())
#define GTR_MESSAGE_TABLE(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_MESSAGE_TABLE, GtrMessageTable))
#define GTR_MESSAGE_TABLE_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_MESSAGE_TABLE, GtrMessageTableClass))
#define GTR_IS_MESSAGE_TABLE(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_MESSAGE_TABLE))
#define GTR_IS_MESSAGE_TABLE_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_MESSAGE_TABLE))
#define GTR_MESSAGE_TABLE_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_MESSAGE_TABLE, GtrMessageTableClass))
/* Private structure type */
typedef struct _GtrMessageTablePrivate GtrMessageTablePrivate;

/*
 * Main object structure
 */
typedef struct _GtrMessageTable GtrMessageTable;

struct _GtrMessageTable
{
  GtkBox parent_instance;

  /* < private > */
  GtrMessageTablePrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtrMessageTableClass GtrMessageTableClass;

struct _GtrMessageTableClass
{
  GtkBoxClass parent_class;
};

/*
 * Public methods
 */
GType
gtr_message_table_get_type (void)
  G_GNUC_CONST;

     GType gtr_message_table_register_type (GTypeModule * module);

     GtkWidget *gtr_message_table_new (GtkWidget * tab);

     void gtr_message_table_populate (GtrMessageTable * table,
                                      GtrMessageContainer * container);

     void
       gtr_message_table_update_translation (GtrMessageTable *
                                             table,
                                             GtrMsg * msg,
                                             gchar * translation);

G_END_DECLS
#endif /* __MESSAGE_TABLE_H__ */
