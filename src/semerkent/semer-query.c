/*
 * (C) 2001-2003 	Fatih Demir <kabalak@kabalak.net>
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
 */

#include "semer-query.h"
#include "semer-utils.h"

#include <locale.h>

G_BEGIN_DECLS

/*
 * Standard creation deletion.
 */
SemerQuery *semer_query_new(const gchar *query_string)
{
	gchar *lang_env;
	SemerQuery *query=g_new0(SemerQuery, 1);
	
	lang_env=setlocale(LC_ALL, "");

	if(query_string)
	{
		query->query=g_strdup(query_string);
	}
	else
	{
		query->query=NULL;
	}

	query->language=g_strdup("Language?!");
	query->context=g_strdup(lang_env);

	semer_utils_free(lang_env);

	return query;
}

void semer_query_free(SemerQuery *query)
{
	g_return_if_fail(SEMER_QUERY(query)!=NULL);

	semer_utils_free(SEMER_QUERY(query)->query);
	semer_utils_free(SEMER_QUERY(query)->language);
	semer_utils_free(SEMER_QUERY(query)->context);
	semer_utils_free(SEMER_QUERY(query));
}

G_END_DECLS
