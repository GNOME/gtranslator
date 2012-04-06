/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *               2008  Igalia
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *   Pablo Sanxiao <psanxiao@gmail.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-msg.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gettext-po.h>

#define GTR_MSG_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_MSG,     \
					 GtrMsgPrivate))


G_DEFINE_TYPE (GtrMsg, gtr_msg, G_TYPE_OBJECT)

struct _GtrMsgPrivate
{
  po_message_iterator_t iterator;

  po_message_t message;

  GtrMsgStatus status;

  GtkTreeRowReference *row_reference;

  gint po_position;
};

static gchar *message_error = NULL;

static void
gtr_msg_init (GtrMsg * msg)
{
  msg->priv = GTR_MSG_GET_PRIVATE (msg);
}

static void
gtr_msg_finalize (GObject * object)
{
  G_OBJECT_CLASS (gtr_msg_parent_class)->finalize (object);
}

static void
gtr_msg_class_init (GtrMsgClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrMsgPrivate));

  object_class->finalize = gtr_msg_finalize;
}

/***************************** Public funcs ***********************************/

/**
 * gtr_msg_new:
 * @iter: the po_message_iterator_t to set into the @msg
 * @message: the
 *
 * Creates a new #GtrMsg.
 *
 * Return value: (transfer full): a new #GtrMsg object
 **/
GtrMsg *
_gtr_msg_new (po_message_iterator_t iter, po_message_t message)
{
  GtrMsg *msg;

  g_return_val_if_fail (iter != NULL || message != NULL, NULL);

  msg = g_object_new (GTR_TYPE_MSG, NULL);

  _gtr_msg_set_iterator (msg, iter);
  _gtr_msg_set_message (msg, message);

  /* Set the status */
  if (gtr_msg_is_fuzzy (msg))
    gtr_msg_set_status (msg, GTR_MSG_STATUS_FUZZY);
  else if (gtr_msg_is_translated (msg))
    gtr_msg_set_status (msg, GTR_MSG_STATUS_TRANSLATED);
  else
    gtr_msg_set_status (msg, GTR_MSG_STATUS_UNTRANSLATED);

  return msg;
}

/**
 * gtr_msg_get_iterator:
 * @msg: a #GtrMsg
 *
 * Return value: the message iterator in gettext format
 **/
po_message_iterator_t
_gtr_msg_get_iterator (GtrMsg * msg)
{
  g_return_val_if_fail (GTR_IS_MSG (msg), NULL);

  return msg->priv->iterator;
}

/**
 * gtr_msg_set_iterator:
 * @msg: a #GtrMsg
 * @iter: the po_message_iterator_t to set into the @msg
 *
 * Sets the iterator into the #GtrMsg class.
 **/
void
_gtr_msg_set_iterator (GtrMsg * msg, po_message_iterator_t iter)
{
  g_return_if_fail (GTR_IS_MSG (msg));

  msg->priv->iterator = iter;
}

/**
 * gtr_msg_get_message:
 * @msg: a #GtrMsg
 *
 * Return value: the message in gettext format
 **/
po_message_t
_gtr_msg_get_message (GtrMsg * msg)
{
  g_return_val_if_fail (GTR_IS_MSG (msg), NULL);

  return msg->priv->message;
}

/**
 * gtr_msg_set_message:
 * @msg: a #GtrMsg
 * @message: the po_message_t to set into the @msg
 *
 * Sets the message into the #GtrMsg class.
 **/
void
_gtr_msg_set_message (GtrMsg * msg, po_message_t message)
{
  g_return_if_fail (GTR_IS_MSG (msg));
  g_return_if_fail (message != NULL);

  msg->priv->message = message;
}

/**
 * gtr_msg_get_row_reference:
 * @msg: a #GtrMsg
 *
 * Returns: the #GtkTreeRowReference corresponding to the message's place
 * in the message table
 */
GtkTreeRowReference *
_gtr_msg_get_row_reference (GtrMsg * msg)
{
  g_return_val_if_fail (GTR_IS_MSG (msg), NULL);

  return msg->priv->row_reference;
}

/**
 * _gtr_msg_set_row_reference:
 * @msg: a #GtrMsg
 * @row_reference: the GtkTreeRowReference corresponding to position in the message table
 *
 * Sets the GtkTreeRowReference from the messages table for the given message
 **/
void
_gtr_msg_set_row_reference (GtrMsg * msg, GtkTreeRowReference * row_reference)
{
  g_return_if_fail (GTR_IS_MSG (msg));

  msg->priv->row_reference = row_reference;
}

/**
 * gtr_msg_is_translated:
 * @msg: a #GtrMsg
 *
 * Gets whether or not the message is translated. See that a fuzzy message
 * is also counted as translated so it must be checked first that the message
 * is fuzzy.
 *
 * Return value: %TRUE if the message is translated
 **/
gboolean
gtr_msg_is_translated (GtrMsg *msg)
{
  g_return_val_if_fail (GTR_IS_MSG (msg), FALSE);

  if (gtr_msg_get_msgid_plural (msg) == NULL)
    return gtr_msg_get_msgstr (msg)[0] != '\0';
  else
    {
      gint i;

      for (i = 0;; i++)
        {
          const gchar *msgstr_i = gtr_msg_get_msgstr_plural (msg, i);
          if (msgstr_i == NULL)
            break;
          if (msgstr_i[0] == '\0')
            return FALSE;
        }

      return TRUE;
    }
}

/**
 * gtr_msg_is_fuzzy:
 * @msg: a #GtrMsg
 * 
 * Return value: TRUE if the message is fuzzy
 **/
gboolean
gtr_msg_is_fuzzy (GtrMsg * msg)
{
  g_return_val_if_fail (GTR_IS_MSG (msg), FALSE);

  return po_message_is_fuzzy (msg->priv->message);
}


/**
 * gtr_msg_set_fuzzy:
 * @msg: a #GtrMsg
 * @fuzzy: the fuzzy value to set to the message
 * 
 * Change the fuzzy mark of a message.
 **/
void
gtr_msg_set_fuzzy (GtrMsg * msg, gboolean fuzzy)
{
  g_return_if_fail (GTR_IS_MSG (msg));

  po_message_set_fuzzy (msg->priv->message, fuzzy);
}

/**
 * gtr_msg_set_status:
 * @msg: a #GtrMsg
 * @status: a #GtrMsgStatus
 * 
 * Sets the status for a message.
 */
void
gtr_msg_set_status (GtrMsg * msg, GtrMsgStatus status)
{
  g_return_if_fail (GTR_IS_MSG (msg));

  msg->priv->status = status;
}

/**
 * gtr_msg_get_status:
 * @msg: a #GtrMsg
 * 
 * Return value: the message's status.
 */
GtrMsgStatus
gtr_msg_get_status (GtrMsg * msg)
{
  g_return_val_if_fail (GTR_IS_MSG (msg), 0);

  return msg->priv->status;
}

/**
 * gtr_msg_get_msgid:
 * @msg: a #GtrMsg
 *
 * Return value: the msgid (untranslated English string) of a message.
 **/
const gchar *
gtr_msg_get_msgid (GtrMsg * msg)
{
  g_return_val_if_fail (GTR_IS_MSG (msg), NULL);

  return po_message_msgid (msg->priv->message);
}


/**
 * gtr_msg_get_msgid_plural:
 * @msg: a #GtrMsg
 * 
 * Return value: (transfer none): the msgid_plural (untranslated English plural
 *               string) of a message, or NULL for a message without plural.
 **/
const gchar *
gtr_msg_get_msgid_plural (GtrMsg * msg)
{
  return po_message_msgid_plural (msg->priv->message);
}


/**
 * gtr_msg_get_msgstr:
 * @msg: a #GtrMsg
 * 
 * Return value: (transfer none): the msgstr (translation) of a message.
 * Return the empty string for an untranslated message.
 **/
const gchar *
gtr_msg_get_msgstr (GtrMsg * msg)
{
  g_return_val_if_fail (GTR_IS_MSG (msg), NULL);

  return po_message_msgstr (msg->priv->message);
}


/**
 * gtr_msg_set_msgstr:
 * @msg: a #GtrMsg
 * @msgstr: the string to set in the @msg
 * 
 * Change the msgstr (translation) of a message.
 * Use an empty string to denote an untranslated message.
 **/
void
gtr_msg_set_msgstr (GtrMsg * msg, const gchar * msgstr)
{
  g_return_if_fail (GTR_IS_MSG (msg));
  g_return_if_fail (msgstr != NULL);

  po_message_set_msgstr (msg->priv->message, msgstr);
}


/**
 * gtr_msg_get_msgstr_plural:
 * @msg: a #GtrMsg
 * @index: the index of the plural array
 *
 * Return value: (transfer none): the msgstr[index] for a message with plural
 *               handling, or NULL when the index is out of range or for a
 *               message without plural.
 **/
const gchar *
gtr_msg_get_msgstr_plural (GtrMsg * msg, gint index)
{
  g_return_val_if_fail (GTR_IS_MSG (msg), NULL);

  return po_message_msgstr_plural (msg->priv->message, index);
}

/**
 * gtr_msg_set_msgstr_plural:
 * @msg: a #GtrMsg
 * @index: the index where to set the msgstr
 * @msgstr: the message to set in the msg
 *
 * Change the msgstr[index] for a message with plural handling.
 * Use a NULL value at the end to reduce the number of plural forms.
 **/
void
gtr_msg_set_msgstr_plural (GtrMsg * msg, gint index, const gchar * msgstr)
{
  g_return_if_fail (GTR_IS_MSG (msg));
  g_return_if_fail (msgstr != NULL);

  po_message_set_msgstr_plural (msg->priv->message, index, msgstr);
}


/**
 * gtr_msg_get_comment:
 * @msg: a #GtrMsg
 *
 * Return value: (transfer none) the comments for a message.
 **/
const gchar *
gtr_msg_get_comment (GtrMsg * msg)
{
  g_return_val_if_fail (GTR_IS_MSG (msg), NULL);

  return po_message_comments (msg->priv->message);
}

/**
 * gtr_msg_set_comment:
 * @msg: a #GtrMsg
 * @comment: the comment to set for a message
 * 
 * Change the comments for a message.
 * comments should be a multiline string, 
 * ending in a newline, or empty.
 **/
void
gtr_msg_set_comment (GtrMsg * msg, const gchar * comment)
{
  g_return_if_fail (GTR_IS_MSG (msg));
  g_return_if_fail (comment != NULL);

  po_message_set_comments (msg->priv->message, comment);
}

/**
 * gtr_msg_get_po_position:
 * @msg: a #GtrMsg
 *
 * Return value: the position of the message.
 *
 * Gets the position of this message in the PO file in relation to the other
 * messages.
 **/
gint
gtr_msg_get_po_position (GtrMsg * msg)
{
  g_return_val_if_fail (GTR_IS_MSG (msg), 0);

  return msg->priv->po_position;
}

/**
 * gtr_msg_set_po_position:
 * @msg: a #GtrMsg
 * @po_position: the numerical position of the message.
 *
 * Sets the numerical position of this message in relation to other messages.
 **/
void
gtr_msg_set_po_position (GtrMsg * msg, gint po_position)
{
  g_return_if_fail (GTR_IS_MSG (msg));

  msg->priv->po_position = po_position;
}

/**
 * gtr_msg_get_extracted_comments:
 * @msg: a #GtrMsg
 *
 * Return value: (transfer none): the extracted comments for a message.
 **/
const gchar *
gtr_msg_get_extracted_comments (GtrMsg * msg)
{
  g_return_val_if_fail (GTR_IS_MSG (msg), NULL);

  return po_message_extracted_comments (msg->priv->message);
}

/**
 * gtr_msg_get_filename:
 * @msg: a #GtrMsg
 * @i: the i-th file for a message.
 *
 * Return value: (transfer none): the i-th filename for a message, or NULL if
 *               @i is out of range.
 */
const gchar *
gtr_msg_get_filename (GtrMsg * msg, gint i)
{
  g_return_val_if_fail (GTR_IS_MSG (msg), NULL);

  po_filepos_t filepos;

  filepos = po_message_filepos (msg->priv->message, i);

  if (filepos == NULL)
    return NULL;

  return po_filepos_file (filepos);
}

/**
 * gtr_msg_get_file_line:
 * @msg: a #GtrMsg
 * @i: the i-th file for a message.
 *
 * Return value: (transfer none): the i-th file line for a message, or NULL if
 *               @i is out of range.
 */
gint *
gtr_msg_get_file_line (GtrMsg * msg, gint i)
{
  g_return_val_if_fail (GTR_IS_MSG (msg), (gint *) 0);

  po_filepos_t filepos;

  filepos = po_message_filepos (msg->priv->message, i);

  if (filepos == NULL)
    return NULL;

  return (gint *) po_filepos_start_line (filepos);
}

/**
 * gtr_msg_get_msgctxt:
 * @msg: a #GtrMsg
 *
 * Return value: (transfer none): the context of a message, or NULL for a
 *               message not restricted to a context.
 */
const gchar *
gtr_msg_get_msgctxt (GtrMsg * msg)
{
  g_return_val_if_fail (GTR_IS_MSG (msg), NULL);

  return po_message_msgctxt (msg->priv->message);
}

/**
 * gtr_msg_get_format:
 * @msg: a #GtrMsg
 *
 * Return the pretty name associated with a format type.
 * For example, for "csharp-format", return "C#".
 * Return NULL if the are no format type in the message.
 * 
 * Return value: (transfer none): the pretty name associated with a format type
 *               or NULL if the message hasn't any format type.
 */
const gchar *
gtr_msg_get_format (GtrMsg * msg)
{
  const gchar *const *format_list;
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
on_gettext_po_xerror (gint severity,
                      po_message_t message,
                      const gchar * filename, size_t lineno, size_t column,
                      gint multiline_p, const gchar * message_text)
{
  if (message_text)
    message_error = g_strdup (message_text);
  else
    message_error = NULL;
}

static void
on_gettext_po_xerror2 (gint severity,
                       po_message_t message1,
                       const gchar * filename1, size_t lineno1,
                       size_t column1, gint multiline_p1,
                       const gchar * message_text1, po_message_t message2,
                       const gchar * filename2, size_t lineno2,
                       size_t column2, gint multiline_p2,
                       const gchar * message_text2)
{
  g_warning ("Error: %s.\n %s", message_text1, message_text2);
}

/**
 * gtr_msg_check:
 * @msg: a #GtrMsg
 * 
 * Test whether the message translation is a valid format string if the message
 * is marked as being a format string.  
 *
 * Return value: (transfer full): the message error or NULL if there is not any
 *               error. Must be freed with g_free.
 **/
gchar *
gtr_msg_check (GtrMsg * msg)
{
  struct po_xerror_handler handler;

  g_return_val_if_fail (msg != NULL, NULL);

  /* We are not freeing the message_error so at start should be NULL
   * always for us
   */
  message_error = NULL;

  handler.xerror = &on_gettext_po_xerror;
  handler.xerror2 = &on_gettext_po_xerror2;

  po_message_check_all (msg->priv->message, msg->priv->iterator, &handler);

  if (gtr_msg_is_fuzzy (msg) || !gtr_msg_is_translated (msg))
    {
      if (message_error)
        g_free (message_error);
      message_error = NULL;
    }

  /*Are there any other way to do this? */
  return message_error;
}
