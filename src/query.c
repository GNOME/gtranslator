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

#include "actions.h"
#include "gui.h"
#include "message.h"
#include "prefs.h"
#include "query.h"
#include "utils.h"

#include <libgnome/gnome-i18n.h>
#include <libgnome/gnome-util.h>

#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <dirent.h>

/*
 * Setup the real language names ala "tr_TR" to get the localized values
 *  for the given "halfwise" language name.
 */
gchar *setup_language(gchar *lang);

/*
 * A simply query method (wraps dgettext).
 */
GtrQuery *gtranslator_query_simple(GtrQuery *query)
{
	gchar *str;
	gchar *original_LC_CTYPE, *original_LC_MESSAGES;

	query->language=setup_language(query->language);
	
	/*
	 * Rescue the current locales.
	 */
	original_LC_MESSAGES=setlocale(LC_MESSAGES, "");
	original_LC_CTYPE=setlocale(LC_CTYPE, "");

	/*
	 * Query the under the preferences' language.
	 */
	setlocale(LC_ALL, query->language);
	
	str=dgettext(query->domain, query->message);

	/*
	 * And now reset the locales to the previous settings.
	 */
	setlocale(LC_MESSAGES, original_LC_MESSAGES);
	setlocale(LC_CTYPE, original_LC_CTYPE);

	/*
	 * Only setup a result if the result from the dgettext()-based
	 *  query is not the same as the previous message.
	 */
	if(str && strcmp(str, query->message))
	{
		GtrQuery *result=gtranslator_new_query(
			query->domain, str, query->language);

		return result;
	}
	else
	{
		return NULL;
	}

	g_free(str);
	g_free(original_LC_CTYPE);
	g_free(original_LC_MESSAGES);
	gtranslator_free_query(&query);
}

/*
 * Just accomplish the given language name to it's full beautifulness.
 */
gchar *setup_language(gchar *lang)
{
	g_return_val_if_fail(lang!=NULL, NULL);

	/*
	 * If the language name does already include an underscore it will
	 *  be surely a complete language name.
	 */  
	if(strchr(lang, '_'))
	{
		return lang;
	}
	else
	{
		/*
		 * Longer language names should also be Ok.
		 */ 
		if(strlen(lang) > 2)
		{
			return lang;
		}
		else
		{
			gchar *taillanguage=lang;
			GString *language=g_string_new(lang);
			
			g_strup(taillanguage);

			language=g_string_down(language);

			language=g_string_append_c(language, '_');
			language=g_string_append(language, taillanguage);

			if(language->len > 0)
			{
				return language->str;
			}
			else
			{
				return NULL;
			}

			g_string_free(language, FALSE);
		}
	}
}

/*
 * Return a list of all matching messages from the domainlist.
 */
GList *gtranslator_query_list(GList *domainlist, const gchar *message,
	gchar *language)
{
	GList *matches=NULL;
	GtrQuery *query;
	GtrQuery *result;
	
	g_return_val_if_fail(domainlist!=NULL, NULL);
	
	/*
	 * Hm, does the list consist out of gchars or not; test it here.
	 */ 
	if(sizeof(domainlist->data)!=sizeof(gchar *))
	{
		return NULL;
	}

	while(domainlist)
	{
		query=gtranslator_new_query(domainlist->data,
			message, language);
		
		result=gtranslator_query_simple(query);

		/*
		 * Only add the result to the list if it's a translation
		 *  ever -- dgettext returns the queried string if it didn't
		 *   find anything for it, so that we should drop that cases.
		 */   
		if(strcmp(result->message, query->message))
		{
			matches=g_list_append(matches, result);
		}

		domainlist=domainlist->next;

		gtranslator_free_query(&query);
	}

	return matches;
}

/*
 * Sets up the list of domains.
 */ 
void gtranslator_query_domains(const gchar *directory)
{
	gchar *localedirectory;
		
	g_return_if_fail(directory!=NULL);

	if(!lc)
	{
		g_warning(_("No language defined in preferences to query the domains for!"));

		return;
	}

	localedirectory=g_strdup_printf("%s/%s/LC_MESSAGES", directory,
		lc);
	
	domains=gtranslator_utils_file_names_from_directory(localedirectory,
		".mo", TRUE, TRUE);
}

/*
 * Create a new GtrQuery with the given values.
 */
GtrQuery *gtranslator_new_query(const gchar *domain,
	const gchar *message, const gchar *language)
{
	GtrQuery *query=g_new0(GtrQuery, 1);

	g_return_val_if_fail(message!=NULL, NULL);
	g_return_val_if_fail(domain!=NULL, NULL);
	g_return_val_if_fail(language!=NULL, NULL);

	query->message=g_strdup(message);
	query->language=g_strdup(language);
	query->domain=g_strdup(domain);

	return query;
}

/*
 * Freeing function:
 */
void gtranslator_free_query(GtrQuery **query)
{
	if(*query)
	{
		g_free((*query)->message);
		g_free((*query)->domain);
		g_free((*query)->language);
		g_free(*query);
	}
}

/*
 * Get all empty msgstr's filled with found msgstr's from the main domain.
 */
void gtranslator_query_gtr_msg(gpointer data, gpointer yeah)
{
	GtrMsg *msg=GTR_MSG(data);

	if(msg && msg->msgid && !msg->msgstr)
	{
		GtrQuery *query;
		GtrQuery *matchingtranslation=NULL;

		/*
		 * Build up the query for the selected default domain and for the
		 *  current msgid.
		 */
		query=gtranslator_new_query(wants.defaultdomain, msg->msgid, lc);

		/*
		 * Get a possible translation for the query and free up the used
		 *  GtrQuery.
		 */
		matchingtranslation=gtranslator_query_simple(query);
		gtranslator_free_query(&query);

		/*
		 * If we did find a matching translation for the msgid and there's
		 *  no msgstr translation yet, copy the found query result into
		 *   the msgstr field of the GtrMsg and free the GtrQuery.
		 */
		if(matchingtranslation && matchingtranslation->message)
		{
			msg->msgstr=g_strdup(matchingtranslation->message);

			msg->status |= GTR_MSG_STATUS_TRANSLATED;

			/*
			 * GUI updates which should be done locally in here.
			 */
			po->file_changed=TRUE;

			gtranslator_free_query(&matchingtranslation);
		}
	}
}

/*
 * Simply execute the gtranslator_query_gtr_msg for every message in the
 *  po file.
 */
void gtranslator_query_accomplish()
{
	g_list_foreach(po->messages, (GFunc) gtranslator_query_gtr_msg, NULL);

	/*
	 * Activate the Save menu/tollbar items on changes.
	 */
	if(po->file_changed)
	{
		gtranslator_actions_enable(ACT_SAVE);
	}
	
	gtranslator_message_show(po->current);
	gtranslator_get_translated_count();
}
