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

/*
 * Extract the comment prefix from the comment.
 */
gchar *extract_pure_comment(GtrComment *comment); 

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
	if(comment_string[0]!='#')
	{
		return NULL;
	}
	else if(comment_string[0]=='#' && comment_string[1])
	{
		switch(comment_string[1])
		{
			case '~':
				comment->type=OBSOLETE;
					break;

			case ':':
				comment->type=REFERENCE_COMMENT;
					break;

			case '.':
				comment->type=SOURCE_COMMENT;
					break;

			case ' ':
				comment->type=TRANSLATOR_COMMENT;
					break;

			case ',':
				comment->type=FLAG;
					break;

			case '-':
				comment->type=INTERNAL_COMMENT;
					break;

			default:
				comment->type=NO_COMMENT;
					break;
		}
	}
	
	return comment;
}

/*
 * Free the GtrComment.
 */
void gtranslator_comment_free(GtrComment **comment)
{
	if(GTR_COMMENT(*comment)->comment)
	{
		g_free(GTR_COMMENT(*comment)->comment);
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
 * Extract the pure comment from the GtrComment -- according to the type.
 */
gchar *extract_pure_comment(GtrComment *comment)
{
	g_return_val_if_fail(comment!=NULL, NULL);
	g_return_val_if_fail(comment->type!=NO_COMMENT, NULL);
}
