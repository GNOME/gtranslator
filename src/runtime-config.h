/*
 * (C) 2001-2003 	Fatih Demir <kabalak@gtranslator.org>
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

#ifndef GTR_RUNTIME_CONFIG_H
#define GTR_RUNTIME_CONFIG_H 1

#include <glib.h>

/*
 * This GtrRuntimeConfig structure is the home for all kinds of runtime config
 *  data -- previously we used dozens of "gtranslator_utils_get_" for the
 *   same task.
 */
typedef struct
{
	gchar	*temp_filename;
	gchar	*check_filename;
	gchar	*crash_filename;
	
	gchar	*save_differently_filename;

	gchar	 special_char;
} GtrRuntimeConfig;

#define GTR_RUNTIME_CONFIG(x) ((GtrRuntimeConfig *) x)

/*
 * This is the generally used runtime config variable in gtranslator.
 */
extern GtrRuntimeConfig *gtranslator_runtime_config;

/*
 * Initialize and return the runtime configuration values.
 */
GtrRuntimeConfig *gtranslator_runtime_config_new(void);

/*
 * Free the runtime config data supplied from the GtrRuntimeConfig structure.
 */
void gtranslator_runtime_config_free(GtrRuntimeConfig *config);

#endif
