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

#include "actions.h"
#include "backend.h"
#include "color-schemes.h"
#include "defines.include"
#include "dialogs.h"
#include "gui.h"
#include "htmlizer.h"
#include "learn.h"
#include "open-differently.h"
#include "parse.h"
#include "prefs.h"
#include "query.h"
#include "runtime-config.h"
#include "session.h"
#include "sighandling.h"
#include "translator.h"
#include "utils.h"
#include "utils_gui.h"

#include <signal.h>

#include <gmodule.h>

#include <gconf/gconf.h>

#include <gtk/gtkmain.h>

#include <libgnome/gnome-util.h>
#include <libgnomeui/gnome-init.h>
#include <libgnomeui/gnome-window-icon.h>

#include <libgnomevfs/gnome-vfs-init.h>

/*
 * The static variables used in the poptTable.
 */
static gchar 	*gtranslator_geometry=NULL;
static gchar 	*save_html_output_file=NULL;
static gchar 	*domains_dir=NULL;
static gchar	*learn_file=NULL;
static gboolean	build_information=FALSE;
static gboolean	no_modules=FALSE;

/*
 * gtranslator's option table.
 */
static struct poptOption gtranslator_options[] = {
	{
	 	NULL, '\0', POPT_ARG_INTL_DOMAIN, PACKAGE,
	 	0, NULL, NULL
	},
	{
		"build-information", 'b', POPT_ARG_NONE, &build_information,
		0, N_("Show build information/specifications"), NULL
	},
	{
		"geometry", 'g', POPT_ARG_STRING, &gtranslator_geometry,
		0, N_("Specify main window geometry"), N_("GEOMETRY")
	},
	{
		"learn", 'l', POPT_ARG_STRING, &learn_file,
		0, N_("Learn the file completely & exit"), N_("FILENAME")
	},
	{
		"no-modules", 'n', POPT_ARG_NONE, &no_modules,
		0, N_("Don't load any backend modules"), NULL
	},
	{
		"querydomaindir", 'q', POPT_ARG_STRING, &domains_dir,
		0, N_("Define another query-domains directory"), N_("LOCALEDIR")
	},
	{
		"webalize", 'w', POPT_ARG_STRING, &save_html_output_file,
		0, N_("HTML file to write to"), N_("HTMLFILE")
	},
	POPT_AUTOHELP {NULL}
};

int main(int argc, char *argv[])
{
	GnomeClient 	*client;
	GnomeClientFlags flags;
	
	poptContext 	context;
	
	const char 	**args;
	
	GError		*error=NULL;

	/*
	 * Initialize gettext.
	 */ 
	bindtextdomain(PACKAGE, GNOMELOCALEDIR);
	textdomain(PACKAGE);

	/*
	 * Set up the signal handler.
	 */
	signal(SIGSEGV, gtranslator_signal_handler);
	signal(SIGILL, gtranslator_signal_handler);
	signal(SIGABRT, gtranslator_signal_handler);
	signal(SIGINT, gtranslator_signal_handler);
	signal(SIGHUP, gtranslator_signal_handler);
	signal(SIGQUIT, gtranslator_signal_handler);
	signal(SIGTERM, gtranslator_signal_handler);

	/*
	 * Initialize the GConf library.
	 */
	if(!(gconf_init(argc,argv, &error)))
	{
		if(error)
		{
			g_warning(_("Error during GConf initialization: %s."),
				error->message);
		}

		g_clear_error(&error);
	}
	
	/*
	 * Initialize gtranslator within libgnomeui.
	 */
	gnome_init_with_popt_table("gtranslator", VERSION, argc, argv,
		gtranslator_options, 0, &context);

	/*
	 * Show up build information if desired.
	 */
	if(build_information)
	{
		#define NICE_PRINT(x); \
			g_print("\n\t\t%s", x); \
			g_print("\n\t");

		g_print("\t");
		g_print(_("gtranslator build information/specs:"));
		g_print("\n\n\t");
		g_print(_("Version and build date:"));
		NICE_PRINT(BUILD_STRING);
		g_print(_("Build GLib/Gtk+/GNOME versions:"));
		NICE_PRINT(BUILD_VERSIONS);
		g_print(_("Backends directory:"));
		NICE_PRINT(BACKENDS_DIR);
		g_print(_("Colorschemes directory:"));
		NICE_PRINT(SCHEMESDIR);
		g_print(_("Scripts directory:"));
		NICE_PRINT(SCRIPTSDIR);
		g_print(_("Window icon:"));
		NICE_PRINT(WINDOW_ICON);
		g_print(_("Own locale directory:"));
		NICE_PRINT(GNOMELOCALEDIR);
		g_print("\n");

		#undef NICE_PRINT

		exit(1);
	}

	/*
	 * If the loading of the modules isn't inhibited, try to load all
	 *  backends.
	 */
	if(!no_modules)
	{
		/*
		 * Test first if the local libgmodule supports modules at all.
		 */
		if(g_module_supported()==FALSE)
		{
			g_warning(_("GModule implementation doesn't support loading dynamic modules!"));
			backends=NULL;
		}
		else
		{
			/*
			 * Load all backends from the default backends 
			 *  directory.
			 */
			 gtranslator_backend_open_all_backends(BACKENDS_DIR);
		}
	}
	else
	{
		backends=NULL;
	}

	/*
	 * Show the application window with icon.
	 */
	gnome_window_icon_set_default_from_file(WINDOW_ICON);

	/*
	 * Create our own .gtranslator directory in the user's home directory.
	 */
	gtranslator_utils_create_gtranslator_directory();

	/* 
	 * Initialize the regular expression cache 
	 */
	rxc = gnome_regex_cache_new_with_size(20);
	gtranslator_preferences_read();

	/*
	 * Read the translator information/data into our generally used 
	 *  GtrTranslator structure.
	 */
	gtranslator_translator=gtranslator_translator_new();
	
	/*
	 * Get the master session management client.
	 */
	client = gnome_master_client();
	
	/*
	 * Connect the signals needed for session management.
	 */
	gtk_signal_connect(GTK_OBJECT(client), "save_yourself",
			   GTK_SIGNAL_FUNC(gtranslator_session_sleep),
			   (gpointer) argv[0]);
	gtk_signal_connect(GTK_OBJECT(client), "die",
			   GTK_SIGNAL_FUNC(gtranslator_session_die), NULL);

	/*
	 * Initialize our generally used GtrRuntimeConfig structure.
	 */
	gtranslator_runtime_config=gtranslator_runtime_config_new();

	/* 
	 * Create the main app-window. 
	 */
	gtranslator_create_main_window();
	gtranslator_utils_restore_geometry(gtranslator_geometry);

	/*
	 * Initialize GnomeVFS right now, if needed.
	 */
	if(!gnome_vfs_initialized())
	{
		gnome_vfs_init();
	}

	/*
	 * Clean up the temporary file in the user's home dir eventually 
	 *  created by gtranslator.
	 */
	gtranslator_utils_remove_temp_files();

	/*
	 * Test if there's a crash recovery file lying around in ~/.gtranslator.
	 */
	if(g_file_exists(gtranslator_runtime_config->crash_filename))
	{
		gtranslator_rescue_file_dialog();
	}

	/*
	 * Load the applied color scheme from the prefs and check it; if it
	 *  doesn't seem to be right apply the original default colors.
	 */ 
	theme=gtranslator_color_scheme_load_from_prefs();

	if(!theme)
	{
		gtranslator_color_scheme_restore_default();
		theme=gtranslator_color_scheme_load_from_prefs();
	}
	
	/*
	 * Parse the domains in the given directory or in some other logical
	 *  manner.
	 */
	if(domains_dir)
	{
		/*
		 * Test if the given directory is even a directory.
		 */
		if(g_file_test(domains_dir, G_FILE_TEST_ISDIR))
		{
			/*
			 * Parse all entries from this directory.
			 */
			gtranslator_query_domains(domains_dir);
		}
	}
	else
	{
		/*
		 * Look into some other environment variables for a sane value
		 *  for the locale directory where the .mo files are lying
		 *   'round -- aka. "the gettext domains".
		 */
		gchar	*localedirectory=NULL;

		gtranslator_utils_get_environment_value(
			"GTRANSLATOR_LOCALEDIR:GNOMELOCALEDIR",
			&localedirectory);

		/*
		 * If the found localedirectory is a directory, then we can
		 *  really start querying all existing domains from this 
		 *   read-in directory.
		 */
		if(localedirectory && 
			g_file_test(localedirectory, G_FILE_TEST_ISDIR))
		{
			gtranslator_query_domains(localedirectory);
			GTR_FREE(localedirectory);
		}
		else
		{
			gtranslator_query_domains(GNOMELOCALEDIR);
		}
	}
	
	/*
	 * If there are any files given on command line, open them
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
			gtranslator_parse_main(args[0]);
			
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
		gtranslator_actions_set_up_state_no_file();
	}
	
	/*
	 * Check the session client flags, and restore state if needed 
	 */
	flags = gnome_client_get_flags(client);

	if(flags & GNOME_CLIENT_RESTORED)
	{
		gtranslator_session_restore(client);
	}

	/*
	 * Init the learn buffer and connected stuff.
	 */
	gtranslator_learn_init();

	/*
	 * Check if any filename for learning was supplied and if yes, 
	 *  learn the file completely, shutdown the learn buffer and exit afterwards.
	 */
	if(learn_file)
	{
		/*
		 * First parse the file completely.
		 */
		if(!gtranslator_open_po_file(learn_file))
		{
			gtranslator_parse_main(learn_file);
		}

		/*
		 * Now learn the file completely and then shut the
		 *  learn system down.
		 */
		gtranslator_learn_po_file(po);
		gtranslator_learn_shutdown();

		/*
		 * Set up the "runtime/filename" config. key to a sane value.
		 */
		gtranslator_config_init();
		gtranslator_config_set_string("runtime/filename", "--- No file ---");
		gtranslator_config_close();

		/*
		 * Free all till now allocated stuff.
		 */
		gtranslator_translator_free(gtranslator_translator);
		gtranslator_preferences_free();
		gnome_regex_cache_destroy(rxc);
		gtranslator_color_scheme_free(&theme);

		/*
		 * Shutdown GnomeVFS.
		 */
		if(gnome_vfs_initialized())
		{
			gnome_vfs_shutdown();
		}

		/*
		 * As everything seemed to went fine, print out a nice
		 *  message informing the user about the success.
		 */
		g_print(_("Learned `%s' successfully.\n"), learn_file);

		return 0;
	}
	
	gtk_widget_show_all(gtranslator_application);
	
	gtk_main();
	return 0;
}

