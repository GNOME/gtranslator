/*
 * Copyright (C) 2008  
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

#ifndef __TRANSLATION_MEMORY_UI_H__
#define __TRANSLATION_MEMORY_UI_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_TRANSLATION_MEMORY_UI	        (gtranslator_translation_memory_ui_get_type ())
#define GTR_TRANSLATION_MEMORY_UI(o)	        (G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_TRANSLATION_MEMORY_UI, GtranslatorTranslationMemoryUi))
#define GTR_TRANSLATION_MEMORY_UI_CLASS(k)      (G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_TRANSLATION_MEMORY_UI, GtranslatorTranslationMemoryUiClass))
#define GTR_IS_TRANSLATION_MEMORY_UI(o)	        (G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_TRANSLATION_MEMORY_UI))  
#define GTR_IS_TRANSLATION_MEMORY_UI_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_TRANSLATION_MEMORY_UI))
#define GTR_TRANSLATION_MEMORY_UI_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_TRANSLATION_MEMORY_UI, GtranslatorTranslationMemoryUiClass))

/* Private structure type */
typedef struct _GtranslatorTranslationMemoryUiPrivate	GtranslatorTranslationMemoryUiPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorTranslationMemoryUi		GtranslatorTranslationMemoryUi;

struct _GtranslatorTranslationMemoryUi
{
        GtkScrolledWindow parent_instance;
	
	/*< private > */
	GtranslatorTranslationMemoryUiPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorTranslationMemoryUiClass	GtranslatorTranslationMemoryUiClass;

struct _GtranslatorTranslationMemoryUiClass
{
        GtkScrolledWindowClass parent_class;
};

/*
 * Public methods
 */
GType		 gtranslator_translation_memory_ui_get_type	   (void) G_GNUC_CONST;

GType		 gtranslator_translation_memory_ui_register_type   (GTypeModule * module);

GtkWidget	*gtranslator_translation_memory_ui_new	           (GtkWidget *tab);

G_END_DECLS

#endif /* __TRANSLATION_MEMORY_UI_H__ */
