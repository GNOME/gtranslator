/*
 * gtr-languages-fetcher.h
 * This file is part of gtranslator
 *
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

#ifndef __GTR_LANGUAGES_FETCHER_H__
#define __GTR_LANGUAGES_FETCHER_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTR_TYPE_LANGUAGES_FETCHER		(gtr_languages_fetcher_get_type ())
#define GTR_LANGUAGES_FETCHER(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_LANGUAGES_FETCHER, GtrLanguagesFetcher))
#define GTR_LANGUAGES_FETCHER_CONST(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_LANGUAGES_FETCHER, GtrLanguagesFetcher const))
#define GTR_LANGUAGES_FETCHER_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GTR_TYPE_LANGUAGES_FETCHER, GtrLanguagesFetcherClass))
#define GTR_IS_LANGUAGES_FETCHER(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTR_TYPE_LANGUAGES_FETCHER))
#define GTR_IS_LANGUAGES_FETCHER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_TYPE_LANGUAGES_FETCHER))
#define GTR_LANGUAGES_FETCHER_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GTR_TYPE_LANGUAGES_FETCHER, GtrLanguagesFetcherClass))

typedef struct _GtrLanguagesFetcher		GtrLanguagesFetcher;
typedef struct _GtrLanguagesFetcherClass	GtrLanguagesFetcherClass;

struct _GtrLanguagesFetcherClass
{
  GtkBoxClass parent_class;

  void (* changed) (GtrLanguagesFetcher *fetcher);
};

GType                  gtr_languages_fetcher_get_type             (void) G_GNUC_CONST;

GtkWidget             *gtr_languages_fetcher_new                  (void);

const gchar           *gtr_languages_fetcher_get_language_name    (GtrLanguagesFetcher *fetcher);

void                   gtr_languages_fetcher_set_language_name    (GtrLanguagesFetcher *fetcher,
                                                                   const gchar         *name);

const gchar           *gtr_languages_fetcher_get_language_code    (GtrLanguagesFetcher *fetcher);

void                   gtr_languages_fetcher_set_language_code    (GtrLanguagesFetcher *fetcher,
                                                                   const gchar         *code);

const gchar           *gtr_languages_fetcher_get_charset          (GtrLanguagesFetcher *fetcher);

void                   gtr_languages_fetcher_set_charset          (GtrLanguagesFetcher *fetcher,
                                                                   const gchar         *charset);

const gchar           *gtr_languages_fetcher_get_encoding         (GtrLanguagesFetcher *fetcher);

void                   gtr_languages_fetcher_set_encoding         (GtrLanguagesFetcher *fetcher,
                                                                   const gchar         *enc);

const gchar           *gtr_languages_fetcher_get_team_email       (GtrLanguagesFetcher *fetcher);

void                   gtr_languages_fetcher_set_team_email       (GtrLanguagesFetcher *fetcher,
                                                                   const gchar         *email);

const gchar           *gtr_languages_fetcher_get_plural_form      (GtrLanguagesFetcher *fetcher);

void                   gtr_languages_fetcher_set_plural_form      (GtrLanguagesFetcher *fetcher,
                                                                   const gchar         *plural_form);

G_END_DECLS

#endif /* __GTR_LANGUAGES_FETCHER_H__ */
