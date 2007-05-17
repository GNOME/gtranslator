/*
 * (C) 2000-2007 	Fatih Demir <kabalak@kabalak.net>
 *			Ross Golder <ross@golder.org>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Peeter Vois <peeter@kabalak.net>
 *			Thomas Ziehmer <thomas@kabalak.net>
 *			Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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
#include "menus.h"
#include "page.h"
#include "dialogs.h"
#include "prefs.h"
#include "gui.h"

#include <libgen.h>

//#define GLADE_PAGE_PATH "../data/glade/tab.glade"
/*Variables*/
#define GLADE_CONTENT_PANE "content_pane"
#define GLADE_TABLE_PANE "table_pane"
#define GLADE_COMMENT "comment"
#define GLADE_EDIT_BUTTON "edit_button"
#define GLADE_TEXT_NOTEBOOK "text_notebook"
#define GLADE_TEXT_MSGID "text_msgid"
#define GLADE_TEXT_MSGID_PLURAL "text_msgid_plural"
#define GLADE_TRANS_NOTEBOOK "trans_notebook"
#define GLADE_TRANS_MSGSTR "trans_msgstr"
#define GLADE_TRANS_MSGSTR_PLURAL "trans_msgstr_plural"
/*Status widgets*/
#define GLADE_TRANSLATED "radiobutton_translated"
#define GLADE_FUZZY "radiobutton_fuzzy"
#define GLADE_UNTRANSLATED "radiobutton_untranslated"

/*
 * The currently active pages
 */
//GList *pages;

/*
 * The currently active page
 */
GtrPage *current_page;

/*
 * Set up the widgets to display the given po file
 */
void gtranslator_page_new(GtrPo *po)
{
	GtrPage *page;
	
	g_return_if_fail(po!=NULL);

	/* Allocate a new page */
	page = g_new0(GtrPage, 1);
	page->po = po;
		

	/*
	 * Set up a document view structure to contain the widgets related
	 * to this file
	 */
	page->content_pane = glade_xml_get_widget(glade, GLADE_CONTENT_PANE);
	page->table_pane = glade_xml_get_widget(glade, GLADE_TABLE_PANE);
	

	/*
	 * Set up the scrolling window for the comments display
	 */	

	page->comment = glade_xml_get_widget(glade, GLADE_COMMENT);
	
	page->edit_button = glade_xml_get_widget(glade, GLADE_EDIT_BUTTON);
	
	gtk_paned_set_position(GTK_PANED(page->content_pane), 0);


	/* Message string box is a vbox, containing one textview in most cases,
	   or two in the case of a plural message */
	page->text_notebook = glade_xml_get_widget(glade, GLADE_TEXT_NOTEBOOK);
	page->text_msgid = glade_xml_get_widget(glade, GLADE_TEXT_MSGID);
	page->text_msgid_plural = glade_xml_get_widget(glade, GLADE_TEXT_MSGID_PLURAL);
	
	
	/* Translation box is a vbox, containing one textview in most cases,
	   or more in the case of a plural message */
	page->trans_notebook = glade_xml_get_widget(glade, GLADE_TRANS_NOTEBOOK);
	page->trans_msgstr = glade_xml_get_widget(glade, GLADE_TRANS_MSGSTR);
	page->trans_msgstr_plural = glade_xml_get_widget(glade, GLADE_TRANS_MSGSTR_PLURAL);

	/*Status radiobuttons*/
	page->translated = glade_xml_get_widget(glade, GLADE_TRANSLATED);
	page->fuzzy = glade_xml_get_widget(glade, GLADE_FUZZY);
	page->untranslated = glade_xml_get_widget(glade, GLADE_UNTRANSLATED);

	/*
	 * Tie up callback for 'comments' button
	 */
	g_signal_connect(G_OBJECT(page->edit_button), "clicked",
			 G_CALLBACK(gtranslator_edit_comment_dialog), NULL);

	/*
	 * If required, set up the messages table and set pane position
	 */	
	if(GtrPreferences.show_messages_table)
	{
		table_pane_position=gtranslator_config_get_int("interface/table_pane_position");
		gtk_paned_set_position(GTK_PANED(page->table_pane), table_pane_position);
		gtranslator_page_show_messages_table(page);
	}else
		gtk_paned_set_position(GTK_PANED(page->table_pane), 0);

	gtk_widget_show(page->table_pane);
	current_page = page;
	
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
	gchar *filename;
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
	
	//Enable save and revert items
	gtk_widget_set_sensitive(gtranslator_menuitems->save, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->revert, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->t_save, TRUE);
}
