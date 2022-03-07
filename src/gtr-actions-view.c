/*
 * Copyright (C) 2007 Seán de Búrca <leftmostcat@gmail.com>
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
 */

#include "gtr-actions.h"

void
gtr_actions_view_context (GtrWindow * window)
{
  GtrTab *tab;
  GtkWidget *context;

  tab = gtr_window_get_active_tab (window);
  context = GTK_WIDGET (gtr_tab_get_context_panel (tab));

  gtk_widget_show(context);
}
