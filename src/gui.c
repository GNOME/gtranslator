/*
 * (C) 2000-2004 	Fatih Demir <kabalak@kabalak.net>
 *			Ross Golder <ross@golder.org>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Peeter Vois <peeter@kabalak.net>
 *			Thomas Ziehmer <thomas@kabalak.net>
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
#include "gui.h"
#include "header_stuff.h"
#include "history.h"
#include "learn.h"
#include "menus.h"
#include "message.h"
#include "messages-table.h"
#include "nautilus-string.h"
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

#include <gtk/gtkdnd.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkmain.h>
#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtktogglebutton.h>
#include <gtk/gtkvbox.h>

#include <libgnomeui/gnome-app.h>
#include <libgnomeui/gnome-app-helper.h>
#include <libgnomeui/gnome-popup-menu.h>

#include <libgnomevfs/gnome-vfs-init.h>
#include <gtkspell/gtkspell.h>

/*
 * Global external variables
 */
GtkWidget *gtranslator_application;
GtkWidget *gtranslator_application_bar;

guint trans_box_insert_text_signal_id;
guint trans_box_delete_text_signal_id;

/*
 * The widgets related to displaying the current document 
 */
GtrDocumentView *document_view;

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
static gint	table_pane_position;

/*
 * Creates the main gtranslator window.
 */
void gtranslator_create_main_window(void)
{
	GtkWidget *tool_bar;
	GtkWidget *search_bar;
	
	/*
	 * Create the app	
	 */
	gtranslator_application = gnome_app_new(PACKAGE_NAME, PACKAGE_NAME);
	
	/*
	 * Set up the menus
	 */
	gnome_app_create_menus(GNOME_APP(gtranslator_application), the_menus);
	
	/*
	 * Start with no document view
	 */
	document_view = NULL;

	/*
	 * Create the tool bar
	 */
	tool_bar = gtk_toolbar_new();
	gnome_app_fill_toolbar(GTK_TOOLBAR(tool_bar), the_toolbar, NULL);
	gnome_app_add_toolbar(GNOME_APP(gtranslator_application),
		GTK_TOOLBAR(tool_bar), "tool_bar", BONOBO_DOCK_ITEM_BEH_EXCLUSIVE,
		BONOBO_DOCK_TOP, 1, 0, 0);

	/*
	 * Create the search bar
	 */
	search_bar = gtk_toolbar_new();
	gnome_app_fill_toolbar(GTK_TOOLBAR(search_bar), the_navibar, NULL);
	gnome_app_add_toolbar(GNOME_APP(gtranslator_application), 
		GTK_TOOLBAR(search_bar), "search_bar", BONOBO_DOCK_ITEM_BEH_EXCLUSIVE,
		BONOBO_DOCK_TOP, 2, 0, 0);

	/*
	 * Set up the application bar
	 */
	gtranslator_application_bar = gnome_appbar_new(TRUE, TRUE, GNOME_PREFERENCES_NEVER);
	gnome_app_set_statusbar(GNOME_APP(gtranslator_application), gtranslator_application_bar);
	
	/*
	 * Make menu hints display on the appbar
	 */
	gnome_app_install_menu_hints(GNOME_APP(gtranslator_application), the_menus);

	gtranslator_actions_set_up_default();

	gtranslator_history_show();

	/*
	 * The callbacks list
	 */
	g_signal_connect(G_OBJECT(gtranslator_application), "delete_event",
			 G_CALLBACK(gtranslator_quit), NULL);

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

}

#ifdef REDUNDANT
/*
 * An own delete text handler which should work on deletion in the translation
 *  box -- undo is called up here, too.
 */
void delete_text_handler(GtkTextBuffer *textbuf, GtkTextIter *start,
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
 * The own quit-code
 */
void gtranslator_quit(GtkWidget  * widget, GdkEventAny  * e,
			     gpointer useless)
{
	/*
	 * If file was changed, but user pressed Cancel, don't quit
	 */
	if (!gtranslator_should_the_file_be_saved_dialog())
		return;

	if (po)
		gtranslator_file_close(NULL, NULL);

	if(GtrPreferences.show_messages_table && (document_view != NULL))
	{
		/*
		 * Get the EPaned's position offset.
		 */
		table_pane_position=gtk_paned_get_position(GTK_PANED(document_view->table_pane));
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
	g_free(document_view);

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


void gtranslator_application_bar_update(gint pos)
{
	gchar 	*str, *status;
	GtrMsg 	*msg;
	const char *msgstr, *msgid_plural;
	gnome_appbar_pop(GNOME_APPBAR(gtranslator_application_bar));

	/*
	 * Get the message.
	 */
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
			status=g_strdup_printf(ngettext("%s [ %i Fuzzy left ]", "%s [ %i Fuzzy left ]", po->fuzzy), _("Fuzzy"), po->fuzzy);
		}	
		else
		{
			status=g_strdup_printf(_("%s [ No fuzzy left ]"), _("Fuzzy"));
			
			/*
			 * Also disable the corresponding button.
			 */
			gtranslator_actions_disable(ACT_NEXT_FUZZY);
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
			gtranslator_actions_disable(ACT_NEXT_UNTRANSLATED, ACT_AUTOTRANSLATE);
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
	 * Set the appbar text.
	 */
	gnome_appbar_push(GNOME_APPBAR(gtranslator_application_bar), str);
	
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
static GtkTextBuffer* get_selection_buffer(GtkTextBuffer *buffer)
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
void gtranslator_clipboard_cut(GtkWidget  * widget, gpointer useless)
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

void gtranslator_clipboard_copy(GtkWidget  * widget, gpointer useless)
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

void gtranslator_clipboard_paste(GtkWidget  * widget, gpointer useless)
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

void gtranslator_selection_clear(GtkWidget *widget, gpointer useless)
{
	GtkTextBuffer *buffer;

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));

	gtk_text_buffer_delete_selection(buffer, TRUE, TRUE);
}

/*
 * Set po->file_changed to TRUE if the text in the translation box has been
 * updated.
 */
void gtranslator_translation_changed(GtkWidget  *buffer, gpointer useless)
{
	GtrMsg *msg = GTR_MSG(po->current->data);
	if (nothing_changes)
		return;
	if (!po->file_changed)
	{
		po->file_changed = TRUE;
		if(po->no_write_perms==FALSE||strstr(po->filename, "/.gtranslator/"))
		{
			gtranslator_actions_enable(ACT_SAVE, ACT_REVERT, ACT_UNDO);
		}
		else
		{
			gtranslator_actions_enable(ACT_REVERT, ACT_UNDO);
		}
	}
	if (!msg->changed)
	{
		msg->changed = TRUE;
		gtranslator_actions_enable(ACT_UNDO);
		if ((GtrPreferences.unmark_fuzzy) 
		     && (msg->is_fuzzy))
		{
		     	gtranslator_message_status_set_fuzzy(msg, FALSE);
			po->fuzzy--;
			gtk_check_menu_item_set_active(
				GTK_CHECK_MENU_ITEM(the_edit_menu[17].widget),
				FALSE
			);
		}
		if(GtrPreferences.show_messages_table)
		{
			gtranslator_messages_table_update_row(msg);
		}
	}
}

#ifdef REDUNDANT
void selection_get_handler(GtkWidget *widget, GtkSelectionData *selection_data,
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
void insert_text_handler (GtkTextBuffer *textbuffer, GtkTextIter *pos,
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
 * Set up the widgets to display the given po file
 */
void gtranslator_show_file(GtrPo *po)
{
	GtkWidget *comments_viewport;
	GtkWidget *vertical_box;
	GtkWidget *horizontal_box;
	GtkWidget *comments_scrolled_window;
	GtkWidget *original_text_scrolled_window;
	GtkWidget *translation_text_scrolled_window;
	
	gchar 	*title;
	
	g_return_if_fail(po!=NULL);

	/*
	 * Set up a document view structure to contain the widgets related
	 * to this file
	 */
	document_view = g_new0(GtrDocumentView, 1);
	document_view->content_pane = gtk_vpaned_new();
	document_view->table_pane = gtk_hpaned_new();
	
	/*
	 * Create the hpane that will hold the messages table and the current
	 * message, even if messages table is suppressed, so it can be
	 * dynamically switched on/off from a menu (rather than a preference
	 * that requires a program restart! yuk!)
	 */
	table_pane_position=gtranslator_config_get_int("interface/table_pane_position");
	gtk_paned_set_position(GTK_PANED(document_view->table_pane), table_pane_position);

	horizontal_box=gtk_hbox_new(FALSE, 1);

	/*
	 * Set up the scrolling window for the comments display
	 */	
	comments_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(comments_scrolled_window),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(horizontal_box), comments_scrolled_window, TRUE, TRUE, 0);
	
	comments_viewport = gtk_viewport_new(NULL, NULL);
	gtk_widget_show(comments_viewport);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(comments_scrolled_window), comments_viewport);
	
	document_view->comment=gtk_label_new("");
	gtk_container_add(GTK_CONTAINER(comments_viewport), document_view->comment);
	
	document_view->edit_button=gtk_button_new_with_label(_("Edit comment"));
	gtk_widget_set_sensitive(document_view->edit_button, FALSE);
	gtk_box_pack_end(GTK_BOX(horizontal_box), document_view->edit_button,
		FALSE, FALSE, 0);
	
	gtk_paned_set_position(GTK_PANED(document_view->content_pane), 0);

	/*
	 * Pack the comments pane and the main content
	 */
	vertical_box=gtk_vbox_new(FALSE, 0);
	gtk_paned_pack1(GTK_PANED(document_view->content_pane), horizontal_box, TRUE, FALSE);
	gtk_paned_pack2(GTK_PANED(document_view->content_pane), vertical_box, FALSE, TRUE);
	
	/* Message string box is a vbox, containing one textview in most cases,
	   or two in the case of a plural message */
	original_text_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(vertical_box), original_text_scrolled_window, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vertical_box), gtk_hseparator_new(), FALSE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(original_text_scrolled_window),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	document_view->text_vbox = gtk_vbox_new(TRUE, 1);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(original_text_scrolled_window), document_view->text_vbox);

	/* Translation box is a vbox, containing one textview in most cases,
	   or more in the case of a plural message */
	translation_text_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(vertical_box), translation_text_scrolled_window, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(translation_text_scrolled_window),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	document_view->trans_vbox = gtk_vbox_new(TRUE, 1);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(translation_text_scrolled_window), document_view->trans_vbox);

	/*
	 * Tie up callback for 'comments' button
	 */
	g_signal_connect(G_OBJECT(document_view->edit_button), "clicked",
			 G_CALLBACK(gtranslator_edit_comment_dialog), NULL);

	/*
	 * If required, set up the messages table
	 */	
	if(GtrPreferences.show_messages_table)
	{
		GtkWidget *messages_table_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
		document_view->messages_tree = gtranslator_messages_table_new();
		gtk_container_add(GTK_CONTAINER(messages_table_scrolled_window), document_view->messages_tree);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(messages_table_scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		
		gtk_paned_pack1(GTK_PANED(document_view->table_pane), messages_table_scrolled_window, FALSE, TRUE);
		gtk_paned_pack2(GTK_PANED(document_view->table_pane), document_view->content_pane, FALSE, TRUE);
	}

	/*
	 * Set the main window
	 */
	if(GtrPreferences.show_messages_table) {
		gtk_widget_show(document_view->table_pane);
		gnome_app_set_contents(GNOME_APP(gtranslator_application), document_view->table_pane);
	} else {
		gtk_widget_show(document_view->content_pane);
		gnome_app_set_contents(GNOME_APP(gtranslator_application), document_view->content_pane);
	}

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
	 * Update the recent files list.
	 */
	gtranslator_history_show();
}

/*
 * Update the progress bar
 */
void gtranslator_update_progress_bar(void)
{
	gfloat percentage;
	
	/*
	 * Calculate the percentage.
	 */
	percentage = 1.0 * (po->translated / g_list_length(po->messages));

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
