/**
*
* (C) 2000 Fatih Demir -- kabalak / kabalak@gmx.net
*
* This is distributed under the GNU GPL V 2.0 or higher which can be
*  found in the file COPYING for further studies.
*
* Enjoy this piece of software, brain-crack and other nice things.
*
* WARNING: Trying to decode the source-code may be hazardous for all your
*	future development in direction to better IQ-Test rankings!
*
* PSC: This has been completely written with vim; the best editor of all.
*
**/

/**
* Include the header.
**/
#include <libgtranslator/stylistics.h>

/**
* This routines sets all the nice colors.
**/
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
	/**
	* Store the color values.
	**/
#define set_color(key,val) \
	g_snprintf(path, 23, "colors/%s_%s", section, key);\
	gtranslator_config_set_int(path, val);
	set_color("red", red);
	set_color("green", green);
	set_color("blue", blue);
	set_color("alpha", alpha);
#undef set_color
}

/**
* And this sets the values to the given GnomeColorPicker.
**/
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
	/**
	* Store the color values.
	**/
#define get_color(key,val) \
	g_snprintf(path, 23, "colors/%s_%s", section, key);\
	val = gtranslator_config_get_int(path);
	get_color("red", red);
	get_color("green", green);
	get_color("blue", blue);
	get_color("alpha", alpha);
#undef get_color
	/**
	* Set the values in the given GnomeColorPicker.
	**/
	gnome_color_picker_set_i16(colorpicker, red,
		green, blue, alpha);
}		

/**
* This function parses the stored informations about font/colors
*  and restores them.
**/
void gtranslator_set_style(GtkWidget *widget)
{
	/**
	* The stuff around this.
	**/
	GtkStyle *style;
	GdkColor *fg, *bg;
	gchar *fontname;

	gtranslator_config_init();
	/**
	* Get the fontname.
	**/
	fontname=gtranslator_config_get_string("font/name");
	/**
	* Test the font name.
	**/
	if(!fontname)
	{
		g_warning(_("No font set! Using default font"));
		/**
		* The default font for gtranslator.
		**/
		fontname=_("-misc-fixed-medium-r-normal-*-*-120-*-*-c-*-iso8859-1");
	}
	/**
	* Check if the widget is existent and get it's style.
	**/
	if(widget) {
		/**
		* Get the style of the given widget.
		**/
		style=gtk_style_copy(gtk_widget_get_style(widget));
	} else {
		g_error ("No widgets defined to manipulate their style");
		return;
	}
	/**
	* Get the background and set the values.
	**/
	bg=&style->base[0];
	/**
	* Get the values for the style as integers, so multiplicate the
	*  float values from the GnomeColorPicker with 65536 to get the nearer
	*   integer value guaranteed by the cast.
	**/
	bg->red=gtranslator_config_get_int("colors/bg_red");
	bg->green=gtranslator_config_get_int("colors/bg_green");
	bg->blue=gtranslator_config_get_int("colors/bg_blue");
	/**
	* And the same for the foreground.
	**/
	fg=&style->text[0];
	fg->red=gtranslator_config_get_int("colors/fg_red");
	fg->green=gtranslator_config_get_int("colors/fg_green");
	fg->blue=gtranslator_config_get_int("colors/fg_blue");
	/**
	* Release the font.
	**/
	gdk_font_unref(style->font);
	/**
	* And set the stored font.
	**/
	style->font=gdk_font_load(fontname);
	if(!style->font)
		g_error(_("Couldn't even load default font!"));
	/**
	* Finally set the style
	**/
	gtk_widget_set_style(GTK_WIDGET(widget), style);
	/**
	* Unref/clean up the style.
	**/
	gtk_style_unref(style);
	g_free(fontname);
	gtranslator_config_close();
}
