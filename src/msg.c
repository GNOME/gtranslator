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
 *     MERCHANMsgILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "msg.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gettext-po.h>

#define GTR_MSG_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_MSG,     \
					 GtranslatorMsgPrivate))


G_DEFINE_TYPE(GtranslatorMsg, gtranslator_msg, G_TYPE_OBJECT)

struct _GtranslatorMsgPrivate
{
	po_message_iterator_t iterator;
	
	po_message_t message;
	
	GtranslatorMsgStatus status;

	GtkTreeRowReference *row_reference;

	gint po_position;
  
        gchar **tm_list;
};

static gchar *message_error = NULL;

static void
gtranslator_msg_init (GtranslatorMsg *msg)
{
	msg->priv = GTR_MSG_GET_PRIVATE (msg);
}

static void
gtranslator_msg_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_msg_parent_class)->finalize (object);
}

static void
gtranslator_msg_class_init (GtranslatorMsgClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorMsgPrivate));

	object_class->finalize = gtranslator_msg_finalize;
}

/***************************** Public funcs ***********************************/

/**
 * gtranslator_msg_new:
 * 
 * Creates a new #GtranslatorMsg.
 * 
 * Return value: a new #GtranslatorMsg object
 **/
GtranslatorMsg *
gtranslator_msg_new(po_message_iterator_t iter)
{
	GtranslatorMsg *msg;
	
	msg = g_object_new (GTR_TYPE_MSG, NULL);
	msg->priv->iterator = iter;
	
	return msg;
}

/**
 * gtranslator_msg_get_message:
 * @msg: a #GtranslatorMsg
 *
 * Return value: the message in gettext format
 **/
po_message_t
gtranslator_msg_get_message(GtranslatorMsg *msg)
{
	return msg->priv->message;
}

/**
 * gtranslator_msg_set_message:
 * @msg: a #GtranslatorMsg
 * @message: the po_message_t to set into the @msg
 *
 * Sets the message into the #GtranslatorMsg class.
 **/
void
gtranslator_msg_set_message(GtranslatorMsg *msg,
			    po_message_t message)
{
	msg->priv->message = message;
}

/**
 * gtranslator_msg_get_row_reference:
 * @msg: a #GtranslatorMsg
 *
 * Returns: the #GtkTreeRowReference corresponding to the message's place
 * in the message table
 */
GtkTreeRowReference *
gtranslator_msg_get_row_reference(GtranslatorMsg *msg)
{
	return msg->priv->row_reference;
}

/**
 * gtranslator_msg_set_row_reference:
 * @msg: a #GtranslatorMsg
 * @tree_iter: the GtkTreeRowReference corresponding to position in the message table
 *
 * Sets the GtkTreeRowReference from the messages table for the given message
 **/
void
gtranslator_msg_set_row_reference(GtranslatorMsg *msg,
				  GtkTreeRowReference *row_reference)
{
	msg->priv->row_reference = row_reference;
}

/**
 * po_message_is_translated:
 * @msg: a #GtranslatorMsg
 * 
 * Return value: TRUE if the message is translated
 **/
gboolean
gtranslator_msg_is_translated (GtranslatorMsg *msg)
{
	if (po_message_msgid_plural(msg->priv->message) == NULL)
		return po_message_msgstr(msg->priv->message)[0] != '\0';	
	else
	{
		gint i;

		for (i = 0; ; i++)
		{
			const gchar *str_i = po_message_msgstr_plural(msg->priv->message, i);
			if (str_i == NULL)
				break;
			if (str_i[0] == '\0')
				return FALSE;
		}
		
		return TRUE;
	}
}

/**
 * gtranslator_msg_is_fuzzy:
 * @msg: a #GtranslatorMsg
 * 
 * Return value: TRUE if the message is fuzzy
 **/
gboolean
gtranslator_msg_is_fuzzy(GtranslatorMsg *msg)
{
	return po_message_is_fuzzy(msg->priv->message);
}


/**
 * gtranslator_msg_set_fuzzy:
 * @msg: a #GtranslatorMsg
 * @fuzzy: the fuzzy value to set to the message
 * 
 * Change the fuzzy mark of a message.
 **/
void
gtranslator_msg_set_fuzzy(GtranslatorMsg *msg,
			  gboolean fuzzy)
{
	po_message_set_fuzzy(msg->priv->message, fuzzy);
}

/**
 * gtranslator_msg_set_status:
 * @msg: a #GtranslatorMsg
 * @status: a #GtranslatorMsgStatus
 * 
 * Sets the status for a message.
 */
void
gtranslator_msg_set_status(GtranslatorMsg *msg,
			   GtranslatorMsgStatus status)
{
	msg->priv->status = status;
}

/**
 * gtranslator_msg_get_status:
 * @msg: a #GtranslatorMsg
 * 
 * Return value: the message's status.
 */
GtranslatorMsgStatus
gtranslator_msg_get_status(GtranslatorMsg *msg)
{
	return msg->priv->status;
}

/**
 * gtranslator_msg_get_msgid:
 * @msg: a #GtranslatorMsg
 *
 * Return value: the msgid (untranslated English string) of a message.
 **/
const gchar *
gtranslator_msg_get_msgid(GtranslatorMsg *msg)
{
	return po_message_msgid(msg->priv->message);
}


/**
 * gtranslator_msg_get_msgid_plural:
 * @msg: a #GtranslatorMsg
 * 
 * Return value: the msgid_plural (untranslated English plural string) of a
 * message, or NULL for a message without plural.
 **/
const gchar *
gtranslator_msg_get_msgid_plural(GtranslatorMsg *msg)
{
	return po_message_msgid_plural(msg->priv->message);	
}


/**
 * gtranslator_msg_get_msgstr:
 * @msg: a #GtranslatorMsg
 * 
 * Return value: the msgstr (translation) of a message.
 * Return the empty string for an untranslated message.
 **/
const gchar *
gtranslator_msg_get_msgstr(GtranslatorMsg *msg)
{
	return po_message_msgstr(msg->priv->message);
}


/**
 * gtranslator_msg_set_msgstr:
 * @msg: a #GtranslatorMsg
 * @msgstr: the string to set in the @msg
 * 
 * Change the msgstr (translation) of a message.
 * Use an empty string to denote an untranslated message.
 **/
void
gtranslator_msg_set_msgstr(GtranslatorMsg *msg,
			   const gchar *msgstr)
{
	po_message_set_msgstr(msg->priv->message, msgstr);
}


/**
 * gtranslator_msg_get_msgstr_plural:
 * @msg: a #GtranslatorMsg
 * @index: the index of the plural array
 *
 * Return value: the msgstr[index] for a message with plural handling, or
 * NULL when the index is out of range or for a message without plural.
 **/
const gchar *
gtranslator_msg_get_msgstr_plural(GtranslatorMsg *msg,
				  gint index)
{
	return po_message_msgstr_plural(msg->priv->message, index);
}

/**
 * gtranslator_msg_set_msgstr_plural:
 * @msg: a #GtranslatorMsg
 * @index: the index where to set the msgstr
 * @msgstr: the message to set in the msg
 *
 * Change the msgstr[index] for a message with plural handling.
 * Use a NULL value at the end to reduce the number of plural forms.
 **/
void
gtranslator_msg_set_msgstr_plural(GtranslatorMsg *msg,
				  gint index,
				  const gchar *msgstr)
{
	po_message_set_msgstr_plural(msg->priv->message,
				     index,
				     msgstr);
}


/**
 * gtranslator_msg_get_comment:
 * @msg: a #GtranslatorMsg
 *
 * Return value: the comments for a message.
 **/
const gchar *
gtranslator_msg_get_comment(GtranslatorMsg *msg)
{
	return po_message_comments(msg->priv->message);
}

/**
 * gtranslator_msg_set_comment:
 * @msg: a #GtranslatorMsg
 * @comment: the comment to set for a message
 * 
 * Change the comments for a message.
 * comments should be a multiline string, 
 * ending in a newline, or empty.
 **/
void
gtranslator_msg_set_comment(GtranslatorMsg *msg,
			    const gchar *comment)
{
	po_message_set_comments(msg->priv->message, comment);
}

/**
 * gtranslator_msg_get_po_position:
 * @msg: a #GtranslatorMsg
 *
 * Return value: the position of the message.
 *
 * Gets the position of this message in the PO file in relation to the other
 * messages.
 **/
gint
gtranslator_msg_get_po_position (GtranslatorMsg *msg)
{
	return msg->priv->po_position;
}

/**
 * gtranslator_msg_set_po_position:
 * @msg: a #GtranslatorMsg
 * @po_position: the numerical position of the message.
 *
 * Sets the numerical position of this message in relation to other messages.
 **/
void
gtranslator_msg_set_po_position (GtranslatorMsg *msg,
				 gint po_position)
{
	msg->priv->po_position = po_position;
}

/**
 * gtranslator_msg_get_extracted_comments:
 * @msg: a #GtranslatorMsg
 *
 * Return value: the extracted comments for a message.
 **/
const gchar *
gtranslator_msg_get_extracted_comments(GtranslatorMsg *msg)
{
	return po_message_extracted_comments(msg->priv->message);
}

/**
 * gtranslator_msg_get_filename:
 * @msg: a #GtranslatorMsg
 * @i: the i-th file for a message.
 *
 * Return value: the i-th filename for a message, or NULL if @i is out
 * of range.
 */
const gchar *
gtranslator_msg_get_filename(GtranslatorMsg *msg,
			     gint i)
{
	po_filepos_t filepos;
	
	filepos = po_message_filepos(msg->priv->message, i);
	
	if(filepos == NULL)
		return NULL;
		
	return po_filepos_file(filepos);
}

/**
 * gtranslator_msg_get_file_line:
 * @msg: a #GtranslatorMsg
 * @i: the i-th file for a message.
 *
 * Return value: the i-th file line for a message, or NULL if @i is out
 * of range.
 */
gint *
gtranslator_msg_get_file_line(GtranslatorMsg *msg,
			      gint i)
{
	po_filepos_t filepos;
	
	filepos = po_message_filepos(msg->priv->message, i);
	
	if(filepos == NULL)
		return NULL;
		
	return (gint *)po_filepos_start_line(filepos);
}

/**
 * gtranslator_msg_get_msgctxt:
 * @msg: a #GtranslatorMsg
 *
 * Return value: the context of a message, or NULL for a 
 * message not restricted to a context.
 */
const gchar *
gtranslator_msg_get_msgctxt(GtranslatorMsg *msg)
{
    	g_return_val_if_fail(GTR_IS_MSG(msg), NULL);

	return po_message_msgctxt(msg->priv->message);
}

/**
 * gtranslator_msg_get_format:
 * @msg: a #GtranslatorMsg
 *
 * Return the pretty name associated with a format type.
 * For example, for "csharp-format", return "C#".
 * Return NULL if the are no format type in the message.
 * 
 * Return value: the pretty name associated with a format type or NULL
 * if the message hasn't any format type.
 */
const gchar *
gtranslator_msg_get_format (GtranslatorMsg *msg)
{
	const gchar * const *format_list;
	gint i;
	
	g_return_val_if_fail (GTR_IS_MSG (msg), NULL);
	
	format_list = po_format_list ();
	
	for (i = 0; format_list[i] != NULL; i++)
	{
		if (po_message_is_format (msg->priv->message, format_list[i]))
			return po_format_pretty_name (format_list[i]);
	}
	
	return NULL;
}

/*
 * Functions to manage the gettext errors
 */
static void
on_gettext_po_xerror(gint severity,
		     po_message_t message,
		     const gchar *filename, size_t lineno, size_t column,
		     gint multiline_p, const gchar *message_text)
{
	if (message_text)
		message_error = g_strdup (message_text);
	else message_error = NULL;
}

static void
on_gettext_po_xerror2(gint severity,
		      po_message_t message1,
		      const gchar *filename1, size_t lineno1, size_t column1,
		      gint multiline_p1, const gchar *message_text1,
		      po_message_t message2,
		      const gchar *filename2, size_t lineno2, size_t column2,
		      gint multiline_p2, const gchar *message_text2)
{
	g_warning("Error: %s.\n %s", message_text1, message_text2);
}


/**
 * gtranslator_msg_check:
 * @msg: a #GtranslatorMsg
 * 
 * Return value: the message error or NULL if there is not any error. Must be
 * freed with g_free.
 *
 * Test whether the message translation is a valid format string if the message
 * is marked as being a format string.  
 **/
gchar *
gtranslator_msg_check(GtranslatorMsg *msg)
{
	struct po_xerror_handler handler;
	
	g_return_val_if_fail(msg != NULL, NULL);

	handler.xerror = &on_gettext_po_xerror;
	handler.xerror2 = &on_gettext_po_xerror2;
	
	po_message_check_all(msg->priv->message, msg->priv->iterator, &handler);
	
	if(gtranslator_msg_is_fuzzy(msg) || !gtranslator_msg_is_translated(msg))
	{
		if (message_error)
			g_free (message_error);
		message_error = NULL;
	}

	/*Are there any other way to do this?*/
	return message_error;
}


gchar **
gtranslator_msg_get_tm_list (GtranslatorMsg *msg)
{
  return msg->priv->tm_list;
}

void
gtranslator_msg_set_tm_list (GtranslatorMsg *msg,
			     gchar **tm_list)
{
  msg->priv->tm_list = tm_list;
}
