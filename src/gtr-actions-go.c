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
 *     MERCHANMSGILITY or FITNESS FOR A PARTICULAR PURMSGSE.  See the
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
#include "gtr-jump-dialog.h"
#include "gtr-po.h"
#include "gtr-tab.h"
#include "gtr-window.h"


void
gtr_message_go_to_first (GtkAction * action, GtrWindow * window)
{
  GtrTab *current;
  GtrPo *po;

  current = gtr_window_get_active_tab (window);
  po = gtr_tab_get_po (current);
  gtr_tab_go_to_first (current);
  _gtr_window_set_sensitive_according_to_message (window, po);
}

void
gtr_message_go_to_previous (GtkAction * action, GtrWindow * window)
{
  GtrTab *current;
  GtrPo *po;

  current = gtr_window_get_active_tab (window);
  po = gtr_tab_get_po (current);
  gtr_tab_go_to_prev (current);
  _gtr_window_set_sensitive_according_to_message (window, po);
}

void
gtr_message_go_to_next (GtkAction * action, GtrWindow * window)
{
  GtrTab *current;
  GtrPo *po;

  current = gtr_window_get_active_tab (window);
  po = gtr_tab_get_po (current);
  gtr_tab_go_to_next (current);
  _gtr_window_set_sensitive_according_to_message (window, po);
}

void
gtr_message_go_to_last (GtkAction * action, GtrWindow * window)
{
  GtrTab *current;
  GtrPo *po;

  current = gtr_window_get_active_tab (window);
  po = gtr_tab_get_po (current);
  gtr_tab_go_to_last (current);
  _gtr_window_set_sensitive_according_to_message (window, po);
}

void
gtr_message_go_to_next_fuzzy (GtkAction * action, GtrWindow * window)
{
  GtrTab *current;
  GtrPo *po;

  current = gtr_window_get_active_tab (window);
  po = gtr_tab_get_po (current);
  if (gtr_tab_go_to_next_fuzzy (current))
    _gtr_window_set_sensitive_according_to_message (window, po);
}

void
gtr_message_go_to_prev_fuzzy (GtkAction * action, GtrWindow * window)
{
  GtrTab *current;
  GtrPo *po;

  current = gtr_window_get_active_tab (window);
  po = gtr_tab_get_po (current);
  if (gtr_tab_go_to_prev_fuzzy (current))
    _gtr_window_set_sensitive_according_to_message (window, po);
}

void
gtr_message_go_to_next_untranslated (GtkAction * action, GtrWindow * window)
{
  GtrTab *current;
  GtrPo *po;

  current = gtr_window_get_active_tab (window);
  po = gtr_tab_get_po (current);
  if (gtr_tab_go_to_next_untrans (current))
    _gtr_window_set_sensitive_according_to_message (window, po);
}

void
gtr_message_go_to_prev_untranslated (GtkAction * action, GtrWindow * window)
{
  GtrTab *current;
  GtrPo *po;

  current = gtr_window_get_active_tab (window);
  po = gtr_tab_get_po (current);
  if (gtr_tab_go_to_prev_untrans (current))
    _gtr_window_set_sensitive_according_to_message (window, po);
}

void
gtr_message_go_to_next_fuzzy_or_untranslated (GtkAction * action,
                                              GtrWindow * window)
{
  GtrTab *current;
  GtrPo *po;

  current = gtr_window_get_active_tab (window);
  po = gtr_tab_get_po (current);
  if (gtr_tab_go_to_next_fuzzy_or_untrans (current))
    _gtr_window_set_sensitive_according_to_message (window, po);
}

void
gtr_message_go_to_prev_fuzzy_or_untranslated (GtkAction * action,
                                              GtrWindow * window)
{
  GtrTab *current;
  GtrPo *po;

  current = gtr_window_get_active_tab (window);
  po = gtr_tab_get_po (current);
  if (gtr_tab_go_to_prev_fuzzy_or_untrans (current))
    _gtr_window_set_sensitive_according_to_message (window, po);
}

void
gtr_message_jump (GtkAction * action, GtrWindow * window)
{
  gtr_show_jump_dialog (window);
}
