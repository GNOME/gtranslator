/*
 * Copyright (C) 2012 Ignacio Casal Quinteiro <icq@gnome.org>
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

#ifndef __GTR_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE_H__
#define __GTR_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE_H__

#include <glib.h>
#include <glib-object.h>
#include <libpeas/peas-extension-base.h>
#include <libpeas/peas-object-module.h>

G_BEGIN_DECLS

#define GTR_TYPE_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE		(gtr_translation_memory_window_activatable_get_type ())
#define GTR_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE, GtrTranslationMemoryWindowActivatable))
#define GTR_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE, GtrTranslationMemoryWindowActivatableClass))
#define GTR_IS_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE))
#define GTR_IS_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE))
#define GTR_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE_WINDOW_ACTIVATABLE, GtrTranslationMemoryWindowActivatableClass))

typedef struct _GtrTranslationMemoryWindowActivatable        GtrTranslationMemoryWindowActivatable;
typedef struct _GtrTranslationMemoryWindowActivatablePrivate GtrTranslationMemoryWindowActivatablePrivate;
typedef struct _GtrTranslationMemoryWindowActivatableClass   GtrTranslationMemoryWindowActivatableClass;

struct _GtrTranslationMemoryWindowActivatable
{
  PeasExtensionBase parent_instance;

  /*< private >*/
  GtrTranslationMemoryWindowActivatablePrivate *priv;
};

struct _GtrTranslationMemoryWindowActivatableClass
{
  PeasExtensionBaseClass parent_class;
};

GType   gtr_translation_memory_window_activatable_get_type      (void) G_GNUC_CONST;

void    gtr_window_activatable_register_peas_type               (PeasObjectModule *module);

G_END_DECLS
#endif /* __GTR_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE_H__ */
