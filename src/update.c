/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gtranslator.org>
 * 			Gediminas Paulauskas <menesis@gtranslator.org>
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

/*
 * The update function of gtranslator's UI is based upon this file and the
 *  my-update.sh script in gtranslator's data directory.
 */

#include "parse.h"
#include "prefs.h"
#include "gui.h"

/*
 * The update function.
 */
void update(GtkWidget *widget, gpointer useless)
{
	/*
	 * The default return value is 200 -- it caused no changes.
	 */
	gint res=200;
	gchar *command;
	gchar *newfile;
	
	/*
	 * Build this magical line.
	 */
	command=g_strdup_printf("%s %s %s 2>&1 1>/dev/null",
		SCRIPTSDIR "/my-update.sh",
		po->filename,
		po->header->prj_name);
	
	/*
	 * Get the filename.
	 */
	newfile=g_strdup(po->filename);
	
	/*
	 * Close the file before updating
	 */
	close_file(NULL, NULL);
	
	if (file_opened != FALSE) {
		g_free(newfile);
		g_free(command);
		return;
	}

	/*
	 * Execute the command.
	 */
	res=system(command);
	
	/*
	 * Before doing an update, my-update.sh checks if
	 *  there's the POTFILE.in file present; if this file
	 *   is missing it returns 201.
	 */
	if(res==201)
	{
		gnome_app_error(GNOME_APP(app1),
			_("No POTFILES.in found!"));
	}
	
	/*
	 * If you wish'em, you get'em ..
	 */
	if(wants.uzi_dialogs)
	{
		/*
		 * The update.sh script returns 200 if the diff
		 *  between the old file and the new file is smaller
		 *   then some lame bits (for the date field).
		 */
		if(res==200)
		{
			gnome_app_message(GNOME_APP(app1),
				_("An update caused no changes."));
		}
		else
		{
			gnome_app_message(GNOME_APP(app1),
				_("The update was successfull."));
		}
	}
	
	/*
	 * Parse the file again.
	 */
	parse(newfile);
	g_free(command);
	g_free(newfile);
}
