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

#include <libgtranslator/team-handle.h>

/**
* This one inits a new language team.
**/
void team_handle_new(gchar *team_code)
{
	/**
	* The local variables.
	**/
	xmlDocPtr teamlist;
	xmlNodePtr teams;
	if(!team_code)
	{
		g_error(_("No language/team code defined to register!"));
	}
	/**
	* Parse the default teamsfile.
	**/
	teamlist=xmlParseFile(MESSAGE_DB_DIR"/teams.xml");
	/**
	* If the document is empty.
	**/
	if(teamlist==NULL)
	{
		/**
		* Create the new root-node.
		**/
		xmlNodePtr root,tmp;
		root=xmlNewDocNode(teamlist,NULL,"teams",NULL);
		tmp=xmlDocSetRootElement(teamlist,root);
		xmlAddChild(root,tmp);
		/**
		* Save the file now.
		**/
		xmlSaveFile(MESSAGE_DB_DIR"/teams.xml",teamlist);
		/**
		* And reopen it.
		**/
		team_handle_new(team_code);
	}
	/**
	* Get the elements.
	**/
	teams=teamlist->xmlRootNode->xmlChildrenNode;
	/**
	* Look if the team is already there.
	**/
	if(lookup_in_doc(teamlist,team_code))
	{
		g_warning(_("Team is already registered!"));
	}
	else
	{
		/**
		* Add a new node with the team_code.
		**/
		xmlNodePtr newnode;
		xmlAttrPtr attribute;
		/**
		* Set a new <team> node ..
		**/
		newnode=xmlNewDocNode(teamlist,NULL,"team",NULL);
		/**
		* The content is the team_code ...
		**/
		attribute=xmlSetProp(newnode,"name",team_code);
		/**
		* Add the node to the document.
		**/
		xmlAddChild(teams,newnode);
		/**
		* Save the file.
		**/
		xmlSaveFile(MESSAGE_DB_DIR"/teams.xml",teamlist);
		/**
		* Free the node pointer.
		**/
		if(newnode)
		{
			xmlFreeNode(newnode);
		}
	}
}

/**
* Returns a GList of all translation-app entries for the specified language.
**/
GList *team_handle_get_all_translations_for_team(gchar *teamname)
{
	GList *list;
	xmlDocPtr teams;
	xmlNodePtr team;
	/**
	* Parse the "teams.xml" file.
	**/
	teams=xmlParseFile(MESSAGE_DB_DIR"/teams.xml");
	/**
	* Print an error message if the file couldn't be opened.
	**/
	if (!teams)
	{
		g_warning(_("Teams file `teams.xml` in %s couldn't be opened!"),MESSAGE_DB_DIR);
	}
	/**
	* Get the elements.
	**/
	team=teams->xmlRootNode->xmlChildrenNode;
	/**
	* Set the list empty.
	**/
	list=NULL;
	/**
	* Search for all the apps.
	**/
	while (team)
	{
		if(!strcmp(team->name, "app"))
		{
			/**
			* Append the app to the list.
			**/
			list=g_list_append(list,(gpointer) g_strdup(xmlGetProp(team, "name")));
		}
		/**
		* Iterate the node.
		**/
		team=team->next;
	}
	/**
	* Check for the list and return it.
	**/
	if (!list)
	{
		g_error(_("Couldn't return the list of the apps."));
	}
	return list;
}

gint lookup_in_doc(xmlDocPtr doc,gchar *req)
{
	xmlNodePtr node;
	/**
	* Get the elements.
	**/
	node=doc->xmlRootNode->xmlChildrenNode;
	while (node)
	{
		/**
		* If the requested node is a tag and there.
		**/
		if(!strcmp(node->name, "team"))
		{
			if(!strcmp(xmlGetProp(node, "name"), req))
			{
				return 1;
			}
		}
		/**
		* Iterate the node.
		**/
		node=node->next;
	}
	/**
	* And if it's not found return '0' at the end.
	**/
	return 0;
}
