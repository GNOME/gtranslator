/**
* Fatih Demir [ kabalak@gmx.net ]
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

#include <gnome.h>

#include <popt-gnome.h>
#include "msg_db.h"
#include "gui.h"
#include "session.h"
#include "prefs.h" 

/**
* Some static variables for the poptTable in the main routines.
**/
static gchar *gtranslator_geometry=NULL;
static gchar *query_string=NULL;

/**
* The popt-options table
**/
static struct poptOption gtranslator_options [] = {
	{
		NULL, '\0', POPT_ARG_INTL_DOMAIN, PACKAGE,
		0, NULL, NULL
	},
	{
		"challenge-length", 'c', POPT_ARG_INT, &challen,
		0, N_("Sets the minimum match for query"), "LENGTH"
	},
	{
                "geometry", 'g', POPT_ARG_STRING, &gtranslator_geometry,
                0, N_("Specifies the main-window geometry"),"GEOMETRY"
        },
	{
		"msg-db", 'm', POPT_ARG_STRING, &msg_db,
		0, N_("The messages db to use"),"MSG_DB"
	},
	{
		"query", 'q', POPT_ARG_STRING, &query_string,
		0, N_("Query the messages db & exit"),"QUERY_STRING"
	},
	POPT_AUTOHELP {NULL}
};

int main(int argc,char *argv[])
{
	/**
	* The Sessionmanagement client
	**/
	GnomeClient *client;
	/**
	* For the arguments
	**/
	poptContext context;
	const char** args;
	/**
	* Check whether NLS is requested.
	**/
	#ifdef ENABLE_NLS
	bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
	textdomain(PACKAGE);
	#endif
	/**
	* Init gtranslator.
	**/
	gnome_init_with_popt_table("gtranslator", VERSION, argc, argv, 
		gtranslator_options, 0, &context);
	args=poptGetArgs(context);
	/**
	* Is a po-file given to start with ?
	* TODO add here laoding of all files, when program becomes MDI
	**/
	if (args)
		parse(args[0]);
	/**
	* Free the poptContext
	**/
	poptFreeContext(context);
	if (challen)
		set_challenge_length(challen);
	else
		set_challenge_length(2);
	/**
	* Read the stored preferences
	**/
	read_prefs();
	/**
        * Init the msg_db.
        **/
        init_msg_db();
	/**
        * If a query has been done.
        **/
        if(query_string)
        {
		/**
		* Simply print it out.
		**/
                g_print(_("Getting results for query: `%s' ...\n"),query_string);
		/**
		* This should be a clearly sightable separator line.
		**/
		g_print("-----------------------------------------------------\n");
		g_print(_("Result                   :  %s."),get_from_msg_db(query_string));
		g_print("\n");
		g_print(_("End of query.\n"));
			/**
			* Exit within the non-GUI circles.
			**/
			exit(0);
        }
	/**
	* Get the client
	**/
	client=gnome_master_client();
	/**
	* Connects the signals for Sessionmanagement
	**/
	gtk_signal_connect(GTK_OBJECT(client), "save_yourself",
		GTK_SIGNAL_FUNC(gtranslator_sleep), (gpointer) argv[0]);
	gtk_signal_connect(GTK_OBJECT(client), "die",
		GTK_SIGNAL_FUNC(gtranslator_dies_for_you), NULL);
	/**
	* Call our stored Sessionmanagement informations now ..
	**/
	if(gnome_client_get_flags(client) & GNOME_CLIENT_RESTORED)
	{
		gnome_config_push_prefix(gnome_client_get_config_prefix(client));
		gnome_config_pop_prefix();
	}	
	/**
	* Creates the main app-window
	**/	
	create_app1();
	/**
        * Set the main window's geometry from prefs.
        **/
        if(!gtranslator_geometry)
        {
                if(if_save_geometry==TRUE)
                {
			gtk_widget_set_uposition(app1,gtranslator_geometry_x,gtranslator_geometry_y);
			gtk_window_set_default_size(GTK_WINDOW(app1),gtranslator_geometry_w,gtranslator_geometry_h);
                }
        }
	/**
        * and if a geometry-definition has been given .. parse it.
        **/
        else
        {
                gint x, y, width, height;
                if(gnome_parse_geometry(gtranslator_geometry, &x, &y,
                        &width, &height))
		{
			if(x!=-1)
			{
				gtk_widget_set_uposition(app1, x, y);
			}
			if((width>0)&&(height>0))
			{
				gtk_window_set_default_size(GTK_WINDOW(app1), width, height);
			}
		}
		else
		{
			g_warning(_("The geometry string \"%s\" coulnd't be parsed!"),gtranslator_geometry);
		}
        }
	/**
	* Disable the buttons if no file is opened.
	**/
	if (!file_opened)
		disable_actions_no_file();
	// Show the application window, with icon, if requested
	#ifdef USE_WINDOW_ICON
        gnome_window_icon_set_from_file(GTK_WINDOW(app1),WINDOW_ICON);
	#endif
	gtk_widget_show_all(app1);
	/**
	* Enter the main Gtk+ main-loop
	**/
	gtk_main();
	return 0;
}
