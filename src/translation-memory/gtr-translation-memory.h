/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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
 */

#ifndef _GTR_TRANSLATION_MEMORY_H_
#define _GTR_TRANSLATION_MEMORY_H_

#include <glib-object.h>
#include "gtr-msg.h"

G_BEGIN_DECLS
#define GTR_TYPE_TRANSLATION_MEMORY (gtr_translation_memory_get_type ())
#define GTR_TRANSLATION_MEMORY(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_TRANSLATION_MEMORY, GtrTranslationMemory))
#define GTR_IS_TRANSLATION_MEMORY(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTR_TYPE_TRANSLATION_MEMORY))
#define GTR_TRANSLATION_MEMORY_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GTR_TYPE_TRANSLATION_MEMORY, GtrTranslationMemoryInterface))

typedef struct _GtrTranslationMemory GtrTranslationMemory;
typedef struct _GtrTranslationMemoryInterface GtrTranslationMemoryInterface;

struct _GtrTranslationMemoryInterface
{
  GTypeInterface g_iface;

  gboolean (*store) (GtrTranslationMemory * obj, GtrMsg * msg);
  gboolean (*store_list) (GtrTranslationMemory * obj, GList * msgs);
  void (*remove) (GtrTranslationMemory *obj,
                  gint                 translation_id);

  GList *(*lookup) (GtrTranslationMemory * obj, const gchar * phrase);
  void (*set_max_omits) (GtrTranslationMemory * obj, gsize omits);
  void (*set_max_delta) (GtrTranslationMemory * obj, gsize delta);
  void (*set_max_items) (GtrTranslationMemory * obj, gint items);
};

typedef struct _GtrTranslationMemoryMatch GtrTranslationMemoryMatch;
struct _GtrTranslationMemoryMatch
{
  gchar *match;
  gint level;
  gint id;
};

GType           gtr_translation_memory_get_type         (void);

gboolean        gtr_translation_memory_store            (GtrTranslationMemory   *obj,
                                                         GtrMsg                 *msg);

gboolean        gtr_translation_memory_store_list       (GtrTranslationMemory   *obj,
                                                         GList                  *msg);

GList          *gtr_translation_memory_lookup           (GtrTranslationMemory   *obj,
                                                         const gchar            *phrase);

void            gtr_translation_memory_set_max_omits    (GtrTranslationMemory   *obj,
                                                         gsize                   omits);

void            gtr_translation_memory_set_max_delta    (GtrTranslationMemory   *obj,
                                                         gsize                   delta);

void            gtr_translation_memory_set_max_items    (GtrTranslationMemory   *obj,
                                                         gint                    items);

G_END_DECLS
#endif
