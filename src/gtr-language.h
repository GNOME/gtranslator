/*
 * gtr-language.h
 * This file is part of gtranslator
 *
 * Copyright (C) 2003, 2004 - Christian Persch
 * Copyright (C) 2006 - Paolo Maggi
 * Copyright (C) 2011 - Ignacio Casal Quinteiro
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

#ifndef __GTR_LANGUAGE_H__
#define __GTR_LANGUAGE_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _GtrLanguage GtrLanguage;

#define GTR_TYPE_LANGUAGE (gtr_language_get_type ())

GType                    gtr_language_get_type           (void) G_GNUC_CONST;

/* These should not be used, they are just to make python bindings happy */
GtrLanguage             *gtr_language_copy               (const GtrLanguage *lang);
void                     gtr_language_free               (GtrLanguage       *lang);

const GSList            *gtr_language_get_languages      (void);

const gchar             *gtr_language_get_code           (const GtrLanguage *lang);

const gchar             *gtr_language_get_name           (const GtrLanguage *lang);

const gchar             *gtr_language_get_plural_form    (const GtrLanguage *lang);

G_END_DECLS

#endif  /* __GTR_LANGUAGE_H__ */
