/*
 * (C) 2000-2001 	Fatih Demir <kabalak@kabalak.net>
 * 			Gediminas Paulauskas <menesis@kabalak.net>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gui.h"
#include "parse.h"
#include "prefs.h"
#include "utils.h"

#include <stdlib.h>

#include <gtk/gtkmain.h>

#include <libgnomeui/gnome-app-util.h>

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
	command=g_strdup_printf("'%s' '%s' '%s' '%s' 2>&1 1>/dev/null",
		SCRIPTSDIR "/my-update.sh",
		g_path_get_basename(po->filename),
		g_path_get_dirname(po->filename),
		po->header->prj_name);
	
	/*
	 * Get the filename.
	 */
	newfile=g_strdup(po->filename);
	
	/*
	 * Close the file before updating
	 */
	gtranslator_file_close(NULL, NULL);
	
	if (file_opened != FALSE) {
		GTR_FREE(newfile);
		GTR_FREE(command);
		return;
	}

	/*
	 * Execute the command.
	 */
	res=system(command);

	/*
	 * Iterate back to the main thread which draws the application --
	 *  this way the user still sees the application GUI and not something
	 *   very unwanted and unnice .-)
	 */
	while(gtk_events_pending())
	{
		gtk_main_iteration();
	}
	
	/*
	 * Before doing an update, my-update.sh checks if
	 *  there's the POTFILE.in file present; if this file
	 *   is missing it returns 201.
	 */
	if((res >> 8)==201)
	{
		gnome_app_error(GNOME_APP(gtranslator_application),
			_("No POTFILES.in found!"));
	}
	
	/*
	 * The update.sh script returns 200 if the diff
	 *  between the old file and the new file is smaller
	 *   then some lame bits (for the date field).
	 */
	if((res >> 8)==200)
	{
		gnome_app_message(GNOME_APP(gtranslator_application),
			_("Po file is uptodate."));
	}
	else
	{
		if(res==0)
		{
			gnome_app_message(GNOME_APP(gtranslator_application),
				_("The update was successfull."));
		}
		else
		{
			gnome_app_warning(GNOME_APP(gtranslator_application),
				_("An error occurred while updating the po file."));	
		}
	}
	
	/*
	 * Parse the file again.
	 */
	gtranslator_parse_main(newfile);
	gtranslator_parse_main_extra();
	GTR_FREE(command);
	GTR_FREE(newfile);
}