/*
 * (C) 2001		Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
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

#include "color-schemes.h"
#include "gui.h"
#include "parse.h"
#include "prefs.h"
#include "syntax.h"
#include "syntax-elements.h"
#include "utils.h"

#include <ctype.h>
#include <string.h>

#include <gtk/gtktext.h>

/*
 * Return if the given string matches our last chars.
 */
gboolean back_match(const gchar *msg, gchar *str, gint pos);

/*
 * Insert the syntax highlighted text into the given text widget.
 */ 
void gtranslator_syntax_insert_text(GtkWidget *textwidget, const gchar *msg)
{
	/************************* MACROS *************************************/
	
	/*
	 * Useful macros for making the text easier to understand/write.
	 */
	#define clear_string(x) x=g_string_truncate(x, 0)
	#define append_char(x, y) x=g_string_append_c(x, y)
	
	/*
	 * Delete the previous characters from the string and readd it
	 *  to the text box.
	 */
	#define string_add(x); \
		if(gtk_text_get_length(GTK_TEXT(textwidget)) > (strlen(x)-1)); \
		{ \
			gtk_text_backward_delete(GTK_TEXT(textwidget), \
				strlen(x)-1); \
			string=g_string_append(string, x); \
		}

	/*
	 * An easifying macro for the new "back_match"function.
	 */
	#define match(x) (back_match(msg, x, cp))

	/**********************************************************************/
	
	GString 	*string=g_string_new("");
	GdkColor 	*color=NULL;
	GdkColor 	*text_bg_color=NULL;
	
	gboolean 	aInserted;
	gchar 		specialchar;
	
	gint 		cp;
	gint 		z=0;
	
	g_return_if_fail(textwidget!=NULL);

	if(!msg)
	{
		return;
	}

	if(GtrPreferences.dot_char)
	{
		specialchar=_("·")[0];
	}
	else
	{
		specialchar=' ';
	}

	for(cp=0; cp < strlen(msg); ++cp)
	{
		clear_string(string);

		/*
		 * Highlight the found elements in this switch tree.
		 */
		switch(msg[cp])
		{
			/*
			 * Hotkeys and comment characters:
			 */ 
			case '_':
				append_char(string, '_');
				
				if(msg[cp+1] && isalpha(msg[cp+1]))
				{
					append_char(string, msg[cp+1]);
					cp++;
				}
				
				color = gtranslator_get_color_from_type(COLOR_HOTKEY);

				break;
		
			/*
			 * Format specifiers:
			 */
			case '%':
				append_char(string, msg[cp]);
				cp++;
			
				while(msg[cp] && msg[cp]!=specialchar &&
					!ispunct(msg[cp]) && !iscntrl(msg[cp]))
				{
					append_char(string, msg[cp]);
					cp++;
				}

				cp--;
				
				color = gtranslator_get_color_from_type(COLOR_C_FORMAT);

				break;
				
			/*
			 * Figures:
			 */ 
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '0':
				append_char(string, msg[cp]);
				
				color = gtranslator_get_color_from_type(COLOR_NUMBER);

				break;

			/*
			 * Punctuation characters:
			 */
			case '.':
			case ';':
			case ',':
			case '!':
			case '?':
			case '-':
				append_char(string, msg[cp]);
				
				color = gtranslator_get_color_from_type(COLOR_PUNCTUATION);

				break;
			
			/*
			 * Quotation characters and "special" characters:
			 */
			case '"':
			case '\'':
			case '`':
			case '(':
			case ')':
			case '[':
			case ']':
			case '{':
			case '}':
			case '<':
			case '>':
			case '&':
			case '@':
			case '$':
			case '#':
			case '/':
			case '\\':
			case '|':
				append_char(string, msg[cp]);
				
				color = gtranslator_get_color_from_type(COLOR_SPECIAL);
				
				break;

			/*
			 * URL/URI prefixes:
			 */
			case ':':
				z=0;
				aInserted=FALSE;
				
				while(prefixes[z]!=NULL)
				{
					if(match(prefixes[z]))
					{
						aInserted=TRUE;
						
						color=gtranslator_get_color_from_type(
							COLOR_ADDRESS);

						string_add(prefixes[z]);
						break;
					}
					
					z++;
				}
				
				if(aInserted==FALSE)
				{
					append_char(string, msg[cp]);

					color=gtranslator_get_color_from_type(
						COLOR_PUNCTUATION);
				}
				
				
				break;
			
			/*
			 * Everything else:
			 */ 
			default:
				aInserted=FALSE;
			
				z=0;
				
				/*
				 * Cruise through the keywords list and check for any
				 *  match.
				 */
				while(keywords[z]!=NULL)
				{
					if(match(keywords[z]))
					{
						aInserted=TRUE;

						string_add(keywords[z]);
						
						color=gtranslator_get_color_from_type(COLOR_KEYWORD);
						break;
					}
					
					z++;
				}
				
				/*
				 * Insert the single normal characters if there couldn't be
				 *  any keyword found.
				 */
				if(aInserted==FALSE)
				{
					/*
					 * Do we have got a "special character"?
					 */
					if(msg[cp]==specialchar)
					{
						color=gtranslator_get_color_from_type(COLOR_SPECIAL_CHAR);
						append_char(string, msg[cp]);
					}
					else
					{
						color=NULL;
						append_char(string, msg[cp]);
					}
				}

				break;
		}

		/*
		 * Load the background color for the text area -- it's not necessarily existent.
		 */
		text_bg_color=gtranslator_get_color_from_type(COLOR_TEXT_BG);

		if(theme->text_bg)
		{
			gtk_text_insert(GTK_TEXT(textwidget), NULL,
				color, text_bg_color, string->str, -1);
		}
		else
		{
			gtk_text_insert(GTK_TEXT(textwidget), NULL,
				color, NULL, string->str, -1);
		}
	}

	g_string_free(string, TRUE);
}

/*
 * Update the syntax in the given GtkText widget.
 */ 
void gtranslator_syntax_update_text(GtkWidget *textwidget)
{
	gchar *text;
	
	g_return_if_fail(textwidget!=NULL);

	text=gtk_editable_get_chars(GTK_EDITABLE(textwidget), 0, -1);

	nothing_changes=TRUE;
	if(text && text[0]!='\0')
	{
		gint pos=gtk_editable_get_position(GTK_EDITABLE(textwidget));

		gtk_text_freeze(GTK_TEXT(textwidget));

		gtk_editable_delete_text(GTK_EDITABLE(textwidget), 0, -1);
		gtranslator_syntax_insert_text(textwidget, text);
		
		gtk_text_thaw(GTK_TEXT(textwidget));

		gtk_editable_set_position(GTK_EDITABLE(textwidget), pos);
	}
	nothing_changes=FALSE;

	GTR_FREE(text);
}

/*
 * Check the given string for equivalence with the last characters.
 */
gboolean back_match(const gchar *msg, gchar *str, gint pos)
{
	gint len=0, i;

	/*
	 * Check all of our used variables for sanity.
	 */
	g_return_val_if_fail(str!=NULL, FALSE);
	g_return_val_if_fail(msg!=NULL, FALSE);

	if(pos < 0)
	{
		return FALSE;
	}

	len=strlen(str);

	/*
	 * Control every char in the original string and the given
	 *  "match" string for equality and return FALSE if this
	 *    fails.
	 */
	for(i=0; i < len; ++i)
	{
		if(!msg[pos-i] || msg[pos-i]!=str[(len-1)-i])
		{
			return FALSE;
		}
	}
	
	return TRUE;
}
