/*
 * Copyright (C) 2020  Priyanka Saggu <priyankasaggu11929@gmail.com>
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
 */
#pragma once

#include <gtksourceview/gtksource.h>
#include <adwaita.h>

G_BEGIN_DECLS

#define GTR_TYPE_SEARCH_BAR (gtr_search_bar_get_type())

G_DECLARE_FINAL_TYPE (GtrSearchBar, gtr_search_bar, GTR, SEARCH_BAR, AdwBin)

enum
{
  GTR_SEARCH_BAR_FIND_RESPONSE = 100,
  GTR_SEARCH_BAR_REPLACE_RESPONSE,
  GTR_SEARCH_BAR_REPLACE_ALL_RESPONSE
};

typedef enum
{
  GTR_SEARCH_BAR_MSG_INFO,
  GTR_SEARCH_BAR_MSG_ERROR
} GtrSearchBarMsg;

GtkEntry        *gtr_search_bar_get_search       (GtrSearchBar *self);
gboolean         gtr_search_bar_get_show_options (GtrSearchBar *self);
void             gtr_search_bar_set_show_options (GtrSearchBar *self,
                                                  gboolean      show_options);
gboolean         gtr_search_bar_get_replace_mode (GtrSearchBar *self);
void             gtr_search_bar_set_replace_mode (GtrSearchBar *self,
                                                  gboolean            replace_mode);
const gchar     *gtr_search_bar_get_search_text (GtrSearchBar *dialog);
void             gtr_search_bar_set_search_text (GtrSearchBar *dialog,
                                                 const gchar *text);

const gchar     *gtr_search_bar_get_replace_text (GtrSearchBar *dialog);
void             gtr_search_bar_set_replace_text (GtrSearchBar *dialog,
                                                  const gchar *text);

gboolean         gtr_search_bar_get_original_text (GtrSearchBar *dialog);
void             gtr_search_bar_set_original_text (GtrSearchBar *dialog,
                                                   gboolean match_case);

gboolean         gtr_search_bar_get_translated_text (GtrSearchBar *dialog);
void             gtr_search_bar_set_translated_text (GtrSearchBar *dialog,
                                                     gboolean match_case);

gboolean         gtr_search_bar_get_match_case (GtrSearchBar *dialog);
void             gtr_search_bar_set_match_case (GtrSearchBar *dialog,
                                                gboolean match_case);

gboolean         gtr_search_bar_get_entire_word (GtrSearchBar * dialog);
void             gtr_search_bar_set_entire_word (GtrSearchBar *dialog,
                                                 gboolean entire_word);

gboolean         gtr_search_bar_get_backwards (GtrSearchBar *dialog);
void             gtr_search_bar_set_backwards (GtrSearchBar *dialog,
                                               gboolean backwards);

gboolean         gtr_search_bar_get_wrap_around (GtrSearchBar *dialog);
void             gtr_search_bar_set_wrap_around (GtrSearchBar *dialog,
                                                 gboolean wrap_around);

void             gtr_search_bar_show_message (GtrSearchBar *dialog,
                                              char *message,
                                              GtrSearchBarMsg info_type);

void             gtr_search_bar_present_with_time (GtrSearchBar * dialog,
                                                   guint32 timestamp);

void             gtr_search_bar_find_next (GtrSearchBar *self);
void             gtr_search_bar_find_prev (GtrSearchBar *self);

void             gtr_search_bar_set_found (GtrSearchBar *self, gboolean found);

GRegex *gtr_search_bar_regex (GtrSearchBar *self);

G_END_DECLS
