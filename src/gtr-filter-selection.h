/*
 * Copyright (C) 2019  Daniel Garcia Moreno <danigm@gnome.org>
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

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTR_TYPE_FILTER_SELECTION (gtr_filter_selection_get_type())

G_DECLARE_DERIVABLE_TYPE (GtrFilterSelection, gtr_filter_selection, GTR, FILTER_SELECTION, GtkMenuButton)

struct _GtrFilterSelectionClass
{
  GtkMenuButtonClass parent_class;
};

GtrFilterSelection*  gtr_filter_selection_new             ();

void
gtr_filter_selection_set_text (GtrFilterSelection *selection,
                               const char *text);

void
gtr_filter_selection_set_options (GtrFilterSelection *self,
                                  GSList *options);
const char *
gtr_filter_selection_get_option (GtrFilterSelection *self);
void
gtr_filter_selection_set_option (GtrFilterSelection *self,
                                 const char         *option);

G_END_DECLS

