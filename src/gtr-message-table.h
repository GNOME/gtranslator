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

#pragma once

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "gtr-message-container.h"
#include "gtr-msg.h"

G_BEGIN_DECLS

#define GTR_TYPE_MESSAGE_TABLE (gtr_message_table_get_type ())
G_DECLARE_FINAL_TYPE (GtrMessageTable, gtr_message_table, GTR, MESSAGE_TABLE, GtkBox)

typedef gboolean (* GtrMessageTableNavigationFunc) (GtrMsg * msg);

typedef enum {
  GTR_NAVIGATE_PREV,
  GTR_NAVIGATE_NEXT,
  GTR_NAVIGATE_FIRST,
  GTR_NAVIGATE_LAST
} GtrMessageTableNavigation;

typedef enum
{
  GTR_MESSAGE_TABLE_SORT_ID,
  GTR_MESSAGE_TABLE_SORT_STATUS,
  GTR_MESSAGE_TABLE_SORT_MSGID,
  GTR_MESSAGE_TABLE_SORT_TRANSLATED,
} GtrMessageTableSortBy;

/*
 * Public methods
 */
GType gtr_message_table_register_type (GTypeModule * module);

GtkWidget *gtr_message_table_new (void);

void gtr_message_table_populate (GtrMessageTable * table,
                                 GtrMessageContainer * container);

GtrMsg * gtr_message_table_navigate (GtrMessageTable * table,
                                     GtrMessageTableNavigation navigation,
                                     GtrMessageTableNavigationFunc func);

void gtr_message_table_select (GtrMessageTable * table, GtrMsg * msg);

void gtr_message_table_update_translation (GtrMessageTable * table,
                                           GtrMsg * msg,
                                           gchar * translation);

void gtr_message_table_sort_by (GtrMessageTable *table,
                                GtrMessageTableSortBy sort);

GtkSingleSelection * gtr_message_table_get_selection_model (GtrMessageTable *table);
GListStore *gtr_message_table_get_model (GtrMessageTable *table);

G_END_DECLS
