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

#include "parse-db.h"

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
	db_list=NULL;
	/**
	* Get the nodes.
	**/
	node=xmldoc->xmlRootNode->xmlChildrenNode;
	/**
	* Print the informations about the message database.
	**/
	g_print(_("Database creator/administrator: %s\n"),xmlGetProp(xmldoc->xmlRootNode, "author"));
	g_print(_("EMail: %s\n"),xmlGetProp(xmldoc->xmlRootNode, "email"));
	while(node!=NULL)
	{
		/**
		* Get the serial.
		**/
		if(!strcmp(node->name, "serial"))
		{
			g_print(_("Message database informations:\n"));
			g_print(_("Date: %s\nSerial: %s\n"), xmlGetProp(node, "date"), xmlNodeGetContent(node));
			
		}
		if(!strcmp(node->name, "msgid"))
		{
			xmlNodePtr newnode;
			newnode=node->xmlChildrenNode;
			if(newnode)
			{
				g_print("Node: %s - %s\n",xmlGetProp(node, "name"),xmlNodeGetContent(newnode));
				/**
				* Free the node.
				**/
				xmlFreeNode(newnode);
			}	
		}
		node=node->next;
	}
}

void parse_db_check(xmlDocPtr test)
{
	xmlNodePtr muhaha;
	/**
	* Get the root element.
	**/
	muhaha=xmlDocGetRootElement(test);
	/**
	* Check it.
	**/
	if(!muhaha)
	{
		xmlFreeDoc(test);
		g_error(_("The message db file is empty!"));
	}
	else
	{
		if(strcmp(muhaha->name, "db"))
		{
			g_error(_("The message db doesn't seem to be a message db xml file."));
		}
	}
	/**
	* Free it if it's still present.
	**/
	xmlFreeNode(muhaha);
}
