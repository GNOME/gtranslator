/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
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

#include "stylistics.h"
#include "preferences.h"

#include <libgnome/gnome-defs.h>
#include <libgnome/gnome-i18n.h>

static gchar* get_path_from_type(ColorType Type);
static void init_colors(void);

static GdkColor colors[COLOR_END];

/*
 * Given a color type, returns a configuration path. Must be freed
 */
gchar* get_path_from_type(ColorType Type)
{
	gchar *section;
	switch(Type)
	{
		case COLOR_FG:
			section="fg";
			break;
		case COLOR_BG:
			section="bg";
			break;
		case COLOR_DOT_CHAR:
			section="dot_char";
			break;
		case COLOR_HOTKEY:
			section="hotkey";
			break;
		case COLOR_C_FORMAT:
			section="c_format";
			break;
		case COLOR_NUMBER:
			section="number";
			break;
		case COLOR_PUNCTUATION:
			section="punctuation";
			break;
		case COLOR_SPECIAL:
			section="special";
			break;
		case COLOR_ADDRESS:
			section="address";
			break;
		case COLOR_KEYWORD:
			section="keyword";
			break;
		default:
			return NULL;
	}
	return g_strdup_printf("colors/%s", section);
}

/*
 * Saves the colors from the given GnomeColorPicker.
 */
void gtranslator_color_values_set(GnomeColorPicker *colorpicker, ColorType Type)
{
	guint8 red, green, blue;
	gchar spec[8];
	gchar *path;
	
	path=get_path_from_type(Type);
	g_return_if_fail(path!=NULL);

	gnome_color_picker_get_i8(GNOME_COLOR_PICKER(colorpicker),
		&red, &green, &blue, NULL);
	
	/*
	 * Store the color values.
	 */
	g_snprintf(spec, 8, "#%02x%02x%02x", red, green, blue);
	gtranslator_config_set_string(path, spec);
	g_free(path);
}

/*
 * Restores the GnomeColorPicker colors.
 */
void gtranslator_color_values_get(GnomeColorPicker *colorpicker, ColorType Type)
{
	GdkColor color;
	gchar *spec;
	gchar *path;
	
	path=get_path_from_type(Type);
	g_return_if_fail(path!=NULL);

	/*
	 * Restore the color values from the preferences.
	 */
	spec=gtranslator_config_get_string(path);
	if(spec)
	{
		gdk_color_parse(spec, &color);
		gnome_color_picker_set_i8(colorpicker, color.red,
			color.green, color.blue, 0);
		g_free(spec);
	}
	g_free(path);
}		

/*
 * Sets the style informations for the given widget
 *  (foreground, background and font).
 */
void gtranslator_set_style(GtkWidget *widget)
{
	GtkStyle *style;
	gchar *spec;
	GdkFont *font;
	gchar *fontname;

	g_return_if_fail(widget != NULL);

	/*
	 * Copy the style informations of the given widget.
	 */
	style=gtk_style_copy(gtk_widget_get_style(widget));
	
	/*
	 * Set up the stored values for the background from the preferences.
	 */
	spec=gtranslator_config_get_string("colors/bg");
	if(spec)
	{
		gdk_color_parse(spec, &style->base[0]);
		g_free(spec);
	}
	
	/*
	 * Do the same for the foreground.
	 */
	spec=gtranslator_config_get_string("colors/fg");
	if(spec)
	{
		gdk_color_parse(spec, &style->text[0]);
		g_free(spec);
	}
	
	/*
	 * The stored font setting.
	 */
	fontname=gtranslator_config_get_string("font/name");

	if(!fontname)
	{
		g_message(_("No font set! Using default font"));
		/*
		 * Use gtranslator's font in this case.
		 */
		fontname=_("-misc-fixed-medium-r-normal-*-*-120-*-*-c-*-iso8859-1");
		gtranslator_config_set_string("font/name", fontname);
	}
	/*
	 * Set the font for the widget, if possible.
	 */
	font=gdk_font_load(fontname);
	if(font)
	{
		/*
		 * Unref old widget font.
		 */
		gdk_font_unref(style->font);
		style->font=font;
	}
	
	/*
	 * The final step: set the widget style.
	 */
	gtk_widget_set_style(widget, style);
	
	/*
	 * Clean up
	 */
	gtk_style_unref(style);
	g_free(fontname);
}

static gboolean colors_initialized=FALSE;

/*
 * Return the requested GdkColor
 */
GdkColor *get_color_from_type(ColorType type)
{
	g_return_val_if_fail(type<COLOR_END, NULL);

	if(!colors_initialized)
	{
		colors_initialized=TRUE;
		init_colors();
	}

	return &colors[type];
}

void init_colors(void)
{
	gdk_color_parse("white",	&colors[COLOR_FG]);
	gdk_color_parse("black",	&colors[COLOR_BG]);
	gdk_color_parse("gray",		&colors[COLOR_DOT_CHAR]);
	gdk_color_parse("blue",		&colors[COLOR_HOTKEY]);
	gdk_color_parse("red",		&colors[COLOR_C_FORMAT]);
	gdk_color_parse("orange",	&colors[COLOR_NUMBER]);
	gdk_color_parse("wheat",	&colors[COLOR_PUNCTUATION]);
	gdk_color_parse("maroon",	&colors[COLOR_SPECIAL]);
}

