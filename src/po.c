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
#include "gtranslator-enum-types.h"

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
	
	GtranslatorPoState state;
};

enum
{
	PROP_0,
	PROP_STATE
};

static gchar *message_error = NULL;

static void
gtranslator_po_get_property (GObject    *object,
			     guint       prop_id,
			     GValue     *value,
			     GParamSpec *pspec)
{
	GtranslatorPo *po = GTR_PO (object);

	switch (prop_id)
	{
		case PROP_STATE:
			g_value_set_enum (value,
					  gtranslator_po_get_state (po));
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
	{
		g_list_foreach (po->priv->domains, (GFunc)g_free, NULL);
		g_list_free (po->priv->domains);
	}

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
	object_class->get_property = gtranslator_po_get_property;	
	
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
			    GTR_PO_ERROR_RECOVERY,
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
		      *charset, *encoding, *plural_forms;

		gchar *space1, *space2, *space3;

		comment = g_strdup(po_message_comments(message));
		
		prj_id_version = po_header_field(msgstr, "Project-Id-Version");
		rmbt = po_header_field(msgstr, "Report-Msgid-Bugs-To");
		pot_date = po_header_field(msgstr, "POT-Creation-Date");
		po_date = po_header_field(msgstr, "PO-Revision-Date");

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

		mime_version = po_header_field(msgstr, "MIME-Version");

		gchar *charset_temp = po_header_field(msgstr, "Content-Type");
		space3 = g_strrstr(charset_temp, "=");

		if (!space3)
		{
			charset = g_strdup("");
		}else {	
			charset = g_strdup(space3 +1);
		}
		
		encoding = po_header_field(msgstr, "Content-Transfer-Encoding");
		
		/*
		 * Plural forms:
		 */
		plural_forms = po_header_field (msgstr, "Plural-Forms");
	
		gtranslator_header_set_comment(priv->header, comment);
		gtranslator_header_set_prj_id_version(priv->header, prj_id_version);
		gtranslator_header_set_rmbt(priv->header, rmbt);
		gtranslator_header_set_pot_date(priv->header, pot_date);
		gtranslator_header_set_po_date(priv->header, po_date);
		gtranslator_header_set_translator(priv->header, translator);
		gtranslator_header_set_tr_email(priv->header, tr_email);
		gtranslator_header_set_prev_translator(priv->header, translator_temp); 
		gtranslator_header_set_language(priv->header, language);
		gtranslator_header_set_lg_email(priv->header, lg_email);
		gtranslator_header_set_mime_version(priv->header, mime_version);
		gtranslator_header_set_charset(priv->header, charset);
		gtranslator_header_set_encoding(priv->header, encoding);
		gtranslator_header_set_plural_forms (priv->header, 
						     plural_forms);
		
		g_free (translator_temp);
		g_free (language_temp);
		g_free (charset_temp);
		g_free (comment);
		g_free (prj_id_version);
		g_free (rmbt);
		g_free (pot_date);
		g_free (po_date);
		g_free (translator);
		g_free (tr_email);
		g_free (language);
		g_free (lg_email);
		g_free (mime_version);
		g_free (charset);
		g_free (encoding);
		g_free (plural_forms);
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
	
	/* Initialize Tab state*/
	po->priv->state = GTR_PO_STATE_SAVED;
}

/**
 * gtranslator_po_save_file:
 * @po: a #GtranslatorPo
 * @error: a GError to manage the exceptions
 * 
 * It saves the po file and if there are any problem it stores the error
 * in @error.
 **/
void
gtranslator_po_save_file(GtranslatorPo *po,
			 GError **error)
{
	struct po_xerror_handler handler;
	po_message_iterator_t iter;
	po_message_t message;
	
	GtranslatorHeader *header;	

	const char *msgstr,
		   *header_comment;
	const char *prev_translator;

	time_t now;
	struct tm *now_here;
	char t[22];
	gchar *year=g_malloc(5);
	gchar *aux;
	
	/*
	 * Initialice the handler error.
	 */
	handler.xerror = &on_gettext_po_xerror;
	handler.xerror2 = &on_gettext_po_xerror2;
	
	if (g_str_has_suffix(po->priv->filename, ".pot"))
	{
		g_set_error (error,
			     GTR_PO_ERROR,
			     GTR_PO_ERROR_FILENAME,
			     _("You are saving a file with a .pot extension.\n"
			     "Pot files are generated by the compilation process.\n"
			     "Your file should likely be named '%s.po'."), 
			     po->priv->filename);
		g_free (year);
		return;
	}
	
	/*
	 * Get header's fields
	 */
	header = gtranslator_po_get_header(po);
	/*
         * Save the previous translator to update the header's comment
         */
	prev_translator = gtranslator_header_get_prev_translator(header);

        /*
         * Determinate the message with the header
         */
	iter = po_message_iterator(gtranslator_po_get_po_file(po), NULL);
	message = po_next_message(iter);
	/*
	 * FIXME: We have to use our msg class to manage this kind of things,
	 * and we have to encapsulate the funcs like po_header_set_field in
	 * our header class.
	 */
	msgstr = po_message_msgstr(message);

	/*
	 * Update the po date 
	 */
	now = time(NULL);
	now_here = localtime(&now);
	strftime(t, 22, "%Y-%m-%d %H:%M%z", now_here);
	strftime(year, 5, "%Y", now_here);
	
	aux = g_strdup (t);
	gtranslator_header_set_po_date (header, aux);
	g_free (aux);
	
	/*
         * Update the header's comment
         */
        aux = g_strconcat(gtranslator_header_get_translator(header), " ", "<", 
			  gtranslator_header_get_tr_email(header), ">", NULL);
	if (strcmp(prev_translator, aux))
	{
		gchar *aux2;
		
		header_comment = po_message_comments (message);
		aux2 = g_strconcat(header_comment, gtranslator_header_get_translator(header), " ", "<",
				   gtranslator_header_get_tr_email(header), ">", ",", " ", year, NULL);
		
		po_message_set_comments (message, aux2);
		g_free (aux2);
	}
	g_free (aux);

	/*
         * Write the header's fields
         */
	msgstr = po_header_set_field (msgstr, "Project-Id-Version",
				      gtranslator_header_get_prj_id_version(header));
	msgstr = po_header_set_field (msgstr, "PO-Revision-Date",
				      gtranslator_header_get_po_date(header));
	
	aux = g_strconcat (gtranslator_header_get_translator(header), " ", "<",
			   gtranslator_header_get_tr_email(header), ">", NULL);
	msgstr = po_header_set_field (msgstr, "Last-Translator", aux);
	g_free (aux);
	
	aux = g_strconcat (gtranslator_header_get_language(header), " ", "<", 
			   gtranslator_header_get_lg_email(header), ">", NULL);
	msgstr = po_header_set_field (msgstr, "Language-Team", aux);
	g_free (aux);
	
	aux = g_strconcat(" text/plain;", " charset=",
			  gtranslator_header_get_charset(header),NULL);
	msgstr = po_header_set_field (msgstr, "Content-Type", aux);
	g_free (aux);
	
	msgstr = po_header_set_field (msgstr, "Content-Transfer-Encoding",
				      gtranslator_header_get_encoding(header));
	po_message_set_msgstr (message, msgstr);
	
	if (!po_file_write (gtranslator_po_get_po_file (po),
			    po->priv->filename, &handler))
	{
		g_set_error (error,
			     GTR_PO_ERROR,
			     GTR_PO_ERROR_FILENAME,
			     _("There was an error writing the PO file: %s"),
			     message_error);
		return;
	}
	
	/*
	 * If the warn if fuzzy option is enabled we have to show an error
	 */
	/*if (gtranslator_prefs_manager_get_warn_if_fuzzy () && po->priv->fuzzy)
	{
		g_set_error (error,
			     GTR_PO_ERROR,
			     GTR_PO_ERROR_OTHER,
			     ngettext ("File %s\ncontains %d fuzzy message", 
				       "File %s\ncontains %d fuzzy messages",
				       po->priv->fuzzy), 
				       po->priv->fuzzy);
	}*/
}

/**
 * gtranslator_po_get_filename:
 * @po: a #GtranslatorPo
 * 
 * Return value: the file name string
 **/
const gchar *
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

/**
 * gtranslator_po_get_state:
 * @po: a #GtranslatorPo
 *
 * Return value: the #GtranslatorPoState value of the @po.
 */
GtranslatorPoState
gtranslator_po_get_state(GtranslatorPo *po)
{
	return po->priv->state;
}

/**
 * gtranslator_po_set_state:
 * @po: a #GtranslatorPo
 * @state: a #GtranslatorPoState
 *
 * Sets the state for a #GtranslatorPo
 */
void
gtranslator_po_set_state(GtranslatorPo *po,
			 GtranslatorPoState state)
{
	po->priv->state = state;
	
	g_object_notify (G_OBJECT (po), "state");
}

/*
 * FIXME: We are not using this func.
 */
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
gtranslator_po_get_prev_untrans (GtranslatorPo *po)
{
	GList *msg;

	msg = po->priv->current;
	while (msg = g_list_previous (msg))
	{
		if(!gtranslator_msg_is_translated (msg->data))
			return msg;
	}

	return NULL;
}

/**
 * gtranslator_po_get_header:
 * @po: a #GtranslatorPo
 *
 * Return value: The #GtranslatorHeader of the @po.
 **/
GtranslatorHeader *
gtranslator_po_get_header(GtranslatorPo *po)
{
	g_return_val_if_fail (GTR_IS_PO (po), NULL);
	
	return po->priv->header;
}

/**
 * gtranslator_po_get_translated_count:
 * @po: a #GtranslatorPo
 * 
 * Return value: the count of the translated messages.
 **/
gint
gtranslator_po_get_translated_count (GtranslatorPo *po)
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
_gtranslator_po_increase_decrease_translated (GtranslatorPo *po,
					      gboolean increase)
{
	g_return_if_fail (GTR_IS_PO (po));
	
	if (increase)
		po->priv->translated++;
	else po->priv->translated--;
}

/**
 * gtranslator_po_get_fuzzy_count:
 * @po: a #GtranslatorPo
 * 
 * Return value: the count of the fuzzy messages.
 **/
gint
gtranslator_po_get_fuzzy_count (GtranslatorPo *po)
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
_gtranslator_po_increase_decrease_fuzzy (GtranslatorPo *po,
					 gboolean increase)
{
	g_return_if_fail (GTR_IS_PO (po));
	
	if (increase)
		po->priv->fuzzy++;
	else po->priv->fuzzy--;
}

/**
 * gtranslator_po_get_untranslated_count:
 * @po: a #GtranslatorPo
 * 
 * Return value: the count of the untranslated messages.
 **/
gint
gtranslator_po_get_untranslated_count (GtranslatorPo *po)
{
	g_return_val_if_fail (GTR_IS_PO (po), -1);
	
	return (g_list_length (po->priv->messages) - po->priv->translated - po->priv->fuzzy);
}

/**
 * gtranslator_po_get_messages_count:
 * @po: a #GtranslatorPo
 * 
 * Return value: the number of messages messages.
 **/
gint
gtranslator_po_get_messages_count (GtranslatorPo *po)
{
	g_return_val_if_fail (GTR_IS_PO (po), -1);
	
	return g_list_length (po->priv->messages);
}

/**
 * gtranslator_po_get_message_position:
 * @po: a #GtranslatorPo
 * 
 * Return value: the number of the current message.
 **/
gint
gtranslator_po_get_message_position (GtranslatorPo *po)
{
	g_return_val_if_fail (GTR_IS_PO (po), -1);
	
	return gtranslator_msg_get_po_position (GTR_MSG (po->priv->current->data));
}

/**
 * gtranslator_po_check_po_file:
 * @po: a #GtranslatorPo
 *
 * Test whether an entire PO file is valid, like msgfmt does it.
 * Return value: If it is invalid, returns the error.
 **/
const gchar *
gtranslator_po_check_po_file (GtranslatorPo *po)
{
	struct po_xerror_handler handler;
	
	g_return_val_if_fail (po != NULL, NULL);

	handler.xerror = &on_gettext_po_xerror;
	handler.xerror2 = &on_gettext_po_xerror2;
	
	if (message_error != NULL)
	{
		g_free (message_error);
		message_error = NULL;
	}
	
	po_file_check_all (po->priv->gettext_po_file, &handler);

	return message_error;
}
