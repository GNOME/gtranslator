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

#ifndef LIBGTRANSLATOR_STYLISTICS_H
#define LIBGTRANSLATOR_STYLISTICS_H 1

/*
 * gtranslator specific style functions.
 */ 

#include <libgnomeui/gnome-color-picker.h>

/*
 * ColorTypes.
 */
typedef enum {
	COLOR_FG,
	COLOR_BG,
	COLOR_SPECIAL_CHAR,
	COLOR_HOTKEY,
	COLOR_C_FORMAT,
	COLOR_NUMBER,
	COLOR_PUNCTUATION,
	COLOR_SPECIAL,
	COLOR_ADDRESS,
	COLOR_KEYWORD,
	COLOR_SPELL_ERROR,
	/* This is a fake color, should be the last */
	COLOR_END
} ColorType;

GdkColor colors[COLOR_END];
void init_colors(void);

/*
 * Saves the color values from the GnomeColorPicker.
 */
void gtranslator_color_values_set(GnomeColorPicker *colorpicker, ColorType Type);

/*
 * Restores the color values for the GnomeColorPicker.
 */
void gtranslator_color_values_get(GnomeColorPicker *colorpicker, ColorType Type);

/*
 * Applies the foreground/background/font settings for the given
 *  widget.
 */
void gtranslator_set_style(GtkWidget *widget);

GdkColor *get_color_from_type(ColorType type);

#endif
