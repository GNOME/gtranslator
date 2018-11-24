/*
 * gtr-history-entry.h
 * This file is part of gtr
 *
 * Copyright (C) 2006  Paolo Borelli
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

/*
 * Modified by the gtr Team, 2006. See the AUTHORS file for a 
 * list of people on the gtr Team.  
 * See the ChangeLog files for a list of changes. 
 *
 * $Id$
 */

#ifndef __GTR_HISTORY_ENTRY_H__
#define __GTR_HISTORY_ENTRY_H__


G_BEGIN_DECLS
#define GTR_TYPE_HISTORY_ENTRY             (gtr_history_entry_get_type ())
#define GTR_HISTORY_ENTRY(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_HISTORY_ENTRY, GtrHistoryEntry))
#define GTR_HISTORY_ENTRY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GTR_TYPE_HISTORY_ENTRY, GtrHistoryEntryClass))
#define GTR_IS_HISTORY_ENTRY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTR_TYPE_HISTORY_ENTRY))
#define GTR_IS_HISTORY_ENTRY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_TYPE_HISTORY_ENTRY))
#define GTR_HISTORY_ENTRY_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GTR_TYPE_HISTORY_ENTRY, GtrHistoryEntryClass))

typedef struct _GtrHistoryEntry GtrHistoryEntry;
typedef struct _GtrHistoryEntryClass GtrHistoryEntryClass;

struct _GtrHistoryEntryClass
{
  GtkComboBoxTextClass parent_class;
};

GType
gtr_history_entry_get_type (void)
  G_GNUC_CONST;

     GtkWidget *gtr_history_entry_new (const gchar * history_id,
                                       gboolean enable_completion);

     void gtr_history_entry_prepend_text (GtrHistoryEntry * entry,
                                          const gchar * text);

     void gtr_history_entry_append_text (GtrHistoryEntry * entry,
                                         const gchar * text);

     void gtr_history_entry_clear (GtrHistoryEntry * entry);

     void gtr_history_entry_set_history_length (GtrHistoryEntry * entry,
                                                guint max_saved);

     guint gtr_history_entry_get_history_length (GtrHistoryEntry * gentry);

     void gtr_history_entry_set_enable_completion
       (GtrHistoryEntry * entry, gboolean enable);

     gboolean gtr_history_entry_get_enable_completion
       (GtrHistoryEntry * entry);

     GtkWidget *gtr_history_entry_get_entry (GtrHistoryEntry * entry);

     typedef gchar *(*GtrHistoryEntryEscapeFunc) (const gchar * str);
     void gtr_history_entry_set_escape_func (GtrHistoryEntry * entry,
                                             GtrHistoryEntryEscapeFunc
                                             escape_func);

G_END_DECLS
#endif /* __GTR_HISTORY_ENTRY_H__ */
/* ex:set ts=8 noet: */
