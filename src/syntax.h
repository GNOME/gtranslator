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
#include "parse.h"

/*
 * GdkColors aren't as logical as natural color names I guess.
 */
typedef enum {
	RED,
	GREEN,
	BLUE,
	BLACK,
	WHITE,
	YELLOW,
	ORANGE,
	NAVY,
	MAROON,
	AQUA
} ColorName;

/*
 * Insert the given text in the highlighted form into the textwidget.
 */
void gtranslator_syntax_insert_text(GtkWidget *textwidget, GtrMsg *msg);

/*
 * Are there any format specifiers in this message? Return TRUE if it's so.
 */
gboolean gtranslator_syntax_get_format(GtrMsg *msg);

/*
 * Returns the requested GdkColor.
 */
GdkColor *gtranslator_syntax_get_gdk_color(ColorName name);

#endif
