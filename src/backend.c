/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *    
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "backend.h"
#include "defines.include"
#include "nautilus-string.h"
#include "utils.h"

#include <gal/util/e-xml-utils.h>
#include <gnome-xml/parser.h>

/*
 * Internal prototypes:
 */
GtrBackendInformations *read_xml_descriptor(const gchar *filename);

/*
 * Read in the xml descriptor file from the given file.
 */
GtrBackendInformations *read_xml_descriptor(const gchar *filename)
{
	GtrBackendInformations *infos=g_new0(GtrBackendInformations, 1);
	xmlDocPtr doc;
	xmlNodePtr node;

	/*
	 * Again a crappy macro for getting the stuff into the struct.
	 */
	#define FillUpInformationsForField(field, propname); \
		if(!nautilus_strcasecmp(node->name, propname)) \
		{ \
			gchar *value=xmlNodeListGetString(doc, node->xmlChildrenNode, 1); \
			if(value) \
			{ \
				g_strstrip(value); \
				infos->field=g_strdup(value); \
				g_free(value); \
			} \
			else \
			{ \
				infos->field=NULL; \
			} \
		}

	doc=xmlParseFile(filename);
	g_return_val_if_fail(doc!=NULL, NULL);

	node=xmlDocGetRootElement(doc);
	
	/*
	 * Check foo'shly if the xml file is a backend descriptor file
	 *  (starts with the "<backend>".
	 */
	if(g_strcasecmp(node->name, "backend"))
	{
		return NULL;
	}
	else
	{
		/*
		 * Fill up the information structure's informations with the
		 *  firstwhind values.
		 */
		infos->name=xmlGetProp(node, "name");

		/*
		 * The description field should always be translated, therefore
		 *  use the gal function to do it so ,-)
		 */
		infos->description=e_xml_get_translated_string_prop_by_name(
			node, "description");
	}

	node=node->xmlChildrenNode;
			
	while(node!=NULL)
	{
		FillUpInformationsForField(modulename, "module");
		FillUpInformationsForField(extensions, "extensions");
		FillUpInformationsForField(filenames, "filenames");
		FillUpInformationsForField(compile_command, "compiler");

		if(!nautilus_strcasecmp(node->name, "compilable"))
		{
			gchar *value=xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
			
			/*
			 * Don't operate on empty compilable infos at all.
			 */
			if(!value)
			{
				break;
			}
			
			g_strstrip(value);
			
			/*
			 * Check if the backend allows us to compile it and set
			 *  the necessary informations to handle this.
			 */
			if(!nautilus_strcasecmp(value, "yes") ||
				!nautilus_strcasecmp(value, "true") ||
				!nautilus_strcasecmp(value, "1"))
			{
				infos->compilable=TRUE;
			}
			else
			{
				infos->compilable=FALSE;
			}
			
			g_free(value);
		}

		node=node->next;
	}

	return infos;
}

/*
 * Adds the backend to out list of available backends.
 */
void gtranslator_backend_add(const gchar *filename)
{
	GtrBackend *backend;
	g_return_if_fail(filename!=NULL);

	backend=g_new0(GtrBackend, 1);
	backend->info=read_xml_descriptor(filename);

	g_return_if_fail(backend->info!=NULL);

	/*
	 * Mr. Debug, never translate such things!
	 */
	g_message("%s (%s) descriptor loaded", backend->info->name, backend->info->description);
}

/*
 * Parse all files in the directory.
 */
gboolean gtranslator_backend_open_all_backends(const gchar *directory)
{
	g_return_val_if_fail(directory!=NULL, FALSE);
	
	/*
	 * Get all backend descriptor files from the directory and sort the
	 *  filename list.
	 */
	backends=gtranslator_utils_file_names_from_directory(directory,
		".xml", TRUE, FALSE, TRUE);

	g_return_val_if_fail(backends!=NULL, FALSE);

	g_list_foreach(backends, (GFunc) gtranslator_backend_add, NULL);

	return TRUE;
}
