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

#ifndef GTR_TRANSLATOR_H
#define GTR_TRANSLATOR_H 1

/*
 * #include <libgnome/gnome-defs.h>
 */

#include "languages.h"

/*
 * Defines a custom GtrTranslator structure which owns all personal information
 *  or configuration of the translator.
 */
typedef struct
{
	/*
	 * Personal properties.
	 */
	gchar		*name;
	gchar		*email;

	/*
	 * Language information -- obtained from prefs or from the languages
	 *  lists in <languages.h>.
	 */
	GtrLanguage	*language; 

	/*
	 * Learn buffer/autotranslation settings.
	 */
	gchar		*learn_buffer;
	gchar		*tm_buffer;
} GtrTranslator;

#define GTR_TRANSLATOR(x) ((GtrTranslator *) x)

/*
 * The generally used GtrTranslator structure for our translator.
 */
extern GtrTranslator *gtranslator_translator;

/*
 * Creation of a default "GtrTranslator" -- nono, we don't work together with
 *  genetics companies yet ,-)
 *
 * gtranslator_translator_new does read the values from the preferences whereas
 *  gtranslator_translator_new_with_default_values tries the usual ping-pong
 *   guesses for locale, language, translator name and EMail address like for 
 *    the first init.
 */
GtrTranslator *gtranslator_translator_new(void);
GtrTranslator *gtranslator_translator_new_with_default_values(void);

/*
 * Change/update/set the translator name & Email safely for the given
 *  GtrTranslator.
 */
void gtranslator_translator_set_translator(GtrTranslator *translator,
	gchar *name, gchar *email);

/*
 * Returns a standardly formed translator string like "TRANSLATOR <EMAIL>" in
 *  a normal case.
 */
gchar *gtranslator_translator_get_translator_string(GtrTranslator *translator);

/*
 * Sets the language values for the translator for the given "language_name" 
 *  (updates the interna of the translator->language structure if necessary).
 */
void gtranslator_translator_set_language(GtrTranslator *translator, 
	gchar *language_name, gchar *custom_group_email);

/*
 * Saves the values from the GtrTranslator structure in our preferences.
 */
void gtranslator_translator_save(GtrTranslator *translator);

/*
 * Created translators also want to be free'd -- here we go.
 */
void gtranslator_translator_free(GtrTranslator *translator);

#endif

