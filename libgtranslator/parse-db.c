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
* Parses the lang.xml file into a GtranslatorDatabase.
**/
GtranslatorDatabase * parse_db_for_lang(gchar *language)
{
	gchar 			*file;
	gboolean 		lusp=FALSE;
	xmlNodePtr 		node=NULL;
	xmlDocPtr 		xmldoc;
	GtranslatorDatabase	*db;
	GList			*messages = NULL;
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
	file=g_strdup_printf("%s/%s.xml",MESSAGE_DB_DIR,language);
 	db = g_new(GtranslatorDatabase, 1);
	db->header = g_new(GtranslatorDatabaseHeader, 1);
	/**
	* Set the filename of the DB.
	**/
	GTR_DB_FILENAME(db)=file;
	/**
	* Print some information to the user.
	**/
	g_print(_("Using %s as the message database... \n"), file);
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
			g_error(_("Couldn't open language base file `%s.xml' in %s.\n"),
				language, MESSAGE_DB_DIR);
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
			GTR_DB_LANG(db)=sarr[0];
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
	* Set the author name.
	**/
	GTR_DB_AUTHOR(db)=g_strdup(
		xmlGetProp(xmldoc->xmlRootNode, "author"));
	/**
	* Print the informations about the message database.
	**/
	g_print(_("Database creator/administrator: %s\n"), GTR_DB_AUTHOR(db));
	/**
	* And the author email for the DB.
	**/
	GTR_DB_AUTHOR_EMAIL(db)=g_strdup(
		xmlGetProp(xmldoc->xmlRootNode, "email"));
	/**
	* Again inform the user about some parts of it.
	**/
	g_print(_("EMail: %s\n"), GTR_DB_AUTHOR_EMAIL(db));
	/**
	* Get the nodes.
	**/
	while(node!=NULL)
	{
                GtrMsg *msg=g_new0(GtrMsg,1);
		/**
		* Get the serial.
		**/
		if(!strcmp(node->name, "serial"))
		{
			/**
			* Print these informations out! We wanna know
			*  the contact persons.
			**/
			g_print(_("Message database informations:\n"));
			g_print(_("Date: %s\nSerial: %s\n"),
				xmlGetProp(node, "date"),
				xmlNodeGetContent(node));
			/**
			* Set the serial information of the DB.
			**/
			GTR_DB_SERIAL(db)=(gint)xmlNodeGetContent(node);
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
	* Set the database messages list to the current list.
	**/
	GTR_DB_LIST(db)=messages;
	/**
	* Return the parsed database.
	**/
	return db;
}
