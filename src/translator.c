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
 * Instantate our "external" gtranslator_translator.
 */
GtrTranslator *gtranslator_translator=NULL;

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
			*destvalue=g_strdup("");
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

	/*
	 * Creathe the new GtrTranslator structure.
	 */
	new_translator=g_new0(GtrTranslator, 1);
	new_translator->language=g_new0(GtrLanguage, 1);

	/*
	 * Read the translator specific values from the preferences.
	 */
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
	 * Read the language specs from the preferences.
	 */
	gtranslator_translator_read_value(&new_translator->language->name,
		"language/name");
	gtranslator_translator_read_value(&new_translator->language->lcode,
		"language/language_code");
	gtranslator_translator_read_value(&new_translator->language->group,
		"language/team_email");
	gtranslator_translator_read_value(&new_translator->language->enc,
		"language/mime_type");
	gtranslator_translator_read_value(&new_translator->language->bits,
		"language/encoding");

	/*
	 * Get the default query domain from the preferences.
	 */
	gtranslator_translator_read_value(&new_translator->query_domain,
		"query/defaultdomain");

	gtranslator_config_close();
	
	return new_translator;
}

/*
 * Creates a more "fresh" GtrTranslator structure without reading from the
 *  preferences -- useful for init tasks.
 */
GtrTranslator *gtranslator_translator_new_with_default_values()
{
	GtrTranslator 	*new_translator;
	gchar		*env_value;
	gint		 i;
	
	env_value=NULL;
	new_translator=g_new0(GtrTranslator, 1);
	new_translator->language=g_new0(GtrLanguage, 1);

	gtranslator_utils_get_environment_value("GTRANSLATOR_TRANSLATOR_NAME:\
		TRANSLATOR_NAME:TRANSLATOR:NAME:LOGNAME:USER", &env_value);

	if(env_value)
	{
		new_translator->name=g_strdup(env_value);
		GTR_FREE(env_value);
	}

	/*
	 * FIXME: Read in the environment values and set the preferences 
	 *  accordingly.
	 */

	for(i=0; i < (sizeof(languages) / sizeof(GtrLanguage)); i++)
	{
		/*
		 * FIXME: Read/accomplish the language values.
		 */
	}

	return new_translator;
}

/*
 * Save the GtrTranslator's data/information into our preferences.
 */
void gtranslator_translator_save(GtrTranslator *translator)
{
	g_return_if_fail(translator!=NULL);
	
	gtranslator_config_init();

	/*
	 * Save the translator's personal settings.
	 */
	gtranslator_config_set_string("translator/name", translator->name);
	gtranslator_config_set_string("translator/email", translator->email);
	gtranslator_config_set_string("query/defaultdomain", 
		translator->query_domain);
	
	/*
	 * Save the language settings.
	 */
	gtranslator_config_set_string("language/name", 
		translator->language->name);
	gtranslator_config_set_string("language/language_code", 
		translator->language->lcode);
	gtranslator_config_set_string("language/team_email", 
		translator->language->group);
	gtranslator_config_set_string("language/mime_type", 
		translator->language->enc);
	gtranslator_config_set_string("language/encoding", 
		translator->language->bits);

	/*
	 * Save the TM/auto translation settings.
	 */
	gtranslator_config_set_string("translator/learn_buffer", 
		translator->learn_buffer);
	gtranslator_config_set_string("translator/translator_memory_buffer", 
		translator->tm_buffer);

	gtranslator_config_close();
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
		GTR_FREE(translator->query_domain);

		GTR_FREE(translator->language->name);
		GTR_FREE(translator->language->lcode);
		GTR_FREE(translator->language->enc);
		GTR_FREE(translator->language->group);
		GTR_FREE(translator->language->bits);
		GTR_FREE(translator->language);

		GTR_FREE(translator);
	}
}
