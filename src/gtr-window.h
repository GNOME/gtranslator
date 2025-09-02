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

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <adwaita.h>

#include "gtr-header.h"
#include "gtr-tab.h"
#include "gtr-view.h"

G_BEGIN_DECLS

#define GTR_TYPE_WINDOW (gtr_window_get_type ())

G_DECLARE_FINAL_TYPE (GtrWindow, gtr_window, GTR, WINDOW, AdwApplicationWindow)

GtrTab *gtr_window_create_tab (GtrWindow * window, GtrPo * po);

void gtr_window_set_po (GtrWindow * window, GtrPo * po);

GtrTab *gtr_window_get_active_tab (GtrWindow * window);

GList *gtr_window_get_all_tabs (GtrWindow * window);

GtrHeader * gtr_window_get_header_from_active_tab (GtrWindow * window);

GtkWidget *gtr_window_get_statusbar (GtrWindow * window);

GtrView *gtr_window_get_active_view (GtrWindow * window);

GList *gtr_window_get_all_views (GtrWindow * window,
                               gboolean original, gboolean translated);

GtkWidget *gtr_window_get_tab_from_location (GtrWindow * window, GFile * location);

void gtr_window_remove_tab (GtrWindow * window);

GtrTranslationMemory * gtr_window_get_tm (GtrWindow *window);
void gtr_window_show_tm_dialog (GtrWindow *window);

void gtr_window_remove_all_pages (GtrWindow *window);

void gtr_window_tm_keybind (GtrWindow *window, GSimpleAction *action);

void gtr_window_show_focus_search_bar (GtrWindow *window, gboolean show);
void gtr_window_toggle_search_bar (GtrWindow *window);

void gtr_window_add_toast (GtrWindow *window, AdwToast *toast);
void gtr_window_add_toast_msg (GtrWindow *window, const char *message);

/** stack app states **/
void gtr_window_show_projects (GtrWindow *window);
void gtr_window_show_poeditor (GtrWindow *window);
void gtr_window_show_dlteams (GtrWindow *window);
void gtr_window_show_greeter (GtrWindow *window);

void gtr_window_save_current_tab (GtrWindow *window);

void gtr_window_open_file_in_browser (GtrWindow  *self,
                                      const char *vcs_web,
                                      const char *module,
                                      const char *file,
                                      const char *branch_name,
                                      int         line_number);

G_END_DECLS
