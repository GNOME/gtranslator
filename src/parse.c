/*
 * (C) 2000 	Fatih Demir <kabalak@gmx.net>
 *		Gediminas Paulauskas <menesis@delfi.lt>
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
#include <sys/param.h>
#include "parse.h"
#include "prefs.h"
#include "dialogs.h"
#include "gui.h"
#include "open-differently.h"
#include <libgnome/gnome-history.h>
#include <libgtranslator/recent-files.h>

/*
 * These are to be used only inside this file
 */
static void append_line(gchar * * old, const gchar  * tail);
static gchar *restore_msg(gchar  * given);
static void write_the_message(gpointer data, gpointer fs);
static gboolean actual_write(const gchar  * name);
static void free_po(void);
static void free_a_message(gpointer data, gpointer useless);
static void determine_translation_status(gpointer data, gpointer useless_stuff);

void mark_msg_fuzzy(GtrMsg  * msg, gboolean fuzzy)
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

/* 
 * FIXME: check if message counts remain correct
 */
void mark_msg_sticky (GtrMsg  * msg, gboolean on)
{
	if (on) {
		msg->msgstr = msg->msgid;
		/*
		 * It is no longer fuzzy
		 */
		mark_msg_fuzzy(msg, FALSE);
		msg->status |= GTR_MSG_STATUS_STICK;
	} else {
		g_free(msg->msgstr);
		msg->msgstr = NULL;
		msg->status &= ~GTR_MSG_STATUS_STICK;
	}
}

static void check_msg_status(GtrMsg  * msg)
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
static void append_line(gchar * * old, const gchar  * tail)
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
		result = g_strdup(to_add);
	else
		result = g_strconcat(*old, to_add, NULL);
	g_free(to_add);
	g_free(*old);
	*old = result;
}

static gboolean actual_parse(void)
{
	FILE *fs;
	gchar line[256];
	guint lines = 0;
	/*
	 * if TRUE, means that a corresponding part is read
	 */
	gboolean msgid_ok = FALSE, msgstr_ok = FALSE, comment_ok = FALSE;
	GtrMsg *msg;
	gchar *error = NULL;

	fs = fopen(po->filename, "r+");
	
	/*
	 * Check if the file exists at all with a libgnome-function.
	 */
	if(!g_file_exists(po->filename))
	{
		error=g_strdup_printf(_("The file `%s' doesn't exist at all!"), po->filename);
		gnome_app_error(GNOME_APP(app1), error);
		if(error)
		{
			g_free(error);
		}
		return FALSE;
	}

	/*
	 * Sorry, but here we can really use the "simple" equivalence check as
	 *  the two strings should be the same -- really.
	 */ 
	if(po->filename==g_strdup_printf("%s/%s",
		g_get_home_dir(), 
		"gtranslator-temp-po-file"))
	{
		/*
		 * Add a GNOME history entry.
		 */
		gnome_history_recently_used(po->filename, "application/x-po", "gtranslator", "Gettext po-file");
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
				tmp = g_strconcat(msg->comment, line, NULL);
				g_free(msg->comment);
				msg->comment = tmp;
			}
		} else {
			/*
			 * get rid of end-of-lines...
			 */
			g_strchomp(line);
			if (strlen(line) == 0) {
				msgstr_ok = TRUE;
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
			} else
			/*
			 * If it's a msgstr. 
			 */
			if (!g_strncasecmp(line, "msgstr \"", 8)) {
				/*
				 * This means the msgid is completed
				 */
				msgid_ok = TRUE;
				if (strlen(line) - 9 > 0)
					append_line(&msg->msgstr, &line[7]);
			} else
			/*
			 * A continuing msgid or msgstr
			 */
			if (line[0] == '"') {
				if ((comment_ok == TRUE)
				    && (msgid_ok == FALSE))
					append_line(&msg->msgid, line);
				else if ((msgid_ok == TRUE)
					 && (msgstr_ok == FALSE))
					append_line(&msg->msgstr, line);
				else
					/*g_assert_not_reached();*/
					goto ERRR;
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
	if ((msgid_ok == TRUE) && (msgstr_ok == FALSE)) {
		check_msg_status(msg);
		po->messages = g_list_prepend(po->messages, (gpointer) msg);
	} else
		/*
		 * not needed allocated structure
		 */
		g_free(msg);
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
	if (!filename) {
		g_warning(_("There's no file to open!"));
		return;
	} else {
		gchar *base = g_basename(filename);

		if (base[0] == '\0') {
			g_warning(_("There's no file to open!"));
			return;
		}
	}

	po = g_new0(GtrPo, 1);
	/*
	 * Get absolute filename.
	 */
	if (!g_path_is_absolute(filename)) {
		char absol[MAXPATHLEN + 1];
		realpath(filename, absol);
		po->filename = g_strdup(absol);
	} else
		po->filename = g_strdup(filename);

	if (!actual_parse()) {
		free_po();
		return;
	}

	/*
	 * Disable the special navigation buttons now.
	 */
	disable_actions(ACT_NEXT_FUZZY, ACT_NEXT_UNTRANSLATED);

	/*
	 * If the first message is header (it always should be)
	 */
	po->header = get_header(GTR_MSG(po->messages->data));
	if (po->header) {
		GList *header_li;
		/*
		 * Unlink it from messages list
		 */
		header_li = po->messages;
		po->messages = g_list_remove_link(po->messages, header_li);
		free_a_message(header_li->data, NULL);
		g_list_free_1(header_li);
	} else
		g_warning(_("The file has no header!"));
	
	g_snprintf(status, sizeof(status),
		   _("Successfully parsed the file \"%s\""), po->filename);
	gnome_appbar_set_status(GNOME_APPBAR(appbar1), status);
	g_snprintf(status, sizeof(status),
		   _("gtranslator -- %s"), po->filename);
	gtk_window_set_title(GTK_WINDOW(app1), status);

	file_opened = TRUE;
	po->file_changed = FALSE;
	po->length = g_list_length(po->messages);
	/*
	 * Set the current message to the first message, and show it 
	 */
	po->current = g_list_first(po->messages);
	display_msg(po->current);
	get_translated_count();
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
	gtranslator_display_recent();
}

void parse_the_file(GtkWidget  * widget, gpointer of_dlg)
{
	gchar *po_file;
	po_file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(of_dlg));

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

void parse_the_file_from_the_recent_files_list(GtkWidget *widget, gpointer filepointer)
{
	/*
	 * Also detect the right open function in the recent files' list.
	 */
	if(!gtranslator_open_po_file((gchar *) filepointer))
	{
		parse((gchar *) filepointer);
	}
}

/*
 * Restores the formatting of a message, done in append_line
 */
static gchar *restore_msg(gchar  * given)
{
	GString *rest;
	gchar *result;
	gint s, lines = 0, here = 8;

	if(!given)
	{
		return "";
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
	result = g_strdup(rest->str);
	g_string_free(rest, TRUE);
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
}

static gboolean actual_write(const gchar  * name)
{
	GtrMsg *header;
	FILE *fs;
	regex_t *rx;

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
	if(!g_strcasecmp(name,
		g_strdup_printf("%s/%s",
			g_get_home_dir(),
			"gtranslator-temp-po-file")))
	{
		/*
		 * Create a new filename to use instead of the
		 *  oldsome "gtranslator-temp-po-file"...
		 */  
		gchar *newfilename;
		newfilename=g_strdup_printf("%s-%s.%s.po",
			po->header->prj_name,
			po->header->prj_version,
			po->header->language);
		/*
		 * Assign the new name.
		 */ 
		name=newfilename;
		/*
		 * Add a foo'sh header entry.
		 */ 
		po->header->comment="#   -- edited with gtranslator.\n";
		/*
		 * Delete the old file.
		 */
		unlink(g_strdup_printf("%s/%s", g_get_home_dir(), "gtranslator-temp-po-file"));
	}
	
	fs = fopen(name, "w");

	/*
	 * Again check if the file exists.
	 */
	if(!g_file_exists(name))
	{
		gchar *my_error=g_strdup_printf(_("The file `%s' doesn't exist at all!"),name);
		gnome_app_error(GNOME_APP(app1),my_error);
		g_free(my_error);
		return FALSE;
	}

	update_header(po->header);
	header = put_header(po->header);
	write_the_message(header, (gpointer) fs);
	free_a_message(header, NULL);

	update_msg();
	/*
	 * Write every message to the file
	 */
	g_list_foreach(po->messages, (GFunc) write_the_message, (gpointer) fs);

	fclose(fs);
	po->file_changed = FALSE;
	return TRUE;
}

/*
 * A callback for OK in Save as... dialog 
 */
void save_the_file(GtkWidget  * widget, gpointer sfa_dlg)
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
void save_current_file(GtkWidget  * widget, gpointer useless)
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

static void free_a_message(gpointer data, gpointer useless)
{
	g_free(GTR_MSG(data)->comment);
	g_free(GTR_MSG(data)->msgid);
	g_free(GTR_MSG(data)->msgstr);
	g_free(data);
}

/*
 * Frees the po variable
 */
static void free_po(void)
{
	if (po->messages) {
		g_list_foreach(po->messages, free_a_message, NULL);
		g_list_free(po->messages);
	}
	if (po->header)
		free_header(po->header);
	g_free(po->filename);
	g_free(po);
	po = NULL;
}

void close_file(GtkWidget  * widget, gpointer useless)
{
	if (!file_opened)
		return;
	/*
	 * If user doesn't know what to do with changed file, return
	 */
	if (!ask_to_save_file)
		return;
	free_po();
	file_opened = FALSE;
	nothing_changes = TRUE;
	clean_text_boxes();
	/*
	 * Set a blank appbar status message.
	 */
	gnome_appbar_push(GNOME_APPBAR(appbar1), "");
	gnome_appbar_set_progress(GNOME_APPBAR(appbar1), 0.00000);
	disable_actions_no_file();
}

void revert_file(GtkWidget  * widget, gpointer useless)
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
void compile(GtkWidget  * widget, gpointer useless)
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
		gnome_app_error(GNOME_APP(app1), _("Sorry, msgfmt isn't available on your system!"));
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
		    _("Compile successfull:\n%s"), line);
		gnome_app_message(GNOME_APP(app1), status);
	}
	fclose(fs);
	remove(RESULT);
	remove(PO_FILE);
	g_free(cmd);
}

/*
 * The recent menus stuff.
 */
void gtranslator_display_recent(void)
{
	/*
	 * Couldn't we do that better with bonobo?
	 */
	gchar *name;
	gchar *menupath = g_strdup (_("_File/Recen_t files/"));
	gint len;
	GnomeUIInfo *menu;
	GnomeHistoryEntry recent;
	GList *list;
	
	/*
	 * Get GNOME's list of the recently used files.
	 */
	list=gnome_history_get_recently_used();
	/*
	 * reverse the list to get the real sequence on
	 *  the routines while parsing the list of recent
	 *   files.
	 */
	list=g_list_reverse(list);
	/*
	 * Are there any recent files ?
	 */
	if((!list) || (!list->data))
	{
		/*
		 * Delete the old entries.
		 */
		gnome_app_remove_menus(GNOME_APP(app1), menupath, 1);
		/*
		 * Create a new GnomeUIInfo widget.
		 */
		menu=g_new0(GnomeUIInfo,2);
		/*
		 * Insert the end point of the menus.
		 */
		menu->type=GNOME_APP_UI_ENDOFINFO;
		/*
		 * Insert this menu into the menupath.
		 */
		gnome_app_insert_menus(GNOME_APP(app1), menupath, menu);
		/*
		 * Return from the loop.
		 */
		return;
	}

	/*
	 * Delete the old entries.
	 */
	gnome_app_remove_menus(GNOME_APP(app1), menupath, 1);

	/*
	 * Create a new GnomeUIInfo widget.
	 */
	menu=g_new0(GnomeUIInfo,2);

	/*
	 * Insert the end point of the menus.
	 */
	menu->type=GNOME_APP_UI_ENDOFINFO;
	/*
	 * Insert this menu into the menupath.
	 */
	gnome_app_insert_menus(GNOME_APP(app1), menupath, menu);
	
	/*
	 * Parse the list, but maximal as many entries as wished
	 *  in the preferences.
	 */
	for(len=((g_list_length(list)-1) < (wants.recent_files-1))
		?(g_list_length(list)-1) : (wants.recent_files-1);
		len >= 0;len--)
	{
		/*
		 * Get the GnomeHistory Entry.
		 */
		recent=g_list_nth_data(list, len);
		
		/*
		 * Copy the filename.
		 */
		name=g_strdup(recent->filename);
		
		/*
		 * If the filename should be checked for existence.
		 */
		if(wants.check_recent_file)
		{
			if(!g_file_exists(name))
			{
				continue;
			}
		}
		
		/*
		 * Set the label name.
		 */
		menu->label=g_strdup_printf("_%i:  %s", len+1,
			recent->filename);
		
		/*
		 * Set the GnomeUIInfo settings and labels.
		 */
		menu->type=GNOME_APP_UI_ITEM;
		menu->hint=g_strdup_printf(_("Open %s"), recent->filename);
		menu->moreinfo=(gpointer)parse_the_file_from_the_recent_files_list;
		menu->user_data=name;
		menu->unused_data=NULL;
		menu->pixmap_type=0;
		menu->pixmap_info=NULL;
		menu->accelerator_key=0;
		
		/*
		 * Insert a GNOMEUIINFO_END equivalent.
		 */
		(menu+1)->type=GNOME_APP_UI_ENDOFINFO;

		/*
		 * Insert it into the menus.
		 */
		gnome_app_insert_menus(GNOME_APP(app1), menupath, menu);
		
		g_free(menu->label);
	}
	/*
	 * Free the string and the GnomeUIInfo structure.
	 */
	if(menu)
	{
		g_free(menu);
	}
	g_free(menupath);
	/*
	 * At last: free the GnomeHistoryEntry list.
	 */
	gnome_history_free_recently_used_list(list);
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
void get_translated_count(void)
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
	 * Get the total percentage.
	 */
	percentage = 1.0  * po->translated / po->length;
	/*
	 * Set the progressbar status.
	 */
	gnome_appbar_set_progress(GNOME_APPBAR(appbar1), percentage);
}
