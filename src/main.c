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

#include "interface.h"
#include "support.h"

int main(int argc,char *argv[])
{
	GtkWidget *app1;
#ifdef ENABLE_NLS
	bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
	textdomain (PACKAGE);
#endif
	gnome_init ("gtranslator", VERSION, argc, argv);
	/**
	* The following code was added by Glade to create one of each component
	* (except popup menus), just so that you see something after building
	* the project. Delete any components that you don't want shown initially.
	**/
	app1 = create_app1 ();
	gtk_widget_show (app1);
	/**
	* Get the main window geometry :
 	**/
	gtranslator_geometry = gnome_geometry_string(app1->window);
	gtk_main ();
	return 0;
}
