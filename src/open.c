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
#include "open-differently.h"
#include "parse.h"
#include "utils.h"
#include "vfs-handle.h"

#include <string.h>
#include <stdlib.h>

#include <libgnome/gnome-util.h>
#include <libgnomeui/gnome-app-util.h>

/*
 * The "backend" for the gzip. bzip2 and uncompress based functions.
 */
void open_compressed_po_file(gchar *file, gchar *command);

/*
 * Check if the needed uncompressing program is present.
 */
void check_for_prog(const gchar *prog); 

/*
 * Detects whether we can open up the given file with the
 *  "special" open functions of gtranslator.
 */
gboolean gtranslator_open_po_file(gchar *file)
{
	/*
	 * Use conditionally the VFS routines to access
	 *  remote files.
	 */
	if(nautilus_str_has_prefix(file, "http://")       ||
		nautilus_str_has_prefix(file, "https://") ||	
		nautilus_str_has_prefix(file, "ftp://")   ||
		nautilus_str_has_prefix(file, "www.")     ||
		nautilus_str_has_prefix(file, "ftp.")     ||
		nautilus_str_has_prefix(file, "file:/"))
	{
	
		file=gtranslator_vfs_handle_open(file);

		/*
		 * If we couldn't get a local representation filename
		 *  for the remote file we do return FALSE.
		 */  
		if(!file)
		{
			return FALSE;
		}
		else
		{
			/*
			 * Here we do open the local representation file
			 *  of the remote file.
			 */  
			gtranslator_parse_main(file);
			
			return TRUE;
		}
	}
	else if(nautilus_str_has_prefix(file, "about:"))
	{
		gtranslator_about_dialog(NULL, NULL);
		return TRUE;
	}

	if(nautilus_istr_has_suffix(file, ".mo") || 
		nautilus_istr_has_suffix(file, ".gmo"))
	{
		gtranslator_open_compiled_po_file(file);
		return TRUE;
	}
	else if(nautilus_istr_has_suffix(file, ".po.gz"))
	{
		gtranslator_open_gzipped_po_file(file);
		return TRUE;
	}
	else if(nautilus_istr_has_suffix(file, ".po.bz2"))
	{
		gtranslator_open_bzip2ed_po_file(file);
		return TRUE;
	}
	else if(nautilus_istr_has_suffix(file, ".po.z"))
	{
		gtranslator_open_compressed_po_file(file);
		return TRUE;
	}
	else if(nautilus_istr_has_suffix(file, ".po.zip"))
	{
		gtranslator_open_ziped_po_file(file);
		return TRUE;
	}
	else if(gtranslator_backend_open(file))
	{
		return TRUE;
	}
	
	return FALSE;
}

/*
 * Check for the given uncompression program.
 */
void check_for_prog(const gchar *prog)
{
	if(!gnome_is_program_in_path(prog))
	{
		gchar *warn;

		warn=g_strdup_printf(_("The necessary uncompression program `%s' is missing!"), prog);

		gnome_app_warning(GNOME_APP(gtranslator_application), warn);

		g_free(warn);
		
		return;
	}
}

/*
 * Open up the given compiled gettext file.
 */
void gtranslator_open_compiled_po_file(gchar *file)
{
	gchar *cmd;
	gchar *tempfilename;

	check_for_prog("msgunfmt");

	tempfilename=gtranslator_utils_get_temp_file_name();
	
	/*
	 * Build up the command to execute in the shell to get the plain
	 *  gettext file.
	 */
	cmd=g_strdup_printf("msgunfmt %s -o %s",
		file,
		tempfilename);
	/* 
	 * Execute the command and test the result.
	 */
	if(!system(cmd))
	{
		/*
		 * If the command could be executed successfully, open the
		 *  plain gettext file.
		 */
		gtranslator_parse_main(tempfilename);
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

	g_free(cmd);
	g_free(tempfilename);
}

/*
 * This acts as the backend function for the gzip & bzip2'ed po file
 *  functions.
 */
void open_compressed_po_file(gchar *file, gchar *command)
{
	gchar *cmd;
	gchar *tempfilename;

	check_for_prog(command);

	tempfilename=gtranslator_utils_get_temp_file_name();
	
	/* 
	 * Set up the command to execute in the system shell.
	 */
	cmd=g_strdup_printf("%s -dc < %s > %s",
		command,
		file,
		tempfilename);

	/*
	 * Execute the command and check the result.
	 */
	if(!system(cmd))
	{
		/*
		 * Open up the "new" plain gettext file.
		 */
		gtranslator_parse_main(tempfilename);
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
	
	g_free(cmd);
	g_free(tempfilename);
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
	gchar *cmd, *tempfilename;

	g_return_if_fail(file!=NULL);

	check_for_prog("unzip");

	tempfilename=gtranslator_utils_get_temp_file_name();

	cmd=g_strdup_printf("unzip -p %s > %s", 
		file,
		tempfilename);

	if(!system(cmd))
	{
		gtranslator_parse_main(tempfilename);
	}
	else
	{
		cmd=g_strdup_printf(_("Couldn't open zip'ed po file `%s'!"),
			file);

		gnome_app_warning(GNOME_APP(gtranslator_application), cmd);
	}

	g_free(cmd);
	g_free(tempfilename);
}
