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

#include "semer-opentag.h"
#include "semer-utils.h"

G_BEGIN_DECLS

/*
 * Internally used functions for writing/parsing.
 */
static void semer_opentag_write_entry(SemerEntry *entry, xmlDocPtr doc);
static void semer_opentag_write_header(SemerFile *file, xmlDocPtr doc);

/*
 * Writes a single SemerEntry into the xml document.
 */
static void semer_opentag_write_entry(SemerEntry *entry, xmlDocPtr doc)
{
	xmlNodePtr node, add_node;
	xmlNodePtr original_node, translation_node;
	
	g_return_if_fail(SEMER_ENTRY(entry)!=NULL);
	g_return_if_fail(doc!=NULL);

	node=doc->xmlRootNode->xmlChildrenNode;

	/*
	 * Search for the main <file> node of the OpenTag document.
	 */
	while(node && g_ascii_strcasecmp(node->name, "file"))
	{
		node=node->next;
	}
	
	/*
	 * Add the <grp> tag for a translation group entry.
	 */
	add_node=xmlNewChild(node, NULL, "grp", NULL);

	/*
	 * Add eventual existing props informations to the translation group
	 *  attributes.
	 */
	if(SEMER_PROPS(entry->props)->id)
	{
		xmlSetProp(add_node, "id", SEMER_PROPS(entry->props)->id);
	}

	if(SEMER_PROPS(entry->props)->domain)
	{
		xmlSetProp(add_node, "ts", SEMER_PROPS(entry->props)->domain);
	}

	/*
	 * Add the original informations node.
	 */
	original_node=xmlNewChild(add_node, NULL, "p", entry->original);
	xmlSetProp(original_node, "lc", entry->original_language);

	/*
	 * Add the translation informations node.
	 */
	translation_node=xmlNewChild(add_node, NULL, "p", entry->translation);
	xmlSetProp(translation_node, "lc", entry->translation_language);
}

/*
 * Write the header node for the OpenTag document "<file>".
 */
static void semer_opentag_write_header(SemerFile *file, xmlDocPtr doc)
{
	xmlNodePtr 	main_file_node;
	gchar		*tool_string;
	
	g_return_if_fail(SEMER_FILE(file)->header!=NULL);
	g_return_if_fail(doc!=NULL);

	/*
	 * Create the main document node.
	 */
	main_file_node=xmlNewChild(doc->xmlRootNode, NULL,
		"file", NULL);
	g_return_if_fail(main_file_node!=NULL);

	/*
	 * Create tool name+version string for the document.
	 */
	tool_string=g_strdup_printf("%s:%s", file->header->tool,
		file->header->tool_version);

	/*
	 * Set it's properties.
	 */
	xmlSetProp(main_file_node, "tool", tool_string); 
	xmlSetProp(main_file_node, "lc", file->header->adminlang);
	xmlSetProp(main_file_node, "datatype", file->header->datatype);

	semer_utils_free(tool_string);
}

/*
 * Parse the opentag file into the SemerFile representation.
 */
SemerFile *semer_opentag_parse_file(const gchar *filename)
{
	SemerFile *file=g_new0(SemerFile, 1);
	
	g_return_val_if_fail(file!=NULL, NULL);
	
	if(!semer_utils_test_file(filename))
	{
		return NULL;
	}
	else
	{
		xmlDocPtr 	doc;
		xmlNodePtr 	node;
		
		doc=xmlParseFile(filename);
		g_return_val_if_fail(doc!=NULL, NULL);
		
		if(!semer_xml_doctype_cmp(doc, "opentag"))
		{
			xmlFreeDoc(doc);
			return NULL;
		}
		
		node=doc->xmlRootNode->xmlChildrenNode;
		
		file->type=SEMER_OPENTAG;
		file->filename=g_strdup(filename);
		
		file->header=NULL;
		
		while(node && file->entries)
		{
			node=node->next;
		}
		
		return file;
	}
}

/*
 * Save the given SemerFile.
 */
void semer_opentag_save_file(SemerFile *file)
{
	xmlDocPtr opentag;
	
	g_return_if_fail(SEMER_FILE(file)!=NULL);
	g_return_if_fail(SEMER_FILE(file)->filename!=NULL);
	
	/*
	 * Create the root document, node and header node (aka. "<file>").
	 */
	opentag=xmlNewDoc("1.0");
	semer_xml_set_root_node(file, opentag);
	semer_opentag_write_header(file, opentag);

	/*
	 * Write all entries using our function .-)
	 */
	semer_file_write_all_entries(file, opentag,
		(SemerEntryWriteFunction) semer_opentag_write_entry);
	
	xmlSaveFormatFile(file->filename, opentag, 2);
	xmlFreeDoc(opentag);
}

G_END_DECLS
