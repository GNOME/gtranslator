/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gmx.net>
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

#include <sys/param.h>
#include <libgtranslator/libgtranslator.h>

/*
 * Returns the list of all translated applications for the given node pointer.
 */
GList *gtranslator_get_apps_for_node(xmlNodePtr ptr);

/*
 * Initializes the default team database.
 */
GtranslatorTeamDatabase *gtranslator_init_team_db(void)
{
	/*
	 * Some local variables.
	 */
	xmlDocPtr		doc;
	xmlNodePtr		node;
	GtranslatorTeamDatabase *db=g_new(GtranslatorTeamDatabase,1);
	/*
	 * Open up the default team database file.
	 */
	doc=xmlParseFile("/teams.xml");
	
	if(!doc)
	{
		/*
		 * Hm, without the default team database, all
		 *  the stuff won't work so we do exit here.
		 */
		g_warning(
		_("Couldn't open the team database file `%s'!"),
			"/teams.xml");
		return NULL;
	}
	
	node=doc->xmlRootNode->xmlChildrenNode;
	
	/*
	 * Set up the serial number & the date of the team database.
	 */
	db->serial=(gint) xmlGetProp(node, "serial");
	db->date=xmlGetProp(node, "date");
	
	/*
	 * Cruise through the xml file and get all the relevant
	 *  informations.
	 **/
	while(node)
	{
		if(!strcmp(node->name, "team"))
		{
			GtranslatorTeam	*team=g_new(GtranslatorTeam,1);
			
			/*
			 * Append the found team to the list.
			 */
			team->name=xmlGetProp(node, "name");
			team->mailing_list=xmlGetProp(node, "email");
		
			/*
			 * Get all the translated applications for the
			 *  language.
			 */
			team->apps=gtranslator_get_apps_for_node(
				node->xmlChildrenNode);
			
			/*
			 * Finally append the team to the list.
			 */
			db->teams=g_list_append(db->teams,
				(gpointer) team);
		}
		node=node->next;
	}
	return db;
}

/*
 * An internally used function.
 */
GList *gtranslator_get_apps_for_node(xmlNodePtr ptr)
{
	GList *apps=g_list_alloc();
	/*
	 * Find all corresponding applications for
	 *  the given node pointer.
	 */
	while(ptr)
	{
		/*
		 * Yeah, we're getting all the "app"
		 *  tags for the language.
		 */
		if(!strcmp(ptr->name, "app"))
		{
			GtranslatorTeamApp *app=g_new(GtranslatorTeamApp,1);
			
			/*
			 * Set up the application name and version.
			 */
			app->appname=xmlGetProp(ptr, "name");
			app->appversion=xmlGetProp(ptr, "version");
			
			/*
			 * Also set up the translators.
			 */
			if(ptr->xmlChildrenNode)
			{
				xmlNodePtr translator_node;
				
				translator_node=ptr->xmlChildrenNode;
				/*
				 * Cruise through the translator nodes
				 *  of the application.
				 */
				while(translator_node)
				{
					GtranslatorTeamAppTranslator *translator=g_new0(
	                                	GtranslatorTeamAppTranslator,1);
					
					/*
					 * This is the current translator -- he/she
					 *  is the first in the list of translators.
					 */
					translator->name=xmlGetProp(translator_node,
						"name");
					translator->email=xmlGetProp(translator_node,
						"email");
					
					/*
					 * Append the translator to the translators' list.
					 */
					app->translators=g_list_append(app->translators,
						(gpointer) translator);
					
					translator_node=translator_node->next;
				}
			}
			/*
			 * Append the application to the list.
			 */
			apps=g_list_append(apps, (gpointer) app);
		}
		ptr=ptr->next;
	}
	return apps;
}
