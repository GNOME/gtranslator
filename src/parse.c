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
        guint lines=1,z=0,msg_pair=0;
	gboolean msgid_ok=FALSE,msgstr_ok=FALSE,comment_ok=FALSE;
	messages=g_list_alloc();
	messages=NULL;
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
	* Parse the file ...
	**/
	while(
	fgets(temp_char,sizeof(temp_char),fs)!=NULL
	)
	{
		/**
		* Create a new structure.
		**/
		gtr_msg *msg=g_new(gtr_msg,1);
		/**
		* Set'em to "".
		**/
		msg->msgid="";
		msg->msgstr=msg->comment=msg->msgid;
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
			/** FIXME 
			*get_header(temp_char);
			**/
		}
		if(!g_strncasecmp(temp_char,"#: ",3))
		{
			/**
			* Create the gtr_msg structure
			*  and set the comment & position.
			**/
			(gint)msg->pos=z;
			(gchar *)msg->comment=(gchar *)g_strdup(temp_char);
			comment_ok=TRUE;
		}
		/**
		* If it's an msgid
		**/
		if(!g_strncasecmp(temp_char,"msgid \"",7))
		{
			gint mid,minid;
			gchar aw[strlen(temp_char)];
			/**
			* The msgid itself
			**/
			minid=0;
			for(mid=6;mid<(strlen(temp_char));++mid)
			{
				aw[minid]=temp_char[mid];
				minid++;
			}	
			(gchar *)msg->msgid=(gchar *)g_strdup(aw);
			msgid_ok=TRUE;
		}
		/**
		* If it's an msgstr.
		**/
		if(!g_strncasecmp(temp_char,"msgstr \"",8))
		{
			gint mstr,mistr;
			gchar aq[strlen(temp_char)];
			/**
			* The msgstr
			**/
			mistr=0;
			for(mstr=7;mstr<(strlen(temp_char));++mstr)
			{
				aq[mistr]=temp_char[mstr];
				mistr++;
			}
			(gchar *)msg->msgstr=(gchar *)g_strdup(aq);
			/**
			* Check the status...
			**/
			if(!g_strcasecmp(((gchar *)msg->msgstr),"\"\""))
			{
				(gtr_msg_status)msg->msg_status=GTRANSLATOR_MSG_STATUS_UNTRANSLATED;
			}
			else
			{
				if(((gchar *)msg->msgstr)!=NULL)
				{
					(gtr_msg_status)msg->msg_status=GTRANSLATOR_MSG_STATUS_TRANSLATED;
				}
				else
				{
					(gtr_msg_status)msg->msg_status=GTRANSLATOR_MSG_STATUS_UNKNOWN;
				}
			}
			msgstr_ok=TRUE;
			msg_pair++;
		}
		/**
		* Already a msgid but not yet a msgstr. This can't be an empty line, or ?
		**/
		if((msgid_ok==TRUE) && (msgstr_ok!=TRUE))
		{
			(gchar *)msg->msgid=g_strconcat((gchar *)msg->msgid,(gchar *)g_strdup(temp_char),NULL);
		}
		/**
		* Yet a msgstr and not a comment.
		**/
		if((msgstr_ok==TRUE) && (comment_ok!=TRUE) && (temp_char!=NULL) && (strlen(temp_char)>0))
		{
			(gchar *)msg->msgstr=g_strconcat((gchar *)msg->msgstr,(gchar *)g_strdup(temp_char),NULL);
		}
		/**
		* we've got both msgid + msgstr -> add the gtr_msg structure.
		**/
		if((msgid_ok==TRUE) && (msgstr_ok==TRUE))
		{
			messages=g_list_append(messages,(gpointer)msg);
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
	#endif
	/**
	* As we've got finished we can do some nonsense
	**/
	enable_buttons();
	gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Parsing has been successfull."));
	/**
	* Set the msg_pair count.
	**/
	msg_pair_count=msg_pair;
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
* Cleans up the text boxes.
**/
void clean_text_boxes()
{
	gtk_text_backward_delete(GTK_TEXT(text1),gtk_text_get_length(GTK_TEXT(text1)));
	gtk_text_backward_delete(GTK_TEXT(trans_box),gtk_text_get_length(GTK_TEXT(trans_box)));
	gtk_text_set_editable(GTK_TEXT(text1),TRUE);
}

/**
* Get's the first msg.
**/
void get_first_msg(GtkWidget *widget,gpointer useless)
{
	gtr_msg *first=g_new(gtr_msg,1);
	clean_text_boxes();
	first=(gtr_msg *)g_list_nth_data(messages,0);
	gtk_text_insert(GTK_TEXT(text1),NULL,NULL,NULL,(gchar *)first->msgid,-1);
	gtk_text_insert(GTK_TEXT(trans_box),NULL,NULL,NULL,(gchar *)first->msgstr,-1);
	if(first)
	{
		gtk_widget_set_sensitive(last_button,TRUE);
		gtk_widget_set_sensitive(next_button,TRUE);
		gtk_widget_set_sensitive(first_button,FALSE);
		gtk_widget_set_sensitive(back_button,FALSE);
		g_free(first);
	}
}

/**
* Get the previous message.
**/
void get_prev_msg(GtkWidget *widget,gpointer useless)
{
	gtr_msg *prev=g_new(gtr_msg,1);
	clean_text_boxes();
	msg_pair--;
	if(msg_pair<=0)
	{
		prev=(gtr_msg *)g_list_nth_data(messages,0);
                gtk_text_insert(GTK_TEXT(text1),NULL,NULL,NULL,(gchar *)prev->msgid,-1);
                gtk_text_insert(GTK_TEXT(trans_box),NULL,NULL,NULL,(gchar *)prev->msgstr,-1);
		gtk_widget_set_sensitive(last_button,TRUE);
                gtk_widget_set_sensitive(next_button,TRUE);
                gtk_widget_set_sensitive(first_button,FALSE);
                gtk_widget_set_sensitive(back_button,FALSE);
		gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("You've reached the first message"));
	}
	else
	{
		prev=(gtr_msg *)g_list_nth_data(messages,msg_pair);
		if(g_list_length(messages)>2)
		{
			gtk_widget_set_sensitive(next_button,TRUE);
		}
		gtk_widget_set_sensitive(back_button,TRUE);
		gtk_widget_set_sensitive(first_button,TRUE);
		gtk_text_insert(GTK_TEXT(text1),NULL,NULL,NULL,(gchar *)prev->msgid,-1);
                gtk_text_insert(GTK_TEXT(trans_box),NULL,NULL,NULL,(gchar *)prev->msgstr,-1);
	}
	if(prev)
	{
		g_free(prev);
	}
}

/**
* Get the next message
**/
void get_next_msg(GtkWidget *widget,gpointer useless)
{
	gtr_msg *next=g_new(gtr_msg,1);
	clean_text_boxes();
	msg_pair++;
	if(msg_pair==(g_list_length(messages)-1))
	{
		next=(gtr_msg *)g_list_nth_data(messages,(g_list_length(messages)-1));
	        gtk_text_insert(GTK_TEXT(text1),NULL,NULL,NULL,(gchar *)next->msgid,-1);
		gtk_text_insert(GTK_TEXT(trans_box),NULL,NULL,NULL,(gchar *)next->msgstr,-1);
		gtk_widget_set_sensitive(first_button,TRUE);
                gtk_widget_set_sensitive(back_button,TRUE);
                gtk_widget_set_sensitive(last_button,FALSE);
                gtk_widget_set_sensitive(next_button,FALSE);
		gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("You've reached the last message."));
	}
	else
	{
		next=(gtr_msg *)g_list_nth_data(messages,msg_pair);
		if(msg_pair>2)
		{
			gtk_widget_set_sensitive(back_button,TRUE);
		}
		gtk_widget_set_sensitive(next_button,TRUE);
                gtk_widget_set_sensitive(last_button,TRUE);
		gtk_text_insert(GTK_TEXT(text1),NULL,NULL,NULL,(gchar *)next->msgid,-1);
                gtk_text_insert(GTK_TEXT(trans_box),NULL,NULL,NULL,(gchar *)next->msgstr,-1);
	}
	if(next)
	{
		g_free(next);
	}
}

/**
* Get the last entry.
**/
void get_last_msg(GtkWidget *widget,gpointer useless)
{
	gtr_msg *last=g_new(gtr_msg,1);
	clean_text_boxes();
	last=(gtr_msg *)g_list_nth_data(messages,(g_list_length(messages)-1));
	gtk_text_insert(GTK_TEXT(text1),NULL,NULL,NULL,(gchar *)last->msgid,-1);
	gtk_text_insert(GTK_TEXT(trans_box),NULL,NULL,NULL,(gchar *)last->msgstr,-1);
	if(last)
	{
		gtk_widget_set_sensitive(first_button,TRUE);
		gtk_widget_set_sensitive(back_button,TRUE);
		gtk_widget_set_sensitive(last_button,FALSE);
		gtk_widget_set_sensitive(next_button,FALSE);
		g_free(last);
	}
}

/**
* The real search function
**/
gchar *search_do(GtkWidget *widget,gpointer wherefrom)
{
	switch((gint)wherefrom)
	{
		case 1:
			g_print("SEARCH! HAS TO BE DONE! ;)\n");
			break;
		case 2:
			g_print("RESEARCH! HAS ALSO TO BE DONE! ;)\n");
			break;
		default :
			break;
	}
}
