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

#include "../languages.h"
#include "semerkent.h"

G_BEGIN_DECLS

/*
 * The internally used static variables.
 */
static gint 		init_loops=0;
static GHashTable	*language_codes_hash=NULL;
static GHashTable	*language_encodings_hash=NULL;

/*
 * Creates/Destroys the internally used hashtables for the languages names etc.
 */
static void semerkent_init_language_hashes(void);
static void semerkent_destroy_language_hashes(void);
static void semerkent_hash_free_function(gpointer key, 
	gpointer value, gpointer most_useless);

/*
 * Free the values.
 */
static void semerkent_hash_free_function(gpointer key, 
        gpointer value, gpointer most_useless)
{
	semer_utils_free(key);
	semer_utils_free(value);
}

/*
 * Create the internal hash for the languages names etc.
 */
static void semerkent_init_language_hashes()
{
	/*
	 * Don't recreate the hash -- only create it once.
	 */
	if(!language_codes_hash)
	{
		gint	no=0;
		
		language_codes_hash=g_hash_table_new(g_str_hash, g_str_equal);
		language_encodings_hash=g_hash_table_new(g_str_hash, g_str_equal);
		
		while(languages[no].name!=NULL)
		{
			if(languages[no].locale)
			{
				g_hash_table_insert(language_codes_hash,
					g_strdup(languages[no].name),
					g_strdup(languages[no].locale));
			}

			if(languages[no].encoding)
			{
				g_hash_table_insert(language_encodings_hash,
					g_strdup(languages[no].name),
					g_strdup(languages[no].encoding));
			}
			
			no++;
		}
	}
}

/*
 * Destroy the created hashes on shutdown.
 */
static void semerkent_destroy_language_hashes()
{
	if(language_codes_hash)
	{
		g_hash_table_foreach(language_codes_hash, 
			(GHFunc) semerkent_hash_free_function, NULL);
		
		g_hash_table_destroy(language_codes_hash);
	}

	if(language_encodings_hash)
	{
		g_hash_table_foreach(language_encodings_hash,
			(GHFunc) semerkent_hash_free_function, NULL);
		
		g_hash_table_destroy(language_encodings_hash);
	}
}

/*
 * Load some default stuff to initialize the Semerkent library.
 */
void semerkent_init()
{
	/*
	 * Don't initialize Semerkent more then once.
	 */
	if(init_loops>=1)
	{
		return;
	}
	else
	{
		init_loops++;
		
		semerkent_init_language_hashes();
	}
}

void semerkent_shutdown()
{
	/*
	 * If we hadn't been initialized yet, don't shut down anything .-)
	 */
	if(init_loops<=0)
	{
		return;
	}
	else
	{
		init_loops--;
		
		semerkent_destroy_language_hashes();
	}
}

/*
 * Simple internal status->return value transition.
 */
gboolean semerkent_is_initialized()
{
	if(init_loops<=0)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

G_END_DECLS
