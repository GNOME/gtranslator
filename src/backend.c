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
 * Utility function to do all the split up.
 */
GList *split_up_field(const gchar *content);

/*
 * Break up the plain line content into a list, a sorted list of elements.
 */
GList *split_up_field(const gchar *content)
{
	GList *list=NULL;
	gchar **array;
	gint i=0;
	
	/*
	 * It's not worth giving here a failed assertion, as it's normally
	 *  possible to get an empty tag.
	 */
	if(!content)
	{
		return NULL;
	}
	
	/*
	 * Split up the content line into is's single gchar elements.
	 */
	array=g_strsplit(content, ";", 0);
	
	while(array[i]!=NULL)
	{
		list=g_list_prepend(list, g_strdup(array[i]));
		
		i++;
	}
	
	g_strfreev(array);
	
	/*
	 * Reverse the list of elements and sort it via nautilus_strcasecmp.
	 */
	list=g_list_reverse(list);
	list=g_list_sort(list, (GCompareFunc) nautilus_strcasecmp);
	
	return list;
}

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
		FillUpInformationsForField(compile_command, "compiler");

		if(!nautilus_strcasecmp(node->name, "compilable"))
		{
			gchar *value=xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
			
			/*
			 * Don't operate on empty compilable infos at all.
			 */
			if(!value)
			{
				infos->compilable=FALSE;
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

		/*
		 * Get the filenames list for the backend.
		 */
		if(!nautilus_strcasecmp(node->name, "filenames"))
		{
			gchar *value=xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
			
			/*
			 * Break up the supplied filenames into the single 
			 *  elements and set it up as a list.
			 */
			infos->filenames=split_up_field(value);
			
			g_free(value);
		}
		
		/*
		 * The supported extensions are also delivered as a list.
		 */
		if(!nautilus_strcasecmp(node->name, "extensions"))
		{
			gchar *value=xmlNodeListGetString(doc, node->xmlChildrenNode, 1);

			/*
			 * Get the extensions into the corresponding list.
			 */
			infos->extensions=split_up_field(value);
			
			g_free(value);
		}
		
		node=node->next;
	}

	/*
	 * Set up the module name as a full path.
	 */
	if(infos->modulename)
	{
		gchar *temp;

		temp=g_strdup(infos->modulename);
		g_free(infos->modulename);

		infos->modulename=g_strdup_printf("%s/%s/%s", 
			g_dirname(filename), infos->name, temp);

		g_free(temp);
	}

	return infos;
}

/*
 * Adds the backend to out list of available backends.
 */
void gtranslator_backend_add(const gchar *filename)
{
	GtrBackend 	*backend;
	GModule		*module;
	gpointer	load_function, save_function, save_as_function;

	/*
	 * The function addresses -- initialize them with NULL.
	 */
	load_function=save_function=save_as_function=NULL;
	
	g_return_if_fail(filename!=NULL);

	backend=g_new0(GtrBackend, 1);
	backend->info=read_xml_descriptor(filename);
	g_return_if_fail(backend->info!=NULL);

	/*
	 * Load the GModule handle for the given file.
	 */
	module=g_module_open(backend->info->modulename, G_MODULE_BIND_LAZY);
	g_return_if_fail(module!=NULL);

	/*
	 * Load all the symbols/functions from the backend module.
	 */
	if(g_module_symbol(module, "backend_open", &load_function) &&
		g_module_symbol(module, "backend_save", &save_function) &&
		g_module_symbol(module, "backend_save_as", &save_as_function))
	{
		/*
		 * Assign the resolved symbol functions to the backend's own
		 *  functions namespace.
		 */
		backend->open_file=load_function;
		backend->save_file=save_function;
		backend->save_file_as=save_as_function;
		
		/*
		 * Mr. Debug, never translate such things!
		 */
		g_print("%s\n (%s) loaded.\n", backend->info->description, 
			backend->info->modulename);

		/*
		 * Finally add this backend to the list of our loaded backends.
		 */
		backends=g_list_prepend(backends, backend);
	}
}

/*
 * Parse all files in the directory.
 */
gboolean gtranslator_backend_open_all_backends(const gchar *directory)
{
	GList *backends_list=NULL;

	g_return_val_if_fail(directory!=NULL, FALSE);
	
	/*
	 * Get all backend descriptor files from the directory and sort the
	 *  filename list.
	 */
	backends_list=gtranslator_utils_file_names_from_directory(directory,
		".xml", TRUE, FALSE, TRUE);

	g_return_val_if_fail(backends_list!=NULL, FALSE);

	/*
	 * Add every xml descriptor and it's module to our backends list.
	 */
	g_list_foreach(backends_list, (GFunc) gtranslator_backend_add, NULL);

	/*
	 * Prepending items needs reversing the list at the end of the process.
	 */
	backends=g_list_reverse(backends);

	return TRUE;
}

/*
 * Remove the given backend module.
 */
gboolean gtranslator_backend_remove(GtrBackend **backend)
{
	g_return_val_if_fail(backend!=NULL, FALSE);

	return TRUE;
}

/*
 * Unload all used/loaded backends and clean up all the stuff around it.
 */
gboolean gtranslator_backend_remove_all_backends(void)
{
	/*
	 * If there aren't any backends left in the list, don't try to remove
	 *  them.
	 */
	if(!backends)
	{
		return FALSE;
	}
	else
	{
		g_list_foreach(backends, (GFunc) 
			gtranslator_backend_remove, NULL);

		return TRUE;
	}

	return FALSE;
}

/*
 * Return TRUE if our backends do support opening the given file.
 */
gboolean gtranslator_backend_open(gchar *filename)
{
	GList *mybackends=NULL;
	
	g_return_val_if_fail(filename!=NULL, FALSE);

	if(!backends)
	{
		return FALSE;
	}

	/*
	 * Operate on a local copy of the backends list.
	 */
	mybackends=g_list_copy(backends);
	
	while(mybackends!=NULL)
	{
		/*
		 * Look if the filename matches the filenames supported by the
		 *  backend module.
		 */
		if(GTR_BACKEND(mybackends->data)->info->filenames && 
			(gtranslator_utils_stringlist_strcasecmp(
			GTR_BACKEND(mybackends->data)->info->filenames,
				g_basename(filename)) !=-1))
		{
			/*
			 * Load the file with the corresponding open handle.
			 */
			GTR_BACKEND(mybackends->data)->open_file(filename, NULL);
			return TRUE;
		}
		else
		{
			/*
			 * Check if the filename is of a supported filetype 
			 *  (extension) of the current backend module.
			 */
			while(GTR_BACKEND(mybackends->data)->info->extensions)
			{
				/*
				 * If the extensions do match open the file
				 *  and return TRUE.
				 */
				if(nautilus_istr_has_suffix(filename, 
					GTR_BACKEND(mybackends->data)->info->extensions->data))
				{
					GTR_BACKEND(mybackends->data)->open_file(filename, NULL);
					return TRUE;
				}
				 
				GTR_BACKEND(mybackends->data)->info->extensions=GTR_BACKEND(mybackends->data)->info->extensions->next;
			}
		}
		
		mybackends=mybackends->next;
	}

	g_list_free(mybackends);

	return FALSE;
}
