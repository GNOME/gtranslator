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
 
#ifndef GTR_COMMENT_H
#define GTR_COMMENT_H 1

#include <glib.h>

/*
 * The supported comment types.
 */
typedef enum
{
	OBSOLETE,
	REFERENCE_COMMENT,
	SOURCE_COMMENT,
	TRANSLATOR_COMMENT,
	FLAG_COMMENT,
	FUZZY_COMMENT,
	FUZZY_C_FORMAT_COMMENT,
	C_FORMAT_COMMENT,
	INTERNAL_COMMENT,
	NO_COMMENT
} GtrCommentType;

/*
 * The new comment structure -- the home for all kinds of comments.
 */
typedef struct
{
	gchar		*comment;
	gchar		*pure_comment;
	
	GtrCommentType	type;
} GtrComment;

#define GTR_COMMENT(x) ((GtrComment *) x)

/*
 * Creates and returns a new GtrComment -- the comment type is automatically
 *  determined from the comment_string.
 */
GtrComment *gtranslator_comment_new(const gchar *comment_string);

/*
 * Append another comment string to the given GtrComment.
 */
void gtranslator_comment_append(GtrComment **comment, const gchar *comment_string);

/*
 * Copy the given GtrComment; NULL parts are also set to NULL for the copy.
 */
GtrComment *gtranslator_comment_copy(GtrComment **comment); 

/*
 * Free the GtrComment.
 */
void gtranslator_comment_free(GtrComment **comment); 

/*
 * Return whether the given message's comment is a "visible" one.
 */
gboolean gtranslator_comment_is_visible(GtrComment *comment);

/*
 * Popup the comment for the current message.
 */
void gtranslator_comment_pop_up(void); 

/*
 * Shows eventual references for the current comment.
 */
void gtranslator_comment_show_references(void);

#endif
