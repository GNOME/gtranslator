/*
 * (C) 2000-2003 	Fatih Demir <kabalak@kabalak.net>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Peeter Vois <peeter@kabalak.net>
 *			Thomas Ziehmer <thomas@kabalak.net>
 *			Ross Golder <ross@kabalak.net>
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
#include "color-schemes.h"
#include "dialogs.h"
#include "dnd.h"
#include "find.h"
#ifdef NOT_PORTED
# include "gtkspell.h"
#endif
#include "gui.h"
#include "header_stuff.h"
#include "history.h"
#include "learn.h"
#include "menus.h"
#include "message.h"
#include "messages-table.h"
#include "parse.h"
#include "prefs.h"
#include "runtime-config.h"
#include "stylistics.h"
#include "syntax.h"
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

/*
 * Global external variables
 */
GtkWidget *gtranslator_application;
GtkWidget *gtranslator_messages_table;
GtkTextView *trans_box;
GtkTextView *text_box;
GtkWidget *gtranslator_application_bar;

GtkWidget *content_pane;
GtkWidget *table_pane;
#ifdef NOT_PORTED
guint text_box_insert_text_signal_id;
guint text_box_delete_text_signal_id;
#endif
guint trans_box_insert_text_signal_id;
guint trans_box_delete_text_signal_id;

/*
 * The comment/extra content area in the main window.
 */
GtrExtraContentArea *extra_content_view;

gboolean nothing_changes;

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

static GtkTextBuffer* get_selection_buffer(GtkTextBuffer *buffer);

/*
 * The target formats
 */
static  GtkTargetEntry dragtypes[] = {
	{ "text/uri-list", 0, TARGET_URI_LIST },
	{ "text/plain", 0, TARGET_NETSCAPE_URL },
	{ "text/plain", 0, TARGET_TEXT_PLAIN }
};

/*
 * Counts the already performed update's count for the syntax updating.
 */

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
	
	GtkWidget *vertical_box;
	
	GtkWidget *original_text_scrolled_window;
	GtkWidget *translation_text_scrolled_window;
	GtkWidget *comments_scrolled_window;
	GtkWidget *comments_viewport;

	GtkWidget *messages_table_scrolled_window;
	/*
	 * Create the app	
	 */
	gtranslator_application = gnome_app_new("gtranslator", "gtranslator");
	gnome_app_create_menus(GNOME_APP(gtranslator_application), the_menus);

	/*
	 * Create all the panes we're using later on.
	 */
	content_pane=gtk_vpaned_new();
	
	if(GtrPreferences.show_messages_table)
	{
		gtranslator_messages_table=gtranslator_messages_table_new();
		table_pane_position=gtranslator_config_get_int(
			"interface/table_pane_position");
		table_pane=gtk_hpaned_new();
		gtk_paned_set_position(GTK_PANED(table_pane), table_pane_position);
	}
	
	extra_content_view=g_new0(GtrExtraContentArea, 1);
	
	extra_content_view->box=gtk_hbox_new(FALSE, 1);
	
	comments_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(comments_scrolled_window),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(extra_content_view->box), comments_scrolled_window, TRUE, TRUE, 0);
	
	comments_viewport = gtk_viewport_new(NULL, NULL);
	gtk_widget_show(comments_viewport);
	gtk_container_add(GTK_CONTAINER(comments_scrolled_window), comments_viewport);
	
	extra_content_view->comment=gtk_label_new("");
	gtk_container_add(GTK_CONTAINER(comments_viewport), extra_content_view->comment);
	
	extra_content_view->edit_button=gtk_button_new_with_label(_("Edit comment"));
	gtk_widget_set_sensitive(extra_content_view->edit_button, FALSE);
	gtk_box_pack_end(GTK_BOX(extra_content_view->box), extra_content_view->edit_button,
		FALSE, FALSE, 0);
	
	gtk_paned_set_position(GTK_PANED(content_pane), 0);

	/*
	 * Create the tool- and search-bar
	 */
	tool_bar = gtk_toolbar_new();
	gnome_app_fill_toolbar(GTK_TOOLBAR(tool_bar), the_toolbar, NULL);
	gnome_app_add_toolbar(GNOME_APP(gtranslator_application), GTK_TOOLBAR(tool_bar),
			      "tool_bar", BONOBO_DOCK_ITEM_BEH_EXCLUSIVE,
			      BONOBO_DOCK_TOP, 1, 0, 0);

	search_bar = gtk_toolbar_new();
	gnome_app_fill_toolbar(GTK_TOOLBAR(search_bar), the_navibar, NULL);
	gnome_app_add_toolbar(GNOME_APP(gtranslator_application), 
		GTK_TOOLBAR(search_bar),
		"search_bar", BONOBO_DOCK_ITEM_BEH_EXCLUSIVE,
		BONOBO_DOCK_TOP, 2, 0, 0);

	vertical_box=gtk_vbox_new(FALSE, 0);

	/*
	 * Perform all the packing action between the EPaneds.
	 */
	gtk_paned_pack1(GTK_PANED(content_pane), extra_content_view->box, 
			TRUE, FALSE);
	gtk_paned_pack2(GTK_PANED(content_pane), vertical_box, FALSE, TRUE);

	if(GtrPreferences.show_messages_table)
	{
		messages_table_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
		gtk_container_add(GTK_CONTAINER(messages_table_scrolled_window), gtranslator_messages_table);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(messages_table_scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		
		gtk_paned_pack1(GTK_PANED(table_pane), messages_table_scrolled_window, FALSE, TRUE);
		gtk_paned_pack2(GTK_PANED(table_pane), content_pane, FALSE, TRUE);
	
		gnome_app_set_contents(GNOME_APP(gtranslator_application), table_pane);
	} else {
		gnome_app_set_contents(GNOME_APP(gtranslator_application), content_pane);
	}

	original_text_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(vertical_box), original_text_scrolled_window, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vertical_box), gtk_hseparator_new(), FALSE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(original_text_scrolled_window),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	text_box = GTK_TEXT_VIEW(gtk_text_view_new ());
	gtk_text_view_set_editable(text_box, FALSE);
	gtk_text_view_set_wrap_mode(text_box, GTK_WRAP_CHAR);
	gtk_text_view_set_cursor_visible(text_box, FALSE);

	gtk_container_add(GTK_CONTAINER(original_text_scrolled_window), GTK_WIDGET(text_box));

	translation_text_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(vertical_box), translation_text_scrolled_window, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(translation_text_scrolled_window),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	trans_box = GTK_TEXT_VIEW(gtk_text_view_new());
	gtk_text_view_set_editable(trans_box, TRUE);
	gtk_text_view_set_wrap_mode(trans_box, GTK_WRAP_CHAR);
	gtk_text_view_set_cursor_visible(trans_box, TRUE);

	gtk_container_add(GTK_CONTAINER(translation_text_scrolled_window), GTK_WIDGET(trans_box));

	gtranslator_application_bar = gnome_appbar_new(TRUE, TRUE, GNOME_PREFERENCES_NEVER);
	gnome_app_set_statusbar(GNOME_APP(gtranslator_application), gtranslator_application_bar);
	
	/*
	 * Make menu hints display on the appbar
	 */
	gnome_app_install_menu_hints(GNOME_APP(gtranslator_application), the_menus);

	gtranslator_actions_set_up_default();
	gtranslator_actions_set_up_state_no_file();

	gtranslator_history_show();

	/*
	 * Set up our special styles for the text boxes.
	 */
	//	gtranslator_set_style(GTK_WIDGET(text_box), 0);
	//gtranslator_set_style(GTK_WIDGET(trans_box), 1);
	/*
	 *  Initialize highlighting
	 */
	 gtranslator_syntax_init(trans_box);
	 gtranslator_syntax_init(text_box);
	/*
	 * The callbacks list
	 */
	g_signal_connect(G_OBJECT(gtranslator_application), "delete_event",
			 G_CALLBACK(gtranslator_quit), NULL);

	g_signal_connect(G_OBJECT(extra_content_view->edit_button), "clicked",
			 G_CALLBACK(gtranslator_edit_comment_dialog), NULL);

	g_signal_connect_after(G_OBJECT(text_box), "selection-get",
			 G_CALLBACK(selection_get_handler), NULL);
	g_signal_connect_after(G_OBJECT(trans_box), "selection-get",
			 G_CALLBACK(selection_get_handler), NULL);

#ifdef NOT_PORTED
	text_box_insert_text_signal_id = g_signal_connect(
		G_OBJECT(gtk_text_view_get_buffer(text_box)), 
		"insert-text",
		G_CALLBACK(insert_text_handler), NULL);
	text_box_delete_text_signal_id = g_signal_connect(
		G_OBJECT(gtk_text_view_get_buffer(text_box)),
		"delete-range",
		G_CALLBACK(delete_text_handler), NULL);
#endif
	trans_box_insert_text_signal_id = g_signal_connect(
		G_OBJECT(gtk_text_view_get_buffer(trans_box)), 
		"insert-text",
		G_CALLBACK(insert_text_handler), NULL);
	trans_box_delete_text_signal_id = g_signal_connect(
		G_OBJECT(gtk_text_view_get_buffer(trans_box)), 
		"delete-range",
		G_CALLBACK(delete_text_handler), NULL);
	
	g_signal_connect(G_OBJECT(gtk_text_view_get_buffer(trans_box)), 
			 "changed",
			 G_CALLBACK(gtranslator_translation_changed), NULL);
	
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
		GTR_FREE(fake_string);
	}
	gtranslator_delete_highlighted(textbuf, start, end, NULL);
	g_signal_stop_emission_by_name (G_OBJECT (textbuf), "delete-text");
}

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

	gtranslator_file_close(NULL, NULL);

	if(GtrPreferences.show_messages_table)
	{
		/*
		 * Get the EPaned's position offset.
		 */
		table_pane_position=gtk_paned_get_position(GTK_PANED(table_pane));
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
	 * Now clean up the list of all colorschemes -- should be safe to do it
	 *  here if we've already shut down most of the GUI elements.
	 */
	gtranslator_color_scheme_delete_schemes_list();

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
	GTR_FREE(extra_content_view);

	/*
	 * Shutdown the eventually (non-)initialized stuff from GnomeVFS.
	 */
	if(gnome_vfs_initialized())
	{
		gnome_vfs_shutdown();
	}

	/*
	 * Free up our used GtrColorScheme "theme".
	 */
	gtranslator_color_scheme_free(&theme);
	
	/*
	 * Quit with the normal Gtk+ quit.
	 */
	gtk_main_quit();
	return;
}

/*
 * Cleans up the text boxes.
 */
void gtranslator_text_boxes_clean()
{
	GtkTextIter start, end;
	GtkTextBuffer *buffer;

	buffer = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(text_box));
	gtk_text_buffer_get_bounds(buffer, &start, &end);
	gtk_text_buffer_delete(buffer, &start, &end);

	buffer = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(trans_box));
	gtk_text_buffer_get_bounds(buffer, &start, &end);
	gtk_text_buffer_delete(buffer, &start, &end);
}

void gtranslator_application_bar_update(gint pos)
{
	gchar 	*str, *status;
	GtrMsg 	*msg;
	
	gnome_appbar_pop(GNOME_APPBAR(gtranslator_application_bar));
	
	/*
	 * Get the message.
	 */
	msg=GTR_MSG(po->current->data);
	
	/*
	 * And append according to the message status the status name.
	 */
	if(msg->status & GTR_MSG_STATUS_FUZZY)
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
	} else if(msg->status & GTR_MSG_STATUS_STICK) {
		status=g_strdup(_("Sticky (Message & translation are the same)"));
	} else if(msg->status & GTR_MSG_STATUS_TRANSLATED) {
		status=g_strdup(_("Translated"));
	} else {
		/*
		 * Message is untranslated 
		 */
		if ((po->length - po->translated)>0)
		{
			gint missya;
			missya = po->length - po->translated;
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
	if(msg->msgid_plural)
	{
		str=g_strdup_printf(_("Message %d / %d / Status: %s (Message contains plural forms)"), pos + 1, po->length, status);
	}
	else
	{
		str=g_strdup_printf(_("Message %d / %d / Status: %s"), pos + 1, po->length, status);
	}
	
	/*
	 * Set the appbar text.
	 */
	gnome_appbar_push(GNOME_APPBAR(gtranslator_application_bar), str);
	
	/*
	 * Update the progressbar.
	 */
	gtranslator_set_progress_bar();

	/*
	 * And free the allocated string.
	 */
	GTR_FREE(str);

	GTR_FREE(status);
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

	/* We're only cutting from the translation box */
	if (gtk_widget_is_focus(GTK_WIDGET(trans_box)) == FALSE)
		return;

	buffer = gtk_text_view_get_buffer(trans_box);
	g_return_if_fail(buffer != NULL);

	/* Copy text after replacing dots with spaces */
	selection_buffer = get_selection_buffer(buffer);
	gtk_text_buffer_copy_clipboard(selection_buffer, gtk_clipboard_get(GDK_NONE));
	
	/* Delete the selected text */
	gtk_text_buffer_get_selection_bounds(buffer, &start_iter, &end_iter);
	gtk_text_buffer_delete_interactive(buffer, &start_iter, &end_iter, TRUE);
	
	gtk_text_view_scroll_mark_onscreen(trans_box,
									   gtk_text_buffer_get_mark(buffer,
																"insert"));
}

void gtranslator_clipboard_copy(GtkWidget  * widget, gpointer useless)
{
	GtkTextView* focused_box = NULL;
	GtkTextBuffer* buffer = NULL;
	GtkTextBuffer* selection_buffer = NULL;
	
	if (gtk_widget_is_focus(GTK_WIDGET(text_box)))
		focused_box = text_box;
	
	if (gtk_widget_is_focus(GTK_WIDGET(trans_box)))
		focused_box = trans_box;
	
	g_return_if_fail(focused_box != NULL);
	
	buffer = gtk_text_view_get_buffer(focused_box);
	g_return_if_fail(buffer != NULL);

	/* Copy text after replacing dots with spaces */
	selection_buffer = get_selection_buffer(buffer);
	gtk_text_buffer_copy_clipboard(selection_buffer, gtk_clipboard_get(GDK_NONE));
  	
	gtk_text_view_scroll_mark_onscreen(focused_box,
									   gtk_text_buffer_get_mark(buffer,
																"insert"));
}

void gtranslator_clipboard_paste(GtkWidget  * widget, gpointer useless)
{
	GtkTextBuffer* buffer = NULL;

	/* We're only pasting into the translation box */
	if (gtk_widget_is_focus(GTK_WIDGET(trans_box)) == FALSE)
		return;

	buffer = gtk_text_view_get_buffer(trans_box);
	g_return_if_fail (buffer != NULL);


	gtk_text_buffer_paste_clipboard(buffer,
					gtk_clipboard_get(GDK_NONE),
					NULL, TRUE);
  	
	gtk_text_view_scroll_mark_onscreen(trans_box,
					   gtk_text_buffer_get_mark(buffer,
								    "insert"));
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


void gtranslator_selection_clear(GtkWidget  * widget, gpointer useless)
{
	GtkTextBuffer *buffer = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(trans_box));
	gtk_text_buffer_delete_selection(buffer, TRUE, TRUE);
}

/*
 * Set po->file_changed to TRUE if the text in the translation box has been
 * updated.
 */
void gtranslator_translation_changed(GtkWidget  *buffer, gpointer useless)
{
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
	if (!message_changed)
	{
		GtrMsg *msg = GTR_MSG(po->current->data);
		message_changed = TRUE;
		gtranslator_actions_enable(ACT_UNDO);
		if ((GtrPreferences.unmark_fuzzy) 
		     && (msg->status & GTR_MSG_STATUS_FUZZY))
		{
		     	gtranslator_message_status_set_fuzzy(msg, FALSE);
			po->fuzzy--;
			gtk_check_menu_item_set_active(
				GTK_CHECK_MENU_ITEM(the_edit_menu[19].widget),
				FALSE
			);
		}
		if(GtrPreferences.show_messages_table)
		{
			gtranslator_messages_table_update_row(msg);
		}
	}
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
