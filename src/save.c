/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gtranslator.org>
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

#include "about.h"
#include "gui.h"
#include "nautilus-string.h"
#include "save.h"
#include "parse.h"
#include "runtime-config.h"
#include "utils.h"
#include "utils_gui.h"
#include "vfs-handle.h"

#include <string.h>
#include <stdlib.h>

#include <libgnomeui/gnome-app-util.h>

/*
 * The "backend" for the gzip. bzip2 and uncompress based functions.
 */
void save_compressed_po_file(const gchar *file, gchar *command);

/*
 * The internally used functions' prototypes.
 */
void gtranslator_save_compiled_po_file(const gchar *file);
void gtranslator_save_gzipped_po_file(const gchar *file);
void gtranslator_save_bzip2ed_po_file(const gchar *file);
void gtranslator_save_compressed_po_file(const gchar *file);
void gtranslator_save_ziped_po_file(const gchar *file);

/*
 * Detects whether we can save the given file with the
 *  "special" save functions of gtranslator.
 */
gboolean gtranslator_save_po_file(const gchar *filename)
{
	if(nautilus_istr_has_suffix(filename, ".mo") || 
		nautilus_istr_has_suffix(filename, ".gmo"))
	{
		gtranslator_save_compiled_po_file(filename);
		return TRUE;
	}
	else if(nautilus_istr_has_suffix(filename, ".po.gz"))
	{
		gtranslator_save_gzipped_po_file(filename);
		return TRUE;
	}
	else if(nautilus_istr_has_suffix(filename, ".po.bz2"))
	{
		gtranslator_save_bzip2ed_po_file(filename);
		return TRUE;
	}
	else if(nautilus_istr_has_suffix(filename, ".po.z"))
	{
		gtranslator_save_compressed_po_file(filename);
		return TRUE;
	}
	else if(nautilus_istr_has_suffix(filename, ".po.zip"))
	{
		gtranslator_save_ziped_po_file(filename);
		return TRUE;
	}
	
	return FALSE;
}

/*
 * Save the given compiled gettext file.
 */
void gtranslator_save_compiled_po_file(const gchar *file)
{
	gchar *cmd;

	if(!gtranslator_utils_check_program("msgfmt", 1))
	{
		return;
	}

	gtranslator_save_file(
		gtranslator_runtime_config->save_differently_filename);
	
	/*
	 * Build up the command to execute in the shell to get the compiled
	 *  gettext file.
	 */
	cmd=g_strdup_printf("msgfmt '%s' -o '%s'",
		gtranslator_runtime_config->save_differently_filename,
		file);
	/* 
	 * Execute the command and test the result.
	 */
	if(system(cmd))
	{
		cmd=g_strdup_printf(_("Couldn't save compiled gettext file `%s'!"),
			file);
		/*
		 * Show a warning to the user.
		 */
		gnome_app_warning(GNOME_APP(gtranslator_application), cmd);
	}

	GTR_FREE(cmd);
}

/*
 * This acts as the backend function for the gzip & bzip2'ed po file
 *  functions.
 */
void save_compressed_po_file(const gchar *file, gchar *command)
{
	gchar *cmd;

	if(!gtranslator_utils_check_program(command, 1))
	{
		return;
	}

	gtranslator_save_file(
		gtranslator_runtime_config->save_differently_filename);
	
	/* 
	 * Set up the command to execute in the system shell.
	 */
	cmd=g_strdup_printf("'%s' -c -q < '%s' > '%s'",
		command,
		gtranslator_runtime_config->save_differently_filename,
		file);

	/*
	 * Execute the command and check the result.
	 */
	if(system(cmd))
	{
		if(!strcmp(command, "compress"))
		{
			cmd=g_strdup_printf(
			_("Couldn't save compressed gettext file `%s'!"),
			file);
		}
		else
		{
			cmd=g_strdup_printf(
				/*
				 * The %s format here stands for the used
				 *  compressions program (gzip, bzip2 etc.)
				 */
				_("Couldn't save %s'd gettext file `%s'!"),
				command,	
				file);
		}
		
		/*
		 * Display the warning to the user.
		 */
		gnome_app_warning(GNOME_APP(gtranslator_application), cmd);
	}
	
	GTR_FREE(cmd);
}

/*
 * Save routines for all the stuff:
 */ 
void gtranslator_save_gzipped_po_file(const gchar *file)
{
	g_return_if_fail(file!=NULL);

	save_compressed_po_file(file, "gzip");
}

void gtranslator_save_bzip2ed_po_file(const gchar *file)
{
	g_return_if_fail(file!=NULL);

	save_compressed_po_file(file, "bzip2");
}

void gtranslator_save_compressed_po_file(const gchar *file)
{
	g_return_if_fail(file!=NULL);

	save_compressed_po_file(file, "compress");
}

void gtranslator_save_ziped_po_file(const gchar *file)
{
	gchar 	*cmd;

	g_return_if_fail(file!=NULL);

	if(!gtranslator_utils_check_program("zip", 1))
	{
		return;
	}

	gtranslator_save_file(
		gtranslator_runtime_config->save_differently_filename);

	cmd=g_strdup_printf("zip -q '%s' '%s'", 
		file,
		gtranslator_runtime_config->save_differently_filename);

	if(system(cmd))
	{
		gtranslator_parse_main(gtranslator_runtime_config->save_differently_filename);
		cmd=g_strdup_printf(_("Couldn't save zip'ed po file `%s'!"),
			file);

		gnome_app_warning(GNOME_APP(gtranslator_application), cmd);
	}

	GTR_FREE(cmd);
}
