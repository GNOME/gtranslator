/**
*
* (C) 2000 Fatih Demir -- kabalak / kabalak@gmx.net
*
* This is distributed under the GNU GPL V 2.0 or higher which can be
*  found in the file COPYING for further studies.
*
* Enjoy this piece of software, brain-crack and other nice things.
*
* WARNING: Trying to decode the source-code may be hazardous for all your
*	future development in direction to better IQ-Test rankings!
*
* PSC: This has been completely written with vim; the best editor of all.
*
**/

#include <libgtranslator/parse-db.h>
#include <libgtranslator/translation-database.h>
#include <libgtranslator/messages.h>

/**
* Parses the lang.xml file.
**/
void parse_db_for_lang(gchar *language)
{
	gchar file[256];
	gboolean lusp=FALSE;
	xmlNodePtr node=NULL;
	xmlDocPtr xmldoc;
	/**
	* Initialize the list.
	**/
	messages=NULL;
	/**
	* Check if we did get a language to search for ..
	**/
	if(!language)
	{
		g_warning(_("No language defined in the query request!"));
	}
	/**
	* Check if we got a 'tr_TR' alike language request ..
	**/
	if(strchr(language, '_'))
	{
		/**
		* If a 'tr_TR' isn't found then we go to the superclass db: 'tr'.
		* Just show a message that tells this.
		**/
		g_print(_("FYI: Will also lookup the superclass message db.\n"));
		lusp=TRUE;
	}
	sprintf(file,"%s/%s.xml",MESSAGE_DB_DIR,language);
	/**
	* Set the filename of the DB.
	**/
	/*strcpy(db->filename, file);*/
	db->filename="ter";
	g_print(_("Using %s as the message database... \n"),file);
	/**
	* Parse the xml file.
	**/
	xmldoc=xmlParseFile(file);
	/**
	* If there's no such file print an error .
	**/
	if(xmldoc==NULL)
	{
		if(lusp==FALSE)
		{
			g_error(_("Couldn't open language base file `%s.xml' in %s.\n"),language,MESSAGE_DB_DIR);
		}
		else
		{
			/**
			* A string array and get the language name into the array's
			*  first element.
			**/
			gchar **sarr;
			sarr=g_strsplit(language,"_",1);
			/**
			* Print some information.
			**/
			g_print(_("Subclass file `%s.xml' not found.\n"),language);
			g_print(_("Trying superclass-file `%s.xml' ...\n"),sarr[0]);
			/**
			* Set the language name.
			**/
			db->header->language=sarr[0];
			/**
			* Recurse within the same function with the new language  word ..
			**/
			parse_db_for_lang(sarr[0]);
			/**
			* Free the string array.
			**/
			if(sarr)
			{
				g_strfreev(sarr);
			}	
		}
	}
	/**
	* Get the nodes.
	**/
	node=xmldoc->xmlRootNode->xmlChildrenNode;
	/**
	* Print the informations about the message database.
	**/
	g_print(_("Database creator/administrator: %s\n"),
		xmlGetProp(xmldoc->xmlRootNode, "author"));
	/**
	* Set the author name.
	**/
	db->header->author=xmlGetProp(xmldoc->xmlRootNode, "author");
	g_print(_("EMail: %s\n"),xmlGetProp(xmldoc->xmlRootNode, "email"));
	/**
	* And the author email for the DB.
	**/
	db->header->author_email=xmlGetProp(xmldoc->xmlRootNode, "email");
	while(node!=NULL)
	{
                GtrMsg *msg=g_new0(GtrMsg,1);
		/**
		* Get the serial.
		**/
		if(!strcmp(node->name, "serial"))
		{
			g_print(_("Message database informations:\n"));
			g_print(_("Date: %s\nSerial: %s\n"), xmlGetProp(node, "date"),
				xmlNodeGetContent(node));
			/**
			* Set the serial information of the DB.
			**/
			db->header->serial=(gint)xmlNodeGetContent(node);
		}
		if(!strcmp(node->name, "msgid"))
		{
			xmlNodePtr newnode;
			newnode=node->xmlChildrenNode;
			if(newnode)
			{
				/**
				* Get the message entries.
				**/
				msg->msgid=xmlGetProp(node, "name");
				msg->msgstr=xmlNodeGetContent(newnode);
				/**
				* Add them to the list.
				**/
				messages=g_list_prepend(messages,
					(gpointer) msg);
				/**
				* Free the node.
				**/
				xmlFreeNode(newnode);
			}	
		}
		node=node->next;
	}
	/**
	* Now reverse the list.
	**/
	messages=g_list_reverse(messages);
	/**
	* Set the DB's messages list to the current lisr.
	**/
	db->messages=messages;
}

/**
* And save the GList as a message database.
**/
void parse_db_save(GList *list)
{
	/**
	* The coming variables.
	**/
	FILE *file;
	/**
	* Fresh-meat :-)
	**/
	GtrMsg *msg=g_new0(GtrMsg,1);
	gchar *temp=g_new0(gchar,1);
	gchar datestr[10];
	time_t timetick;
	struct tm *timezone;
	/**
	* Make an assertion for the list.
	**/
	g_assert(list!=NULL);
	/**
	* Now combine the old filename and the suffix
	*  to the new filename.
	**/
	sprintf(temp, "%s.new", db->filename);
	/**
	* Open the file to be written.
	**/
	file=fopen(temp, "w");
	/**
	* Again do an assertion, but this time on the file.
	**/
	g_assert(file!=NULL);
	/**
	* Print the file header.
	**/
	fprintf(file,"<?xml version=\"1.0\"?>\n");
	/**
	* Write the header with all the informations.
	**/
	fprintf(file,
		"<db language=\"%s\" author=\"%s\" email=\"%s\">\n",
		db->header->language, db->header->author, db->header->author_email);
	/**
	* Get the date.
	**/
	timetick=time(NULL);
	timezone=localtime(&timetick);
	/**
	* And print it on the line.
	**/
	strftime(datestr, sizeof(datestr), "%Y-%m-%d", timezone);
	/**
	* Also write the serial date and number.
	**/
	fprintf(file,"<serial date=\"%s\">%i</serial>", datestr,
		db->header->serial);
	/**
	* And get every entry from the list.
	**/
	while(list!=NULL)
	{
		/**
		* Get the current data from the list.
		**/
		msg=(gpointer)list->data;
		/**
		* Print out the formatted output of the message entry.
		**/
		fprintf(file,"\n\t<msgid name=\"%s\">\n\t\t<msgstr>%s</msgstr>\n\t</msgid>",
			msg->msgid, msg->msgstr);
		/**
		* Iterate to the next node in the list.
		**/	
		list=list->next;
	}
	/**
	* Print the last tag.
	**/
	fprintf(file,"\n</db>");
	/**
	* Free the used variables.
	**/
	if(msg)
	{
		g_free(msg);
	}
	if(temp)
	{
		g_free(temp);
	}		
	/**
	* Close the file stream if it's still here :-)
	**/
	if(file)
	{
		fclose(file);
	}	
}
