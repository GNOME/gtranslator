/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
 *			Peeter Vois <peeter@gtranslator.org>
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
#include "color-schemes.h"
#include "dialogs.h"
#include "dnd.h"
#include "find.h"
#include "gtkspell.h"
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

/*
 * The comment/extra content area in the main window.
 */
GtrExtraContentArea *extra_content_view;

gboolean nothing_changes;

/*
 * Popup menu creation/deletion functions.
 */
static gint create_popup_menu(GtkText *widget, GdkEventButton *event, 
	gpointer d);

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

/*
 * To get the left/right moves from the cursor.
 */ 
static gint gtranslator_keyhandler(GtkWidget *widget, GdkEventKey *event);

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
 * Pop's up the curious popup-menu.
 */
static gint create_popup_menu(GtkText *widget, GdkEventButton *event, gpointer d)
{
	/*
	 * Only react on rightclick.
	 */
	if(event->button==3)
	{
		/*
		 * Only respond if a file has been present/opened and if the
		 * corresponding option is set.
		 */
		if((GtrPreferences.popup_menu) && (file_opened==TRUE))
		{
			GtkWidget *popup_menu;

			popup_menu=gnome_popup_menu_new(the_main_popup_menu);

			gtk_menu_popup(GTK_MENU(popup_menu), NULL, NULL,
				NULL, NULL, event->button, event->time);

			return TRUE;
		}
	}
	return FALSE;
}

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
	table_pane=gtk_hpaned_new();
	

	if(GtrPreferences.show_messages_table)
	{
		gtranslator_messages_table=gtranslator_messages_table_new();
	}
	else
	{
		/*
		 * Foo, foo, woo, foo -- really not the right way to do this.
		 */
		gtranslator_messages_table=gtk_label_new("");
	}

	if(GtrPreferences.show_messages_table)
	{
		table_pane_position=gtranslator_config_get_int(
			"interface/table_pane_position");

		gtk_paned_set_position(GTK_PANED(table_pane), table_pane_position);
	}
	else
	{
		gtk_paned_set_position(GTK_PANED(table_pane), -1);
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
// XXX // GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
	gnome_app_fill_toolbar(GTK_TOOLBAR(tool_bar), the_toolbar, NULL);
	gnome_app_add_toolbar(GNOME_APP(gtranslator_application), GTK_TOOLBAR(tool_bar),
			      "tool_bar", BONOBO_DOCK_ITEM_BEH_EXCLUSIVE,
			      BONOBO_DOCK_TOP, 1, 0, 0);

	search_bar = gtk_toolbar_new(); 
// XXX GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
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

	messages_table_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(messages_table_scrolled_window), gtranslator_messages_table);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(messages_table_scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_paned_pack1(GTK_PANED(table_pane), messages_table_scrolled_window, FALSE, TRUE);
	gtk_paned_pack2(GTK_PANED(table_pane), content_pane, FALSE, TRUE);
	
	gnome_app_set_contents(GNOME_APP(gtranslator_application), table_pane);

	original_text_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(vertical_box), original_text_scrolled_window, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vertical_box), gtk_hseparator_new(), FALSE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(original_text_scrolled_window),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	// XXX
	text_box = GTK_TEXT_VIEW(gtk_text_view_new ());
	gtk_text_view_set_editable(text_box, FALSE);
	gtk_text_view_set_wrap_mode(text_box, GTK_WRAP_CHAR);
	gtk_text_view_set_cursor_visible(text_box, FALSE);
	
//	text_box = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_box_view));

	gtk_container_add(GTK_CONTAINER(original_text_scrolled_window), GTK_WIDGET(text_box));
	//gtk_text_set_editable(GTK_TEXT(text_box), FALSE);

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
/*
	g_signal_connect(G_OBJECT(gtk_text_view_get_buffer(text_box)), 
			 "insert-text",
			 G_CALLBACK(insert_text_handler), NULL);
	g_signal_connect(G_OBJECT(gtk_text_view_get_buffer(text_box)),
			 "delete-range",
			 G_CALLBACK(delete_text_handler), NULL);
*/

/*	g_signal_connect(G_OBJECT(gtk_text_view_get_buffer(trans_box)), 
			 "insert-text",
			 G_CALLBACK(insert_text_handler), NULL);

	g_signal_connect(G_OBJECT(gtk_text_view_get_buffer(trans_box)), 
			 "delete-range",
			 G_CALLBACK(delete_text_handler), NULL);
	
	g_signal_connect(G_OBJECT(gtk_text_view_get_buffer(trans_box)), 
			 "changed",
			 G_CALLBACK(gtranslator_translation_changed), NULL);
*/
	g_signal_connect(G_OBJECT(text_box), "button-press-event",
			 G_CALLBACK(create_popup_menu), NULL);
	g_signal_connect(G_OBJECT(trans_box), "button-press-event",
			 G_CALLBACK(create_popup_menu), NULL);
	
	g_signal_connect(G_OBJECT(gtranslator_application), "key-press-event",
			 G_CALLBACK(gtranslator_keyhandler), NULL);
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
		
//		gtranslator_undo_register_deletion(fake_string, start_position);
		GTR_FREE(fake_string);
	}
	gtranslator_delete_highlighted(textbuf, start, end, NULL);
	g_signal_stop_emission_by_name (G_OBJECT (textbuf), "delete_text");
}

/*
 * The own quit-code
 */
gint gtranslator_quit(GtkWidget  * widget, GdkEventAny  * e,
			     gpointer useless)
{
	/*
	 * If file was changed, but user pressed Cancel, don't quit
	 */
	if (!gtranslator_should_the_file_be_saved_dialog())
		return TRUE;

	gtranslator_file_close(NULL, NULL);

	/*
	 * Get the EPaned's position offset.
	 */
	table_pane_position=gtk_paned_get_position(GTK_PANED(table_pane));
	
	/*
	 * Store the pane position in the preferences.
	 */
	gtranslator_config_set_int("interface/table_pane_position", table_pane_position);
	gtranslator_utils_save_geometry();

	/*
	 * "Flush" our runtime config string for the current filename.
	 */
	gtranslator_config_set_string("runtime/filename", "--- No file ---");

	/*
	 * Save the messages table state.
	 */
	if(GtrPreferences.show_messages_table)
	{
		gtranslator_messages_table_save_state();
	}

	/*
	 * Shutdown our internal learning system.
	 */
	if(gtranslator_learn_initialized())
	{
		gtranslator_learn_shutdown();
	}

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
	return FALSE;
}

/*
 * Cleans up the text boxes.
 */
void gtranslator_text_boxes_clean()
{
	GtkTextIter start, end;
	GtkTextBuffer *buffer;

	buffer = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(text_box));
	gtk_text_buffer_get_start_iter(buffer, &start);
	gtk_text_buffer_get_end_iter(buffer, &end);
	gtk_text_buffer_delete(buffer, &start, &end);

	buffer = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(trans_box));
	gtk_text_buffer_get_start_iter(buffer, &start);
	gtk_text_buffer_get_end_iter(buffer, &end);
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
			status=g_strdup_printf(_("%s [ %i Fuzzy left ]"), _("Fuzzy"), po->fuzzy);
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
			status=g_strdup_printf(_("%s [ %i Untranslated left ]"), _("Untranslated"), missya);
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
	 * Assign the first part.
	 */
	str=g_strdup_printf(_("Message %d / %d / Status: %s"), pos + 1, po->length, status);
	
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
 * The text oriented callbacks
 */
void gtranslator_clipboard_cut(GtkWidget  * widget, gpointer useless)
{
	gtk_editable_cut_clipboard(GTK_EDITABLE(trans_box));
}

void gtranslator_clipboard_copy(GtkWidget  * widget, gpointer useless)
{
	gtk_editable_copy_clipboard(GTK_EDITABLE(trans_box));
}

void gtranslator_clipboard_paste(GtkWidget  * widget, gpointer useless)
{
	gtk_editable_paste_clipboard(GTK_EDITABLE(trans_box));
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
			gtk_check_menu_item_set_active(
				GTK_CHECK_MENU_ITEM(the_msg_status_menu[0].widget),
				FALSE
			);
		}
		if(GtrPreferences.show_messages_table)
		{
			gtranslator_messages_table_update_message_status(msg);
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
//		gtranslator_undo_register_insertion(text, *position);

		if(GtrPreferences.dot_char)
		{
			gchar *old;
			old = result;
			result = gtranslator_utils_invert_dot((gchar*)result);
			g_free(old);
		}
	}
	gtranslator_insert_highlighted(textbuffer, pos, result, length, NULL);
	g_free(result);
	g_signal_stop_emission_by_name (G_OBJECT(textbuffer), "insert_text");
	gtk_text_buffer_place_cursor(textbuffer, pos);
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

/*
 * The own keyhandler to get the left/right/up/down actions.
 */ 
static gint gtranslator_keyhandler(GtkWidget *widget, GdkEventKey *event)
{
	g_return_val_if_fail(widget!=NULL, FALSE);
	g_return_val_if_fail(event!=NULL, FALSE);
	#define IfGood(x) \
	if(GTK_WIDGET_SENSITIVE(GTK_WIDGET(((GnomeUIInfo)(x)).widget)))

	if(file_opened)
	{
		/*
		 * If we're having a Ctrl+Shift+Arrow Key issue, then respect
		 *  this to be something another then just Ctrl+Arrow Key.
		 */
		if(event->state & GDK_SHIFT_MASK)
		{
			return TRUE;
		}

		/*
		 * Now handle out "normal" navigation keys consisting out of
		 *  Ctrl+Arrow Key.
		 */
		if(event->state & GDK_CONTROL_MASK)
		{
			switch(event->keyval)
			{
				case GDK_Left:
				case GDK_Up:
					IfGood(the_navibar[1])
					{
						gtranslator_message_go_to_previous(NULL, NULL);
					}
					break;
				
				case GDK_Right:
				case GDK_Down:
					IfGood(the_navibar[3])
					{
						gtranslator_message_go_to_next(NULL, NULL);
					}
					break;

				case GDK_Page_Up:
					IfGood(the_navibar[0])
					{
						gtranslator_message_go_to_first(NULL, NULL);
					}
					break;
					
				case GDK_Page_Down:
					IfGood(the_navibar[4])
					{
						gtranslator_message_go_to_last(NULL, NULL);
					}
					break;
					
				default:
					break;
			}
		}
	}
	
	return TRUE;
}
