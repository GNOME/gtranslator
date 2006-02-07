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

#include "actions.h"
#include "page.h"
#include "dialogs.h"
#include "prefs.h"

#include <libgen.h>

/*
 * The currently active pages
 */
GList *pages;

/*
 * The currently active page
 */
GtrPage *current_page;

/*
 * Set up the widgets to display the given po file
 */
GtrPage *gtranslator_page_new(GtrPo *po)
{
	GtrPage *page;
	
	GtkWidget *comments_viewport;
	GtkWidget *vertical_box;
	GtkWidget *horizontal_box;
	GtkWidget *comments_scrolled_window;
	GtkWidget *original_text_scrolled_window;
	GtkWidget *translation_text_scrolled_window;
	
	g_return_val_if_fail(po!=NULL, NULL);

	/* Allocate a new page */
	page = g_new0(GtrPage, 1);
	page->po = po;
	
	/*
	 * Set up a document view structure to contain the widgets related
	 * to this file
	 */
	page->content_pane = gtk_vpaned_new();
	page->table_pane = gtk_hpaned_new();
	
	/*
	 * Create the hpane that will hold the messages table and the current
	 * message, even if messages table is suppressed, so it can be
	 * dynamically switched on/off from a menu (rather than a preference
	 * that requires a program restart! yuk!)
	 */
	table_pane_position=gtranslator_config_get_int("interface/table_pane_position");
	gtk_paned_set_position(GTK_PANED(page->table_pane), table_pane_position);

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
	
	page->comment=gtk_label_new("");
	gtk_container_add(GTK_CONTAINER(comments_viewport), page->comment);
	
	page->edit_button=gtk_button_new_with_label(_("Edit comment"));
	gtk_widget_set_sensitive(page->edit_button, FALSE);
	gtk_box_pack_end(GTK_BOX(horizontal_box), page->edit_button,
		FALSE, FALSE, 0);
	
	gtk_paned_set_position(GTK_PANED(page->content_pane), 0);

	/*
	 * Pack the comments pane and the main content
	 */
	vertical_box=gtk_vbox_new(FALSE, 0);
	gtk_paned_pack1(GTK_PANED(page->content_pane), horizontal_box, TRUE, FALSE);
	gtk_paned_pack2(GTK_PANED(page->content_pane), vertical_box, FALSE, TRUE);
	
	/* Message string box is a vbox, containing one textview in most cases,
	   or two in the case of a plural message */
	original_text_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(vertical_box), original_text_scrolled_window, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vertical_box), gtk_hseparator_new(), FALSE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(original_text_scrolled_window),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	page->text_vbox = gtk_vbox_new(TRUE, 1);
	gtk_box_set_homogeneous(GTK_BOX(page->text_vbox), TRUE);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(original_text_scrolled_window), page->text_vbox);

	/* Translation box is a vbox, containing one textview in most cases,
	   or more in the case of a plural message */
	translation_text_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(vertical_box), translation_text_scrolled_window, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(translation_text_scrolled_window),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	page->trans_vbox = gtk_vbox_new(TRUE, 1);
	gtk_box_set_homogeneous(GTK_BOX(page->trans_vbox), TRUE);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(translation_text_scrolled_window), page->trans_vbox);

	/*
	 * Tie up callback for 'comments' button
	 */
	g_signal_connect(G_OBJECT(page->edit_button), "clicked",
			 G_CALLBACK(gtranslator_edit_comment_dialog), NULL);

	/*
	 * If required, set up the messages table
	 */	
	if(GtrPreferences.show_messages_table)
		gtranslator_page_show_messages_table(page);

	return page;
}

void gtranslator_page_show_messages_table(GtrPage *page) {
	GtkWidget *messages_table_scrolled_window = gtk_scrolled_window_new(NULL, NULL);

	page->messages_table = gtranslator_messages_table_new();
	gtranslator_messages_table_populate(page->messages_table, page->po->messages);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(messages_table_scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(messages_table_scrolled_window), page->messages_table->widget);
		
	gtk_paned_pack1(GTK_PANED(page->table_pane), messages_table_scrolled_window, FALSE, TRUE);
	gtk_paned_pack2(GTK_PANED(page->table_pane), page->content_pane, FALSE, TRUE);
}

void gtranslator_page_hide_messages_table(GtrPage *page) {
	// FIXME
}

gboolean gtranslator_page_autosave(GtrPage *page) {
	char *filename;
	gchar *autosave_filename;
	GError *error = NULL;
	
	/*
	 * As the file didn't change, we don't need to autosave it, but
	 *  the timeout function must still return TRUE for getting it's
	 *   periodic sense.
	 */
	if(!page->po->file_changed) return TRUE;

	/*
	 * OK, save the file to an autosave file
	 */
	filename = g_strdup(page->po->filename);
	autosave_filename = g_strdup_printf("%s/.%s.autosave.%ld",
		dirname(filename),
		basename(filename),
		(long int)getpid());
	g_free(filename);
	gtranslator_save_file(page->po, autosave_filename, &error);
	g_free(autosave_filename);
		
	if(error != NULL) {
		GtkWidget *dialog;
		dialog = gtk_message_dialog_new(
			GTK_WINDOW(gtranslator_application),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_WARNING,
			GTK_BUTTONS_OK,
			_("Error autosaving file: %s"), error->message);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		g_clear_error(&error);
		return FALSE;
	}
		
	return TRUE;

}

void gtranslator_page_dirty(GtkTextBuffer *textbuffer, gpointer user_data) {
	/* Unpack the page pointer */
	GtrPage *page;

	g_assert(user_data != NULL);
	
	page = (GtrPage*)user_data;
	page->po->file_changed = TRUE;
	
	// TODO: make notebook tab go red with an asterisk to mark an unsaved page
	
	gtranslator_actions_enable(ACT_SAVE, ACT_REVERT);
}
