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
#include "sidebar.h"
#include "stylistics.h"
#include "syntax.h"
#include "undo.h"
#include "utils.h"
#include "utils_gui.h"
#include "views.h"

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

#include <gal/e-paned/e-hpaned.h>
#include <gal/e-paned/e-vpaned.h>

#include <gal/e-table/e-table.h>
#include <gal/e-table/e-table-scrolled.h>

/*
 * Global external variables
 */
GtkWidget *gtranslator_application;
GtkWidget *gtranslator_messages_table;
GtkWidget *trans_box;
GtkWidget *text_box;
GtkWidget *gtranslator_application_bar;

GtkWidget *sidebar_pane;
GtkWidget *content_pane;
GtkWidget *table_pane;

/*
 * The comment/extra content area in the main window.
 */
GtrExtraContentArea *extra_content_view;

/*
 * Internally used local-global variables
 */
static GtkWidget *views_sidebar;

gboolean nothing_changes;

/*
 * Popup menu creation/deletion functions.
 */
static gint create_popup_menu(GtkText *widget, GdkEventButton *event, 
	gpointer d);

/*
 * Internal text callbacks/handlers.
 */
static void insert_text_handler (GtkEditable *editable, const gchar *text,
	gint length, gint *position, gpointer data);

static void delete_text_handler(GtkEditable *editable, gint start_position,
	gint end_position);

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
static gint 	update_count=0;

/*
 * Pane positions storage variables.
 */
static gint 	sidebar_pane_position;
static gint	table_pane_position;

/*
 * The popup-menu.
 */
static GnomeUIInfo the_popup_menu[] = {
	GNOMEUIINFO_MENU_OPEN_ITEM(gtranslator_open_file_dialog, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_CUT_ITEM(gtranslator_clipboard_cut, NULL),
	GNOMEUIINFO_MENU_COPY_ITEM(gtranslator_clipboard_copy, NULL),
	GNOMEUIINFO_MENU_PASTE_ITEM(gtranslator_clipboard_paste, NULL),
	GNOMEUIINFO_MENU_CLEAR_ITEM(gtranslator_selection_clear, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_CLOSE_ITEM(gtranslator_file_close, NULL),
	GNOMEUIINFO_MENU_SAVE_AS_ITEM(gtranslator_save_file_as_dialog, NULL),
	GNOMEUIINFO_END
};

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

			popup_menu=gnome_popup_menu_new(the_popup_menu);

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

	/*
	 * Create the app	
	 */
	gtranslator_application = gnome_app_new("gtranslator", "gtranslator");
	gnome_app_create_menus(GNOME_APP(gtranslator_application), the_menus);

	/*
	 * Create all the panes we're using later on.
	 */
	sidebar_pane=e_hpaned_new();
	content_pane=e_vpaned_new();
	table_pane=e_hpaned_new();
	
	/*
	 * Create the sidebars and/or messages table.
	 */
	views_sidebar=gtranslator_sidebar_new();

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

	gtranslator_config_init();

	/*
	 * Activate the paned widgets if desired and fill them up with the right positions.
	 */
	if(GtrPreferences.show_sidebar)
	{
		sidebar_pane_position=gtranslator_config_get_int(
			"interface/sidebar_pane_position");
		
		e_paned_set_position(E_PANED(sidebar_pane), sidebar_pane_position);
	}
	else
	{
		e_paned_set_position(E_PANED(sidebar_pane), 0);
	}

	if(GtrPreferences.show_messages_table)
	{
		table_pane_position=gtranslator_config_get_int(
			"interface/table_pane_position");

		e_paned_set_position(E_PANED(table_pane), table_pane_position);
	}
	else
	{
		e_paned_set_position(E_PANED(table_pane), 0);
	}
	
	gtranslator_config_close();

	extra_content_view=g_new0(GtrExtraContentArea, 1);
	
	extra_content_view->box=gtk_hbox_new(FALSE, 1);
	extra_content_view->comment=gtk_label_new("");
	extra_content_view->edit_button=gtk_button_new_with_label(_("Edit comment"));

	gtk_box_pack_start(GTK_BOX(extra_content_view->box), extra_content_view->comment, 
		FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(extra_content_view->box), extra_content_view->edit_button, 
		FALSE, FALSE, 0);
	
	gtk_widget_set_sensitive(extra_content_view->edit_button, FALSE);
	e_paned_set_position(E_PANED(content_pane), 0);

	/*
	 * Create the tool- and search-bar
	 */
	tool_bar =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
	gnome_app_fill_toolbar(GTK_TOOLBAR(tool_bar), the_toolbar, NULL);
	gnome_app_add_toolbar(GNOME_APP(gtranslator_application), GTK_TOOLBAR(tool_bar),
			      "tool_bar", GNOME_DOCK_ITEM_BEH_EXCLUSIVE,
			      GNOME_DOCK_TOP, 1, 0, 0);

	search_bar = gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
				     GTK_TOOLBAR_BOTH);
	gnome_app_fill_toolbar(GTK_TOOLBAR(search_bar), the_navibar, NULL);
	gnome_app_add_toolbar(GNOME_APP(gtranslator_application), 
		GTK_TOOLBAR(search_bar),
		"search_bar", GNOME_DOCK_ITEM_BEH_EXCLUSIVE,
		GNOME_DOCK_TOP, 2, 0, 0);

	vertical_box=gtk_vbox_new(FALSE, 0);

	/*
	 * Perform all the packing action between the EPaneds.
	 */
	e_paned_pack1(E_PANED(content_pane), extra_content_view->box, TRUE, FALSE);
	e_paned_pack2(E_PANED(content_pane), vertical_box, TRUE, FALSE);

	e_paned_pack1(E_PANED(table_pane), gtranslator_messages_table, TRUE, FALSE);
	e_paned_pack2(E_PANED(table_pane), content_pane, TRUE, FALSE);
	
	e_paned_pack1(E_PANED(sidebar_pane), views_sidebar, TRUE, FALSE);
	e_paned_pack2(E_PANED(sidebar_pane), table_pane, TRUE, FALSE);
	gnome_app_set_contents(GNOME_APP(gtranslator_application), sidebar_pane);

	original_text_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(vertical_box), original_text_scrolled_window, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(original_text_scrolled_window),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	text_box=gtk_text_new(NULL,NULL);
	
	gtk_container_add(GTK_CONTAINER(original_text_scrolled_window), text_box);
	
	gtk_text_set_editable(GTK_TEXT(text_box), FALSE);

	translation_text_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(vertical_box), translation_text_scrolled_window, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(translation_text_scrolled_window),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	trans_box = gtk_text_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(translation_text_scrolled_window), trans_box);

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
	gtranslator_config_init();
	gtranslator_set_style(text_box, 0);
	gtranslator_set_style(trans_box, 1);
	gtranslator_config_close();
	
	/*
	 * The callbacks list
	 */
	gtk_signal_connect(GTK_OBJECT(gtranslator_application), "delete_event",
			   GTK_SIGNAL_FUNC(gtranslator_quit), NULL);

	gtk_signal_connect(GTK_OBJECT(extra_content_view->edit_button), "clicked",
			   GTK_SIGNAL_FUNC(gtranslator_edit_comment_dialog), NULL);

	gtk_signal_connect_after(GTK_OBJECT(text_box), "selection_get",
			   GTK_SIGNAL_FUNC(selection_get_handler), NULL);
	gtk_signal_connect_after(GTK_OBJECT(trans_box), "selection_get",
			   GTK_SIGNAL_FUNC(selection_get_handler), NULL);

	gtk_signal_connect(GTK_OBJECT(trans_box), "insert_text",
			   GTK_SIGNAL_FUNC(insert_text_handler), NULL);
	gtk_signal_connect(GTK_OBJECT(trans_box), "delete_text",
			   GTK_SIGNAL_FUNC(delete_text_handler), NULL);
	
	gtk_signal_connect(GTK_OBJECT(trans_box), "changed",
			   GTK_SIGNAL_FUNC(gtranslator_translation_changed), NULL);
	
	gtk_signal_connect(GTK_OBJECT(text_box), "button_press_event",
			   GTK_SIGNAL_FUNC(create_popup_menu), NULL);
	gtk_signal_connect(GTK_OBJECT(trans_box), "button_press_event",
			   GTK_SIGNAL_FUNC(create_popup_menu), NULL);
	
	gtk_signal_connect(GTK_OBJECT(gtranslator_application), "key_press_event",
			   GTK_SIGNAL_FUNC(gtranslator_keyhandler), NULL);
	/*
	 * The D'n'D signals
	 */
	gtk_drag_dest_set(GTK_WIDGET(gtranslator_application),
			  GTK_DEST_DEFAULT_ALL | GTK_DEST_DEFAULT_HIGHLIGHT,
			  dragtypes, sizeof(dragtypes) / sizeof(dragtypes[0]),
			  GDK_ACTION_COPY);
	gtk_signal_connect(GTK_OBJECT(gtranslator_application), "drag_data_received",
			   GTK_SIGNAL_FUNC(gtranslator_dnd),
			   GUINT_TO_POINTER(dnd_type));
}

/*
 * An own delete text handler which should work on deletion in the translation
 *  box -- undo is called up here, too.
 */
static void delete_text_handler(GtkEditable *editable, gint start_position,
	gint end_position)
{
	/*
	 * Check for dumb values and do only catch real deletions.
	 */
	if(start_position >= 0 && end_position >= 1 && 
		(end_position > start_position))
	{
		gchar	*fake_string;

		/*
		 * Fill up our fake_string for the strlen() call in undo.c.
		 */
		fake_string=gtk_editable_get_chars(GTK_EDITABLE(trans_box),
			start_position, (end_position - start_position));
		
		gtranslator_undo_register_deletion(fake_string, start_position);
		GTR_FREE(fake_string);
	}
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
	 * Initialize the config and set the pane positions -- if needed.
	 */
	gtranslator_config_init();

	if(GtrPreferences.show_sidebar)
	{
		sidebar_pane_position=e_paned_get_position(E_PANED(sidebar_pane));
		gtranslator_config_set_int("interface/sidebar_pane_position", sidebar_pane_position);
	}

	/*
	 * Get the EPaned's position offset.
	 */
	table_pane_position=e_paned_get_position(E_PANED(table_pane));
	
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
	 * Free the preferences stuff.
	 */
	gtranslator_preferences_free();
	gnome_regex_cache_destroy(rxc);

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
	gtranslator_config_close();

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
	gtk_editable_delete_text(GTK_EDITABLE(text_box), 0, -1);
	gtk_editable_delete_text(GTK_EDITABLE(trans_box), 0, -1);
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
		status=g_strdup(_("Stick"));
	} else if(msg->status & GTR_MSG_STATUS_TRANSLATED) {
		status=g_strdup(_("Translated"));
	} else {
		/*
		 * Message is untranslated 
		 */
		if ((po->length - po->translated)>0)
		{
			guint missya;
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
	gtk_editable_delete_selection(GTK_EDITABLE(trans_box));
}

/*
 * Set po->file_changed to TRUE if the text in the translation box has been
 * updated.
 */
void gtranslator_translation_changed(GtkWidget  * widget, gpointer useless)
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
			gtranslator_messages_table_update_message_status(msg);
			gtranslator_actions_disable_fuzzy_menu_item();
		}
	}

	if(widget)
	{
		gint selpos=0;

		update_count++;

		/*
		 * Check if there were already made an update -- don't update
		 *  the syntax 3x for only one reason/change.
		 */
		if(update_count >= 2)
		{
			update_count=0;
			return;
		}
	
		/*
		 * Determine any selections the translation box currently owns.
		 */
		if(GTK_EDITABLE(widget)->has_selection)
		{
			selpos=gtk_text_get_point(GTK_TEXT(widget));
		}
		
		gtranslator_syntax_update_text(widget);

		/*
		 * If there were any position from the selection we can 
		 *  now again go there.
		 */
		if(selpos > 0)
		{
			gtk_text_set_point(GTK_TEXT(widget), selpos);
			selpos=0;
		}
	}
}

/*
 * When inserting text, exchange spaces with dot chars 
 */
void insert_text_handler (GtkEditable *editable, const gchar *text,
			  gint length, gint *position, gpointer data)
{
	gchar *result;

	if (nothing_changes)
		return;
		
	/*
	 * Do all these steps only if the option to use the '·' is set.
	 */
	if(!GtrPreferences.dot_char)
		return;

	result=g_strdup(text);

	/*
	 * Register the text for an insertion undo action.
	 */
	gtranslator_undo_register_insertion(result, *position);
	
	gtranslator_utils_invert_dot(result);
	
	gtk_signal_handler_block_by_func(GTK_OBJECT(editable),
					 GTK_SIGNAL_FUNC(insert_text_handler),
					 data);
	
	gtk_editable_insert_text(editable, result, length, position);
	
	gtk_signal_handler_unblock_by_func(GTK_OBJECT(editable),
					   GTK_SIGNAL_FUNC(insert_text_handler),
					   data);
	
	gtk_signal_emit_stop_by_name (GTK_OBJECT (editable), "insert_text");

	GTR_FREE(result);
}

void selection_get_handler(GtkWidget *widget, GtkSelectionData *selection_data,
			   guint info, guint time_stamp, gpointer data)
{
	gchar *text;

	if(!GtrPreferences.dot_char)
		return;

	text = g_strndup(selection_data->data, selection_data->length);
	gtranslator_utils_invert_dot(text);
	gtk_selection_data_set(selection_data, selection_data->type,
			       8, text, strlen(text));
	GTR_FREE(text);
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

/*
 * Switch to the given view.
 */
void gtranslator_switch_views(GtkWidget *widget, gpointer view)
{
	gtranslator_views_set(GPOINTER_TO_INT(view));
}
