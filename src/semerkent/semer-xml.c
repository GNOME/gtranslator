/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
 *
 * semerkent is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU Library General Public License as published by the
 *   Free Software Foundation; either version 2 of the License, or (at your
 *    option) any later version.
 *
 * semerkent is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 *    License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * semerkent is being developed around gtranslator, so that any wishes or own
 *  requests should be mailed to me, as I will naturally keep semerkent very
 *   close to gtranslator. This shouldn't avoid any use of semerkent from other
 *    applications/packages.
 *
 * The homepage for semerkent is: http://semerkent.sourceforge.net
 */

#include "semer-file.h"
#include "semer-utils.h"
#include "semer-xml.h"

G_BEGIN_DECLS

/*
 * Get the given prop -- return NULL in bad case.
 */
gchar *semer_xml_get_prop(xmlNodePtr node, gchar *prop_name)
{
	gchar *property;

	g_return_val_if_fail(node!=NULL, NULL);
	g_return_val_if_fail(prop_name!=NULL, NULL);

	property=xmlGetProp(node, prop_name);
	
	if(!property)
	{
		return NULL;
	}
	
	return g_strdup(property);
	semer_utils_free(property);
}

/*
 * Get the content of a tag.
 */
gchar *semer_xml_get_content(xmlNodePtr node)
{
	gchar *content;
	
	g_return_val_if_fail(node!=NULL, NULL);
	
	content=xmlNodeListGetString(node->doc, node->xmlChildrenNode, 1);

	if(!content)
	{
		return NULL;
	}

	return g_strdup(content);
	semer_utils_free(content);
}

/*
 * Test for our beloved "doctype".
 */
gboolean semer_xml_doctype_cmp(xmlDocPtr doc, gchar *doctypename)
{
	xmlNodePtr node;
	
	g_return_val_if_fail(doc!=NULL, FALSE);
	g_return_val_if_fail(doctypename!=NULL, FALSE);

	node=xmlDocGetRootElement(doc);

	if(!node)
	{
		return FALSE;
	}
	else if(g_ascii_strcasecmp(node->name, doctypename))
	{
		return FALSE;
	}
	else if(!node->xmlChildrenNode)
	{
		return FALSE;
	}

	xmlFreeNode(node);

	return TRUE;
}

/*
 * eel functions adapted to Semerkent (original author: Darin Adler).
 */
xmlNodePtr semer_xml_get_by_name(xmlNodePtr parent, const gchar *name)
{
	xmlNodePtr node;
	
	g_return_val_if_fail(parent!=NULL, NULL);
	g_return_val_if_fail(name!=NULL, NULL);

	for(node=parent->xmlChildrenNode; node; node=node->next)
	{
		if(!xmlStrcmp(node->name, name))
		{
			return node;
		}
	}

	return NULL;
}

/*
 * Removes the ptr-to-node node from the document.
 */
void semer_xml_remove_node(xmlNodePtr node)
{
	g_return_if_fail(node!=NULL);
	g_return_if_fail(node->doc!=NULL);
	g_return_if_fail(node->parent!=NULL);
	g_return_if_fail(node->doc->xmlRootNode!=node);

	if(!node->prev)
	{
		node->parent->xmlChildrenNode=node->next;
	}
	else
	{
		node->prev->next=node->next;
	}

	if(!node->next)
	{
		node->parent->last=node->prev;
	}
	else
	{
		node->next->prev=node->prev;
	}

	node->doc=NULL;
	node->parent=NULL;
	node->next=NULL;
	node->prev=NULL;
}

/*
 * Set up the header/root node.
 */
void semer_xml_set_root_node(gpointer file, xmlDocPtr doc)
{
	xmlNodePtr node;
	
	gchar	*name=g_strdup("unknown");
	gchar	*dtd=NULL;
	gchar	*version=g_strdup("unknown");

	g_return_if_fail(file!=NULL);
	g_return_if_fail(doc!=NULL);

	/*
	 * Corresponding to the filetypes we're getting the
	 *  informations here.
	 */
	switch(SEMER_FILE(file)->type)
	{
		case SEMER_TMX:
			semer_utils_free(name);
			semer_utils_free(version);

			name=g_strdup("tmx");
			version=g_strdup("1.1");
			dtd=g_strdup("tmx11.dtd");
				break;

		case SEMER_OPENTAG:
			semer_utils_free(name);
			semer_utils_free(version);

			name=g_strdup("opentag");
			version=g_strdup("1.2");
				break;
		
		case SEMER_UMTF:
			semer_utils_free(name);
			semer_utils_free(version);

			name=g_strdup("umtf");
			version=g_strdup("0.6");
				break;

		case SEMER_XLIFF:
			semer_utils_free(name);
			semer_utils_free(version);

			name=g_strdup("xliff");
			version=g_strdup("1.0");
				break;
				
		default:
			break;
	}
	
	/*
	 * Set up the main document tag ("root node").
	 */
	node=xmlNewDocNode(doc, NULL, name, NULL);
	xmlDocSetRootElement(doc, node);

	/*
	 * Set DTD and version informations/props if possible.
	 */
	if(dtd)
	{
		xmlNewDtd(doc, name, NULL, dtd);
	}

	if(version)
	{
		xmlSetProp(node, "version", version);
	}

	/*
	 * Free the used data.
	 */
	semer_utils_free(name);
	semer_utils_free(version);
	semer_utils_free(dtd);
}

G_END_DECLS
