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

#include "semer-props.h"
#include "semer-tmx.h"
#include "semer-utils.h"

G_BEGIN_DECLS

/*
 * Utility use-functions:
 */
static SemerEntry *semer_tmx_parse_entry_from_node(xmlNodePtr node); 

static void semer_tmx_write_entry(SemerEntry *entry, xmlDocPtr doc);
static void semer_tmx_write_header(SemerFile *file, xmlDocPtr doc);	

/*
 * Get the props for our node.
 */
static SemerProps *semer_tmx_props_from_node(xmlNodePtr node);

/*
 * Return a fresh header grepped from the node.
 */
static SemerHeader *semer_tmx_header_from_node(xmlNodePtr node);

/*
 * Write the entry as a node.
 */
static void semer_tmx_write_entry(SemerEntry *entry, xmlDocPtr doc)
{
	xmlNodePtr node, add_node;
	xmlNodePtr original_node, translation_node;
	xmlNodePtr content_node;
	
	g_return_if_fail(entry!=NULL);
	g_return_if_fail(doc!=NULL);

	/*
	 * Add a new child node for the entry to the main doc node.
	 */
	node=doc->xmlRootNode->xmlChildrenNode;
	
	/*
	 * Iterate till the body tag of the TMX document.
	 */
	while(node && g_ascii_strcasecmp(node->name, "body"))
	{
		node=node->next;
	}
	
	/*
	 * The main <tu> node which holds the original & the translation.
	 */
	add_node=xmlNewChild(node, NULL, "tu", NULL);

	/*
	 * Add all eventual existing properties for the <tu> from the
	 *  SemerEntry.
	 */
	if(SEMER_PROPS(entry->props)->id)
	{
		xmlSetProp(add_node, "tuid",
			SEMER_PROPS(entry->props)->id);
	}

	if(SEMER_PROPS(entry->props)->type)
	{
		xmlSetProp(add_node, "datatype", 
			SEMER_PROPS(entry->props)->type);
	}
	else
	{
		xmlSetProp(add_node, "datatype", "Text");
	}

	if(SEMER_PROPS(entry->props)->changedate)
	{
		xmlSetProp(add_node, "changedate",
			SEMER_PROPS(entry->props)->changedate);
	}

	if(SEMER_PROPS(entry->props)->client)
	{
		xmlNodePtr client_node;

		client_node=xmlNewChild(add_node, NULL, "prop",
			SEMER_PROPS(entry->props)->client);

		xmlSetProp(client_node, "type", "x-Client");
	}

	if(SEMER_PROPS(entry->props)->domain)
	{
		xmlNodePtr domain_node;

		domain_node=xmlNewChild(add_node, NULL, "prop",
			SEMER_PROPS(entry->props)->domain);

		xmlSetProp(domain_node, "type", "x-Domain");
	}
	
	/*
	 * The <tuv> node with the original entries.
	 */
	original_node=xmlNewChild(add_node, NULL, "tuv", NULL);
	xmlSetProp(original_node, "lang", entry->original_language);

	content_node=xmlNewChild(original_node, NULL, "seg", entry->original);
	
	/*
	 * The corresponding <tuv> node for the translated entries.
	 */
	translation_node=xmlNewChild(add_node, NULL, "tuv", NULL);
	xmlSetProp(translation_node, "lang", entry->translation_language);
	
	content_node=xmlNewChild(translation_node, NULL, "seg", 
		entry->translation);
}

/*
 * Write out the file's header to the given doc.
 */
static void semer_tmx_write_header(SemerFile *file, xmlDocPtr doc)
{
	xmlNodePtr header, body;

	g_return_if_fail(file!=NULL);
	g_return_if_fail(doc!=NULL);

	header=xmlNewChild(doc->xmlRootNode, NULL, "header", NULL);
	xmlSetProp(header, "creationtool", file->header->tool);
	xmlSetProp(header, "creationtoolsversion", file->header->tool_version);
	xmlSetProp(header, "datatype", file->header->datatype);
	xmlSetProp(header, "segtype", file->header->segtype);
	xmlSetProp(header, "adminlang", file->header->adminlang);
	xmlSetProp(header, "srclang", file->header->srclang);
	xmlSetProp(header, "creationdate", semer_utils_tmx_date_new());
	xmlSetProp(header, "creationid", file->header->creationid);

	body=xmlNewChild(doc->xmlChildrenNode, NULL, "body", NULL);
}

/*
 * Parse the TMX entries into SemerEntries.
 */
static SemerEntry *semer_tmx_parse_entry_from_node(xmlNodePtr node)
{
	SemerEntry *entry=g_new0(SemerEntry, 1);
	
	g_return_val_if_fail(node!=NULL, NULL);

	entry->props=semer_tmx_props_from_node(node);
	node=node->xmlChildrenNode;

	if(node)
	{
		entry->original_language=semer_xml_get_prop(node, "lang");
		
		node=node->xmlChildrenNode;
		
		entry->original=semer_xml_get_content(node);
		node=node->parent;

		entry->translation_language=semer_xml_get_prop(node, "lang");
		
		node=node->xmlChildrenNode;
		
		entry->translation=semer_xml_get_content(node);
		node=node->parent;
	}
	else
	{
		entry->original=entry->original_language=NULL;
		entry->translation=entry->translation_language=NULL;
	}
	
	return entry;
}

/*
 * Get the properties.
 */
static SemerProps *semer_tmx_props_from_node(xmlNodePtr node)
{
	SemerProps *props=g_new0(SemerProps, 1);
	
	g_return_val_if_fail(node!=NULL, NULL);

	props->id=semer_xml_get_prop(node, "tuid");
	props->type=semer_xml_get_prop(node, "type");
	props->changedate=semer_xml_get_prop(node, "changedate");

	return props;
}

/*
 * Return a fresh header entry grepped from the node (should be toplevel node).
 */
static SemerHeader *semer_tmx_header_from_node(xmlNodePtr node)
{
	SemerHeader *header;
	g_return_val_if_fail(node!=NULL, NULL);
	g_return_val_if_fail(!g_ascii_strcasecmp(node->name, "header"), NULL);

	#define GET_HEADER_DATA(prop, field); \
		if(semer_xml_get_prop(node, prop)) \
		{ \
			gchar *propvalue=semer_xml_get_prop(node, prop); \
			\
			semer_utils_free(header->field); \
			\
			if(propvalue) \
			{ \
				header->field=g_strdup(propvalue); \
				semer_utils_free(propvalue); \
			} \
			else \
			{ \
				header->field=NULL; \
			} \
		}

	header=semer_header_new();

	GET_HEADER_DATA("creationtool", tool);
	GET_HEADER_DATA("creationtoolsversion", tool_version);
	GET_HEADER_DATA("datatype", datatype);
	GET_HEADER_DATA("segtype", segtype);
	GET_HEADER_DATA("adminlang", adminlang);
	GET_HEADER_DATA("srclang", srclang);
	GET_HEADER_DATA("o-tmf", format);
	GET_HEADER_DATA("o-encoding", encoding);
	GET_HEADER_DATA("creationid", creationid);
	GET_HEADER_DATA("creationdate", creationdate);
	#undef GET_HEADER_DATA

	return header;
}

/*
 * Parse the file into the specs.
 */
SemerFile *semer_tmx_parse_file(const gchar *filename)
{
	SemerFile *file=g_new0(SemerFile, 1);
	
	g_return_val_if_fail(filename!=NULL, NULL);

	if(!semer_utils_test_file(filename))
	{
		return NULL;
	}
	else
	{
		xmlDocPtr doc;
		xmlNodePtr node;
	
		doc=xmlParseFile(filename);
		g_return_val_if_fail(doc!=NULL, NULL);

		if(!semer_xml_doctype_cmp(doc, "tmx"))
		{
			return NULL;
		}
		
		node=doc->xmlRootNode->xmlChildrenNode;
	
		file->type=SEMER_TMX;
		file->filename=g_strdup(filename);
		
		while(!g_ascii_strcasecmp(node->name, "header"))
		{
			file->header=semer_tmx_header_from_node(node);
			node=node->next;
		}
	
	
		/*
		 * FIXME: Parse routines.
		 */
		semer_tmx_parse_entry_from_node(NULL);
	}

	return file;
}

/*
 * Save the given file as a TMX translation memory.
 */
void semer_tmx_save_file(SemerFile *file)
{
	xmlDocPtr tmx;
	
	g_return_if_fail(SEMER_FILE(file)!=NULL);
	g_return_if_fail(SEMER_FILE(file)->filename!=NULL);

	/*
	 * Write the main document informations/tags.
	 */
	tmx=xmlNewDoc("1.0");
	
	semer_xml_set_root_node(file, tmx);
	semer_tmx_write_header(file, tmx);

	/*
	 * Write all entries for the file using our new generalizing function.
	 */
	semer_file_write_all_entries(file, tmx,
		(SemerEntryWriteFunction) semer_tmx_write_entry);
	
	xmlSaveFormatFile(file->filename, tmx, 2);
	xmlFreeDoc(tmx);
}

G_END_DECLS
