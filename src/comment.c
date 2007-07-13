/*
 * (C) 2001-2002 	Fatih Demir <kabalak@kabalak.net>
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

#include "actions.h"
#include "comment.h"
#include "page.h"
#include "nautilus-string.h"
#include "prefs.h"
#include "utils.h"
#include "menus.h"

#include <gtk/gtklabel.h>
/*
 * A small new structure table to make the comment type recognition
 *  a bit easier and more convenient.
 */
typedef struct
{
	gchar		*prefix;
	GtrCommentType	type;
} GtrCommentPrefixTypeGroup;

/*
 * The static table with the GtrCommentPrefixTypeGroup definitions.
 */
static GtrCommentPrefixTypeGroup GtrPrefixTypes[] =
{
	{ "#~", OBSOLETE },
	{ "#:", REFERENCE_COMMENT },
	{ "#.", SOURCE_COMMENT },
	{ "# ", TRANSLATOR_COMMENT },
	{ "#, fuzzy, c-format", FUZZY_C_FORMAT_COMMENT },
	{ "#, fuzzy", FUZZY_COMMENT },
	{ "#, c-format", C_FORMAT_COMMENT },
	{ "#,", FLAG_COMMENT },
	{ "#-", INTERNAL_COMMENT },
	{ "#>", INTERNAL_COMMENT },
	{ "* ", NO_COMMENT },
	{ NULL, NO_COMMENT }
};

/*
 * Creates and returns a new GtrComment -- the comment type is automatically
 *  determined.
 */
GtrComment *gtranslator_comment_new(const gchar *comment_string)
{
	GtrComment *comment=g_new0(GtrComment, 1);
	
	g_return_val_if_fail(comment_string!=NULL, NULL);

	if(comment_string[0]!='#')
		return NULL;

	comment->comment=g_strdup(comment_string);

	gint	c=0;
		
	/*
	 * Check for any match in our prefix/types table.
	 */
	while(GtrPrefixTypes[c].prefix!=NULL)
	{
		/*
		 * If the prefix could be "matched", get type and 
		 *  pure_comment out of the full comment text.
		 */
		if(nautilus_istr_has_prefix(comment->comment, 
			GtrPrefixTypes[c].prefix))
		{
			gint 	i=0;
			
			/*
			 * Set the current type flag.
			 */
			comment->type |= GtrPrefixTypes[c].type;

			/*
			 * Strip the matched prefix out of the string.
			 */
			comment->pure_comment=nautilus_str_replace_substring(comment->comment, GtrPrefixTypes[c].prefix, "");

			/*
			 * Now do also strip all the other prefixes out
			 *  of the pure_comment.
			 */
			while(GtrPrefixTypes[i].prefix!=NULL)
			{
				comment->pure_comment=nautilus_str_replace_substring(comment->pure_comment, GtrPrefixTypes[i].prefix, "");
					
				i++;
			}
		}
		
		c++;
	}

	/*
	 * Remove any lungering space on the beginning/end of the pure_comment 
	 *  of the GtrComment.
	 */
	if(comment->pure_comment)
	{
		comment->pure_comment=g_strstrip(comment->pure_comment);
	}
	
#ifdef UTF8_COMMENT
	/*
	 * Set up the UTF-8 representations for the GtrComment parts.
	 */
	comment->utf8_comment=
		g_convert(comment->comment, -1,
	                  "UTF-8", po->header->charset,
	                  NULL, NULL, NULL);

	if(GTR_COMMENT(comment)->pure_comment)
	{
		comment->pure_utf8_comment=
			g_convert(comment->pure_comment, -1,
				  "UTF-8", po->header->charset,
				  NULL, NULL, NULL);
	}
	else
	{
		comment->pure_utf8_comment=NULL;
	}
#endif

	/*
	 * Set the fuzzy flag at all if the comment stands for a fuzzy message.
	 */
	if(strstr(comment->comment, "#, fuzzy"))
	{
		comment->type |= FUZZY_COMMENT;
	}
	
	return comment;
}

/*
 * Appends the given comment string to the GtrComment.
 */
void gtranslator_comment_append(GtrComment **comment, const gchar *comment_string)
{
	gchar *comment_core_string;
	
	g_return_if_fail(GTR_COMMENT(*comment)!=NULL);
	g_return_if_fail(comment_string!=NULL);

	comment_core_string=g_strconcat(GTR_COMMENT(*comment)->comment, comment_string, NULL);
	gtranslator_comment_free(comment);

	*comment=gtranslator_comment_new(comment_core_string);
}

/*
 * Update the given GtrComment's fields/informations according to the given,
 *  new full_comment.
 */
void gtranslator_comment_update(GtrComment **comment, const gchar *full_comment)
{
	g_return_if_fail(GTR_COMMENT(*comment)!=NULL);
	g_return_if_fail(full_comment!=NULL);

	gtranslator_comment_free(comment);
	*comment=gtranslator_comment_new(full_comment);
}

/*
 * Copy the given GtrComment.
 */
GtrComment *gtranslator_comment_copy(GtrComment *comment)
{
	GtrComment *copy=g_new0(GtrComment, 1);
	
	g_return_val_if_fail(comment!=NULL, NULL);

	copy->comment=g_strdup(comment->comment);
	copy->pure_comment=g_strdup(comment->pure_comment);
#ifdef UTF8_COMMENT
	copy->utf8_comment=g_strdup(comment->utf8_comment);
	copy->pure_utf8_comment=g_strdup(comment->pure_utf8_comment);
#endif
	copy->type=comment->type;	

	return copy;
}

/*
 * Returns the "normal" and plain comment contents string.
 */
gchar *gtranslator_comment_get_comment_contents(GtrComment *comment)
{
	g_return_val_if_fail(GTR_COMMENT(comment)!=NULL, "");
	g_return_val_if_fail(GTR_COMMENT(comment)->comment!=NULL, "");

	return (GTR_COMMENT(comment)->comment);
}

/*
 * Free the GtrComment.
 */
void gtranslator_comment_free(GtrComment **comment)
{
	if(*comment)
	{
		g_free((*comment)->comment);
		g_free((*comment)->pure_comment);
#ifdef UTF8_COMMENT
		g_free((*comment)->utf8_comment);
		g_free((*comment)->pure_utf8_comment);
#endif
		
		g_free(*comment);
	}
}

/*
 * Search for the given string in our comment.
 */
gboolean gtranslator_comment_search(GtrComment *comment, const gchar *search_string)
{
	g_return_val_if_fail(GTR_COMMENT(comment)!=NULL, FALSE);
	g_return_val_if_fail(GTR_COMMENT(comment)->comment!=NULL, FALSE);
	g_return_val_if_fail(search_string!=NULL, FALSE);

	/*
	 * Check for the string in the comment parts according to a kabalak-logic.
	 */
	if(search_string[0]=='#')
	{
		if(strstr(comment->comment, search_string))
		{
			return TRUE;
		}
	}
	else if(strstr(comment->pure_comment, search_string))
	{
		return TRUE;
	}
#ifdef UTF8_COMMENT
	else if(strstr(comment->pure_utf8_comment, search_string))
	{
		return TRUE;
	}
#endif

	return FALSE;
}

/*
 * Show the current message's comment.
 */
void gtranslator_comment_display(GtrComment *comment)
{
	gchar	*comment_display_str=NULL;
	
	if(!comment || !comment->pure_comment)
	{
		return;
	}
	else
	{
		if(!GtrPreferences.show_comment)
		{
			comment_display_str=g_strdup(" ");
		}
		else
		{
		        comment_display_str=g_strdup(comment->pure_comment);
		}
	}

	gtk_label_set_text(GTK_LABEL(current_page->comment), 
		comment_display_str);

	if((GTR_COMMENT(comment)->type & TRANSLATOR_COMMENT ||
		GTR_COMMENT(comment)->type & SOURCE_COMMENT) &&
		nautilus_strcasecmp(comment_display_str, " "))
	{
		//Enable widgets
		gtk_widget_set_sensitive(current_page->edit_comment, TRUE);
		gtk_widget_set_sensitive(gtranslator_menuitems->comment, TRUE);
	}
	else
	{
		//Disable widgets
		gtk_widget_set_sensitive(current_page->edit_comment, FALSE);
		gtk_widget_set_sensitive(gtranslator_menuitems->comment, FALSE);
	}

	g_free(comment_display_str);
}

/*
 * Hide any currently shown comment (area).
 */
void gtranslator_comment_hide()
{
	gtk_label_set_text(GTK_LABEL(current_page->comment), "");
	gtk_widget_set_sensitive(current_page->edit_comment, FALSE);
	gtk_paned_set_position(GTK_PANED(current_page->content_pane), 0);
}
