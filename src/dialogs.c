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

#include "about.h"
#include "actions.h"
#include "bookmark.h"
#include "dialogs.h"
#include "find.h"
#include "gui.h"
#include "learn.h"
#include "message.h"
#include "messages-table.h"
#include "nautilus-string.h"
#include "open.h"
#include "parse.h"
#include "prefs.h"
#include "replace.h"
#include "runtime-config.h"
#include "translator.h"
#include "utils.h"
#include "utils_gui.h"

#include <string.h>
#include <locale.h>
#include <gnome.h>
#include <libgnomeui/libgnomeui.h>

/*
 * Functions to be used only internally in this file
 */
static void gtranslator_go_to_dialog_clicked(GtkDialog * dialog, gint button,
					     gpointer data);
static void match_case_toggled(GtkWidget * widget, gpointer useless);
static void ih_toggled(GtkWidget *widget, gpointer useless);

/* Responses for the replace dialog */
typedef enum {
	GTR_REPLACE_ONCE,
	GTR_REPLACE_ALL
} GtrReplaceDialogResponse;

/* Responses for the query dialog */
typedef enum {
	GTR_RESPONSE_QUERY,
	GTR_RESPONSE_QUERY_CONTENT
} GtrQueryDialogResponse;

/*
 * The open URI dialog signal function:
 */ 
void gtranslator_open_uri_dialog_clicked(GtkDialog *dialog, gint button,
	gpointer entrydata);

void gtranslator_dialog_show(GtkWidget ** dlg, const gchar * wmname)
{
	if (wmname != NULL)
		gtk_window_set_wmclass(GTK_WINDOW(*dlg), wmname, PACKAGE_NAME);
	g_signal_connect(G_OBJECT(*dlg), "destroy",
			 G_CALLBACK(gtk_widget_destroyed), dlg);

	gtk_widget_show_all(*dlg);
}

/*
 * The "Open file" dialog.
 */
void gtranslator_open_file_dialog(GtkWidget * widget, gpointer useless)
{
	static GtkWidget *dialog = NULL;
	if (!gtranslator_should_the_file_be_saved_dialog()) {
		if (dialog)
			gtk_widget_destroy(dialog);
		return;
	}
	
	if(dialog != NULL) {
		gtk_window_present(GTK_WINDOW(dialog));
		return;
	}
	dialog = gtk_file_selection_new(_("gtranslator -- open po file"));
	
	g_signal_connect(G_OBJECT(GTK_FILE_SELECTION(dialog)->ok_button),
			   "clicked", GTK_SIGNAL_FUNC(gtranslator_parse_the_file_from_file_dialog),
			   (gpointer) dialog);
	g_signal_connect_swapped(G_OBJECT
				 (GTK_FILE_SELECTION(dialog)->cancel_button),
				 "clicked",
				 G_CALLBACK(gtk_widget_destroy),
				 G_OBJECT(dialog));

	gtranslator_file_dialogs_set_directory(&dialog);
	
	/*
	 * Make the dialog transient, gtranslator_dialog_show does not do it
	 *  because it is not a GtkDialog.
	 */
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(gtranslator_application));
	gtranslator_dialog_show(&dialog, NULL );
}

/*
 * "Save as" dialog.
 */
void gtranslator_save_file_as_dialog(GtkWidget * widget, gpointer useless)
{
	static GtkWidget *dialog = NULL;
	if(dialog != NULL) {
		gtk_window_present(GTK_WINDOW(dialog));
		return;
	}

	/*
	 * Let the user use the file selector to decide where to save it
	 */
	dialog = gtk_file_selection_new(_("gtranslator -- save file as.."));

	g_signal_connect(G_OBJECT(GTK_FILE_SELECTION(dialog)->ok_button),
		"clicked",
		GTK_SIGNAL_FUNC(gtranslator_save_file_dialog),
		(gpointer) dialog);

	g_signal_connect_swapped(G_OBJECT
		(GTK_FILE_SELECTION(dialog)->cancel_button),
		"clicked",
		G_CALLBACK(gtk_widget_destroy),
		G_OBJECT(dialog));

	gtranslator_file_dialogs_set_directory(&dialog);
	
	/*
	 * Make the dialog transient.
	 */
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(gtranslator_application));
	gtranslator_dialog_show(&dialog, "gtranslator -- save");
}

/* 
 * If file was changed, asks the user, what to do, according to response, saves
 *  the file or not, and returns TRUE. If neither of YES and NO was pressed,
 *   returns FALSE.
 */
gboolean gtranslator_should_the_file_be_saved_dialog(void)
{
	GtkWidget *dialog;
	gint reply;

	if (!po || (!po->file_changed))
		return TRUE;
	dialog=gtk_message_dialog_new(
		GTK_WINDOW(gtranslator_application),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_WARNING,
		GTK_BUTTONS_NONE,
		_("File %s\nwas changed. Save?"),
		po->filename);
	gtk_dialog_add_buttons(
		GTK_DIALOG(dialog),
		GTK_STOCK_CANCEL,
		GTK_RESPONSE_CANCEL,
		_("Don't save"),
		GTK_RESPONSE_REJECT,
		GTK_STOCK_SAVE,
		GTK_RESPONSE_ACCEPT,
		NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);

	reply = gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);

	if (reply == GTK_RESPONSE_ACCEPT) {
		gtranslator_save_current_file_dialog(NULL, NULL);
		return TRUE;
	}
	if (reply == GTK_RESPONSE_REJECT) {
		po->file_changed = FALSE;
		return TRUE;
	}
	return FALSE;
}


/*
 * Display a small text widget with an editable content.
 */
void gtranslator_edit_comment_dialog(GtkWidget *widget, gpointer useless)
{
	static GtkWidget *dialog=NULL;

	GtkWidget 	*inner_table;
	GtkWidget 	*comment_box;
	GtrComment 	*comment;
	
	gint reply=0;

	dialog=gtk_dialog_new_with_buttons(
		_("gtranslator -- edit comment"),
		GTK_WINDOW(gtranslator_application),
		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OK, GTK_RESPONSE_OK,
		NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);

	/*
	 * Create and pack the inner_table into the dialog.
	 */
	inner_table=gtk_table_new(1, 2, FALSE);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), 
		inner_table);

	/*
	 * Get the current comment from the current message.
	 */
	comment=GTR_COMMENT(GTR_MSG(po->current->data)->comment);
	g_return_if_fail(comment!=NULL);

	/*
	 * Use our util. function to get a labeled text box into the dialog.
	 */
	comment_box=gtranslator_utils_attach_text_with_label(inner_table, 1,
		_("Comment:"), comment->comment, NULL);

	/*
	 * The window should be resizable and somehow bigger then normally.
	 */
	gtk_window_set_resizable(GTK_WINDOW(dialog), TRUE);
	gtk_window_set_default_size(GTK_WINDOW(dialog), 380, 200);
	
	gtranslator_dialog_show(&dialog, _("gtranslator -- edit comment"));
	
	reply=gtk_dialog_run(GTK_DIALOG(dialog));

	/*
	 * Now operate on the contents as the user pressed "Ok".
	 */
	if(reply==GTK_RESPONSE_OK)
	{
		gchar		 *comment_dialog_contents;
		gchar		**checkarray;

		GString		  *comment_string=g_string_new("");
		
		gint		  array_pos=0;

		GtkTextBuffer   *buff;
		GtkTextIter     start, end;

		/*
		 * Get the comment box contents.
		 */
		buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(comment_box));
		gtk_text_buffer_get_bounds(buff, &start, &end);
		comment_dialog_contents = gtk_text_buffer_get_text(buff, &start, &end, FALSE);
		g_return_if_fail(comment_dialog_contents!=NULL);
		
		/*
		 * Check if the user did change anything in the comment_box.
		 */
		if(!strcmp(comment_dialog_contents, 
			GTR_COMMENT(comment)->comment))
		{
			/*
			 * If the contents are still the same simply return.
			 */
			gtk_widget_destroy(GTK_WIDGET(dialog));
			return;
		}

		/*
		 * Split the comment up and check every part of it.
		 */
		checkarray=g_strsplit(comment_dialog_contents, "\n", 0);

		while(checkarray[array_pos]!=NULL)
		{
			if(checkarray[array_pos][0]!='#')
			{
				comment_string=g_string_append(comment_string, "# ");
				comment_string=g_string_append(comment_string,
					checkarray[array_pos]);
			}
			else
			{
				comment_string=g_string_append(comment_string,
					checkarray[array_pos]);
			}

			comment_string=g_string_append(comment_string, "\n");
			
			array_pos++;
		}

		/*
		 * Update the GtrComment and activate the GUI elements for the save
		 *  function.
		 */
		gtranslator_comment_update(&comment, comment_string->str);
		gtranslator_actions_enable(ACT_SAVE);

		/*
		 * Free all the used stuff here.
		 */
		g_free(comment_dialog_contents);
		g_strfreev(checkarray);
		g_string_free(comment_string, TRUE);
		
		/*
		 * Set the label contents in the GUI.
		 */
		gtk_label_set_text(GTK_LABEL(document_view->comment),
			comment->pure_comment);
	}
	
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

/*
 * The ultimate dialog -- it should be really idiot-proof to avoid unwanted loss of
 *  work.
 */
void gtranslator_remove_all_translations_dialog(GtkWidget *widget, gpointer useless)
{
	static GtkWidget *dialog=NULL;
	
	gint 	 reply=0;

	/*
	 * Translators: This text should really be VERY clear -- the translator/user
	 *  is about to remove ALL translations from the po file!
	 */
	dialog=gtk_message_dialog_new(
		GTK_WINDOW(gtranslator_application),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_WARNING,
		GTK_BUTTONS_YES_NO,
		_("Should ALL translations from `%s' be removed?"),
		po->filename);

	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_YES);

	/*
	 * Run the dialog!
	 */
	reply=gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(GTK_WIDGET(dialog));

	if(reply==GTK_RESPONSE_YES)
	{
		/*
		 * The user wanted it so, so perform the removal.
		 */
		gtranslator_remove_all_translations(po);
	}
}

/*
 * Set the current/last used directory up for the given file dialog.
 */
void gtranslator_file_dialogs_set_directory(GtkWidget **fileselection)
{
	gchar *directory;
	
	if(po && po->filename)
	{
		directory=g_path_get_dirname(po->filename);
		gtk_file_selection_complete(GTK_FILE_SELECTION(*fileselection),
			directory);
	}
	else
	{
		directory=gtranslator_config_get_string("informations/last_directory");

		if(directory && g_file_test(directory, G_FILE_TEST_IS_DIR))
		{
			gtk_file_selection_complete(
				GTK_FILE_SELECTION(*fileselection), directory);
		}
		else
		{
			directory=g_strdup(g_get_home_dir());
			gtk_file_selection_complete(
				GTK_FILE_SELECTION(*fileselection), directory);
		}
	}

	g_free(directory);
}

/*
 * Store the given filename's directory for our file dialogs completion-routine.
 */
void gtranslator_file_dialogs_store_directory(const gchar *filename)
{
	gchar *directory;

	g_return_if_fail(filename!=NULL);
	
	directory=g_path_get_dirname(filename);
	
	gtranslator_config_set_string("informations/last_directory", directory);

	g_free(directory);
}

/*
 * The "Go to" functions.
 */
static void gtranslator_go_to_dialog_clicked(GtkDialog * dialog, gint button,
					     gpointer data)
{
	if (button == GTK_RESPONSE_OK)
	{
		GtkSpinButton *spin = GTK_SPIN_BUTTON(data);
		guint number;
		
		gtk_spin_button_update(spin);
		number = gtk_spin_button_get_value_as_int(spin) - 1;
		gtranslator_message_go_to_no(GTK_WIDGET(dialog),
					     GUINT_TO_POINTER(number));
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

void gtranslator_go_to_dialog(GtkWidget * widget, gpointer useless)
{
	static GtkWidget *dialog = NULL;
	static GtkObject *adjustment;
	GtkWidget *spin, *label;

	if (dialog)
		gtk_adjustment_set_value(GTK_ADJUSTMENT(adjustment),
					 g_list_position(po->messages,
							 po->current) + 1);
	if(dialog != NULL) {
		gtk_window_present(GTK_WINDOW(dialog));
		return;
	}
	dialog = gtk_dialog_new_with_buttons(
		_("gtranslator -- go to"),
		GTK_WINDOW(gtranslator_application),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE,
		GTK_STOCK_JUMP_TO, GTK_RESPONSE_OK,
		NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);

	label = gtk_label_new(_("Go to message number:"));
	
	/*
	 * Display current message number and let it change from first to last .
	 */
	adjustment =
	    gtk_adjustment_new(g_list_position(po->messages, po->current) + 1,
	    		       1, g_list_length(po->messages), 1, 10, 10);
	spin = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1, 0);
	gtk_spin_button_set_update_policy(GTK_SPIN_BUTTON(spin),
					  GTK_UPDATE_IF_VALID);
	
	/*
	 * Pack the label & the Gnome entry into the dialog.
	 */
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label,
			   FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), spin,
			   FALSE, FALSE, 0);
	
	g_signal_connect(G_OBJECT(dialog), "response",
			 G_CALLBACK(gtranslator_go_to_dialog_clicked),
			   spin);
	gtk_window_set_focus(GTK_WINDOW(dialog), spin);
	gtranslator_dialog_show(&dialog, "gtranslator -- goto");
}

static void match_case_toggled(GtkWidget * widget, gpointer useless)
{
	GtrPreferences.match_case =
	    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	gtranslator_update_regex_flags();
	gtranslator_config_set_bool("find/case_sensitive",
			      GtrPreferences.match_case);
}

static void ih_toggled(GtkWidget *widget, gpointer useless)
{
	GtrPreferences.ignore_hotkeys =
	    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	gtranslator_config_set_bool("find/ignore_hotkeys",
			      GtrPreferences.ignore_hotkeys);
}

void gtranslator_find_dialog(GtkWidget * widget, gpointer useless)
{
	static GtkWidget *dialog = NULL;

	GtkWidget *label, *findy, *subfindy, *match_case;
	GtkWidget *ih_button, *sbox, *fi_english, *fi_translation, *fi_comments, *fi_label;

	gint reply=0;

	if(dialog != NULL) {
		gtk_window_present(GTK_WINDOW(dialog));
		return;
	}

	dialog = gtk_dialog_new_with_buttons(_("Find in the po file"),
					     GTK_WINDOW(gtranslator_application),
					     GTK_DIALOG_DESTROY_WITH_PARENT,
					     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					     GTK_STOCK_FIND, GTK_RESPONSE_OK,
					     NULL);

	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);

	label = gtk_label_new(_("Enter search string:"));
	findy = gnome_entry_new("FINDY");
	subfindy = gnome_entry_gtk_entry(GNOME_ENTRY(findy));
	gtk_entry_set_activates_default(GTK_ENTRY(subfindy), TRUE);
	
	match_case = gtk_check_button_new_with_label(_("Case sensitive"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(match_case),
				     GtrPreferences.match_case);

	sbox=gtk_hbox_new(FALSE, 3);
	fi_label=gtk_label_new(_("Find in:"));

	gtk_box_pack_start(GTK_BOX(sbox), GTK_WIDGET(fi_label), FALSE, TRUE, 2);

	fi_comments=gtk_check_button_new_with_label(_("Comments"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fi_comments), GtrPreferences.fi_comments);

	fi_english=gtk_check_button_new_with_label(_("English"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fi_english), GtrPreferences.fi_english);

	fi_translation=gtk_check_button_new_with_label(_("Translation"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fi_translation), GtrPreferences.fi_translation);

	gtk_box_pack_start(GTK_BOX(sbox), GTK_WIDGET(fi_comments), FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(sbox), GTK_WIDGET(fi_english), FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(sbox), GTK_WIDGET(fi_translation), FALSE, FALSE, 2);
	
	/*
	 * Translators: this means that the hotkeys ("_" etc.) are ignored
	 *  during the search action.
	 */
	ih_button=gtk_check_button_new_with_label(_("Ignore hotkeys"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ih_button),
    			   GtrPreferences.ignore_hotkeys);

	/*
	 * Pack the single elements into the dialog and the box in da box.
	 */
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label,
			   FALSE, FALSE, 3);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), findy,
			   FALSE, FALSE, 3);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), match_case,
			   FALSE, FALSE, 3);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), ih_button,
			   FALSE, FALSE, 3);

	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), sbox,
			   FALSE, FALSE, 3);
	
	g_signal_connect(G_OBJECT(match_case), "toggled",
			 G_CALLBACK(match_case_toggled), NULL);
	g_signal_connect(G_OBJECT(ih_button), "toggled",
			 G_CALLBACK(ih_toggled), NULL);
	gtk_window_set_focus(GTK_WINDOW(dialog), 
		gnome_entry_gtk_entry(GNOME_ENTRY(findy)));
	
	gtranslator_dialog_show(&dialog, "gtranslator -- find");
	
	reply=gtk_dialog_run(GTK_DIALOG(dialog));

	if(reply==GTK_RESPONSE_CANCEL || reply==GTK_RESPONSE_CLOSE)
	{
		gtk_widget_destroy(GTK_WIDGET(dialog));
	}
	else
	{
		GtkWidget	*entry=NULL;
		gchar 		*find_text;

		entry=gnome_entry_gtk_entry(GNOME_ENTRY(findy));

		find_text=g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
		g_return_if_fail(find_text!=NULL);

		if(GtrPreferences.ignore_hotkeys)
		{
			gchar   *newstr;

			newstr=nautilus_str_strip_chr(find_text, GtrPreferences.hotkey_char);
			g_free(find_text);
			find_text=newstr;
		}

		GtrPreferences.fi_comments=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fi_comments));
		GtrPreferences.fi_english=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fi_english));
		GtrPreferences.fi_translation=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fi_translation));


		gtranslator_config_set_bool("find/find_in_comments", GtrPreferences.fi_comments);
		gtranslator_config_set_bool("find/find_in_english", GtrPreferences.fi_english);
		gtranslator_config_set_bool("find/find_in_translation", GtrPreferences.fi_translation);
		
		/*
		 * Save the last search settings for a future search.
		 */
		gtranslator_find(NULL, find_text, GtrPreferences.fi_comments, 
			GtrPreferences.fi_english, GtrPreferences.fi_translation);
		gtranslator_actions_enable(ACT_FIND_AGAIN, ACT_END);

		gtk_widget_destroy(GTK_WIDGET(dialog));
	}
}

/*
 * The replace dialog -- based on the find dialog.
 */
void gtranslator_replace_dialog(GtkWidget *widget, gpointer useless)
{
	int reply;
	static GtkWidget *dialog = NULL;

	GtkWidget *label, *sndlabel, *replace_in_label;
	GtkWidget *findy, *replacy;
	GtkWidget *rbox, *ri_english, *ri_translation, *ri_comments;

	if(dialog != NULL) {
		gtk_window_present(GTK_WINDOW(dialog));
		return;
	}

	dialog=gtk_dialog_new_with_buttons(
		_("gtranslator -- replace"),
		GTK_WINDOW(gtranslator_application),
		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE,
		_("Replace"), GTR_REPLACE_ONCE,
		_("Replace all"), GTR_REPLACE_ALL,
		NULL);

	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTR_REPLACE_ONCE);
	
	label=gtk_label_new(_("String to replace:"));
	findy=gnome_entry_new("REPLACE_THIS");

	sndlabel=gtk_label_new(_("Replace string:"));
	replacy=gnome_entry_new("REPLACE_WITH_THIS");

	rbox=gtk_hbox_new(FALSE, 3);

	replace_in_label=gtk_label_new(_("Replace in:"));
	gtk_box_pack_start(GTK_BOX(rbox), GTK_WIDGET(replace_in_label), FALSE, TRUE, 2);

	ri_comments=gtk_check_button_new_with_label(_("Comments"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ri_comments), GtrPreferences.ri_comments);

	ri_english=gtk_check_button_new_with_label(_("English"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ri_english), GtrPreferences.ri_english);

	ri_translation=gtk_check_button_new_with_label(_("Translation"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ri_translation), GtrPreferences.ri_translation);

	gtk_box_pack_start(GTK_BOX(rbox), GTK_WIDGET(ri_comments), FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(rbox), GTK_WIDGET(ri_english), FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(rbox), GTK_WIDGET(ri_translation), FALSE, FALSE, 2);
	
	/*
	 * Pack the single elements into the dialog.
	 */
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label,
		FALSE, FALSE, 3);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), findy, 
		FALSE, FALSE, 3);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), sndlabel,
		FALSE, FALSE, 3);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), replacy,
		FALSE, FALSE, 3);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), rbox,
		FALSE, FALSE, 3);
	
	gtk_window_set_focus(GTK_WINDOW(dialog), 
		gnome_entry_gtk_entry(GNOME_ENTRY(findy)));

	gtranslator_dialog_show(&dialog, "gtranslator -- replace");

 SHOW_DIALOG:
	reply=gtk_dialog_run(GTK_DIALOG(dialog));

	if(reply != GTR_REPLACE_ONCE && reply != GTR_REPLACE_ALL)
	{
		gtk_widget_destroy(GTK_WIDGET(dialog));
	}
	else
	{
		gchar *findme, *replaceme;
		GtrReplace *rpl;

		GtrPreferences.ri_comments=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ri_comments));
		GtrPreferences.ri_english=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ri_english));
		GtrPreferences.ri_translation=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ri_translation));

		gtranslator_config_set_bool("replace/replace_in_comments", GtrPreferences.ri_comments);
		gtranslator_config_set_bool("replace/replace_in_english", GtrPreferences.ri_english);
		gtranslator_config_set_bool("replace/replace_in_translation", GtrPreferences.ri_translation);
		
		findme=gtk_editable_get_chars(GTK_EDITABLE(
			gnome_entry_gtk_entry(GNOME_ENTRY(findy))), 0, -1);

		replaceme=gtk_editable_get_chars(GTK_EDITABLE(
			gnome_entry_gtk_entry(GNOME_ENTRY(replacy))), 0, -1);

		if(!findme || strlen(findme)<=0)
		{
			GtkWidget *message_dlg;
			
			message_dlg = gtk_message_dialog_new (
				GTK_WINDOW (gtranslator_application),
				GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_WARNING,
				GTK_BUTTONS_OK,
				_("Please enter a string to replace!"));
			
			gtk_dialog_set_default_response (GTK_DIALOG (message_dlg), GTK_RESPONSE_OK);
			
			gtk_window_set_resizable (GTK_WINDOW (message_dlg), FALSE);
			
			gtk_dialog_run (GTK_DIALOG (message_dlg));
			gtk_widget_destroy (message_dlg);

			goto SHOW_DIALOG;
			
			return;
		}

		if(reply==GTR_REPLACE_ALL)
		{
			rpl=gtranslator_replace_new(findme, replaceme, TRUE, 0,
				GtrPreferences.ri_comments, GtrPreferences.ri_english,
				GtrPreferences.ri_translation);
		}
		else
		{
			rpl=gtranslator_replace_new(findme, replaceme, FALSE, 
				g_list_position(po->messages, po->current),
				GtrPreferences.ri_comments, GtrPreferences.ri_english,
				GtrPreferences.ri_translation);
		}

		gtk_widget_destroy(GTK_WIDGET(dialog));

		g_free(findme);
		g_free(replaceme);
		
		gtranslator_replace_run(rpl);
	}
	
}

/* 
 * Tells the user the file is already open, and ask if they are sure
 * they want to open it.
 */
gint gtranslator_already_open_dialog(GtkWidget *widget, gpointer filename)
{
	GtkWidget *dialog;

	gint reply;

	gchar *fname;

	fname = g_strdup((gchar*)filename);

	dialog=gtk_message_dialog_new(
		GTK_WINDOW(gtranslator_application),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_WARNING,
		GTK_BUTTONS_YES_NO,
		_("The file\n"
		  "\n"
		  "   %s\n"
		  "\n"
		  "is already open in another instance of gtranslator!\n"
		  "Please close the other instance of gtranslator handling\n"
		  "this file currently to re-gain access to this file.\n"
		  "\n"
		  "Shall fool gtranslator open this file anyway ?"), 
		fname);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_NO);

	/*
	 * Run dialog and process response
	 */
	reply=gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(GTK_WIDGET(dialog));
	g_free(fname);

	return reply;
}

/* 
 * Tells the user the file has changed on disk, and ask if they wish
 * to revert.
 */
gint gtranslator_file_revert_dialog(GtkWidget *widget, gpointer filename)
{
	GtkWidget *dialog;

	gint reply;

	gchar *fname;

	fname = g_strdup((gchar*)filename);

	dialog=gtk_message_dialog_new(
		GTK_WINDOW(gtranslator_application),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_WARNING,
		GTK_BUTTONS_YES_NO,
		_("File %s\nwas changed. Do you want to revert to saved copy?"),
		fname);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_NO);

	/*
	 * Run dialog and process response
	 */
	reply=gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(GTK_WIDGET(dialog));
	g_free(fname);

	return reply;
}

/*
 * Requests for an URI to open. Uses pregiven protocol list.
 */ 
void gtranslator_open_uri_dialog(GtkWidget *widget, gpointer useless)
{
	static GtkWidget *dialog=NULL;
	GtkWidget *entry, *subentry;
	GtkWidget *label;

	if(dialog != NULL) {
		gtk_window_present(GTK_WINDOW(dialog));
		return;
	}

	dialog=gtk_dialog_new_with_buttons(
		_("gtranslator -- open from URI"),
		GTK_WINDOW(gtranslator_application),
		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
		NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);

	entry=gnome_entry_new("URI");
	subentry = gnome_entry_gtk_entry(GNOME_ENTRY(entry));
	gtk_entry_set_activates_default(GTK_ENTRY(subentry), TRUE);

	label=gtk_label_new(_("Enter URI:"));
	
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label,
		FALSE, FALSE, 0);
	
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), entry,
		FALSE, FALSE, 0);
	gtk_window_set_focus(GTK_WINDOW(dialog),
		gnome_entry_gtk_entry(GNOME_ENTRY(entry)));

	g_signal_connect(G_OBJECT(dialog), "response",
		GTK_SIGNAL_FUNC(gtranslator_open_uri_dialog_clicked), entry);
			
	gtranslator_dialog_show(&dialog, "gtranslator -- open URI");
}

/*
 * Checks the URI before it's passed to the core functions.
 */ 
void gtranslator_open_uri_dialog_clicked(GtkDialog *dialog, gint button,
	gpointer entrydata)
{
	GString *uri=g_string_new("");
	GError *error;

	if(button==GTK_RESPONSE_OK)
	{
		/*
		 * Get the URI data from the GnomeEntry.
		 */ 
		uri=g_string_append(uri, gtk_editable_get_chars(
			GTK_EDITABLE(gnome_entry_gtk_entry(entrydata)),
				0, -1));

		
		if(uri->len <= 0)
		{
			gtranslator_utils_error_dialog(_("No URI given!"));	
		}
		else
		{
				gtk_widget_destroy(GTK_WIDGET(dialog));
			if(!gtranslator_parse_main(uri->str, &error)) {
				gtranslator_utils_error_dialog(error->message);
			}
		}
	}
	else if(button==2)
	{
		gnome_app_message(GNOME_APP(gtranslator_application),
			_(
"URIs are used to locate files uniquely on different systems.\n\
The standard Internet addresses (URLs) are also URIs -- you can\n\
use them to open remote po files lying on servers with standard protocols\n\
like \"http\", \"ftp\" or any other access method supported by GnomeVFS."
"\n"
"Some example URIs for clarification (these ones are supported):\n\
http://www.gtranslator.org/remote-po/gtranslator.pot\n\
ftp://anonymous@ftp.somewhere.com/<A-REMOTE-PO-FILE>\n\n\
file:///<PO-FILE> or\n\
http://www.DOMAIN.COM/PO-FILE"));
	}
	else
	{
		gtk_widget_destroy(GTK_WIDGET(dialog));
	}

	g_string_free(uri, FALSE);
	
}

/*
 * Possibly rescue the given file.
 */
void gtranslator_rescue_file_dialog(void)
{
	GtkWidget *dialog;
	GError *error;
	gchar *original_filename;
	gint reply;
	
	/*
	 * Get the original filename for the crash-file from the prefs.
	 */
	original_filename=gtranslator_config_get_string("crash/filename");

	dialog=gtk_message_dialog_new(
		GTK_WINDOW(gtranslator_application),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_WARNING,
		GTK_BUTTONS_NONE,
		_("Open recovery file for `%s'?\n\
It was saved by gtranslator before gtranslator last closed\n\
and may contain your hard work!\n"),
		original_filename);
	
	gtk_dialog_add_buttons(GTK_DIALOG(dialog),
		/*
		 * Translators: All this is about recovering a po file after
		 *  a crash or something; postbone means that the user will
		 *   get asked the next time he starts up gtranslator again
		 *    if he/she wants to recover file.
		 */
		_("Postpone decision"),
		GTK_RESPONSE_CANCEL,
		_("Ignore recovery file"),
		GTK_RESPONSE_REJECT,
		_("Recover file"),
		GTK_RESPONSE_ACCEPT,
		NULL);

	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);

	reply = gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);

	if(reply==GTK_RESPONSE_ACCEPT)
	{
		/*
		 * Move the recovery file to the original filename and re-open
		 *  it now again.
		 */ 
		rename(gtranslator_runtime_config->crash_filename, 
			original_filename);

		if(!gtranslator_parse_main(original_filename, &error)) {
			gnome_app_warning(GNOME_APP(gtranslator_application),
				error->message);
		}
	}
	else if(reply==GTK_RESPONSE_REJECT)
	{
		/*
		 * Remove the crash recovery file.
		 */
		unlink(gtranslator_runtime_config->crash_filename);
	}

	g_free(original_filename);
}

/*
 * Asks the user if he/she does really want to use autotranslation.
 */
void gtranslator_auto_translation_dialog(void)
{
	static GtkWidget *at_dialog=NULL;
	gint 		reply;
	GError		*error;

	if(at_dialog != NULL) {
		gtk_window_present(GTK_WINDOW(at_dialog));
		return;
	}

	at_dialog=gtk_message_dialog_new(
		GTK_WINDOW(gtranslator_application),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_QUESTION,
		GTK_BUTTONS_YES_NO,
		_("Should gtranslator autotranslate the file using information\n\
from your personal learn buffer?"));

	gtk_dialog_set_default_response(GTK_DIALOG(at_dialog), GTK_RESPONSE_YES);

	gtranslator_dialog_show(&at_dialog, "gtranslator -- autotranslate?");
	reply=gtk_dialog_run(GTK_DIALOG(at_dialog));
	
	/*
	 * Only handle the "Yes" case as we do not think about the "No" case --
	 *  the user doesn't want any autotranslation.
	 */
	if(reply==GTK_RESPONSE_YES)
	{
		/*
		 * Autotranslate the missing entries.
		 */
		if(!gtranslator_learn_autotranslate(po, TRUE, &error))
		{
			gnome_app_warning(GNOME_APP(gtranslator_application),
				error->message);
		}
	}

	gtk_widget_destroy(GTK_WIDGET(at_dialog));
}

/*
 * A little more enhanced bookmark adding dialog & such like comment entering.
 */
void gtranslator_bookmark_adding_dialog(GtkWidget *widget, gpointer useless)
{
	static GtkWidget *dialog=NULL;

	GtkWidget 	*inner_table;
	GtkWidget 	*comment_box;
	
	gint reply=0;

	dialog=gtk_dialog_new_with_buttons(
		_("gtranslator -- add bookmark with comment?"),
		GTK_WINDOW(gtranslator_application),
		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OK, GTK_RESPONSE_OK,
		NULL);

	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);

	/*
	 * Create and pack the inner_table into the dialog.
	 */
	inner_table=gtk_table_new(1, 2, FALSE);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), 
		inner_table);

	/*
	 * Use our util. function to get a labeled text box into the dialog.
	 */
	comment_box=gtranslator_utils_attach_text_with_label(inner_table, 1,
		_("Comment for the bookmark:"), _("No comment"), NULL);

	/*
	 * The window should be resizable and somehow bigger then normally.
	 */
	gtk_window_set_resizable(GTK_WINDOW(dialog), TRUE);
	gtk_window_set_default_size(GTK_WINDOW(dialog), 380, 200);
	
	gtranslator_dialog_show(&dialog, _("gtranslator -- add bookmark with comment?"));
	
	reply=gtk_dialog_run(GTK_DIALOG(dialog));

	/*
	 * Now operate on the contents as the user pressed "Ok".
	 */
	if(reply==GTK_RESPONSE_OK)
	{
		gchar		*comment_dialog_contents;

		GtkTextBuffer   *buff;
		GtkTextIter     start, end;

		/*
		 * Get the comment box contents.
		 */
		buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(comment_box));
		gtk_text_buffer_get_start_iter(buff, &start);
		gtk_text_buffer_get_end_iter(buff, &end);
		comment_dialog_contents = gtk_text_buffer_get_text(buff, &start, &end, FALSE);
		g_return_if_fail(comment_dialog_contents!=NULL);

		gtranslator_bookmark_add_direct_with_comment(comment_dialog_contents);

		g_free(comment_dialog_contents);
	}
	
	gtk_widget_destroy(GTK_WIDGET(dialog));
}
