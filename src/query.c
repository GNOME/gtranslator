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

#include "query.h"
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
GtrQueryResult *gtranslator_query_simple(GtrQuery *query)
{
	gchar *str;

	g_return_val_if_fail(query!=NULL, NULL);
	g_return_val_if_fail(query->domain!=NULL, NULL);
	g_return_val_if_fail(query->message!=NULL, NULL);

	query->language=setup_language(query->language);
	
	setlocale(LC_ALL, query->language);

	str=dgettext(query->domain, query->message);

	if(strcmp(str, query->message))
	{
		GtrQueryResult *result=g_new(GtrQueryResult, 1);

		result->translation=str;
		result->domain=query->domain;

		return result;
	}
	else
	{
		return NULL;
	}
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
			gchar *taillanguage=g_strdup(lang);
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

			g_string_free(language, 1);
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
	GtrQueryResult *result;
	
	g_return_val_if_fail(domainlist!=NULL, NULL);
	
	/*
	 * Hm, does the list consist out of gchars or not; test it here.
	 */ 
	if(sizeof(domainlist->data)!=sizeof(gchar *))
	{
		return NULL;
	}

	query=g_new(GtrQuery, 1);
	
	while(domainlist)
	{
		/* FIXME: these are never freed */
		query->domain=g_strdup(domainlist->data);
		query->message=g_strdup(message);
		query->language=g_strdup(language);
		
		result=gtranslator_query_simple(query);

		/*
		 * Only add the result to the list if it's a translation
		 *  ever -- dgettext returns the queried string if it didn't
		 *   find anything for it, so that we should drop that cases.
		 */   
		if(strcmp(result->translation, query->message))
		{
			matches=g_list_append(matches, result->translation);
		}

		domainlist=domainlist->next;
	}

	g_free(query);
	return matches;
}

/*
 * Return a list of all domains in the given directory.
 */ 
GList *gtranslator_query_domains(const gchar *directory)
{
	struct dirent *entry;
	DIR *dir;
	GList *domains=NULL;
		
	g_return_val_if_fail(directory!=NULL, NULL);
	
	dir=opendir(directory);
	
	if(!dir)
	{
		g_warning(_("Couldn't open locales directory `%s'!"), directory);
		return NULL;
	}

	/*
	 * Get all the entries in the directory -- but first strip out the .mo
	 *  extension.
	 */ 
	while((entry=readdir(dir))!=NULL)
	{
		/*
		 * Don't use non-gettext objects.
		 */ 
		if((strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
			&& (	strstr(entry->d_name, ".mo") ||
				strstr(entry->d_name, ".gmo")))
		{
			gchar *domainname;

			domainname=gtranslator_strip_out(entry->d_name);

			/*
			 * Check the pure filename before adding it to the
			 *  domains' list.
			 */
			if(domainname)
			{
				domains=g_list_append(domains, domainname);
			}
		}
	}

	closedir(dir);

	domains=g_list_sort(domains, (GCompareFunc) strcmp);

	return domains;
}

/*
 * Strip the filename to get a "raw" enough filename.
 */ 
gchar *gtranslator_strip_out(gchar *filename)
{
	GString *o=g_string_new("");
	gint count=0;

	filename=g_basename(filename);

	/*
	 * Strip all extensions after the _first_ point.
	 */ 
	while(filename[count]!='.')
	{
		o=g_string_append_c(o, filename[count]);

		count++;
	}

	if(o->len > 0)
	{
		return o->str;
	}
	else
	{
		return NULL;
	}

	g_string_free(o, 1);
}
