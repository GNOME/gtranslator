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
#include "backend.h"
#include "gui.h"
#include "nautilus-string.h"
#include "open.h"
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
void open_compressed_po_file(gchar *file, gchar *command);

/*
 * Internally used functions' prototypes.
 */ 
void gtranslator_open_compiled_po_file(gchar *file);
void gtranslator_open_gzipped_po_file(gchar *file);
void gtranslator_open_bzip2ed_po_file(gchar *file);
void gtranslator_open_compressed_po_file(gchar *file);
void gtranslator_open_ziped_po_file(gchar *file); 

/*
 * Open the way to the beautiful land of gtranslator...
 */
void gtranslator_open_file(gchar *filename)
{
	g_return_if_fail(filename!=NULL);
	
	/*
	 * Use conditionally the VFS routines to access
	 *  remote files.
	 */
	if(gtranslator_utils_uri_supported(filename))
	{
		filename=gtranslator_vfs_handle_open(filename);

		if(filename)
		{
			/*
			 * Here we do open the local representation file
			 *  of the remote file.
			 */  
			gtranslator_parse_main(filename);
		}
	}
	else if(nautilus_str_has_prefix(filename, "about:"))
	{
		gtranslator_about_dialog(NULL, NULL);
	}

	if(nautilus_istr_has_suffix(filename, ".mo") || 
		nautilus_istr_has_suffix(filename, ".gmo"))
	{
		gtranslator_open_compiled_po_file(filename);
	}
	else if(nautilus_istr_has_suffix(filename, ".po.gz"))
	{
		gtranslator_open_gzipped_po_file(filename);
	}
	else if(nautilus_istr_has_suffix(filename, ".po.bz2"))
	{
		gtranslator_open_bzip2ed_po_file(filename);
	}
	else if(nautilus_istr_has_suffix(filename, ".po.z"))
	{
		gtranslator_open_compressed_po_file(filename);
	}
	else if(nautilus_istr_has_suffix(filename, ".po.zip"))
	{
		gtranslator_open_ziped_po_file(filename);
	}
	else if(!gtranslator_backend_open(filename))
	{
		gtranslator_parse_main(filename);
	}
}

/*
 * Open up the given compiled gettext file.
 */
void gtranslator_open_compiled_po_file(gchar *file)
{
	gchar *cmd;

	if(!gtranslator_utils_check_program("msgunfmt", 0))
	{
		return;
	}

	/*
	 * Build up the command to execute in the shell to get the plain
	 *  gettext file.
	 */
	cmd=g_strdup_printf("msgunfmt '%s' -o '%s'",
		file,
		gtranslator_runtime_config->temp_filename);
	/* 
	 * Execute the command and test the result.
	 */
	if(!system(cmd))
	{
		/*
		 * If the command could be executed successfully, open the
		 *  plain gettext file.
		 */
		gtranslator_parse_main(gtranslator_runtime_config->temp_filename);
	}
	else
	{
		cmd=g_strdup_printf(_("Couldn't open compiled gettext file `%s'!"),
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
void open_compressed_po_file(gchar *file, gchar *command)
{
	gchar *cmd;

	if(!gtranslator_utils_check_program(command, 0))
	{
		return;
	}

	/* 
	 * Set up the command to execute in the system shell.
	 */
	cmd=g_strdup_printf("'%s' -dc < '%s' > '%s'",
		command,
		file,
		gtranslator_runtime_config->temp_filename);

	/*
	 * Execute the command and check the result.
	 */
	if(!system(cmd))
	{
		/*
		 * Open up the "new" plain gettext file.
		 */
		gtranslator_parse_main(gtranslator_runtime_config->temp_filename);
	}
	else
	{
		if(!strcmp(command, "uncompress"))
		{
			cmd=g_strdup_printf(
			_("Couldn't open compressed gettext file `%s'!"),
			file);
		}
		else
		{
			cmd=g_strdup_printf(
				/*
				 * The %s format here stands for the used
				 *  compressions program (gzip, bzip2 etc.)
				 */
				_("Couldn't open %s'd gettext file `%s'!"),
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
 * Open up the gzip'ed po file.
 */ 
void gtranslator_open_gzipped_po_file(gchar *file)
{
	g_return_if_fail(file!=NULL);

	open_compressed_po_file(file, "gzip");
}

/*
 * Open up the bzip2'ed po file.
 */
void gtranslator_open_bzip2ed_po_file(gchar *file)
{
	g_return_if_fail(file!=NULL);

	open_compressed_po_file(file, "bzip2");
}

/*
 * Open up the Z'ed po file.
 */
void gtranslator_open_compressed_po_file(gchar *file)
{
	g_return_if_fail(file!=NULL);

	open_compressed_po_file(file, "uncompress");
}

/*
 * Open up zip'ed po file.
 */
void gtranslator_open_ziped_po_file(gchar *file)
{
	gchar 	*cmd;

	g_return_if_fail(file!=NULL);

	if(!gtranslator_utils_check_program("unzip", 0))
	{
		return;
	}

	cmd=g_strdup_printf("unzip -p '%s' > '%s'", 
		file,
		gtranslator_runtime_config->temp_filename);

	if(!system(cmd))
	{
		gtranslator_parse_main(gtranslator_runtime_config->temp_filename);
	}
	else
	{
		cmd=g_strdup_printf(_("Couldn't open zip'ed po file `%s'!"),
			file);

		gnome_app_warning(GNOME_APP(gtranslator_application), cmd);
	}

	GTR_FREE(cmd);
}
