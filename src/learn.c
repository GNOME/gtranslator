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

/*
 * The GtrLearnBuffer structure which holds all informations/parts of the
 *  learn buffer related stuff -- some say personal translation memory to it..
 */
typedef struct
{
	GHashTable	*hash;
	
	gchar		*filename;
	gchar		*serial_date;

	gint		serial;
	gboolean	init_status;
	xmlDocPtr	doc;
	xmlNodePtr	current_node;
} GtrLearnBuffer;

#define GTR_LEARN_BUFFER(x) ((GtrLearnBuffer *) x)

/*
 * The generally used GtrLearnBuffer -- should hold all elements of the
 *  learn process.
 */
static GtrLearnBuffer	*gtranslator_learn_buffer=NULL;

/*
 * Do the hard internal work -- mostly GHashTable related.
 */
static void gtranslator_learn_buffer_hash_from_current_node(void);
static void gtranslator_learn_buffer_free_hash_entry(gpointer key, gpointer value, gpointer useless);
static void gtranslator_learn_buffer_write_hash_entry(gpointer key, gpointer value, gpointer useless);

static void gtranslator_learn_buffer_set_umtf_date(void);

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
	node=gtranslator_learn_buffer->current_node;

	while(node && nautilus_strcasecmp(node->name, "value"))
	{
		node=node->next;
	}

	if(node && !nautilus_strcasecmp(node->name, "value"))
	{
		original=xmlNodeListGetString(gtranslator_learn_buffer->doc,
			node->xmlChildrenNode, 1);
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
			translation=xmlNodeListGetString(gtranslator_learn_buffer->doc,
				node->xmlChildrenNode, 1);
		}
	}

	if(original && translation)
	{
		g_hash_table_insert(gtranslator_learn_buffer->hash, 
			g_strdup(original), g_strdup(translation));

		g_free(original);
		g_free(translation);
	}
}

/*
 * Free the hash table entry/data.
 */
static void gtranslator_learn_buffer_free_hash_entry(gpointer key, gpointer value, gpointer useless)
{
	if(key)
	{
		g_free(key);
	}
	
	if(value)
	{
		g_free(value);
	}
}

/*
 * Write the hash entries -- one entry for a msgid/msgstr pair.
 */
static void gtranslator_learn_buffer_write_hash_entry(gpointer key, gpointer value, gpointer useless)
{
	xmlNodePtr new_node;
	xmlNodePtr value_node;
	xmlNodePtr translation_node;
	xmlNodePtr translation_value_node;

	g_return_if_fail(gtranslator_learn_buffer->current_node!=NULL);

	new_node=xmlNewChild(gtranslator_learn_buffer->current_node, NULL, "message", NULL);
	value_node=xmlNewChild(new_node, NULL, "value", (gchar *) key);

	translation_node=xmlNewChild(new_node, NULL, "translation", NULL);
	translation_value_node=xmlNewChild(translation_node, NULL, "value", (gchar *) value);
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

	/*
	 * Free any old serial date in our structure.
	 */
	if(gtranslator_learn_buffer->serial_date)
	{
		g_free(gtranslator_learn_buffer->serial_date);
	}
	
	gtranslator_learn_buffer->serial_date=g_strdup(date_string);
	g_free(date_string);
}

/*
 * Initialize our internal learn buffers .-)
 */
void gtranslator_learn_init()
{
	gtranslator_learn_buffer->filename=g_strdup_printf(
		"%s/.gtranslator/umtf-learn-buffer.xml", g_get_home_dir());

	gtranslator_learn_buffer->hash=g_hash_table_new(g_str_hash, g_str_equal);

	/*
	 * Read in our autolearned xml document.
	 */
	if(g_file_test(gtranslator_learn_buffer->filename, G_FILE_TEST_ISFILE))
	{
		xmlNodePtr node;
		
		gtranslator_learn_buffer->doc=xmlParseFile(gtranslator_learn_buffer->filename);
		g_return_if_fail(gtranslator_learn_buffer->doc!=NULL);

		gtranslator_learn_buffer->current_node=gtranslator_learn_buffer->doc->xmlRootNode;
		g_return_if_fail(gtranslator_learn_buffer->current_node!=NULL);

		node=gtranslator_learn_buffer->current_node;

		/*
		 * Parse the message entry via gtranslator_learn_buffer_hash_from_current_node
		 *  function.
		 */
		while(node && node->name)
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
					
					g_free(contentstring);
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
			
			if(!nautilus_strcasecmp(node->name, "message"))
			{
				gtranslator_learn_buffer->current_node=node;
				gtranslator_learn_buffer_hash_from_current_node();
			}

			node=node->next;
		}

		xmlFreeDoc(gtranslator_learn_buffer->doc);
	}
	else
	{
		/*
		 * If no learn buffer is present (e.g. on first gtranslator startup)
		 *  set up a really foo'sh hash table with only one entry: "gtranslator" .-)
		 */
		g_hash_table_insert(gtranslator_learn_buffer->hash,
			g_strdup("gtranslator"), g_strdup("gtranslator"));
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
	
	gchar		*serial_string;
	
	g_return_if_fail(gtranslator_learn_buffer->doc!=NULL);
	g_return_if_fail(gtranslator_learn_buffer->filename!=NULL);
	g_return_if_fail(gtranslator_learn_buffer->init_status==TRUE);

	/*
	 * Create the XML document.
	 */
	gtranslator_learn_buffer->doc=xmlNewDoc("1.0");

	/*
	 * Set up the main <umtf> document root node and set it's version attribute.
	 */
	root_node=xmlNewDocNode(gtranslator_learn_buffer->doc, NULL, "umtf", NULL);
	xmlSetProp(root_node, "version", "0.6");
	xmlDocSetRootElement(gtranslator_learn_buffer->doc, root_node);

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
	 * Set the UMTF date string for our internal GtrLearnBuffer.
	 */
	gtranslator_learn_buffer_set_umtf_date();
	g_return_if_fail(gtranslator_learn_buffer->serial_date!=NULL);

	/*
	 * Build the serial string -- the serial number must be in gchar form to be
	 *  written by xmlSetProp.
	 */
	if(gtranslator_learn_buffer->serial > 1)
	{
		serial_string=g_strdup_printf("%i", gtranslator_learn_buffer->serial);
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
	g_free(serial_string);
	
	/*
	 * Clean up the hash table we're using, write it's contents, free them and destroy
	 *  the hash table.
	 */
	gtranslator_learn_buffer->current_node=serial_node;
	
	g_hash_table_foreach(gtranslator_learn_buffer->hash, 
		(GHFunc) gtranslator_learn_buffer_write_hash_entry, NULL);
	
	g_hash_table_foreach(gtranslator_learn_buffer->hash, 
		(GHFunc) gtranslator_learn_buffer_free_hash_entry, NULL);
	
	g_hash_table_destroy(gtranslator_learn_buffer->hash);

	/*
	 * Save the file and free all our used elements.
	 */
	xmlSaveFile(gtranslator_learn_buffer->filename, gtranslator_learn_buffer->doc);
	xmlFreeDoc(gtranslator_learn_buffer->doc);
	xmlFreeNode(gtranslator_learn_buffer->current_node);

	g_free(gtranslator_learn_buffer->serial_date);
	g_free(gtranslator_learn_buffer->filename);
	g_free(gtranslator_learn_buffer);
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
	found_string=(gchar *) g_hash_table_lookup(gtranslator_learn_buffer->hash, 
		(gconstpointer) search_string);

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
