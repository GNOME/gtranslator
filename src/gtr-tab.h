/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *               2008  Igalia 
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
 *   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *   Pablo Sanxiao <psanxiao@gmail.com>
 */
#pragma once

#include <adwaita.h>
#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "gtr-context.h"
#include "gtr-msg.h"
#include "gtr-message-table.h"
#include "gtr-po.h"
#include "gtr-view.h"
#include "gtr-search-bar.h"

G_BEGIN_DECLS

#define GTR_TYPE_TAB (gtr_tab_get_type ())

G_DECLARE_DERIVABLE_TYPE (GtrTab, gtr_tab, GTR, TAB, AdwBin)

struct _GtrTabClass
{
  AdwBinClass parent_class;

  void (*showed_message) (GtrTab * tab, GtrMsg * msg);
  void (*message_changed) (GtrTab * tab, GtrMsg * msg);
  void (*message_edition_finished) (GtrTab * tab, GtrMsg * msg);
  void (*selection_changed) (GtrTab * tab);
  void (*searchbar_toggled) (GtrTab * tab, gboolean revealed);
};

typedef enum
{
  GTR_TAB_MOVE_NONE,
  GTR_TAB_MOVE_NEXT,
  GTR_TAB_MOVE_PREV
} GtrTabMove;

typedef enum
{
  GTR_TAB_PLACEMENT_NONE = 0,
  GTR_TAB_PLACEMENT_TOP,
  GTR_TAB_PLACEMENT_BOTTOM,
  GTR_TAB_PLACEMENT_RIGHT,
  GTR_TAB_PLACEMENT_LEFT,
  GTR_TAB_PLACEMENT_CENTER,
  GTR_TAB_PLACEMENT_FLOATING
} GtrTabPlacement;

/*
 * Public methods
 */
GtrTab *gtr_tab_new (GtrPo * po, GtkWindow *window);

GtrPo *gtr_tab_get_po (GtrTab * tab);

gint gtr_tab_get_active_trans_tab (GtrTab * tab);

GtrContextPanel *gtr_tab_get_context_panel (GtrTab * tab);

GtrView *gtr_tab_get_active_view (GtrTab * tab);

GList *gtr_tab_get_all_views (GtrTab * tab,
                            gboolean original, gboolean translated);

void gtr_tab_message_go_to (GtrTab * tab,
                            GtrMsg * to_go,
                            gboolean searching, GtrTabMove move);

GtrTab *gtr_tab_get_from_document (GtrPo * po);

gboolean gtr_tab_get_autosave_enabled (GtrTab * tab);

void gtr_tab_set_autosave_enabled (GtrTab * tab, gboolean enable);

gint gtr_tab_get_autosave_interval (GtrTab * tab);

void gtr_tab_set_autosave_interval (GtrTab * tab, gint interval);

void gtr_tab_clear_msgstr_views (GtrTab * tab);

void gtr_tab_copy_to_translation (GtrTab * tab);

void gtr_tab_copy_original (GtrTab * tab);

void gtr_tab_block_movement (GtrTab * tab);

void gtr_tab_unblock_movement (GtrTab * tab);

void gtr_tab_go_to_next (GtrTab * tab);

void gtr_tab_go_to_prev (GtrTab * tab);

void gtr_tab_go_to_first (GtrTab * tab);

void gtr_tab_go_to_last (GtrTab * tab);

gboolean gtr_tab_go_to_next_fuzzy (GtrTab * tab);

gboolean gtr_tab_go_to_prev_fuzzy (GtrTab * tab);

gboolean gtr_tab_go_to_next_untrans (GtrTab * tab);

gboolean gtr_tab_go_to_prev_untrans (GtrTab * tab);

gboolean gtr_tab_go_to_next_fuzzy_or_untrans (GtrTab * tab);

gboolean gtr_tab_go_to_prev_fuzzy_or_untrans (GtrTab * tab);

void gtr_tab_go_to_number (GtrTab * tab, gint number);
void gtr_tab_go_to_position (GtrTab *tab, gint position);

void gtr_tab_set_info_bar (GtrTab * tab, GtkWidget * infobar);

void gtr_tab_set_info (GtrTab * tab, const char * primary, const char * secondary);

GtrMsg * gtr_tab_get_msg (GtrTab *tab);

void gtr_tab_set_progress (GtrTab *tab, gint trans, gint untrans, gint fuzzy);

void gtr_tab_find_replace (GtrTab *tab, gboolean set);

void gtr_tab_focus_search_bar (GtrTab *tab);
void gtr_tab_show_hide_search_bar (GtrTab * tab, gboolean show);
void gtr_tab_find_next (GtrTab * tab);
void gtr_tab_find_prev (GtrTab * tab);
void gtr_tab_find_set_replace (GtrTab * tab, gboolean replace);

GtkSingleSelection * gtr_tab_get_selection_model (GtrTab *tab);
GListStore *gtr_tab_get_model (GtrTab *tab);

/* notebook */
void gtr_tab_enable_upload (GtrTab *tab, gboolean enable);
void gtr_tab_enable_find_button (GtrTab *tab, gboolean enable);

GtkWindow *gtr_tab_get_window (GtrTab *tab);

/* Semi-public methods */

gchar *_gtr_tab_get_tooltips (GtrTab *tab);

gboolean _gtr_tab_can_close (GtrTab * tab);

G_END_DECLS
