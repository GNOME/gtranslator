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

#include <string.h>

#include "dialogs.h"
#include "parse.h"
#include "prefs.h"
#include "gui.h"
#include "open-differently.h"
#include "history.h"
#include "sidebar.h"
#include "gtkspell.h"

/*
 * These are to be used only inside this file
 */
static void append_line(gchar ** old, const gchar * tail);
static void write_the_message(gpointer data, gpointer fs);
static gboolean actual_write(const gchar * name);
static gchar *restore_msg(const gchar * given);
static void determine_translation_status(gpointer data, gpointer useless_stuff);

void mark_msg_fuzzy(GtrMsg * msg, gboolean fuzzy)
{
	regex_t *rex;
	regmatch_t pos[3];
	gchar *comment = msg->comment;
	
	/* 
	 * If fuzzy status is already correct
	 */
	if (((msg->status & GTR_MSG_STATUS_FUZZY) != 0) == fuzzy)
		return;
	if (fuzzy) {
		msg->status |= GTR_MSG_STATUS_FUZZY;
		po->fuzzy++;
		rex = gnome_regex_cache_compile(rxc,
			  "^(#), c-format", 
			  REG_EXTENDED | REG_NEWLINE);
		if (!regexec(rex, comment, 3, pos, 0)) {
			comment[pos[1].rm_so] = '\0';
			msg->comment = g_strdup_printf("%s#, fuzzy%s", comment, 
					    comment+pos[1].rm_eo);
		} else {
			msg->comment = g_strdup_printf("%s#, fuzzy\n", comment);
		}
		g_free(comment);
	} else {
		msg->status &= ~GTR_MSG_STATUS_FUZZY;
		po->fuzzy--;
		rex = gnome_regex_cache_compile(rxc, 
			  "(^#, fuzzy$)|^#, (fuzzy,) c-format",
			  REG_EXTENDED | REG_NEWLINE);
		if (!regexec(rex, comment, 3, pos, 0)) {
			gint i = (pos[1].rm_so == -1) ? 2 : 1;
			strcpy(comment+pos[i].rm_so, comment+pos[i].rm_eo+1);
		}
	}
}

void mark_msg_sticky (GtrMsg * msg, gboolean on)
{
	if (on) {
		g_free(msg->msgstr);
		msg->msgstr = strdup(msg->msgid);
		
		/*
		 * It is no longer fuzzy
		 */
		mark_msg_fuzzy(msg, FALSE);
		msg->status |= GTR_MSG_STATUS_STICK;
	} else {
		g_free(msg->msgstr);
		msg->msgstr = strdup("");
		msg->status &= ~GTR_MSG_STATUS_STICK;
	}
	gtranslator_get_translated_count();
}

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
 * 
 * TODO: make this use GString
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

gboolean actual_parse(void)
{
	FILE *fs;
	gchar line[256];
	guint lines = 0;
	
	/*
	 * If TRUE, means that a corresponding part is read
	 */
	gboolean msgid_ok = FALSE, msgstr_ok = FALSE, comment_ok = FALSE;
	GtrMsg *msg;
	gchar *error = NULL;

	/*
	 * The write permissions aren't always guaranteed.
	 */ 
	if(po->no_write_perms==FALSE)
	{
		fs = fopen(po->filename, "r+");
	}
	else
	{
		fs = fopen(po->filename, "r");
	}
	
	/*
	 * As the po-file seems to exist, set the "count parameters" to 0.
	 */
	po->translated=0;
	po->fuzzy=0;
	
	msg = g_new0(GtrMsg, 1);
	/*
	 * Parse the file line by line...
	 */
	while (fgets(line, sizeof(line), fs) != NULL) {
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
				msg->comment = g_strdup(line);
			} else {
				gchar *tmp;
				tmp = g_strconcat(msg->comment,	line, NULL);

				g_free(msg->comment);
				msg->comment = tmp;
			}
		}
		else if(msg->comment && 
			strstr(msg->comment, "#~ msgstr") && g_strstrip(line))
		{
			/*
			 * Catch up the extralong obsolete messages and make
			 *  them stored in the right human-readable format
			 *   and not without any newlines.
			 */
			gchar *zup;

			zup=g_strconcat(msg->comment, "\n", NULL);
			g_free(msg->comment);
			msg->comment=zup;
		}
		else {
			/*
			 * Get rid of end-of-lines...
			 */
			g_strchomp(line);
			if (strlen(line) == 0) {
				if(msgid_ok==TRUE)
				{
					msgstr_ok = TRUE;
				}
			} else
			/*
			 * If it's a msgid
			 */
			if (!g_strncasecmp(line, "msgid \"", 7)) {
				/*
				 * This means the comment is completed
				 */
				comment_ok = TRUE;
				if (strlen(line) - 8 > 0)
					append_line(&msg->msgid, &line[6]);
			} 
			/*
			 * If it's a msgstr. 
			 */
			else if (!g_strncasecmp(line, "msgstr \"", 8))
			{
				/*
				 * This means the msgid is completed
				 */
				msgid_ok = TRUE;
				if (strlen(line) - 9 > 0)
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
					goto ERRR;
				}
			} else {
				ERRR:
				error = g_strdup_printf(_("Error in file \"%s\"\nat line %d.\nPlease check the file and try again."), po->filename, lines);
				gnome_app_error(GNOME_APP(app1), error);
				g_free(error);
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
	 * If there was no newline at end of file
	 */
	if ((msgid_ok == TRUE) && (msgstr_ok == FALSE))
	{
		check_msg_status(msg);
		po->messages = g_list_prepend(po->messages, (gpointer) msg);
	}
	else if((msgid_ok==FALSE) &&
		  (msgstr_ok==FALSE) &&
		  msg->comment)
	{
		/*
		 * Store the obsolete entries if wished.
		 */
		if(wants.keep_obsolete)
		{
			po->obsolete=g_strdup(msg->comment);
		}
		  
		/*
		 * Not needed allocated structure
		 */
		g_free(msg);
	}
	fclose(fs);

	if (po->messages == NULL) {
		error = g_strdup_printf(_("The file is empty:\n%s"), 
			   po->filename);
		gnome_app_error(GNOME_APP(app1), error);
		g_free(error);
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
void parse(const gchar *filename)
{
	/*
	 * Use the new core function.
	 */
	parse_core(filename);
	
	/*
	 * Test if such a file does exist.
	 */
	if(!g_file_exists(po->filename))
	{
		gchar *error;
		error=g_strdup_printf(
			_("The file `%s' doesn't exist at all!"),
			po->filename);
		
		gnome_app_error(GNOME_APP(app1), error);
		
		g_free(error);

		return;
	}

	/*
	 * Disable the special navigation buttons now.
	 */
	disable_actions(ACT_NEXT_FUZZY, ACT_NEXT_UNTRANSLATED);
	
	if(po->no_write_perms==TRUE||
		!strcmp(g_basename(po->filename), "gtranslator-temp-po-file")||
		strstr(po->filename, "/.gtranslator-"))
	{
		disable_actions(ACT_SAVE);
	}

	g_snprintf(status, sizeof(status),
		   _("Successfully parsed the file \"%s\""), po->filename);
	gnome_appbar_set_status(GNOME_APPBAR(appbar1), status);
	g_snprintf(status, sizeof(status),
		   _("gtranslator -- %s"), po->filename);
	gtk_window_set_title(GTK_WINDOW(app1), status);

	/*
	 * Test if the file has got a header and then check if the header
	 *  is still in a virginary form or not.
	 */  
	if(po->header)
	{
		/*
		 * If we've got still the default values for project
		 *  and version the "Edit Header" dialog pops up and
		 *   makes it possible to enter the missing informations.
		 */   
		if(!strcmp(po->header->prj_name, "PACKAGE")
			&& !strcmp(po->header->prj_version, "VERSION"))
		{
			/*
			 * Substitute the values for the header fields.
			 */
			gtranslator_header_fill_up(po->header);
			edit_header(NULL, NULL);
		}
	}
	
	/*
	 * Test if the filename is NOT equivalent to our temp file's name
	 */
	if(strcmp(g_basename(po->filename), "gtranslator-temp-po-file"))
	{
		gtranslator_history_add(po->filename,
			po->header->prj_name, po->header->prj_version);
	
	}

	/*
	 * Add the view for the current file.
	 */ 
	gtranslator_sidebar_activate_views();
	
	display_msg(po->current);
	gtranslator_get_translated_count();
	enable_actions_just_opened();
	/*
	 * Is there any fuzzy message ?
	 */
	if(po->fuzzy>0)
	{
		/*
		 * Then enable the Fuzzy buttons/entries in the menus
		 */
		enable_actions(ACT_NEXT_FUZZY);
	}
	/*
	 * Is there any untranslated message ?
	 */
	if((po->length - po->translated) > 0)
	{
		/*
		 * Then enable the Untranslated buttons/entries in the menus
		 */
		enable_actions(ACT_NEXT_UNTRANSLATED);
	}
	/*
	 * Disable the actions for the first/back navigation actions.
	 */
	disable_actions(ACT_FIRST, ACT_BACK);
	
	/*
	 * Update the recent files list.
	 */
	gtranslator_history_show();
}

void parse_the_file(GtkWidget * widget, gpointer of_dlg)
{
	gchar *po_file;
	po_file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(of_dlg));

	if(file_opened)
		close_file(NULL, NULL);
	/*
	 * Detect via the new functions the right open function for the file.
	 */
	if(!gtranslator_open_po_file(po_file))
	{
		/*
		 * Open it as a "normal" gettext po file.
		 */ 
		parse(po_file);
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

	id = restore_msg(msg->msgid);
	str = restore_msg(msg->msgstr);
	
	if (msg->comment)
		fprintf((FILE *) fs, "%smsgid \"%s\"\nmsgstr \"%s\"\n\n",
			msg->comment, id, str);
	else
		fprintf((FILE *) fs, "msgid \"%s\"\nmsgstr \"%s\"\n\n",
			id, str);
	/*
	 * Strange, but these cause SIGSEGV
	g_free(id);
	g_free(msg);
	*/
}

static gboolean actual_write(const gchar * name)
{
	GtrMsg *header;
	FILE *fs;
	regex_t *rx;
	gchar *tempo;

	rx = gnome_regex_cache_compile(rxc, ".pot$", 0);
	if (!regexec(rx, name, 0, NULL, 0)) {
		gchar *warn;
		warn = g_strdup_printf(_("You are trying to save file with *.pot extension.\nHowever, such files are generated by program.\nYour file should be named with *.po extension, most likely %s.po"), lc);
		gnome_app_warning(GNOME_APP(app1), warn);
		return FALSE;
	}
	/*
	 * Check if the filename is equal to our internally
	 *  used "gtranslator-temp-po-file"..
	 */
	tempo=g_strdup_printf("%s/%s", g_get_home_dir(),
			      "gtranslator-temp-po-file");

	if(!strcmp(name, tempo))
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

	fs = fopen(name, "w");

	/*
	 * Again check if the file exists.
	 */
	if(!g_file_exists(name))
	{
		gchar *my_error=g_strdup_printf(
			_("The file `%s' doesn't exist at all!"), name);
		gnome_app_error(GNOME_APP(app1), my_error);
		
		g_free(my_error);
		return FALSE;
	}

	update_header(po->header);
	header = put_header(po->header);
	write_the_message(header, (gpointer) fs);
	free_a_message(header, NULL);

	gtranslator_update_msg();
	
	/*
	 * Write every message to the file
	 */
	g_list_foreach(po->messages, (GFunc) write_the_message, (gpointer) fs);

	/*
	 * Store the obsolete entries in the file -- if wished and possible.
	 */
	if(wants.keep_obsolete && po->obsolete)
	{
		fprintf(fs, "%s", po->obsolete);
	}
	
	fclose(fs);
	po->file_changed = FALSE;

	/* If user wants to, warn it about fuzzy mesages left */
	if(wants.warn_if_fuzzy && po->fuzzy)
	{
		gchar *warn;
		warn = g_strdup_printf(_("File %s\n"
				       "contains %d fuzzy messages"),
				       po->filename, po->fuzzy);
		gnome_warning_dialog_parented(warn, GTK_WINDOW(app1));
		g_free(warn);
	}
	
	return TRUE;
}

/*
 * A callback for OK in Save as... dialog 
 */
void save_the_file(GtkWidget * widget, gpointer sfa_dlg)
{
	gchar *po_file;
	po_file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(sfa_dlg));
	if (!actual_write(po_file))
		return;
	g_free(po->filename);
	po->filename = g_strdup(po_file);
	gtk_widget_destroy(GTK_WIDGET(sfa_dlg));
}

/*
 * A callback for Save
 */
void save_current_file(GtkWidget * widget, gpointer useless)
{
	if (!po->file_changed) {
		if (wants.dont_save_unchanged_files)
			return;
		if (wants.warn_if_no_change) {
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
			show_nice_dialog(&dialog, _("gtranslator -- unchanged"));
			reply = gnome_dialog_run(GNOME_DIALOG(dialog));
			g_free(question);
			if (reply != GNOME_YES)
				return;
		}
	}

	actual_write(po->filename);

	disable_actions(ACT_SAVE);
}

void free_a_message(gpointer data, gpointer useless)
{
	g_free(GTR_MSG(data)->comment);
	g_free(GTR_MSG(data)->msgid);
	g_free(GTR_MSG(data)->msgstr);
	g_free(data);
}

/*
 * Frees the po variable
 */
void free_po(void)
{
	if(!po)
		return;
	if (po->messages) {
		g_list_foreach(po->messages, free_a_message, NULL);
		g_list_free(po->messages);
	}
	if (po->header)
		free_header(po->header);
	g_free(po->filename);
	g_free(po->obsolete);
	g_free(po);
	po = NULL;
}

void close_file(GtkWidget * widget, gpointer useless)
{
	if (!file_opened)
		return;
	/*
	 * If user doesn't know what to do with changed file, return
	 */
	if (!ask_to_save_file)
		return;
	free_po();

	gtranslator_sidebar_clear_views();
	
	file_opened = FALSE;
	nothing_changes = TRUE;

	/*
	 * Stop gtkspell.
	 */ 
	if(wants.instant_spell_check && gtkspell_running())
	{
		gtkspell_stop();
	}
	
	clean_text_boxes();
	/*
	 * Set a blank appbar status message.
	 */
	gnome_appbar_push(GNOME_APPBAR(appbar1), "");
	gnome_appbar_set_progress(GNOME_APPBAR(appbar1), 0.00000);
	disable_actions_no_file();
}

void revert_file(GtkWidget * widget, gpointer useless)
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
		show_nice_dialog(&dialog, "gtranslator -- revert");
		reply = gnome_dialog_run(GNOME_DIALOG(dialog));
		g_free(question);
		if (reply != GNOME_YES)
			return;
	}
	save_this = g_strdup(po->filename);
	/*
	 * Let close_file know it doesn't matter if file was changed
	 */
	po->file_changed = FALSE;
	close_file(NULL, NULL);
	parse(save_this);
	g_free(save_this);
}

/*
 * The compile function
 */
void compile(GtkWidget * widget, gpointer useless)
{
	gchar *cmd;
	gint res = 1;
	FILE *fs;
	gboolean changed = po->file_changed;

	/*
	 * Check if msgfmt is available on the system.
	 */
	if(!gnome_is_program_in_path("msgfmt"))
	{
		gnome_app_error(GNOME_APP(app1), 
			_("Sorry, msgfmt isn't available on your system!"));
		return;
	}
			
	if (!file_opened) 
		return;
	
#define RESULT "gtr_result.tmp"
#define PO_FILE "gtr_po.tmp"
	actual_write(PO_FILE);
	po->file_changed = changed;
	cmd = g_strdup_printf("msgfmt -v -c -o /dev/null %s >%s 2>&1",
			    PO_FILE,RESULT);
	res = system(cmd);
	fs=fopen(RESULT,"r");
	/*
	 * If there has been an error show an error-box
	 */
	if (res != 0) {
		compile_error_dialog(fs);
	} else {
		gchar line[128];
		fgets(line, 128, fs);
		g_strchomp(line);
		g_snprintf(status, sizeof(status),
		    _("Compile successful:\n%s"), line);
		gnome_app_message(GNOME_APP(app1), status);
	}
	fclose(fs);
	remove(RESULT);
	remove(PO_FILE);
	g_free(cmd);
}

/*
 * Checks the given file for read permissions first and then
 *  for the right write permissions.
 */
gboolean gtranslator_check_file_perms(GtrPo *po_file)
{
	FILE *file;
	gchar *error_message;

	/*
	 * Open the file first for reading.
	 */
	file=fopen(po_file->filename, "r");
	if(po_file && !file)
	{
		/*
		 * Create an error box and prevent further reading
		 *  of the file.
		 */  
		error_message=g_strdup_printf(
			_("You don't have read permissions on file `%s'"),
			po_file->filename);
		gnome_app_error(GNOME_APP(app1), error_message);

		return FALSE;
	}
	else
	{
		/*
		 * Open the same file also for a write-permission check.
		 */ 
		file=fopen(po_file->filename, "r+");
		if(po_file && !file)
		{
			/*
			 * Show a warning box to the user and warn him about
			 *  the fact of lacking write permissions.
			 */  
			error_message=g_strdup_printf(
				_("You don't have write permissions on file `%s'.\n\
This means that you should save it as a copy to a local directory\n\
of your choice."),
				po_file->filename);
			gnome_app_warning(GNOME_APP(app1), error_message);

			po_file->no_write_perms=TRUE;
		
			return TRUE;
		}
		else
		{
			po_file->no_write_perms=FALSE;
		}
	}

	fclose(file);

	return TRUE;
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
	if(message->status & GTR_MSG_STATUS_STICK)
		po->translated++;
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
	gnome_appbar_set_progress(GNOME_APPBAR(appbar1),
				  1.0 * po->translated / po->length);
}

