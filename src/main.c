/*
 * (C) 2000-2007 	Fatih Demir <kabalak@kabalak.net>
 *			Ross Golder <ross@golder.org>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Peeter Vois <peeter@kabalak.net>
 * 			Ignacio Casal <nacho.resa@gmail.com>
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
#include "application.h"
#include "prefs-manager-app.h"
#include "plugins-engine.h"
#include "utils.h"

#include <locale.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gio/gio.h>

#include <gconf/gconf.h>

static gchar **file_arguments = NULL;

static const GOptionEntry options [] =
{
	{ G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_FILENAME_ARRAY, &file_arguments,
	  NULL, N_("[FILE...]") }, /* collects file arguments */

	{NULL}
};

static GSList *
get_command_line_data ()
{
	GSList *file_list = NULL;
	
	if (file_arguments)
	{
		gint i;

		for (i = 0; file_arguments[i]; i++) 
		{			
			GFile  *file;
			gchar *uri;

			file = g_file_new_for_commandline_arg (file_arguments[i]);
			uri = g_file_get_uri (file);
			g_object_unref (file);

			
			if (uri != NULL){
				file_list = g_slist_prepend (file_list, 
							     uri);
				
			}
			else
				g_print (_("%s: malformed file name or URI.\n"),
					 file_arguments[i]);
		}

		file_list = g_slist_reverse (file_list);
	}
	
	return file_list;
}

/*
 * The ubiquitous main function...
 */
gint
main(gint argc,
     gchar *argv[])
{
	GError *error = NULL;
	GtranslatorPluginsEngine *engine;
	GtranslatorWindow *window;
	GSList *file_list = NULL;
	GOptionContext *context;
	gchar *filename;
	gchar *config_folder;
	GList *profiles_list = NULL;
	GFile *file;

	/*
	 * Initialize gettext.
	 */ 
	setlocale (LC_ALL, "");
	
	bindtextdomain(GETTEXT_PACKAGE, GNOMELOCALEDIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);

	g_set_application_name(_("Gtranslator"));
	
	/* Setup command line options */
	context = g_option_context_new (_("- Edit PO files"));
	g_option_context_add_main_entries (context, options, GETTEXT_PACKAGE);

	/*
	 * Initialize the GConf library.
	 */
	if(!(gconf_init(argc, argv, &error)))
	{
		if(error)
		{
			g_warning(_("Error during GConf initialization: %s."),
				error->message);
		}

		g_clear_error(&error);
	}

	if (!g_thread_supported()) g_thread_init(NULL);
	gtk_init(&argc, &argv);
	
	g_option_context_parse(context, &argc, &argv, NULL);

	/*
	 * Show the application window with icon.
	 */
	gtk_window_set_default_icon_from_file(WINDOW_ICON, &error);
	if(error)
	{
		g_warning(_("Error setting the default window icon: %s"),
			  error->message);
		g_clear_error(&error);
	}
	
	/*
	 * We set the default icon dir
	 */
	gtk_icon_theme_append_search_path (gtk_icon_theme_get_default (),
					   PIXMAPSDIR);
	
	/*
	 * Init preferences manager
	 */
	gtranslator_prefs_manager_app_init();
	
	/*
	 * Init plugin engine
	 */
	engine = gtranslator_plugins_engine_get_default ();
	
	gtk_about_dialog_set_url_hook (gtranslator_utils_activate_url, NULL, NULL);

	/*
	 * Load profiles list
	 */
	 config_folder = gtranslator_utils_get_user_config_dir ();
	 filename = g_build_filename (config_folder,
				      "profiles.xml",
				      NULL);
	 file = g_file_new_for_path (filename);
  
	 if (g_file_query_exists (file, NULL)) {
	   profiles_list = gtranslator_profile_get_profiles_from_xml_file (filename);
	 }

	 gtranslator_application_set_profiles (GTR_APP, profiles_list);
		
	/* 
	 * Create the main app-window. 
	 */
	window = gtranslator_application_open_window(GTR_APP);
	
	/*
	 * Now we open the files passed as arguments
	 */
	file_list = get_command_line_data ();
	if (file_list)
	{
		gtranslator_actions_load_uris (window, (const GSList *)file_list);
		g_slist_foreach (file_list, (GFunc) g_free, NULL);
		g_slist_free (file_list);
	}
	
	/*
	 * Enter main GTK loop
	 */
	gtk_main();
	
	gtranslator_prefs_manager_app_shutdown();

	return 0;
}
