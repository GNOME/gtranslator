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
	gchar temp_char[126];
	gchar *zamane;
        guint lines=1;
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
	gnome_appbar_refresh(GNOME_APPBAR(appbar1));
        /**
        * Open the parse fstream
        **/
        fs=fopen(po,"r+");
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
        sprintf(status,_("Finished reading \"%s\", %i lines."),po,lines);
        gnome_appbar_set_status(GNOME_APPBAR(appbar1),status);
	gnome_appbar_refresh(GNOME_APPBAR(appbar1));
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
	/*
	* Set up an informative status message
	**/
	gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Parsing the list entries."));
        for(count=1;count<(lines-1);count++)
        {
		/**
                * Create a gtr_msg structure(*)
                **/
		gtr_msg *message[count];
		/**
		* Get the current data into a temp. char
		**/
        	(gpointer)zamane=g_list_nth_data(temp,count);
		g_print("<`%i'> `%s'.\n",count,(gchar *)zamane);
		if(!g_strcasecmp(zamane,"msgid \""))
		{
			message[count]->msgid=zamane;
			message[count]->pos=count;
			g_print("Birim `%i' : `%s' .\n",message[count]->pos,message[count]->msgid);
		}
		else
		{
			/**
			* If we do get a "^msgstr" :
			**/
			if(!g_strcasecmp(zamane,"msgstr \""))
			{
				/**
				* Check if a msgid has been already found.
				**/
				if(message[count]->msgid)
				{
					/**
					* If we've got one set the struct infos
					**/
					message[count]->msgstr=zamane;
					/**
					* A fake comment as I'dn't integrated	
					*  a comments-setting function yet.
					**/
					message[count]->comment="An example comment";
					/**
					* If the msgstr contains more than msgstr ""
					**/
					if(strlen(zamane)>8)	
					{
						message[count]->msg_status=GTRANSLATOR_MSG_STATUS_TRANSLATED;
					}
					/**
					* If not set the approximiate status.
					**/
					else
					{
						message[count]->msg_status=GTRANSLATOR_MSG_STATUS_UNTRANSLATED;
					}
				}
			}
		}
        }
		
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
