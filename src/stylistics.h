/*
 * (C) 2000-2007 	Fatih Demir <kabalak@kabalak.net>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 * 			Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef GTR_STYLISTICS_H
#define GTR_STYLISTICS_H 1

#include <gtk/gtkwidget.h>
#include <gdk/gdk.h>

/*
 * gtranslator specific style functions.
 */ 

/*
 * Applies font settings for GtkTextViews.
 */
void gtranslator_style_set_font(GtkWidget *widget, gchar *font);

/*
 * Applies color settings for GtkTextViews.
 */
void gtranslator_style_set_color(GtkWidget *widget, GdkColor *color);


#endif
