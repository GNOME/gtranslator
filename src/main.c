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
#include "color-schemes.h"
#include "defines.include"
#include "dialogs.h"
#include "gui.h"
#include "htmlizer.h"
#include "open-differently.h"
#include "parse.h"
#include "prefs.h"
#include "query.h"
#include "session.h"
#include "sighandling.h"
#include "utils.h"

#include <gtk/gtkmain.h>

#include <signal.h>

#include <libgnome/gnome-util.h>
#include <libgnomeui/gnome-init.h>
#include <libgnomeui/gnome-window-icon.h>

#ifdef GCONF_IS_PRESENT
#include <gconf/gconf.h>
#endif

/*
 * The static variables used in the poptTable.
 */
static gchar 	*gtranslator_geometry=NULL;
static gchar 	*scheme_filename=NULL;
static gchar 	*save_html_output_file=NULL;
static gchar 	*domains_dir=NULL;
static gboolean	build_informations=FALSE;

/*
 * gtranslator's option table.
 */
static struct poptOption gtranslator_options[] = {
	{
	 	NULL, '\0', POPT_ARG_INTL_DOMAIN, PACKAGE,
	 	0, NULL, NULL
	},
	{
		"build-informations", 'b', POPT_ARG_NONE, &build_informations,
		0, N_("Shows build informations/specifications"), NULL
	},
	{
		"geometry", 'g', POPT_ARG_STRING, &gtranslator_geometry,
		0, N_("Specifies the main-window geometry"), N_("GEOMETRY")
	},
	{
		"querydomaindir", 'q', POPT_ARG_STRING, &domains_dir,
		0, N_("Define another query domains directory"), N_("LOCALEDIR")
	},
	{
		"scheme", 's', POPT_ARG_STRING, &scheme_filename,
		0, N_("Syntax color scheme to use"), N_("SCHEMEFILE")
	},
	{
		"webalize", 'w', POPT_ARG_STRING, &save_html_output_file,
		0, N_("HTML file to write to"), N_("HTMLFILE")
	},
	POPT_AUTOHELP {NULL}
};

int main(int argc, char *argv[])
{
	GnomeClient *client;
	GnomeClientFlags flags;
	poptContext context;
	const char **args;
	gchar *sp_file;

	/*
	 * GConf error handling variable.
	 */
	#ifdef GCONF_IS_PRESENT
	GError	*error=NULL;
	#endif

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
	signal(SIGSEGV, gtranslator_signal_handler);
	signal(SIGINT, gtranslator_signal_handler);
	signal(SIGHUP, gtranslator_signal_handler);
	signal(SIGQUIT, gtranslator_signal_handler);

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
	 * Show up build informations if desired.
	 */
	if(build_informations)
	{
		#define NICE_PRINT(x); \
			g_print("\n\t\t%s", x); \
			g_print("\n\t");

		g_print("\t");
		g_print(_("gtranslator build informations/specs:"));
		g_print("\n\n\t");
		g_print(_("Version and build date:"));
		NICE_PRINT(BUILD_STRING);
		g_print(_("Build-Glib/Gtk+/Gnome versions:"));
		NICE_PRINT(BUILD_VERSIONS);
		g_print(_("Backends directory:"));
		NICE_PRINT(BACKENDS_DIR);
		g_print(_("TM backends directory:"));
		NICE_PRINT(BACKENDS_TM_DIR);
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
	 * Show the application window with icon.
	 */
	gnome_window_icon_set_default_from_file(WINDOW_ICON);
	
	/* 
	 * Initialize the regular expression cache 
	 */
	rxc = gnome_regex_cache_new_with_size(20);
	gtranslator_preferences_read();
	
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
	 * Create the main app-window. 
	 */
	gtranslator_create_main_window();
	gtranslator_utils_restore_geometry(gtranslator_geometry);
	
	/*
	 * Check if a temporary file from the last session had been
	 *  left.
	 */
	sp_file=g_strdup_printf("%s/%s", g_get_home_dir(),
		"gtranslator-temp-po-file");
	
	if(g_file_exists(sp_file))
	{
		unlink(sp_file);
	}

	g_free(sp_file);

	/*
	 * Test if there's a crash recovery file lying around in ~.
	 */
	sp_file=g_strdup_printf("%s/%s", g_get_home_dir(),
		".crash-gtranslator.po");
	
	if(g_file_exists(sp_file))
	{
		gtranslator_rescue_file_dialog();
	}

	g_free(sp_file);
	
	/*
	 * Load the given color scheme file.
	 */ 
	if(scheme_filename)
	{
		/*
		 * Apply the given color scheme if possible.
		 */ 
		gtranslator_color_scheme_apply(scheme_filename);
	}

	/*
	 * Load the applied color scheme from the prefs and check it; if it
	 *  doesn't seem to be right apply the original default color names.
	 */ 
	theme=gtranslator_color_scheme_load_from_prefs();

	if(!theme)
	{
		gtranslator_color_scheme_restore_default();
		theme=gtranslator_color_scheme_load_from_prefs();
	}
	
	/*
	 * Set up the text boxes with the _new_ style.
	 */ 
	gtranslator_config_init();
	gtranslator_set_style(text_box);
	gtranslator_set_style(trans_box);
	gtranslator_config_close();
	
	/*
	 * Parse the domains in the given directory or in GNOMELOCALEDIR.
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
		gtranslator_query_domains(GNOMELOCALEDIR);
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
	if (flags & GNOME_CLIENT_RESTORED)
	{
		gtranslator_session_restore(client);
	}
	
	gtk_widget_show_all(gtranslator_application);
	
	/*
	 * Enter the Gtk+ main-loop.
	 */
	gtk_main();
	return 0;
}

