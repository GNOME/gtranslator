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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "nautilus-string.h"
#include "prefs.h"
#include "translator.h"
#include "utils.h"

/*
 * Instantate our "external" translator.
 */
GtrTranslator *translator=NULL;

/*
 * A helper function which reads and sets the values easily and safely.
 */
static void gtranslator_translator_read_value(gchar **destvalue,
	gchar	*configpath);

/*
 * Set the values safely from the given configpath's.
 */
static void gtranslator_translator_read_value(gchar **destvalue,
        gchar	*configpath)
{
	if(!configpath)
	{
		*destvalue=NULL;
	}
	else
	{
		gchar	*value=NULL;

		/*
		 * Read the value from the preferences and set the string up
		 *  if any value could be read -- if not, set the given 
		 *   deststring to NULL.
		 */
		value=gtranslator_config_get_string(configpath);

		if(value)
		{
			*destvalue=g_strdup(value);
			GTR_FREE(value);
		}
		else
		{
			*destvalue=NULL;
		}
	}
}

/*
 * Creates a new GtrTranslator structure with the information/configuration
 *  from the preferences.
 */
GtrTranslator *gtranslator_translator_new()
{
	GtrTranslator 	*new_translator;
	gchar		*language_name_value=NULL;
	
	new_translator=g_new0(GtrTranslator, 1);

	gtranslator_config_init();
	gtranslator_translator_read_value(&new_translator->name, 
		"translator/name");
	gtranslator_translator_read_value(&new_translator->email, 
		"translator/email");
	gtranslator_translator_read_value(&new_translator->learn_buffer,
		"translator/learn_buffer");
	gtranslator_translator_read_value(&new_translator->tm_buffer,
		"translator/translator_memory_buffer");

	/*
	 * Read the language name from the preferences.
	 */
	language_name_value=gtranslator_config_get_string("language/name");
	g_return_val_if_fail(language_name_value!=NULL, new_translator);

	/*
	 * FIXME: Set the language from the list + the group EMail from the
	 *  prefs.
	 */

	gtranslator_config_close();
	
	return new_translator;
}

/*
 * Creates a more "fresh" GtrTranslator structure without reading from the
 *  preferences -- useful for init tasks.
 */
GtrTranslator *gtranslator_translator_new_with_default_values()
{
	GtrTranslator *new_translator=g_new0(GtrTranslator, 1);

	/*
	 * FIXME: Is just temporary compiling placeholder.
	 */
	if(languages[2].name)
	{
	}
	return new_translator;
}

/*
 * Save the GtrTranslator's data/information into our preferences.
 */
void gtranslator_translator_save(GtrTranslator *translator)
{
}

/*
 * Free the GtrTranslator safely and rightly.
 */
void gtranslator_translator_free(GtrTranslator *translator)
{
	if(translator)
	{
		GTR_FREE(translator->name);
		GTR_FREE(translator->email);
		GTR_FREE(translator->learn_buffer);
		GTR_FREE(translator->tm_buffer);

		GTR_FREE(translator);
	}
}
