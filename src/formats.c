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

#include "formats.h"
#include "messages.h"
#include "nautilus-string.h"

#include <string.h>

/*
 * The internal formats checking function.
 */
static void gtranslator_formats_fill_check_data(const gchar *c_string, GString *string);

/*
 * Do the hard traversing work -- formats are hard to grep ,-)
 */
static void gtranslator_formats_fill_check_data(const gchar *c_string, GString *string)
{
	g_return_if_fail(c_string!=NULL);

	/*
	 * Check if there are any '%' signs; if not, return.
	 */
	if(!strchr(c_string, '%'))
	{
		return;
	}
	else
	{
		gchar	*tuz;
		guint	 formats;

		formats=nautilus_str_count_characters(c_string, '%');

		if(formats <= 1)
		{
			/*
			 * Get the characters after the first '%'.
			 */
			tuz=nautilus_str_get_after_prefix(c_string, "%");

			if(*tuz=='%')
			{
				string=g_string_append_c(string, '%');
			}
		}
		else
		{
			g_message(">>%u", formats);
		}
	}
}

/*
 * Checks whether the formats are right or wrong .-)
 */
gboolean gtranslator_formats_check(GtrMsg *message)
{
	GString		*id_formats, *str_formats;
	
	g_return_val_if_fail(GTR_MSG(message)!=NULL, FALSE);
	g_return_val_if_fail(GTR_MSG(message)->msgid!=NULL, FALSE);

	/*
	 * "Initialize" the variables -- get it to be right.
	 */
	id_formats=g_string_new("");
	str_formats=g_string_new("");

	/*
	 * Set the GString's to the right format specifiers.
	 */
	gtranslator_formats_fill_check_data(GTR_MSG(message)->msgid, id_formats);
	gtranslator_formats_fill_check_data(GTR_MSG(message)->msgstr, str_formats);

	g_string_free(id_formats, TRUE);
	g_string_free(str_formats, TRUE);

	return TRUE;
}
