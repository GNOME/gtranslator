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

#ifndef GTR_QUERY_H
#define GTR_QUERY_H 1

#include "parse.h"

/*
 * Quite simple structure for the queries and results.
 */
typedef struct {
	gchar *domain;
	gchar *message;
	gchar *language;
} GtrQuery;

typedef struct {
	gchar *domain;
	gchar *translation;
} GtrQueryResult;

#define GTR_QUERY(x) ((GtrQuery *)(x))
#define GTR_QUERY_RESULT(x) ((GtrQueryResult *)(x))

/*
 * The generally used domains list.
 */
GList *domains;

/*
 * Returns the non-prefixed and non-suffixed filename back.
 */
gchar *gtranslator_strip_out(gchar *filename);

/*
 * Simply query for the gettext translation for the given message in the
 *  domain (program/package) and for the given language.
 */ 
GtrQueryResult *gtranslator_query_simple(GtrQuery *query);

/*
 * Get all ocurrences for the message in the given domainlist and return a
 *  GList out of GtrQueryResults.
 */
GList *gtranslator_query_list(GList *domainlist, const gchar *message,
	gchar *language);

/*
 * Set up the "domains" GList with all domains in the given directory for
 *  the language which is specified in the preferences.
 */
void gtranslator_query_domains(const gchar *directory);

/*
 * Creation/deletion of GtrQuery(Result)s:
 */
GtrQuery *gtranslator_new_query(const gchar *domain, 
	const gchar *message, const gchar *language);

void gtranslator_free_query(GtrQuery **query);
void gtranslator_free_query_result(GtrQueryResult **result);

/*
 * Automatically query for all empty msgstr's the possible translations and
 *  insert them.
 */
void gtranslator_query_all(gpointer data, gpointer yeah);

#endif
