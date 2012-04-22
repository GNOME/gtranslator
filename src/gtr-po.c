/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *                     Fatih Demir <kabalak@kabalak.net>
 *		       Ross Golder <ross@golder.org>
 *		       Gediminas Paulauskas <menesis@kabalak.net>
 *               2008  Pablo Sanxiao <psanxiao@gmail.com>
 *                     Igalia
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
 *   Pablo Sanxiao <psanxiao@gmail.com>
 *   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *   Fatih Demir <kabalak@kabalak.net>
 *   Ross Golder <ross@golder.org>
 *   Gediminas Paulauskas <menesis@kabalak.net>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-application.h"
#include "gtr-debug.h"
#include "gtr-file-dialogs.h"
#include "gtr-po.h"
#include "gtr-msg.h"
#include "gtr-enum-types.h"
#include "gtr-profile.h"
#include "gtr-utils.h"
#include "gtr-message-container.h"

#include <string.h>
#include <errno.h>

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gettext-po.h>
#include <gio/gio.h>

static void gtr_po_message_container_init (GtrMessageContainerInterface *iface);

#define GTR_PO_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_PO,     \
					 GtrPoPrivate))


G_DEFINE_TYPE_WITH_CODE (GtrPo, gtr_po, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GTR_TYPE_MESSAGE_CONTAINER,
                                                gtr_po_message_container_init))

struct _GtrPoPrivate
{
  /* The location of the file to open */
  GFile *location;

  /* Gettext's file handle */
  po_file_t gettext_po_file;

  /* Message iter */
  po_message_iterator_t iter;

  /* The message domains in this file */
  GList *domains;

  /* Parsed list of GtrMsgs for the current domains' messagelist */
  GList *messages;

  /* A pointer to the currently displayed message */
  GList *current;

  /* The obsolete messages are stored within this gchar. */
  gchar *obsolete;

  /* Is the file write-permitted? (read-only) */
  gboolean no_write_perms;

  /* Translated entries count */
  guint translated;

  /* Fuzzy entries count */
  guint fuzzy;

  /* Autosave timeout timer */
  guint autosave_timeout;

  /* Header object */
  GtrHeader *header;

  GtrPoState state;

  /* Marks if the file was changed;  */
  guint file_changed : 1;
};

enum
{
  PROP_0,
  PROP_LOCATION,
  PROP_STATE
};

static gchar *message_error = NULL;

static void
gtr_po_set_property (GObject      *object,
                     guint         prop_id,
                     const GValue *value,
                     GParamSpec   *pspec)
{
  GtrPo *po = GTR_PO (object);

  switch (prop_id)
    {
    case PROP_LOCATION:
      gtr_po_set_location (po, G_FILE (g_value_get_object (value)));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtr_po_get_property (GObject * object,
                     guint prop_id, GValue * value, GParamSpec * pspec)
{
  GtrPo *po = GTR_PO (object);

  switch (prop_id)
    {
    case PROP_LOCATION:
      g_value_take_object (value, gtr_po_get_location (po));
      break;
    case PROP_STATE:
      g_value_set_enum (value, gtr_po_get_state (po));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

/*
 * A helper function simply increments the "translated" variable of the
 *  po-file.
 */
static void
determine_translation_status (GtrMsg * msg, GtrPo * po)
{
  if (gtr_msg_is_fuzzy (msg))
    po->priv->fuzzy++;
  else if (gtr_msg_is_translated (msg))
    po->priv->translated++;
}

/*
 * Update the count of the completed translated entries.
 */
static void
gtr_po_update_translated_count (GtrPo * po)
{
  po->priv->translated = 0;
  po->priv->fuzzy = 0;
  g_list_foreach (po->priv->messages,
                  (GFunc) determine_translation_status, po);
}

static void
gtr_po_init (GtrPo * po)
{
  po->priv = GTR_PO_GET_PRIVATE (po);

  po->priv->location = NULL;
  po->priv->gettext_po_file = NULL;
}

static void
gtr_po_finalize (GObject * object)
{
  GtrPo *po = GTR_PO (object);

  g_list_free_full (po->priv->messages, g_object_unref);
  g_list_free_full (po->priv->domains, g_free);
  g_free (po->priv->obsolete);

  if (po->priv->gettext_po_file)
    po_file_free (po->priv->gettext_po_file);

  G_OBJECT_CLASS (gtr_po_parent_class)->finalize (object);
}

static void
gtr_po_dispose (GObject * object)
{
  GtrPo *po = GTR_PO (object);

  g_clear_object (&po->priv->location);

  G_OBJECT_CLASS (gtr_po_parent_class)->dispose (object);
}

static GtrMsg *
gtr_po_message_container_get_message (GtrMessageContainer *container,
                                      gint number)
{
  GtrPo *po = GTR_PO (container);

  return g_list_nth_data (po->priv->messages, number);
}

static gint
gtr_po_message_container_get_message_number (GtrMessageContainer * container,
                                             GtrMsg * msg)
{
  GtrPo *po = GTR_PO (container);
  GList *list;

  list = g_list_find (po->priv->messages, msg);
  return g_list_position (po->priv->messages, list);
}

static gint
gtr_po_message_container_get_count (GtrMessageContainer * container)
{
  GtrPo *po = GTR_PO (container);

  return g_list_length (po->priv->messages);
}

static void
gtr_po_message_container_init (GtrMessageContainerInterface * iface)
{
  iface->get_message = gtr_po_message_container_get_message;
  iface->get_message_number = gtr_po_message_container_get_message_number;
  iface->get_count = gtr_po_message_container_get_count;
}

static void
gtr_po_class_init (GtrPoClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrPoPrivate));

  object_class->finalize = gtr_po_finalize;
  object_class->dispose = gtr_po_dispose;
  object_class->get_property = gtr_po_get_property;
  object_class->set_property = gtr_po_set_property;

  g_object_class_install_property (object_class,
                                   PROP_LOCATION,
                                   g_param_spec_object ("location",
                                                        "Location",
                                                        "The po's location",
                                                        G_TYPE_FILE,
                                                        G_PARAM_READWRITE));

  g_object_class_install_property (object_class,
                                   PROP_STATE,
                                   g_param_spec_enum ("state",
                                                      "State",
                                                      "The po's state",
                                                      GTR_TYPE_PO_STATE,
                                                      GTR_PO_STATE_SAVED,
                                                      G_PARAM_READABLE));
}

/*
 * Functions for errors handling.
 */
GQuark
gtr_po_error_quark (void)
{
  static GQuark quark = 0;
  if (!quark)
    quark = g_quark_from_static_string ("gtr_po_parser_error");
  return quark;
}

static void
on_gettext_po_xerror (gint severity,
                      po_message_t message,
                      const gchar * filename, size_t lineno, size_t column,
                      gint multiline_p, const gchar * message_text)
{
  message_error = g_strdup (message_text);
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
  message_error = g_strdup_printf ("%s.\n %s", message_text1, message_text2);
}

static gboolean
po_file_is_empty (po_file_t file)
{
  const gchar *const *domains = po_file_domains (file);

  for (; *domains != NULL; domains++)
    {
      po_message_iterator_t iter = po_message_iterator (file, *domains);
      if (po_next_message (iter) != NULL)
        {
          po_message_iterator_free (iter);
          return FALSE;
        }
      po_message_iterator_free (iter);
    }
  return TRUE;
}

/**
 * is_read_only:
 * @location: a GFile Object that represents the file to check
 *
 * This method is copied from gedit, file gedit-commands-file.c
 *
 * Returns: False if file is writeable. True if file doesn't exists, is read-only or read-only attribute can't be check
 */
static gboolean
is_read_only (const gchar * filename)
{
  gboolean ret = TRUE;          /* default to read only */
  GFileInfo *info;
  GFile *location;

  location = g_file_new_for_path (filename);

  if (!g_file_query_exists (location, NULL))
    return FALSE;

  info = g_file_query_info (location,
                            G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE,
                            G_FILE_QUERY_INFO_NONE, NULL, NULL);
  g_object_unref (location);

  if (info != NULL)
    {
      if (g_file_info_has_attribute (info, G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE))
        {
          ret = !g_file_info_get_attribute_boolean (info,
                                                    G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE);
        }

      g_object_unref (info);
    }

  return ret;
}

/***************************** Public funcs ***********************************/

/**
 * gtr_po_new:
 *
 * Creates a new #GtrPo.
 *
 * Returns: a new #GtrPo object
 **/
GtrPo *
gtr_po_new (void)
{
  GtrPo *po;

  po = g_object_new (GTR_TYPE_PO, NULL);

  return po;
}

static gboolean
_gtr_po_load (GtrPo * po, GFile * location, GError ** error)
{
  GtrPoPrivate *priv = po->priv;
  struct po_xerror_handler handler;
  po_message_iterator_t iter;
  po_message_t message;
  const gchar *msgstr;
  gchar *filename;

  /*
   * Initialize the handler error.
   */
  handler.xerror = &on_gettext_po_xerror;
  handler.xerror2 = &on_gettext_po_xerror2;

  if (message_error != NULL)
    {
      g_free (message_error);
      message_error = NULL;
    }

  filename = g_file_get_path (location);

  if (po->priv->gettext_po_file)
    po_file_free (po->priv->gettext_po_file);

  if (priv->header)
    {
      g_object_unref (priv->header);
      priv->header = NULL;
    }

  if (priv->iter)
    {
      po_message_iterator_free (priv->iter);
      priv->iter = NULL;
    }

  priv->gettext_po_file = po_file_read (filename, &handler);
  g_free (filename);

  if (po_file_is_empty (priv->gettext_po_file))
    {
      g_set_error (error,
                   GTR_PO_ERROR,
                   GTR_PO_ERROR_FILE_EMPTY, _("The file is empty"));
      return FALSE;
    }

  if (!po->priv->gettext_po_file)
    {
      g_set_error (error,
                   GTR_PO_ERROR,
                   GTR_PO_ERROR_FILENAME,
                   _("Failed opening file '%s': %s"),
                   filename, g_strerror (errno));
      g_free (filename);
      return FALSE;
    }

  iter = po_message_iterator (priv->gettext_po_file, NULL);
  message = po_next_message (iter);
  msgstr = po_message_msgstr (message);

  if (!strncmp (msgstr, "Project-Id-Version: ", 20))
    priv->header = gtr_header_new (iter, message);
  else
    {
      /* FIXME: add a header with default values */
      po_message_iterator_free (iter);
      iter = po_message_iterator (priv->gettext_po_file, NULL);
    }

  priv->iter = iter;

  return TRUE;
}

static gboolean
_gtr_po_load_ensure_utf8 (GtrPo * po, GError ** error)
{
  GMappedFile *mapped;
  const gchar *content;
  gboolean utf8_valid;
  gchar *filename;
  gsize size;

  filename = g_file_get_path (po->priv->location);
  mapped = g_mapped_file_new (filename, FALSE, error);
  g_free (filename);

  if (!mapped)
    return FALSE;

  content = g_mapped_file_get_contents (mapped);
  size = g_mapped_file_get_length (mapped);

  utf8_valid = g_utf8_validate (content, size, NULL);

  if (!_gtr_po_load (po, po->priv->location, error))
    {
      g_mapped_file_unref (mapped);
      return FALSE;
    }

  if (!utf8_valid &&
      po->priv->header)
    {
      gchar *charset = NULL;

      if (po->priv->header)
        charset = gtr_header_get_charset (po->priv->header);

      if (charset && *charset && strcmp (charset, "UTF-8") != 0)
        {
          GOutputStream *converter_stream, *stream;
          GCharsetConverter *converter;
          GIOStream *iostream;
          GFile *tmp;

          /* Store UTF-8 converted file in $TMP */
          converter = g_charset_converter_new ("UTF-8", charset, NULL);

          if (!converter)
            {
              g_set_error (error,
                           GTR_PO_ERROR,
                           GTR_PO_ERROR_ENCODING,
                           _("Could not convert from charset '%s' to UTF-8"),
                           charset);
              g_mapped_file_unref (mapped);
              g_free (charset);
              return FALSE;
            }

          g_free (charset);
          tmp = g_file_new_tmp ("gtranslator-XXXXXX.po",
                                (GFileIOStream **) &iostream,
                                NULL);

          if (!tmp)
            {
              g_set_error (error,
                           GTR_PO_ERROR,
                           GTR_PO_ERROR_ENCODING,
                           _("Could not store temporary "
                             "file for encoding conversion"));
              g_mapped_file_unref (mapped);
              g_object_unref (converter);
              return FALSE;
            }

          stream = g_io_stream_get_output_stream (iostream);
          converter_stream =
            g_converter_output_stream_new (stream,
                                           G_CONVERTER (converter));


          if (!g_output_stream_write_all (converter_stream,
                                          content, size, NULL,
                                          NULL, NULL))
            {
              g_set_error (error,
                           GTR_PO_ERROR,
                           GTR_PO_ERROR_ENCODING,
                           _("Could not store temporary "
                             "file for encoding conversion"));
              g_object_unref (converter_stream);
              g_object_unref (iostream);
              g_object_unref (converter);
              g_mapped_file_unref (mapped);
              return FALSE;
            }

          g_object_unref (converter_stream);
          g_object_unref (iostream);
          g_object_unref (converter);

          /* Now load again the converted file */
          if (!_gtr_po_load (po, tmp, error))
            {
              g_mapped_file_unref (mapped);
              return FALSE;
            }

          /* Ensure Content-Type is set correctly
           * in the header as per the content
           */
          if (po->priv->header)
            gtr_header_set_charset (po->priv->header, "UTF-8");

          utf8_valid = TRUE;
        }
    }

  g_mapped_file_unref (mapped);

  if (!utf8_valid)
    {
      g_set_error (error,
                   GTR_PO_ERROR,
                   GTR_PO_ERROR_ENCODING,
                   _("All attempt to convert the file to UTF-8 has failed, "
                     "use the msgconv or iconv command line tools before "
                     "opening this file with gtranslator"));
      return FALSE;
    }

  return TRUE;
}

/**
 * gtr_po_parse:
 * @po: a #GtrPo
 * @location: the file to open
 * @error: a variable to store the errors
 *
 * Parses all things related to the #GtrPo and initilizes all neccessary
 * variables.
 **/
void
gtr_po_parse (GtrPo * po, GFile * location, GError ** error)
{
  GtrPoPrivate *priv = po->priv;
  GtrMsg *msg;
  po_message_t message;
  po_message_iterator_t iter;
  const gchar *const *domains;
  gint i = 0;
  gint pos = 1;

  g_return_if_fail (GTR_IS_PO (po));
  g_return_if_fail (location != NULL);

  if (message_error != NULL)
    {
      g_free (message_error);
      message_error = NULL;
    }

  /*
   * Get filename path.
   */
  po->priv->location = g_file_dup (location);

  if (!_gtr_po_load_ensure_utf8 (po, error))
    {
      g_object_unref (po);
      return;
    }

  /*
   * No need to return; this can be corrected by the user
   */
  if (message_error != NULL)
    {
      g_set_error (error,
                   GTR_PO_ERROR, GTR_PO_ERROR_RECOVERY, "%s", message_error);
    }

  /*
   * Determine the message domains to track
   */
  if (!(domains = po_file_domains (priv->gettext_po_file)))
    {
      if (*error != NULL)
        g_clear_error (error);
      g_set_error (error,
                   GTR_PO_ERROR,
                   GTR_PO_ERROR_GETTEXT,
                   _("Gettext returned a null message domain list."));
      g_object_unref (po);
      return;
    }
  while (domains[i])
    {
      priv->domains = g_list_append (priv->domains, g_strdup (domains[i]));
      i++;
    }

  /*
   * Determine whether first message is the header or not, and
   * if so, process it seperately. Otherwise, treat as a normal
   * message.
   */
  priv->messages = NULL;
  iter = priv->iter;

  /* Post-process these into a linked list of GtrMsgs. */
  while ((message = po_next_message (iter)))
    {
      /*FIXME: We have to change this:
       * we have to add a gtr_msg_is_obsolete fund msg.c
       * and detect if we want obsoletes messages in show message
       */
      if (!po_message_is_obsolete (message))
        {
          /* Unpack into a GtrMsg */
          msg = _gtr_msg_new (iter, message);

          /* Set position in PO file */
          gtr_msg_set_po_position (msg, pos++);

          /* Build up messages */
          priv->messages = g_list_prepend (priv->messages, msg);
        }
    }

  if (priv->messages == NULL)
    {
      if (*error != NULL)
        g_clear_error (error);
      g_set_error (error,
                   GTR_PO_ERROR,
                   GTR_PO_ERROR_OTHER,
                   _("No messages obtained from parser."));
      g_object_unref (po);
      return;
    }

  priv->messages = g_list_reverse (priv->messages);

  /*
   * Set the current message to the first message.
   */
  priv->current = g_list_first (priv->messages);

  gtr_po_update_translated_count (po);

  /* Initialize Tab state */
  po->priv->state = GTR_PO_STATE_SAVED;
}

/**
 * gtr_po_save_file:
 * @po: a #GtrPo
 * @error: a GError to manage the exceptions
 *
 * It saves the po file and if there are any problem it stores the error
 * in @error.
 **/
void
gtr_po_save_file (GtrPo * po, GError ** error)
{
  struct po_xerror_handler handler;
  gchar *filename;
  GtrHeader *header;

  /*
   * Initialice the handler error.
   */
  handler.xerror = &on_gettext_po_xerror;
  handler.xerror2 = &on_gettext_po_xerror2;

  filename = g_file_get_path (po->priv->location);

  if (g_str_has_suffix (filename, ".pot"))
    {
      // Remove suffix
      filename[strlen (filename) - 4] = '\0';
      g_set_error (error,
                   GTR_PO_ERROR,
                   GTR_PO_ERROR_FILENAME,
                   _("You are saving a file with a .pot extension.\n"
                     "Pot files are generated by the compilation process.\n"
                     "Your file should likely be named '%s.po'."), filename);
      g_free (filename);
      return;
    }


  if (is_read_only (filename))
    {
      g_set_error (error,
                   GTR_PO_ERROR,
                   GTR_PO_ERROR_READONLY,
                   _("The file %s is read-only, and can not be overwritten"),
                   filename);
      g_free (filename);
      return;
    }


  /*
   * Check if the file is right
   */
  /*msg_error = gtr_po_check_po_file (po);
     if (msg_error != NULL)
     {
     g_set_error (error,
     GTR_PO_ERROR,
     GTR_PO_ERROR_GETTEXT,
     _("There is an error in the PO file: %s"),
     msg_error);
     g_free (msg_error);
     } */


  /* Save header fields into msg */
  header = gtr_po_get_header (po);
  gtr_header_update_header (header);

  if (!po_file_write (gtr_po_get_po_file (po), filename, &handler))
    {
      g_set_error (error,
                   GTR_PO_ERROR,
                   GTR_PO_ERROR_FILENAME,
                   _("There was an error writing the PO file: %s"),
                   message_error);
      g_free (message_error);
      g_free (filename);
      return;
    }
  g_free (filename);

  /* If we are here everything is ok and we can set the state as saved */
  gtr_po_set_state (po, GTR_PO_STATE_SAVED);

  /*
   * If the warn if fuzzy option is enabled we have to show an error
   */
  /*if (gtr_prefs_manager_get_warn_if_fuzzy () && po->priv->fuzzy)
     {
     g_set_error (error,
     GTR_PO_ERROR,
     GTR_PO_ERROR_OTHER,
     ngettext ("File %s\ncontains %d fuzzy message",
     "File %s\ncontains %d fuzzy messages",
     po->priv->fuzzy),
     po->priv->fuzzy);
     } */
}

/**
 * gtr_po_get_location:
 * @po: a #GtrPo
 *
 * Gets the GFile of the po file.
 *
 * Returns: (transfer full): the GFile associated with the @po. The returned
 *          location must be freed with g_object_unref.
 **/
GFile *
gtr_po_get_location (GtrPo * po)
{
  g_return_val_if_fail (GTR_IS_PO (po), NULL);

  return g_file_dup (po->priv->location);
}

/**
 * gtr_po_set_location:
 * @po: a #GtrPo
 * @location: The GFile to set to the #GtrPo
 *
 * Sets the GFile location within the #GtrPo object.
 **/
void
gtr_po_set_location (GtrPo * po, GFile * location)
{
  g_return_if_fail (GTR_IS_PO (po));

  if (po->priv->location)
    {
      if (g_file_equal (po->priv->location, location))
        return;
      g_object_unref (po->priv->location);
    }

  po->priv->location = g_file_dup (location);

  g_object_notify (G_OBJECT (po), "location");
}

/**
 * gtr_po_get_state:
 * @po: a #GtrPo
 *
 * Return value: the #GtrPoState value of the @po.
 */
GtrPoState
gtr_po_get_state (GtrPo * po)
{
  g_return_val_if_fail (GTR_IS_PO (po), 0);

  return po->priv->state;
}

/**
 * gtr_po_set_state:
 * @po: a #GtrPo
 * @state: a #GtrPoState
 *
 * Sets the state for a #GtrPo
 */
void
gtr_po_set_state (GtrPo * po, GtrPoState state)
{
  g_return_if_fail (GTR_IS_PO (po));

  po->priv->state = state;

  g_object_notify (G_OBJECT (po), "state");
}

/*
 * FIXME: We are not using this func.
 */
gboolean
gtr_po_get_write_perms (GtrPo * po)
{
  return po->priv->no_write_perms;
}

/**
 * gtr_po_get_messages:
 * @po: a #GtrPo
 *
 * Return value: (transfer container): a pointer to the messages list
 **/
GList *
gtr_po_get_messages (GtrPo * po)
{
  g_return_val_if_fail (GTR_IS_PO (po), NULL);

  return po->priv->messages;
}

/**
 * FIXME: this is hack, we should fix it
 * gtr_po_set_messages:
 * @po: a #GtrPo
 * @messages: a pointer to a new messages list.
 *
 * Sets an updated list of messages.
 **/
void
gtr_po_set_messages (GtrPo * po, GList * messages)
{
  g_return_if_fail (GTR_IS_PO (po));

  po->priv->messages = messages;
}

/**
 * gtr_po_get_current_message:
 * @po: a #GtrPo
 *
 * Return value: (transfer none) (element-type GtrMsg): a pointer to the current message
 **/
GList *
gtr_po_get_current_message (GtrPo * po)
{
  return po->priv->current;
}

/**
 * gtr_po_update_current_message:
 * @po: a #GtrPo
 * @msg: the message where should point the current message.
 *
 * Sets the new current message to the message that is passed in
 * the argument.
 **/
void
gtr_po_update_current_message (GtrPo * po, GtrMsg * msg)
{
  gint i;
  i = g_list_index (po->priv->messages, msg);
  po->priv->current = g_list_nth (po->priv->messages, i);
}

/**
 * gtr_po_get_domains:
 * @po: a #GtrPo
 *
 * Return value: (transfer none): a pointer to the domains list
 **/
GList *
gtr_po_get_domains (GtrPo * po)
{
  return po->priv->domains;
}

/**
 * gtr_po_get_po_file:
 * @po: a #GtrPo
 *
 * Gets the gettext file.
 *
 * Return value: the gettext file
 **/
po_file_t
gtr_po_get_po_file (GtrPo * po)
{
  return po->priv->gettext_po_file;
}

/**
 * gtr_po_get_next_fuzzy:
 * @po: a #GtrPo
 *
 * Return value: (transfer none): a pointer to the next fuzzy message
 **/
GList *
gtr_po_get_next_fuzzy (GtrPo * po)
{
  GList *msg;

  msg = po->priv->current;
  while ((msg = g_list_next (msg)))
    {
      if (gtr_msg_is_fuzzy (msg->data))
        return msg;
    }

  return NULL;
}


/**
 * gtr_po_get_prev_fuzzy:
 * @po: a #GtrPo
 *
 * Return value: (transfer none): a pointer to the previously fuzzy message
 **/
GList *
gtr_po_get_prev_fuzzy (GtrPo * po)
{
  GList *msg;

  msg = po->priv->current;
  while ((msg = g_list_previous (msg)))
    {
      if (gtr_msg_is_fuzzy (msg->data))
        return msg;
    }

  return NULL;
}


/**
 * gtr_po_get_next_untrans:
 * @po: a #GtrPo
 *
 * Return value: (transfer none): a pointer to the next untranslated message
 **/
GList *
gtr_po_get_next_untrans (GtrPo * po)
{
  GList *msg;

  msg = po->priv->current;
  while ((msg = g_list_next (msg)))
    {
      if (!gtr_msg_is_translated (msg->data))
        return msg;
    }

  return NULL;
}


/**
 * gtr_po_get_prev_untrans:
 * @po: a #GtrPo
 *
 * Return value: (transfer none): a pointer to the previously untranslated
 *                message or NULL if there are not previously untranslated
 *                message.
 **/
GList *
gtr_po_get_prev_untrans (GtrPo * po)
{
  GList *msg;

  msg = po->priv->current;
  while ((msg = g_list_previous (msg)))
    {
      if (!gtr_msg_is_translated (msg->data))
        return msg;
    }

  return NULL;
}

/**
 * gtr_po_get_next_fuzzy_or_untrans:
 * @po: a #GtrPo
 *
 * Return value: (transfer none): a pointer to the next fuzzy or untranslated
 *               message or NULL if there is not next fuzzy or untranslated
 *               message.
 **/
GList *
gtr_po_get_next_fuzzy_or_untrans (GtrPo * po)
{
  GList *msg;

  msg = po->priv->current;
  while ((msg = g_list_next (msg)))
    {
      if (gtr_msg_is_fuzzy (msg->data) || !gtr_msg_is_translated (msg->data))
        return msg;
    }

  return NULL;
}

/**
 * gtr_po_get_prev_fuzzy_or_untrans:
 * @po: a #GtrPo
 *
 * Return value: (transfer none): a pointer to the previously fuzzy or
 *               untranslated message or NULL if there is not previously 
 *               fuzzy or untranslated message.
 **/
GList *
gtr_po_get_prev_fuzzy_or_untrans (GtrPo * po)
{
  GList *msg;

  msg = po->priv->current;
  while ((msg = g_list_previous (msg)))
    {
      if (gtr_msg_is_fuzzy (msg->data) || !gtr_msg_is_translated (msg->data))
        return msg;
    }

  return NULL;
}

/**
 * gtr_po_get_msg_from_number:
 * @po: a #GtrPo
 * @number: the message to jump
 *
 * Gets the message at the given position.
 *
 * Returns: (transfer none): the message at the given position.
 */
GList *
gtr_po_get_msg_from_number (GtrPo * po, gint number)
{
  g_return_val_if_fail (GTR_IS_PO (po), NULL);

  return g_list_nth (po->priv->messages, number);
}

/**
 * gtr_po_get_header:
 * @po: a #GtrPo
 *
 * Return value: (transfer none): The #GtrHeader of the @po.
 **/
GtrHeader *
gtr_po_get_header (GtrPo * po)
{
  g_return_val_if_fail (GTR_IS_PO (po), NULL);

  return po->priv->header;
}

/**
 * gtr_po_get_translated_count:
 * @po: a #GtrPo
 *
 * Return value: the count of the translated messages.
 **/
gint
gtr_po_get_translated_count (GtrPo * po)
{
  g_return_val_if_fail (GTR_IS_PO (po), -1);

  return po->priv->translated;
}

/*
 * This func decrease or increase the count of translated
 * messages in 1.
 * This funcs must not be exported.
 */
void
_gtr_po_increase_decrease_translated (GtrPo * po, gboolean increase)
{
  g_return_if_fail (GTR_IS_PO (po));

  if (increase)
    po->priv->translated++;
  else
    po->priv->translated--;
}

/**
 * gtr_po_get_fuzzy_count:
 * @po: a #GtrPo
 *
 * Return value: the count of the fuzzy messages.
 **/
gint
gtr_po_get_fuzzy_count (GtrPo * po)
{
  g_return_val_if_fail (GTR_IS_PO (po), -1);

  return po->priv->fuzzy;
}

/*
 * This func decrease or increase the count of fuzzy
 * messages in 1.
 * This funcs must not be exported.
 */
void
_gtr_po_increase_decrease_fuzzy (GtrPo * po, gboolean increase)
{
  g_return_if_fail (GTR_IS_PO (po));

  if (increase)
    po->priv->fuzzy++;
  else
    po->priv->fuzzy--;
}

/**
 * gtr_po_get_untranslated_count:
 * @po: a #GtrPo
 *
 * Return value: the count of the untranslated messages.
 **/
gint
gtr_po_get_untranslated_count (GtrPo * po)
{
  g_return_val_if_fail (GTR_IS_PO (po), -1);

  /*DEBUG_PRINT ("length: %d | translated: %d | fuzzy: %d | untranslated: %d", g_list_length (po->priv->messages),
     po->priv->translated, po->priv->fuzzy, (g_list_length (po->priv->messages) - po->priv->translated - po->priv->fuzzy)); */

  return (g_list_length (po->priv->messages) - po->priv->translated -
          po->priv->fuzzy);
}

/**
 * gtr_po_get_messages_count:
 * @po: a #GtrPo
 *
 * Return value: the number of messages messages.
 **/
gint
gtr_po_get_messages_count (GtrPo * po)
{
  g_return_val_if_fail (GTR_IS_PO (po), -1);

  return g_list_length (po->priv->messages);
}

/**
 * gtr_po_get_message_position:
 * @po: a #GtrPo
 *
 * Return value: the number of the current message.
 **/
gint
gtr_po_get_message_position (GtrPo * po)
{
  g_return_val_if_fail (GTR_IS_PO (po), -1);

  return gtr_msg_get_po_position (GTR_MSG (po->priv->current->data));
}

/**
 * gtr_po_check_po_file:
 * @po: a #GtrPo
 *
 * Test whether an entire PO file is valid, like msgfmt does it.
 * Returns: If it is invalid, returns the error. The return value must be freed
 * with g_free.
 **/
gchar *
gtr_po_check_po_file (GtrPo * po)
{
  struct po_xerror_handler handler;

  g_return_val_if_fail (po != NULL, NULL);

  handler.xerror = &on_gettext_po_xerror;
  handler.xerror2 = &on_gettext_po_xerror2;
  message_error = NULL;

  po_file_check_all (po->priv->gettext_po_file, &handler);

  return message_error;
}
