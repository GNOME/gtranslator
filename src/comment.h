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
	OBSOLETE		= 0,
	REFERENCE_COMMENT	= (1 << 0),
	SOURCE_COMMENT		= (1 << 1),
	TRANSLATOR_COMMENT	= (1 << 2),
	FLAG_COMMENT		= (1 << 3),
	FUZZY_COMMENT		= (1 << 4),
	FUZZY_C_FORMAT_COMMENT	= (1 << 5),
	C_FORMAT_COMMENT	= (1 << 6),
	INTERNAL_COMMENT	= (1 << 7),
	NO_COMMENT		= (1 << 8)
} GtrCommentType;

/*
 * The new comment structure -- the home for all kinds of comments.
 */
typedef struct
{
	gchar		*comment;
	gchar		*utf8_comment;

	gchar		*pure_comment;
	gchar		*pure_utf8_comment;
	
	GtrCommentType	type;
} GtrComment;

#define GTR_COMMENT(x) ((GtrComment *) x)
#define GTR_COMMENT_TYPE(x) (GTR_COMMENT(x)->type)

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
 * Assign the given full_comment (which must be a full comment string _with_ 
 *  prefix) to the comment and update the comment informations/fields.
 */
void gtranslator_comment_update(GtrComment **comment, const gchar *full_comment);

/*
 * Check if the given string is contained in any comment field of the GtrComment.
 */
gboolean gtranslator_comment_search(GtrComment *comment, const gchar *search_string);

/*
 * Copy the given GtrComment; NULL parts are also set to NULL for the copy.
 */
GtrComment *gtranslator_comment_copy(GtrComment *comment); 

/*
 * Return a savely copy of the comment contents by itself.
 */
gchar *gtranslator_comment_get_comment_contents(GtrComment *comment);

/*
 * Free the GtrComment.
 */
void gtranslator_comment_free(GtrComment **comment); 

/*
 * Display the current comment in it's full beautifulness .-)
 */
void gtranslator_comment_display(GtrComment *comment); 

/*
 * Clears/hides the comment viewing (area).
 */
void gtranslator_comment_hide(void);

#endif
