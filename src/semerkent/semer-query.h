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

#ifndef SEMER_QUERY_H
#define SEMER_QUERY_H 1

#include <glib.h>

G_BEGIN_DECLS

/*
 * The query structure.
 */
typedef struct
{
	gchar	*query;
	gchar	*language;
	gchar	*context;
} SemerQuery;

#define SEMER_QUERY(x) ((SemerQuery *) x)
#define IS_SEMER_QUERY(x) (sizeof(x) == sizeof(SemerQuery *))

/*
 * Standard functions:
 */
SemerQuery *semer_query_new(const gchar *query_string);
void semer_query_free(SemerQuery *query);

G_END_DECLS
	
#endif /* SEMER_QUERY_H */
