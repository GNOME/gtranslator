/*
 * (C) 2001 		Fatih Demir <kabalak@gtranslator.org>
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

#include "defines.include"
#include "parse.h"
#include "prefs.h"
#include "utils.h"

#include <string.h>

/*
 * Sets up some useful options as the user runs the first time gtranslator.
 */
void gtranslator_preferences_init_default_values()
{
	gchar *date;
	
	/*
	 * Seee if gtranslator has been run before.
	 */ 
	gtranslator_config_init();
	date=gtranslator_config_get_string("informations/last_run_on");
	gtranslator_config_close();

	/*
	 * Initialize the default values.
	 */ 
	if(!date)
	{
		gtranslator_config_init();
		
		/*
		 * Determine the language name from the environment.
		 */
		if(!lc)
		{
			gchar	*language_name_for_prefs_init=NULL;
			
			lc=gtranslator_utils_get_environment_locale();

			/*
			 * Well, if we couldn't determine any locale, assume
			 *  plain "English" -- should beware us from crashing.
			 */
			if(!lc)
			{
				gtranslator_utils_set_language_values_by_language("English");
			}
			else
			{
				language_name_for_prefs_init=gtranslator_utils_get_language_name_by_locale_code(lc);
				g_return_if_fail(language_name_for_prefs_init!=NULL);

				gtranslator_utils_set_language_values_by_language(language_name_for_prefs_init);
			}
		}

		/*
		 * Also try out some magic for getting the translator name/EMail
		 *  address pair.
		 */
		if(!author || !email)
		{
			gchar	*value=NULL;
			gint	 i=0;

			const gchar *name_env_variables[] =
			{
				"GTRANSLATOR_TRANSLATOR_NAME",
				"TRANSLATOR_NAME",
				"NAME",
				"LOGNAME",
				NULL
			};

			const gchar *email_env_variables[] =
			{
				"GTRANSLATOR_TRANSLATOR_EMAIL_ADDRESS",
				"GTRANSLATOR_TRANSLATOR_EMAIL",
				"TRANSLATOR_EMAIL",
				"EMAIL_ADDRESS",
				"EMAIL",
				NULL
			};

			/*
			 * Try our defined environment variables for a value.
			 */
			while(name_env_variables[i]!=NULL && !value)
			{
				value=g_getenv(name_env_variables[i]);
				i++;
			}

			/*
			 * If we've found a value set it as the initial 
			 *  translator name in the prefs.
			 */
			if(value)
			{
				gtranslator_config_set_string("translator/name", value);
			}

			/*
			 * Reset the used variables to their initial values.
			 */
			i=0;
			value=NULL;

			/*
			 * Also check the EMail address environment variables.
			 */
			while(email_env_variables[i]!=NULL && !value)
			{
				value=g_getenv(email_env_variables[i]);
				i++;
			}

			/*
			 * Well, the EMail address is also filled out if any
			 *  corresponding environment variable could be found.
			 */
			if(value && strchr(value, '@') && strchr(value, '.'))
			{
				gtranslator_config_set_string("translator/email", value);
			}
		}

		/*
		 * Useful options which should be set to TRUE.
		 */ 
		gtranslator_config_set_bool("toggles/set_non_fuzzy_if_changed", TRUE);
		gtranslator_config_set_bool("toggles/check_recent_files", TRUE);
		gtranslator_config_set_bool("toggles/do_not_save_unchanged_files", TRUE);
		gtranslator_config_set_bool("toggles/keep_obsolete", TRUE);
		gtranslator_config_set_bool("toggles/sweep_compile_file", FALSE);
		gtranslator_config_set_bool("toggles/use_learn_buffer", TRUE);
		
		/*
		 * GUI related default values.
		 */
		gtranslator_config_set_bool("toggles/save_geometry", TRUE);
		gtranslator_config_set_bool("toggles/use_dot_char", TRUE);
		gtranslator_config_set_bool("toggles/show_sidebar", TRUE);
		gtranslator_config_set_bool("toggles/show_messages_table", TRUE);
		gtranslator_config_set_bool("toggles/collapse_translated_entries", TRUE);
		gtranslator_config_set_bool("toggles/enable_popup_menu", TRUE);
		gtranslator_config_set_bool("toggles/use_update_function", TRUE);
		gtranslator_config_set_bool("toggles/rambo_function", FALSE);
		
		/*
		 * Startup saneness should set this also to TRUE.
		 */
		gtranslator_config_set_bool("toggles/fill_header", TRUE);
		gtranslator_config_set_bool("find/case_sensitive", FALSE);

		/*
		 * Autosave options; it should be on, with suffix "autosave" 
		 *  and timeout on 5 minutes.
		 */
		gtranslator_config_set_bool("toggles/autosave", TRUE);
		gtranslator_config_set_bool("toggles/autosave_with_suffix", TRUE);
		gtranslator_config_set_string("informations/autosave_suffix", "autosave");
		gtranslator_config_set_float("informations/autosave_timeout", 5.0);

		/*
		 * We do want maximally 10 history entries per default.
		 */
		gtranslator_config_set_float("informations/max_history_entries", 10.0); 

		/*
		 * Initialize the default highlight colors.
		 */
		gtranslator_colors_init_default_colors();

		/*
		 * Set the default colorscheme name + filename.
		 */
		gtranslator_config_set_string("scheme/name", "default.xml");
		gtranslator_config_set_string("scheme/filename", SCHEMESDIR
			"/default.xml");

		/*
		 * Avoid re-initialization of the default values via setting
		 *  a last run date.
		 */  
		gtranslator_config_set_last_run_date();
		gtranslator_config_close();

	}

	GTR_FREE(date);
}

/*
 * Setup the default colors.
 */ 
void gtranslator_colors_init_default_colors()
{
	gtranslator_config_init();
	
	/*
	 * Set black on white as default colors for the color pickers.
	 */ 
	gtranslator_config_set_string("colors/own_fg", "#000000");
	gtranslator_config_set_string("colors/own_bg", "#ffffff");
	
	/*
	 * The default syntax colors.
	 */
	gtranslator_config_set_string("colors/fg", "black");
	gtranslator_config_set_string("colors/bg", "white");
	gtranslator_config_set_string("colors/text_bg", "white");
	gtranslator_config_set_string("colors/special_char", "grey");
	gtranslator_config_set_string("colors/hotkey", "blue");
	gtranslator_config_set_string("colors/c_format", "red");
	gtranslator_config_set_string("colors/number", "orange");
	gtranslator_config_set_string("colors/punctuation", "darkblue");
	gtranslator_config_set_string("colors/special", "maroon");
	gtranslator_config_set_string("colors/address", "maroon");
	gtranslator_config_set_string("colors/keyword", "darkred");

	/*
	 * The default colors for the messages table.
	 */
	gtranslator_config_set_string("colors/messages_table_translated", "#000000");
	gtranslator_config_set_string("colors/messages_table_fuzzy", "#ff0000");
	gtranslator_config_set_string("colors/messages_table_untranslated", "#a7453e");
	
	gtranslator_config_close();
}
