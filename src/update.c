#include "parse.h"
#include "prefs.h"
#include "gui.h"

/**
* The update function.
**/
void update(GtkWidget *widget, gpointer useless)
{
	gint res=1;
	gchar *command;
	gchar *newfile;

	/**
	* Build this magical line.
	* FIXME: use real ./update.pl found in packages
	**/
	command=g_strdup_printf("%s %s %s 2>&1 1>/dev/null", SCRIPTSDIR "/update.sh",
		po->filename, po->header->prj_name);
	/**
	* Get the filename.
	**/
	newfile=g_strdup(po->filename);
	/**
	* Close the file before updating
	**/
	close_file(NULL, NULL);
	if (file_opened != FALSE) {
		g_free(newfile);
		g_free(command);
		return;
	}

	/**
	* Execute the command.
	**/
	res=system(command);
	/**
	* If you wish'em, you get'em ..
	**/
	if(wants.uzi_dialogs)
	{
		if(res!=0)
			gnome_app_message(GNOME_APP(app1),
				_("An update would cause no changes."));
		else
			gnome_app_message(GNOME_APP(app1),
				_("The update was successfull."));
	}
	parse(newfile);
	g_free(command);
	g_free(newfile);
}
