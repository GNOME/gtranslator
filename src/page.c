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

#include "page.h"
#include "dialogs.h"
#include "prefs.h"

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
	
	gchar 	*title;
	
	g_return_if_fail(po!=NULL);

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
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(original_text_scrolled_window), page->text_vbox);

	/* Translation box is a vbox, containing one textview in most cases,
	   or more in the case of a plural message */
	translation_text_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(vertical_box), translation_text_scrolled_window, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(translation_text_scrolled_window),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	page->trans_vbox = gtk_vbox_new(TRUE, 1);
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
	{
		GtkWidget *messages_table_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
		page->messages_tree = (GtkWidget*)gtranslator_messages_table_new();
		gtk_container_add(GTK_CONTAINER(messages_table_scrolled_window), page->messages_tree);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(messages_table_scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		
		gtk_paned_pack1(GTK_PANED(page->table_pane), messages_table_scrolled_window, FALSE, TRUE);
		gtk_paned_pack2(GTK_PANED(page->table_pane), page->content_pane, FALSE, TRUE);
	}

	return page;
}
