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
#include "interface.h"
#include "support.h"
#include "session.h"
#include "messages.h"
#include "prefs.h" 

static gchar *file_to_open=NULL;
static gint yazoo=0;

/**
* The popt-options table
**/
static struct poptOption gtranslator_options [] = {
	{
		"file", 'f', POPT_ARG_STRING, &file_to_open,
		0,N_("Po-file to open at startup"), "PO-FILE"
	},
	{
		"yazoo-test", 'y', POPT_ARG_INT, &yazoo,
		0,N_("A test option"), "TEST"
	},
	{
		"msg_db", 'm', POPT_ARG_STRING, &msg_db,
		0,N_("The msg_db to use"),"MSG_DB"
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
	gtk_widget_show(app1);
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
		}
	}
	/**
	* Get the main window geometry :
 	**/
	gtranslator_geometry=gnome_geometry_string(app1->window);
	/**
	* Return to the main Gtk+ main-loop
	**/
	gtk_main();
	return 0;
}
