/*
 * Copyright (C) 2007   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *			Fatih Demir <kabalak@kabalak.net>
 *			Ross Golder <ross@golder.org>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Pablo Sanxiao <psanxiao@gmail.com>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANPOILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "file-dialogs.h"
#include "po.h"
#include "msg.h"

#include <string.h>
#include <errno.h>

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gettext-po.h>

#define GTR_PO_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_PO,     \
					 GtranslatorPoPrivate))


G_DEFINE_TYPE(GtranslatorPo, gtranslator_po, G_TYPE_OBJECT)

struct _GtranslatorPoPrivate
{
	/*
	 * Absolute file name
	 */
	gchar *filename;
	
	/*
	 * Gettext's file handle
	 */
	po_file_t gettext_po_file;
	
	/*
	 * Message iter
	 */
	po_message_iterator_t iter;

	/*
	 * The message domains in this file
	 */
	GList *domains;
	
	/*
	 * Parsed list of GtrMsgs for the current domains' messagelist
	 */
	GList *messages;
	
	/*
	 * A pointer to the currently displayed message 
	 */
	GList *current;
	
	/*
	 * The obsolete messages are stored within this gchar.
	 */
	gchar *obsolete;
	
	/*
	 * Marks if the file was changed; 
	 */
	guint file_changed : 1;
	
	/*
	 * Is the file write-permitted? (read-only)
	 */
	gboolean no_write_perms;

	/*
	 * Translated entries count
	 */
	guint translated;
	
	/*
	 * Fuzzy entries count
	 */
	guint fuzzy;

	/*
	 * Autosave timeout timer
	 */
	guint autosave_timeout;

	/*
	 * Header object
	 */
	GtranslatorHeader *header;
};

static gchar *message_error = NULL;

/*
 * A helper function simply increments the "translated" variable of the
 *  po-file.
 */
static void 
determine_translation_status(GtranslatorMsg *msg,
			     GtranslatorPo *po)
{
	if(gtranslator_msg_is_fuzzy(msg))
		po->priv->fuzzy++;
	else if(gtranslator_msg_is_translated(msg))
		po->priv->translated++;
}

/*
 * Update the count of the completed translated entries.
 */
static void 
gtranslator_po_update_translated_count(GtranslatorPo *po)
{
	po->priv->translated = 0;
	po->priv->fuzzy = 0;
	g_list_foreach(po->priv->messages,
		       (GFunc) determine_translation_status,
		       po);
}

static void
gtranslator_po_init (GtranslatorPo *po)
{
	po->priv = GTR_PO_GET_PRIVATE (po);
}

static void
gtranslator_po_finalize (GObject *object)
{
	GtranslatorPo *po = GTR_PO(object);

	if (po->priv->messages) {
		g_list_foreach (po->priv->messages, (GFunc)g_object_unref, NULL);
		g_list_free (po->priv->messages);
	}
	if (po->priv->domains)
		g_list_free (po->priv->domains);

	g_free (po->priv->filename);
	g_free (po->priv->obsolete);

	if (po->priv->gettext_po_file)
		po_file_free (po->priv->gettext_po_file);

	G_OBJECT_CLASS (gtranslator_po_parent_class)->finalize(object);
}

static void
gtranslator_po_class_init (GtranslatorPoClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorPoPrivate));

	object_class->finalize = gtranslator_po_finalize;
}

/*
 * Functions for errors handling.
 */
GQuark 
gtranslator_po_error_quark (void)
{
	static GQuark quark = 0;
	if (!quark)
		quark = g_quark_from_static_string ("gtranslator_po_parser_error");
	return quark;
}

static void
on_gettext_po_xerror(gint severity,
		     po_message_t message,
		     const gchar *filename, size_t lineno, size_t column,
		     gint multiline_p, const gchar *message_text)
{
	message_error = g_strdup(message_text);
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
	message_error = g_strdup_printf("%s.\n %s",message_text1, message_text2);
}

/***************************** Public funcs ***********************************/

/**
 * gtranslator_po_new:
 *
 * Return value: a new #GtranslatorPo object
 **/
GtranslatorPo *
gtranslator_po_new(void)
{
	GtranslatorPo *po;
	
	po = g_object_new(GTR_TYPE_PO, NULL);
	
	return po;
}

/**
 * gtranslator_po_parse:
 * @po: a #GtranslatorPo
 * @filename: the filename path to open
 * @error: a variable to store the errors
 *
 * 
 **/
void
gtranslator_po_parse(GtranslatorPo *po,
		     const gchar *filename,
		     GError **error)
{
	GtranslatorPoPrivate *priv = po->priv;
	GtranslatorMsg *msg;
	struct po_xerror_handler handler;
	po_message_t message;
	po_message_iterator_t iter;
	const gchar *msgstr;
	const gchar * const *domains;
	gchar *base;
	gint i = 0;
	gint pos = 1;
	
	g_return_if_fail(filename!=NULL);
	g_return_if_fail(GTR_IS_PO(po));

	base=g_path_get_basename(filename);
	g_return_if_fail(base[0]!='\0');
	g_free(base);
	
	/*
	 * Initialice the handler error.
	 */
	handler.xerror = &on_gettext_po_xerror;
	handler.xerror2 = &on_gettext_po_xerror2;
	
	if(message_error != NULL)
	{
		g_free(message_error);
		message_error = NULL;
	}

	/*
	 * Get absolute filename.
	 */
	if (!g_path_is_absolute(filename)) 
		priv->filename = g_build_filename(filename);
	else
		priv->filename = g_strdup(filename);
	
	priv->gettext_po_file = po_file_read(priv->filename,
					     &handler);
	if(priv->gettext_po_file == NULL)
	{
		g_set_error(error,
			    GTR_PO_ERROR,
			    GTR_PO_ERROR_FILENAME,
			    _("Failed opening file '%s': %s"),
			    priv->filename, g_strerror(errno));
		g_object_unref(po);
		return;
	}
	
	/*
	 * No need to return; this can be corrected by the user
	 */
	if(message_error != NULL) {
		g_set_error(error,
			    GTR_PO_ERROR,
			    GTR_PO_ERROR_GETTEXT,
			    message_error);
	}
	
	/*
	 * Determine the message domains to track
	 */
	if(!(domains = po_file_domains(priv->gettext_po_file))) {
		g_set_error(error,
			    GTR_PO_ERROR,
			    GTR_PO_ERROR_GETTEXT,
			    _("Gettext returned a null message domain list."));
		g_object_unref(po);
		return;
	}
	while(domains[i]) {
		priv->domains = g_list_append(priv->domains, g_strdup(domains[i]));
		i++;
	}

	/*
	 * Determine whether first message is the header or not, and
	 * if so, process it seperately. Otherwise, treat as a normal
	 * message.
	 */
	priv->messages = NULL;
	priv->iter = iter = po_message_iterator(priv->gettext_po_file, NULL);
	message = po_next_message(iter);
	msgstr = po_message_msgstr(message);
	if (!strncmp(msgstr, "Project-Id-Version: ", 20)) {
		
		/* Parse into our header structure */

		priv->header = gtranslator_header_new();		
		
		gchar *comment, *prj_id_version, *rmbt, *pot_date, *po_date,
		      *translator, *tr_email, *language, *lg_email, *mime_version,
		      *charset, *encoding;

		gchar *space1, *space2, *space3;

		comment = g_strdup(po_message_comments(message));
		
		prj_id_version = po_header_field(msgstr, "Project-Id-Version");
		rmbt = g_strdup(po_header_field(msgstr, "Report-Msgid-Bugs-To"));
		pot_date = g_strdup(po_header_field(msgstr, "POT-Creation-Date"));
		po_date = g_strdup(po_header_field(msgstr, "PO-Revision-Date"));

		gchar *translator_temp = po_header_field(msgstr, "Last-Translator");
		space1 = g_strrstr(translator_temp, " <");

		if (!space1)
		{
			translator = g_strdup(translator_temp);
			tr_email = g_strdup("");
		} else {
			translator = g_strndup(translator_temp, space1 - translator_temp);
			tr_email = g_strndup(space1 + 2, strlen(space1)-3);
		}

		gchar *language_temp = po_header_field(msgstr, "Language-Team");
		space2 = g_strrstr(language_temp, " <");
		
		if (!space2)
		{
			language = g_strdup(language_temp);
			lg_email = g_strdup("");
		}else {
			language = g_strndup(language_temp, space2 - language_temp);
			lg_email = g_strndup(space2 + 2, strlen(space2)-3);
		}

		mime_version = g_strdup(po_header_field(msgstr, "MIME-Version"));

		gchar *charset_temp = g_strdup(po_header_field(msgstr, "Content-Type"));
		space3 = g_strrstr(charset_temp, "=");

		if (!space3)
		{
			charset = g_strdup("");
		}else {	
			charset = g_strdup(space3 +1);
		}
		
		encoding = g_strdup(po_header_field(msgstr, "Content-Transfer-Encoding"));
	
		gtranslator_header_set_comment(priv->header, comment);
		gtranslator_header_set_prj_id_version(priv->header, prj_id_version);
		gtranslator_header_set_rmbt(priv->header, rmbt);
		gtranslator_header_set_pot_date(priv->header, pot_date);
		gtranslator_header_set_po_date(priv->header, po_date);
		gtranslator_header_set_translator(priv->header, translator);
		gtranslator_header_set_tr_email(priv->header, tr_email);
		gtranslator_header_set_language(priv->header, language);
		gtranslator_header_set_lg_email(priv->header, lg_email);
		gtranslator_header_set_mime_version(priv->header, mime_version);
		gtranslator_header_set_charset(priv->header, charset);
		gtranslator_header_set_encoding(priv->header, encoding);
		
		g_free(translator_temp);
		g_free(language_temp);
		g_free(charset_temp);
	}
	else {
		/* Reset our pointer */
		iter = po_message_iterator(priv->gettext_po_file, NULL);
	}
		
	/*
	 * Post-process these into a linked list of GtrMsgs.
	 */
	while ((message = po_next_message (iter)))
	{
		/*FIXME: We have to change this:
		 * we have to add a gtranslator_msg_is_obsolete fund msg.c
		 * and detect if we want obsoletes messages in show message
		 */
		if (!po_message_is_obsolete (message))
		{
			/* Unpack into a GtrMsg */
			msg = gtranslator_msg_new (iter);
			gtranslator_msg_set_message (msg, message);
  
			/* Set the status */
			if (gtranslator_msg_is_fuzzy (msg))
				gtranslator_msg_set_status (msg, GTR_MSG_STATUS_FUZZY);
			else if (gtranslator_msg_is_translated (msg))
				gtranslator_msg_set_status (msg, GTR_MSG_STATUS_TRANSLATED);
			else gtranslator_msg_set_status (msg, GTR_MSG_STATUS_UNTRANSLATED);

			/* Set position in PO file */
			gtranslator_msg_set_po_position (msg, pos++);
			
			/* Build up messages */
			priv->messages = g_list_prepend (priv->messages, msg);
		}
	}
	
	if (priv->messages == NULL) {
		g_set_error(error,
			    GTR_PO_ERROR,
			    GTR_PO_ERROR_OTHER,
			    _("No messages obtained from parser."));
		g_object_unref(po);
		return;
	}
	
	priv->messages = g_list_reverse (priv->messages);

	/*
	 * Set the current message to the first message.
	 */
	priv->current = g_list_first(priv->messages);
	
	gtranslator_po_update_translated_count(po);
}

/**
 * gtranslator_po_get_filename:
 * @po: a #GtranslatorPo
 * 
 * Return value: the file name string
 **/
gchar *
gtranslator_po_get_filename(GtranslatorPo *po)
{
	return po->priv->filename;
}

/**
 * gtranslator_po_set_filename:
 * @po: a #GtranslatorPo
 * @data: The file name text you want to set
 *
 * Sets the text path within the #GtranslatorPo object. It overwrites any text
 * that was there before.
 **/
void
gtranslator_po_set_filename(GtranslatorPo *po,
			    gchar *data)
{
	g_return_if_fail(GTR_IS_PO(po));
	
	if(po->priv->filename)
		g_free(po->priv->filename);
	po->priv->filename = g_strdup(data);
}

gboolean
gtranslator_po_get_write_perms(GtranslatorPo *po)
{
	return po->priv->no_write_perms;
}

/**
 * gtranslator_po_get_messages:
 * @po: a #GtranslatorPo
 *
 * Return value: a pointer to the messages list
 **/
GList *
gtranslator_po_get_messages(GtranslatorPo *po)
{
	return po->priv->messages;
}

/**
 * gtranslator_po_set_messages:
 * @po: a #GtranslatorPo
 * @messages: a pointer to a new messages list.
 *
 * Sets an updated list of messages.
 **/
void
gtranslator_po_set_messages (GtranslatorPo *po,
			     GList *messages)
{
	po->priv->messages = messages;
}

/**
 * gtranslator_po_get_current_message:
 * @po: a #GtranslatorPo
 *
 * Return value: a pointer to the current message
 **/
GList *
gtranslator_po_get_current_message(GtranslatorPo *po)
{
	return po->priv->current;
}


/**
 * gtranslator_po_update_current_message:
 * @po: a #GtranslatorPo
 * @msg: the message where should point the current message.
 * 
 * Sets the new current message to the message that is passed in
 * the argument.
 **/
void
gtranslator_po_update_current_message(GtranslatorPo *po,
				      GtranslatorMsg *msg)
{
	gint i;
	i = g_list_index(po->priv->messages, msg);
	po->priv->current = g_list_nth(po->priv->messages, i);
}

/**
 * gtranslator_po_get_domains:
 * @po: a #GtranslatorPo
 *
 * Return value: a pointer to the domains list
 **/
GList *
gtranslator_po_get_domains(GtranslatorPo *po)
{
	return po->priv->domains;
}

/**
 * gtranslator_po_get_po_file:
 * @po: a #GtranslatorPo
 * 
 * Return value: the gettext file
 **/
po_file_t
gtranslator_po_get_po_file(GtranslatorPo *po)
{
	return po->priv->gettext_po_file;
}

/**
 * gtranslator_po_get_next_fuzzy:
 * @po: a #GtranslatorPo
 *
 * Return value: a pointer to the next fuzzy message
 **/
GList *
gtranslator_po_get_next_fuzzy(GtranslatorPo *po)
{
	GList *msg;

	msg = po->priv->current;
	while (msg = g_list_next(msg))
	{
		if (gtranslator_msg_is_fuzzy(msg->data))
			return msg;
	}

	return NULL;
}


/**
 * gtranslator_po_get_prev_fuzzy:
 * @po: a #GtranslatorPo
 *
 * Return value: a pointer to the previously fuzzy message
 **/
GList *
gtranslator_po_get_prev_fuzzy(GtranslatorPo *po)
{
	GList *msg;

	msg = po->priv->current;
	while (msg = g_list_previous(msg))
	{
		if (gtranslator_msg_is_fuzzy(msg->data))
			return msg;
	}

	return NULL;
}


/**
 * gtranslator_po_get_next_untrans:
 * @po: a #GtranslatorPo
 *
 * Return value: a pointer to the next untranslated message
 **/
GList *
gtranslator_po_get_next_untrans(GtranslatorPo *po)
{
	GList *msg;

	msg = po->priv->current;
	while (msg = g_list_next(msg))
	{
		if(!gtranslator_msg_is_translated(msg->data))
			return msg;
	}

	return NULL;
}


/**
 * gtranslator_po_get_prev_untrans:
 * @po: a #GtranslatorPo
 *
 * Return value: a pointer to the previously untranslated message
 * or NULL if there are not previously untranslated message.
 **/
GList *
gtranslator_po_get_prev_untrans(GtranslatorPo *po)
{
	GList *msg;

	msg = po->priv->current;
	while (msg = g_list_previous(msg))
	{
		if(!gtranslator_msg_is_translated(msg->data))
			return msg;
	}

	return NULL;
}

GtranslatorHeader *
gtranslator_po_get_header(GtranslatorPo *po)
{
	return po->priv->header;
}

gint
gtranslator_po_get_translated_count(GtranslatorPo *po)
{
	return po->priv->translated;
}

void
gtranslator_po_increase_decrease_translated(GtranslatorPo *po,
					    gboolean increase)
{
	g_return_if_fail(GTR_IS_PO(po));
	
	if(increase)
		po->priv->translated++;
	else po->priv->translated--;
}

gint
gtranslator_po_get_fuzzy_count(GtranslatorPo *po)
{
	return po->priv->fuzzy;
}

void
gtranslator_po_increase_decrease_fuzzy(GtranslatorPo *po,
				       gboolean increase)
{
	g_return_if_fail(GTR_IS_PO(po));
	
	if(increase)
		po->priv->fuzzy++;
	else po->priv->fuzzy--;
}

gint
gtranslator_po_get_untranslated_count(GtranslatorPo *po)
{
	return (g_list_length(po->priv->messages) - po->priv->translated - po->priv->fuzzy);
}

gint
gtranslator_po_get_messages_count(GtranslatorPo *po)
{
	return g_list_length(po->priv->messages);
}

gint
gtranslator_po_get_message_position(GtranslatorPo *po)
{
	return gtranslator_msg_get_po_position(GTR_MSG(po->priv->current->data));
}

/**
 * gtranslator_po_check_po_file:
 * @po: a #GtranslatorPo
 *
 * Test whether an entire file PO file is valid, like msgfmt does it.
 * Return value: If it is invalid, returns the error.
 **/
const gchar *
gtranslator_po_check_po_file(GtranslatorPo *po)
{
	struct po_xerror_handler handler;
	
	g_return_val_if_fail(po != NULL, NULL);

	handler.xerror = &on_gettext_po_xerror;
	handler.xerror2 = &on_gettext_po_xerror2;
	
	if(message_error != NULL)
	{
		g_free(message_error);
		message_error = NULL;
	}
	
	po_file_check_all(po->priv->gettext_po_file, &handler);

	return message_error;
}
