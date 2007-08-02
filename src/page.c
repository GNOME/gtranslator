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
#include "draw-spaces.h"
#include "menus.h"
#include "page.h"
#include "dialogs.h"
#include "prefs.h"
#include "gui.h"
#include "view.h"

/*Variables*/
#define GLADE_CONTENT_PANE "content_pane"
#define GLADE_TABLE_PANE "table_pane"
#define GLADE_NOTEBOOK_PANE "notebook_pane"
#define GLADE_COMBOBOX_PANE "combobox_pane"
#define GLADE_COMMENT "comment"
#define GLADE_EDIT_COMMENT "edit_comment"
#define GLADE_TEXT_NOTEBOOK "text_notebook"
#define GLADE_TEXT_MSGID "text_msgid"
#define GLADE_TEXT_MSGID_PLURAL "text_msgid_plural"
#define GLADE_TRANS_NOTEBOOK "trans_notebook"
#define GLADE_TRANS_MSGSTR "trans_msgstr"
/*Status widgets*/
#define GLADE_TRANSLATED "radiobutton_translated"
#define GLADE_FUZZY "radiobutton_fuzzy"
#define GLADE_UNTRANSLATED "radiobutton_untranslated"

/*
 * The currently active page
 */
GtrPage *current_page;

static GtkWidget *
gtranslator_page_append_page(const gchar *tab_label,
			     GtkWidget *notebook)
{
	GtkWidget *scroll;
	GtkWidget *label;
	GtkWidget *widget;
	
	label = gtk_label_new(tab_label);
	
	scroll = gtk_scrolled_window_new(NULL, NULL);
	
	widget = gtranslator_view_new();
	
	g_signal_connect(widget, "event-after",
			 G_CALLBACK(on_event_after), NULL);
	
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll),
					      widget);
	
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	
	gtk_widget_show_all(scroll);
	
	
	
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll, label);
	return widget;
}

/*
 * Set up the widgets to display the given po file
 */
void 
gtranslator_page_new(GtrPo *po)
{
	GtrPage *page;
	gint i = 0;
	gchar *label;
	
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
    	page->combobox_pane = glade_xml_get_widget(glade, GLADE_COMBOBOX_PANE);
	

	/*
	 * Set up the scrolling window for the comments display
	 */

	page->comment = glade_xml_get_widget(glade, GLADE_COMMENT);
	
	page->edit_comment = glade_xml_get_widget(glade, GLADE_EDIT_COMMENT);
	
	gtk_paned_set_position(GTK_PANED(page->content_pane), 0);


	/*
	 * Orignal text widgets
	 */
	page->text_notebook = glade_xml_get_widget(glade, GLADE_TEXT_NOTEBOOK);
	page->text_msgid = gtranslator_page_append_page("Singular", page->text_notebook);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(page->text_msgid), FALSE);
	page->text_msgid_plural = gtranslator_page_append_page("Plural", page->text_notebook);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(page->text_msgid_plural), FALSE);
	
	/*
	 * Translation widgets
	 */
	page->trans_notebook = glade_xml_get_widget(glade, GLADE_TRANS_NOTEBOOK);
	do{
		label = g_strdup_printf("Plural %d", i+1);
		page->trans_msgstr[i] = gtranslator_page_append_page(label,
								     page->trans_notebook);
		i++;
		g_free(label);
	}while(i < (gint)GtrPreferences.nplurals);

	/*
	 * Status radiobuttons
	 */
	page->translated = glade_xml_get_widget(glade, GLADE_TRANSLATED);
	page->fuzzy = glade_xml_get_widget(glade, GLADE_FUZZY);
	page->untranslated = glade_xml_get_widget(glade, GLADE_UNTRANSLATED);

	/*
	 * Tie up callback for 'comments' button
	 */
	g_signal_connect(G_OBJECT(page->edit_comment), "clicked",
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
	
	/*
	 * Check if we'd to use special styles.
	 * Set the own specs for the style.
	 */ 
	gtranslator_set_style(page->text_msgid, 0);
	gtranslator_set_style(page->text_msgid_plural, 0);
	i = 0;
	do
	{
		gtranslator_set_style(page->trans_msgstr[i], 1);
		i++;
	}while(i < (gint)GtrPreferences.nplurals);
	gtk_widget_grab_focus(page->trans_msgstr[0]);
	
	current_page = page;
}

void 
gtranslator_page_show_messages_table(GtrPage *page) 
{

	page->messages_table = gtranslator_messages_table_new();
	gtranslator_messages_table_populate(page->messages_table, page->po->messages);
}

/*void
gtranslator_set_up_combobox(GtrPage *page)
{
	GtkWidget *combo;
	GtkTreeIter iter;
	GtkListStore* store;
    	gchar data[]{
		"Message table",
		"Translation memories"
	    }
	
	enum {
		COLUMN_STRING,
		N_COLUMNS
	};
	
	
	
	combo = glade_xml_get_widget(glade, name);
									  
	store = gtk_list_store_new(1, G_TYPE_STRING);
		
	for(i = 0; i < 2; i++)
    {
		gtk_list_store_append(store; &iter);
		gtk_list_store_set(store,&iter,0,data[i],-1);
    }
    	
									  
	gtk_combo_box_set_model(GTK_COMBO_BOX(combo), GTK_TREE_MODEL(store));
	gtk_combo_box_entry_set_text_column(GTK_COMBO_BOX_ENTRY(combo), 0);

	if (value)
		gtk_entry_set_text(GTK_ENTRY(GTK_BIN(combo)->child), value);
	
	g_signal_connect(GTK_ENTRY(GTK_BIN(combo)->child), "changed",
			 G_CALLBACK(callback), user_data);
	return combo;
}*/

void 
gtranslator_page_hide_messages_table(GtrPage *page) 
{
	// FIXME
}

gboolean 
gtranslator_page_autosave(GtrPage *page) 
{
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
		//Error message
	    	gtranslator_show_message(_("Error autosaving file:"), error->message);
		g_clear_error(&error);
		return FALSE;
	}
		
	return TRUE;

}

void
gtranslator_page_dirty(GtkTextBuffer *textbuffer,
		       gpointer user_data) 
{
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
