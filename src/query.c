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

/*
 * A simply query method (wraps dgettext).
 */
gchar *gtranslator_query_simple(const gchar *domain, const char *message)
{
	GString *str=g_string_new("");
	
	g_return_val_if_fail(domain!=NULL, NULL);
	g_return_val_if_fail(message!=NULL, NULL);

	str=g_string_append(str, dgettext(domain, message));

	if(str->len > 0)
	{
		return str->str;
	}
	else
	{
		return NULL;
	}

	g_string_free(str, 1);
}
