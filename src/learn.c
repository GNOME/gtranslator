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
#include "parse.h"
#include "prefs.h"
#include "utils.h"

#include <time.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <libgnome/gnome-i18n.h>
#include <libgnome/gnome-util.h>

#define UMTF_FILENAME "autolearn-umtf.xml"

/*
 * The internally used variables for learning.
 */
static gboolean 	init_status=FALSE;
static gint		serial=0;
static GHashTable	*learn_hash=NULL;

/*
 * Do the hard hash work.
 */
static void gtranslator_learn_hash_from_node(xmlNodePtr node);
static void gtranslator_learn_free_hash_entry(gpointer key, gpointer value, gpointer useless);
static void gtranslator_learn_write_hash_entry(gpointer key, gpointer value, gpointer node);

/*
 * Return an UMTF date string.
 */
static gchar *gtranslator_learn_get_umtf_date(void);

/*
 * Hash the entries from the given node point.
 */
static void gtranslator_learn_hash_from_node(xmlNodePtr node)
{
	gchar	*original, *translation;

	original = translation = NULL;

	while(node && nautilus_strcasecmp(node->name, "value"))
	{
		node=node->next;
	}

	if(node && !nautilus_strcasecmp(node->name, "value"))
	{
		original=xmlNodeGetContent(node);
	}

	while(node && nautilus_strcasecmp(node->name, "translation"))
	{
		node=node->next;
	}

	if(node)
	{
		while(node && nautilus_strcasecmp(node->name, "value"))
		{
			node=node->next;
		}

		if(node && !nautilus_strcasecmp(node->name, "value"))
		{
			translation=xmlNodeGetContent(node);
		}
	}

	if(original && translation)
	{
		g_hash_table_insert(learn_hash, 
			g_strdup(original), g_strdup(translation));

		g_free(original);
		g_free(translation);
	}
}

/*
 * Free the hash table entry/data.
 */
static void gtranslator_learn_free_hash_entry(gpointer key, gpointer value, gpointer useless)
{
	g_free(key);
	g_free(value);
}

/*
 * Write the hash entries -- one entry for a msgid/msgstr pair.
 */
static void gtranslator_learn_write_hash_entry(gpointer key, gpointer value, gpointer node)
{
	xmlNodePtr new_node;
	xmlNodePtr value_node;
	xmlNodePtr translation_node;
	xmlNodePtr translation_value_node;

	g_return_if_fail(node!=NULL);

	new_node=xmlNewChild(node, NULL, "message", NULL);
	value_node=xmlNewChild(new_node, NULL, "value", (gchar *) key);

	translation_node=xmlNewChild(new_node, NULL, "translation", NULL);
	translation_value_node=xmlNewChild(translation_node, NULL, "value", (gchar *) value);
}

/*
 * Create/return an UMTF formed date string.
 */
static gchar *gtranslator_learn_get_umtf_date()
{
	gchar		*date_string;
	struct tm	*time_struct;
	time_t		now;

	date_string=g_malloc(20);

	now=time(NULL);
	time_struct=localtime(&now);

	strftime(date_string, 20, "%Y-%m-%d %H:%M:%S", time_struct);

	return date_string;
}

/*
 * Initialize our internal learn buffers .-)
 */
void gtranslator_learn_init()
{
	gchar		*learn_base_file;
	
	g_return_if_fail(init_status==FALSE);

	learn_base_file=g_strdup_printf("%s/.gtranslator/" UMTF_FILENAME,
		g_get_home_dir());

	learn_hash=g_hash_table_new(g_str_hash, g_str_equal);

	/*
	 * Read in our autolearn xml document.
	 */
	if(g_file_test(learn_base_file, G_FILE_TEST_ISFILE))
	{
		xmlDocPtr doc;
		xmlNodePtr node;

		doc=xmlParseFile(learn_base_file);
		g_return_if_fail(doc!=NULL);

		node=doc->xmlRootNode;
		g_return_if_fail(node!=NULL);

		/*
		 * Parse the message entry via gtranslator_learn_hash_from_node
		 *  function.
		 */
		while(node)
		{
			/*
			 * Read in the serial number.
			 */
			if(node->name && !nautilus_strcasecmp(node->name, "serial"))
			{
				gchar	*contentstring;

				contentstring=xmlNodeGetContent(node);
				
				if(contentstring)
				{
					/*
					 * Read the serial number in and 
					 *  increment it by one.
					 */
					sscanf(contentstring, "%i", &serial);
					g_free(contentstring);

					serial++;
				}
			}
			
			if(node->name && !nautilus_strcasecmp(node->name, "message"))
			{
				gtranslator_learn_hash_from_node(node);
			}

			node=node->next;
		}

		xmlFreeDoc(doc);
	}
	else
	{
		/*
		 * Setup a foo'sh hash content for the empty cases: Use 
		 *  "gtranslator" as a msgid/msgstr pair ,-)
		 */
		g_hash_table_insert(learn_hash, 
			g_strdup("gtranslator"), g_strdup("gtranslator"));
	}
	
	init_status=TRUE;
}

/*
 * Return our internal state.
 */
gboolean gtranslator_learn_initialized()
{
	return init_status;
}

/*
 * Clean up our learn space -- we're empty afterwards.
 */
void gtranslator_learn_shutdown()
{
	xmlDocPtr	doc;
	
	xmlNodePtr	root_node;
	xmlNodePtr	language_node;
	xmlNodePtr	translator_node;
	xmlNodePtr	serial_node;
	
	gchar 		*filename;
	gchar		*date;
	gchar		*serial_string;
	
	g_return_if_fail(init_status==TRUE);

	filename=g_strdup_printf("%s/.gtranslator/" UMTF_FILENAME,
		g_get_home_dir());

	/*
	 * Create the XML document.
	 */
	doc=xmlNewDoc("1.0");

	/*
	 * Set up the main <umtf> document root node and set it's version attribute.
	 */
	root_node=xmlNewDocNode(doc, NULL, "umtf", NULL);
	xmlSetProp(root_node, "version", "0.6");
	xmlDocSetRootElement(doc, root_node);

	/*
	 * Set the header <language> tag with language informations.
	 */
	language_node=xmlNewChild(root_node, NULL, "language", NULL);

	if(language)
	{
		xmlSetProp(language_node, "ename", language);
		xmlSetProp(language_node, "name", _(language));
	}

	if(lc)
	{
		xmlSetProp(language_node, "code", lc);
	}

	if(lg)
	{
		xmlSetProp(language_node, "email", lg);
	}

	/*
	 * Set <translator> node with translator informations -- if available.
	 */
	translator_node=xmlNewChild(root_node, NULL, "translator", NULL);

	if(author)
	{
		xmlSetProp(translator_node, "name", author);
	}

	if(email)
	{
		xmlSetProp(translator_node, "email", email);
	}

	/*
	 * Get the current date in UMTF-alike form.
	 */
	date=gtranslator_learn_get_umtf_date();

	/*
	 * Build the serial string -- the serial number must be in gchar form to be
	 *  written by xmlSetProp.
	 */
	if(serial>=1)
	{
		serial_string=g_strdup_printf("%i", serial);
	}
	else
	{
		serial_string=g_strdup_printf("%i", 1);
	}

	/*
	 * Add the <serial> node with erial string and date.
	 */
	serial_node=xmlNewChild(root_node, NULL, "serial", serial_string);
	xmlSetProp(serial_node, "date", date);

	g_free(serial_string);
	g_free(date);
	
	/*
	 * Clean up the hash table we're using, write it's contents, free them and destroy
	 *  the hash table.
	 */
	g_hash_table_foreach(learn_hash, (GHFunc) gtranslator_learn_write_hash_entry, root_node); 
	g_hash_table_foreach(learn_hash, (GHFunc) gtranslator_learn_free_hash_entry, NULL);
	g_hash_table_destroy(learn_hash);

	/*
	 * Save the file and free our used XML document.
	 */
	xmlSaveFile(filename, doc);
	xmlFreeDoc(doc);
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
		g_hash_table_insert(learn_hash, 
			g_strdup(id_string), g_strdup(str_string));
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
	if(g_hash_table_lookup(learn_hash, (gconstpointer) string))
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
	gchar	*found_string;
	
	g_return_val_if_fail(search_string!=NULL, NULL);

	/*
	 * Look the given search_string up in our internally used hash table.
	 */
	found_string=(gchar *) g_hash_table_lookup(learn_hash, (gconstpointer) search_string);

	/*
	 * Return it via g_strdup, free it or return NULL in bad case .-(
	 */
	if(found_string)
	{
		return g_strdup(found_string);
		g_free(found_string);
	}
	else
	{
		return NULL;
	}
}
