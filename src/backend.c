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
#include "utils.h"

#include <gnome-xml/parser.h>
#include <gnome-xml/tree.h>

/*
 * Internal prototypes:
 */
void read_xml_descriptor(const gchar *filename, GtrBackendInformations **info);

/*
 * Read in the xml descriptor file from the given file.
 */
void read_xml_descriptor(const gchar *filename, GtrBackendInformations **info)
{
	xmlDocPtr doc;
	xmlNodePtr node;
	
	doc=xmlParseFile(filename);
	g_return_if_fail(doc!=NULL);

	node=xmlDocGetRootElement(doc);
	
	if(g_strcasecmp(node->name, "backend"))
	{
		return;
	}

	node=node->xmlChildrenNode;

	while(node!=NULL)
	{
		/*
		 * FIXME
		 */

		node=node->next;
	}
}

/*
 * Adds the backend to out list of available backends.
 */
void gtranslator_backend_add(const gchar *filename)
{
	GtrBackend *backend;
	g_return_if_fail(filename!=NULL);

	backend=g_new0(GtrBackend, 1);
	read_xml_descriptor(filename, &GTR_BACKEND_INFORMATIONS(backend->info));
}

/*
 * Parse all files in the directory.
 */
gboolean gtranslator_backend_open_all_backends(const gchar *directory)
{
	if(!directory)
	{
		directory=BACKENDS_DIR;
	}
	
	backends=gtranslator_utils_file_names_from_directory(directory,
		".xml", TRUE, FALSE);

	g_return_val_if_fail(backends!=NULL, FALSE);

	return TRUE;
}
