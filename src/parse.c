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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "actions.h"
#include "dialogs.h"
#include "gtkspell.h"
#include "gui.h"
#include "history.h"
#include "message.h"
#include "nautilus-string.h"
#include "open-differently.h"
#include "parse.h"
#include "prefs.h"
#include "sidebar.h"
#include "utils.h"

#include <gtk/gtkfilesel.h>

#include <libgnomeui/gnome-appbar.h>
#include <libgnomeui/gnome-app-util.h>
#include <libgnomeui/gnome-dialog-util.h>
#include <libgnomeui/gnome-messagebox.h>
#include <libgnomeui/gnome-stock.h>
#include <libgnomeui/gnome-uidefs.h>
#include <libgnome/gnome-util.h>

/* Global variables */
GtrPo *po;
gboolean file_opened;
gboolean message_changed;
GnomeRegexCache *rxc;
guint autosave_source_tag=1;

/*
 * These are to be used only inside this file
 */
static void append_line(gchar ** old, const gchar * tail);
static gboolean add_to_obsolete(gchar *comment);
static void write_the_message(gpointer data, gpointer fs);
static gchar *restore_msg(const gchar * given);
static void determine_translation_status(gpointer data, gpointer useless_stuff);

static void check_msg_status(GtrMsg * msg)
{
	if (msg->msgstr)
		msg->status = GTR_MSG_STATUS_TRANSLATED;
	if ((msg->comment) && (strstr(msg->comment, "#, fuzzy")))
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
		g_free(*old);
		g_free(to_add);
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
				tmp=g_strconcat(po->obsolete, "\n", 
						comment, NULL);
				g_free(po->obsolete);
				po->obsolete=tmp;
			}
		}
		return TRUE;
	}
	return FALSE;
}

gboolean gtranslator_parse_core(void)
{
	FILE *fs;
	char *line;
	guint lines = 0;
	
	/*
	 * If TRUE, means that a corresponding part is read
	 */
	gboolean msgid_ok = FALSE, msgstr_ok = FALSE, comment_ok = FALSE;
	GtrMsg *msg;

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
				/*msg->pos = lines;*/
				msg->comment = g_strdup(line);
			} else {
				gchar *tmp;
				tmp = g_strconcat(msg->comment,	line, NULL);

				g_free(msg->comment);
				msg->comment = tmp;
			}
		}
		else {
			g_strchomp(line);

			if (line[0] == '\0') {
				if(msgid_ok==TRUE)
				{
					msgstr_ok = TRUE;
				}
				else
				{
					if(add_to_obsolete(msg->comment)) {
						g_free(msg->comment);
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
				gtranslator_error(_("Error in file \"%s\"\nat line %d.\nPlease check the file and try again."), po->filename, lines);
				g_free(msg);
				return FALSE;
			}
		}
		/*
		 * we've got both msgid + msgstr
		 */
		if ((msgid_ok == TRUE) && (msgstr_ok == TRUE)) {
			check_msg_status(msg);
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
		po->messages = g_list_prepend(po->messages, (gpointer) msg);
	}
	else if(add_to_obsolete(msg->comment))
	{
		g_free(msg->comment);
		g_free(msg);
	}

	fclose(fs);

	if (po->messages == NULL) {
		gtranslator_error(_("The file is empty:\n%s"), po->filename);
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
	gchar *title;

	/*
	 * Test if such a file does exist.
	 */
	if(!g_file_exists(filename))
	{
		gtranslator_error(_("The file `%s' doesn't exist at all!"),
			filename);
		return;
	}

	/*
	 * Use the new core function.
	 */
	gtranslator_parse(filename);

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
	 * Test if the filename is NOT equivalent to our temp file's name
	 */
	if(strcmp(g_basename(po->filename), "gtranslator-temp-po-file"))
	{
		gtranslator_history_add(po->filename,
			po->header->prj_name, po->header->prj_version);
	
		title=g_strdup_printf(_("gtranslator -- %s"), po->filename);
		gtk_window_set_title(GTK_WINDOW(gtranslator_application), title);
		g_free(title);
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
	 * Detect via the new functions the right open function for the file.
	 */
	if(!gtranslator_open_po_file(po_file))
	{
		/*
		 * Open it as a "normal" gettext po file.
		 */ 
		gtranslator_parse_main(po_file);
	}
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
	GtrMsg *msg = GTR_MSG(data);
	gchar *id, *str;

	g_return_if_fail(fs!=NULL);
	g_return_if_fail(msg!=NULL);

	id = restore_msg(msg->msgid);
	str = restore_msg(msg->msgstr);
	
	if(msg->comment)
	{
		fprintf((FILE *) fs, "%smsgid \"%s\"\nmsgstr \"%s\"\n\n",
			msg->comment, id, str);
	}
	else
	{
		fprintf((FILE *) fs, "msgid \"%s\"\nmsgstr \"%s\"\n\n",
			id, str);
	}
	
	/*
	 * Unknown segfault reason causes us to uncomment the two g_free's.
	 *
	 * g_free(id);
	 * g_free(msg);
	 */
}

gboolean gtranslator_save_file(const gchar *name)
{
	GtrMsg *header;
	FILE *fs;
	gchar *tempo;

	if (nautilus_istr_has_suffix(name, ".pot"))
	{
		gchar *warn;
		warn = g_strdup_printf(_("You are trying to save file with *.pot extension.\nHowever, such files are generated by program.\nYour file should be named with *.po extension, most likely %s.po"), lc);
		gnome_app_warning(GNOME_APP(gtranslator_application), warn);
		
		g_free(warn);
		return FALSE;
	}
	
	tempo=gtranslator_utils_get_temp_file_name();

	/* FIXME: pop up save as... dialog here */
	if(!nautilus_strcmp(name, tempo))
	{
		/*
		 * Create a new filename to use instead of the
		 *  oldsome "gtranslator-temp-po-file"...
		 */  
		name=g_strdup_printf("%s-%s.%s.po",
			po->header->prj_name,
			po->header->prj_version,
			po->header->language);

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
		unlink(tempo);
	}

	g_free(tempo);

	gtranslator_file_dialogs_store_directory(name);

	fs = fopen(name, "w");
	if(!fs)
	{
		gtranslator_error(_("Could not open file `%s' for writing!"),
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
		fprintf(fs, po->obsolete);
	}

	/*
	 * If we do append a suffix to the autogenerated files, then we'd
	 *  also not disable the file saving entry.
	 */
	if(GtrPreferences.autosave && GtrPreferences.autosave_with_suffix &&
		(autosave_source_tag > 1))
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
		gnome_warning_dialog_parented(warn, GTK_WINDOW(gtranslator_application));
		g_free(warn);
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
	g_free(po->filename);
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
			g_free(question);
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
	if (po->messages) {
		g_list_foreach(po->messages, gtranslator_message_free, NULL);
		g_list_free(po->messages);
	}
	if (po->header)
		gtranslator_header_free(po->header);
	g_free(po->filename);
	g_free(po->obsolete);
	g_free(po);
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
	gtranslator_sidebar_clear_views();
	
	file_opened = FALSE;
	nothing_changes = TRUE;

	/*
	 * Stop gtkspell.
	 */ 
	if(GtrPreferences.instant_spell_check && gtkspell_running())
	{
		gtkspell_stop();
	}
	
	gtranslator_text_boxes_clean();
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
}

void gtranslator_file_revert(GtkWidget * widget, gpointer useless)
{
	gchar *save_this;
	if (po->file_changed) {
		GtkWidget *dialog;
		gchar *question;
		gint reply;
		question =
		    g_strdup_printf(_
				    ("File %s\nwas changed. Do you want to revert to saved copy?"),
				    po->filename);
		dialog =
		    gnome_message_box_new(question,
					  GNOME_MESSAGE_BOX_QUESTION,
					  GNOME_STOCK_BUTTON_YES,
					  GNOME_STOCK_BUTTON_NO,
					  GNOME_STOCK_BUTTON_CANCEL, NULL);
		gtranslator_dialog_show(&dialog, "gtranslator -- revert");
		reply = gnome_dialog_run(GNOME_DIALOG(dialog));
		g_free(question);
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
	g_free(save_this);
}

/*
 * The compile function
 */
void compile(GtkWidget * widget, gpointer useless)
{
	gchar *cmd, *status;
	gchar line[128];
	gint res = 1;
	FILE *fs;

	/*
	 * Check if msgfmt is available on the system.
	 */
	if(!gnome_is_program_in_path("msgfmt"))
	{
		gtranslator_error(_("Sorry, msgfmt isn't available on your system!"));
		return;
	}
			
	if (!file_opened) 
		return;
	
	if (!gtranslator_save_file(po->filename))
		return;

#define RESULT "gtr_result.tmp"
	cmd = g_strdup_printf("msgfmt -v -c -o /dev/null %s >%s 2>&1",
			    po->filename, RESULT);
	res = system(cmd);
	fs=fopen(RESULT,"r");
	/*
	 * If there has been an error show an error-box
	 */
	if (res != 0) {
		gtranslator_compile_error_dialog(fs);
	} else {
		fgets(line, sizeof(line), fs);
		g_strchomp(line);
		status=g_strdup_printf(_("Compile successful:\n%s"), line);
		gnome_app_message(GNOME_APP(gtranslator_application), status);
		g_free(status);
	}
	fclose(fs);
	remove(RESULT);
	g_free(cmd);
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
	/*
	 * Sticky messages are always translated, do not count them twice
	if(message->status & GTR_MSG_STATUS_STICK)
		po->translated++;
	*/
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
	/*
	 * Set the progressbar status.
	 */
	gnome_appbar_set_progress(GNOME_APPBAR(gtranslator_application_bar),
				  1.0 * po->translated / po->length);
}

