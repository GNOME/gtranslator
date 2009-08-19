/*
 * history-entry.h
 * This file is part of gtranslator
 *
 * Copyright (C) 2006 - Paolo Borelli
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, 
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the gtranslator Team, 2006. See the AUTHORS file for a 
 * list of people on the gtranslator Team.  
 * See the ChangeLog files for a list of changes. 
 *
 * $Id: history-entry.h 5931 2007-09-25 20:05:40Z pborelli $
 */

#ifndef __GTR_HISTORY_ENTRY_H__
#define __GTR_HISTORY_ENTRY_H__


G_BEGIN_DECLS
#define GTR_TYPE_HISTORY_ENTRY             (gtranslator_history_entry_get_type ())
#define GTR_HISTORY_ENTRY(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_HISTORY_ENTRY, GtranslatorHistoryEntry))
#define GTR_HISTORY_ENTRY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GTR_TYPE_HISTORY_ENTRY, GtranslatorHistoryEntryClass))
#define GTR_IS_HISTORY_ENTRY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTR_TYPE_HISTORY_ENTRY))
#define GTR_IS_HISTORY_ENTRY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_TYPE_HISTORY_ENTRY))
#define GTR_HISTORY_ENTRY_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GTR_TYPE_HISTORY_ENTRY, GtranslatorHistoryEntryClass))
typedef struct _GtranslatorHistoryEntry GtranslatorHistoryEntry;
typedef struct _GtranslatorHistoryEntryClass GtranslatorHistoryEntryClass;
typedef struct _GtranslatorHistoryEntryPrivate GtranslatorHistoryEntryPrivate;

struct _GtranslatorHistoryEntryClass
{
  GtkComboBoxEntryClass parent_class;
};

struct _GtranslatorHistoryEntry
{
  GtkComboBoxEntry parent_instance;

  GtranslatorHistoryEntryPrivate *priv;
};

GType
gtranslator_history_entry_get_type (void)
  G_GNUC_CONST;

     GtkWidget *gtranslator_history_entry_new (const gchar * history_id,
					       gboolean enable_completion);

     void gtranslator_history_entry_prepend_text (GtranslatorHistoryEntry *
						  entry, const gchar * text);

     void gtranslator_history_entry_append_text (GtranslatorHistoryEntry *
						 entry, const gchar * text);

     void gtranslator_history_entry_clear (GtranslatorHistoryEntry * entry);

     void
       gtranslator_history_entry_set_history_length (GtranslatorHistoryEntry *
						     entry, guint max_saved);

     guint
       gtranslator_history_entry_get_history_length (GtranslatorHistoryEntry *
						     gentry);

     gchar *gtranslator_history_entry_get_history_id (GtranslatorHistoryEntry
						      * entry);

     void gtranslator_history_entry_set_enable_completion
       (GtranslatorHistoryEntry * entry, gboolean enable);

     gboolean gtranslator_history_entry_get_enable_completion
       (GtranslatorHistoryEntry * entry);

     GtkWidget *gtranslator_history_entry_get_entry (GtranslatorHistoryEntry *
						     entry);

     typedef gchar *(*GtranslatorHistoryEntryEscapeFunc) (const gchar * str);
     void gtranslator_history_entry_set_escape_func (GtranslatorHistoryEntry *
						     entry,
						     GtranslatorHistoryEntryEscapeFunc
						     escape_func);

G_END_DECLS
#endif /* __GTR_HISTORY_ENTRY_H__ */
