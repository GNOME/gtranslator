/*
 * (C) 2000-2002 	Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
 *			Peeter Vois <peeter@gtranslator.org>
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
#include "open.h"
#include "parse.h"
#include "prefs.h"
#include "runtime-config.h"
#include "save.h"
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
static gchar	*learn_file=NULL;
static gchar	*auto_translate_file=NULL;
static gchar	*exporting_po_file=NULL;
static gboolean	build_information=FALSE;
static gboolean	no_modules=FALSE;
gboolean 	nosyntax=FALSE;

/*
 * gtranslator's option table.
 */
static struct poptOption gtranslator_options[] = {
	{
	 	NULL, '\0', POPT_ARG_INTL_DOMAIN, PACKAGE,
	 	0, NULL, NULL
	},
	{
		"auto-translate", 'a', POPT_ARG_STRING, &auto_translate_file,
		0, N_("Auto translate the po file & exit"), N_("FILENAME")
	},
	{
		"build-information", 'b', POPT_ARG_NONE, &build_information,
		0, N_("Show build information/specifications"), NULL
	},
	{
		"export-learn-buffer", 'e', POPT_ARG_STRING, &exporting_po_file,
		0, N_("Export learn buffer to a plain po file"), N_("PO_FILE")
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
		"webalize", 'w', POPT_ARG_STRING, &save_html_output_file,
		0, N_("HTML file to write to"), N_("HTMLFILE")
	},
	{
		"nosyntax", 'y', POPT_ARG_NONE, &nosyntax,
		0, N_("Don't highlight syntax"), NULL 
	},
	POPT_AUTOHELP {NULL}
};

int main(int argc, char *argv[])
{
	GnomeClient 	*client=NULL;
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
	signal(SIGKILL, gtranslator_signal_handler);
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

	/* Initialize configuration client */
	gtranslator_config_init();

	/*
	 * Read all of our "normal" preferences -- translator data is now
	 *  outsourced into the GtrTranslator structure.
	 */
	gtranslator_preferences_read();

	/*
	 * If the loading of the modules isn't inhibited, try to load all
	 *  backends.
	 */
	if(!GtrPreferences.load_backends || no_modules)
	{
		backends=NULL;
	}
	else
	{
		/*
		 * Test first if the local libgmodule supports modules at all.
		 */
		if(!g_module_supported())
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

	/*
	 * Show the application window with icon.
	 */
	if(!auto_translate_file || !learn_file)
	{
		gnome_window_icon_set_default_from_file(WINDOW_ICON);
	}

	/*
	 * Create our own .gtranslator directory in the user's home directory.
	 */
	gtranslator_utils_create_gtranslator_directory();

	/*
	 * Read the translator information/data into our generally used 
	 *  GtrTranslator structure.
	 */
	if(!gtranslator_translator)
	{
		gtranslator_translator=gtranslator_translator_new();
	}
	
	if(!auto_translate_file || !learn_file)
	{
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
	}

	/*
	 * Initialize our generally used GtrRuntimeConfig structure.
	 */
	gtranslator_runtime_config=gtranslator_runtime_config_new();

	/* 
	 * Create the main app-window. 
	 */
	if(!auto_translate_file || !learn_file)
	{
		gtranslator_create_main_window();
		gtranslator_utils_restore_geometry(gtranslator_geometry);
	}

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

	if(!auto_translate_file || !learn_file)
	{
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
	}
	
	/*
	 * If there are any files given on command line, open them
	 */
	file_opened = FALSE;
	args = poptGetArgs(context);

	/*
	 * Auto translate the given file argument and exit without starting the
	 *  GUI building routines.
	 */
	if(auto_translate_file)
	{
		/*
		 * Initialize learn buffer, open file and auto translate all
		 *  possible strings.
		 */
		gtranslator_learn_init();
		gtranslator_open_file(auto_translate_file);
		gtranslator_learn_autotranslate(FALSE);
		
		/*
		 * If any change has been made to the po file: save it.
		 */
		if(po->file_changed)
		{
			if(!gtranslator_save_po_file(auto_translate_file))
			{
				gtranslator_save_file(auto_translate_file);
			}
		}

		gtranslator_learn_shutdown();
		
		/*
		 * Set up the "runtime/filename" config. key to a sane value.
		 */
		gtranslator_config_set_string("runtime/filename", "--- No file ---");

		/*
		 * Free all till now allocated stuff.
		 */
		gtranslator_translator_free(gtranslator_translator);
		gtranslator_preferences_free();

		/*
		 * Shutdown GnomeVFS.
		 */
		if(gnome_vfs_initialized())
		{
			gnome_vfs_shutdown();
		}

		return 0;
	}

	/*
	 * Open up the arguments as files (for now, only the first file is
	 *  opened).
	 */
	if (args)
	{
		/*
		 * Try to open up the supported "special" gettext file types.
		 */ 
		gtranslator_open_file((gchar *)args[0]);

		/*
		 * Also write the HTML output of the given file if
		 *  desired.
		 */  
		if(save_html_output_file)
		{
			gtranslator_htmlizer(save_html_output_file);
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
	
	if(!auto_translate_file || !learn_file)
	{
		/*
		 * Check the session client flags, and restore state if needed 
		 */
		flags = gnome_client_get_flags(client);
	
		if(flags & GNOME_CLIENT_RESTORED)
		{
			gtranslator_session_restore(client);
		}
	}

	/*
	 * Init the learn buffer and connected stuff.
	 */
	gtranslator_learn_init();

	/*
	 * Check if any filename for learning was supplied and if yes, learn
	 *  the file completely, shutdown the learn buffer & exit afterwards.
	 */
	if(learn_file)
	{
		/*
		 * First parse the file completely.
		 */
		gtranslator_open_file(learn_file);

		/*
		 * Now learn the file completely and then shut the
		 *  learn system down.
		 */
		gtranslator_learn_po_file(po);
		gtranslator_learn_shutdown();

		/*
		 * Set up the "runtime/filename" config. key to a sane value.
		 */
		gtranslator_config_set_string("runtime/filename", "--- No file ---");

		/*
		 * Free all till now allocated stuff.
		 */
		gtranslator_translator_free(gtranslator_translator);
		gtranslator_preferences_free();

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
	
	/*
	 * If we've got the task to export a learn buffer to a plain po file
	 *  then we should do this now after all the other tasks we could have
	 *   to have to be done (?!).
	 */
	if(exporting_po_file && exporting_po_file[0]!='\0')
	{
		gtranslator_learn_export_to_po_file(exporting_po_file);

		/*
		 * The usual free'ing orgies are now coming along...
		 */
		
		gtranslator_learn_shutdown();

		/*
		 * Set up the "runtime/filename" config. key to a sane value.
		 */
		gtranslator_config_set_string("runtime/filename", "--- No file ---");

		/*
		 * Free all till now allocated stuff.
		 */
		gtranslator_translator_free(gtranslator_translator);
		gtranslator_preferences_free();

		/*
		 * Shutdown GnomeVFS.
		 */
		if(gnome_vfs_initialized())
		{
			gnome_vfs_shutdown();
		}

		/*
		 * Give us another small status feedback about the export.
		 */
		g_print(_("Exported learn buffer to `%s'.\n"), exporting_po_file);

		return 0;
	}
	
	gtk_widget_show_all(gtranslator_application);

	/*
	 * Create the list of colorschemes if we are starting up in a GUI.
	 */
	gtranslator_color_scheme_create_schemes_list();

	/*
	 * Now do also show up the full colorschemes list in the "_Settings"
	 *  main menu.
	 */
	gtranslator_color_scheme_show_list();
	
	gtk_main();
	return 0;
}
