/*
 * (C) 2001-2003 	Fatih Demir <kabalak@gtranslator.org>
 * 			Thomas Ziehmer <thomas@gtranslator.org>
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

#include "comment.h"
#include "syntax.h"
#include "utf8.h"
#include "utils.h"

#include <gtk/gtkeditable.h>
#include <gtk/gtkentry.h>
#include <gtk/gtktext.h>
#include <gtk/gtkwidget.h>

/*
 * Return whether we do have got a UTF-8 file or not.
 */
gboolean gtranslator_utf8_po_file_is_utf8(GtrPo *po)
{
	g_return_val_if_fail(file_opened!=FALSE, FALSE);
	g_return_val_if_fail(po!=NULL, FALSE);
	g_return_val_if_fail(po->header!=NULL, FALSE);
	g_return_val_if_fail(po->header->charset!=NULL, FALSE);

	return (!g_ascii_strcasecmp(po->header->charset, "utf-8") ||
		!g_ascii_strcasecmp(po->header->charset, "utf8"));	
}

/*
 * Convert the given GtrMsg to UTF-8.
 */
void gtranslator_utf8_convert_message_to_utf8(GtrMsg *msg, const gchar *orig_enc)
{
	gchar *msgstr;
	gchar *msgid;

	g_return_if_fail(msg!=NULL);

	if (msg->msgstr) {
		msgstr=msg->msgstr;
		msg->msgstr=g_convert(msgstr, -1, 
	                	      "UTF-8", orig_enc, 
	        	              NULL, NULL, NULL);
		g_free(msgstr);
	}

	msgid=msg->msgid;
	msg->msgid=g_convert(msgid, -1, 
	                      "UTF-8", orig_enc, 
	                      NULL, NULL, NULL);
	g_free(msgid);

}

/*
 * Convert the given GtrMsg from UTF-8 to a "normal" string.
 */
void gtranslator_utf8_convert_message_from_utf8(GtrMsg *msg, const gchar *orig_enc)
{
	gchar *msgstr;

	g_return_if_fail(msg!=NULL);

	msgstr=msg->msgstr;
	msg->msgstr=g_convert(msgstr, -1, 
	                      orig_enc, "UTF-8",
	                      NULL, NULL, NULL);
	g_free(msgstr);
}

/*
 * Convert all messages completely to UTF-8.
 */
void gtranslator_utf8_convert_po_to_utf8(GtrPo *po)
{
	gchar		*author;
	GtrComment	*header_comment;
	
	g_return_if_fail(po!=NULL);
	g_return_if_fail(po->messages!=NULL);
	
	g_list_foreach(po->messages, 
		(GFunc) gtranslator_utf8_convert_message_to_utf8, po->header->charset);

	/*
	 * Convert the author name to UTF-8.
	 */
	author=g_convert(po->header->translator, -1, 
	                 po->header->charset, "UTF-8",
	                 NULL, NULL, NULL);
	g_free(po->header->translator);
	po->header->translator=author;

	/*
	 * Convert the header comment to a GtrComment and get the UTF-8 version of it.
	 */
	header_comment=gtranslator_comment_new(po->header->comment);
	GTR_FREE(po->header->comment);

	po->header->comment=g_strdup(GTR_COMMENT(header_comment)->comment);
	gtranslator_comment_free(&header_comment);

	/*
	 * Now assign the new charset name: UTF-8 .-)
	 */
	GTR_FREE(po->header->charset);
	po->header->charset=g_strdup("UTF-8");
}

/*
 * Convert all messages completely from UTF-8.
 */
void gtranslator_utf8_convert_po_from_utf8(GtrPo *po)
{
	g_return_if_fail(po->messages!=NULL);
	
	g_list_foreach(po->messages,
		(GFunc) gtranslator_utf8_convert_message_from_utf8, po->header->charset);

	/*
	 * Assign the converted charset value.
	 */
	if(po->locale_charset)
	{
		GTR_FREE(po->header->charset);
		po->header->charset=g_strdup(po->locale_charset);
	}
}

/*
 * Simply saves some typing...
 * [depracated] - I can't find anywhere that uses this
 */
gchar *gtranslator_utf8_convert_to_utf8(const gchar *orig_enc, const gchar *str)
{
	g_return_val_if_fail(str!=NULL, NULL);

	return (g_convert(str, -1, "UTF-8", orig_enc, NULL, NULL, NULL));
}
