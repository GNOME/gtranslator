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
#endif // HAVE_CONFIG_H

#include <gnome.h>

#include <popt-gnome.h>
#include "msg_db.h"
#include "dnd.h"
#include "interface.h"
#include "support.h"
#include "session.h"
#include "messages.h"
#include "prefs.h" 

/**
* Some static variables for the poptTable
**/
static gchar *file_to_open=NULL;

/**
* The popt-options table
**/
static struct poptOption gtranslator_options [] = {
	{
		"file", 'f', POPT_ARG_STRING, &file_to_open,
		0,N_("Po-file to open at startup"), "PO_FILE"
	},
	{
		"msg-db", 'm', POPT_ARG_STRING, &msg_db,
		0,N_("The msg_db to use"),"MSG_DB"
	},
	{
		"geometry", 'g', POPT_ARG_STRING, &gtranslator_geometry,
		0,N_("Specifies the main-window geometry"),"GEOMETRY"
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
	* The main-windows widget
	**/
	GtkWidget *app1;
	/**
	* For the arguments
	**/
	poptContext context;
	
#ifdef ENABLE_NLS
	bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
	textdomain(PACKAGE);
#endif
	gnome_init_with_popt_table("gtranslator", VERSION, argc, argv, 
		gtranslator_options, 0, &context);
	
	/**
	* Free the poptContext
	**/
	poptFreeContext(context);
	/**
	* Read the stored preferences
	**/
	read_prefs();
	/**
	* Get the client
	**/
	client=gnome_master_client();
	/**
	* Connects the signalc for Sessionmanagement
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
	app1=create_app1();
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
	gtk_widget_show(app1);
        /**
        * Init the msg_db.
        **/
	init_msg_db();
	/**
	* Is a po-file given to start with ?
	**/
	if((file_to_open) && (strlen(file_to_open)>0))
	{
		FILE *test;
		/**
		* Test if the file which was specified on the cmd-line exists
		**/
		test=fopen(file_to_open,"r+");
		if(test==NULL)	
		{
			/**
			* If the file isn't openable give a warning
			**/
			g_warning(_("The file \"%s\" doesn't exist or isn't readable!"),file_to_open);
			g_warning(_("Skipping it."));
		}
		else
		{	
			/**
			* Parse/open the file
			**/
			parse(file_to_open);
			file_opened=TRUE;
			i_love_this_file=file_to_open;
		}
	}
	/**
	* Disable the buttons if no file is opened.
	**/
	if(file_opened!=TRUE)
	{
		disable_buttons();
	}
	/**
	* Disable the ReSearch button on startup
	**/
	gtk_widget_set_sensitive(search_again_button,FALSE);
	/**
	* Return to the main Gtk+ main-loop
	**/
	gtk_main();
	return 0;
}
