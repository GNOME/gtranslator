/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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

#ifndef _GTR_TRANSLATION_MEMORY_H_
#define _GTR_TRANSLATION_MEMORY_H_

#include <glib-object.h>

G_BEGIN_DECLS

#define GTR_TYPE_TRANSLATION_MEMORY (gtranslator_translation_memory_get_type ())
#define GTR_TRANSLATION_MEMORY(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_TRANSLATION_MEMORY, GtranslatorTranslationMemory))
#define GTR_IS_TRANSLATION_MEMORY(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTR_TYPE_TRANSLATION_MEMORY))
#define GTR_TRANSLATION_MEMORY_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GTR_TYPE_TRANSLATION_MEMORY, GtranslatorTranslationMemoryIface))

typedef struct _GtranslatorTranslationMemory GtranslatorTranslationMemory;
typedef struct _GtranslatorTranslationMemoryIface GtranslatorTranslationMemoryIface;

struct _GtranslatorTranslationMemoryIface {
	GTypeInterface g_iface;
	

	gboolean (*store) (GtranslatorTranslationMemory *obj,
			   const gchar *original,
			   const gchar *translation);
	GList * (*lookup) (GtranslatorTranslationMemory *obj,
			   const gchar *phrase);
	void (*set_max_omits) (GtranslatorTranslationMemory *obj,
			       gsize omits);
	void (*set_max_delta) (GtranslatorTranslationMemory *obj,
			       gsize delta);
	void (*set_max_items) (GtranslatorTranslationMemory *obj,
			       gint items);
};

typedef struct _GtranslatorTranslationMemoryMatch GtranslatorTranslationMemoryMatch;
struct _GtranslatorTranslationMemoryMatch {
	gchar *match;
	gint level;
};

GType  gtranslator_translation_memory_get_type            (void);

gboolean   gtranslator_translation_memory_store           (GtranslatorTranslationMemory *obj,
							   const gchar *original,
							   const gchar *translation);
						       
GList *   gtranslator_translation_memory_lookup           (GtranslatorTranslationMemory *obj,
							   const gchar *phrase);

void      gtranslator_translation_memory_set_max_omits    (GtranslatorTranslationMemory *obj,
							   gsize omits);

void      gtranslator_translation_memory_set_max_delta    (GtranslatorTranslationMemory *obj,
							   gsize delta);

void      gtranslator_translation_memory_set_max_items    (GtranslatorTranslationMemory *obj,
							   gint items);

G_END_DECLS

#endif
