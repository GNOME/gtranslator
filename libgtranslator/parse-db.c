/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gmx.net>
 *			Gediminas Paulauskas <menesis@delfi.lt>
 * 
 * libgtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 * libgtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <libgtranslator/parse-db.h>
#include <libgtranslator/translation-database.h>
#include <libgtranslator/messages.h>

/*
 * Loads the given language's database and returns the created
 *  language database.
 */
GtranslatorDatabase *parse_db_for_lang(gchar *language)
{
	gchar 			*file;
	gboolean 		lusp=FALSE;
	xmlNodePtr 		node=NULL;
	xmlDocPtr 		xmldoc;
	GtranslatorDatabase	*db;
	GList			*messages = NULL;
	
	if(!language)
	{
		g_warning(_("No language defined in the query request!"));
	}
	
	/*
	 * If the requested language is coded in a long sequence (e.g. "tr_TR").
	 */
	if(strchr(language, '_'))
	{
		/*
		 * If a subclass message/translation database should be loaded,
		 *  we'd also look for the superclass message database.
		 */
		g_print(_("FYI: Will also lookup the superclass message db.\n"));
		lusp=TRUE;
	}
	
	file=g_strdup_printf("%s/%s.xml",MESSAGE_DB_DIR,language);
 	db = g_new(GtranslatorDatabase, 1);
	db->header = g_new(GtranslatorDatabaseHeader, 1);
	
	/*
	 * Show some informations to the user.
	 */
	GTR_DB_FILENAME(db)=file;
	g_print(_("Using %s as the message database... \n"), file);
	
	xmldoc=xmlParseFile(file);
	/*
	 * Check the resulting xml document.
	 */
	if(xmldoc==NULL)
	{
		if(lusp==FALSE)
		{
			g_error(_("Couldn't open language base file `%s.xml' in %s.\n"),
				language, MESSAGE_DB_DIR);
		}
		else
		{
			gchar **sarr;
			/*
			 * Split up the language code into it's parts.
			 */
			sarr=g_strsplit(language,"_",1);
	
			/*
			 * Print out some information.
			 */
			g_print(_("Subclass file `%s.xml' not found.\n"),language);
			g_print(_("Trying superclass-file `%s.xml' ...\n"),sarr[0]);
	
			/*
			 * Get the language's name itself.
			 */
			GTR_DB_LANG(db)=sarr[0];

			/*
			 * Recursively call this function again for the superclass
			 *  message database.
			 */
			parse_db_for_lang(sarr[0]);
	
			g_strfreev(sarr);
		}
	}
	node=xmldoc->xmlRootNode->xmlChildrenNode;
	/*
	 * Get the database author's name.
	 */
	GTR_DB_TRANSLATOR(db)=g_strdup(
		xmlGetProp(xmldoc->xmlRootNode, "author"));
	GTR_DB_TRANSLATOR_EMAIL(db)=g_strdup(
		xmlGetProp(xmldoc->xmlRootNode, "email"));

	/*
	 * Again print out some informations.
	 */
	g_print(_("Database creator/administrator: %s\n"), GTR_DB_TRANSLATOR(db));
	g_print(_("EMail: %s\n"), GTR_DB_TRANSLATOR_EMAIL(db));
	
	while(node!=NULL)
	{
                GtrMsg *msg=g_new0(GtrMsg,1);
		/*
		 * Set up the serial information of the database.
		 */
		if(!strcmp(node->name, "serial"))
		{
			/*
			 * Print out the general informations about the
			 *  message database.
			 */
			g_print(_("Message database informations:\n"));
			g_print(_("Date: %s\nSerial: %s\n"),
				xmlGetProp(node, "date"),
				xmlNodeGetContent(node));
			/*
			 * Set up the serial information of the message
			 *  database.
			 */
			GTR_DB_SERIAL(db)=(gint)xmlNodeGetContent(node);
		}
		if(!strcmp(node->name, "msgid"))
		{
			xmlNodePtr newnode;
			newnode=node->xmlChildrenNode;
			if(newnode)
			{
				/*
				 * Get the GtrMsg parts out of the database.
				 */
				msg->msgid=xmlGetProp(node, "name");
				msg->msgstr=xmlNodeGetContent(newnode);
	
				/*
				 * Append the GtrMsg to the list.
				 */
				messages=g_list_prepend(messages,
					(gpointer) msg);
	
				xmlFreeNode(newnode);
			}
		}
		node=node->next;
	}
	/*
	 * Reverse the list as we did _pre_pend all the items
	 *  till now.
	 */
	messages=g_list_reverse(messages);
	
	/*
	 * Set up the database's messages list.
	 */
	GTR_DB_LIST(db)=messages;
	
	return db;
}
