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

#include "backend.h"
#include "dialogs.h"
#include "prefs.h"
#include "runtime-config.h"
#include "sighandling.h"
#include "translator.h"
#include "utils.h"

#include <signal.h>
#include <stdlib.h>
#include <libgnome/gnome-i18n.h>

static gint signalscount=0;

/*
 * Get and enclose the signals that could put much translation work within
 *  gtranslator into the trash.
 */ 
void gtranslator_signal_handler(int signal)
{
	if(signalscount > 0)
	{
		return;
	}	
	signalscount = 1;

	if(po && po->file_changed)
	{
		/*
		 * Store the original filename into the
		 *  preferences.
		 */ 
		gtranslator_config_init();
		gtranslator_config_set_string("crash/filename",
			po->filename);

		/*
		 * Save the file under the special filename.
		 */
		gtranslator_save_file(
			gtranslator_runtime_config->crash_filename);
	}

	gtranslator_config_set_string("runtime/filename", "--- No file ---");
	gtranslator_config_close();

	/*
	 * Free some other stuff we're definetely using already or whose free/
	 *  removal functions are smart enough to be used here ,-)
	 */
	gtranslator_backend_remove_all_backends();
	gtranslator_translator_free(gtranslator_translator);
	gtranslator_preferences_free();
	gtranslator_runtime_config_free(gtranslator_runtime_config);
	gnome_regex_cache_destroy(rxc);

	exit(1);
}
