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

#include <stdio.h>

#include <ctype.h>
#include <string.h>

/*
 * The internal formats checking function.
 */
static void gtranslator_formats_fill_check_data(const gchar *c_string, GString *string, gboolean hmpf);

/*
 * Do the hard traversing work -- formats are hard to grep ,-)
 */
static void gtranslator_formats_fill_check_data(const gchar *c_string, GString *string, gboolean hmpf)
{
	if(!c_string)
	{
		return;
	}

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

		/*
		 * Get the characters after the first '%'.
		 */
		tuz=nautilus_str_get_after_prefix(c_string, "%");

		/*
		 * Return if there's no '%' in the string.
		 */
		if(!tuz) 
		{
			return;
		}
		else
		{
			/*
			 * Or go one character forward -- the '%' is left behind now.
			 */
			tuz++;
		}

		/*
		 * Iterate through the characters.
		 */
		while(*tuz)
		{
			/*
			 * Quite normal format characters.
			 */
			if(!isspace(*tuz) && !isdigit(*tuz) && !ispunct(*tuz))
			{
				string=g_string_append_c(string, *tuz);
			}
			else if(*tuz=='%')
			{
				/*
				 * An "escaped" % ("%%").
				 */
				string=g_string_append_c(string, *tuz);
			}
			else if(isdigit(*tuz))
			{
				gchar	*restformat;

				restformat=nautilus_str_get_after_prefix(tuz, "$");

				/*
				 * Only try our %POS$FORMAT matching if "hmpf" is TRUE.
				 *  (msgid's shouldn't contain any formats, so we don't 
				 *    check for weird things there).
				 */
				if(!restformat || !hmpf)
				{
					/*
					 * If there's no further '$' sign, then this must
					 *  be something like %3 etc. which is used by KDE for
					 *   some special markup in their po files.
					 */
					string=g_string_append_c(string, *tuz);
					
					continue;
				}
				else
				{
					gint	position;
					GString	*position_string=g_string_new("");
					
					/*
					 * Append the current format character to our position_string.
					 */
					restformat++;
					position_string=g_string_append_c(position_string, *tuz);
					
					/*
					 * "Read" the format prefix number for ordering.
					 */
					sscanf(position_string->str, "%i", &position);

					/*
					 * Check the string length for brevity.
					 */
					if(position <= 0)
					{
						return;
					}
					else if(position > string->len)
					{
						gint	length=string->len;

						while(length <= position)
						{
							string=g_string_append(string, " ");
							length++;
						}
					}

					/*
					 * Insert the format character at the right position.
					 */
					string=g_string_insert_c(string, position--, *restformat);
					g_string_free(position_string, TRUE);
				}
			}
			else if(*tuz=='-')
			{
				/*
				 * Ignore any "%-s" formed, special align formats' '-' symbol.
				 */
				continue;
			}
			else if(isspace(*tuz) || ispunct(*tuz))
			{
				/*
				 * Search if there are any other '%'s in the resting
				 *  string.
				 */
				tuz=nautilus_str_get_after_prefix(tuz, "%");

				/*
				 * Sanity check and return if nothing is left to do.
				 */
				if(!tuz)
				{
					return;
				}
			}

			tuz++;
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
	gtranslator_formats_fill_check_data(GTR_MSG(message)->msgid, id_formats, FALSE);
	gtranslator_formats_fill_check_data(GTR_MSG(message)->msgstr, str_formats, TRUE);

	/*
	 * Check the length's of the two GString's for equality.
	 */
	if(id_formats->len!=str_formats->len)
	{
		return FALSE;
	}
	else
	{
		gint	i=0;
		gchar	*id, *str;

		g_return_val_if_fail(id_formats->str!=NULL, FALSE);
		g_return_val_if_fail(str_formats->str!=NULL, FALSE);

		id=g_strstrip(id_formats->str);
		str=g_strstrip(str_formats->str);

		for(i=(strlen(id) - 1); i >= 0; --i)
		{
			if(id[i] && str[i] && id[i]!=str[i])
			{
				return FALSE;
			}
		}
	}

	g_string_free(id_formats, TRUE);
	g_string_free(str_formats, TRUE);

	return TRUE;
}
