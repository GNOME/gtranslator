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

#include "color-schemes.h"
#include "prefs.h"
#include "preferences.h"
#include "stylistics.h"
#include "utils.h"

#include <libgnome/gnome-defs.h>
#include <libgnome/gnome-i18n.h>

gchar *get_path_from_type(ColorType Type);


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
		case COLOR_SPECIAL_CHAR:
			section="special_char";
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
		case COLOR_SPELL_ERROR:
			section="spell_error";
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
	gchar *path=NULL;
	
	if(Type==COLOR_FG)
	{
		path="colors/own_fg";
	}
	else if(Type==COLOR_BG)
	{
		path="colors/own_bg";
	}

	g_return_if_fail(path!=NULL);

	gnome_color_picker_get_i8(GNOME_COLOR_PICKER(colorpicker),
		&red, &green, &blue, NULL);
	
	/*
	 * Store the color values.
	 */
	g_snprintf(spec, 8, "#%02x%02x%02x", red, green, blue);
	gtranslator_config_set_string(path, spec);
}

/*
 * Restores the GnomeColorPicker colors.
 */
void gtranslator_color_values_get(GnomeColorPicker *colorpicker, ColorType Type)
{
	GdkColor color;
	gchar *spec;
	gchar *path=NULL;
	
	if(Type==COLOR_FG)
	{
		path="colors/own_fg";
	}
	else if(Type==COLOR_BG)
	{
		path="colors/own_bg";
	}
	
	g_return_if_fail(path!=NULL);

	/*
	 * Restore the color values from the preferences.
	 */
	spec=gtranslator_config_get_string(path);

	if(!spec)
	{
		/*
		 * Possibly convert the old color specs to their new place.
		 */
		gtranslator_utils_old_colors_to_new_location();
		
		/*
		 * And reread the specs from the now correct location.
		 */
		spec=gtranslator_config_get_string(path);
	}

	if(spec)
	{
		gdk_color_parse(spec, &color);
		gnome_color_picker_set_i16(colorpicker, color.red,
			color.green, color.blue, 0);
		g_free(spec);
	}
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
	if(GtrPreferences.use_own_specs)
	{
		spec=gtranslator_config_get_string("colors/own_bg");
	}
	else
	{
		spec=gtranslator_config_get_string("colors/bg");
	}
	
	if(spec)
	{
		gdk_color_parse(spec, &style->base[0]);
		g_free(spec);
	}
	
	/*
	 * Do the same for the foreground.
	 */
	if(GtrPreferences.use_own_specs)
	{
		spec=gtranslator_config_get_string("colors/own_fg");
	}
	else
	{
		spec=gtranslator_config_get_string("colors/fg");
	}
	
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
GdkColor *gtranslator_get_color_from_type(ColorType type)
{
	g_return_val_if_fail(type < COLOR_END, NULL);

	if(!colors_initialized)
	{
		colors_initialized=TRUE;
		gtranslator_colors_initialize();
	}

	return &colors[type];
}

void gtranslator_colors_initialize(void)
{
	/*
	 * If no theme is specified/given use the default colors.
	 */ 
	if(!theme)
	{
		gtranslator_color_scheme_restore_default();

		theme=gtranslator_color_scheme_load_from_prefs();
	}
	
	gdk_color_parse(theme->fg,		&colors[COLOR_FG]);
	gdk_color_parse(theme->bg,		&colors[COLOR_BG]);
	gdk_color_parse(theme->special_char,	&colors[COLOR_SPECIAL_CHAR]);
	gdk_color_parse(theme->hotkey,		&colors[COLOR_HOTKEY]);
	gdk_color_parse(theme->c_format,	&colors[COLOR_C_FORMAT]);
	gdk_color_parse(theme->number,		&colors[COLOR_NUMBER]);
	gdk_color_parse(theme->punctuation,	&colors[COLOR_PUNCTUATION]);
	gdk_color_parse(theme->keyword,		&colors[COLOR_KEYWORD]);
	gdk_color_parse(theme->address,		&colors[COLOR_ADDRESS]);
	gdk_color_parse(theme->special,		&colors[COLOR_SPECIAL]);
	gdk_color_parse(theme->spell_error,	&colors[COLOR_SPELL_ERROR]);
}

