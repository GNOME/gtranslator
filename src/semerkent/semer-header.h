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

#ifndef SEMER_HEADER_H
#define SEMER_HEADER_H 1

#include <glib.h>

G_BEGIN_DECLS

/*
 * The abstraction header for all the translation memories.
 */
typedef struct
{
	gchar	*tool;
	gchar	*tool_version;
	
	gchar	*datatype;
	gchar	*segtype;

	gchar	*adminlang;
	gchar	*srclang;

	gchar	*format;
	gchar	*encoding;

	gchar	*creationid;
	gchar	*creationdate;
} SemerHeader;

/*
 * Simple defines.
 */
#define SEMER_HEADER(x) ((SemerHeader *) x)
#define IS_SEMER_HEADER(x) (sizeof(x) == sizeof(SemerHeader *))

/*
 * Creation/deletion of SemerHeaders.
 */
SemerHeader *semer_header_new(void);
SemerHeader *semer_header_new_with_defaults(void);
void semer_header_free(SemerHeader *header);

/*
 * Copy the given header.
 */
SemerHeader *semer_header_copy(SemerHeader *header);

G_END_DECLS
	
#endif /* SEMER_HEADER_H */
