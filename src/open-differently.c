/*
 * (C) 2000 	Fatih Demir <kabalak@gmx.net>
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

/*
 * Open up the given compiled gettext file.
 */
void gtranslator_open_compiled_po_file(gchar *file)
{
	gchar *cmd;
	gchar *tempfilename;
	/*
	 * Set the name of the temporary file we will be using.
	 */
	tempfilename=g_strdup_printf("%s/temp_po_file",
		((g_getenv("TMPDIR")) ? g_getenv("TMPDIR"): "/tmp"));
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
	gchar *cmd;
	gchar *tempfilename;
	/*
	 * Build the a temporary filename in the same way as for the
	 *  gtranslator_open_compiled_po_file function.
	 */
	tempfilename=g_strdup_printf("%s/temp_po_file",
		((g_getenv("TMPDIR")) ? g_getenv("TMPDIR"): "/tmp"));
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
