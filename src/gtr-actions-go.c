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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gi18n.h>

#include "gtr-actions.h"
#include "gtr-tab.h"
#include "gtr-window.h"


void
gtr_message_go_to_first (GtrWindow * window)
{
  GtrTab *current;

  current = gtr_window_get_active_tab (window);
  gtr_tab_go_to_first (current);
}

void
gtr_message_go_to_previous (GtrWindow * window)
{
  GtrTab *current;

  current = gtr_window_get_active_tab (window);
  gtr_tab_go_to_prev (current);
}

void
gtr_message_go_to_next (GtrWindow * window)
{
  GtrTab *current;

  current = gtr_window_get_active_tab (window);
  gtr_tab_go_to_next (current);
}

void
gtr_message_go_to_last (GtrWindow * window)
{
  GtrTab *current;

  current = gtr_window_get_active_tab (window);
  gtr_tab_go_to_last (current);
}

void
gtr_message_go_to_next_fuzzy (GtrWindow * window)
{
  GtrTab *current;

  current = gtr_window_get_active_tab (window);
  gtr_tab_go_to_next_fuzzy (current);
}

void
gtr_message_go_to_prev_fuzzy (GtrWindow * window)
{
  GtrTab *current;

  current = gtr_window_get_active_tab (window);
  gtr_tab_go_to_prev_fuzzy (current);
}

void
gtr_message_go_to_next_untranslated (GtrWindow * window)
{
  GtrTab *current;

  current = gtr_window_get_active_tab (window);
  gtr_tab_go_to_next_untrans (current);
}

void
gtr_message_go_to_prev_untranslated (GtrWindow * window)
{
  GtrTab *current;

  current = gtr_window_get_active_tab (window);
  gtr_tab_go_to_prev_untrans (current);
}

void
gtr_message_go_to_next_fuzzy_or_untranslated (GtrWindow * window)
{
  GtrTab *current;

  current = gtr_window_get_active_tab (window);
  gtr_tab_go_to_next_fuzzy_or_untrans (current);
}

void
gtr_message_go_to_prev_fuzzy_or_untranslated (GtrWindow * window)
{
  GtrTab *current;

  current = gtr_window_get_active_tab (window);
  gtr_tab_go_to_prev_fuzzy_or_untrans (current);
}
