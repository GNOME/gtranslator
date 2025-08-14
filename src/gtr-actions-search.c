/*
 * This file is part of gtranslator
 *
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * Copyright (C) 1998, 1999 Alex Roberts, Evan Lawrence
 * Copyright (C) 2000, 2001 Chema Celorio, Paolo Maggi
 * Copyright (C) 2002-2006 Paolo Maggi
 *
 * Based in gedit-commands-search.c
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <glib/gi18n.h>
#include <gdk/gdkkeysyms.h>
#include <gtksourceview/gtksource.h>

#include "gtr-actions.h"
#include "gtr-msg.h"
#include "gtr-utils.h"
#include "gtr-view.h"
#include "gtr-window.h"
#include "gtr-search-bar.h"


#define GTR_LAST_SEARCH_DATA_KEY	"gtr-last-search-data-key"

/* data stored in the document
 * to persist the last searched
 * words and options
 */

static void
last_search_data_free (LastSearchData *data)
{
  g_free (data->find_text);
  g_free (data->replace_text);
  g_free (data);
}

void
last_search_data_set (LastSearchData * data, GtrSearchBar * dialog)
{
  const gchar *str;

  str = gtr_search_bar_get_search_text (dialog);
  if (str != NULL && *str != '\0')
    {
      g_free (data->find_text);
      data->find_text = gtr_utils_unescape_search_text (str);
    }

  str = gtr_search_bar_get_replace_text (dialog);
  if (str != NULL && *str != '\0')
    {
      g_free (data->replace_text);
      data->replace_text = gtr_utils_unescape_search_text (str);
    }

  data->original_text = gtr_search_bar_get_original_text (dialog);
  data->translated_text = gtr_search_bar_get_translated_text (dialog);
  data->match_case = gtr_search_bar_get_match_case (dialog);
  data->entire_word = gtr_search_bar_get_entire_word (dialog);
  data->wrap_around = gtr_search_bar_get_wrap_around (dialog);
}

void
search_bar_set_last_find_text (GtrSearchBar * dialog,
                                  LastSearchData * data)
{
  if (data->find_text)
    gtr_search_bar_set_search_text (dialog, data->find_text);
}

void
search_bar_set_last_replace_text (GtrSearchBar * dialog,
                                  LastSearchData * data)
{
  if (data->replace_text)
    gtr_search_bar_set_replace_text (dialog, data->replace_text);
}

void
search_bar_set_last_options (GtrSearchBar *   dialog,
                             LastSearchData * data)
{
  gtr_search_bar_set_original_text (dialog, data->original_text);
  gtr_search_bar_set_translated_text (dialog, data->translated_text);
  gtr_search_bar_set_match_case (dialog, data->match_case);
  gtr_search_bar_set_entire_word (dialog, data->entire_word);
  gtr_search_bar_set_wrap_around (dialog, data->wrap_around);
}

/*
 * Used to get the old search data and store the new values.
 */
static void
restore_last_searched_data (GtrSearchBar * dialog, GtrTab * tab)
{
  LastSearchData *data;

  data = g_object_get_data (G_OBJECT (tab), GTR_LAST_SEARCH_DATA_KEY);

  if (data == NULL)
    {
      data = g_new0 (LastSearchData, 1);
      last_search_data_set (data, dialog);

      g_object_set_data (G_OBJECT (tab), GTR_LAST_SEARCH_DATA_KEY, data);

      g_object_weak_ref (G_OBJECT (tab),
                         (GWeakNotify) last_search_data_free, data);
    }
  else
    {
      last_search_data_set (data, dialog);
    }
}

int
run_search (GtrView * view, int offset)
{
  GtkSourceBuffer *doc;
  GtkTextIter start_iter;
  GtkTextIter match_start;
  GtkTextIter match_end;
  int found = 0;

  g_return_val_if_fail (GTR_IS_VIEW (view), FALSE);

  doc = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)));

  gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER (doc), &start_iter, offset);
  found = gtr_view_search_forward (view,
                                   &start_iter,
                                   NULL, &match_start, &match_end);

  if (found)
    {
      gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER (doc), &match_start);

      gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER (doc),
                                         "selection_bound", &match_end);
      return gtk_text_iter_get_offset (&match_end);
    }
  else
    {
      gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER (doc), &start_iter);
    }

  return 0;
}

static gboolean
_msg_equal_func (GtrMsg *msg, GRegex *regex, gboolean original_text,
                 gboolean translated_text)
{
  const gchar *original = NULL;
  const gchar *translated = NULL;
  gboolean found = FALSE;

  original = gtr_msg_get_msgid (msg);
  translated = gtr_msg_get_msgstr (msg);
  // TODO: search also in plurals msgs

  if (original_text)
    found = found || g_regex_match (regex, original, 0, NULL);
  if (translated_text)
    found = found || g_regex_match (regex, translated, 0, NULL);

  return found;
}

static gint
find_in_selection_model (GtrWindow *window, GtrSearchBar *searchbar,
                         gint current_pos, gboolean search_backwards)
{
  GtrTab *tab = gtr_window_get_active_tab (window);
  GListModel *model = G_LIST_MODEL (gtr_tab_get_selection_model (tab));
  g_autoptr (GRegex) regex = gtr_search_bar_regex (searchbar);

  gboolean original_text;
  gboolean translated_text;
  gboolean wrap_around;
  gboolean found = FALSE;
  gint pos = -1;
  gint i = 0;
  gint nitems = g_list_model_get_n_items (model);

  /* Views where find */
  original_text = gtr_search_bar_get_original_text (searchbar);
  translated_text = gtr_search_bar_get_translated_text (searchbar);
  wrap_around = gtr_search_bar_get_wrap_around (searchbar);

  i = current_pos;
  if (search_backwards)
    {
      i = current_pos - 1;
      if (wrap_around && i <= 0)
        i = nitems - 1;
    }
  else
    {
      i = current_pos + 1;
      if (wrap_around && i >= nitems)
        i = 0;
    }

  // Stop when found or at the end of the list
  while (!found && i >= 0 && i < nitems && i != current_pos)
    {
      g_autoptr(GtrMsg) msg = GTR_MSG (g_list_model_get_item (model, i));
      if (_msg_equal_func (msg, regex, original_text, translated_text))
        {
          found = TRUE;
          pos = i;
          break;
        }

      if (search_backwards)
        i--;
      else
        i++;

      if (wrap_around)
        {
          i = i % nitems;
          if (i < 0)
            i = nitems + i;
        }
    }

  return pos;
}

gboolean
find_in_list (GtrWindow *window, GtrSearchBar *searchbar)
{
  GList *views, *list;
  gchar *search_text;
  const gchar *entry_text;
  gboolean original_text;
  gboolean translated_text;
  gboolean match_case;
  gboolean entire_word;
  guint flags = 0;
  guint old_flags = 0;

  // Use static to keep the search state between different calls
  static GList *viewsaux = NULL;
  // offset of last found word in textview
  static int offset = 0;

  entry_text = gtr_search_bar_get_search_text (searchbar);

  /* Views where find */
  original_text = gtr_search_bar_get_original_text (searchbar);
  translated_text = gtr_search_bar_get_translated_text (searchbar);

  /* Flags */
  match_case = gtr_search_bar_get_match_case (searchbar);
  entire_word = gtr_search_bar_get_entire_word (searchbar);

  if (!original_text && !translated_text)
    return FALSE;

  // Search the word in gtr-views widgets
  views = gtr_window_get_all_views (window, original_text, translated_text);
  list = views;

  GTR_SEARCH_SET_CASE_SENSITIVE (flags, match_case);
  GTR_SEARCH_SET_ENTIRE_WORD (flags, entire_word);

  // Set the search options in all views
  while (list != NULL)
    {
      search_text =
        gtr_view_get_search_text (GTR_VIEW (list->data), &old_flags);

      if ((search_text == NULL) ||
          (strcmp (search_text, entry_text) != 0) || (flags != old_flags))
        {
          gtr_view_set_search_text (GTR_VIEW (list->data), entry_text, flags);
        }

      g_free (search_text);
      list = list->next;
    }

  // Do the search in all views, keeping the state from previous calls.
  if (viewsaux == NULL)
    viewsaux = views;
  else
    g_free (views);

  while (viewsaux != NULL)
    {
      offset = run_search (GTR_VIEW (viewsaux->data), offset);
      if (offset > 0)
        return TRUE;
      if (viewsaux->next != NULL)
        {
          viewsaux = viewsaux->next;
        }
      else
        {
          g_free (viewsaux);
          viewsaux = NULL;
        }
    }

  return FALSE;
}

void
do_find (GtrSearchBar *searchbar, GtrWindow *window, gboolean search_backwards)
{
  GtrTab *tab;
  gint pos = -1;
  gint current_pos = -1;
  gboolean found = FALSE;

  /* Used to store search options */
  tab = gtr_window_get_active_tab (window);

  found = find_in_list (window, searchbar);
  // It appears in the current selected message, so nothing to do here
  if (found)
    {
      gtr_search_bar_set_found (searchbar, TRUE);
      return;
    }

  // Not found in the current message, so look for the followings
  current_pos
      = gtk_single_selection_get_selected (gtr_tab_get_selection_model (tab));
  pos = find_in_selection_model (window, searchbar, current_pos,
                                 search_backwards);
  gtr_search_bar_set_found (searchbar, pos > -1);
  if (pos >= 0)
    {
      gtk_single_selection_set_selected (gtr_tab_get_selection_model (tab),
                                         pos);
      found = find_in_list (window, searchbar);
    }

  restore_last_searched_data (searchbar, tab);
}

static void
replace_selected_text (GtkTextBuffer * buffer, const gchar * replace)
{
  g_return_if_fail (gtk_text_buffer_get_selection_bounds
                    (buffer, NULL, NULL));
  g_return_if_fail (replace != NULL);

  gtk_text_buffer_begin_user_action (buffer);

  gtk_text_buffer_delete_selection (buffer, FALSE, TRUE);

  gtk_text_buffer_insert_at_cursor (buffer, replace, strlen (replace));

  gtk_text_buffer_end_user_action (buffer);
}

void
do_replace (GtrSearchBar * dialog, GtrWindow * window)
{
  GtrView *view;
  const gchar *search_entry_text;
  const gchar *replace_entry_text;
  gchar *unescaped_search_text;
  gchar *unescaped_replace_text;
  gchar *selected_text = NULL;
  gboolean match_case;
  GtrTab *tab;

  view = gtr_window_get_active_view (window);
  if (view == NULL)
    return;

  tab = gtr_window_get_active_tab (window);
  gtr_tab_find_replace (tab, TRUE);

  search_entry_text = gtr_search_bar_get_search_text (dialog);
  g_return_if_fail ((search_entry_text) != NULL);
  g_return_if_fail ((*search_entry_text) != '\0');

  /* replace text may be "", we just delete */
  replace_entry_text = gtr_search_bar_get_replace_text (dialog);
  g_return_if_fail ((replace_entry_text) != NULL);

  unescaped_search_text = gtr_utils_unescape_search_text (search_entry_text);

  gtr_view_get_selected_text (view, &selected_text, NULL);

  match_case = gtr_search_bar_get_match_case (dialog);

  if ((selected_text == NULL) ||
      (match_case && (strcmp (selected_text, unescaped_search_text) != 0)) ||
      (!match_case && !g_utf8_caselessnmatch (selected_text,
                                              unescaped_search_text,
                                              strlen (selected_text),
                                              strlen (unescaped_search_text))
       != 0))
    {
      do_find (dialog, window, FALSE);
      g_free (unescaped_search_text);
      g_free (selected_text);
      gtr_tab_find_replace (tab, FALSE);

      return;
    }

  unescaped_replace_text =
    gtr_utils_unescape_search_text (replace_entry_text);
  replace_selected_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)),
                         unescaped_replace_text);

  g_free (unescaped_search_text);
  g_free (selected_text);
  g_free (unescaped_replace_text);

  do_find (dialog, window, FALSE);
  gtr_tab_find_replace (tab, FALSE);
}

void
do_replace_all (GtrSearchBar *searchbar, GtrWindow * window)
{
  GtrTab *tab;
  g_autoptr (GList) views = NULL;
  GList *l;
  GList *current_msg, *aux;
  const gchar *search_entry_text;
  const gchar *replace_entry_text;
  gboolean match_case;
  gboolean entire_word;
  guint flags = 0;
  gint count = 0;

  tab = gtr_window_get_active_tab (window);
  gtr_tab_find_replace (tab, TRUE);

  /* Get only translated textviews */
  views = gtr_window_get_all_views (window, FALSE, TRUE);

  current_msg = gtr_po_get_current_message (gtr_tab_get_po (tab));

  g_return_if_fail (views != NULL);
  g_return_if_fail (current_msg != NULL);

  l = views;
  aux = current_msg;

  search_entry_text = gtr_search_bar_get_search_text (searchbar);
  g_return_if_fail ((search_entry_text) != NULL);
  g_return_if_fail ((*search_entry_text) != '\0');

  /* replace text may be "", we just delete all occurrences */
  replace_entry_text = gtr_search_bar_get_replace_text (searchbar);
  g_return_if_fail ((replace_entry_text) != NULL);

  match_case = gtr_search_bar_get_match_case (searchbar);
  entire_word = gtr_search_bar_get_entire_word (searchbar);

  GTR_SEARCH_SET_CASE_SENSITIVE (flags, match_case);
  GTR_SEARCH_SET_ENTIRE_WORD (flags, entire_word);


  do
    {
      while (l)
        {
          count += gtr_view_replace_all (GTR_VIEW (l->data),
                                         search_entry_text,
                                         replace_entry_text, flags);
          l = l->next;
        }

      l = views;

      if (aux->next == NULL)
        aux = g_list_first (aux);
      else
        aux = aux->next;
      gtr_tab_message_go_to (tab, aux->data, TRUE, GTR_TAB_MOVE_NONE);
    }
  while (current_msg != aux);

  gtr_tab_message_go_to (tab, aux->data, FALSE, GTR_TAB_MOVE_NONE);

  restore_last_searched_data (searchbar, tab);
  gtr_tab_find_replace (tab, FALSE);
}
