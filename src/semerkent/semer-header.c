/*
 * (C) 2001-2003 	Fatih Demir <kabalak@gtranslator.org>
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

#ifdef HAVE_CONFIG_H
#include "../../config.h"
#endif

#include "semer-header.h"
#include "semer-utils.h"

G_BEGIN_DECLS

/*
 * Create a new, blank SemerHeader.
 */
SemerHeader *semer_header_new()
{
	SemerHeader *header=g_new0(SemerHeader, 1);

	header->tool=NULL;
	header->tool_version=NULL;
	header->datatype=NULL;
	header->segtype=NULL;
	header->adminlang=NULL;
	header->srclang=NULL;
	header->format=NULL;
	header->encoding=NULL;
	header->creationid=NULL;
	header->creationdate=NULL;

	return header;
}

/*
 * Create a well-formed header with "good" values.
 */
SemerHeader *semer_header_new_with_defaults()
{
	SemerHeader *header=g_new0(SemerHeader, 1);

	header->tool=g_strdup("Semerkent");
	header->tool_version=g_strdup(VERSION);
	header->datatype=g_strdup("PlainText");
	header->segtype=g_strdup("sentence");
	header->adminlang=g_strdup("EN-US");
	header->srclang=g_strdup("EN");
	header->format=g_strdup("PlainText");
	header->encoding=g_strdup("ASCII");
	header->creationid=g_strdup("Semerkent");
	header->creationdate=semer_utils_tmx_date_new();

	return header;
}

/*
 * Free the given SemerHeader.
 */
void semer_header_free(SemerHeader *header)
{
	g_return_if_fail(SEMER_HEADER(header)!=NULL);

	semer_utils_free(SEMER_HEADER(header)->tool);
	semer_utils_free(SEMER_HEADER(header)->tool_version);
	semer_utils_free(SEMER_HEADER(header)->datatype);
	semer_utils_free(SEMER_HEADER(header)->segtype);
	semer_utils_free(SEMER_HEADER(header)->adminlang);
	semer_utils_free(SEMER_HEADER(header)->srclang);
	semer_utils_free(SEMER_HEADER(header)->format);
	semer_utils_free(SEMER_HEADER(header)->encoding);
	semer_utils_free(SEMER_HEADER(header)->creationid);
	semer_utils_free(SEMER_HEADER(header)->creationdate);
}

SemerHeader *semer_header_copy(SemerHeader *header)
{
	SemerHeader *copy=g_new0(SemerHeader, 1);
	
	g_return_val_if_fail(SEMER_HEADER(header)!=NULL, NULL);
	
	copy->tool=semer_utils_strcopy(SEMER_HEADER(header)->tool);
	copy->tool_version=semer_utils_strcopy(SEMER_HEADER(header)->tool_version);
	copy->datatype=semer_utils_strcopy(SEMER_HEADER(header)->datatype);
	copy->segtype=semer_utils_strcopy(SEMER_HEADER(header)->segtype);
	copy->adminlang=semer_utils_strcopy(SEMER_HEADER(header)->adminlang);
	copy->srclang=semer_utils_strcopy(SEMER_HEADER(header)->srclang);
	copy->format=semer_utils_strcopy(SEMER_HEADER(header)->format);
	copy->encoding=semer_utils_strcopy(SEMER_HEADER(header)->encoding);
	copy->creationid=semer_utils_strcopy(SEMER_HEADER(header)->creationid);
	copy->creationdate=semer_utils_strcopy(SEMER_HEADER(header)->creationdate);

	return copy;
}

G_END_DECLS
