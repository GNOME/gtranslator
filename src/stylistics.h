/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gtranslator.org>
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

#ifndef LIBGTRANSLATOR_STYLISTICS_H
#define LIBGTRANSLATOR_STYLISTICS_H 1

/*
 * gtranslator specific style functions.
 */ 

#include "prefs.h"
#include <libgnomeui/gnome-color-picker.h>

/*
 * ColorValueTypes.
 */
typedef enum {
	COLOR_VALUE_FG,
	COLOR_VALUE_BG,
	COLOR_VALUE_DOT_CHAR,
	COLOR_VALUE_END_CHAR,
	COLOR_VALUE_SELECTION
} ColorValueType;


/*
 * Saves the color values from the GnomeColorPicker.
 */
void 	gtranslator_color_values_set(GnomeColorPicker *colorpicker,
	ColorValueType Type);

/*
 * Restores the color values for the GnomeColorPicker.
 */
void 	gtranslator_color_values_get(GnomeColorPicker *colorpicker,
	ColorValueType Type);

/*
 * Applies the foreground/background/font settings for the given
 *  widget.
 */
void 	gtranslator_set_style(GtkWidget *widget);

#endif
