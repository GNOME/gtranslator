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

#include "handle-db.h"

/**
* Adds a node to the given doc.
**/
gint gtranslator_add_node_to_doc(xmlDocPtr doc,gchar *nodename,gchar *nodecontent)
{
	/**
	* Check if a nodename has been defined.
	**/
	if(!nodename)
	{
		g_warning(_("No node name given!"));
			return 1;
	}
	/**
	* Check for the content.
	**/
	if(!nodecontent)
	{
		g_warning(_("No content for the node `%s' given! Leaving node empty.."),nodename);
		nodecontent="";	
	}
	/**
	* Check the doc.
	**/
	if(!doc)
	{
		g_warning(_("No document present to add the node `%s'\n"),nodename);
			return 1;
	}
	else
	{
		/**
		* Hmm, open and add the node.
		**/
		xmlNodePtr newnode, addnode=NULL;
		/**
		* Get the nodes.
		**/
		newnode=doc->xmlRootNode->xmlChildrenNode;
		/**
		* Again a simply check.
		**/
		if(!newnode)
		{
			g_warning(_("Couldn't get the nodes!"));
				/**
				* This is an I/O error, isn't it ?
				**/
				return 1;
		}
		/**
		* Set the node properties with the given parameters.
		**/
		addnode=xmlNewDocNode(doc, NULL, nodename, nodecontent);
		/**
		* Check if the node could be created.
		**/
		if(!addnode)
		{
			g_warning(_("Couldn't generate node `%s'!"), nodename);
				return 1;
		}
		xmlAddChild(newnode,addnode);
		/**
		* Free all the stuff.
		**/
		if(newnode)
		{
			xmlFreeNode(newnode);
		}
		if(addnode)
		{	
			xmlFreeNode(addnode);
		}	
		/**
		* Return 0
    		**/
		return 0;
	}	
}
