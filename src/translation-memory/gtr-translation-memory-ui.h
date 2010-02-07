/*
 * Copyright (C) 2008 Igalia  
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
 *
 * Authors:
 *   Pablo Sanxiao <psanxiao@gmail.com>
 */

#ifndef __TRANSLATION_MEMORY_UI_H__
#define __TRANSLATION_MEMORY_UI_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_TRANSLATION_MEMORY_UI	        (gtr_translation_memory_ui_get_type ())
#define GTR_TRANSLATION_MEMORY_UI(o)	        (G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_TRANSLATION_MEMORY_UI, GtrTranslationMemoryUi))
#define GTR_TRANSLATION_MEMORY_UI_CLASS(k)      (G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_TRANSLATION_MEMORY_UI, GtrTranslationMemoryUiClass))
#define GTR_IS_TRANSLATION_MEMORY_UI(o)	        (G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_TRANSLATION_MEMORY_UI))
#define GTR_IS_TRANSLATION_MEMORY_UI_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_TRANSLATION_MEMORY_UI))
#define GTR_TRANSLATION_MEMORY_UI_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_TRANSLATION_MEMORY_UI, GtrTranslationMemoryUiClass))
/* Private structure type */
typedef struct _GtrTranslationMemoryUiPrivate GtrTranslationMemoryUiPrivate;

/*
 * Main object structure
 */
typedef struct _GtrTranslationMemoryUi GtrTranslationMemoryUi;

struct _GtrTranslationMemoryUi
{
  GtkScrolledWindow parent_instance;

  /*< private > */
  GtrTranslationMemoryUiPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtrTranslationMemoryUiClass GtrTranslationMemoryUiClass;

struct _GtrTranslationMemoryUiClass
{
  GtkScrolledWindowClass parent_class;
};

/*
 * Public methods
 */
GType
gtr_translation_memory_ui_get_type (void)
  G_GNUC_CONST;

     GType gtr_translation_memory_ui_register_type (GTypeModule * module);

     GtkWidget *gtr_translation_memory_ui_new (GtkWidget * tab);

G_END_DECLS
#endif /* __TRANSLATION_MEMORY_UI_H__ */
