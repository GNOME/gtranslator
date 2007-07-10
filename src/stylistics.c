/*
 * (C) 2000-2001 	Fatih Demir <kabalak@kabalak.net>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Joe Man <trmetal@yahoo.com.hk>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "preferences.h"
#include "stylistics.h"

#include <gtk/gtk.h>
#include <glib/gi18n.h>

void 
gtranslator_style_set_font(GtkWidget *widget, gchar *font)
{
	PangoFontDescription *pango;
	pango = pango_font_description_from_string(font);
	gtk_widget_modify_font(widget, pango);
}

void 
gtranslator_style_set_color(GtkWidget *widget, GdkColor *color)
{
	gtk_widget_modify_text(widget, GTK_STATE_NORMAL, color);
}





