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

#ifndef STYLISTICS_H
#define STYLISTICS_H 1

/**
* Include the preferences headers and the GNOME headers.
**/
#include <libgtranslator/preferences.h>
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
* This function let's all your special font/color wishes
*  come true ( at least for 2 widgets :-) ).
**/
void gtranslator_set_style(GtkWidget *one, GtkWidget *two);

#endif
