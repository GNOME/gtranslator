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
	}
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
 * Popup the comment for the current message.
 */
void gtranslator_comment_pop_up()
{
	g_return_if_fail(file_opened==TRUE);
}

/*
 * Shows eventual references for the current comment.
 */
void gtranslator_comment_show_references()
{
	g_return_if_fail(file_opened==TRUE);
}
