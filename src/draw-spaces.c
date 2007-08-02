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
 *	Based in drawspaces gedit plugin.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "draw-spaces.h"

#include <glib.h>
#include <gtk/gtk.h>

static void draw_tabs_and_spaces(GtkWidget *view, GdkEventExpose *event,
				 GtkTextIter iter, GtkTextIter end);


void
on_event_after(GtkWidget *view,
	       GdkEventExpose *event,
	       gpointer useless)
{
	if(event->type != GDK_EXPOSE ||
	   event->window != gtk_text_view_get_window(GTK_TEXT_VIEW(view),
						     GTK_TEXT_WINDOW_TEXT))
		return;
	gint x, y;
	GtkTextIter start;
	GtkTextIter end;
	
	gtk_text_view_buffer_to_window_coords(GTK_TEXT_VIEW(view), GTK_TEXT_WINDOW_TEXT,
					      event->area.x, event->area.y, &x, &y);
	gtk_text_view_get_line_at_y(GTK_TEXT_VIEW(view), &start, y, NULL);
	gtk_text_view_get_line_at_y(GTK_TEXT_VIEW(view), &end, y + event->area.height , NULL);
	gtk_text_iter_forward_to_line_end(&end);
	draw_tabs_and_spaces(view, event, start, end);
}

static void
draw_space_at_iter(cairo_t *cr,
		   GtkWidget *view,
		   GtkTextIter *iter)
{
	GdkRectangle rect;
	gint x, y;
	
	if (!GTK_IS_TEXT_VIEW(view))
		return;
	gtk_text_view_get_iter_location(GTK_TEXT_VIEW(view), iter, &rect);
	gtk_text_view_buffer_to_window_coords(GTK_TEXT_VIEW(view), GTK_TEXT_WINDOW_TEXT,
					      rect.x + rect.width /2,
					      rect.y + rect.height * 2/3, &x, &y);
	
	cairo_save(cr);
	cairo_move_to(cr, x, y);
	cairo_arc(cr, x, y, 0.8, 0, 2 * G_PI);
	cairo_restore(cr);
	cairo_stroke(cr);
}

static void
draw_nbsp_at_iter(cairo_t *cr,
		  GtkTextView *view,
		  GtkTextIter *iter)
{
	GdkRectangle rect;
	gint x, y;
	gtk_text_view_get_iter_location(view, iter, &rect);
	gtk_text_view_buffer_to_window_coords(view, GTK_TEXT_WINDOW_TEXT,
					      rect.x,
					      rect.y + rect.height / 2, &x, &y);	
	cairo_save(cr);
	cairo_move_to(cr, x+2, y-2);
	cairo_rel_line_to(cr, +7, 0);
	cairo_rel_line_to(cr, 0, 4);
	cairo_rel_move_to(cr, -3, 0);
	cairo_rel_line_to(cr, 5, 0);
	cairo_rel_line_to(cr, -2.5, 4);
	cairo_rel_line_to(cr, -2.5, -4);
	cairo_restore(cr);
	cairo_fill(cr);
	
}

static void
draw_tab_at_iter(cairo_t *cr,
		 GtkTextView *view,
		 GtkTextIter *iter)
{
	GdkRectangle rect;
	gint x, y;
	gtk_text_view_get_iter_location(view, iter, &rect);
	gtk_text_view_buffer_to_window_coords(view, GTK_TEXT_WINDOW_TEXT,
					      rect.x,
					      rect.y + rect.height * 2/3, &x, &y);
	cairo_save(cr);
	cairo_move_to(cr, x + 4, y);
	cairo_rel_line_to(cr, rect.width -8, 0);
	cairo_rel_line_to(cr, -3, -3);
	cairo_rel_move_to(cr, +3, +3);
	cairo_rel_line_to(cr, -3, +3);
	cairo_restore(cr);
	cairo_stroke(cr);
}

static void
draw_tabs_and_spaces(GtkWidget *view, GdkEventExpose *event,
		     GtkTextIter iter, GtkTextIter end)
{
	cairo_t *cr;
	GdkColor color;
	gunichar c;
	cr = gdk_cairo_create(event->window);
	//This color should be configurable
	gdk_color_parse("#000000", &color);
	gdk_cairo_set_source_color(cr, &color);
	cairo_set_line_width(cr, 0.8);
	
	while (gtk_text_iter_compare(&iter, &end) != 0)
	{
		c = gtk_text_iter_get_char(&iter);
		if(c == '\t')//TODO: Color should be set here to have a different color
			//for differents char (This should be themeable
			draw_tab_at_iter(cr, GTK_TEXT_VIEW(view), &iter);
		else if(c == '\040')
			draw_space_at_iter(cr, view, &iter);
		else if(c == '\n')
			draw_nbsp_at_iter(cr, GTK_TEXT_VIEW(view), &iter);
		if(!gtk_text_iter_forward_char(&iter))
			break;
	}
}
