/**
* Fatih Demir <kabalak@gmx.net>
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* The "initial"-file 
*
* -- only source 
**/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <popt-gnome.h>

#include "gui.h"
#include "session.h"
#include "prefs.h"
#include "parse.h"
#include <libgtranslator/parse-db.h>
#include <libgtranslator/team-handle.h>

#ifdef GCONF_IS_PRESENT
#include <gconf/gconf.h>
#endif

/**
* Some static variables for the poptTable in the main routines.
**/
static gchar *gtranslator_geometry = NULL;

/**
* The popt-options table
**/
static struct poptOption gtranslator_options[] = {
	{
	 NULL, '\0', POPT_ARG_INTL_DOMAIN, PACKAGE,
	 0, NULL, NULL
	},
	{
	 "geometry", 'g', POPT_ARG_STRING, &gtranslator_geometry,
	 0, N_("Specifies the main-window geometry"), "GEOMETRY"
	},
	POPT_AUTOHELP {NULL}
};

int main(int argc, char *argv[])
{
	/**
	* The Sessionmanagement client
	**/
	GnomeClient *client;
	GnomeClientFlags flags;
	/**
	* For the arguments
	**/
	poptContext context;
	const char **args;
	/**
	* The GConf error stuff.
	**/
	#ifdef GCONF_IS_PRESENT
	GError	*error=NULL;
	#endif
	
	/**
	* Is i18n-support wished ?
	**/
	#ifdef ENABLE_NLS
	bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
	textdomain(PACKAGE);
	#endif

	/**
	* Do we have GConf? If yes, init it. 
	**/
	#ifdef GCONF_IS_PRESENT
	if(!(gconf_init(argc,argv, &error)))
	{
		/**
		* If there's an error description.
		**/
		if(error)
		{
			g_warning(_("Error during GConf initialization: %s."), error->message);
		}
		/**
		* Clean up the error.
		**/
		g_clear_error(&error);
	}
	#endif
	
	/**
	* Init gtranslator.
	**/
	gnome_init_with_popt_table("gtranslator", VERSION, argc, argv,
				   gtranslator_options, 0, &context);
	/* Initialize the regular expression cache */
	rxc = gnome_regex_cache_new_with_size(20);
	read_prefs();
	/**
	* Get the client
	**/
	client = gnome_master_client();
	/**
	* Connect the signals for Sessionmanagement
	**/
	gtk_signal_connect(GTK_OBJECT(client), "save_yourself",
			   GTK_SIGNAL_FUNC(gtranslator_sleep),
			   (gpointer) argv[0]);
	gtk_signal_connect(GTK_OBJECT(client), "die",
			   GTK_SIGNAL_FUNC(gtranslator_dies_for_you), NULL);

	/* Create the main app-window */
	create_app1();
	restore_geometry(gtranslator_geometry);

	/**
	* If there are any files given on command line, open them
	* TODO add here loading of all files, when program becomes MDI
	**/
	file_opened = FALSE;
	args = poptGetArgs(context);

	if (args)
		parse(args[0]);
	poptFreeContext(context);
	/* Disable the buttons if no file is opened. */
	if (!file_opened)
		disable_actions_no_file();
	/* Disable spell checking, because it's not written yet */
	disable_actions(ACT_SPELL);

	/* Check the session client flags, and restore state if needed */
	flags = gnome_client_get_flags(client);
	if (flags & GNOME_CLIENT_RESTORED)
		restore_session(client);
	/* Show the application window, with icon, if requested */
	#ifdef USE_WINDOW_ICON
	gnome_window_icon_set_from_file(GTK_WINDOW(app1), WINDOW_ICON);
	#endif
	gtk_widget_show_all(app1);
	/* Enter the Gtk+ main-loop */
	gtk_main();
	return 0;
}

