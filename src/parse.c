/*
 * (C) 2000-2003 	Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
 *			Ross Golder <ross@gtranslator.org>
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

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/param.h>

#include "actions.h"
#include "comment.h"
#include "dialogs.h"
#ifdef NOT_PORTED
# include "gtkspell.h"
#endif
#include "gui.h"
#include "history.h"
#include "message.h"
#include "messages-table.h"
#include "nautilus-string.h"
#include "open.h"
#include "parse.h"
#include "prefs.h"
#include "runtime-config.h"
#include "save.h"
#include "translator.h"
#include "undo.h"
#ifdef UTF8_CODE
# include "utf8.h"
#endif
#include "utils.h"
#include "utils_gui.h"

#include <gtk/gtkfilesel.h>
#include <gtk/gtkmain.h>
#include <gtk/gtklabel.h>

#include <libgnomeui/gnome-appbar.h>
#include <libgnomeui/gnome-app-util.h>
#include <libgnomeui/gnome-dialog-util.h>
#include <libgnomeui/gnome-messagebox.h>
#include <libgnomeui/gnome-uidefs.h>

/* Global variables */
GtrPo *po;
gboolean file_opened;
gboolean message_changed;
guint autosave_source_tag=1;
gboolean open_anyway=FALSE;

/*
 * These are to be used only inside this file
 */
static void append_line(gchar ** old, const gchar * tail, gboolean continuation);
static gboolean add_to_obsolete(GtrPo *po, gchar *comment);
static void write_the_message(gpointer data, gpointer fs);
static gchar *restore_msg(const gchar * given);
static void determine_translation_status(gpointer data, gpointer useless_stuff);

/*
 * Remove the translation and update the message data.
 */
static void remove_translation(gpointer message, gpointer useless);

static void check_msg_status(GtrMsg * msg)
{
	if (msg->msgstr)
		msg->status = GTR_MSG_STATUS_TRANSLATED;
	if ((msg->comment) && (GTR_COMMENT(msg->comment)->type & FUZZY_COMMENT))
		msg->status |= GTR_MSG_STATUS_FUZZY;
}

/*
 * Formats tail to look good when displayed and easier to maintain. Removes/
 * unneeded \'s and "'s and replaces \\n with real newline.
 * Then appends this to *old and updates the pointer.
 */
static void append_line(gchar ** old, const gchar * tail, gboolean continuation)
{
	gchar *to_add = g_new(gchar, strlen(tail) * 2);
	gchar *result;
	gint s, d = 0;

	if (continuation)
		to_add[d++] = '\n';

	for (s = 1; s < strlen(tail) - 1; s++) {
		if(tail[s] == '\\') {
			switch(tail[s + 1]) {
			case '\"':
				s++;
				break;
			}
		}
		to_add[d++] = tail[s];
	}
	to_add[d] = 0;
	if (*old == NULL)
		result = to_add;
	else
	{
		result = g_strconcat(*old, to_add, NULL);
		GTR_FREE(*old);
		GTR_FREE(to_add);
	}
	*old = result;
}

gboolean add_to_obsolete(GtrPo *po, gchar *comment)
{
	if(comment && strstr(comment, "#~ msgid"))
	{
		if(GtrPreferences.keep_obsolete) {
			if(po->obsolete == NULL)
			{
				po->obsolete=g_strdup(comment);
			}
			else
			{
				gchar *tmp;
				
				tmp=g_strconcat(po->obsolete, "\n", comment, NULL);
				
				GTR_FREE(po->obsolete);
				po->obsolete=tmp;
			}
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*
 * The core parsing function for the given po file.
 */ 
GtrPo *gtranslator_parse(const gchar *filename)
{
	GtrPo *po;
	gboolean first_is_fuzzy;
	gchar *base;

	g_return_val_if_fail(filename!=NULL, NULL);

	base=g_path_get_basename(filename);
	g_return_val_if_fail(base[0]!='\0', NULL);
	g_free(base);

	po = g_new0(GtrPo, 1);

	/*
	 * Get absolute filename.
	 */
	if (!g_path_is_absolute(filename)) 
	{
		char absol[MAXPATHLEN + 1];
		realpath(filename, absol);
		po->filename = g_strdup(absol);
	}
	else
	{
		po->filename = g_strdup(filename);
	}
	
	/*
	 * Check the right file access permissions.
	 */
	if(gtranslator_utils_check_file_permissions(po)==FALSE)
	{
		return NULL;
	}
	
	if (!gtranslator_parse_core(po))
	{
		gtranslator_po_free(po);
		return NULL;
	}

#define FIRST_MSG GTR_MSG(po->messages->data)
	first_is_fuzzy=(FIRST_MSG->status & GTR_MSG_STATUS_FUZZY) != 0;
	gtranslator_message_status_set_fuzzy(FIRST_MSG, FALSE);
	po->fuzzy--;
	
	if (po->header)
	{
		GList *header_li;
		
		/*
		 * Unlink it from messages list
		 */
		header_li = po->messages;
		po->messages = g_list_remove_link(po->messages, header_li);
		gtranslator_message_free(header_li->data, NULL);
		g_list_free_1(header_li);
	}
	else
	{
		gtranslator_message_status_set_fuzzy(FIRST_MSG, first_is_fuzzy);
		if(first_is_fuzzy)
			po->fuzzy++;
		else
			po->fuzzy--;
	}
	
	file_opened = TRUE;
	po->file_changed = FALSE;
	po->length = g_list_length(po->messages);

#ifdef UTF8_CODE
	/*
	 * Set the utf8 field of the GtrPo to TRUE if we are editing an UTF-8 
	 *  encoded file.
	 */
	if(gtranslator_utf8_po_file_is_utf8(po))
	{
		po->utf8=TRUE;
	}
	else
	{
		po->utf8=FALSE;
	}

	po->locale_charset=gtranslator_utils_get_locale_charset(po);
	
	/* FIXME: converting the messages to UTF8 there since I don't
	   know the parsing code, and I don't feel like figuring it out
	   so I temporarily put this here to ensure all the strings are 
	   properly encoded. Please fix it...
	*/
	if (po->utf8 == FALSE) {
		gtranslator_utf8_convert_po_to_utf8(po);
	}
#endif

	/*
	 * Set the current message to the first message.
	 */
	po->current = g_list_first(po->messages);

	return po;
}

gboolean gtranslator_parse_core(GtrPo *po)
{
	GtrMsg 	*msg;
	
	FILE 	*fs;
	char 	*rawline;
	char 	*line;

	guint 	 lines=0;
	guint	 position=-1;

	GError  *errv;

	/*
	 * If TRUE, means that a corresponding part is read
	 */
	gboolean msgid_ok = FALSE, msgstr_ok = FALSE, comment_ok = FALSE;

	fs = fopen(po->filename, "r");
	
	/*
	 * As the po-file seems to exist, set the "count parameters" to 0.
	 */
	po->translated=0;
	po->fuzzy=0;
	
	msg = g_new0(GtrMsg, 1);
	
	/*
	 * Parse the file line by line...
	 */
	while ((rawline = gtranslator_utils_getline (fs)) != NULL) {
		
		lines++;

		/*
		 * Convert lines from charset specified in header
		 */
		if (po->header && po->header->charset) {
			/*
			 * Check if the header charset line is in a sane state.
			 */
			if(!nautilus_strcmp(po->header->charset, "CHARSET"))
			{
				/*
				 * If the default CHARSET definition is present
				 *  in the po file header, then we do now simply
				 *   assume the file to be UTF-8 as it's the
				 *    default charset for GNOME.
				 */
				GTR_FREE(po->header->charset);
				po->header->charset=g_strdup("UTF-8");
			}

			/*
			 * Convert from header charset to UTF-8
			 */
			line = g_convert(rawline, strlen(rawline), "utf-8", po->header->charset, NULL, NULL, &errv);
			if(!line) {
				g_warning("%s:%d: could not convert line from '%s' to UTF-8: %s", po->filename, lines, po->header->charset, errv->message);
				continue;
			}
		}
		else {
			line = rawline;
		}

       		/*
		 * Warn if result still not valid UTF-8
		 */
		if(!g_utf8_validate(line, strlen(line), NULL))
			g_warning("%s:%d: line contains invalid UTF-8", po->filename, lines);
			
		/*
		 * If it's a comment, no matter of what kind. It belongs to
		 * the message pair below
		 */
		if (line[0] == '#') {
			/*
			 * Set the comment & position.
			 */
			if (msg->comment == NULL) {
				msg->pos = lines;
				msg->comment = gtranslator_comment_new(line);
			} else {
				gchar *tmp;
				tmp = g_strconcat(GTR_COMMENT(msg->comment)->comment, line, NULL);
				gtranslator_comment_update(&msg->comment, tmp);
			}
		}
		else {
			g_strchomp(line);

			if (line[0] == '\0') {
				if(msgid_ok==TRUE)
				{
					msgstr_ok = TRUE;
				}
				else if(msg->comment)
				{
					if(add_to_obsolete(po, GTR_COMMENT(msg->comment)->comment))
					{
						gtranslator_comment_free(&msg->comment);
						msg->comment=NULL;
					}
				}
			}
			else if (nautilus_str_has_prefix(line, "msgid \"")) {
				/*
				 * This means the comment is completed
				 */
				comment_ok = TRUE;
				if (line[8] != '\0')
					append_line(&msg->msgid, &line[6], FALSE);
			} 
			else if (nautilus_str_has_prefix(line, "msgid_plural \""))
			{
				/*
				 * Now we've got a new msgid_plural tag.
				 */
				if(line[15]!='\0')
				{
					append_line(&msg->msgid_plural, &line[13], FALSE);
				}
			}
			/*
			 * If it's a msgstr. 
			 */
			else if (nautilus_str_has_prefix(line, "msgstr \""))
			{
				/*
				 * This means the msgid is completed
				 */
				msgid_ok = TRUE;
				if (line[9] != '\0')
					append_line(&msg->msgstr, &line[7], FALSE);
			}
			else if(nautilus_str_has_prefix(line, "msgstr[0] \""))
			{
				/*
				 * Now we've got a msgstr item in here.
				 */
				msgid_ok = TRUE;
				if(line[12]!='\0')
				{
					append_line(&msg->msgstr, &line[10], FALSE);
				}
			}
			else if(nautilus_str_has_prefix(line, "msgstr[1] \""))
			{
				if(line[12]!='\0')
				{
					append_line(&msg->msgstr_1, &line[10], FALSE);
				}
			}
			else if(nautilus_str_has_prefix(line, "msgstr[2] \""))
			{
				if(line[12]!='\0')
				{
					append_line(&msg->msgstr_2, &line[10], FALSE);
				}
			}
			else
			/*
			 * A continuing msgid or msgstr
			 */
			if (line[0] == '"') {
				if((comment_ok == TRUE) &&
				   (msgid_ok == FALSE))
				{
					append_line(&msg->msgid, line, TRUE);
				}
				else if((comment_ok == TRUE) && msgid_ok == FALSE &&
				    msg->msgid_plural)
				{
					append_line(&msg->msgid_plural, line, TRUE);
				}
				else if((msgid_ok == TRUE) &&
					(msgstr_ok == FALSE))
				{
					append_line(&msg->msgstr, line, TRUE);
				}
				else if((msgid_ok == TRUE) && (msgstr_ok == FALSE))
				{
					if(msg->msgstr && !msg->msgstr_1 && !msg->msgstr_2)
					{
						append_line(&msg->msgstr, line, TRUE);
					}
					else if(msg->msgstr && msg->msgstr_1 && !msg->msgstr_2)
					{
						append_line(&msg->msgstr_1, line, TRUE);
					}
					else
					{
						append_line(&msg->msgstr_2, line, TRUE);
					}
				}
				else if((comment_ok == FALSE) &&
					(msgid_ok == FALSE) &&
					(msgstr_ok == FALSE))
				{
					/*
					 * This might seem to be a nonsense
					 *  issue/check, but it occurs if a
					 *   multiple line message features an
					 *    empty line before the msgstr
					 *     content parts (b.g.o #68782).
					 */
					continue;
				}
				else
				{
					g_assert_not_reached();
				}
			} else {
				gtranslator_utils_error_dialog(_("Error in file \"%s\"\nat line %d.\nPlease check the file and try again."), po->filename, lines);
				GTR_FREE(msg);
				return FALSE;
			}
		}
		/*
		 * we've got both msgid + msgstr
		 */
		if ((msgid_ok == TRUE) && (msgstr_ok == TRUE)) {
			check_msg_status(msg);
			
			position++;
			msg->no=position;

			po->messages =
			    g_list_prepend(po->messages, (gpointer) msg);

			/*
			 * The first message should always be the header. Parse it
			 * do determine charset to read rest of file in with.
			 */
			if(position == 0) {
				po->header = GTR_HEADER(gtranslator_header_get(FIRST_MSG));
			}

			/*
			 * Reset the status of message
			 */
			msgid_ok = msgstr_ok = comment_ok = FALSE;
			msg = g_new0(GtrMsg, 1);
		}
	}
	
	/*
	 * If there was no empty line at end of file
	 */
	if ((msgid_ok == TRUE) && (msgstr_ok == FALSE))
	{
		check_msg_status(msg);
		
		position++;
		msg->no=position;
		
		po->messages = g_list_prepend(po->messages, (gpointer) msg);
	}
	else if(msg->comment && msg->comment->comment &&
		add_to_obsolete(po, GTR_COMMENT(msg->comment)->comment))
	{
		gtranslator_comment_free(&msg->comment);
		GTR_FREE(msg);
	}

	fclose(fs);

	if (po->messages == NULL) {
		gtranslator_utils_error_dialog(_("The file is empty:\n%s"), po->filename);
		return FALSE;
	}
	
	/*
	 * We've prepended every message, so let's reverse now
	 */
	po->messages = g_list_reverse(po->messages);
	return TRUE;
}

/*
 * The internally used parse-function
 */
void gtranslator_parse_main(const gchar *filename)
{
	/*
	 * Test if such a file does exist.
	 */
	if(!g_file_test(filename, G_FILE_TEST_EXISTS))
	{
		gtranslator_utils_error_dialog(
			_("The file `%s' doesn't exist at all!"),
				filename);
		return;
	}

	/*
	 * Check if the given is already open within another instance.
	 */
	if(gtranslator_utils_check_file_being_open(filename))
	{
		if(!open_anyway)
			return;
	}

	gtranslator_config_set_string("runtime/filename", 
				      (gchar *) filename);

	/*
	 * Use the new core function.
	 */
	po = gtranslator_parse(filename);

	/*
	 * Iterate to the main GUI thread -- well, no locks for the GUI should
	 *  be visible -- avoids "the clean my gtranslator window with dialog"
	 *   party game ,-)
	 */
	while(gtk_events_pending())
	{
		gtk_main_iteration();
	}

	if(po==NULL)
		return;

	gtranslator_actions_set_up_file_opened();

	gtranslator_get_translated_count();
	
	/*
	 * Is there any fuzzy message ?
	 */
	if(po->fuzzy>0)
	{
		/*
		 * Then enable the Fuzzy buttons/entries in the menus
		 */
		gtranslator_actions_enable(ACT_NEXT_FUZZY);

		/*
		 * If there is the corresponding pref and a fuzzy message, then
		 *  we'd enable the corresponding menu entries for "remove all
		 *   translations.
		 */
		if(GtrPreferences.rambo_function)
		{
			gtranslator_actions_enable(ACT_REMOVE_ALL_TRANSLATIONS);
		}
	}
	
	/*
	 * Is there any untranslated message ?
	 */
	if((po->length - po->translated) > 0)
	{
		/*
		 * Then enable the Untranslated buttons/entries in the menus
		 */
		gtranslator_actions_enable(ACT_NEXT_UNTRANSLATED);
	}

	/*
	 * If there are any translated messages, enable the "remove all
	 *  translations" functionality if needed.
	 */
	if((po->translated > 1) && GtrPreferences.rambo_function)
	{
		gtranslator_actions_enable(ACT_REMOVE_ALL_TRANSLATIONS);
	}

	if(po->header)
	{
		/*
		 * Fix b.g.o #120596 via checking for our table to be present.
		 */
		if(GtrPreferences.show_messages_table && !po->table_already_created)
		{
			gtranslator_messages_table_create();
		}

		/*
		 * If we've got still some default values,
		 * substitute with values from preferences,
		 */
		if(gtranslator_header_fill_up(po->header) ||
		   !strcmp(po->header->prj_name, "PACKAGE") ||
		   !strcmp(po->header->prj_version, "VERSION"))
		{
			gtranslator_translation_changed(NULL, NULL);

			/*
			 * Pop up the "Edit Header" so that user can verify
			 * automatically done changes and provide PACKAGE name.
			 */   
			gtranslator_header_edit_dialog(NULL, NULL);
		}
	}
	else
	{
		gtranslator_translation_changed(NULL, NULL);
		/* Create good header */
		po->header=gtranslator_header_create_from_prefs();
		/* But PACKAGE and VERSION should be entered by user */
		gtranslator_header_edit_dialog(NULL, NULL);
	}
}

/*
 * GUI-related stuff. Should only be called when the GUI needs updating. Could
 * probably also be moved to 'gui.c'.
 */
void gtranslator_parse_main_extra()
{	
	gchar         *base;
	gchar 	*title;
	
	g_return_if_fail(po!=NULL);

	if(!po->header) {
		/*
		 * Provide a default po->header to avoid segfaults (#62244)
		 */
		g_warning(_("Header record not found - defaulting from preferences"));
		po->header=gtranslator_header_create_from_prefs();
	}
  
	gtranslator_application_bar_update(0);

	/*
	 * Test if the filename is NOT equivalent to our temp files' names.
	 */
	base = g_path_get_basename(po->filename);
	if(nautilus_strcasecmp(base, gtranslator_runtime_config->temp_filename) || 
	   nautilus_strcasecmp(base, gtranslator_runtime_config->crash_filename))
	{
		gtranslator_history_add(po->filename,
			po->header->prj_name, po->header->prj_version);
	
		title=g_strdup_printf(_("gtranslator -- %s"), po->filename);
		gtk_window_set_title(GTK_WINDOW(gtranslator_application), title);
		GTR_FREE(title);
	}
	g_free(base);

	/*
	 * Update the recent files list.
	 */
	gtranslator_history_show();

	/*
	 * Create and set the messages table/tree up.
	 */
	if(GtrPreferences.show_messages_table)
	{
		gtranslator_messages_table_clear();
		gtranslator_messages_table_create();
	}

	/*
	 * Show the current message and avoid a crash with editing plurals
	 *  without having any active TreeModel present.
	 */
	gtranslator_message_show(po->current->data);

	/*
	 * Hook up the autosaving function if wished.
	 */
	if(GtrPreferences.autosave)
	{
		autosave_source_tag=g_timeout_add(
			(GtrPreferences.autosave_timeout * 60000),
			(GSourceFunc) gtranslator_utils_autosave, NULL);
	}
}

void gtranslator_parse_the_file_from_file_dialog(GtkWidget * widget, gpointer of_dlg)
{
	gchar *po_file;
	po_file = g_strdup(gtk_file_selection_get_filename(GTK_FILE_SELECTION(of_dlg)));

	gtranslator_file_dialogs_store_directory(po_file);

	if(file_opened)
		gtranslator_file_close(NULL, NULL);
	/*
	 * Open the file via our centralized opening function.
	 */
	gtranslator_open_file(po_file);

	/*
	 * Destroy the dialog 
	 */
	gtk_widget_destroy(GTK_WIDGET(of_dlg));
}

/*
 * Restores the formatting of a message, done in append_line
 */
static gchar *restore_msg(const gchar * given)
{
	gchar *result;
	GString *rest;
	gint s, lines = 0;

	if(!given)
	{
		return g_strdup("");
	}

	rest = g_string_sized_new(strlen(given));
	for (s = 0; s < strlen(given); s++) {
		switch(given[s]) {
		case '\"':
			g_string_append(rest, "\\\"");
			break;
		case '\n':
			g_string_append(rest, "\"\n\"");
			lines++;
			break;
		default:
			g_string_append_c(rest, given[s]);
		}
	}
	result = rest->str;
	g_string_free(rest, FALSE);
	return result;
}

/*
 * Writes one message to a file stream 
 */
static void write_the_message(gpointer data, gpointer fs)
{
	GtrMsg 	*msg;
	GString *string;
	gchar 	*id;
	gchar	*str;
	gchar   *line;
	GError  *errv;

	msg=GTR_MSG(data);

	g_return_if_fail(fs!=NULL);
	g_return_if_fail(msg!=NULL);

	/*
	 * Initialize the used GString with the comment of the message -- 
	 *  if there's any comment for it of course.
	 */
	if(msg->comment)
	{
		string=g_string_new(GTR_COMMENT(msg->comment)->comment);
	}
	else
	{
		string=g_string_new("");
	}

	/*
	 * Preface for the msgid -- the content follows below.
	 */
	string=g_string_append(string,"msgid \"");
	
	/*
	 * Restore the msgid, append it and free it.
	 */
	id=restore_msg(msg->msgid);
	string=g_string_append(string, id);
	GTR_FREE(id);

	/*
	 * Check if we've got a pluram forms message.
	 */
	if(po->header->plural_forms && msg->msgid_plural && msg->msgstr)
	{
		string=g_string_append(string, "\"\nmsgid_plural \"");

		id=restore_msg(msg->msgid_plural);
		string=g_string_append(string, id);
		GTR_FREE(id);

		id=restore_msg(msg->msgstr);

		string=g_string_append(string, "\"\nmsgstr[0] \"");
		string=g_string_append(string, id);
		GTR_FREE(id);

		if(msg->msgstr_1)
		{
			id=restore_msg(msg->msgstr_1);

			string=g_string_append(string, "\"\nmsgstr[1] \"");
			string=g_string_append(string, id);
			GTR_FREE(id);
		}

		if(msg->msgstr_2)
		{
			id=restore_msg(msg->msgstr_2);

			string=g_string_append(string, "\"\nmsgstr[2] \"");
			string=g_string_append(string, id);
			GTR_FREE(id);
		}
	}
	else
	{
		/*
		 * Preface for the msgstr -- the content comes below.
		 */
		string=g_string_append(string, "\"\nmsgstr \"");
	
		str=restore_msg(msg->msgstr);
		string=g_string_append(string, str);
		GTR_FREE(str);
	}
	
	/*
	 * Convert lines from charset specified in header
	 */
	if (po->header && po->header->charset) {
		/*
		 * Convert from header charset to UTF-8
		 */
		line = g_convert(string->str, strlen(string->str), po->header->charset, "utf-8", NULL, NULL, &errv);
		if(!line) {
			g_warning("could not convert message number '%d' from UTF-8 to '%s' to UTF-8: %s", msg->pos, po->header->charset, errv->message);
			line = string->str;
		}
	}
	else {
		line = string->str;
	}
	
	/*
	 * Write the string content and the newlines to our write stream.
	 */
	fprintf((FILE *) fs, "%s\"\n\n", line);
	
	g_string_free(string, TRUE);
}

gboolean gtranslator_save_file(const gchar *name)
{
	GtrMsg 	*header;
	FILE 	*fs;

	if(nautilus_istr_has_suffix(name, ".pot"))
	{
		gchar *warn;
		warn = g_strdup_printf(_("You are saving a file with a .pot extension.\n\
Pot files are generated by the compilation process.\n\
Your file should likely be named '%s.po'."), 
			gtranslator_translator->language->locale);
		gnome_app_warning(GNOME_APP(gtranslator_application), warn);
		
		GTR_FREE(warn);
		return FALSE;
	}
	else if(gtranslator_save_po_file(name))
	{
		return TRUE;
	}
	
	if(!nautilus_strcmp(name, gtranslator_runtime_config->temp_filename))
	{
		/*
		 * Create a new filename to use instead of the temporarily
		 *  created and used file name.
		 */
		if(po->header->prj_version)
		{
			name=g_strdup_printf("%s-%s.%s.po",
				po->header->prj_name,
				po->header->prj_version,
				po->header->language);
		}
		else
		{
			if(po->header->language)
			{
				name=g_strdup_printf("%s.%s.po",
					po->header->prj_name, 
					po->header->language);
			}
			else
			{
				name=g_strdup_printf("%s.po",
					po->header->prj_name);
			}
		}

		/*
		 * Add a foo'sh header entry but only if no header is present.
		 */ 
		if(!po->header->comment)
		{
			po->header->comment="#\n#   -- edited with gtranslator.\n#\n";
		}
		
		/*
		 * Delete the old file.
		 */
		unlink(gtranslator_runtime_config->temp_filename);
	}

	gtranslator_file_dialogs_store_directory(name);

	fs = fopen(name, "w");
	if(!fs)
	{
		gtranslator_utils_error_dialog(
			_("Could not open file `%s' for writing!"),
				  name);
		
		return FALSE;
	}

	gtranslator_header_update(po->header);
	header = gtranslator_header_put(po->header);
	write_the_message(header, (gpointer) fs);
	gtranslator_message_free(header, NULL);

	gtranslator_message_update();
	
	/*
	 * Write every message to the file
	 */
	g_list_foreach(po->messages, (GFunc) write_the_message, (gpointer) fs);

	/*
	 * Store the obsolete entries in the file -- if wished and possible.
	 */
	if(GtrPreferences.keep_obsolete && po->obsolete)
	{
		fprintf(fs, "%s", po->obsolete);
	}

	/*
	 * If we do append a suffix to the autogenerated files, then we'd
	 *  also not disable the file saving entry and we don't also disable
	 *   the save entry on simple compile testing -- check for filename
	 *    equality.
	 */
	if((GtrPreferences.autosave && GtrPreferences.autosave_with_suffix &&
	    	(autosave_source_tag > 1)) ||
	    !nautilus_strcmp(po->filename, name))
	{
		gtranslator_actions_disable(ACT_SAVE);
	}
	
	fclose(fs);
	po->file_changed = FALSE;

	/* If user GtrPreferences to, warn it about fuzzy mesages left */
	if(GtrPreferences.warn_if_fuzzy && po->fuzzy)
	{
		GtkWidget *dialog;

		dialog = gtk_message_dialog_new(
			GTK_WINDOW(gtranslator_application),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_WARNING,
			GTK_BUTTONS_OK,
			_("File %s\ncontains %d fuzzy messages"),
			po->filename, po->fuzzy);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
	}

	return TRUE;
}

/*
 * A callback for OK in Save as... dialog 
 */
void gtranslator_save_file_dialog(GtkWidget * widget, gpointer sfa_dlg)
{
	gchar *po_file;
	po_file = g_strdup(gtk_file_selection_get_filename(GTK_FILE_SELECTION(sfa_dlg)));
	if (!gtranslator_save_file(po_file))
		return;
	GTR_FREE(po->filename);
	po->filename = g_strdup(po_file);
	gtk_widget_destroy(GTK_WIDGET(sfa_dlg));
}

/*
 * A callback for Save
 */
void gtranslator_save_current_file_dialog(GtkWidget * widget, gpointer useless)
{
	if (!po->file_changed) {
		return;
	}

	gtranslator_save_file(po->filename);
}

/*
 * Frees the po variable
 */
void gtranslator_po_free(GtrPo *po)
{
	g_return_if_fail(po!=NULL);

	if (po->messages) 
	{
		g_list_foreach(po->messages, gtranslator_message_free, NULL);
		g_list_free(po->messages);
	}
	
	if (po->header)
	{
		gtranslator_header_free(po->header);
	}
	
	GTR_FREE(po->locale_charset);
	GTR_FREE(po->filename);
	GTR_FREE(po->obsolete);
	GTR_FREE(po);
}

void gtranslator_file_close(GtkWidget * widget, gpointer useless)
{
	if (!file_opened)
		return;
	/*
	 * If user doesn't know what to do with changed file, return
	 */
	if (!gtranslator_should_the_file_be_saved_dialog())
		return;
	
	gtranslator_po_free(po);
	gtranslator_undo_clean_register();
	
	file_opened = FALSE;
	nothing_changes = TRUE;

	/*
	 * Clear the messages table.
	 */
	if(GtrPreferences.show_messages_table)
	{
		gtranslator_messages_table_clear();
	}

#ifdef NOT_PORTED
	/*
	 * Stop gtkspell.
	 */ 
	if(GtrPreferences.instant_spell_check && gtkspell_running())
	{
		gtkspell_stop();
	}
#endif /* NOT_PORTED */	

	gtranslator_text_boxes_clean();
	gtk_label_set_text(GTK_LABEL(extra_content_view->comment), "");
	gtk_widget_set_sensitive(GTK_WIDGET(extra_content_view->edit_button), FALSE);
	gtk_paned_set_position(GTK_PANED(content_pane), 0);
	
	gtranslator_actions_set_up_state_no_file();

	/*
	 * Set blank status, progress and window title
	 */
	gnome_appbar_clear_stack(GNOME_APPBAR(gtranslator_application_bar));
	gnome_appbar_set_progress_percentage(GNOME_APPBAR(gtranslator_application_bar), 0.00000);
	gtk_window_set_title(GTK_WINDOW(gtranslator_application), _("gtranslator"));

	/*
	 * Remove the source tag and set the source tag to '1'.
	 */
	g_source_remove(autosave_source_tag);
	autosave_source_tag=1;

	/*
	 * "Remove" the stored "runtime/filename" key.
	 */
	gtranslator_config_set_string("runtime/filename", "--- No file ---");

	nothing_changes = FALSE;
}

void gtranslator_file_revert(GtkWidget * widget, gpointer useless)
{
	gchar *save_this;
	if (po->file_changed) {
		guint reply;
		reply = gtranslator_file_revert_dialog(NULL, po->filename);
		if (reply != GTK_RESPONSE_YES)
			return;
	}
	save_this = g_strdup(po->filename);
	/*
	 * Let gtranslator_file_close know it doesn't matter if file was changed
	 */
	po->file_changed = FALSE;
	gtranslator_file_close(NULL, NULL);
	gtranslator_parse_main(save_this);
	gtranslator_parse_main_extra();
	GTR_FREE(save_this);
}

/*
 * Remove the given message's translation -- also update the flags.
 */
static void remove_translation(gpointer message, gpointer useless)
{
	GtrMsg 	*msg=GTR_MSG(message);
	gboolean internal_change=FALSE;
	
	g_return_if_fail(msg!=NULL);
	g_return_if_fail(msg->msgid!=NULL);

	/*
	 * Free any translation which is left over.
	 */
	if(msg->msgstr)
	{
		GTR_FREE(msg->msgstr);
		internal_change=TRUE;
	}

	/*
	 * Remove the "translated" status bits and set the untranslated bit.
	 */
	if(msg->status & GTR_MSG_STATUS_TRANSLATED)
	{
		msg->status &= ~GTR_MSG_STATUS_TRANSLATED;
		internal_change=TRUE;
	}
	
	if(msg->status & GTR_MSG_STATUS_FUZZY)
	{
		gtranslator_message_status_set_fuzzy(msg, FALSE);
		po->fuzzy--;
		internal_change=TRUE;
	}

	msg->status |= GTR_MSG_STATUS_UNKNOWN;

	/*
	 * If we did perform any internal change, activate the Save/Revert
	 *  actions and set the po file's "file_changed" variable to TRUE.
	 */
	if(internal_change)
	{
		po->file_changed=TRUE;
		
		gtranslator_actions_enable(ACT_SAVE, ACT_REVERT);
	}
}

/*
 * Remove all translations from the current po file.
 */
void gtranslator_remove_all_translations()
{
	gboolean	prev_file_changed=po->file_changed;
	
	g_return_if_fail(file_opened==TRUE);
	g_return_if_fail(po->messages!=NULL);

	/*
	 * Remove the translations, update the statistics.
	 */
	g_list_foreach(po->messages, (GFunc) remove_translation, NULL);
	gtranslator_get_translated_count();

	/*
	 * Update (clear and rebuild) the messages table/tree.
	 */
	if(GtrPreferences.show_messages_table)
	{
		gtranslator_messages_table_clear();
		gtranslator_messages_table_create();
	}

	/*
	 * Re-go to the current message if any change has been made.
	 */
	if(!prev_file_changed && po->file_changed)
	{
		gtranslator_message_go_to(po->current);
	}

	/*
	 * Check if there are any messages left over which could be
	 *  removed -- enable/disable the menu entry accordingly.
	 */
	if(po->translated < 1 || po->fuzzy < 1)
	{
		gtranslator_actions_disable(ACT_REMOVE_ALL_TRANSLATIONS);
	}

	/*
	 * Enable the next fuzzy, untranslated menu/toolbar entries according to
	 *  the messages stati.
	 */
	if(po->fuzzy > 0)
	{
		gtranslator_actions_enable(ACT_NEXT_FUZZY);
	}

	if((po->length - po->translated) > 0)
	{
		gtranslator_actions_enable(ACT_NEXT_UNTRANSLATED);
	}
}

/*
 * The compile function
 */
void compile(GtkWidget * widget, gpointer useless)
{
	gchar	*cmd,
		*status,
		*test_file_name,
		*output_file_name,
		*result_file_name;
		
	gchar 	line[128];
	gint 	res = 1;
	FILE 	*fs;
	

	/*
	 * Check if msgfmt is available on the system.
	 */
	if(!g_find_program_in_path("msgfmt"))
	{
		gtranslator_utils_error_dialog(_("Sorry, msgfmt isn't available on your system!"));
		return;
	}
			
	if (!file_opened) 
	{
		return;
	}

	gtranslator_utils_get_compile_file_names(&test_file_name, 
		&output_file_name, &result_file_name);

	if (!gtranslator_save_file(test_file_name))
	{
		return;
	}

	cmd = g_strdup_printf("msgfmt -v -c -o '%s' '%s' > '%s' 2>&1",
			    output_file_name, test_file_name, result_file_name);
	
	res = system(cmd);
	fs=fopen(result_file_name,"r");

	/*
	 * If there has been an error show an error-box with some kind of
	 *  cleaned error message.
	 */
	if (res != 0)
	{
		gtranslator_compile_error_dialog(fs);
	}
	else
	{
		fgets(line, sizeof(line), fs);
		g_strchomp(line);
		status=g_strdup_printf(_("Compile successful:\n%s"), line);
		gnome_app_message(GNOME_APP(gtranslator_application), status);
		GTR_FREE(status);
	}
	
	fclose(fs);
	GTR_FREE(cmd);

	gtranslator_utils_remove_compile_files(&test_file_name, 
		&output_file_name, &result_file_name);
}

/*
 * A helper function simply increments the "translated" variable of the
 *  po-file.
 */
static void determine_translation_status(gpointer data, gpointer useless_stuff)
{
	GtrMsg *message = GTR_MSG(data);
	if(message->status & GTR_MSG_STATUS_TRANSLATED)
		po->translated++;
	if(message->status & GTR_MSG_STATUS_FUZZY)
		po->fuzzy++;
}

/*
 * Now get the complete count of the translated entries.
 */
void gtranslator_get_translated_count(void)
{
	po->translated = 0;
	po->fuzzy = 0;
	g_list_foreach(po->messages, (GFunc) determine_translation_status,
		       NULL);

	/*
	 * Update the progress bar.
	 */
	gtranslator_set_progress_bar();
}

void gtranslator_set_progress_bar(void)
{
	gfloat percentage;
	
	/*
	 * Calculate the percentage.
	 */
	percentage=1.0 * po->translated / po->length;

	/*
	 * Set the progress only if the values are reasonable.
	 */
	if(percentage > 0.0 || percentage < 1.0)
	{
		/*
		 * Set the progressbar status.
		 */
		gnome_appbar_set_progress_percentage(
			GNOME_APPBAR(gtranslator_application_bar),
			percentage);
	}
}
