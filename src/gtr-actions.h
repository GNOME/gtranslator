/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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
 *
 */

#ifndef __ACTIONS_H__
#define __ACTIONS_H__

#include <gtk/gtk.h>
#include <gio/gio.h>

#include "gtr-window.h"
#include "gtr-search-bar.h"
#include <libsoup/soup.h>

G_BEGIN_DECLS
/*File*/

typedef struct _LastSearchData LastSearchData;
struct _LastSearchData
{
  gchar *find_text;
  gchar *replace_text;

  gint original_text:1;
  gint translated_text:1;
  gint fuzzy_messages:1;
  gint match_case:1;
  gint entire_word:1;
  gint backwards:1;
  gint wrap_around:1;
};

void gtr_want_to_save_current_dialog (GtrWindow * window, void (*callback)(GtrWindow *));

void gtr_open_file_dialog (GtrWindow * window);

void gtr_save_file_as_dialog (GtrWindow * window);

void gtr_upload_file_dialog (GtrWindow * window);

void gtr_close_tab (GtrTab * tab, GtrWindow * window);

void gtr_file_close (GtrWindow * window);

void gtr_file_quit (GtrWindow * window);

void gtr_actions_load_locations (GtrWindow * window,
                                 GSList * locations);

void _gtr_actions_file_close_all (GtrWindow * window);

void _gtr_actions_file_save_all (GtrWindow * window);

/*Edit*/
void gtr_actions_edit_cut (GtrWindow * window);

void gtr_actions_edit_copy (GtrWindow * window);

void gtr_actions_edit_paste (GtrWindow * window);

void gtr_actions_edit_header (GtrWindow * window);

void gtr_actions_fix_plurals (GtrWindow *window);

void gtr_message_copy_to_translation (GtrWindow * window);
void gtr_message_copy_original (GtrWindow * window);

void gtr_message_status_toggle_fuzzy (GtrWindow * window);

void gtr_actions_edit_clear (GtrWindow * window);

/*Go*/
void gtr_message_go_to_first (GtrWindow * window);

void gtr_message_go_to_previous (GtrWindow * window);

void gtr_message_go_to_next (GtrWindow * window);

void gtr_message_go_to_last (GtrWindow * window);

void gtr_message_go_to_next_fuzzy (GtrWindow * window);

void gtr_message_go_to_prev_fuzzy (GtrWindow * window);

void gtr_message_go_to_next_untranslated (GtrWindow * window);

void gtr_message_go_to_prev_untranslated (GtrWindow * window);

void gtr_message_go_to_next_fuzzy_or_untranslated (GtrWindow * window);

void gtr_message_go_to_prev_fuzzy_or_untranslated (GtrWindow * window);

/*Search*/
void _gtr_actions_search_find (GtrWindow * window);

void _gtr_actions_search_replace (GtrWindow * window);

void gtr_page_stop_search (GtrTab * tab,
                           GtrSearchBar * search_bar);

void gtr_page_notify_child_revealed (GtrTab * tab,
                                GParamSpec * pspec,
                                GtkRevealer * revealer);

void last_search_data_set (LastSearchData * data,
                           GtrSearchBar * dialog);

void search_bar_set_last_find_text (GtrSearchBar * dialog,
                                    LastSearchData * data);

void search_bar_set_last_replace_text (GtrSearchBar * dialog,
                                       LastSearchData * data);

void search_bar_set_last_options (GtrSearchBar * dialog,
                                  LastSearchData * data);

gboolean run_search (GtrView * view,
                    gboolean follow);

gboolean find_in_list (GtrWindow *window, GtrSearchBar *searchbar);

void do_find (GtrSearchBar * dialog,
              GtrWindow * window,
              gboolean search_backwards);

void do_replace (GtrSearchBar * dialog,
                 GtrWindow * window);

void do_replace_all (GtrSearchBar * dialog,
                     GtrWindow * window);

/*Documents*/
//void gtr_actions_documents_next_document (GtrWindow * window);

//void gtr_actions_documents_previous_document (GtrWindow * window);

/*Help*/
void gtr_window_show_home_page (gpointer useless);

G_END_DECLS
#endif
