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

#include "comment.h"
#include "nautilus-string.h"
#include "parse.h"
#include "utf8.h"

/*
 * Creates and returns a new GtrComment -- the comment type is automatically
 *  determined.
 */
GtrComment *gtranslator_comment_new(const gchar *comment_string)
{
	GtrComment *comment=g_new0(GtrComment, 1);
	
	g_return_val_if_fail(comment_string!=NULL, NULL);

	comment->comment=g_strdup(comment_string);

	/*
	 * Categorize the current given comment.
	 */
	if(comment->comment[0]!='#')
	{
		g_free(comment->comment);
		return NULL;
	}
	else
	{
		if(nautilus_istr_has_prefix(comment->comment, "#~"))
		{
			comment->type=OBSOLETE;
			comment->pure_comment=nautilus_str_get_after_prefix(comment->comment, "#~");
		}
		else if(nautilus_istr_has_prefix(comment->comment, "#:"))
		{
			comment->type=REFERENCE_COMMENT;
			comment->pure_comment=nautilus_str_get_after_prefix(comment->comment, "#:");
		}
		else if(nautilus_istr_has_prefix(comment->comment, "#."))
		{
			comment->type=SOURCE_COMMENT;
			comment->pure_comment=nautilus_str_get_after_prefix(comment->comment, "#.");
		}
		else if(nautilus_istr_has_prefix(comment->comment, "# "))
		{
			comment->type=TRANSLATOR_COMMENT;
			comment->pure_comment=nautilus_str_get_after_prefix(comment->comment, "# ");
		}
		else if(nautilus_istr_has_prefix(comment->comment, "#,"))
		{
			/*
			 * Determine all the nice flag-types from the po files.
			 */
			if(nautilus_istr_has_prefix(comment->comment, "#, c-format"))
			{
				comment->type=C_FORMAT_COMMENT;
				comment->pure_comment=nautilus_str_get_after_prefix(
					comment->comment, "#, c-format");
			}
			else if(nautilus_istr_has_prefix(comment->comment, "#, fuzzy, c-format"))
			{
				comment->type=FUZZY_C_FORMAT_COMMENT;
				comment->pure_comment=nautilus_str_get_after_prefix(
					comment->comment, "#, fuzzy, c-format");
			}
			else if(nautilus_istr_has_prefix(comment->comment, "#, fuzzy"))
			{
				comment->type=FUZZY_COMMENT;
				comment->pure_comment=nautilus_str_get_after_prefix(
					comment->comment,  "#, fuzzy");
			}
			else
			{
				comment->type=FLAG_COMMENT;
				comment->pure_comment=nautilus_str_get_after_prefix(
					comment->comment, "#,");
			}
		}
		else if(nautilus_istr_has_prefix(comment->comment, "#-"))
		{
			comment->type=INTERNAL_COMMENT;
			comment->pure_comment=nautilus_str_get_after_prefix(comment->comment, "#-");
		}
		else
		{
			comment->type=NO_COMMENT;
			comment->pure_comment=nautilus_str_get_after_prefix(comment->comment, "#");
		}
	}

	comment->utf8_comment=gtranslator_utf8_get_utf8_string(&comment->pure_comment);
	
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
 * Copy the given Gtrcomment pointer.
 */
GtrComment *gtranslator_comment_copy(GtrComment **comment)
{
	GtrComment *copy=g_new0(GtrComment, 1);
	
	g_return_val_if_fail(GTR_COMMENT(*comment)!=NULL, NULL);

	/*
	 * Copy the single elements of the GtrComment.
	 */
	if(GTR_COMMENT(*comment)->comment)
	{
		copy->comment=g_strdup(GTR_COMMENT(*comment)->comment);
	}
	else
	{
		copy->comment=NULL;
	}

	if(GTR_COMMENT(*comment)->pure_comment)
	{
		copy->pure_comment=g_strdup(GTR_COMMENT(*comment)->pure_comment);
	}
	else
	{
		copy->pure_comment=NULL;
	}

	if(GTR_COMMENT(*comment)->utf8_comment)
	{
		copy->utf8_comment=g_strdup(GTR_COMMENT(*comment)->utf8_comment);
	}
	else
	{
		copy->utf8_comment=NULL;
	}

	copy->type=GTR_COMMENT(*comment)->type;	

	return copy;
}

/*
 * Free the GtrComment.
 */
void gtranslator_comment_free(GtrComment **comment)
{
	if(GTR_COMMENT(*comment)->comment)
	{
		g_free(GTR_COMMENT(*comment)->comment);
		g_free(GTR_COMMENT(*comment)->pure_comment);
		g_free(GTR_COMMENT(*comment)->utf8_comment);
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
	else if(strstr(comment->utf8_comment, search_string))
	{
		return TRUE;
	}

	return FALSE;
}

/*
 * Determine if the comment type is a "visible" one.
 */
gboolean gtranslator_comment_is_visible(GtrComment *comment)
{
	g_return_val_if_fail(comment!=NULL, FALSE);

	if(comment->type==REFERENCE_COMMENT || comment->type==SOURCE_COMMENT ||
		comment->type==TRANSLATOR_COMMENT)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*
 * Show the current message's comment.
 */
void gtranslator_comment_display(GtrComment *comment)
{
	g_return_if_fail(file_opened==TRUE);
	g_return_if_fail(GTR_COMMENT(comment)!=NULL);

	if(comment->type==TRANSLATOR_COMMENT)
	{
	}
	else
	{
	}
}

/*
 * Shows eventual references for the current comment.
 */
void gtranslator_comment_show_references(GtrComment *comment)
{
	g_return_if_fail(file_opened==TRUE);
	g_return_if_fail(GTR_COMMENT(comment)!=NULL);

	if(comment->type==REFERENCE_COMMENT)
	{
		g_warning("FIXME: Show references.");
	}
}
