/*
 * (C) 2000-2004 	Fatih Demir <kabalak@kabalak.net>
 *			Ross Golder <ross@golder.org>
 *			Gediminas Paulauskas <menesis@kabalak.net>
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
#include <error.h>

#include <sys/param.h>

#include "actions.h"
#include "comment.h"
#include "dialogs.h"
#include "gui.h"
#include "history.h"
#include "message.h"
#include "messages-table.h"
#include "nautilus-string.h"
#include "page.h"
#include "prefs.h"
#include "runtime-config.h"
#include "translator.h"
#include "undo.h"
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

/* Error handler stuff */
GQuark gtranslator_parser_error_quark (void)
{
	static GQuark quark = 0;
	if (!quark)
		quark = g_quark_from_static_string ("gtranslator_parser_error");
	return quark;
}

/* Parser dialog components */
GtkWidget *parser_dialog;
GtkTextBuffer *parser_dialog_buffer;
gboolean parser_errors;

void gtranslator_parser_dialog_destroy(GtkWidget *widget);

void gtranslator_parser_dialog_create(void) {
	GtkWidget *view;

	/*
	 * Prepare a graphical tail :) (the notorious GTK+ FAQ)
	 */
	view = gtk_text_view_new ();
	gtk_widget_set_size_request(GTK_WIDGET(view), 320, 200);
	g_object_set(view, "editable", FALSE, NULL);
	parser_dialog_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
	parser_dialog = gtk_dialog_new_with_buttons (_("Parser output"),
		GTK_WINDOW(gtranslator_application),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_STOCK_OK,
		GTK_RESPONSE_NONE,
		NULL);
	g_signal_connect_swapped (parser_dialog, "response",
		G_CALLBACK (gtranslator_parser_dialog_destroy),
		NULL);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(parser_dialog)->vbox), view);
	gtk_widget_show_all (parser_dialog);
}

void gtranslator_parser_dialog_destroy(GtkWidget *widget)
{
	gtk_widget_destroy(parser_dialog);
	parser_dialog = NULL;
}

/*
 * Error handler functions for gettext parser.
 * See gettext-po.h for more info.
 */
void gettext_error(int status, int errnum, const char *format, ...) {
	va_list args;
	GtkTextIter iter;
	char *buf = NULL;

	/* Kick off a parser dialog if one doesn't exist */
	if(parser_dialog == NULL)
		gtranslator_parser_dialog_create();

	/* Obtain post-formatted error message */
	va_start(args, format);
	buf = g_strdup_printf(format, args);
	gtk_text_buffer_get_iter_at_offset (parser_dialog_buffer, &iter, -1);
	gtk_text_buffer_insert (parser_dialog_buffer, &iter, buf, -1);
	g_free(buf);
	va_end(args);

	/* Don't open the file after all */
	parser_errors = TRUE;

	/* According to gettext-po.h, we must do this */
	error_message_count++;
}

void gettext_error_at_line(int status, int errnum, const char *filename,
	unsigned int lineno, const char *format, ...) {
	va_list args;
	char *buf = NULL, *errmsg;

	/* Obtain post-formatted error message */
	va_start(args, format);
	buf = g_strdup_printf(format, args);
	errmsg = g_strdup_printf("%s:%u: %s\n", filename, lineno, buf);
	gettext_error(status, errnum, errmsg);
	g_free(errmsg);
	g_free(buf);
	va_end(args);

	/* Don't open the file after all */
	parser_errors = TRUE;

	/* According to gettext-po.h, we must do this */
	error_message_count++;
}

void gettext_multiline_warning(char *prefix, char *message) {
	char *buf = NULL;
	GtkTextIter iter;

	/* Kick off a parser dialog if one doesn't exist */
	if(parser_dialog == NULL)
		gtranslator_parser_dialog_create();

	/* Obtain post-formatted error message */
	buf = g_strdup_printf("warning (%s): %s\n", prefix, message);
	gtk_text_buffer_get_iter_at_offset (parser_dialog_buffer, &iter, -1);
	gtk_text_buffer_insert (parser_dialog_buffer, &iter, buf, -1);
	g_free(buf);

	/* According to gettext-po.h, we must do this */
	g_free(prefix);
	g_free(message);
}

void gettext_multiline_error(char *prefix, char *message) {
	char *buf = NULL;
	GtkTextIter iter;

	/* Kick off a parser dialog if one doesn't exist */
	if(parser_dialog == NULL)
		gtranslator_parser_dialog_create();

	/* Obtain post-formatted error message */
	buf = g_strdup_printf("error (%s): %s", prefix, message);
	gtk_text_buffer_get_iter_at_offset (parser_dialog_buffer, &iter, -1);
	gtk_text_buffer_insert (parser_dialog_buffer, &iter, buf, -1);
	g_free(buf);
	
	/* Don't open the file after all */
	parser_errors = TRUE;

	/* According to gettext-po.h, we must do this */
	g_free(message);
	if(prefix != NULL) {
		error_message_count++;
		g_free(prefix);
	}
}

struct po_error_handler gettext_error_handler = {
	gettext_error,
	gettext_error_at_line,
	gettext_multiline_warning,
	gettext_multiline_error
};

/*
 * The core parsing function for the given po file.
 */ 
GtrPo *gtranslator_parse(const gchar *filename, GError **error)
{
	GtrPo *po;
	GtrMsg *msg;
	gchar *base;
	int i = 0;
	po_message_iterator_t iter;
	po_message_t message;
	const char * const *domains;
	const char *msgstr;
	
	g_return_val_if_fail(filename!=NULL, NULL);

	base=g_path_get_basename(filename);
	g_return_val_if_fail(base[0]!='\0', NULL);
	g_free(base);

	/*
	 * Start a new PO file record
	 */
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
	 * Open the PO file, using gettext's utility function
	 */
	parser_errors = FALSE;
	po->gettext_po_file = po_file_read(po->filename, &gettext_error_handler);
	if(po->gettext_po_file == NULL) {
		g_set_error(error,
			GTR_PARSER_ERROR,
			GTR_PARSER_ERROR_GETTEXT,
			_("Failed opening file '%s': %s"),
			po->filename, strerror(errno));
		gtranslator_po_free(po);
		return NULL;
	}
	
	/*
	 * If there were errors, abandon this page
	 */
	if(parser_errors) {
		gtranslator_po_free(po);
		return NULL;
	}
	
	/*
	 * Determine the message domains to track
	 */
	if(!(domains = po_file_domains(po->gettext_po_file))) {
		g_set_error(error,
			GTR_PARSER_ERROR,
			GTR_PARSER_ERROR_GETTEXT,
			_("Gettext returned a null message domain list."));
		gtranslator_po_free(po);
		return NULL;
	}
	while(domains[i]) {
		po->domains = g_list_append(po->domains, g_strdup(domains[i]));
		i++;
	}

	/*
	 * Determine whether first message is the header or not, and
	 * if so, process it seperately. Otherwise, treat as a normal
	 * message.
	 */
	po->messages = NULL;
	iter = po_message_iterator(po->gettext_po_file, NULL);
	message = po_next_message(iter);
	msgstr = po_message_msgstr(message);
	if(!strncmp(msgstr, "Project-Id-Version: ", 20)) {
		/* TODO: parse into our header structure */
	}
	else {
		/* Reset our pointer */
		iter = po_message_iterator(po->gettext_po_file, NULL);
	}
		
	/*
	 * Post-process these into a linked list of GtrMsgs.
	 */
	while((message = po_next_message(iter)))
	{
		/* Unpack into a GtrMsg */
		msg = g_new0(GtrMsg, 1);
		msg->message = message;
  
		/* Build up messages */
		po->messages = g_list_append(po->messages, msg);
	}
	if(po->messages == NULL) {
		g_set_error(error,
			GTR_PARSER_ERROR,
			GTR_PARSER_ERROR_OTHER,
			_("No messages obtained from parser."));
		gtranslator_po_free(po);
		return NULL;
	}
	po_message_iterator_free(iter);

	/*
	 * Set the current message to the first message.
	 */
	po->current = g_list_first(po->messages);

	return po;
}

/*
 * The main file opening function. Checks that the file isn't already open,
 * and if not, opens it in a new tab.
 */
gboolean gtranslator_open(const gchar *filename, GError **error)
{
	gchar	*base;
	GList	*pagelist;
	GtrPage	*page;
	GtrPo	*po;
	GtkWidget	*page_label;
	
	/*
	 * Check the file isn't already open
	 */
	pagelist = pages;
	while(pagelist) {
		page = (GtrPage*)pagelist->data;
		if(!strcmp(page->po->filename, filename)) {
			/* Tell user and maybe active that tab */
			g_warning("File '%s' is already open.", page->po->filename);
			gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_widget), page->num);
			return TRUE;
		}
		pagelist = pagelist->next;
	}
	
	/*
	 * If the filename can't be opened, pass the error back to the caller
	 * to handle.
	 */
	if(!(po = gtranslator_parse(filename, error)))
		return FALSE;

	/*
	 * If not a crash/temporary file, add to the history.
	 */
	base = g_path_get_basename(po->filename);
	if(nautilus_strcasecmp(base, gtranslator_runtime_config->temp_filename) || 
	   nautilus_strcasecmp(base, gtranslator_runtime_config->crash_filename))
	{
		const char *header = po_file_domain_header(po->gettext_po_file, NULL);
		if(header) {
			const char *project_id = po_header_field(header, "Project-Id-Version");
			if(project_id) {
				gtranslator_history_add(po->filename, (gchar *)project_id);
			}
			else {
				gtranslator_history_add(po->filename, _("N/A"));
			}
		}
		else {
			gtranslator_history_add(po->filename, _("N/A"));
		}
	}
	g_free(base);

	/*
	 * Create a page to add to our list of open files
	 */
	page = gtranslator_page_new(po);
	pages = g_list_append(pages, (gpointer)page);
	
	/*
	 * Create a notebook page to display it in the GUI
	 */
	page_label = gtk_label_new(po->filename);
	if(GtrPreferences.show_messages_table) {
		page->num = gtk_notebook_append_page(GTK_NOTEBOOK(notebook_widget),
			page->table_pane, page_label);
	}
	else {
		page->num = gtk_notebook_append_page(GTK_NOTEBOOK(notebook_widget),
			page->content_pane, page_label);
	}
	gtk_widget_show_all(notebook_widget);
	
	/*
	 * Switch to that page
	 */
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_widget), page->num);

#ifdef HOLD_ON_A_SEC
	/*
	 * Set window title
	 */
	title=g_strdup_printf(_("gtranslator -- %s"), po->filename);
	gtk_window_set_title(GTK_WINDOW(gtranslator_application), title);
	g_free(title);

	/*
	 * Show the current message.
	 */
	gtranslator_message_show(po->current->data);

	/*
	 * Enable/disable application bar options
	 */
	gtranslator_application_bar_update(0);

	/*
	 * Iterate to the main GUI thread -- well, no locks for the GUI should
	 *  be visible -- avoids "the clean my gtranslator window with dialog"
	 *   party game ,-)
	 */
	while(gtk_events_pending())
	{
		gtk_main_iteration();
	}

#endif

	/*
	 * Update the recent files list.
	 */
	gtranslator_history_show();

	gtranslator_actions_set_up_file_opened();

	gtranslator_update_translated_count(po);
	gtranslator_update_progress_bar();
		
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
	if((g_list_length(po->messages) - po->translated) > 0)
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
	
	/*
	 * Hook up the autosaving function if wished.
	 */
	if(GtrPreferences.autosave)
	{
		po->autosave_timeout = g_timeout_add(
			(GtrPreferences.autosave_timeout * 60000),
			(GSourceFunc) gtranslator_utils_autosave, po);
	}

	return TRUE;
}

void gtranslator_parse_the_file_from_file_dialog(GtkWidget * dialog)
{
	gchar *po_file;
	GError *error;
	po_file = g_strdup(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));

	gtranslator_file_dialogs_store_directory(po_file);

	/*
	 * Open the file via our centralized opening function.
	 */
	if(!gtranslator_open(po_file, &error)) {
		if(error) {
			gnome_app_warning(GNOME_APP(gtranslator_application),
				error->message);
			g_error_free(error);
		}
	}

	/*
	 * Destroy the dialog 
	 */
	gtk_widget_destroy(dialog);
}

gboolean gtranslator_save_file(GtrPo *po, const gchar *name, GError **error)
{
	if(nautilus_istr_has_suffix(name, ".pot"))
	{
		g_set_error(error,
			GTR_PARSER_ERROR,
			GTR_PARSER_ERROR_FILENAME,
			_("You are saving a file with a .pot extension.\n\
Pot files are generated by the compilation process.\n\
Your file should likely be named '%s.po'."), 
			gtranslator_translator->language->locale);
		return FALSE;
	}
	
	gtranslator_file_dialogs_store_directory(name);

	/*
	 * Let gettext do the work here
	 */
	if(!po_file_write(po->gettext_po_file, name, &gettext_error_handler))
		return FALSE;

	/*
	 * If we do append a suffix to the autogenerated files, then we'd
	 *  also not disable the file saving entry and we don't also disable
	 *   the save entry on simple compile testing -- check for filename
	 *    equality.
	 */
	if((GtrPreferences.autosave && GtrPreferences.autosave_with_suffix &&
	    	(po->autosave_timeout > 1)) ||
	    !nautilus_strcmp(po->filename, name))
	{
		gtranslator_actions_disable(ACT_SAVE);
	}
	
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
			ngettext("File %s\ncontains %d fuzzy message", "File %s\ncontains %d fuzzy messages", po->fuzzy),
			po->filename, po->fuzzy);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
	}

	return TRUE;
}

/*
 * A callback for OK in Save as... dialog 
 */
void gtranslator_save_file_dialog(GtkWidget *dialog)
{
	gchar *po_file;
	GError *error = NULL;
	po_file = g_strdup(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
	if (!gtranslator_save_file(current_page->po, po_file, &error)) {
		GtkWidget *dialog;
		g_assert(error != NULL);
		dialog = gtk_message_dialog_new(
			GTK_WINDOW(gtranslator_application),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_WARNING,
			GTK_BUTTONS_OK,
			error->message);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		g_clear_error(&error);
		return;
	}
	g_free(current_page->po->filename);
	current_page->po->filename = g_strdup(po_file);
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

/*
 * A callback for Save
 */
void gtranslator_save_current_file_dialog(GtkWidget * widget, gpointer useless)
{
	GError *error;
	
	g_return_if_fail(current_page->po->file_changed);

	if (!gtranslator_save_file(current_page->po, current_page->po->filename, &error)) {
		GtkWidget *dialog;
		g_assert(error != NULL);
		dialog = gtk_message_dialog_new(
			GTK_WINDOW(gtranslator_application),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_WARNING,
			GTK_BUTTONS_OK,
			error->message);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		g_clear_error(&error);
		return;
	}
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
	
	g_free(po->filename);
	g_free(po->obsolete);
	g_free(po);
}

void gtranslator_file_close(GtkWidget * widget, gpointer useless)
{
	GtrPo *po;
	
	g_assert(current_page != NULL);

	/*
	 * If user doesn't know what to do with changed file, return
	 */
	if (!gtranslator_should_the_file_be_saved_dialog(current_page))
		return;
	
	gtranslator_po_free(po);
	gtranslator_undo_clean_register();
	
	nothing_changes = TRUE;

	gtranslator_actions_set_up_state_no_file();

	/*
	 * Set blank status, progress and window title
	 */
	gtk_window_set_title(GTK_WINDOW(gtranslator_application), _(PACKAGE_NAME));
	gnome_appbar_clear_stack(GNOME_APPBAR(gtranslator_application_bar));
	gnome_appbar_set_progress_percentage(GNOME_APPBAR(gtranslator_application_bar), 0.00000);

	/*
	 * Remove the source tag and set the source tag to '1'.
	 */
	if(po->autosave_timeout > 1)
	{
		g_source_remove(po->autosave_timeout);
		po->autosave_timeout = 1;
	}

	/*
	 * "Remove" the stored "runtime/filename" key.
	 */
	gtranslator_config_set_string("runtime/filename", "--- No file ---");

	nothing_changes = FALSE;
}

void gtranslator_file_revert(GtkWidget * widget, gpointer useless)
{
	GError *error;
	
	g_assert(current_page != NULL);
	
	if (current_page->po->file_changed) {
		guint reply;
		reply = gtranslator_file_revert_dialog(NULL, current_page->po->filename);
		if (reply != GTK_RESPONSE_YES)
			return;
	}

	/*
	 * Close the file (ignore changes)
	 */
	current_page->po->file_changed = FALSE;
	gtranslator_file_close(NULL, NULL);
	
	/*
	 * Re-open the file
	 */
	if(!gtranslator_open(current_page->po->filename, &error))
	{
		if(error) {
			gnome_app_warning(GNOME_APP(gtranslator_application),
				error->message);
			g_error_free(error);
		}
		return;
	}
}

/*
 * Remove all translations from the current po file.
 */
void gtranslator_remove_all_translations(GtrPo *po)
{
	gboolean	prev_file_changed=po->file_changed;
	
	g_return_if_fail(po!=NULL);
	g_return_if_fail(po->messages!=NULL);

	/*
	 * Remove the translations, update the statistics.
	 */
	g_list_foreach(po->messages, (GFunc) gtranslator_message_clear_translation, NULL);
	gtranslator_update_translated_count(po);
	gtranslator_update_progress_bar();
	
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

	if((g_list_length(po->messages) - po->translated) > 0)
	{
		gtranslator_actions_enable(ACT_NEXT_UNTRANSLATED);
	}
}

/*
 * A helper function simply increments the "translated" variable of the
 *  po-file.
 */
static void determine_translation_status(gpointer data, gpointer useless_stuff)
{
	GtrMsg *message = GTR_MSG(data);
	if(message->status & GTR_MSG_STATUS_TRANSLATED)
		current_page->po->translated++;
	if(message->status & GTR_MSG_STATUS_FUZZY)
		current_page->po->fuzzy++;
}

/*
 * Update the count of the completed translated entries.
 */
void gtranslator_update_translated_count(GtrPo *po)
{
	po->translated = 0;
	po->fuzzy = 0;
	g_list_foreach(po->messages, (GFunc) determine_translation_status,
			NULL);
}
