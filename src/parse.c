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
#include <error.h>
#include <errno.h>
#include <unistd.h>

/**
* The file-stream for the file-check & the message parsing
**/
FILE *fs;

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
		g_error(_("\nThe file stream is lost!\n%s"),strerror(errno));
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
	gchar *zamane=NULL;
        guint lines=1,z=0,msg_pair=0;
	/**
        * If there's no selection ( is this possible within a Gtk+ fileselection ? )
        **/
        if(!po)
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
	* Set the global filename
	**/
	i_love_this_file=po;
	/**
	* Parse the file ...
	**/
	while(
	fgets(temp_char,sizeof(temp_char),fs)!=NULL
	)
	{
		/**
		* Create a new structure.
		**/
		gtr_msg *msg=g_new0(gtr_msg,1);
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
		/* 1/ */!g_strncasecmp(temp_char,"\"",1)
		&&
		/* 2/ */strstr(temp_char,": ")
		&&
		/* 3/ */(msg_pair<=1)
		)
		{
			/**
			* Use the functions defined & used in header_stuff.*
			*  to rip the header off.
			**/
			get_header(temp_char);
		}
		if(!g_strncasecmp(temp_char,"#: ",3))
		{
			/**
			* Create the gtr_msg structure
			*  and set the comment & position.
			**/
			msg_pair++;
			msg->pos=z;
			msg->comment=g_strdup(temp_char);
		}
		if(!g_strncasecmp(temp_char,"msgid \"",7))
		{
			/**
			* The msgid itself
			**/
			msg->msgid=g_strdup(temp_char);
		}
		if(!g_strncasecmp(temp_char,"msgstr \"",8))
		{
			/**
			* The msgstr
			**/
			msg->msgstr=g_strdup(temp_char);
		}
		/**
		* If a structure is existent, free it.
		**/
		if(msg!=NULL)
		{
			g_free(msg);
		}
	}
	/**
	* Show an updated status
	**/
	sprintf(status,_("Finished reading \"%s\", %i lines."),po,z);
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
	usleep(500000);
	#endif // HAVE_USLEEP
	/**
	* As we've got finished we can do some nonsense
	**/
	enable_buttons();
	gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Parsing has been successfull."));
}

/**
* The new routine
**/
void parse_the_file(GtkWidget *widget,gpointer useless)
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

/**
* Get's the first msg.
**/
void get_first_msg(GtkWidget *widget,gpointer useless)
{
	/* TODO */
}

/**
* The real search function
**/
gchar *search_do(GtkWidget *widget,gpointer wherefrom)
{
	switch((gint)wherefrom)
	{
		case 1:
			g_print("SEARCH! HAS TO BE DONE!\n");
			break;
		case 2:
			g_print("RESEARCH! HAS ALSO TO BE DONE!\n");
			break;
		default :
			break;
	}
}
