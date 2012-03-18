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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GTR_TRANSLATION_MEMORY_DIALOG_H__
#define __GTR_TRANSLATION_MEMORY_DIALOG_H__

#include <gtk/gtk.h>
#include "gtr-translation-memory.h"

G_BEGIN_DECLS

#define GTR_TYPE_TRANSLATION_MEMORY_DIALOG             (gtr_translation_memory_dialog_get_type ())
#define GTR_TRANSLATION_MEMORY_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_TRANSLATION_MEMORY_DIALOG, GtrTranslationMemoryDialog))
#define GTR_TRANSLATION_MEMORY_DIALOG_CONST(obj)       (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_TRANSLATION_MEMORY_DIALOG, GtrTranslationMemoryDialog const))
#define GTR_TRANSLATION_MEMORY_DIALOG_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GTR_TYPE_TRANSLATION_MEMORY_DIALOG, GtrTranslationMemoryDialogClass))
#define GTR_IS_TRANSLATION_MEMORY_DIALOG(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTR_TYPE_TRANSLATION_MEMORY_DIALOG))
#define GTR_IS_TRANSLATION_MEMORY_DIALOG_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_TYPE_TRANSLATION_MEMORY_DIALOG))
#define GTR_TRANSLATION_MEMORY_DIALOG_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GTR_TYPE_TRANSLATION_MEMORY_DIALOG, GtrTranslationMemoryDialogClass))

typedef struct _GtrTranslationMemoryDialog         GtrTranslationMemoryDialog;
typedef struct _GtrTranslationMemoryDialogClass    GtrTranslationMemoryDialogClass;
typedef struct _GtrTranslationMemoryDialogPrivate  GtrTranslationMemoryDialogPrivate;

struct _GtrTranslationMemoryDialog
{
  GtkDialog parent;

  GtrTranslationMemoryDialogPrivate *priv;
};

struct _GtrTranslationMemoryDialogClass
{
  GtkDialogClass parent_class;
};

GType gtr_translation_memory_dialog_get_type (void) G_GNUC_CONST;

GtkWidget *gtr_translation_memory_dialog_new (GtrTranslationMemory *translation_memory);

G_END_DECLS

#endif /* __GTR_TRANSLATION_MEMORY_DIALOG_H__ */
