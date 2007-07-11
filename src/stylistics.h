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

/*
 * gtranslator specific style functions.
 */ 

#include <gtk/gtkcolorbutton.h>

/*
 * ColorTypes.
 */
typedef enum {
	COLOR_NONE = 1,
	COLOR_FG,
	COLOR_BG,
	COLOR_TEXT_BG,
	COLOR_SPECIAL_CHAR,
	COLOR_HOTKEY,
	COLOR_C_FORMAT,
	COLOR_NUMBER,
	COLOR_PUNCTUATION,
	COLOR_SPECIAL,
	COLOR_ADDRESS,
	COLOR_KEYWORD,
	COLOR_SPELL_ERROR,

	/*
	 * The colors for the entries in the messages table/tree.
	 */
	COLOR_MESSAGES_TABLE_UNTRANSLATED,
	COLOR_MESSAGES_TABLE_FUZZY,
	COLOR_MESSAGES_TABLE_TRANSLATED,
	
	/*
	 * This is a fake color, should be the last 
	 */
	COLOR_END
} ColorType;

GdkColor colors[COLOR_END];
void gtranslator_colors_initialize(void);

/*
 * Saves the color values from the GtkColorButton.
 */
void gtranslator_color_values_set(GtkColorButton *colorbutton, ColorType Type);

/*
 * Restores the color values for the GtkColorButton.
 */
void gtranslator_color_values_get(GtkColorButton *colorbutton, ColorType Type);

/*
 * Applies the foreground/background/font settings for the given
 *  widget.
 */
void gtranslator_set_style(GtkWidget *widget, gint foo_us_and_spec_the_widget);

/*
 * Utility function for getting the stuff working.
 */
GdkColor *gtranslator_get_color_from_type(ColorType type);

#endif
