/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
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

#include <popt-gnome.h>

#include "gui.h"
#include "session.h"
#include "prefs.h"
#include "parse.h"
#include "open-differently.h"
#include "htmlizer.h"

#ifdef GCONF_IS_PRESENT
#include <gconf/gconf.h>
#endif

/*
 * The static variables used in the poptTable.
 */
static gchar *gtranslator_geometry = NULL;
static gchar *save_html_output_file = NULL;

/*
 * gtranslator's option table.
 */
static struct poptOption gtranslator_options[] = {
	{
	 	NULL, '\0', POPT_ARG_INTL_DOMAIN, PACKAGE,
	 	0, NULL, NULL
	},
	{
		"geometry", 'g', POPT_ARG_STRING, &gtranslator_geometry,
		0, N_("Specifies the main-window geometry"), "GEOMETRY"
	},
	{
		"webalize", 'w', POPT_ARG_STRING, &save_html_output_file,
		0, N_("HTML file to write to"), "HTMLFILE"
	},
	POPT_AUTOHELP {NULL}
};

int main(int argc, char *argv[])
{
	GnomeClient *client;
	GnomeClientFlags flags;
	poptContext context;
	const char **args;
	
	/*
	 * GConf error handling variable.
	 */
	#ifdef GCONF_IS_PRESENT
	GError	*error=NULL;
	#endif

	/*
	 * Initialize gettext.
	 */ 
	
	bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
	textdomain(PACKAGE);

	/*
	 * Initialize the GConf library conditionally.
	 */
	#ifdef GCONF_IS_PRESENT
	if(!(gconf_init(argc,argv, &error)))
	{
		if(error)
		{
			g_warning(_("Error during GConf initialization: %s."),
				error->message);
		}
		g_clear_error(&error);
	}
	#endif
	
	/*
	 * Initialize gtranslator within libgnomeui.
	 */
	gnome_init_with_popt_table("gtranslator", VERSION, argc, argv,
				   gtranslator_options, 0, &context);

	/* 
	 * Initialize the regular expression cache 
	 */
	rxc = gnome_regex_cache_new_with_size(20);
	read_prefs();
	
	/*
	 * Get the master session management client.
	 */
	client = gnome_master_client();
	
	/*
	 * Connect the signals needed for session management.
	 */
	gtk_signal_connect(GTK_OBJECT(client), "save_yourself",
			   GTK_SIGNAL_FUNC(gtranslator_sleep),
			   (gpointer) argv[0]);
	gtk_signal_connect(GTK_OBJECT(client), "die",
			   GTK_SIGNAL_FUNC(gtranslator_dies_for_you), NULL);

	/* 
	 * Create the main app-window. 
	 */
	create_app1();
	restore_geometry(gtranslator_geometry);

	/*
	 * Check if a temporary file from the last session had been
	 *  left.
	 */
	if(g_file_exists(g_strdup_printf("%s/%s",
				g_get_home_dir(),
				"gtranslator-temp-po-file")))
	{
		unlink(g_strdup_printf("%s/%s",
				g_get_home_dir(), 
				"gtranslator-temp-po-file"));
	}
	
	/*
	 * If there are any files given on command line, open them
	 * TODO add here loading of all files, when program becomes MDI
	 */
	file_opened = FALSE;
	args = poptGetArgs(context);
	
	/*
	 * Open up the arguments as files (for now, only the first file is
	 *  opened).
	 */
	if (args)
	{
		/*
		 * Try to open up the supported "special" gettext file types.
		 */ 
		if(!gtranslator_open_po_file((gchar *)args[0]))
		{
			/*
			 * Open the file as a "normal" gettext po file
			 */ 
			parse(args[0]);
			
			/*
			 * Also write the HTML output of the given file if
			 *  desired.
			 */  
			if(save_html_output_file)
			{
				gtranslator_htmlizer(po, save_html_output_file);
			}
		}
	}
	
	poptFreeContext(context);
	
	/*
	 * Disable the buttons if no file is opened.
	 */
	if (!file_opened)
	{
		disable_actions_no_file();
	}
	
	/*
	 * Check the session client flags, and restore state if needed 
	 */
	flags = gnome_client_get_flags(client);
	if (flags & GNOME_CLIENT_RESTORED)
	{
		restore_session(client);
	}
	
	/*
	 * Show the application window (with icon, if requested)
	 */
	#ifdef USE_WINDOW_ICON
	gnome_window_icon_set_from_file(GTK_WINDOW(app1), WINDOW_ICON);
	#endif
	gtk_widget_show_all(app1);
	
	/*
	 * Enter the Gtk+ main-loop.
	 */
	gtk_main();
	return 0;
}

