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

#include "parse.h"
#include "prefs.h"

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
		/*
		 * Useful options which shoule be set to TRUE.
		 */ 
		gtranslator_config_init();
		gtranslator_config_set_bool("toggles/save_geometry", TRUE);
		gtranslator_config_set_bool(
			"toggles/set_non_fuzzy_if_changed", TRUE);
		gtranslator_config_set_bool(
			"toggles/check_recent_files", TRUE);
		gtranslator_config_set_bool(
			"toggles/do_not_save_unchanged_files", TRUE);
		gtranslator_config_set_bool(
			"toggles/use_dot_char", TRUE);
		gtranslator_config_set_bool(
			"toggles/keep_obsolete", FALSE);	

		/*
		 * Initialize the default highlight colors.
		 */
		gtranslator_colors_init_default_colors(); 

		/*
		 * Avoid re-initialization of the default values via setting
		 *  a last run date.
		 */  
		gtranslator_config_set_last_run_date();
		gtranslator_config_close();

	}

	g_free(date);
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
	gtranslator_config_set_string("colors/own_bg", "#FFFFFF");
	
	/*
	 * The default syntax colors.
	 */
	gtranslator_config_set_string("colors/fg", "black");
	gtranslator_config_set_string("colors/bg", "white");
	gtranslator_config_set_string("colors/special_char", "grey");
	gtranslator_config_set_string("colors/hotkey", "blue");
	gtranslator_config_set_string("colors/c_format", "red");
	gtranslator_config_set_string("colors/number", "orange");
	gtranslator_config_set_string("colors/punctuation", "darkblue");
	gtranslator_config_set_string("colors/special", "maroon");
	gtranslator_config_set_string("colors/address", "maroon");
	gtranslator_config_set_string("colors/keyword", "darkred");
	
	gtranslator_config_close();
}