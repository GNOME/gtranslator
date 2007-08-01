/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "args-tags.h"
#include <glib.h>
#include <gtk/gtk.h>


#ifdef __GLIB_2_14__

static gchar * args[] =
{
	"%[ndioxXucsfeEgGp]",
	"%([0-9]+(\\$))?[-+'#0]?[0-9]*(.[0-9]+)?[hlL]?[dioxXucsfeEgGp]",
	"%[0-9]+"
};

static gchar *tags[]=
{
	"</[A-Za-z0-9\\n]+>",
	"<[A-Za-z0-9\\n]+[^>]*/?>",
	"http:\\/\\/[a-zA-Z0-9\\.\\-_/~]+",
	"mailto:[a-z0-9\\.\\-_]+@[a-z0-9\\.\\-_]+",
	"<?[a-z0-9\\.\\-_]+@[a-z0-9\\.\\-_]+>?",
	"&[a-z,A-Z,\\-,0-9,#\\.]*;"
};

static void
gtranslator_regex(GtkTextBuffer *buffer, gchar *exp,
		  gchar *text, const gchar *color)
{
	GRegex *regex;
	GMatchInfo *match_info;
	gint start_pos, end_pos;
	gint match_num, match_count;
	GtkTextTag *tag;
	GtkTextIter start, end;
	
	regex = g_regex_new(exp, 0, 0, NULL);
	g_regex_match(regex, text, 0, &match_info);
	while(g_match_info_matches(match_info))
	{
		match_count = g_match_info_get_match_count(match_info);
		g_match_info_fetch_pos(match_info, 0, &start_pos, &end_pos);
	
		gtk_text_buffer_get_start_iter(buffer, &start);
		gtk_text_buffer_get_start_iter(buffer, &end);
		gtk_text_iter_forward_chars(&start, start_pos);
		gtk_text_iter_forward_chars(&end, end_pos);
	
		tag = gtk_text_buffer_create_tag (buffer, NULL, 
						  "foreground", color, 
						  NULL);
		gtk_text_buffer_apply_tag(buffer, tag,
					  &start, &end);
		g_match_info_next (match_info, NULL);
	}
	g_match_info_free (match_info);
	g_regex_unref (regex);
}


/*
 * Callback func called when a message change.
 * This change the tags and arguments to a specific color.
 */
void
gtranslator_args_tags(GtkWidget *widget,
		      gpointer useless)
{
	
	gint i;
	GtkTextBuffer *buffer;
	GtkTextIter start, end;
	gchar *string;
	
	buffer = GTK_TEXT_BUFFER(widget);
	gtk_text_buffer_get_bounds(buffer, &start, &end);
	string = gtk_text_buffer_get_text(buffer, &start, &end, 0);
	
	if(string == NULL)
		return;
	
	for(i = 0; i < 3; i++)
		gtranslator_regex(buffer, args[i], string, "#0000FF");
	for(i = 0; i < 6; i++)
		gtranslator_regex(buffer, tags[i], string, "#42a042");
}




#endif


