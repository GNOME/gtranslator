/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
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
#include "gtkspell.h"
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
#include "sidebar.h"
#include "translator.h"
#include "undo.h"
#include "utf8.h"
#include "utils.h"
#include "utils_gui.h"

#include <gtk/gtkfilesel.h>
#include <gtk/gtkmain.h>
#include <gtk/gtklabel.h>

#include <libgnome/gnome-util.h>

#include <libgnomeui/gnome-appbar.h>
#include <libgnomeui/gnome-app-util.h>
#include <libgnomeui/gnome-dialog-util.h>
#include <libgnomeui/gnome-messagebox.h>
#include <libgnomeui/gnome-stock.h>
#include <libgnomeui/gnome-uidefs.h>

#include <gal/e-paned/e-paned.h>

/* Global variables */
GtrPo *po;
gboolean file_opened;
gboolean message_changed;
guint autosave_source_tag=1;

/*
 * These are to be used only inside this file
 */
static void append_line(gchar ** old, const gchar * tail);
static gboolean add_to_obsolete(gchar *comment);
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
static void append_line(gchar ** old, const gchar * tail)
{
	gchar *to_add = g_new(gchar, strlen(tail));
	gchar *result;
	gint s, d = 0;
	for (s = 1; s < strlen(tail) - 1; s++) {
		if (tail[s] == '\\') {
			s++;
			if (tail[s] == 'n')
				to_add[d++] = '\n';
			else if(tail[s] == 'r')
				to_add[d++] = '\r';
			else if (tail[s] == 't')
				to_add[d++] = '\t';
			else if ((tail[s] == '"') || (tail[s] == '\\'))
				to_add[d++] = tail[s];
			else {
				to_add[d++] = '\\';
				to_add[d++] = tail[s];
				g_warning
				    (_("New escaped char found: \\%c\nAdd this to parse.c, line %i."),
				     tail[s], __LINE__);
			}
		} else {
			to_add[d++] = tail[s];
		}
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

gboolean add_to_obsolete(gchar *comment)
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
void gtranslator_parse(const gchar *filename)
{
	gboolean first_is_fuzzy;
	gchar *base;

	g_return_if_fail(filename!=NULL);

	base=g_basename(filename);
	g_return_if_fail(base[0]!='\0');

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
		return;
	}
	
	if (!gtranslator_parse_core())
	{
		gtranslator_po_free();
		return;
	}

#define FIRST_MSG GTR_MSG(po->messages->data)
	first_is_fuzzy=(FIRST_MSG->status & GTR_MSG_STATUS_FUZZY) != 0;
	gtranslator_message_status_set_fuzzy(FIRST_MSG, FALSE);
	
	/*
	 * If the first message is header (it always should be)
	 */
	po->header = gtranslator_header_get(FIRST_MSG);

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
	}
	
	file_opened = TRUE;
	po->file_changed = FALSE;
	po->length = g_list_length(po->messages);

	/*
	 * Set the utf8 field of the GtrPo to TRUE if we are editing an UTF-8 
	 *  encoded file.
	 */
	if(gtranslator_utf8_po_file_is_utf8())
	{
		po->utf8=TRUE;
	}
	else
	{
		po->utf8=FALSE;
	}

	po->locale_charset=gtranslator_utils_get_locale_charset();

	/*
	 * Set the current message to the first message.
	 */
	po->current = g_list_first(po->messages);
}

gboolean gtranslator_parse_core(void)
{
	GtrMsg 	*msg;
	
	FILE 	*fs;
	char 	*line;
	
	guint 	 lines=0;
	guint	 position=-1;
	
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
	while ((line = gtranslator_utils_getline (fs)) != NULL) {
		
		lines++;
		
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
					if(add_to_obsolete(GTR_COMMENT(msg->comment)->comment))
					{
						gtranslator_comment_free(&msg->comment);
						msg->comment=NULL;
					}
				}
			} else
			/*
			 * If it's a msgid
			 */
			if (nautilus_str_has_prefix(line, "msgid \"")) {
				/*
				 * This means the comment is completed
				 */
				comment_ok = TRUE;
				if (line[8] != '\0')
					append_line(&msg->msgid, &line[6]);
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
					append_line(&msg->msgstr, &line[7]);
			}
			else
			/*
			 * A continuing msgid or msgstr
			 */
			if (line[0] == '"') {
				if((comment_ok == TRUE) &&
				   (msgid_ok == FALSE))
				{
					append_line(&msg->msgid, line);
				}
				else if((msgid_ok == TRUE) &&
					(msgstr_ok == FALSE))
				{
					append_line(&msg->msgstr, line);
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
		add_to_obsolete(GTR_COMMENT(msg->comment)->comment))
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
	gchar 	*title;

	/*
	 * Test if such a file does exist.
	 */
	if(!g_file_exists(filename))
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
		return;
	}
	else
	{
		gtranslator_config_init();
		gtranslator_config_set_string("runtime/filename", 
			(gchar *) filename);
		gtranslator_config_close();
	}

	/*
	 * Use the new core function.
	 */
	gtranslator_parse(filename);

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

	gtranslator_application_bar_update(0);
	
	if(po->header)
	{
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
	
	gtranslator_message_show(po->current);

	/*
	 * Test if the filename is NOT equivalent to our temp files' names.
	 */
	if(nautilus_strcasecmp(g_basename(po->filename), 
		gtranslator_runtime_config->temp_filename) || 
	   nautilus_strcasecmp(g_basename(po->filename),
	   	gtranslator_runtime_config->backend_filename) ||
	   nautilus_strcasecmp(g_basename(po->filename),
	   	gtranslator_runtime_config->crash_filename))
		
	{
		gtranslator_history_add(po->filename,
			po->header->prj_name, po->header->prj_version);
	
		title=g_strdup_printf(_("gtranslator -- %s"), po->filename);
		gtk_window_set_title(GTK_WINDOW(gtranslator_application), title);
		GTR_FREE(title);
	}

	/*
	 * Update the recent files list.
	 */
	gtranslator_history_show();

	/*
	 * Add the view for the current file.
	 */ 
	gtranslator_sidebar_activate_views();

	/*
	 * Create and set the messages table/tree up.
	 */
	if(GtrPreferences.show_messages_table)
	{
		gtranslator_messages_table_create();
	}

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
	po_file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(of_dlg));

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
	gint s, lines = 0, here = 8;

	if(!given)
	{
		return g_strdup("");
	}

	rest = g_string_sized_new(strlen(given));
	for (s = 0; s < strlen(given); s++) {
		if (given[s] == '\n') {
			if ((!lines) && (s + 1 < strlen(given))) {
				lines++;
				g_string_prepend(rest, "\"\n\"");
			}
			if ((s + 1 < strlen(given)) && (here < 78)) {
				g_string_append(rest, "\\n\"\n\"");
				here = 0;
				lines++;
			} else
				g_string_append(rest, "\\n");
		} else if (given[s] == '\"') {
			g_string_append(rest, "\\\"");
			here++;
		} else if (given[s] == '\t') {
			g_string_append(rest, "\\t");
			here++;
		} else if (given[s] == '\\') {
			g_string_append(rest, "\\\\");
			here++;
		} else {
			g_string_append_c(rest, given[s]);
		}
		here++;
		if (here > 78) {
			if (!lines) {
				g_string_prepend(rest, "\"\n\"");
				here -= 7;
			} else {
				if(rest->str)
				{
					g_string_insert(rest,
							(strrchr(rest->str, ' ') -
							 rest->str) + 1, "\"\n\"");
					here =
					    rest->len - (strrchr(rest->str, '"') -
							 rest->str);
				}			 
			}
			lines++;
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
	 * Preface for the msgstr -- the content comes below.
	 */
	string=g_string_append(string, "\"\nmsgstr \"");
	
	str=restore_msg(msg->msgstr);
	string=g_string_append(string, str);
	GTR_FREE(str);
	
	/*
	 * Write the string content and the newlines to our write stream.
	 */
	fprintf((FILE *) fs, "%s\"\n\n", string->str);
	
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
		gchar *warn;
		warn = g_strdup_printf(_("File %s\n"
				       "contains %d fuzzy messages"),
				       po->filename, po->fuzzy);
		gnome_warning_dialog_parented(warn, 
			GTK_WINDOW(gtranslator_application));
		GTR_FREE(warn);
	}

	return TRUE;
}

/*
 * A callback for OK in Save as... dialog 
 */
void gtranslator_save_file_dialog(GtkWidget * widget, gpointer sfa_dlg)
{
	gchar *po_file;
	po_file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(sfa_dlg));
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
		if (GtrPreferences.dont_save_unchanged_files)
			return;
		if (GtrPreferences.warn_if_no_change) {
			GtkWidget *dialog;
			gchar *question;
			gint reply;
			question = g_strdup_printf(_("You didn't change anything in\n%s\nDo you want to save it anyway?"),
					    po->filename);
			dialog = gnome_message_box_new(question,
			    GNOME_MESSAGE_BOX_QUESTION,
			    GNOME_STOCK_BUTTON_YES,
			    GNOME_STOCK_BUTTON_NO,
			    GNOME_STOCK_BUTTON_CANCEL,
			    NULL);
			gtranslator_dialog_show(&dialog, _("gtranslator -- unchanged"));
			reply = gnome_dialog_run(GNOME_DIALOG(dialog));
			GTR_FREE(question);
			if (reply != GNOME_YES)
				return;
		}
	}

	gtranslator_save_file(po->filename);
}

/*
 * Frees the po variable
 */
void gtranslator_po_free(void)
{
	if(!po)
		return;
	if (po->messages) 
	{
		g_list_foreach(po->messages, gtranslator_message_free, NULL);
		g_list_free(po->messages);
	}
	
	if (po->header)
	{
		gtranslator_header_free(po->header);
	}
	
	if(po->locale_charset)
	{
		GTR_FREE(po->locale_charset);
	}
	
	GTR_FREE(po->filename);
	GTR_FREE(po->obsolete);
	GTR_FREE(po);
	po = NULL;
}

void gtranslator_file_close(GtkWidget * widget, gpointer useless)
{
	if (!file_opened)
		return;
	/*
	 * If user doesn't know what to do with changed file, return
	 */
	if (!gtranslator_should_the_file_be_saved_dialog)
		return;
	
	gtranslator_po_free();
	gtranslator_undo_clean_register();
	gtranslator_sidebar_clear_views();
	
	file_opened = FALSE;
	nothing_changes = TRUE;

	/*
	 * Clear the messages table.
	 */
	if(GtrPreferences.show_messages_table)
	{
		gtranslator_messages_table_clear();
	}

	/*
	 * Stop gtkspell.
	 */ 
	if(GtrPreferences.instant_spell_check && gtkspell_running())
	{
		gtkspell_stop();
	}
	
	gtranslator_text_boxes_clean();
	gtk_label_set_text(GTK_LABEL(extra_content_view->comment), "");
	gtk_widget_set_sensitive(GTK_WIDGET(extra_content_view->edit_button), FALSE);
	e_paned_set_position(E_PANED(content_pane), 0);
	
	gtranslator_actions_set_up_state_no_file();

	/*
	 * Set blank status, progress and window title
	 */
	gnome_appbar_clear_stack(GNOME_APPBAR(gtranslator_application_bar));
	gnome_appbar_set_progress(GNOME_APPBAR(gtranslator_application_bar), 0.00000);
	gtk_window_set_title(GTK_WINDOW(gtranslator_application), _("gtranslator"));

	/*
	 * Remove the source tag and set the source tag to '1'.
	 */
	g_source_remove(autosave_source_tag);
	autosave_source_tag=1;

	/*
	 * "Remove" the stored "runtime/filename" key.
	 */
	gtranslator_config_init();
	gtranslator_config_set_string("runtime/filename", "--- No file ---");
	gtranslator_config_close();

	nothing_changes = FALSE;
}

void gtranslator_file_revert(GtkWidget * widget, gpointer useless)
{
	gchar *save_this;
	if (po->file_changed) {
		GtkWidget *dialog;
		gchar *question;
		gint reply;
		question =
		    g_strdup_printf(
				    _("File %s\nwas changed. Do you want to revert to saved copy?"),
				    po->filename);
		dialog =
		    gnome_message_box_new(question,
					  GNOME_MESSAGE_BOX_QUESTION,
					  GNOME_STOCK_BUTTON_YES,
					  GNOME_STOCK_BUTTON_NO,
					  GNOME_STOCK_BUTTON_CANCEL, NULL);
		gtranslator_dialog_show(&dialog, "gtranslator -- revert");
		reply = gnome_dialog_run(GNOME_DIALOG(dialog));
		GTR_FREE(question);
		if (reply != GNOME_YES)
			return;
	}
	save_this = g_strdup(po->filename);
	/*
	 * Let gtranslator_file_close know it doesn't matter if file was changed
	 */
	po->file_changed = FALSE;
	gtranslator_file_close(NULL, NULL);
	gtranslator_parse_main(save_this);
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
	if(!gnome_is_program_in_path("msgfmt"))
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
		gnome_appbar_set_progress(
			GNOME_APPBAR(gtranslator_application_bar),
			percentage);
	}
}

