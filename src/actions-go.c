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

#include "actions.h"
#include "po.h"
#include "tab.h"
#include "window.h"


void 
gtranslator_message_go_to_first(GtkAction *action,
				GtranslatorWindow *window)
{
	GtranslatorTab *current;
	GtranslatorPo *po;
	
	current = gtranslator_window_get_active_tab(window);
	po = gtranslator_tab_get_po(current);
	gtranslator_tab_message_go_to(current,
				      g_list_first(gtranslator_po_get_current_message(po)));
	set_sensitive_according_to_message(window, po);
}

void 
gtranslator_message_go_to_previous(GtkAction *action,
				   GtranslatorWindow *window)
{
	GtranslatorTab *current;
	GtranslatorPo *po;
	
	current = gtranslator_window_get_active_tab(window);
	po = gtranslator_tab_get_po(current);
	gtranslator_tab_message_go_to(current,
				      g_list_previous(gtranslator_po_get_current_message(po)));
	set_sensitive_according_to_message(window, po);
}

void 
gtranslator_message_go_to_next(GtkAction *action,
			       GtranslatorWindow *window)
{
	GtranslatorTab *current;
	GtranslatorPo *po;
	
	current = gtranslator_window_get_active_tab(window);
	po = gtranslator_tab_get_po(current);
	gtranslator_tab_message_go_to(current,
				  g_list_next(gtranslator_po_get_current_message(po)));
	set_sensitive_according_to_message(window, po);
}

void 
gtranslator_message_go_to_last(GtkAction *action,
			       GtranslatorWindow *window)
{
	GtranslatorTab *current;
	GtranslatorPo *po;
	
	current = gtranslator_window_get_active_tab(window);
	po = gtranslator_tab_get_po(current);
	gtranslator_tab_message_go_to(current,
				      g_list_last(gtranslator_po_get_current_message(po)));
	set_sensitive_according_to_message(window, po);
}

void 
gtranslator_message_go_to_no(GtkAction *action,
			     GtranslatorWindow *window)
{
	/*gtranslator_message_go_to(g_list_nth(current_page->po->messages,
					     GPOINTER_TO_UINT(number)));*/
}

void
gtranslator_message_go_to_next_fuzzy(GtkAction *action,
				     GtranslatorWindow *window)
{
	GtranslatorTab *current;
	GtranslatorPo *po;
	GList *msg;
	
	current = gtranslator_window_get_active_tab(window);
	po = gtranslator_tab_get_po(current);
	msg = gtranslator_po_get_next_fuzzy(po);
	if(msg != NULL)
	{
		gtranslator_tab_message_go_to(current, msg);
		set_sensitive_according_to_message(window, po);
	}
}

void
gtranslator_message_go_to_prev_fuzzy(GtkAction *action,
				     GtranslatorWindow *window)
{
	GtranslatorTab *current;
	GtranslatorPo *po;
	GList *msg;
	
	current = gtranslator_window_get_active_tab(window);
	po = gtranslator_tab_get_po(current);
	msg = gtranslator_po_get_prev_fuzzy(po);
	if(msg != NULL)
	{
		gtranslator_tab_message_go_to(current, msg);
		set_sensitive_according_to_message(window, po);
	}
}

void
gtranslator_message_go_to_next_untranslated(GtkAction *action,
					    GtranslatorWindow *window)
{
	GtranslatorTab *current;
	GtranslatorPo *po;
	GList *msg;
	
	current = gtranslator_window_get_active_tab(window);
	po = gtranslator_tab_get_po(current);
	msg = gtranslator_po_get_next_untrans(po);
	if(msg != NULL)
	{
		gtranslator_tab_message_go_to(current, msg);
		set_sensitive_according_to_message(window, po);
	}
}

void
gtranslator_message_go_to_prev_untranslated(GtkAction *action,
					    GtranslatorWindow *window)
{
	GtranslatorTab *current;
	GtranslatorPo *po;
	GList *msg;
	
	current = gtranslator_window_get_active_tab(window);
	po = gtranslator_tab_get_po(current);
	msg = gtranslator_po_get_prev_untrans(po);
	if(msg != NULL)
	{
		gtranslator_tab_message_go_to(current, msg);
		set_sensitive_according_to_message(window, po);
	}
}
