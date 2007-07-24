/*
 * gucharmap.c - Character map side-pane for gtranslator
 * 
 * Copyright (C) 2006 Steve Frécinaux
 *		 2007 Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include "charmap-panel.h"
#include "gui.h"
#include "page.h"

#include <gucharmap/gucharmap-table.h>
#include <gucharmap/gucharmap-unicode-info.h>

#define GLADE_GUCHARMAP_BOX "gucharmap_box"


static void
on_table_status_message (GucharmapTable *chartable,
			 gpointer message)
{
	pop_statusbar_data(NULL, NULL);

	if (message)
		push_statusbar_data(NULL, message);
}

on_table_set_active_char (GucharmapTable *chartable,
			  gunichar wc,
			  gpointer useless)
{
	GString *gs;
	const gchar *temp;
	const gchar **temps;
	gint i;

	gs = g_string_new (NULL);
	g_string_append_printf (gs, "U+%4.4X %s", wc, 
				gucharmap_get_unicode_name (wc));

	temps = gucharmap_get_nameslist_equals (wc);
	if (temps)
	{
		g_string_append_printf (gs, "   = %s", temps[0]);
		for (i = 1;  temps[i];  i++)
			g_string_append_printf (gs, "; %s", temps[i]);
		g_free (temps);
	}

	temps = gucharmap_get_nameslist_stars (wc);
	if (temps)
	{
		g_string_append_printf (gs, "   \342\200\242 %s", temps[0]);
		for (i = 1;  temps[i];  i++)
			g_string_append_printf (gs, "; %s", temps[i]);
		g_free (temps);
	}

	on_table_status_message (chartable, gs->str);
	g_string_free (gs, TRUE);
}

static gboolean
on_table_focus_out_event (GtkWidget      *drawing_area,
			  GdkEventFocus  *event,
			  gpointer useless)
{
	on_table_status_message (NULL, NULL);
	return FALSE;
}

static void
on_table_activate (GucharmapTable *chartable, 
		   gunichar wc, 
		   gpointer useless)
{
	GtkTextView   *view;
	GtkTextBuffer *document;
	GtkTextIter start, end;
	gchar buffer[6];
	gchar length;
	
	g_return_if_fail (gucharmap_unichar_validate (wc));
	
	//Is needed a func that returns the trans_msgstr active
	//view = GTK_TEXT_VIEW (gedit_window_get_active_view (window));
	view = GTK_TEXT_VIEW(current_page->trans_msgstr[0]);
	
	if (!view || !gtk_text_view_get_editable (view))
		return;
	
	document = gtk_text_view_get_buffer (view);
	
	g_return_if_fail (document != NULL);
	
	length = g_unichar_to_utf8 (wc, buffer);

	gtk_text_buffer_begin_user_action (document);
		
	gtk_text_buffer_get_selection_bounds (document, &start, &end);

	gtk_text_buffer_delete_interactive (document, &start, &end, TRUE);
	if (gtk_text_iter_editable (&start, TRUE))
		gtk_text_buffer_insert (document, &start, buffer, length);
	
	gtk_text_buffer_end_user_action (document);
}

void
gtranslator_gucharmap_create()
{
	GtkWidget *panel;
	GtkWidget *gucharmap_box;
	GucharmapTable *table;

	panel = gtranslator_charmap_panel_new();
	table = gtranslator_charmap_panel_get_table(GTR_CHARMAP_PANEL(panel));
	gucharmap_box = glade_xml_get_widget(glade, GLADE_GUCHARMAP_BOX);
	
	gtk_box_pack_start(GTK_BOX(gucharmap_box), panel, TRUE, TRUE, 0);
	
	g_signal_connect (table,
			  "status-message",
			  G_CALLBACK (on_table_status_message),
			  NULL);

	g_signal_connect (table,
			  "set-active-char",
			  G_CALLBACK (on_table_set_active_char),
			  NULL);
	
	/* Note: GucharmapTable does not provide focus-out-event ... */
	g_signal_connect (table->drawing_area,
			  "focus-out-event",
			  G_CALLBACK (on_table_focus_out_event),
			  NULL);
	
	g_signal_connect (table,
			  "activate", 
			  G_CALLBACK (on_table_activate),
			  NULL);
	
	gtk_widget_show_all(panel);
}
