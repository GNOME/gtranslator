/*
 * (C) 2001		Fatih Demir <kabalak@gtranslator.org>
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

#ifndef GTR_SYNTAX_H
#define GTR_SYNTAX_H 1

#include "gui.h"
#include "stylistics.h"

/*
 * GdkColors aren't as logical as natural color names I guess.
 */
typedef enum {
	AQUA,
	BLACK,
	BLUE,
	BROWN,
	GREEN,
	MAROON,
	NAVY,
	ORANGE,
	RED,
	WHITE,
	YELLOW
} ColorName;

/*
 * Insert the given text in the highlighted form into the textwidget.
 */
void gtranslator_syntax_insert_text(GtkWidget *textwidget, const gchar *msg);

/*
 * Update the text in the given textwidget.
 */
void gtranslator_syntax_update_text(GtkWidget *textwidget);

/*
 * Returns the requested GdkColor.
 */
GdkColor *gtranslator_syntax_get_gdk_color(ColorName name);

/*
 * Returns the section name for the ColorValueType.
 */
gchar *gtranslator_syntax_get_section_name(ColorValueType Type);

#endif
