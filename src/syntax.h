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

#include <gtk/gtktext.h>

/*
 * This function creates syntax textdata and ties it to widget.
 * Text widget should have 0 characters inside.
 */
 
void gtranslator_syntax_init(GtkEditable *textwidget);

/*
 * This function will insert text highlighted into widget.
 * 1. Add characters to the widget
 * 2. Add to characterdata the new entries
 * 3. Let parser create new characterdata
 * 4. Compare new characterdata with old one and update changed chars
 */
void gtranslator_insert_highlighted(
		GtkEditable *textwidget,
		gchar	*text,
		gint	*pos,
		gint	addlen);

/*
 * This function will delete text and rehighlight.
 * 1. Delete characters from the widget
 * 2. Remove characterdata of deleted chars
 * 3. Let parser create new characterdata
 * 4. Compare new characterdata with old one and update changed chars
 */
void gtranslator_delete_highlighted(
		GtkEditable *textwidget,
		gint	pos,
		gint	len);
		
/*
 * Will check syntax.
 * returns new highlight information
 */
GString *gtranslator_parse_syntax(GtkEditable *textwidget);

/*
 *
 */
void gtranslator_insert_text(GtkText *editable, const gchar *text);

#endif
