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

/**
* The file-stream for the file-check & the message parsing
**/
FILE *fs;

GList *a;

/**
* A simple stream-check (I love the ifstream.good()-func from C++ ....)
**/
void check_file(FILE *stream)
{
	if(stream==NULL)
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
void parse(gchar *po)
{
	/**
	* Some variables
	**/
	gchar temp_char[128];
	gchar *zamane;
        guint lines=1,z=0;
	guint msgid_count,msgstr_count,comments_count;
	msgid_count=msgstr_count=comments_count=0;
	/**
        * If there's no selection ( is this possible within a Gtk+ fileselection ? )
        **/
        if((!po)||(strlen(po)<=0))
        {
                g_warning(_("There's no file to open or I couldn't understand `%s'!"),po);
        }
        /**
        * Set up a status message
        **/
        sprintf(status,_("Current file : \"%s\"."),po);
        gnome_appbar_set_status(GNOME_APPBAR(appbar1),status);	
        /**
        * Open the parse fstream
        **/
        fs=fopen(po,"r+");
	/**
	* Check the file-stream
	**/
	check_file(fs);
        /**
        * Allocate the lists
        **/
        head=g_list_alloc();
	temp=g_list_alloc();	
        /**
        * Parse the file ...
        **/
        while(
        fgets(temp_char,sizeof(temp_char),fs)!=NULL
        )
        {
		z++;
		/**
		* Try to get the header :
		*
		* 1/If it starts with "
		* 2/And includes a ": " sequence
		* 3/And if there has been ONLY one msgid/str-pair yet
		* 
		**/
		if(
		!g_strncasecmp(temp_char,"\"",1)
		&&
		strstr(temp_char,": ")
		&&
		(msgid_count<=1)&&(msgstr_count<=1)
		)
		{
			/**
			* Add the current header-line
			**/
			g_print("HEADER : %s",temp_char);
			head=g_list_prepend(temp,(gpointer)temp_char);
			temp=head;
		}
		if(!g_strncasecmp(temp_char,"#: ",3))
		{
			comments_count++;
		}
		if(!g_strncasecmp(temp_char,"msgid \"",7))
		{
			msgid_count++;
		}
		if(!g_strncasecmp(temp_char,"msgstr \"",8))
		{
			msgstr_count++;
		}
        }
	g_print("List length : \"%i\"\n",g_list_length(head));
        /**
        * The list length ( aka lines count )
        **/
        lines=g_list_length(head);
	for(z=1;z<lines;z++)
	{
		g_print("Printing %i. : %s",z,(gchar *)g_list_nth_data(head,z));
	}
        /**
        * Show an updated status
        **/
        sprintf(status,_("Finished reading \"%s\", %i lines."),po,lines);
        gnome_appbar_set_status(GNOME_APPBAR(appbar1),status);
	/**
	* So the other functions can get a point 
	**/
	file_opened=TRUE;
	#ifdef HAVE_USLEEP
	/**
	* Wait for a small amount of time while the user can read the status message
	*  above, if he has got usleep on his machine.
	**/
	usleep(150000);
	#endif // HAVE_USLEEP
	/**
	* As we've got finished we can do some nonsense
	**/
	apply_header();
	enable_buttons();
	gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Parsing has been successfull."));
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
