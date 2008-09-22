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

#include "application.h"
#include "debug.h"
#include "file-dialogs.h"
#include "po.h"
#include "msg.h"
#include "gtranslator-enum-types.h"
#include "prefs-manager.h"
#include "profile.h"
#include "utils.h"

#include <string.h>
#include <errno.h>

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gettext-po.h>
#include <gio/gio.h>

#define GTR_PO_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_PO,     \
					 GtranslatorPoPrivate))


G_DEFINE_TYPE(GtranslatorPo, gtranslator_po, G_TYPE_OBJECT)

struct _GtranslatorPoPrivate
{
	GFile *location;
	
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
	
	po->priv->location = NULL;
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

	if (po->priv->location)
		g_object_unref (po->priv->location);
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

static gboolean
po_file_is_empty (po_file_t file)
{
	const gchar * const * domains = po_file_domains (file);
	
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

/***************************** Public funcs ***********************************/

/**
 * gtranslator_po_new:
 *
 * Creates a new #GtranslatorPo.
 *
 * Returns: a new #GtranslatorPo object
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
 * @location: the file to open
 * @error: a variable to store the errors
 *
 * Parses all things related to the #GtranslatorPo and initilizes all neccessary
 * variables.
 **/
void
gtranslator_po_parse (GtranslatorPo *po,
		      GFile *location,
		      GError **error)
{
	GtranslatorPoPrivate *priv = po->priv;
	gchar *filename;
	GtranslatorMsg *msg;
	struct po_xerror_handler handler;
	po_message_t message;
	po_message_iterator_t iter;
	const gchar *msgstr;
	const gchar * const *domains;
	gchar *base;
	gint i = 0;
	gint pos = 1;
	
	g_return_if_fail (GTR_IS_PO (po));
	g_return_if_fail (location != NULL);
	
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
	 * Get filename path.
	 */
	po->priv->location = g_file_dup (location);
	filename = g_file_get_path (location);
	
	priv->gettext_po_file = po_file_read (filename,
					      &handler);
	if (priv->gettext_po_file == NULL)
	{
		g_set_error (error,
			     GTR_PO_ERROR,
			     GTR_PO_ERROR_FILENAME,
			     _("Failed opening file '%s': %s"),
			     filename, g_strerror (errno));
		g_object_unref(po);
		g_free (filename);
		return;
	}
	g_free (filename);
	
	/*
	 * No need to return; this can be corrected by the user
	 */
	if(message_error != NULL) {
		g_set_error(error,
			    GTR_PO_ERROR,
			    GTR_PO_ERROR_RECOVERY,
			    message_error);
	}
	
	if (po_file_is_empty (priv->gettext_po_file))
	{
		g_set_error (error,
			     GTR_PO_ERROR,
			     GTR_PO_ERROR_FILE_EMPTY,
			     _("The file is empty"));
		g_object_unref(po);
		return;
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
			msg = gtranslator_msg_new (iter, message);

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
 * gtranslator_po_save_header_in_msg:
 * @po: a #GtranslatorPo
 * 
 * It saves the header's values into the msgstr
 **/
void
gtranslator_po_save_header_in_msg (GtranslatorPo *po)
{
	po_message_iterator_t iter;
	po_message_t message;
	
	GtranslatorHeader *header;	

	const char *msgstr,
		   *header_comment;
	const char *prev_translator;

	gchar *current_date;
	gchar *current_time;
	gchar *year;
	gchar *new_date;
	gchar *aux;
	gchar *aux2;
	const gchar *comments;
	gchar **comments_lines;
	gchar **comments_translator_values;

	gboolean take_my_options;

	gint i = 0;
	gint j;
	gint k = 0;
	gint l;

	gchar *new_comments = "";
	gchar *old_line;
	gchar *line = "";
	gchar *comp_year;
	gchar *line_without_dot;

	take_my_options = gtranslator_prefs_manager_get_take_my_options ();

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
	 * If button take_my_options is pulsed, then header's values
	 * should be taking from default profile
	 */

	if (take_my_options) {

	  GtranslatorProfile *active_profile;

	  active_profile = gtranslator_application_get_active_profile (GTR_APP);
   
	  gtranslator_header_set_translator (header, gtranslator_profile_get_author_name (active_profile));
	  gtranslator_header_set_tr_email (header, gtranslator_profile_get_author_email (active_profile));
	  gtranslator_header_set_language (header, gtranslator_profile_get_language_name (active_profile));
	  gtranslator_header_set_charset (header, gtranslator_profile_get_charset (active_profile));
	  gtranslator_header_set_encoding (header, gtranslator_profile_get_encoding (active_profile));
	  gtranslator_header_set_lg_email (header, gtranslator_profile_get_group_email (active_profile));
	  gtranslator_header_set_plural_forms (header, gtranslator_profile_get_plurals (active_profile));
	}

	/*
	 * Update the po date 
	 */
	current_date = gtranslator_utils_get_current_date ();
	current_time = gtranslator_utils_get_current_time ();
	year = gtranslator_utils_get_current_year ();

	new_date = g_strconcat (current_date, " ", current_time, NULL);

	gtranslator_header_set_po_date (header, new_date);

	g_free (new_date);

	/*
         * Update the header's comment
         */
	comments = po_message_comments (message);
	comments_lines = g_strsplit (comments, "\n", -1);
	
	/*
	 * Searching if the current translator is in comments.
	 */
	while (comments_lines[i] != NULL) {
	  if (g_str_has_prefix (comments_lines[i], prev_translator)) {
	    comments_translator_values = g_strsplit (comments_lines[i], ",", -1);
	    j = i;
	  }
	  i++;
	}
	aux = g_strconcat(gtranslator_header_get_translator(header), " ", "<",
			  gtranslator_header_get_tr_email(header), ">", NULL);
	
	comp_year = g_strconcat (" ", year, ".", NULL);

	/*
	 * Current translator is already in comments but its last year < current year.
	 */
	if ((g_utf8_collate (prev_translator, aux) == 0) && 
	    (g_utf8_collate (comments_translator_values[g_strv_length (comments_translator_values)-1], comp_year) != 0)) {
	  
	  if (g_str_has_suffix (comments_lines[j], ".")) {
	    line_without_dot = g_strndup (comments_lines[j], g_utf8_strlen(comments_lines[j], -1) -1);  
	    line = g_strconcat (line_without_dot, ", ", year, ".", NULL);
	    g_free (line_without_dot);
	  }else {
	    line = g_strconcat (comments_lines[j], ", ", year, ".", NULL);
	  }
	  
	  for (l=j; l<(g_strv_length (comments_lines)); l++) {
	    comments_lines[l] = comments_lines[l+1];
	  }
	  
	  comments_lines[g_strv_length (comments_lines)-1] = line;
	  while (comments_lines[k] != NULL) {
	    new_comments = g_strconcat (new_comments, comments_lines[k], "\n", NULL);
	    k++;
	    }
	  po_message_set_comments (message, new_comments);
	  
	  g_free (line);
	  g_free (new_comments);
	}    

	/*
	 * Current translator is not in the comments.
	 */
	if (g_utf8_collate (prev_translator, aux) != 0) {
	  
	  header_comment = po_message_comments (message);
	  aux2 = g_strconcat(header_comment, gtranslator_header_get_translator(header), " ", "<",
			     gtranslator_header_get_tr_email(header), ">", ",", " ", year, ".", NULL);
	  
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
	gchar *msg_error;
	gchar *filename;
	
	/*
	 * Initialice the handler error.
	 */
	handler.xerror = &on_gettext_po_xerror;
	handler.xerror2 = &on_gettext_po_xerror2;
	
	filename = g_file_get_path (po->priv->location);
	
	if (g_str_has_suffix (filename, ".pot"))
	{
		g_set_error (error,
			     GTR_PO_ERROR,
			     GTR_PO_ERROR_FILENAME,
			     _("You are saving a file with a .pot extension.\n"
			     "Pot files are generated by the compilation process.\n"
			     "Your file should likely be named '%s.po'."), 
			     filename);
		g_free (filename);
		return;
	}
	
	/*
	 * Check if the file is right
	 */
	/*msg_error = gtranslator_po_check_po_file (po);
	if (msg_error != NULL)
	{
		g_set_error (error,
			     GTR_PO_ERROR,
			     GTR_PO_ERROR_GETTEXT,
			     _("There is an error in the PO file: %s"),
			     msg_error);
		g_free (msg_error);
	}*/
	
	
	/*
	 * Save header fields into msg
	 */
	gtranslator_po_save_header_in_msg (po);	
	
	if (!po_file_write (gtranslator_po_get_po_file (po),
			    filename, &handler))
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
	gtranslator_po_set_state (po, GTR_PO_STATE_SAVED);
	
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
 * Gets the GFile of the po file.
 * 
 * Returns: the GFile associated with the @po. The returned location must be freed
 * with g_object_unref.
 **/
GFile *
gtranslator_po_get_location (GtranslatorPo *po)
{
	g_return_val_if_fail (GTR_IS_PO (po), NULL);
	
	return g_file_dup (po->priv->location);
}

/**
 * gtranslator_po_set_location:
 * @po: a #GtranslatorPo
 * @location: The GFile to set to the #GtranslatorPo
 *
 * Sets the GFile location within the #GtranslatorPo object.
 **/
void
gtranslator_po_set_location (GtranslatorPo *po,
			     GFile *location)
{
	g_return_if_fail(GTR_IS_PO(po));
	
	if (po->priv->location)
		g_object_unref (po->priv->location);
	
	po->priv->location = g_file_dup (location);
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
 * gtranslator_po_get_next_fuzzy_or_untrans:
 * @po: a #GtranslatorPo
 *
 * Return value: a pointer to the next fuzzy or untranslated message
 * or NULL if there is not next fuzzy or untranslated message.
 **/
GList *
gtranslator_po_get_next_fuzzy_or_untrans (GtranslatorPo *po)
{
	GList *msg;
	
	msg = po->priv->current;
	while (msg = g_list_next (msg))
	{
		if (gtranslator_msg_is_fuzzy (msg->data) ||
		    !gtranslator_msg_is_translated (msg->data))
			return msg;
	}
	
	return NULL;
}

/**
 * gtranslator_po_get_prev_fuzzy_or_untrans:
 * @po: a #GtranslatorPo
 *
 * Return value: a pointer to the previously fuzzy or untranslated message
 * or NULL if there is not previously fuzzy or untranslated message.
 **/
GList *
gtranslator_po_get_prev_fuzzy_or_untrans (GtranslatorPo *po)
{
	GList *msg;
	
	msg = po->priv->current;
	while (msg = g_list_previous (msg))
	{
		if (gtranslator_msg_is_fuzzy (msg->data) ||
		    !gtranslator_msg_is_translated (msg->data))
			return msg;
	}
	
	return NULL;
}

/**
 * gtranslator_po_get_msg_from_number:
 * @po: a #GtranslatorPo
 * @number: the message to jump
 *
 * Gets the message at the given position.
 */
GList *
gtranslator_po_get_msg_from_number (GtranslatorPo *po,
				    gint number)
{
	g_return_val_if_fail (GTR_IS_PO (po), NULL);
	
	return g_list_nth (po->priv->messages, number);
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
	
	/*DEBUG_PRINT ("length: %d | translated: %d | fuzzy: %d | untranslated: %d", g_list_length (po->priv->messages),
		     po->priv->translated, po->priv->fuzzy, (g_list_length (po->priv->messages) - po->priv->translated - po->priv->fuzzy));*/
	
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
 * Returns: If it is invalid, returns the error. The return value must be freed
 * with g_free.
 **/
gchar *
gtranslator_po_check_po_file (GtranslatorPo *po)
{
	struct po_xerror_handler handler;
	
	g_return_val_if_fail (po != NULL, NULL);

	handler.xerror = &on_gettext_po_xerror;
	handler.xerror2 = &on_gettext_po_xerror2;
	message_error = NULL;
	
	po_file_check_all (po->priv->gettext_po_file, &handler);

	return message_error;
}
