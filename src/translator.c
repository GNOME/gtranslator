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

#include <libgnome/gnome-i18n.h>

/*
 * Instantate our "external" gtranslator_translator.
 */
GtrTranslator 	*gtranslator_translator=NULL;

/*
 * A helper function which reads and sets the values easily and safely.
 */
static void gtranslator_translator_read_value(gchar **destvalue,
	gchar *configpath);

/*
 * Reads the envpath for sane values and if it finds something useful,
 *  sets up destvalue.
 */
static void gtranslator_translator_read_env_value(gchar *envpath,
	gchar **destvalue);

/*
 * Returns GtrLanguage values from the preferences.
 */
static GtrLanguage *gtranslator_translator_read_language(void);

/*
 * Set the values safely from the given configpath's.
 */
static void gtranslator_translator_read_value(gchar **destvalue,
        gchar *configpath)
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
 * Determine environmental values from the given envpath.
 */
static void gtranslator_translator_read_env_value(gchar *envpath,
	gchar **destvalue)
{
	gchar	*value=NULL;

	g_return_if_fail(envpath!=NULL);

	gtranslator_utils_get_environment_value(envpath,
		&value);

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

/*
 * Return the newly parsed and set up GtrLanguage from the preferences.
 */
static GtrLanguage *gtranslator_translator_read_language()
{
	GtrLanguage *language=g_new0(GtrLanguage, 1);
    
    	/*
	 * Read the language specs from the preferences.
	 */
	gtranslator_translator_read_value(&language->name,
		"language/name");
	gtranslator_translator_read_value(&language->locale,
		"language/language_code");
	gtranslator_translator_read_value(&language->group_email,
		"language/team_email");
	gtranslator_translator_read_value(&language->encoding,
		"language/mime_type");
	gtranslator_translator_read_value(&language->bits,
		"language/encoding");

	return language;
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

	/*
	 * Read the translator specific values from the preferences.
	 */
	gtranslator_translator_read_value(&new_translator->name, 
		"translator/name");
	gtranslator_translator_read_value(&new_translator->email, 
		"translator/email");
	gtranslator_translator_read_value(&new_translator->learn_buffer,
		"translator/learn_buffer");
	gtranslator_translator_read_value(&new_translator->tm_buffer,
		"translator/translator_memory_buffer");

	/*
	 * Read our already existing language values from the preferences.
	 */
	new_translator->language=gtranslator_translator_read_language();

	/*
	 * Test if we're using still the default fallback learn buffer name
	 *  "en.xml" even though a new (changed) language was specified in
	 *    the prefs.
	 */
	if(!nautilus_strcasecmp(new_translator->learn_buffer, "en.xml") &&
		nautilus_strcasecmp(new_translator->language->name, "English"))
	{
		GTR_FREE(new_translator->learn_buffer);

		new_translator->learn_buffer=g_strdup_printf("%s.xml",
			(new_translator->language->locale ?
				new_translator->language->locale : "en"));
	}

	/*
	 * Get the default query domain from the preferences.
	 */
	gtranslator_translator_read_value(&new_translator->query_domain,
		"query/defaultdomain");

	return new_translator;
}

/*
 * Creates a more "fresh" GtrTranslator structure without reading from the
 *  preferences -- useful for init tasks.
 */
GtrTranslator *gtranslator_translator_new_with_default_values()
{
	GtrTranslator 	*new_translator;

	gchar		*language_locale=NULL;
	gchar		*natural_language_name=NULL;
	
	new_translator=g_new0(GtrTranslator, 1);
	new_translator->language=g_new0(GtrLanguage, 1);

	/*
	 * Do our environment dancing for the translator name & EMail address.
	 */
	gtranslator_translator_read_env_value(
		"GTRANSLATOR_TRANSLATOR_NAME:TRANSLATOR_NAME:TRANSLATOR:\
		NAME:CONTACT_NAME:LOGNAME:USER",
		&new_translator->name);
	
	gtranslator_translator_read_env_value(
		"GTRANSLATOR_TRANSLATOR_EMAIL_ADDRESS:\
		GTRANSLATOR_TRANSLATOR_EMAIL:TRANSLATOR_EMAIL:\
		EMAIL_ADDRESS:MAIL_ADDRESS:EMAIL:CONTACT_EMAIL",
		&new_translator->email);
		
	/*
	 * Try to "guess" the default query domain from the environment.
	 */
	gtranslator_translator_read_env_value(
		"GTRANSLATROR_DEFAULT_QUERY_DOMAIN:DEFAULT_QUERY_DOMAIN:\
		GETTEXT_QUERY_DOMAIN:QUERY_DOMAIN",
		&new_translator->query_domain);

	/*
	 * Determine the language locale setting in our environment.
	 */
	language_locale=gtranslator_utils_get_environment_locale();

	/*
	 * If we could determine any sane locale, then we do set up the
	 *  preferences values accordingly here, if not, we assume English
	 *   and continue.
	 */
	if(!language_locale || strlen(language_locale) < 2)
	{
		natural_language_name="English";
		gtranslator_utils_set_language_values_by_language("English");
	}
	else
	{
		natural_language_name=gtranslator_utils_get_language_name_by_locale_code(language_locale);
		
		/*
		 * If we could get a natural, normal language name for the
		 *  given locale, set the corresponding values in the prefs, 
		 *   else set up the default "English" values as if we didn't
		 *    find any locale environment variables.
		 */
		if(!natural_language_name)
		{
			natural_language_name="English";
		}
	}

	/*
	 * Now operate on our languages list and get the corresponding values
	 *  for the language.
	 */
	if(natural_language_name)
	{
		gint	i=0;

		while(languages[i].name!=NULL)
		{
			if(!nautilus_strcasecmp(languages[i].name, 
				natural_language_name))
			{
				/*
				 * Assign the values for the language from
				 *  the languages list here.
				 */
				new_translator->language->name=g_strdup(languages[i].name);
				new_translator->language->locale=g_strdup(languages[i].locale);
				new_translator->language->encoding=g_strdup(languages[i].encoding);
				new_translator->language->bits=g_strdup(languages[i].bits);
				
				GTR_STRDUP(new_translator->language->group_email, languages[i].group_email);
				
				break;
			}
			
			i++;
		}
	}
	
	/*
	 * The default learn buffer name now goes by the language locale (e.g.
	 *  "tr.xml" for Turkish) -- is better for the future expansion of the
	 *    learn buffer and translation memory mechanisms.
	 */
	new_translator->learn_buffer=g_strdup_printf("%s.xml", new_translator->language->locale);

	/*
	 * The default translation memory is named "translation-memory.xml".
	 */
	new_translator->tm_buffer=g_strdup("translation-memory.xml");

	return new_translator;
}

/*
 * Return the well-formed translator string we're using in many places.
 */
gchar *gtranslator_translator_get_translator_string(GtrTranslator *translator)
{
	gchar	*translator_string=NULL;
	
	g_return_val_if_fail(translator!=NULL, NULL);

	if(translator->name && translator->email)
	{
		translator_string=g_strdup_printf("%s <%s>",
			translator->name, translator->email);
	}
	else if(translator->name && !translator->email)
	{
		translator_string=g_strdup(translator->name);
	}
	else if(!translator->name && translator->email)
	{
		translator_string=g_strdup(translator->email);
	}
	else
	{
		translator_string=g_strdup(_("Unknown"));
	}

	return translator_string;
}

/*
 * Set the translator name + Email information of the given GtrTranslator
 *  safely from the given arguments.
 */
void gtranslator_translator_set_translator(GtrTranslator *translator,
	gchar *name, gchar *email)
{
	g_return_if_fail(translator!=NULL);
	g_return_if_fail(name!=NULL);

	/*
	 * Free & set the translator name in any case.
	 */
	GTR_FREE(GTR_TRANSLATOR(translator)->name);
	translator->name=g_strdup(name);

	GTR_FREE(GTR_TRANSLATOR(translator)->email);

	/*
	 * Check for the given EMail string and also check the given EMail
	 *  address for brevity.
	 */
	if(email && strlen(email) > 6 && 
		strchr(email, '@') && strchr(email, '.'))
	{
		translator->email=g_strdup(email);
	}
	else
	{
		translator->email=NULL;
	}
}

/*
 * Update the language values to match the values for the given "language_name"
 *  (the special "custom_group_email" string is available as the user can edit
 *    the group EMail address to a custom value; this is honored here; can be
 *     safely given as NULL).
 */
void gtranslator_translator_set_language(GtrTranslator *translator, 
	gchar *language_name, gchar *custom_group_email)
{
	gint	z=0;
	
	g_return_if_fail(translator!=NULL);
	g_return_if_fail(GTR_TRANSLATOR(translator)->language!=NULL);
	g_return_if_fail(language_name!=NULL);

	/*
	 * Cruise through the list, finding the corresponding language entry
	 *  (hopefully).
	 */
	while(languages[z].name!=NULL)
	{
		if(!nautilus_strcasecmp(languages[z].name, language_name))
		{
			/*
			 * As we've found the values to update, free the old
			 *  values.
			 */
			GTR_FREE(translator->language->name);
			GTR_FREE(translator->language->locale);
			GTR_FREE(translator->language->encoding);
			GTR_FREE(translator->language->bits);
			GTR_FREE(translator->language->group_email);

			/*
			 * And assign now the new (found in the list) values.
			 */
			translator->language->name=g_strdup(languages[z].name);
			translator->language->locale=g_strdup(languages[z].locale);
			translator->language->encoding=g_strdup(languages[z].encoding);
			translator->language->bits=g_strdup(languages[z].bits);

			/*
			 * If we did get a custom EMail address for the 
			 *  translator, use it here.
			 */
			if(custom_group_email)
			{
				translator->language->group_email=g_strdup(custom_group_email);
			}
			else
			{
				GTR_STRDUP(translator->language->group_email, languages[z].group_email);
			}
				break;
		}
	}
}

/*
 * Save the GtrTranslator's data/information into our preferences.
 */
void gtranslator_translator_save(GtrTranslator *translator)
{
	g_return_if_fail(translator!=NULL);
	
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
		translator->language->locale);
	gtranslator_config_set_string("language/team_email", 
		translator->language->group_email);
	gtranslator_config_set_string("language/mime_type", 
		translator->language->encoding);
	gtranslator_config_set_string("language/encoding", 
		translator->language->bits);

	/*
	 * Save the TM/auto translation settings.
	 */
	gtranslator_config_set_string("translator/learn_buffer", 
		translator->learn_buffer);
	gtranslator_config_set_string("translator/translator_memory_buffer", 
		translator->tm_buffer);
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
		GTR_FREE(translator->language->locale);
		GTR_FREE(translator->language->encoding);
		GTR_FREE(translator->language->group_email);
		GTR_FREE(translator->language->bits);
		GTR_FREE(translator->language);

		GTR_FREE(translator);
	}
}
