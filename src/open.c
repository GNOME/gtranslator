/*
 * (C) 2000-2001 	Fatih Demir <kabalak@kabalak.net>
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
gboolean _open_compressed_po_file(gchar *file, gchar *command, GError **error);

/*
 * Internally used functions' prototypes.
 */ 
gboolean _gtranslator_open_compiled_po_file(gchar *file, GError **error);
gboolean _gtranslator_open_gzipped_po_file(gchar *file, GError **error);
gboolean _gtranslator_open_bzip2ed_po_file(gchar *file, GError **error);
gboolean _gtranslator_open_compressed_po_file(gchar *file, GError **error);
gboolean _gtranslator_open_ziped_po_file(gchar *file, GError **error); 

/*
 * Wrapper for a plethora of different file-type handlers.
 */
gboolean gtranslator_open_file(gchar *filename, GError **error)
{
	g_return_if_fail(filename!=NULL);
	
	/*
	 * Intercept about dialog request
	 */
	if(nautilus_str_has_prefix(filename, "about:"))
	{
		gtranslator_about_dialog(NULL, NULL);
		return TRUE;
	}

	/*
	 * Use conditionally the VFS routines to access
	 *  remote files.
	 */  
	if(gtranslator_utils_uri_supported(filename))
	{
		filename = gtranslator_vfs_handle_open(filename);
		if(filename != NULL) {
			return _gtranslator_read_from(filename, error);
		}
	}

	/*
	 * need to msgunfmt it?
	 */
	if(nautilus_istr_has_suffix(filename, ".mo") || 
		nautilus_istr_has_suffix(filename, ".gmo"))
	{
		return _gtranslator_open_compiled_po_file(filename, error);
	}

	/*
	 * need to decompress it?
	 */
	if(nautilus_istr_has_suffix(filename, ".po.gz"))
	{
		return _gtranslator_open_gzipped_po_file(filename, error);
	}
	else if(nautilus_istr_has_suffix(filename, ".po.bz2"))
	{
		return _gtranslator_open_bzip2ed_po_file(filename, error);
	}
	else if(nautilus_istr_has_suffix(filename, ".po.z"))
	{
		return _gtranslator_open_compressed_po_file(filename, error);
	}
	else if(nautilus_istr_has_suffix(filename, ".po.zip"))
	{
		return _gtranslator_open_ziped_po_file(filename, error);
	}

	return _gtranslator_read_from(filename, error);
}

/*
 * Open up the given compiled gettext file.
 */
gboolean _gtranslator_open_compiled_po_file(gchar *file, GError **error)
{
	gchar *cmd, *warning;

	if(!gtranslator_utils_check_program("msgunfmt", 0))
	{
		g_set_error(error,
			GTR_OPEN_FILE_ERROR,
			GTR_OPEN_FILE_ERROR_MISSING_PROGRAM,
			_("Couldn't find the '%s' program."),
			"msgunfmt");
		return FALSE;
	}

	/*
	 * Build up the command to execute in the shell to get the plain
	 *  gettext file.
	 */
	cmd = g_strdup_printf("msgunfmt '%s' -o '%s'",
		file,
		gtranslator_runtime_config->temp_filename);

	/* 
	 * Execute the command and test the result.
	 */
	if(system(cmd))
	{
		g_set_error(error,
			GTR_OPEN_FILE_ERROR,
			GTR_OPEN_FILE_ERROR_OTHER,
			_("Couldn't open compiled gettext file `%s'!"),
			file);
		g_free(cmd);
		return FALSE;
	}
	g_free(cmd);

		/*
		 * If the command could be executed successfully, open the
		 *  plain gettext file.
		 */
	if(!gtranslator_open_file(
		gtranslator_runtime_config->temp_filename, error))
	{
		return FALSE;
	}

	return TRUE;
}

/*
 * This acts as the backend function for the gzip & bzip2'ed po file
 *  functions.
 */
gboolean _open_compressed_po_file(gchar *file, gchar *command, GError **error)
{
	gchar *cmd;

	if(!gtranslator_utils_check_program(command, 0))
	{
		g_set_error(error,
			GTR_OPEN_FILE_ERROR,
			GTR_OPEN_FILE_ERROR_MISSING_PROGRAM,
			_("Couldn't find the '%s' program."),
			command);
		return FALSE;
	}

	/* 
	 * Set up the command to execute in the system shell.
	 */
	cmd=g_strdup_printf("'%s' -dc < '%s' > '%s'",
		command,
		file,
		gtranslator_runtime_config->temp_filename);

	/*
	 * Execute the command and test the result.
	 */
	if(system(cmd))
	{
		g_set_error(error,
			GTR_OPEN_FILE_ERROR,
			GTR_OPEN_FILE_ERROR_OTHER,
			_("Couldn't open compiled gettext file `%s' with '%s'!"),
			file, command);
		g_free(cmd);
		return FALSE;
	}
	g_free(cmd);

	/*
	 * Open up the "new" plain gettext file.
	 */
	if(!gtranslator_parse_main(gtranslator_runtime_config->temp_filename,
		error))
	{
		return FALSE;
	}

	return TRUE;
}

/*
 * Open up the gzip'ed po file.
 */ 
gboolean _gtranslator_open_gzipped_po_file(gchar *file, GError **error)
{
	g_return_if_fail(file!=NULL);

	return _open_compressed_po_file(file, "gzip", error);
}

/*
 * Open up the bzip2'ed po file.
 */
gboolean _gtranslator_open_bzip2ed_po_file(gchar *file, GError **error)
{
	g_return_if_fail(file!=NULL);

	return _open_compressed_po_file(file, "bzip2", error);
}

/*
 * Open up the Z'ed po file.
 */
gboolean _gtranslator_open_compressed_po_file(gchar *file, GError **error)
{
	g_return_if_fail(file!=NULL);

	return _open_compressed_po_file(file, "uncompress", error);
}

/*
 * Open up zip'ed po file.
 */
gboolean _gtranslator_open_ziped_po_file(gchar *file, GError **error)
{
	gchar 	*cmd;

	g_return_if_fail(file!=NULL);

	if(!gtranslator_utils_check_program("unzip", 0))
	{
		g_set_error(error,
			GTR_OPEN_FILE_ERROR,
			GTR_OPEN_FILE_ERROR_MISSING_PROGRAM,
			_("Couldn't find the '%s' program."),
			"unzip");
		return FALSE;
	}

	cmd=g_strdup_printf("unzip -p '%s' > '%s'", 
		file,
		gtranslator_runtime_config->temp_filename);

	if(system(cmd))
	{
		g_set_error(error,
			GTR_OPEN_FILE_ERROR,
			GTR_OPEN_FILE_ERROR_OTHER,
			_("Couldn't open compiled gettext file `%s'!"),
			file);
		g_free(cmd);
		return FALSE;
	}
	g_free(cmd);

	return _gtranslator_read_from(
		gtranslator_runtime_config->temp_filename, error);
}

/*
 * Common parser code
 */
gboolean _gtranslator_read_from(gchar *file, GError **error)
{
	if(!gtranslator_parse_main(file, error))
	{
		return FALSE;
	}
	return TRUE;
}

GQuark
gtranslator_open_file_error_quark (void)
{
  static GQuark quark = 0;
  if (!quark)
    quark = g_quark_from_static_string ("gtranslator_open_file_error");

  return quark;
}

