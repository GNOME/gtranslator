/*
 * (C) 2001		Fatih Demir <kabalak@gtranslator.org>
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

#include "syntax.h"
#include "parse.h"
#include "gui.h"

#include <ctype.h>
#include <string.h>

#include <gtk/gtktext.h>

/*
 * The keywords table.
 */
static gchar *keywords[] = {
	"CVS",
	"FDL",
	"FIXME",
	"Gdk",
	"Glib",
	"GNU",
	"GNOME",
	"Gnome",
	"GPL",
	"Gtk+",
	"KDE",
	"Kde",
	"NULL",
	"URI",
	"URL",
	"URN",
	"X11",
	"WWW",
	NULL
};

/*
 * The URL/URI prefixes table:
 */
static gchar *prefixes[] =  {
	"file:",
	"ftp:",
	"ghelp:",
	"gnome-search:",
	"https:",
	"http:",
	"info:",
	"man:",
	"medusa:",
	"search:",
	NULL
};

/*
 * Determine if the current given message contains any format specifier
 *  in msgid/msgstr parts.
 */  
gboolean gtranslator_syntax_get_format(GtrMsg *msg);

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
		gtk_text_backward_delete(GTK_TEXT(textwidget), \
			strlen(x)-1); \
		string=g_string_append(string, x);

	/*
	 * An easifying macro for the new "back_match"function.
	 */
	#define match(x) (back_match(msg, x, cp))

	/**********************************************************************/
	
	GString *string=g_string_new("");
	GdkColor *color;
	
	gboolean aInserted=FALSE;
	
	gint cp;
	gint z=0, mindex=0;
	
	g_return_if_fail(textwidget!=NULL);

	if(!msg)
	{
		return;
	}

	mindex=gtk_editable_get_position(GTK_EDITABLE(textwidget));
	
	gtk_text_freeze(GTK_TEXT(textwidget));

	for(cp=0; cp < strlen(msg); ++cp)
	{
		/*
		 * Highlight the found elements in this switch tree.
		 */
		switch(msg[cp])
		{
			/*
			 * Hotkeys and comment characters:
			 */ 
			case '_':
				clear_string(string);

				if(msg[cp] && msg[cp+1] && isalpha(msg[cp+1]))
				{
					append_char(string, msg[cp]);
					append_char(string, msg[cp+1]);
					
					cp++;
				}
				else
				{
					append_char(string, msg[cp]);
				}
				
				color = get_color_from_type(COLOR_HOTKEY);

				break;
		
			/*
			 * Format specifiers:
			 */
			case '%':
				clear_string(string);

				if(msg[cp+1] && msg[cp+1]=='l' && msg[cp+2])
				{
					append_char(string, msg[cp]);
					append_char(string, msg[cp+1]);
					append_char(string, msg[cp+2]);
					
					cp=cp+2;
				}
				else
				{
					if(msg[cp] && msg[cp+1])
					{
						append_char(string, msg[cp]);
						append_char(string, msg[cp+1]);
						
						cp++;
					}
					else
					{
						append_char(string, msg[cp]);
					}
				}

				color = get_color_from_type(COLOR_C_FORMAT);

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
				clear_string(string);
				
				append_char(string, msg[cp]);
				
				color = get_color_from_type(COLOR_NUMBER);

				break;

			/*
			 * "Special characters":
			 */
			case '·':
				clear_string(string);

				append_char(string, msg[cp]);

				color = get_color_from_type(COLOR_SPECIAL_CHAR);

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
				clear_string(string);

				append_char(string, msg[cp]);
				
				color = get_color_from_type(COLOR_PUNCTUATION);

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
				clear_string(string);

				append_char(string, msg[cp]);
				
				color = get_color_from_type(COLOR_SPECIAL);
				
				break;

			/*
			 * URL/URI prefixes:
			 */
			case ':':
				clear_string(string);
				
				z=0;
				
				while(prefixes[z]!=NULL)
				{
					if(match(prefixes[z]))
					{
						aInserted=TRUE;
						
						color=get_color_from_type(
							COLOR_ADDRESS);

						string_add(prefixes[z]);
					}
					
					z++;
				}
				
				if(aInserted!=TRUE)
				{
					append_char(string, msg[cp]);

					color=get_color_from_type(
						COLOR_PUNCTUATION);
				}
				
				
				break;
			
			/*
			 * Everything else:
			 */ 
			default:
				clear_string(string);
				
				color=NULL;
			
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
						
						color=get_color_from_type(COLOR_KEYWORD);
					}
					
					z++;
				}
				
				/*
				 * Insert the single normal characters if there couldn't be
				 *  any keyword found.
				 */
				if(aInserted!=TRUE)
				{
					append_char(string, msg[cp]);
				}

				break;
		}

		gtk_text_insert(GTK_TEXT(textwidget),
			NULL, color, NULL,
			string->str, -1);
	}

	gtk_text_thaw(GTK_TEXT(textwidget));

	if(mindex >= 0 && mindex < gtk_text_get_length(GTK_TEXT(textwidget)))
	{
		gtk_editable_set_position(GTK_EDITABLE(textwidget), mindex);
	}

	g_string_free(string, FALSE);
}

/*
 * Update the syntax in the given GtkText widget.
 */ 
void gtranslator_syntax_update_text(GtkWidget *textwidget)
{
	GString *str;
	gchar *text;
	
	g_return_if_fail(textwidget!=NULL);

	text=gtk_editable_get_chars(GTK_EDITABLE(textwidget), 0, -1);
	str=g_string_new(text);
	g_free(text);

	if(str->len > 0)
	{
		gint pos;
		
		pos=gtk_editable_get_position(GTK_EDITABLE(textwidget));
		
		gtk_text_freeze(GTK_TEXT(textwidget));

		gtk_editable_delete_text(GTK_EDITABLE(textwidget), 0, -1);

		gtranslator_syntax_insert_text(textwidget, str->str);

		gtk_text_thaw(GTK_TEXT(textwidget));

		if(pos >= 0 && pos <= gtk_text_get_length(GTK_TEXT(textwidget)))
		{
			gtk_editable_set_position(
				GTK_EDITABLE(textwidget), pos);
		}
	}

	g_string_free(str, FALSE);
}

/*
 * Check if the given message does contain any format specifiers.
 */ 
gboolean gtranslator_syntax_get_format(GtrMsg *msg)
{
	g_return_val_if_fail(msg!=NULL, FALSE);

	/*
	 * Simply determine if there is any '%' character in the msgid
	 *  and if existent in the msgstr of the message.
	 */  
	if(strchr(GTR_MSG(msg)->msgid, '%'))
	{
		return TRUE;
	}
	if(GTR_MSG(msg)->msgstr &&
		strchr(GTR_MSG(msg)->msgstr, '%'))
	{
		return TRUE;
	}

	return FALSE;
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
