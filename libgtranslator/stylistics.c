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
#include <stylistics.h>

/**
* This routines sets all the nice colors.
**/
void gtranslator_color_values_set(GnomeColorPicker *colorpicker,
	ColorValueType Type)
{
	gdouble red, green, blue, alpha;
	gchar *path=g_new0(gchar,1);
	gnome_color_picker_get_d(GNOME_COLOR_PICKER(colorpicker),
		&red, &green, &blue, &alpha);
	/**
	* Initialize the configuration stuff and store the given
	*  doubles via the fake gfloats.
	**/
	gtranslator_config_init();
	/**
	* Get the different paths via the Type defined in the
	*  request.
	**/
	switch(Type)
	{
		case COLOR_VALUE_FG:
			path=g_strdup_printf("colors/%s",
				"fg");
			break;
		case COLOR_VALUE_BG:
			path=g_strdup_printf("colors/%s",
				"bg");
			break;	
		case COLOR_VALUE_DOT_CHAR:
			path=g_strdup_printf("colors/%s",
				"dot_char");
			break;
		case COLOR_VALUE_END_CHAR:
			path=g_strdup_printf("colors/%s",
				"end_char");
			break;
		case COLOR_VALUE_SELECTION:
			path=g_strdup_printf("colors/%s",
				"selection");
			break;	
		default:
			break;
	}
	/**
	* Just a sanity check.
	**/
	if(!path)
	{
		g_warning(_("No color path found!"));
		return;
	}
	/**
	* Store the color values.
	**/
	gtranslator_config_set_float(g_strdup_printf("%s/%s",
		path, "red"), red);
	gtranslator_config_set_float(g_strdup_printf("%s/%s",
		path, "green"), green);
	gtranslator_config_set_float(g_strdup_printf("%s/%s",
		path, "blue"), blue);
	gtranslator_config_set_float(g_strdup_printf("%s/%s",
		path, "alpha"), alpha);			
	/**
	* And close the preferences now.
	**/	
	gtranslator_config_close();
	/**
	* Free the gchar at least.
	**/
	if(path)
	{
		g_free(path);
	}	
}

/**
* And this sets the values to the given GnomeColorPicker.
**/
void gtranslator_color_values_get(GnomeColorPicker *colorpicker,
	ColorValueType Type)
{
	gdouble red, green, blue, alpha;
	gchar *path=g_new0(gchar,1);
	/**
	* Initialize the config-stuff.
	**/
	gtranslator_config_init();
	/**
	* Again the switch tree from above.
	**/
	switch(Type)
	{
		case COLOR_VALUE_FG:
			path=g_strdup_printf("colors/%s",
				"fg");
			break;
		case COLOR_VALUE_BG:
			path=g_strdup_printf("colors/%s",
				"bg");
			break;	
		case COLOR_VALUE_DOT_CHAR:
			path=g_strdup_printf("colors/%s",
				"dot_char");
			break;
		case COLOR_VALUE_END_CHAR:
			path=g_strdup_printf("colors/%s",
				"end_char");
			break;
		case COLOR_VALUE_SELECTION:
			path=g_strdup_printf("colors/%s",
				"selection");
			break;	
		default:
			break;
	}
	/**
	* Just a sanity check.
	**/
	if(!path)
	{
		g_warning(_("No color path found!"));
		return;
	}
	/**
	* Fill up the colors.
	**/
	red=gtranslator_config_get_double(g_strdup_printf("%s/%s",
		path, "red"));
	green=gtranslator_config_get_double(g_strdup_printf("%s/%s",
		path, "green"));
	blue=gtranslator_config_get_double(g_strdup_printf("%s/%s",
		path, "blue"));
	alpha=gtranslator_config_get_double(g_strdup_printf("%s/%s",
		path, "alpha"));
	/**
	* The same as above again, close the preferences.
	**/	
	gtranslator_config_close();
	/**
	* Set the values in the given GnomeColorPicker.
	**/
	gnome_color_picker_set_d(colorpicker, red,
		green, blue, alpha);
	/**
	* And free the gchar.
	**/
	if(path)
	{
		g_free(path);
	}		
}		

/**
* This function parses the stored informations about font/colors
*  and restores them.
**/
void gtranslator_set_style(GtkWidget *one, GtkWidget *two)
{
	/**
	* The stuff around this.
	**/
	GtkStyle *style;
	GdkColor *fg, *bg;
	gchar *fontname=g_new0(gchar,1);
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
	if(!gdk_font_load(fontname))
	{
		g_error(_("Couldn't even load default font!"));
	}
	/**
	* Check if the widget one is existent and get it's style.
	**/
	if(one)
	{
		/**
		* Get the style of the first given widget.
		**/
		style=gtk_style_copy(gtk_widget_get_style(one));
	}
	else
	{
		/**
		* Test also the second widget.
		**/
		if(two==NULL)
		{
			g_warning(
			_("No widgets defined to manipulate their style"));
			return;
		}
		/**
		* Try getting the style from the second widget.
		**/
		style=gtk_style_copy(gtk_widget_get_style(two));
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
	bg->red=(int) (gtranslator_config_get_float("colors/bg/red")*65536);
	bg->green=(int) (gtranslator_config_get_float("colors/bg/green")*65536);
	bg->blue=(int) (gtranslator_config_get_float("colors/bg/blue")*65536);
	/**
	* And the same for the foreground.
	**/
	fg=&style->text[0];
	fg->red=(int) (gtranslator_config_get_float("colors/fg/red")*65536);
	fg->green=(int) (gtranslator_config_get_float("colors/fg/green")*65536);
	fg->blue=(int) (gtranslator_config_get_float("colors/fg/blue")*65536);
	/**
	* Release the font.
	**/
	gdk_font_unref(style->font);
	/**
	* And set the stored font.
	**/
	style->font=gdk_font_load(fontname);
	/**
	* Test for both of the widgets and set then 
	*  their styles after the conditional.
	**/
	if(one)
	{
		gtk_widget_set_style(GTK_WIDGET(one), style);
	}
	if(two)
	{
		gtk_widget_set_style(GTK_WIDGET(two), style);
	}
	/**
	* Unref/clean up the style.
	**/
	gtk_style_unref(style);
}
