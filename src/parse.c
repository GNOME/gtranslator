/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* Here will be some useful parts of gtranslator 
* next periods of time ....
*
* -- the source
**/

#include "parse.h"
#include <errno.h>
#include <unistd.h>
#include "gtr_dialogs.h"
#include "header_stuff.h"

/**
* The file-stream for the file-check & the message parsing
**/
FILE *fs;

/**
* A simple stream-check (I love the ifstream.good()-func from C++ ....)
**/
void check_file(FILE *stream)
{
	if(stream == NULL)
	{
		/**
		* If there are any problems , give a
		*  message ..
		**/
		g_error(_("\nThe file stream is lost! Error no. `%i'"),errno);
		
	}
}

/**
* The internally used parse-function
**/
void parse(gchar *po_file)
{
	/**
	* Some variables
	**/
	gchar temp_char[126];
        guint lines=1;
	/**
        * If there's no selection ( is this possible within a Gtk+ fileselection ? )
        **/
        if((!po_file)||(strlen(po_file)<=0))
        {
                g_error(_("There's no file to open or I couldn't understand `%s'!"),po_file);
        }
        /**
        * Set up a status message
        **/
        sprintf(status,_("Current file : \"%s\"."),po_file);
        gnome_appbar_set_status(GNOME_APPBAR(appbar1),status);
        /**
        * Open the parse fstream
        **/
        fs=fopen(po_file,"r+");
        /**
        * Allocate the lists
        **/
        temp=g_list_alloc();
        head=g_list_alloc();
        /**
        * Parse the file ...
        **/
        while(
        fgets(status,sizeof(temp_char),fs) != NULL
        )
        {
                temp=g_list_append(temp,(gpointer)temp_char);
        }
        /**
        * The list length ( aka lines count )
        **/
        lines=g_list_length(temp);
        /**
        * Show an updated status
        **/
        sprintf(status,_("Finished reading \"%s\", %i lines."),po_file,lines);
        gnome_appbar_set_status(GNOME_APPBAR(appbar1),status);
	file_opened=TRUE;
        for(count=1;count<lines;count++)
        {
                /** TODO **/
        }
}

/**
* The new routine
**/
void parse_the_file(GtkWidget *widget,gpointer filename)
{
	gchar *po_file;
	/**
	* Get the filename from the widget
	**/
	po_file=gtk_file_selection_get_filename(GTK_FILE_SELECTION(of_dlg));
	gtk_widget_hide(of_dlg);
	/**
	* Call the function above
	**/
	parse(po_file);
}
