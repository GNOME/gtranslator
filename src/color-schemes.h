/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
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

#ifndef GTR_COLORSCHEMES_H
#define GTR_COLORSCHEMES_H

#include "stylistics.h"

/*
 * A small information structure.
 */
typedef struct {
	gchar *name;
	gchar *version;

	gchar *author;
	gchar *author_email;

	gchar *filename;
} GtrColorSchemeInformations;

/*
 * The colorscheme structure with all the colors/informations.
 */ 
typedef struct {
	GtrColorSchemeInformations *info;

	gchar *fg;
	gchar *bg;
	gchar *special_char;
	gchar *hotkey;
	gchar *c_format;
	gchar *number;
	gchar *punctuation;
	gchar *special;
	gchar *address;
	gchar *keyword;
	gchar *spell_error;
} GtrColorScheme;

#define GTR_COLOR_SCHEME(x) \
	((GtrColorScheme *) x)

#define GTR_COLOR_SCHEME_INFORMATIONS(x) \
	((GtrColorSchemeInformations *) x)

/*
 * The general GtrColorScheme we do use in gtranslator.
 */
GtrColorScheme *theme; 

/*
 * Free the given GtrColorScheme.
 */
void free_color_scheme(GtrColorScheme **scheme);

/*
 * Returns the GtrColorScheme struct for the given file (a colorscheme) or NULL
 *  if it isn't a colorscheme file.
 */
GtrColorScheme *gtranslator_color_scheme_open(const gchar *filename);

/*
 * Applies the given color scheme as the default one.
 */
void gtranslator_color_scheme_apply(const gchar *filename);

/*
 * Load the current stored GtrColorScheme from the preferences.
 */ 
GtrColorScheme *gtranslator_color_scheme_load_from_prefs(void);

/*
 * Sets up the original hardcoded default syntax highlighting
 *  colors up.
 */
void gtranslator_color_scheme_restore_default(void);

/*
 * Give a list consisting out of GtrColorSchemeInformations about the color
 *  schemes we've found in SCHEMESDIR -- and in directory if it's given.
 */
GList *gtranslator_color_scheme_list(const gchar *directory);

/*
 * Copy the given GtrColorScheme(Informations).
 */
GtrColorScheme *gtranslator_color_scheme_copy(GtrColorScheme *scheme);
GtrColorSchemeInformations *gtranslator_color_scheme_infos_copy(
	GtrColorSchemeInformations *infos);


#endif
