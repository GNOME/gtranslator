/*
 * (C) 2001 	Fatih Demir <kabalak@gmx.net>
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

#include "htmlizer.h"

/*
 * Returns a html'ified string of the 'original' textstring.
 */
gchar *gtranslator_htmlizer(gchar *textstring)
{
	GString *string;
	gint len=0, pif;

	/*
	 * A macro which easifies our work and saves much time.
	 */
	#define EndHtml(x) \
	string=g_string_append_c(string, textstring[x]); \
	string=g_string_append(string, "</font>");
	
	g_return_val_if_fail(textstring!=NULL, NULL);

	len=strlen(textstring);
	
	/*
	 * Create the new GString.
	 */ 
	string=g_string_new("<html><body>");

	/*
	 * Parse the single string characters.
	 */ 
	for(pif=0;pif < len; ++pif)
	{
		switch(textstring[pif])
		{
			/*
			 * Figures:
			 */ 
			 case '0':
			 case '1':
			 case '2':
			 case '3':
			 case '4':
			 case '5':
			 case '6':
			 case '7':
			 case '8':
			 case '9':
				 
				string=g_string_append(string,
					"<font color=\"orange\">");
				EndHtml(pif);
				break;
		
			/*
			 * Format paremeters & specifiers:
			 */ 
			 case '%':
				
				string=g_string_append(string,
					"<font color=\"red\">%");
				EndHtml(pif+1);
				pif++;
				break;
			
			/*
			 * Underscore; the usual hotkey specifier for GNOME.
			 */ 
			 case '_':
				
				string=g_string_append(string,
					"<font color=\"blue\">_");
				EndHtml(pif+1);
				pif++;
				break;
			
			/*
			 * Free space indicators.
			 */
			 case ' ':
			 case '·':
				
				string=g_string_append(string,
					"<font color=\"yellow\">");
				EndHtml(pif);	
				break;
		
			/*
			 * Punctuation characters.
			 */
			 case '.':
			 case ':':
			 case ',':
			 case ';':
			 case '!':
			 case '?':
				
				string=g_string_append(string,
					"<font color=\"grey\">");	
				EndHtml(pif);
				break;
			
			/*
			 * Text marker characters.
			 */
			 case '"':
			 case '\'':
			 case '`':
				
				string=g_string_append(string,
					"<font color=\"silver\">");
				EndHtml(pif);
				break;
				
			default:
				string=g_string_append_c(string,
					textstring[pif]);
				break;	
		}
	}

	string=g_string_append(string, "</body></html>");
	
	return string->str;
}
