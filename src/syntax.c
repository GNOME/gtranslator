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
#include "preferences.h"

#include <ctype.h>

/*
 * Determine if the current given message contains any format specifier
 *  in msgid/msgstr parts.
 */  
gboolean gtranslator_syntax_get_format(GtrMsg *msg);

/*
 * Insert the syntax highlighted text into the given text widget.
 */ 
void gtranslator_syntax_insert_text(GtkWidget *textwidget, const gchar *msg)
{
	#define clear_string(x) x=g_string_truncate(x, 0)
	GString *string=g_string_new("");
	GdkColor *color;
	gint cp;
	
	g_return_if_fail(textwidget!=NULL);

	if(!msg)
	{
		return;
	}

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
					string=g_string_append_c(string,
						msg[cp]);
					
					string=g_string_append_c(string,
						msg[cp+1]);
					cp++;
				}
				else
				{
					string=g_string_append_c(string,
						msg[cp]);
				}
				
				color = gtranslator_syntax_get_gdk_color(BLUE);

				break;
		
			/*
			 * Format specifiers:
			 */
			case '%':
				clear_string(string);

				if(msg[cp+1] && msg[cp+2] && msg[cp+1]=='l')
				{
					string=g_string_append_c(string,
						msg[cp]);

					string=g_string_append_c(string,
						msg[cp+1]);

					string=g_string_append_c(string,
						msg[cp+2]);

					cp=cp+2;
				}
				else
				{
					if(msg[cp] && msg[cp+1])
					{
						string=g_string_append_c(string,
							msg[cp]);

						string=g_string_append_c(string,
							msg[cp+1]);
						cp++;
					}
					else
					{
						string=g_string_append_c(string,
							msg[cp]);	
					}
				}

				color = gtranslator_syntax_get_gdk_color(RED);

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

				string=g_string_append_c(string,
					msg[cp]);

				color = gtranslator_syntax_get_gdk_color(ORANGE);

				break;
		
			/*
			 * Punctuation characters:
			 */
			case '.':
			case ':':
			case ';':
			case ',':
			case '!':
			case '?':
			case '-':
				clear_string(string);

				string=g_string_append_c(string,
					msg[cp]);
				
				color = gtranslator_syntax_get_gdk_color(BROWN);

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

				string=g_string_append_c(string,
					msg[cp]);
				
				color = gtranslator_syntax_get_gdk_color(MAROON);
				
				break;
				
			/*
			 * Everything else:
			 */ 
			default:
				clear_string(string);
				
				string=g_string_append_c(string,
					msg[cp]);
				
				color = NULL;

				break;
		}

		gtk_text_insert(GTK_TEXT(textwidget),
			NULL, color, NULL,
			string->str, -1);
	}

	gtk_text_thaw(GTK_TEXT(textwidget));
	
	g_string_free(string, 0);
}

/*
 * Update the syntax in the given GtkText widget.
 */ 
void gtranslator_syntax_update_text(GtkWidget *textwidget)
{
	GString *str=g_string_new("");
	
	g_return_if_fail(textwidget!=NULL);

	str=g_string_append(str, gtk_editable_get_chars(
		GTK_EDITABLE(textwidget), 0, -1));

	if(str->len>0)
	{
		gint pos;
		gtk_text_freeze(GTK_TEXT(textwidget));
		
		pos=gtk_editable_get_position(GTK_EDITABLE(textwidget));
		
		gtk_editable_delete_text(GTK_EDITABLE(textwidget), 0, -1);

		gtranslator_syntax_insert_text(textwidget, str->str);
		
		gtk_text_thaw(GTK_TEXT(textwidget));

		if(pos > 0 && pos < gtk_text_get_length(GTK_TEXT(textwidget)))
		{
			gtk_editable_set_position(
				GTK_EDITABLE(textwidget), pos);
		}
	}
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
 * Return the requested GdkColor -- it is newly allocated and should
 *  possibly be freed somewhere.
 */
GdkColor *gtranslator_syntax_get_gdk_color(ColorName name)
{
	GdkColor *color;
	
	color=g_new0(GdkColor,1);

	switch(name)
	{
		case RED:
			color->red=(gushort) 65535;
			color->green=color->blue=(gushort) 0;
			break;

		case GREEN:
			color->green=(gushort) 65535;
			color->red=color->blue=(gushort) 0;
			break;
		
		case BLUE:
			color->blue=(gushort) 65535;
			color->red=color->green=(gushort) 0;
			break;
			
		case BLACK:
			color->red=color->green=color->blue=(gushort) 65535;
			break;
			
		case WHITE:
			color->red=color->green=color->blue=(gushort) 0;
			break;
		
		case YELLOW:
			color->red=color->green=(gushort) 65535;
			color->blue=(gushort) 0;
			break;
		
		case ORANGE:
			color->red=(gushort) 65535;
			color->green=(gushort) 43954;
			color->blue=(gushort) 0;
			break;
		
		case NAVY:
			color->red=(gushort) 9744;
			color->green=(gushort) 6773;
			color->blue=(gushort) 65535;
			break;
			
		case MAROON:
			color->red=(gushort) 41208;
			color->green=(gushort) 17705;
			color->blue=(gushort) 39422;
			break;
			
		case AQUA:
			color->red=(gushort) 0;
			color->green=color->blue=(gushort) 65535;
			break;
		
		case BROWN:
			color->red=(gushort) 48655;
			color->green=(gushort) 22576;
			color->blue=(gushort) 14757;
			break;
			
		default:
			/*
			 * Get the stored default values for the foreground
			 *  or the user specified ones.
			 */  
			gtranslator_config_init();
			
			color->red=gtranslator_config_get_int(
				"colors/fg_red");
			color->green=gtranslator_config_get_int(
				"colors/fg_green");
			color->blue=gtranslator_config_get_int(
				"colors/fg_blue");
			
			gtranslator_config_close();
			
			break;
	}

	
	color->pixel=(gulong) (
		(color->red)*65536 + (color->green)*255 + color->blue);

	gdk_color_alloc(gtk_widget_get_colormap(app1), color);

	return color;
}
