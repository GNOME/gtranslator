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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "learn.h"
#include "nautilus-string.h"
#include "prefs.h"
#include "translator.h"
#include "utils.h"

#include <time.h>

#include <gnome-xml/parser.h>
#include <gnome-xml/tree.h>

#include <libgnome/gnome-i18n.h>
#include <libgnome/gnome-util.h>

#include <gal/util/e-xml-utils.h>

/*
 * The GtrLearnBuffer structure which holds all informations/parts of the
 *  learn buffer related stuff -- some say personal translation memory to it..
 */
typedef struct
{
	GHashTable	*hash;

	GList		*resources;
	
	gchar		*filename;
	gchar		*encoding;
	gchar		*serial_date;

	gint		serial;
	gint 		index;
	
	gboolean	init_status;
	gboolean	changed;

	xmlDocPtr	doc;
	xmlNodePtr	current_node;
} GtrLearnBuffer;

#define GTR_LEARN_BUFFER(x) ((GtrLearnBuffer *) x)

/*
 * Holds the data for a resource in the index' resouces list.
 */
typedef struct
{
	gchar		*package;
	
	gchar		*updated;
	gchar		*premiereversion;

	gint		 index;
} GtrLearnResource;

#define GTR_LEARN_RESOURCE(x) ((GtrLearnResource *) x)

/*
 * The generally used GtrLearnBuffer -- should hold all elements of the
 *  learn process.
 */
GtrLearnBuffer		*gtranslator_learn_buffer;

/*
 * Do the hard internal work -- mostly GHashTable related.
 */
static void gtranslator_learn_buffer_hash_from_current_node(void);
static void gtranslator_learn_buffer_free_hash_entry(gpointer key, 
	gpointer value, gpointer useless);
static void gtranslator_learn_buffer_write_hash_entry(gpointer key,
	gpointer value, gpointer useless);

static void gtranslator_learn_buffer_learn_function(gpointer date, 
	gpointer useless);

static void gtranslator_learn_buffer_set_umtf_date(void);

/*
 * Escape the characters in the given string to make learning really
 *  work with all characters.
 */
static gchar *gtranslator_learn_buffer_escape(gchar *str);

/*
 * Internal GtrLearnResources list sort routine.
 */
static gint gtranslator_learn_buffer_sort_learn_resource(
	gconstpointer one, gconstpointer two);

/*
 * Hash the entries from the given node point.
 */
static void gtranslator_learn_buffer_hash_from_current_node()
{
	xmlNodePtr	node;

	gchar		*original;
	gchar		*translation;

	original = translation = NULL;

	g_return_if_fail(gtranslator_learn_buffer->current_node!=NULL);
	node=gtranslator_learn_buffer->current_node->xmlChildrenNode;

	while(node && nautilus_strcasecmp(node->name, "value"))
	{
		GTR_ITER(node);
	}

	/*
	 * Get the original message's value by the <value> node.
	 */
	if(node && !nautilus_strcasecmp(node->name, "value"))
	{
		original=xmlNodeListGetString(gtranslator_learn_buffer->doc,
			node->xmlChildrenNode, 1);
	}

	/*
	 * Cruise through the nodes searching the next <translation> node.
	 */
	while(node && nautilus_strcasecmp(node->name, "translation"))
	{
		GTR_ITER(node);
	}

	/*
	 * If there's a translation node with content then we can use the node
	 *  for further extraction.
	 */
	if(node && node->xmlChildrenNode)
	{
		node=node->xmlChildrenNode;
		
		/*
		 * Now go for the <value> node under the <translation> node.
		 */
		while(node && nautilus_strcasecmp(node->name, "value"))
		{
			GTR_ITER(node);
		}

		/*
		 * Here we do get the node contents.
		 */
		if(node && !nautilus_strcasecmp(node->name, "value"))
		{
			translation=xmlNodeListGetString(gtranslator_learn_buffer->doc,
				node->xmlChildrenNode, 1);
		}
	}

	/*
	 * If we did find both node contents then we can insert'em into "our" hash.
	 */
	if(original && translation)
	{
		g_hash_table_insert(gtranslator_learn_buffer->hash, 
			g_strdup(original), g_strdup(translation));

		GTR_FREE(original);
		GTR_FREE(translation);
	}
}

/*
 * Free the hash table entry/data.
 */
static void gtranslator_learn_buffer_free_hash_entry(gpointer key, gpointer value, gpointer useless)
{
	GTR_FREE(key);
	GTR_FREE(value);
}

/*
 * Write the hash entries -- one entry for a msgid/msgstr pair.
 */
static void gtranslator_learn_buffer_write_hash_entry(gpointer key, gpointer value, gpointer useless)
{
	gchar		*string_to_write;
	xmlNodePtr 	new_node;
	xmlNodePtr 	value_node;
	xmlNodePtr 	translation_node;
	xmlNodePtr 	translation_value_node;

	g_return_if_fail(gtranslator_learn_buffer->current_node!=NULL);

	new_node=xmlNewChild(gtranslator_learn_buffer->current_node, NULL, 
		"message", NULL);

	/*
	 * Escape the string to the right and necessary form before writing it.
	 */
	string_to_write=gtranslator_learn_buffer_escape((gchar *) key);
	value_node=xmlNewChild(new_node, NULL, "value", string_to_write);
	GTR_FREE(string_to_write);

	translation_node=xmlNewChild(new_node, NULL, "translation", NULL);

	/*
	 * Again escape the values to be given to the libxml routines.
	 */
	string_to_write=gtranslator_learn_buffer_escape((gchar *) value);
	translation_value_node=xmlNewChild(translation_node, NULL, 
		"value", string_to_write);
	GTR_FREE(string_to_write);
}

/*
 * Set the UMTF serial date for our internal GtrLearnBuffer.
 */
static void gtranslator_learn_buffer_set_umtf_date()
{
	gchar		*date_string;
	struct tm	*time_struct;
	time_t		now;

	g_return_if_fail(gtranslator_learn_buffer->init_status==TRUE);

	date_string=g_malloc(20);

	/*
	 * Get the time/date elements.
	 */
	now=time(NULL);
	time_struct=localtime(&now);

	/*
	 * Buold the UMTF alike date string.
	 */
	strftime(date_string, 20, "%Y-%m-%d %H:%M:%S", time_struct);

	GTR_FREE(gtranslator_learn_buffer->serial_date);
	gtranslator_learn_buffer->serial_date=g_strdup(date_string);
	
	GTR_FREE(date_string);
}

/*
 * Escape the necessary characters before writing them to the XML
 *  document.
 */
static gchar *gtranslator_learn_buffer_escape(gchar *str)
{
	gchar	*write_string=NULL;
	
	g_return_val_if_fail(str!=NULL, NULL);

	/*
	 * Escape all necessary characters rightly for saving in the XML file.
	 */
	write_string=nautilus_str_replace_substring(str, "&", "&amp;");
	write_string=nautilus_str_replace_substring(write_string, "<", "&lt;");
	write_string=nautilus_str_replace_substring(write_string, ">", "&gt;");
	write_string=nautilus_str_replace_substring(write_string, "'", "&apos;");
	write_string=nautilus_str_replace_substring(write_string, "\"", "&quot;");
	

	return write_string;
}

/*
 * A sorting function as helper for the g_list_sort calls the GtrLearnResources'
 *  list in the GtrLearnBuffer.
 */
static gint gtranslator_learn_buffer_sort_learn_resource(
	gconstpointer one, gconstpointer two)
{
	/*
	 * Now catch the cases where index <=> on the two GtrLearnResources.
	 */
	if(GTR_LEARN_RESOURCE(one)->index==GTR_LEARN_RESOURCE(two)->index)
	{
		return 0;
	}
	else if(GTR_LEARN_RESOURCE(one)->index < GTR_LEARN_RESOURCE(two)->index)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

/*
 * Learn the given data entry (a GtrMsg) automatically.
 */
static void gtranslator_learn_buffer_learn_function(gpointer data, 
	gpointer useless)
{
	GtrMsg 	*message=GTR_MSG(data);
	
	g_return_if_fail(message!=NULL);
	g_return_if_fail(GTR_MSG(message)->msgid!=NULL);

	/*
	 * Learn only translated messages.
	 */
	if(message->msgstr && (message->status & GTR_MSG_STATUS_TRANSLATED))
	{
		gtranslator_learn_string(message->msgid, message->msgstr);
	}
}

/*
 * Initialize our internal learn buffers .-)
 */
void gtranslator_learn_init()
{
	/*
	 * Create a new instance of our GtrLearnBuffer.
	 */
	gtranslator_learn_buffer=g_new0(GtrLearnBuffer, 1);
	
	gtranslator_learn_buffer->filename=g_strdup_printf(
		"%s/.gtranslator/umtf/learn-buffer.xml", g_get_home_dir());

	gtranslator_learn_buffer->hash=g_hash_table_new(g_str_hash, g_str_equal);
	gtranslator_learn_buffer->resources=NULL;
	gtranslator_learn_buffer->index=0;

	/*
	 * Read in our autolearned xml document.
	 */
	if(g_file_test(gtranslator_learn_buffer->filename, G_FILE_TEST_ISFILE))
	{
		xmlNodePtr node;
		
		gtranslator_learn_buffer->doc=xmlParseFile(gtranslator_learn_buffer->filename);
		g_return_if_fail(gtranslator_learn_buffer->doc!=NULL);

		gtranslator_learn_buffer->encoding=e_xml_get_string_prop_by_name(gtranslator_learn_buffer->doc->xmlRootNode, "encoding");

		gtranslator_learn_buffer->current_node=gtranslator_learn_buffer->doc->xmlRootNode->xmlChildrenNode;
		g_return_if_fail(gtranslator_learn_buffer->current_node!=NULL);

		node=gtranslator_learn_buffer->current_node;

		/*
		 * Parse the message entry via the
		 *  "gtranslator_learn_buffer_hash_from_current_node" function.
		 */
		while(node!=NULL)
		{
			/*
			 * Read in the serial number.
			 */
			if(!nautilus_strcasecmp(node->name, "serial"))
			{
				gchar		*contentstring=NULL;

				contentstring=xmlNodeListGetString(gtranslator_learn_buffer->doc,
					node->xmlChildrenNode, 1);
				
				if(contentstring)
				{
					/*
					 * Read the serial number from the node contents.
					 */
					sscanf(contentstring, "%i", 
						&gtranslator_learn_buffer->serial);
					
					GTR_FREE(contentstring);
				}
				else
				{
					gtranslator_learn_buffer->serial=0;
				}

				/*
				 * Increment the serial number of the learn buffer.
				 */
				gtranslator_learn_buffer->serial++;
			}

			/*
			 * Read in any resources' found in the index.
			 */
			if(!nautilus_strcasecmp(node->name, "index"))
			{
				xmlNodePtr	resources_node;

				resources_node=node->xmlChildrenNode;

				while(resources_node)
				{
					GtrLearnResource *resource=g_new0(GtrLearnResource, 1);

					resource->package=e_xml_get_string_prop_by_name(resources_node, "package");
					resource->updated=e_xml_get_string_prop_by_name(resources_node, "updated");
					resource->premiereversion=e_xml_get_string_prop_by_name(resources_node, "premiereversion");
					resource->index=e_xml_get_integer_prop_by_name(resources_node, "index");

					/*
					 * Add the current parsed in resource to the learn buffer's
					 *  resources list.
					 */
					gtranslator_learn_buffer->resources=g_list_prepend(
						gtranslator_learn_buffer->resources, 
							(gpointer) resource);

					/*
					 * Prepend & reverse are good friends in case of GList's .-)
					 */
					gtranslator_learn_buffer->resources=g_list_reverse(
						gtranslator_learn_buffer->resources);

					/*
					 * Increment the index'/resources count of the learn buffer.
					 */
					gtranslator_learn_buffer->index++;
					
					GTR_ITER(resources_node);
				}
			}
			
			if(!nautilus_strcasecmp(node->name, "message"))
			{
				gtranslator_learn_buffer->current_node=node;
				gtranslator_learn_buffer_hash_from_current_node();
			}

			GTR_ITER(node);
		}

		xmlFreeDoc(gtranslator_learn_buffer->doc);

		gtranslator_learn_buffer->changed=FALSE;
	}
	else
	{
		gtranslator_learn_buffer->encoding=NULL;
		gtranslator_learn_buffer->changed=TRUE;
	}
	
	/*
	 * Now we'd be inited after all.
	 */
	gtranslator_learn_buffer->init_status=TRUE;
}

/*
 * Return our internal state.
 */
gboolean gtranslator_learn_initialized()
{
	return (gtranslator_learn_buffer->init_status);
}

/*
 * Clean up our learn space -- we're empty afterwards.
 */
void gtranslator_learn_shutdown()
{
	xmlNodePtr	root_node;
	xmlNodePtr	language_node;
	xmlNodePtr	translator_node;
	xmlNodePtr	serial_node;
	xmlNodePtr	index_node;
	
	gchar		*serial_string;
	
	g_return_if_fail(gtranslator_learn_buffer->filename!=NULL);
	g_return_if_fail(gtranslator_learn_buffer->init_status==TRUE);

	/*
	 * Only build the new XML document if any change in the learn buffer
	 *  had been performed.
	 */
	if(gtranslator_learn_buffer->changed)
	{
		/*
		 * Create the XML document.
		 */
		gtranslator_learn_buffer->doc=xmlNewDoc("1.0");
	
		/*
		 * Set the encoding of the XML file to get a cleanly-structured 
		 *  and well-formed XML document.
		 */
		if(gtranslator_learn_buffer->encoding)
		{
			gtranslator_learn_buffer->doc->encoding=g_strdup(gtranslator_learn_buffer->encoding);
			GTR_FREE(gtranslator_learn_buffer->encoding);
		}
	
		/*
		 * Set up the main <umtf> document root node.
		 */
		root_node=xmlNewDocNode(gtranslator_learn_buffer->doc, NULL, 
			"umtf", NULL);
	
		xmlSetProp(root_node, "version", "0.6");
		xmlDocSetRootElement(gtranslator_learn_buffer->doc, root_node);
	
		/*
		 * Set the header <language> tag with language informations.
		 */
		language_node=xmlNewChild(root_node, NULL, "language", NULL);
		
		e_xml_set_string_prop_by_name(language_node, "ename", 
			gtranslator_translator->language->name);
		e_xml_set_string_prop_by_name(language_node, "code", 
			gtranslator_translator->language->locale);
		e_xml_set_string_prop_by_name(language_node, "email", 
			gtranslator_translator->language->group_email);
	
		/*
		 * Set <translator> node with translator informations -- if available.
		 */
		translator_node=xmlNewChild(root_node, NULL, "translator", NULL);
		
		e_xml_set_string_prop_by_name(translator_node, "name", gtranslator_translator->name);
		e_xml_set_string_prop_by_name(translator_node, "email", gtranslator_translator->email);
	
		/*
		 * Set the UMTF date string for our internal GtrLearnBuffer.
		 */
		gtranslator_learn_buffer_set_umtf_date();
		g_return_if_fail(gtranslator_learn_buffer->serial_date!=NULL);
	
		/*
		 * Build the serial string -- the serial number must be in gchar form 
		 *  to be written by xmlSetProp.
		 */
		if(gtranslator_learn_buffer->serial > 1)
		{
			serial_string=g_strdup_printf("%i", 
				gtranslator_learn_buffer->serial);
		}
		else
		{
			serial_string=g_strdup_printf("%i", 1);
		}
	
		/*
		 * Add the <serial> node with the serial string and date.
		 */
		serial_node=xmlNewChild(root_node, NULL, "serial", serial_string);
		xmlSetProp(serial_node, "date", gtranslator_learn_buffer->serial_date);
		GTR_FREE(serial_string);
	
		/*
		 * Write the <index> area -- if possible with contents.
		 */
		index_node=xmlNewChild(root_node, NULL, "index", NULL);
	
		if(index_node)
		{
			xmlNodePtr	resource_node;
	
			/*
			 * Sort the resources' list if there's any.
			 */
			if(gtranslator_learn_buffer->resources)
			{
				gtranslator_learn_buffer->resources=g_list_sort(
					gtranslator_learn_buffer->resources, 
						(GCompareFunc) gtranslator_learn_buffer_sort_learn_resource);
			}
	
			/*
			 * Write any resources entry from the list.
			 */
			while(gtranslator_learn_buffer->resources!=NULL)
			{
				GtrLearnResource *resource=GTR_LEARN_RESOURCE(
					gtranslator_learn_buffer->resources->data);
	
				resource_node=xmlNewChild(index_node, NULL, "resource", NULL);
				g_return_if_fail(resource_node!=NULL);
				
				/*
				 * Write all the attributes for a resource entry.
				 */
				e_xml_set_string_prop_by_name(resource_node, "package", resource->package);
				e_xml_set_string_prop_by_name(resource_node, "updated", resource->updated);
				e_xml_set_string_prop_by_name(resource_node, "premiereversion", resource->premiereversion); 
				
				e_xml_set_integer_prop_by_name(resource_node, "index", resource->index);
				
				GTR_ITER(gtranslator_learn_buffer->resources);
			}
		}
		
		/*
		 * Clean up the hash table we're using, write it's contents, 
		 *  free them and destroy the hash table.
		 */
		gtranslator_learn_buffer->current_node=serial_node->parent;
		g_return_if_fail(gtranslator_learn_buffer->current_node!=NULL);
	
		g_hash_table_foreach(gtranslator_learn_buffer->hash, 
			(GHFunc) gtranslator_learn_buffer_write_hash_entry, NULL);
	}
	
	g_hash_table_foreach(gtranslator_learn_buffer->hash, 
		(GHFunc) gtranslator_learn_buffer_free_hash_entry, NULL);
	
	g_hash_table_destroy(gtranslator_learn_buffer->hash);

	/*
	 * Save the file if necessary due to a learn buffer change.
	 */
	if(gtranslator_learn_buffer->changed)
	{
		xmlSetDocCompressMode(gtranslator_learn_buffer->doc, 9);
		xmlSaveFile(gtranslator_learn_buffer->filename, gtranslator_learn_buffer->doc);
		xmlFreeDoc(gtranslator_learn_buffer->doc);
	}

	g_list_free(gtranslator_learn_buffer->resources);

	GTR_FREE(gtranslator_learn_buffer->serial_date);
	GTR_FREE(gtranslator_learn_buffer->filename);
	GTR_FREE(gtranslator_learn_buffer);
}

/*
 * Learn the complete GtrPo file's messages.
 */
void gtranslator_learn_po_file(GtrPo *po_file)
{
	GtrLearnResource *resource;
	
	g_return_if_fail(po_file!=NULL);
	g_return_if_fail(GTR_PO(po_file)->messages!=NULL);

	/*
	 * Learn all translated messages.
	 */
	g_list_foreach(GTR_PO(po_file)->messages, 
		(GFunc) gtranslator_learn_buffer_learn_function, NULL);

	/*
	 * And add a resource link in the UMTF file.
	 */
	resource=g_new0(GtrLearnResource, 1);
	
	/*
	 * Read in the resource informations from the po file's header.
	 */
	resource->package=po->header->prj_name;
	resource->updated=po->header->po_date;
	if(po->header->prj_version)
	{
		resource->premiereversion=po->header->prj_version;
	}
	else
	{
		resource->premiereversion="0";
	}

	/*
	 * Increment the index count by one.
	 */
	resource->index=gtranslator_learn_buffer->index;
	resource->index++;

	gtranslator_learn_buffer->resources=g_list_prepend(
		gtranslator_learn_buffer->resources, 
			(gpointer) resource);

	gtranslator_learn_buffer->resources=g_list_reverse(
		gtranslator_learn_buffer->resources);
}

/*
 * Export the learn buffer to a gettext po file -- is really not quite perfect
 *  but fills it's purpose.
 */
void gtranslator_learn_export_to_po_file(const gchar *po_file)
{
	g_return_if_fail(po_file!=NULL);
	g_return_if_fail(gtranslator_learn_buffer->init_status==TRUE);

	/*
	 * FIXME: Write.
	 */
}

/*
 * Add it to our learned list!
 */
void gtranslator_learn_string(const gchar *id_string, const gchar *str_string)
{
	g_return_if_fail(id_string!=NULL);
	g_return_if_fail(str_string!=NULL);

	/*
	 * Insert the id/str_string pair only if there's no entry for the
	 *  id_string yet.
	 */
	if(!gtranslator_learn_learned(id_string))
	{
		g_hash_table_insert(gtranslator_learn_buffer->hash, 
			g_strdup(id_string), g_strdup(str_string));

		/*
		 * As we've got now learned a new string we set the "changed"
		 *  field of the learn buffer.
		 */
		gtranslator_learn_buffer->changed=TRUE;
	}
}

/*
 * Returns the learn status.
 */
gboolean gtranslator_learn_learned(const gchar *string)
{
	g_return_val_if_fail(string!=NULL, FALSE);

	/*
	 * Simple lookup encapsulation.
	 */
	if(g_hash_table_lookup(gtranslator_learn_buffer->hash, (gconstpointer) string))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*
 * The "query" function which returns the string found in the learned_buffer;
 *  or NULL if nothing was found.
 */
gchar *gtranslator_learn_get_learned_string(const gchar *search_string)
{
	gchar		*found_string;
	
	g_return_val_if_fail(search_string!=NULL, NULL);
	g_return_val_if_fail(gtranslator_learn_buffer->init_status==TRUE, NULL);

	/*
	 * Look the given search_string up in our internally used hash table.
	 */
	found_string=(gchar *) g_hash_table_lookup(
		gtranslator_learn_buffer->hash, (gconstpointer) search_string);

	/*
	 * Check if there had been any exact match -- if not try other 
	 *  "matching" methods with somehow more intelligent patterns.
	 */
	if(found_string)
	{
		return found_string;
	}
	else if(GtrPreferences.fuzzy_matching)
	{
		gchar	*query_string;

	    	/*
	     	 * Get a new query string without any punctuation characters.
	    	 */
		query_string=gtranslator_utils_strip_all_punctuation_chars(search_string);
	    
		/*
		 * Try this new query.
		 */
		found_string=(gchar *) g_hash_table_lookup(
			gtranslator_learn_buffer->hash, 
				(gconstpointer) query_string);

		/*
		 * Don't trust too short query results .-)
		 */
		if(!found_string || strlen(found_string) <= 2)
		{
			return NULL;
		}
		else
		{
			return found_string;
		}
	}
	else
	{
		return NULL;
	}
}
