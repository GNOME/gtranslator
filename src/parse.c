/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* Major changes here done by Gediminas Paulauskas <menesis@delfi.lt>
* Now this file is absolutely useful. Just needs some extensions sometimes...
*
* -- the source
**/

#include "parse.h"

static GList *header_li;

// Are written below. Maybe should move to "parse.h", but they are private...
void append_line(gchar **old, const gchar *tail);
gchar * restore_msg(gchar *given);
void write_the_message(gpointer data, gpointer fs);
void actual_write(const gchar *name);
void free_a_message(gpointer data, gpointer useless);

/**
* A simple stream-check (I love the ifstream.good()-func from C++ ....)
**/
void check_file(FILE *stream)
{
	if (stream==NULL)
	{
		/**
		* If there are any problems , give a
		*  message ..
		**/
		g_error(_("\nThe file stream is lost!\n%s"),g_strerror(errno));
	}
}

/*
 * returns the status of a message
 */
GtrMsgStatus get_msg_status(GtrMsg *msg)
{
	if (msg->msgstr)
		msg->status |= GTRANSLATOR_MSG_STATUS_TRANSLATED;
	else
		msg->status &= ~GTRANSLATOR_MSG_STATUS_TRANSLATED;
	if (strstr(msg->comment,"#, fuzzy"))
		msg->status |= GTRANSLATOR_MSG_STATUS_FUZZY;
	else
		msg->status &= ~GTRANSLATOR_MSG_STATUS_FUZZY;
	return msg->status;
}

// Formats tail to look good when displayed and easier to maintain. Removes
// unneeded \'s and "'s and replaces \\n with real newline.
// Then appends this to *old and updates the pointer.
// TODO: make this use GString
void append_line(gchar **old, const gchar *tail)
{
	gchar *to_add=g_new(gchar,strlen(tail));
	gchar *result;
	gint s,d=0;
	for (s=1;s<strlen(tail)-1;s++) {
		if (tail[s]=='\\') {
			s++;
			if (tail[s]=='n')
				to_add[d++]='\n';
			else if (tail[s]=='t')
				to_add[d++]='\t';
			else if ((tail[s]=='"') || (tail[s]=='\\'))
				to_add[d++]=tail[s];
			else {
				to_add[d++]='\\';
				to_add[d++]=tail[s];
				g_warning("New escaped char found: \\%c\nAddthis to parse.c, line %i",
					tail[s], __LINE__);
			}
		} else {
			to_add[d++]=tail[s];
		}
	}
	to_add[d]=0;
	if (*old==NULL)
		result=g_strdup(to_add);
	else
		result=g_strconcat(*old,to_add,NULL);
	g_free(to_add);
	g_free(*old);
	*old=result;
}

/**
* The internally used parse-function
**/
void parse(const char *po)
{
	/**
	* The file-stream for the file-check & the message parsing
	**/
	FILE *fs;
	gchar line[128];
        guint lines=0;
	// if TRUE, means that a corresponding part is read
	gboolean msgid_ok=FALSE,msgstr_ok=FALSE,comment_ok=FALSE;
	GtrMsg *msg;

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
        g_snprintf(status,128,_("Current file : \"%s\"."),po);
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
	i_love_this_file=g_strdup(po);
	/**
	* Create a new structure.
	**/
	msg=g_new0(GtrMsg,1);
	/**
	* Parse the file line by line...
	**/
	while (fgets(line,sizeof(line),fs)!=NULL)
	{
		lines++;
		/*
		 * If it's a comment, no matter of what kind. It belongs to
		 * the message pair below
		 */
		if(line[0]=='#')
		{
			/**
			* Set the comment & position.
			**/
			if (msg->comment==NULL) {
				msg->pos=lines;
				msg->comment=g_strdup(line);
			} else {
				gchar *tmp=g_strconcat(msg->comment,line,NULL);
				g_free(msg->comment);
				msg->comment=tmp;
			}
		} else {
			// get rid of end-of-lines... later we will add ours ;)
			g_strchomp(line);
			if (strlen(line)==0) 
					msgstr_ok=TRUE;
			else 
			/**
			* If it's an msgid
			**/
			if (!g_strncasecmp(line,"msgid \"",7))
			{
				// This means the comment is completed
				comment_ok=TRUE;
				if (strlen(line)-8>0)
					append_line(&msg->msgid,&line[6]);
			} else
			/**
			* If it's an msgstr.
			**/
			if(!g_strncasecmp(line,"msgstr \"",8))
			{
				// This means the msgid is completed
				msgid_ok=TRUE;
				if (strlen(line)-9>0)
					append_line(&msg->msgstr,&line[7]);
			} else
			// A continuing msgid or msgstr
			if (line[0]=='"') {
				if ((comment_ok==TRUE) && (msgid_ok==FALSE))
					append_line(&msg->msgid,line);
				else
				if ((msgid_ok==TRUE) && (msgstr_ok==FALSE))
					append_line(&msg->msgstr,line);
				else g_assert_not_reached();
			}
			else g_assert_not_reached();
		}
		/**
		* we've got both msgid + msgstr
		**/
		if((msgid_ok==TRUE) && (msgstr_ok==TRUE))
		{
			// Check the status...
			get_msg_status(msg);
			// Add the GtrMsg structure.
			messages=g_list_append(messages,(gpointer)msg);
			// Reset the status of message
			msgid_ok=msgstr_ok=comment_ok=FALSE;
			// Create a new structure.
			msg=g_new0(GtrMsg,1);
			}
	}
	if ((msgid_ok==TRUE) && (msgstr_ok==FALSE)) {
		get_msg_status(msg);
		messages=g_list_append(messages,(gpointer)msg);
	} else
		// not needed allocated structure
		g_free(msg);
	// close the data file
	fclose(fs);
	// If the first message is header (it always should be)
	if (get_header(GTR_MSG(messages->data)))
	{
		// Unlink it from messages list
		header_li=messages;
		messages=g_list_remove_link(messages,header_li);
	}
	/**
	* Show an updated status
	**/
	g_snprintf(status,128,_("Successfully finished parsing \"%s\", %i lines."),po,lines);
	gnome_appbar_set_status(GNOME_APPBAR(appbar1),status);
	file_opened=TRUE;
	file_changed=FALSE;
	enable_actions("compile","save_as","close",
		"cut","copy","paste","clear","find","header",
		"next","last","goto",
		"translated","fuzzy","stick",
		"add",NULL);
	/*
	 * Set the current message to the first message, and show it
	 */
	cur_msg=g_list_first(messages);
	display_msg(cur_msg);
}

void parse_the_file(GtkWidget *widget,gpointer of_dlg)
{
	gchar *po_file;
	/**
	* Get the filename from the widget
	**/
	po_file=gtk_file_selection_get_filename(GTK_FILE_SELECTION(of_dlg));
	/**
	* Call the function above
	**/
	parse(po_file);
	// Destroy the dialog
	gtk_widget_destroy(GTK_WIDGET(of_dlg));
}

// Restores the formatting of a message, done in append_line
gchar * restore_msg(gchar *given)
{
	GString *rest;
	gchar *result;
	gint s,lines=0,here=8;
	
	if (!given) return "";
	
	rest=g_string_sized_new(strlen(given));
	for (s=0;s<strlen(given);s++)
	{
		if (given[s]=='\n') {
			if ((!lines) && (s+1<strlen(given))) {
				lines++;
				g_string_prepend(rest,"\"\n\"");
			}
			if ((s+1<strlen(given)) && (here<78)) {
				g_string_append(rest,"\\n\"\n\"");
				here=0;
				lines++;
			} 
			else g_string_append(rest,"\\n");
		} else 
		if (given[s]=='\"') {
			g_string_append(rest,"\\\"");
			here++;
		} else
		if (given[s]=='\t') {
			g_string_append(rest,"\\t");
			here++;
		} else
		if (given[s]=='\\') {
			g_string_append(rest,"\\\\");
			here++;
		} else {
			g_string_append_c(rest,given[s]);
		}
		here++;
		if (here>78) {
			if (!lines) {
				g_string_prepend(rest,"\"\n\"");
				here-=7;
			} else {
				g_string_insert(rest,
					(strrchr(rest->str,' ')-rest->str)+1,
					"\"\n\"");
				here=rest->len-(strrchr(rest->str,'"')-rest->str);
			}
			lines++;
		}
	}
	// Free and return
	result=g_strdup(rest->str);
	g_string_free(rest,FALSE);
	return result;
}

// Writes one message to a file stream
void write_the_message(gpointer data, gpointer fs)
{
	GtrMsg *msg=GTR_MSG(data);
	gchar *id, *str;
	
	id=restore_msg(msg->msgid);
	str=restore_msg(msg->msgstr);

	fprintf((FILE *)fs, "%smsgid \"%s\"\nmsgstr \"%s\"\n\n",
		msg->comment, id, str);

	// Free the written strings
//	g_free(id);
//	g_free(str);
}

void actual_write(const gchar *name)
{
	FILE *fs;
	fs=fopen(name,"w");
	check_file(fs);

	update_msg(cur_msg);
	write_the_message(header_li->data,(gpointer)fs);
	// Write every message to the file
	g_list_foreach(messages,(GFunc)write_the_message,(gpointer)fs);
	
	fclose(fs);
	file_changed=FALSE;
}

// A callback for OK in Save as... dialog
void save_the_file(GtkWidget *widget,gpointer sfa_dlg)
{
	gchar *po_file;
	/**
	* Get the filename from the widget
	**/
	po_file=gtk_file_selection_get_filename(GTK_FILE_SELECTION(sfa_dlg));
	actual_write(po_file);
	g_free(i_love_this_file);
	i_love_this_file=g_strdup(po_file);
	// And destroy the dialog...
	gtk_widget_destroy(GTK_WIDGET(sfa_dlg));
}

// A callback for Save
void save_current_file(GtkWidget *widget,gpointer useless)
{
	if (!file_changed) {
		if (if_dont_save_unchanged_files) return;
		if (if_warn_if_no_change) {
			GtkWidget *dialog;
			gchar *question;
			gint reply;
			question=g_strdup_printf(_("You didn't change anything in\n%s\n!\nDo you want to save it anyway?"),
				i_love_this_file);
			dialog=gnome_message_box_new(question,
				GNOME_MESSAGE_BOX_QUESTION,
				GNOME_STOCK_BUTTON_YES, GNOME_STOCK_BUTTON_NO,
				GNOME_STOCK_BUTTON_CANCEL, NULL);
			show_nice_dialog(&dialog,"gtranslator -- unchanged");
			reply=gnome_dialog_run(GNOME_DIALOG(dialog));
			g_free(question);
			if (reply!=GNOME_YES) return;
		}
	}
	actual_write(i_love_this_file);
}

void free_a_message(gpointer data, gpointer useless)
{
	g_free(GTR_MSG(data)->comment);
	g_free(GTR_MSG(data)->msgid);
	g_free(GTR_MSG(data)->msgstr);
	g_free(data);
}

void free_messages(void)
{
	g_list_foreach(messages,free_a_message,NULL);
	g_list_free(messages);
}

void close_file(GtkWidget *widget, gpointer useless)
{
	if (!file_opened) return;
	/* If user doesn't know what to do with changed file, return */
	if (!ask_to_save_file) return;
	free_messages();
	free_header(ph);
	ph=NULL;
	g_list_free_1(header_li);
	g_free(i_love_this_file);
	i_love_this_file=NULL;
	file_opened=FALSE;
	clean_text_boxes();
	disable_actions_no_file();
}

void revert_file(GtkWidget *widget, gpointer useless)
{
	gchar *save_this;
	if (file_changed) {
		GtkWidget *dialog;
		gchar *question;
		gint reply;
		question=g_strdup_printf(_("File %s\nwas changed. Do you want to revert to saved copy?"),
			i_love_this_file);
		dialog=gnome_message_box_new(question,
			GNOME_MESSAGE_BOX_QUESTION,
			GNOME_STOCK_BUTTON_YES, GNOME_STOCK_BUTTON_NO,
			GNOME_STOCK_BUTTON_CANCEL, NULL);
		show_nice_dialog(&dialog,"gtranslator -- revert");
		reply=gnome_dialog_run(GNOME_DIALOG(dialog));
		g_free(question);
		if (reply!=GNOME_YES) return;
	}
	save_this=g_strdup(i_love_this_file);
	/* Let close_file know it doesn't matter if file was changed */
	file_changed=FALSE;
	close_file(NULL,NULL);
	parse(save_this);
	g_free(save_this);
}

/**
* The compile function
* TODO: should 'msgfmt -v -c POFILE' and show errors found in file
**/
void compile(GtkWidget *widget,gpointer useless)
{
	/**
	* Simply compile the .po-file
	**/
	gchar *cmd;
	gint res=1;
	/**
	* If a filename has been set already, we can try to compile it.
	**/
	if((i_love_this_file!=NULL))
	{
		cmd=g_strdup_printf("msgfmt -v -c -o /dev/null %s",
			i_love_this_file);
		res=system(cmd);
		/**
		* If there has been an error show an error-box
		**/
		if(res!=0)
		{
			/**
			* Should be shown if there is a non-zero return value
			*  from msgfmt FILE > gtr_test.tmp 2>&1
			**/
			gtk_widget_show_all(gnome_app_error(GNOME_APP(app1),
			_("An error occured while msgfmt was executed.\nPlease check your .po file again.")));
		}
		else
		{
			gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Po-file has been compiled successfully."));
		}
		free(cmd);
	}
}

