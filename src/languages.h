/*
 * (C) 2000-2003 	Fatih Demir <kabalak@gtranslator.org>
 *			GNOME I18N List <gnome-i18n@gnome.org>
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

#ifndef GTR_LANGUAGES_H
#define GTR_LANGUAGES_H 1

#include <glib.h>

/*
 * #include <libgnome/gnome-defs.h>
 */

/*
 * The language structure with all the essential data for gtranslator:
 */
typedef struct {

	/*
	 * 1 - The name of the language (e.g. "Turkish").
	 */
	gchar *name;
	
	/*
	 * 2 - The language-code (e.g. "tr").
	 */
	gchar *locale;
	
	/*
	 * 3 - The encoding mostly used for the language (e.g. "iso-8859-9").
	 */
	gchar *encoding;
	
	/*
	 * 4 - The group's EMail list (e.g. "gnome-turk@gnome.org").
	 */
	gchar *group_email;
	
	/*
	 * 5 - The transfer bit count for the language (e.g. "8bit").
	 */
	gchar *bits;
} GtrLanguage;

/*
 * Declare the generally used languages list externally here.
 */
extern GtrLanguage languages[];

#endif
