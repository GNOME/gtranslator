/*
 * (C) 2001-2002	Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
 *			Joe Man <trmetal@yahoo.com.hk>
 *			Peeter Vois <peeter@gtranslator.org>
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
#include "runtime-config.h"
#include "syntax.h"
#include "syntax-elements.h"
#include "utils.h"

#include <ctype.h>
#include <string.h>

#include <gtk/gtk.h>

static gboolean back_match(const gchar *msg, gchar *str, gint pos);
void gtranslator_update_highlighted(
		GtkTextBuffer *textbuffer,
		gpointer userdata );

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


/*
 * Frees text highlighting data
 */
static void text_data_free(gpointer data)
{
	g_return_if_fail(data!=NULL);
	g_string_free((GString*)data, TRUE);
}

/*
 * This function initializes the syntax parser for text widget.
 * The highlighting info will be inside string, so ~250 colors possible
 */
void gtranslator_syntax_init(GtkTextBuffer *textbuffer)
{
	GString *n;
	
	g_return_if_fail(textbuffer != NULL);
	
	n = g_string_new("");
	
	gtk_object_set_data_full(
		GTK_OBJECT(textbuffer), 
		"textdata", 
		n,
		text_data_free);
	
	gtk_signal_connect(GTK_OBJECT(textbuffer), "insert_text",
			   GTK_SIGNAL_FUNC(gtranslator_insert_highlighted), NULL);
	gtk_signal_connect(GTK_OBJECT(textbuffer), "delete_text",
			   GTK_SIGNAL_FUNC(gtranslator_delete_highlighted), NULL);
	gtk_signal_connect(GTK_OBJECT(textbuffer), "changed",
			   GTK_SIGNAL_FUNC(gtranslator_update_highlighted), NULL);
}

void gtranslator_update_highlighted(
		GtkTextBuffer *textbuffer,
		gpointer userdata )
{
	GString *newdata, *olddata;
	gint i,j;
	GtkTextIter I, J;
	guint point;
	gchar type;

	//	gtk_text_freeze( GTK_TEXT( textwidget ) );
	
	olddata = (GString *)gtk_object_get_data(
		GTK_OBJECT(textbuffer), 
		"textdata");
		
	//	point = gtk_text_get_point(GTK_TEXT(textwidget));
	/* Parse highlighting */
	newdata = gtranslator_parse_syntax(textbuffer);

	gtk_text_buffer_get_start_iter(textbuffer, &I);
	gtk_text_buffer_get_start_iter(textbuffer, &J);
	/* Update highlighting */
	for(i=0; i<newdata->len;)
	{
	  gtk_text_iter_forward_char(&I);
	  
		if((type = *(newdata->str + i)) != 
			*(olddata->str + i))
		{
		        static gchar   *c;

			gtk_text_iter_set_offset(&J, i);
			for( j=0; newdata->str[i] == newdata->str[i+j]; j++ )
				if( newdata->len <= (i+j) )
					break;
			gtk_text_iter_forward_char(&J);
			c = gtk_text_iter_get_text(&I, &J);
			//gtk_text_set_point(GTK_TEXT(textwidget), i);
			//gtk_text_forward_delete(GTK_TEXT(textwidget), j);
			//gtk_text_insert(
			//	GTK_TEXT(textwidget),
			//	NULL,
			//	gtranslator_get_color_from_type((gint)type),
			//	gtranslator_get_color_from_type(COLOR_TEXT_BG),
			//	c, j);
			g_free(c);
			i += j;
		}
		else
			i++;
	}
	/* Free olddata and register newdata */
	gtk_object_set_data_full(
		GTK_OBJECT(textbuffer), 
		"textdata", 
		newdata,
		text_data_free);

	/* gtk_text_set_point( GTK_TEXT( textwidget ), point ); */

	//	gtk_text_thaw( GTK_TEXT( textwidget ) );
	//gtk_editable_set_position( GTK_EDITABLE( textwidget), point );
}

/*
 * This function will update highlightion information for text.
 */
void gtranslator_insert_highlighted(
		GtkTextBuffer *textbuffer,
		gchar	*text,
		gint	addlen,
		gint	*pos,
		gpointer userdata)
{
	GString *olddata;
	gint i;
	
	g_return_if_fail(textbuffer != NULL);
	g_return_if_fail(text!=NULL);

	olddata = (GString *)gtk_object_get_data(
		GTK_OBJECT(textbuffer), 
		"textdata");
	for(i=0; i<addlen; i++)
	{
		olddata = g_string_insert_c(olddata, *pos, COLOR_NONE);
	}
}

/*
 * This function will delete text and rehighlight.
 */
void gtranslator_delete_highlighted(
		GtkTextBuffer *textbuffer,
		gint	startpos,
		gint	endpos,
		gpointer	userdata)
{
	GString *olddata;
	gint len;
	
	g_return_if_fail(textbuffer != NULL);
	g_return_if_fail(startpos >= 0);

	if(endpos < 0){
	  len = gtk_text_buffer_get_char_count(textbuffer) - startpos;
	}
	else {
		len = endpos - startpos;
	}
	
	olddata = (GString *)gtk_object_get_data(
		GTK_OBJECT(textbuffer), 
		"textdata");
	olddata = g_string_erase( olddata, startpos, len);
}

GString *gtranslator_parse_syntax(GtkTextBuffer *textbuffer)
{
	GString *string;
	gboolean aInserted;
	gchar specialchar, *msg;
	GtkTextIter start, end;
	gint cp;
	gint z=0;

	/**********************/
	/*For multibyte       */
	
	GdkWChar *wc, ch;
	gchar* mb;	
	gint k, i;
	
	/************************/

	extern gboolean nosyntax;
	
	g_return_val_if_fail(textbuffer!=NULL, NULL);

	gtk_text_buffer_get_bounds(textbuffer, &start, &end);
	msg = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
	       
	if(GtrPreferences.dot_char)
	{
		specialchar=gtranslator_runtime_config->special_char;
	}
	else
	{
		specialchar=' ';
	}


	if(nosyntax || !GtrPreferences.highlight)
	{
		gint i;
		/* The foreground color is always the same */
		string = g_string_new("");
		for(i=0; i<strlen(msg); i++)
			g_string_append_c(string, COLOR_FG);
		return(string);
	}

	/**********************/
	/*
	 *  For multibyte        
	 */

	string = g_string_new("");

	wc = g_new (GdkWChar, strlen(msg) + 1);	
	gdk_mbstowcs(wc, msg, strlen(msg));	
	
	k = 0;
	
	/**********************/
	for(cp=0; cp < strlen(msg); ++cp)
	{
		/*****************************************/
		/*               
		 *  for multibyte character
		 */
		
		ch = wc[cp + 1 - k];
		wc[cp + 1 - k] = 0;		
		mb = gdk_wcstombs(wc + cp - k);		
		wc[cp + 1 - k] = ch;
		
		/*if multibyte character, no highlight*/
		if (mb && strlen(mb) > 1)
		{
			for (i=0; i < strlen(mb); i++)
			{
				g_string_append_c(string, COLOR_FG);		    
				cp++;
				k++;
			}
			cp--;
			k--;

			g_free(mb);
			continue;
		}
		g_free(mb);
		/******************************************/
		/*
		 * Highlight the found elements in this switch tree.
		 */
			
		switch(msg[cp])
		{
			/*
			 * Hotkeys and comment characters:
			 */ 
			case '_':
				g_string_append_c(string, COLOR_HOTKEY);
				
				if(msg[cp+1] && isalpha(msg[cp+1]))
				{
					g_string_append_c(string, COLOR_HOTKEY);
					cp++;
				}
				break;
		
			/*
			 * Format specifiers:
			 */
			case '%':
				g_string_append_c(string, COLOR_C_FORMAT);
				cp++;
			
				while(msg[cp] && msg[cp]!=specialchar &&
					!ispunct(msg[cp]) && !iscntrl(msg[cp]))
				{
					g_string_append_c(string, COLOR_C_FORMAT);
					cp++;
				}
				cp--;
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
				g_string_append_c(string, COLOR_NUMBER);
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
				g_string_append_c(string, COLOR_PUNCTUATION);
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
				g_string_append_c(string, COLOR_SPECIAL);
				break;

			/*
			 * URL/URI prefixes:
			 */
			case ':':
				z=0;
				aInserted=FALSE;
				while(prefixes[z]!=NULL)
				{
					if(back_match(msg, prefixes[z], cp))
					{
						gint i = strlen(prefixes[z]);
						aInserted = TRUE;
						g_string_truncate(string, string -> len - i + 1);
						while(i>0)
						{
							g_string_append_c(string, COLOR_ADDRESS);
							i --;
						}
					}
					z++;
				}
				
				if(aInserted==FALSE)
				{
					g_string_append_c(string, COLOR_PUNCTUATION);
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
					if(back_match(msg, keywords[z], cp))
					{
						gint i = strlen(keywords[z]);
						aInserted = TRUE;
						g_string_truncate(string, string -> len - i + 1);
						while(i>0)
						{
							g_string_append_c(string, COLOR_KEYWORD);
							i --;
						}
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
						g_string_append_c(string, COLOR_SPECIAL_CHAR);
					}
					else{
						g_string_append_c(string, COLOR_FG);
					}
				}

				break;
		}

	}

	/*******************************/
	/*               
	*  for multibyte character, free the memory
	*/
	g_free(wc);
	    
	/*******************************/	

	return string;
}

// XXX fix it
void gtranslator_insert_text(GtkTextBuffer *editable, const gchar *text)
{
  GtkTextIter start, end;
  gint pos=0;

  gtk_text_buffer_get_bounds (editable, &start, &end);

  /* First, delete old contents */
	gtk_signal_emit_by_name(
		GTK_OBJECT(editable),
		"delete-text",
		0,
		-1);
	if (!text)
		return;
	/* Second add new context */
	gtk_signal_emit_by_name(
		GTK_OBJECT(editable),
		"insert-text",
		text,
		strlen(text),
		&pos);
	gtk_signal_emit_by_name(
		GTK_OBJECT(editable),
		"changed");

}

