/*
 * (C) 2000-2007 	Fatih Demir <kabalak@kabalak.net>
 *			Ross Golder <ross@golder.org>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Peeter Vois <peeter@kabalak.net>
 *			Thomas Ziehmer <thomas@kabalak.net>
 *			Ignacio Casal <nacho.resa@gmail.com>
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

#include "about.h"
#include "actions.h"
#include "bookmark.h"
#include "dialogs.h"
#include "dnd.h"
#include "find.h"
#include "header_stuff.h"
#include "history.h"
#include "learn.h"
#include "gucharmap.h"
#include "menus.h"
#include "message.h"
#include "messages-table.h"
#include "nautilus-string.h"
#include "page.h"
#include "parse.h"
#include "prefs.h"
#include "runtime-config.h"
#include "stylistics.h"
#include "translator.h"
#include "undo.h"
#include "utils.h"
#include "utils_gui.h"

#include <string.h>

#include <gdk/gdkkeysyms.h>

#include <gtk/gtk.h>
#include <glade/glade.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <libgnomevfs/gnome-vfs-init.h>

/*
 * Glade file path
 */
#define GLADE_PATH "../data/glade/main_window.glade"

/*
 * Glade widgets names
 */
#define GLADE_MAIN_WINDOW "main_window"
//#define GLADE_MAIN_NOTEBOOK "main_notebook"
#define GLADE_PROGRESS_BAR "progress_bar"
#define GLADE_STATUS_BAR "status_bar"
#define GLADE_TOOLBAR "toolbar"
#define GLADE_WARNING_LABEL "warning_label"
#define GLADE_WARNING_BUTTON "warning_button"
#define GLADE_WARNING_HBOX "warning_hbox"
#define GLADE_WARNING_EVENTBOX "warning_eventbox"


/*
 * Global external variables
 */
GtkWidget *gtranslator_application;
GtkWidget *gtranslator_status_bar;
GtkWidget *gtranslator_progress_bar;
GtkWidget *gtranslator_toolbar;


GladeXML *glade;

guint trans_box_insert_text_signal_id;
guint trans_box_delete_text_signal_id;

/*
 * Warning widget
 */
static GtkWidget *warning_hbox;

/*
 * The widgets related to displaying the current document 
 */
gboolean nothing_changes;

static GtkTextBuffer* get_selection_buffer(GtkTextBuffer *buffer);

#ifdef REDUNDANT
/*
 * Internal text callbacks/handlers.
 */
static void insert_text_handler (GtkTextBuffer *editable, GtkTextIter *pos,
				 const gchar *text,
				 gint length, gpointer data);

static void delete_text_handler(GtkTextBuffer *textbuf, GtkTextIter *start,
				GtkTextIter *end);

static void selection_get_handler(GtkWidget *widget,
	GtkSelectionData *selection_data, guint info,
	guint time_stamp, gpointer data);
#endif /* REDUNDANT */

/*
 * Defined later
 */
void gtranslator_gui_switch_page(GtkNotebook *notebook, GtkNotebookPage *notebook_page, guint pagenum, gpointer user_data);

/*
 * The target formats
 */
static  GtkTargetEntry dragtypes[] = {
	{ "text/uri-list", 0, TARGET_URI_LIST },
	{ "text/plain", 0, TARGET_NETSCAPE_URL },
	{ "text/plain", 0, TARGET_TEXT_PLAIN }
};

/*
 * Function to pack/re-pack the main window with/without a messages table
 */
void gtranslator_repack_window(void);

/*
 * Pane positions storage variables.
 */
gint table_pane_position;

/*
 * To get warning_hbox widget
 */
GtkWidget *
gtranslator_gui_get_warning_hbox()
{
   	return warning_hbox;
}

/*
 * Creates the main gtranslator application window.
 */
void 
gtranslator_create_main_window(void)
{
	gchar *path_glade;
	
	/*
	 * Initialize glade library, load the interface
	 */
	//path_glade = g_strconcat(DATADIR, GLADE_PATH, NULL);
	glade = glade_xml_new(GLADE_PATH, NULL, NULL);
    	
	/*
	* Initialize menus
	*/
	gtranslator_menuitems_set_up();
	connect_menu_signals();
	
	/*
	 * Create the app	
	 */
	gtranslator_application = glade_xml_get_widget(glade, GLADE_MAIN_WINDOW);
	
	/*
	 * Set up the menus
	 */
	//gnome_app_create_menus(GNOME_APP(gtranslator_application), the_menus);
	
	/*
	 * Start with no document view
	 */
	current_page = NULL;

	/*
	 * Set up the application bar
	 */
	gtranslator_status_bar = glade_xml_get_widget(glade, GLADE_STATUS_BAR);
	gtranslator_progress_bar = glade_xml_get_widget(glade, GLADE_PROGRESS_BAR);
	gtranslator_toolbar = glade_xml_get_widget(glade, GLADE_TOOLBAR);

    
    	/*
    	 * Warning message widgets
    	 */
    	warning_hbox = glade_xml_get_widget(glade, GLADE_WARNING_HBOX);
    	
    
    	/*
    	 * Gucharmap set up
    	 */
    	gtranslator_gucharmap_new();
	
	/*
	 * Make menu hints display on the appbar
	 */
	//gnome_app_install_menu_hints(GNOME_APP(gtranslator_application), the_menus);

	/*
	 * Enable the default menubar/toolbar options
	 */
	//gtranslator_actions_set_up_default();

	/*
	 * Populate the 'Files/Recent' submenu
	 */
	//gtranslator_history_show();

	/*
	 * Hook up the 'close window' callback.
	 */
	g_signal_connect(G_OBJECT(gtranslator_application),
			"delete_event",
			 G_CALLBACK(gtranslator_application_delete_event_cb),
			 NULL);

	/*
	 * The D'n'D signals
	 */
	gtk_drag_dest_set(GTK_WIDGET(gtranslator_application),
			  GTK_DEST_DEFAULT_ALL | GTK_DEST_DEFAULT_HIGHLIGHT,
			  dragtypes, sizeof(dragtypes) / sizeof(dragtypes[0]),
			  GDK_ACTION_COPY);

	g_signal_connect(G_OBJECT(gtranslator_application), "drag-data-received",
			 G_CALLBACK(gtranslator_dnd),
			 GUINT_TO_POINTER(dnd_type));

	
	/*
	 * Resize the window accordingly
	 */
	gtranslator_utils_restore_geometry(NULL);

	/*
	 * Set most options inactive
	 */
	gtranslator_actions_set_up_state_no_file();
	
	/*
	 * Finally, put everything on display
	 */
	gtk_widget_show_all(gtranslator_application);
}

#ifdef REDUNDANT
/*
 * An own delete text handler which should work on deletion in the translation
 *  box -- undo is called up here, too.
 */
void 
delete_text_handler(GtkTextBuffer *textbuf, GtkTextIter *start,
			 GtkTextIter *end)
{
	return;
	/*
	 * Check for dumb values and do only catch real deletions.
	 */
	if(gtk_text_iter_compare(start, end)>=0)
	{
		gchar	*fake_string;

		/*
		 * Fill up our fake_string for the strlen() call in undo.c.
		 */
		fake_string=gtk_text_buffer_get_text(textbuf, 
						     start, end, FALSE);
		
//		gtranslator_undo_register_deletion(fake_string, start, end);
		g_free(fake_string);
	}
	gtk_text_buffer_delete(textbuf, start, end);
	g_signal_stop_emission_by_name (G_OBJECT (textbuf), "delete-text");
}
#endif /* REDUNDANT */

/*
 * Callback called when the user closes the main window
 *
 * This callback is connected to the delete-event signal on the
 * gtranslator GnomeApp (ie the main window) and should be called when the user
 * closes the main window.
 * It returns true to stop other handlers from being invoked for the event.
 */
gboolean 
gtranslator_application_delete_event_cb(GtkWidget  * widget,
						 GdkEvent  * event,
						 gpointer user_data)
{
	gtranslator_quit();
	return TRUE;
}

/*
 * Callback called when the user uses the quit command (^Q or Quit in the menu)
 */
void 
gtranslator_menu_quit_cb(void  * data)
{
	gtranslator_quit();
}

/*
 * The own quit-code
 */
void 
gtranslator_quit()
{
	GList *pagelist;
	GtrPage *page;
	
	/*
	 * Iterate the open files for files with changes
	 */
/*	pagelist = pages;
	while(pagelist && pagelist->data) {
		page = (GtrPage*)pagelist->data;
		if(page->po->file_changed) {
			if(!gtranslator_should_the_file_be_saved_dialog(page)) {
				return;
			}
		}
		pagelist = g_list_remove(pagelist, page);
	}*/

	if(GtrPreferences.show_messages_table && (current_page != NULL))
	{
		/*
		 * Get the EPaned's position offset.
		 */
		table_pane_position=gtk_paned_get_position(GTK_PANED(current_page->table_pane));
		/*
		 * Store the pane position in the preferences.
		 */
		gtranslator_config_set_int("interface/table_pane_position", table_pane_position);
	}
	
	gtranslator_utils_save_geometry();

	/*
	 * "Flush" our runtime config string for the current filename.
	 */
	gtranslator_config_set_string("runtime/filename", "--- No file ---");

	/*
	 * Shutdown our internal learning system.
	 */
	if(gtranslator_learn_initialized())
	{
		gtranslator_learn_shutdown();
	}

	/*
	 * Now save the bookmarks' list before closing/exiting the application.
	 */
	gtranslator_bookmark_save_list();

	/*
	 * Free the used GtrTranslator structure.
	 */
	gtranslator_translator_free(gtranslator_translator);

	/*
	 * Free any lungering stuff 'round -- free prefs.
	 */
	gtranslator_preferences_free();

	/*
	 * Remove any lungering temp. file.
	 */
	gtranslator_utils_remove_temp_files();

	/*
	 * Free our used runtime config structure.
	 */
	gtranslator_runtime_config_free(gtranslator_runtime_config);
	
	/*
	 * Store the current date.
	 */
	gtranslator_config_set_last_run_date();

	/*
	 * Free our used content area variable on exit .-)
	 */
	g_free(current_page);

	/*
	 * Shutdown the eventually (non-)initialized stuff from GnomeVFS.
	 */
	if(gnome_vfs_initialized())
	{
		gnome_vfs_shutdown();
	}

	/*
	 * Quit with the normal Gtk+ quit.
	 */
	gtk_main_quit();
	return;
}


void 
gtranslator_application_bar_update(gint pos)
{
	gchar 	*str, *status;
	GtrMsg 	*msg;
	GtrPo	*po;
	const char *msgstr, *msgid_plural;
	guint context_id;
    
	gtk_statusbar_pop(GTK_STATUSBAR(gtranslator_status_bar), context_id);

	/*
	 * Get the po file and message.
	 */
	po = current_page->po;
	msg = GTR_MSG(po->current->data);
	msgstr = po_message_msgstr(msg->message);
	msgid_plural = po_message_msgid_plural(msg->message);

	/*
	 * And append according to the message status the status name.
	 */
	if(msg->is_fuzzy)
	{
		if(po->fuzzy>0)
		{
			status = g_strdup_printf(ngettext("%s [ %i Fuzzy left ]", "%s [ %i Fuzzy left ]", po->fuzzy), _("Fuzzy"), po->fuzzy);
		}	
		else
		{
			status = g_strdup_printf(_("%s [ No fuzzy left ]"), _("Fuzzy"));
			
			/*
			 * Also disable the corresponding button.
			 */
			gtk_widget_set_sensitive(gtranslator_menuitems->next_fuzzy, FALSE);
		}

	} else if(msgstr[0] != '\0') {
		status=g_strdup(_("Translated"));
	} else {
		/*
		 * Message is untranslated 
		 */
		gint missya = g_list_length(po->messages) - po->translated;
		if (missya)
		{
			status=g_strdup_printf(ngettext("%s [ %i Untranslated left ]", "%s [ %i Untranslated left ]", missya), _("Untranslated"), missya);
		} else {
			status=g_strdup_printf(_("%s [ No untranslated left ]"), _("Untranslated"));
			
			/*
			 * Also disable the coressponding buttons for the
			 *  next untranslated message/accomplish function.
			 */
			gtk_widget_set_sensitive(gtranslator_menuitems->next_untranslated, FALSE);
			gtk_widget_set_sensitive(gtranslator_menuitems->autotranslate, FALSE);
		}	
	}
	
	/*
	 * Assign the toolbar message string according to the message quality.
	 *
	 * If it's a plural forms message we do have got a more enhanced and
	 *  longer message there indicating a plural forms message.
	 *
	 */
	if(msgid_plural)
	{
		str=g_strdup_printf(_("Message %d / %d / Status: %s (Message contains plural forms)"), pos + 1, g_list_length(po->messages), status);
	}
	else
	{
		str=g_strdup_printf(_("Message %d / %d / Status: %s"), pos + 1, g_list_length(po->messages), status);
	}
	
	/*
	 * Set the statusbar text.
	 */
	gtk_statusbar_push(GTK_STATUSBAR(gtranslator_status_bar), context_id, str);
	
	/*
	 * Update the progressbar.
	 */
	gtranslator_update_progress_bar();

	/*
	 * And free the allocated string.
	 */
	g_free(str);

	g_free(status);
}

/*
 * Returns a buffer containing the selection contents having dots
 * properly replaced with spaces.
 */
static GtkTextBuffer* 
get_selection_buffer(GtkTextBuffer *buffer)
{
	gchar* result = NULL;
	GtkTextIter start_iter;
	GtkTextIter end_iter;
	static GtkTextBuffer* selection_buffer = NULL;
	
	if (!GtrPreferences.dot_char) {
		return buffer;
	}

	gtk_text_buffer_get_selection_bounds(buffer, &start_iter, &end_iter);
	
	if (!gtk_text_iter_equal(&start_iter, &end_iter)) {	
		if (selection_buffer != NULL) {		
			g_free(selection_buffer);
		}
    
		selection_buffer = gtk_text_buffer_new(gtk_text_buffer_get_tag_table(buffer));

		result = gtk_text_buffer_get_text(buffer, &start_iter, &end_iter, FALSE);
		result = gtranslator_utils_invert_dot(result);

		gtk_text_buffer_set_text(selection_buffer, result, strlen(result));

		gtk_text_buffer_get_bounds(selection_buffer, &start_iter, &end_iter);
		gtk_text_buffer_place_cursor(selection_buffer, &end_iter);
		gtk_text_buffer_move_mark_by_name(selection_buffer, "selection_bound", &start_iter);
	}

	return selection_buffer;
}

/*
 * The text oriented callbacks
 */
void 
gtranslator_clipboard_cut(GtkWidget  * widget, gpointer useless)
{
	GtkTextBuffer* buffer = NULL;
	GtkTextBuffer* selection_buffer = NULL;
	GtkTextIter start_iter;
	GtkTextIter end_iter;

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
	g_return_if_fail(buffer != NULL);

	/* Copy text after replacing dots with spaces */
	selection_buffer = get_selection_buffer(buffer);
	gtk_text_buffer_copy_clipboard(selection_buffer, gtk_clipboard_get(GDK_NONE));
	
	/* Delete the selected text */
	gtk_text_buffer_get_selection_bounds(buffer, &start_iter, &end_iter);
	gtk_text_buffer_delete_interactive(buffer, &start_iter, &end_iter, TRUE);
	
	gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(widget),
		gtk_text_buffer_get_mark(buffer, "insert"));
}

void 
gtranslator_clipboard_copy(GtkWidget  * widget, gpointer useless)
{
	GtkTextBuffer* buffer = NULL;
	GtkTextBuffer* selection_buffer = NULL;
	
	g_return_if_fail(widget != NULL);
	
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
	g_return_if_fail(buffer != NULL);

	/* Copy text after replacing dots with spaces */
	selection_buffer = get_selection_buffer(buffer);
	gtk_text_buffer_copy_clipboard(selection_buffer, gtk_clipboard_get(GDK_NONE));
  	
	gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(widget),
		gtk_text_buffer_get_mark(buffer, "insert"));
}

void 
gtranslator_clipboard_paste(GtkWidget  * widget, gpointer useless)
{
	GtkTextBuffer* buffer = NULL;

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
	g_return_if_fail (buffer != NULL);


	gtk_text_buffer_paste_clipboard(buffer,
					gtk_clipboard_get(GDK_NONE),
					NULL, TRUE);
  	
	gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(widget),
		gtk_text_buffer_get_mark(buffer, "insert"));
}

void
gtranslator_selection_set(GtkTextView *text_view, gint start, gint end)
{
	GtkTextBuffer *buffer;
	GtkTextIter start_iter;
	GtkTextIter end_iter;

	g_return_if_fail(text_view != NULL);
	g_return_if_fail(start >= 0);
	g_return_if_fail((end > start) || (end < 0));
	
	buffer = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(text_view));
	g_return_if_fail(buffer != NULL);	

	gtk_text_buffer_get_iter_at_offset (buffer, &start_iter, start);

	if (end < 0)
		gtk_text_buffer_get_end_iter(buffer, &end_iter);
	else
		gtk_text_buffer_get_iter_at_offset(buffer, &end_iter, end);
	
	gtk_text_buffer_place_cursor(buffer, &end_iter);

	gtk_text_buffer_move_mark_by_name(buffer, "selection_bound", &start_iter);
}

void 
gtranslator_selection_clear(GtkWidget *widget, gpointer useless)
{
	GtkTextBuffer *buffer;

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));

	gtk_text_buffer_delete_selection(buffer, TRUE, TRUE);
}

/*
 * Set po->file_changed to TRUE if the text in the translation box has been
 * updated.
 */
void 
gtranslator_translation_changed(GtkWidget  *buffer, gpointer useless)
{
	GtrMsg *msg = GTR_MSG(current_page->po->current->data);
	if (nothing_changes)
		return;
	if (!current_page->po->file_changed)
	{
		current_page->po->file_changed = TRUE;
		if(current_page->po->no_write_perms==FALSE||strstr(current_page->po->filename, "/.gtranslator/"))
		{
			//Enable widgets
			gtk_widget_set_sensitive(gtranslator_menuitems->save, TRUE);
		    	gtk_widget_set_sensitive(gtranslator_menuitems->t_save, TRUE);
			gtk_widget_set_sensitive(gtranslator_menuitems->revert, TRUE);
			gtk_widget_set_sensitive(gtranslator_menuitems->undo, TRUE);
		    	gtk_widget_set_sensitive(gtranslator_menuitems->t_undo, TRUE);
		}
		else
		{
			//Enable widgets
			gtk_widget_set_sensitive(gtranslator_menuitems->revert, TRUE);
			gtk_widget_set_sensitive(gtranslator_menuitems->undo, TRUE);
		    	gtk_widget_set_sensitive(gtranslator_menuitems->t_undo, TRUE);
		}
	}
	if (!msg->changed)
	{
		msg->changed = TRUE;
		//Enable undo item
		gtk_widget_set_sensitive(gtranslator_menuitems->undo, TRUE);
	    	gtk_widget_set_sensitive(gtranslator_menuitems->t_undo, TRUE);
		if ((GtrPreferences.unmark_fuzzy) 
		     && (msg->is_fuzzy))
		{
	     	gtranslator_message_status_set_fuzzy(msg, FALSE);
	     	/* the callback on this GtkCheckMenuItem will be called 
	     	 * and the fuzzy count will be correctly decreased */
			/*gtk_check_menu_item_set_active(
				GTK_CHECK_MENU_ITEM(the_edit_menu[17].widget),
				FALSE
			);*/
		}
		if(current_page->messages_table)
		{
			gtranslator_messages_table_update_row(current_page->messages_table, msg);
		}
	}
}

#ifdef REDUNDANT
void 
selection_get_handler(GtkWidget *widget, GtkSelectionData *selection_data,
			   guint info, guint time_stamp, gpointer data)
{
	gchar *text;
	gchar *undotted_text;

	if(!GtrPreferences.dot_char)
		return;

	text = g_strndup(selection_data->data, selection_data->length);
	undotted_text = gtranslator_utils_invert_dot(text);
	gtk_selection_data_set(selection_data, selection_data->type,
			       8, undotted_text, strlen(text));
	g_free(text);
	g_free(undotted_text);
}

/*
 * When inserting text, exchange spaces with dot chars 
 */
void 
insert_text_handler (GtkTextBuffer *textbuffer, GtkTextIter *pos,
			  const gchar *text,
			  gint length, gpointer data)
{
	gchar *result;

	result=g_strdup(text);
	if (!nothing_changes)
	{
		/*
		* Register the text for an insertion undo action.
		*/
//		gtranslator_undo_register_insertion(text, pos);

		if(GtrPreferences.dot_char)
		{
			gchar *old;
			old = result;
			result = gtranslator_utils_invert_dot((gchar*)result);
			g_free(old);
			length = strlen(result);
		}
	}

#ifdef NOT_PORTED
	gtranslator_insert_highlighted(textbuffer, pos, result, length, data);
#endif
	g_signal_handler_block(textbuffer, trans_box_insert_text_signal_id);
	gtk_text_buffer_insert(textbuffer, pos, result, length);
	g_signal_handler_unblock(textbuffer, trans_box_insert_text_signal_id);

	g_signal_stop_emission_by_name (G_OBJECT(textbuffer), "insert-text");
	//gtk_text_buffer_place_cursor(textbuffer, pos);

	g_free(result);
}
#endif


/*
 * Notebook callback used whenever the page is changed
 */
void 
gtranslator_gui_switch_page(GtkNotebook *notebook, GtkNotebookPage *notebook_page, guint pagenum, gpointer user_data) {
	GList *pagelist;
	GtrPage *page;
	
	/*
	 * Set the 'current_page' global pointer, so the right widgets/files_added
	 * get updated etc.
	 */
	/*pagelist = pages;
	while(pagelist) {
		page = (GtrPage*)pagelist->data;
		if(page->num == pagenum) {
			current_page = page;
			break;
		}
		pagelist = pagelist->next;
	}*/
	
	/*
	 * Set 'save' and 'revert' etc.
	 */
	if(current_page->po->file_changed) {
		//Enable widgets
		gtk_widget_set_sensitive(gtranslator_menuitems->save, TRUE);
	    	gtk_widget_set_sensitive(gtranslator_menuitems->t_save, TRUE);
		gtk_widget_set_sensitive(gtranslator_menuitems->revert, TRUE);
		gtk_widget_set_sensitive(gtranslator_menuitems->undo, TRUE);
	    	gtk_widget_set_sensitive(gtranslator_menuitems->t_undo, TRUE);
	}
	else {
		//Disable widgets
		gtk_widget_set_sensitive(gtranslator_menuitems->save, FALSE);
	    	gtk_widget_set_sensitive(gtranslator_menuitems->t_save, FALSE);
		gtk_widget_set_sensitive(gtranslator_menuitems->revert, FALSE);
		gtk_widget_set_sensitive(gtranslator_menuitems->undo, FALSE);
	    	gtk_widget_set_sensitive(gtranslator_menuitems->t_undo, FALSE);
	}
}

/*
 * Update the progress bar
 */
void gtranslator_update_progress_bar(void)
{
	gdouble percentage;
	
	/*
	 * Calculate the percentage.
	 */
	percentage = (gdouble)(current_page->po->translated / (gdouble)g_list_length(current_page->po->messages));

	/*
	 * Set the progress only if the values are reasonable.
	 */
	if(percentage > 0.0 || percentage < 1.0)
	{
		/*
		 * Set the progressbar status.
		 */
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(gtranslator_progress_bar),
						percentage);
	}
}
