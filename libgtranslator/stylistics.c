/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gmx.net>
 *			Gediminas Paulauskas <menesis@delfi.lt>
 *
 * libgtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 * libgtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <libgtranslator/libgtranslator.h>

/*
 * Saves the colors from the given GnomeColorPicker.
 */
void gtranslator_color_values_set(GnomeColorPicker *colorpicker,
	ColorValueType Type)
{
	gushort red, green, blue, alpha;
	gchar path[23];
	gchar *section;
	switch(Type)
	{
		case COLOR_VALUE_FG:
			section="fg";
			break;
		case COLOR_VALUE_BG:
			section="bg";
			break;	
		case COLOR_VALUE_DOT_CHAR:
			section="dot_char";
			break;
		case COLOR_VALUE_END_CHAR:
			section="end_char";
			break;
		case COLOR_VALUE_SELECTION:
			section="selection";
			break;	
		default:
			g_warning(_("No color path found!"));
			return;
	}
	
	gnome_color_picker_get_i16(GNOME_COLOR_PICKER(colorpicker),
		&red, &green, &blue, &alpha);
	
	/*
	 * Store the color values.
	 */
#define set_color(key,val) \
	g_snprintf(path, 23, "colors/%s_%s", section, key);\
	gtranslator_config_set_int(path, val);
	set_color("red", red);
	set_color("green", green);
	set_color("blue", blue);
	set_color("alpha", alpha);
#undef set_color
}

/*
 * Restores the GnomeColorPicker colors.
 */
void gtranslator_color_values_get(GnomeColorPicker *colorpicker,
	ColorValueType Type)
{
	gushort red, green, blue, alpha;
	gchar path[23];
	gchar *section;
	switch(Type)
	{
		case COLOR_VALUE_FG:
			section="fg";
			break;
		case COLOR_VALUE_BG:
			section="bg";
			break;	
		case COLOR_VALUE_DOT_CHAR:
			section="dot_char";
			break;
		case COLOR_VALUE_END_CHAR:
			section="end_char";
			break;
		case COLOR_VALUE_SELECTION:
			section="selection";
			break;	
		default:
			g_warning(_("No color path found!"));
			return;
	}
	
	/*
	 * Restore the color values from the preferences.
	 */
#define get_color(key,val) \
	g_snprintf(path, 23, "colors/%s_%s", section, key);\
	val = gtranslator_config_get_int(path);
	get_color("red", red);
	get_color("green", green);
	get_color("blue", blue);
	get_color("alpha", alpha);
#undef get_color
	
	gnome_color_picker_set_i16(colorpicker, red,
		green, blue, alpha);
}		

/*
 * Sets the style informations for the given widget
 *  (foreground, background and font).
 */
void gtranslator_set_style(GtkWidget *widget)
{
	GtkStyle *style;
	GdkColor *fg, *bg;
	gchar *fontname;

	gtranslator_config_init();
	
	/*
	 * The stored font setting.
	 */
	fontname=gtranslator_config_get_string("font/name");

	if(!fontname)
	{
		g_warning(_("No font set! Using default font"));
		/*
		 * Use gtranslator's font in this case.
		 */
		fontname=_("-misc-fixed-medium-r-normal-*-*-120-*-*-c-*-iso8859-1");
	}
	/*
	 * Check the given widget and get the widget style.
	 */
	if(widget)
	{
		/*
		 * Copy the style informations of the given widget.
		 */
		style=gtk_style_copy(gtk_widget_get_style(widget));
	}
	else
	{
		g_error ("No widgets defined to manipulate their style");
		return;
	}
	
	/*
	 * Get the background informations/values of the widget.
	 */
	bg=&style->base[0];
	
	/*
	 * Set up the stored values for the background from the preferences.
	 */
	bg->red=gtranslator_config_get_int("colors/bg_red");
	bg->green=gtranslator_config_get_int("colors/bg_green");
	bg->blue=gtranslator_config_get_int("colors/bg_blue");
	
	/*
	 * Do the same for the foreground.
	 */
	fg=&style->text[0];
	fg->red=gtranslator_config_get_int("colors/fg_red");
	fg->green=gtranslator_config_get_int("colors/fg_green");
	fg->blue=gtranslator_config_get_int("colors/fg_blue");
	
	/*
	 * Unref/free the widget font.
	 */
	gdk_font_unref(style->font);
	
	/*
	 * Set the font for the widget from the stored fontname.
	 */
	style->font=gdk_font_load(fontname);
	if(!style->font)
	{	
		g_error(_("Couldn't even load default font!"));
	}
	
	/*
	 * The final step: set the widget style.
	 */
	gtk_widget_set_style(GTK_WIDGET(widget), style);
	
	/*
	 * Clean up and close gtranslator's config interface.
	 */
	gtk_style_unref(style);
	g_free(fontname);
	gtranslator_config_close();
}
