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
**/

#ifndef LIBGTRANSLATOR_STYLISTICS_H
#define LIBGTRANSLATOR_STYLISTICS_H 1

/**
* Include the preferences headers and the GNOME headers.
**/
#include "preferences.h"
#include <libgnomeui/gnome-color-picker.h>

/**
* The enumeration for the ColorValueTypes.
**/
typedef enum {
	COLOR_VALUE_FG,
	COLOR_VALUE_BG,
	COLOR_VALUE_DOT_CHAR,
	COLOR_VALUE_END_CHAR,
	COLOR_VALUE_SELECTION
} ColorValueType;


/**
* The routines written here are usefull for storing and getting
*  colors from GnomeColorPickers.
**/

/**
* This gets the colors from the given GnomeColorPicker and sets
*  them in the preferences.
**/
void gtranslator_color_values_set(GnomeColorPicker *colorpicker,
	ColorValueType Type);

/**
* And this restores the given colors from the preferences to the
*  given gdoubles from the pointers.
**/
void gtranslator_color_values_get(GnomeColorPicker *colorpicker,
	ColorValueType Type);

/**
* This function applies the stored values for fg/bg and font.
**/
void gtranslator_set_style(GtkWidget *widget);

#endif
