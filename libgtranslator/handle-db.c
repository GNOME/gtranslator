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
* Two simple macros.
**/
#define FREE_NODE(x); if(x!=NULL) { xmlFreeNode(x); }
#define FREE_PROP(x); if(x!=NULL) { xmlFreeProp(x); }

/**
* Adds a node to the doc.
**/
gint add_node(xmlDocPtr doc,gchar *nodename,gchar *nodecontent)
{
	/**
	* Check if a nodename has been defined.
	**/
	if(!nodename)
	{
		g_warning(_("No node name given!"));
			return 0;
	}
	/**
	* Check for the content.
	**/
	if(!nodecontent)
	{
		g_warning(_("No content for the node `%s' given! Leaving node empty.."),nodename);
		nodecontent=" ";	
	}
	/**
	* Check the doc.
	**/
	if(!doc)
	{
		g_warning(_("No document present to add the node `%s'\n"),nodename);
			return 0;
	}
	else
	{
		/**
		* Hmm, open and add the node.
		**/
		xmlNodePtr newnode, addnode=NULL;
		xmlAttrPtr attr;
		/**
		* Get the nodes.
		**/
		newnode=doc->xmlRootNode->xmlChildrenNode;
		if(!newnode)
		{
			g_warning(_("Couldn't get the nodes!"));
				return 0;
		}
		attr=xmlSetProp(addnode, nodename, nodecontent);
		addnode=xmlAddChild(newnode,addnode);
		/**
		* Free all the stuff.
		**/
		FREE_NODE(addnode);
		FREE_NODE(newnode);
		FREE_PROP(attr);
		/**
		* Return a value != 0.
    		**/
		return 1;
	}	
}
