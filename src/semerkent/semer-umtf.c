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

#include "semer-entry.h"
#include "semer-umtf.h"
#include "semer-xml.h"

G_BEGIN_DECLS

/*
 * Internal writing routines.
 */
static void semer_umtf_write_header(SemerFile *file, xmlDocPtr doc);

static void semer_umtf_write_entry(SemerEntry *entry, xmlDocPtr doc);

/*
 * Write the file "header" for the UMTF file -- there's no header
 *  structure in UMTF but we'd see the language node as the header .-)
 */
static void semer_umtf_write_header(SemerFile *file, xmlDocPtr doc)
{
	xmlNodePtr	language;
	
	g_return_if_fail(SEMER_FILE(file)!=NULL);
	g_return_if_fail(doc!=NULL);

	language=xmlNewChild(doc->xmlRootNode, NULL, "language", NULL);
	xmlSetProp(language, "code", file->header->srclang);
}

/*
 * Write a single SemerEntry to the xml document.
 */
static void semer_umtf_write_entry(SemerEntry *entry, xmlDocPtr doc)
{
	xmlNodePtr node, add_node;
	xmlNodePtr original_node;
	xmlNodePtr translation_node, translation_deep_node;
	
	g_return_if_fail(entry!=NULL);
	g_return_if_fail(doc!=NULL);

	node=doc->xmlRootNode->xmlChildrenNode;

	/*
	 * Iterate till our "header" part is finished.
	 */
	while(node && g_ascii_strcasecmp(node->name, "language"))
	{
		node=node->next;
	}

	add_node=xmlNewChild(node, NULL, "message", NULL);
	original_node=xmlNewChild(add_node, NULL, "value", entry->original);
	translation_node=xmlNewChild(add_node, NULL, "translation", NULL);
	translation_deep_node=xmlNewChild(translation_node, NULL, "value", entry->translation);
}

/*
 * Parse the UMTF file into a SemerFile.
 */
SemerFile *semer_umtf_parse_file(const gchar *filename)
{
	SemerFile *file=g_new0(SemerFile, 1);

	
	g_return_val_if_fail(filename!=NULL, NULL);

	return file;
}

/*
 * Save the SemerFile in a UMTF form.
 */
void semer_umtf_save_file(SemerFile *file)
{
	xmlDocPtr umtf;
	
	g_return_if_fail(SEMER_FILE(file)!=NULL);
	g_return_if_fail(SEMER_FILE(file)->filename!=NULL);

	/*
	 * Create the main document for the UMTF informations.
	 */
	umtf=xmlNewDoc("1.0");
	semer_xml_set_root_node(file, umtf);
	semer_umtf_write_header(file, umtf);

	/*
	 * Write out all the nice & nifty SemerEntry's.
	 */
	semer_file_write_all_entries(file, umtf,
		(SemerEntryWriteFunction) semer_umtf_write_entry);

	xmlSaveFormatFile(file->filename, umtf, 2);
	xmlFreeDoc(umtf);
}

G_END_DECLS
