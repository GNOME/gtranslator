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
* The internally used function which gets all apps etc. for
*  the current node in the team database.
**/
GList *gtranslator_get_apps_for_node(xmlNodePtr ptr);

/**
* Open the given file or the default team database.
**/
GtranslatorTeamDatabase *gtranslator_open_team_db(gchar *team_db_file)
{
	/**
	* Some local variables.
	**/
	gchar			*filename=g_new0(gchar,1);
	xmlDocPtr		doc;
	xmlNodePtr		node;
	GtranslatorTeamDatabase *db=g_new(GtranslatorTeamDatabase,1);
	/**
	* Test if we've got a different team database
	*  is given.
	**/
	if(!team_db_file)
	{
		/**
		* Then build-up the default team database
		*  location.
		**/
		sprintf(filename, "%s/%s", MESSAGE_DB_DIR,
			"teams.xml");
	}
	else
	{
		/**
		* Or check if the given team database filename
		*  is an absolute filename.
		**/
		if(!g_path_is_absolute(team_db_file))
		{
			/**
			* If not, set up a "absolute-like"
			*  filename.
			**/
			sprintf(filename, "%s/%s",
				g_get_current_dir(),
				team_db_file);
		}		
	}
	/**
	* Now open up the team database file with libxml.
	**/
	doc=xmlParseFile(filename);
	/**
	* Check if the xml document could be opened.
	**/
	if(!doc)
	{
		/**
		* This is definitely not Ok... So we do
		*  exit now...
		**/
		g_warning(
		_("Couldn't open the team database file `%s'!"),
			filename);
		return NULL;	
	}
	/**
	* Open the nodes.
	**/
	node=doc->xmlRootNode->xmlChildrenNode;
	/**
	* Did we get any entries?
	**/
	if(!node)
	{
		/**
		* Print out a nice warning message and exit
		*  from here.
		**/
		g_warning(
		_("The team database file `%s' doesn't contain any entries!"),
			filename);
		return NULL;
	}
	/**
	* Now we can get the serial for the GtranslatorTeamDatabase.
	**/
	db->serial=(gint) xmlGetProp(node, "serial");
	/**
	* And the date for the team databse file.
	**/
	db->date=xmlGetProp(node, "date");
	/**
	* So, we are right here now, use our loved functions
	*  from DV-lib :-)
	**/
	while(node)
	{
		/**
		* Are we now at a <team> tag?
		**/
		if(!strcmp(node->name, "team"))
		{
			/**
			* Create a new GtranslatorTeam structure.
			**/
			GtranslatorTeam	*team=g_new(GtranslatorTeam,1);
			/**
			* Add this team with it's specs to the list.
			**/
			team->name=xmlGetProp(node, "name");
			team->mailing_list=xmlGetProp(node, "email");
			/**
			* Yeah, now get the apps for the team.
			**/
			team->apps=gtranslator_get_apps_for_node(
				node->xmlChildrenNode);
			/**
			* And now append this team to the list.
			**/
			db->teams=g_list_append(db->teams, (gpointer) team);
		}
		/**
		* Iterate the node.
		**/
		node=node->next;
	}
	/**
	* Return the GtranslatorTeamDatabase.
	**/
	return db;
}

/**
* A helper function for the internal use.
**/
GList *gtranslator_get_apps_for_node(xmlNodePtr ptr)
{
	/**
	* Create a new list.
	**/
	GList *apps=g_list_alloc();
	/**
	* Now get the apps.
	**/
	while(ptr)
	{
		/**
		* Test if this is an app tag.
		**/
		if(!strcmp(ptr->name, "app"))
		{
			/**
			* Create a new GtranslatorTeamApp.
			**/
			GtranslatorTeamApp *app=g_new(GtranslatorTeamApp,1);
			/**
			* Get the name and the version.
			**/
			app->appname=xmlGetProp(ptr, "name");
			app->appversion=xmlGetProp(ptr, "version");
			/**
			* Now do get the translators.
			**/
			if(ptr->xmlChildrenNode)
			{
				/**
				* The translator node.
				**/
				xmlNodePtr translator_node;
				/**
				* Assign the node.
				**/
				translator_node=ptr->xmlChildrenNode;
				/**
				* Cruise through our translators.
				**/
				while(translator_node)
				{
					/**
	                                * The GtranslatorTeamAppTranslator.
	                                **/
					GtranslatorTeamAppTranslator *translator=g_new0(
	                                	GtranslatorTeamAppTranslator,1);
					/**
					* Get the current ones information.
					**/
					translator->name=xmlGetProp(translator_node,
						"name");
					translator->email=xmlGetProp(translator_node,
						"email");
					/**
					* Now add the translator to the translators' list.
					**/
					app->translators=g_list_append(app->translators,
						(gpointer) translator);
					/**
					* Go to the next translator.
					**/
					translator_node=translator_node->next;
				}		
			}
			/**
			* Add this to the apps' list.
			**/
			apps=g_list_append(apps, (gpointer) app);
		}
		/**
		* Go to the next app.
		**/
		ptr=ptr->next;
	}	
	/**
	* Return the apps' list.
	**/
	return apps;
}
