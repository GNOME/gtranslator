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

#include <ctype.h>
#include <string.h>
#include <locale.h>

/*
 * Setup the real language names ala "tr_TR" to get the localized values
 *  for the given "halfwise" language name.
 */
void setup_language(char **lang);

/*
 * A simply query method (wraps dgettext).
 */
gchar *gtranslator_query_simple(const gchar *domain, const char *message,
	gchar *language)
{
	GString *str=g_string_new("");

	g_return_val_if_fail(language!=NULL, NULL);
	g_return_val_if_fail(domain!=NULL, NULL);
	g_return_val_if_fail(message!=NULL, NULL);

	setup_language(&language);
	
	setlocale(LC_ALL, language);

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

void setup_language(char **lang)
{
	g_return_if_fail(*lang!=NULL);

	/*
	 * If the language name does already include an underscore it will
	 *  be surely a complete language name.
	 */  
	if(strchr(*lang, '_'))
	{
		return;
	}
	else
	{
		/*
		 * Longer language names should also be Ok.
		 */ 
		if(strlen(*lang) > 2)
		{
			return;
		}
		else
		{
			gchar *newlang;
			gchar tail[1];
			#define assign(x, y, z); \
			if(*x[z] && *x[z]!='\0' && isupper(*x[z])) \
			{ \
				y[z]=*x[z]; \
				tolower(*x[z]); \
			} \
			else \
			{ \
				y[z]=toupper(*x[z]); \
			}

			assign(lang, tail, 0);
			assign(lang, tail, 1);

			g_message("0+1 done");

			newlang=*lang;

			sprintf(*lang, "%s_%c%c", newlang,
				tail[0], tail[1]);

			g_free(newlang);
		}
	}
}
