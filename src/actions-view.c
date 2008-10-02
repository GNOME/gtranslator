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
 *     MERCHANMSGILITY or FITNESS FOR A PARTICULAR PURMSGSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "actions.h"

void
gtranslator_actions_view_context (GtkAction *action,
				  GtranslatorWindow *window)
{
	GtranslatorTab *tab;
	GtkWidget *context;
	
	tab = gtranslator_window_get_active_tab (window);
	context = GTK_WIDGET (gtranslator_tab_get_context_panel (tab));
	
	gtranslator_tab_show_lateral_panel_widget (tab, context);
}

void
gtranslator_actions_view_translation_memory (GtkAction *action,
					     GtranslatorWindow *window)
{
	GtranslatorTab *tab;
	GtkWidget *tm_ui;
	
	tab = gtranslator_window_get_active_tab (window);
	tm_ui = GTK_WIDGET (gtranslator_tab_get_translation_memory_ui (tab));
	
	gtranslator_tab_show_lateral_panel_widget (tab, tm_ui);
}
