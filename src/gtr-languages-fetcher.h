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

#pragma once

#include <gtk/gtk.h>
#include <adwaita.h>

G_BEGIN_DECLS

#define GTR_TYPE_LANGUAGES_FETCHER (gtr_languages_fetcher_get_type ())

G_DECLARE_FINAL_TYPE (GtrLanguagesFetcher, gtr_languages_fetcher,
                      GTR, LANGUAGES_FETCHER, AdwPreferencesGroup)


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

const gchar           *gtr_languages_fetcher_get_plural_form      (GtrLanguagesFetcher *fetcher);

void                   gtr_languages_fetcher_set_plural_form      (GtrLanguagesFetcher *fetcher,
                                                                   const gchar         *plural_form);

G_END_DECLS
