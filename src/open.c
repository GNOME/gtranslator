/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gmx.net>
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

#include "open-differently.h"
#include <libgtranslator/vfs-handle.h>


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
	#ifdef USE_VFS_STUFF
	file=gtranslator_vfs_handle_open_file(file);

	if(!file)
	{
		return FALSE;
	}
	#endif
		
	/*
	 * Reverse the filename for an easier catching
	 *  of the supported suffixes.
	 */  
	g_strreverse(file);
	
	/*
	 * For the compiled gettext files we do support
	 *  the "mo" and "gmo" suffixes; detect them.
	 */  
	if(!g_strncasecmp(file, "om.", 3)
		||!g_strncasecmp(file, "omg.", 4))
	{
		/*
		 * Reverse the filename for the function into
		 *  the original form again and open them then.
		 */
		g_strreverse(file);
		gtranslator_open_compiled_po_file(file);
		return TRUE;
	}
	/*
	 * And for the gzip'ed po files we do support
	 *  the "po.gz" suffixes. detect the suffix if
	 *   possible.
	 */  
	if(!g_strncasecmp(file, "zg.op.", 6))
	{
		/*
		 * This function opens up the gzip'ed gettext file
		 *  via the special gtranslator function.
		 */ 
		g_strreverse(file);
		gtranslator_open_gzipped_po_file(file);
		return TRUE;
	}

	/*
	 * Reverse the filename again into the original
	 *  form to allow the normal parsing routines to
	 *   work.
	 */   
	g_strreverse(file);
	return FALSE;
}

/*
 * Open up the given compiled gettext file.
 */
void gtranslator_open_compiled_po_file(gchar *file)
{
	gchar *cmd=g_new0(gchar,1);
	gchar *tempfilename=g_new0(gchar,1);
	/*
	 * Set the name of the temporary file we will be using.
	 */
	tempfilename=g_strdup_printf("%s/gtranslator-temp-po-file",
		g_get_home_dir());
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
		parse(tempfilename);
	}
	else
	{
		cmd=g_strdup_printf(_("Couldn't open compiled gettext file `%s'!"),
			file);
		/*
		 * Show a warning to the user.
		 */
		gnome_app_warning(GNOME_APP(app1), cmd);
	}

	g_free(cmd);
	g_free(tempfilename);
}

/*
 * This function is almost only an adaptation of the gtranslator_open_compiled_po_file
 *  function.
 */
void gtranslator_open_gzipped_po_file(gchar *file)
{
	gchar *cmd=g_new0(gchar,1);
	gchar *tempfilename=g_new0(gchar,1);

	/*
	 * Build the a temporary filename in the same way as for the
	 *  gtranslator_open_compiled_po_file function.
	 */
	tempfilename=g_strdup_printf("%s/gtranslator-temp-po-file",
		g_get_home_dir());
	/* 
	 * Set up the command to execute in the system shell.
	 */
	cmd=g_strdup_printf("gzip -dc < %s > %s",
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
		parse(tempfilename);
	}
	else
	{
		cmd=g_strdup_printf(_("Couldn't open gzip'd gettext file `%s'!"),
			file);
		/*
		 * Display the warning to the user.
		 */
		gnome_app_warning(GNOME_APP(app1), cmd);
	}
	
	g_free(cmd);
	g_free(tempfilename);
}
