/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gmx.net>
 *			Gediminas Paulauskas <menesis@delfi.lt>
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

#include "dialogs.h"
#include "find.h"
#include <libgtranslator/preferences.h>

/*
 * Functions to be used only internally in this file
 */
static void goto_dlg_clicked(GnomeDialog * dialog, gint button,
			     gpointer adjustment);
static void match_case_toggled(GtkWidget * widget, gpointer useless);
static void find_dlg_clicked(GnomeDialog * dialog, gint button,
			     gpointer findy);

void show_nice_dialog(GtkWidget ** dlg, const gchar * wmname)
{
	if (wmname != NULL)
		gtk_window_set_wmclass(GTK_WINDOW(*dlg), wmname, "gtranslator");
	if (GNOME_IS_DIALOG(*dlg))
		gnome_dialog_set_parent(GNOME_DIALOG(*dlg), GTK_WINDOW(app1));
	#ifdef USE_WINDOW_ICON
	gnome_window_icon_set_from_file(GTK_WINDOW(*dlg), WINDOW_ICON);
	#endif
	gtk_signal_connect(GTK_OBJECT(*dlg), "destroy",
			   GTK_SIGNAL_FUNC(gtk_widget_destroyed), dlg);
	gtk_widget_show_all(*dlg);
}

/*
 * The "Open file" dialog.
 */
void open_file(GtkWidget * widget, gpointer useless)
{
	static GtkWidget *dialog = NULL;
	if (!ask_to_save_file()) {
		if (dialog)
			gtk_widget_destroy(dialog);
		return;
	}
	raise_and_return_if_exists(dialog);
	dialog = gtk_file_selection_new(_("gtranslator -- open a po-file"));
	
	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(dialog)->ok_button),
			   "clicked", GTK_SIGNAL_FUNC(parse_the_file),
			   (gpointer) dialog);
	gtk_signal_connect_object(GTK_OBJECT
				  (GTK_FILE_SELECTION(dialog)->cancel_button),
				  "clicked",
				  GTK_SIGNAL_FUNC(gtk_widget_destroy),
				  GTK_OBJECT(dialog));
	if (po && po->filename)
	{
		gchar *dir=g_dirname(po->filename);
		gtk_file_selection_complete(GTK_FILE_SELECTION(dialog), dir);
		g_free(dir);
	}
	/*
	 * Make the dialog transient, show_nice_dialog does not do it
	 *  because it is not a GnomeDialog.
	 */
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(app1));
	show_nice_dialog(&dialog, "gtranslator -- open");
}

/*
 * "Save as" dialog.
 */
void save_file_as(GtkWidget * widget, gpointer useless)
{
	static GtkWidget *dialog = NULL;
	raise_and_return_if_exists(dialog);

	/*
	 * If we do have write perms for the file we can save it under each
	 *  filename but if we don't have write perms for it, we'd try to
	 *   save it in our local directory.
	 *
	 * OR: The filename points to a copy-result po file, then we do apply
	 *  the same dialog tactics.
	 */   
	if(po->no_write_perms==FALSE||strstr(po->filename, "/.gtranslator-"))
	{
		dialog = gtk_file_selection_new(_("gtranslator -- save file as.."));
	}
	else
	{
		dialog = gtk_file_selection_new(_("gtranslator -- save local copy of file as.."));

		/*
		 * Set a local filename in the users home directory with the 
		 *  same filename as the original but with a project prefix
		 *   (e.g. "gtranslator-tr.po").
		 */  
		gtk_file_selection_set_filename(GTK_FILE_SELECTION(dialog),
			g_strdup_printf("%s/%s-%s",
				g_get_home_dir(),
				po->header->prj_name,
				g_basename(po->filename)));
	}
	
	/*
	 * gtk_file_selection_complete(GTK_FILE_SELECTION(dialog),"*.po");
	 */
	
	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(dialog)->ok_button),
			   "clicked", GTK_SIGNAL_FUNC(save_the_file),
			   (gpointer) dialog);
	gtk_signal_connect_object(GTK_OBJECT
				  (GTK_FILE_SELECTION(dialog)->cancel_button),
				  "clicked",
				  GTK_SIGNAL_FUNC(gtk_widget_destroy),
				  GTK_OBJECT(dialog));
	if (po->filename)
	{
		gchar *dir=g_dirname(po->filename);
		gtk_file_selection_complete(GTK_FILE_SELECTION(dialog), dir);
		g_free(dir);
	}
	/*
	 * Make the dialog transient.
	 */
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(app1));
	show_nice_dialog(&dialog, "gtranslator -- save");
}

/* 
 * If file was changed, asks the user, what to do, according to response, saves
 *  the file or not, and returns TRUE. If neither of YES and NO was pressed,
 *   returns FALSE.
 */
gboolean ask_to_save_file(void)
{
	GtkWidget *dialog;
	gchar *question;
	gint reply;

	if ((!file_opened) || (!po->file_changed))
		return TRUE;
	question = g_strdup_printf(_("File %s\nwas changed. Save?"),
				   po->filename);
	dialog = gnome_message_box_new(question, GNOME_MESSAGE_BOX_QUESTION,
				       GNOME_STOCK_BUTTON_YES,
				       GNOME_STOCK_BUTTON_NO,
				       GNOME_STOCK_BUTTON_CANCEL, NULL);
	show_nice_dialog(&dialog, "gtranslator -- ask");
	reply = gnome_dialog_run(GNOME_DIALOG(dialog));
	g_free(question);
	if (reply == GNOME_YES)
		save_current_file(NULL, NULL);
	else if (reply == GNOME_NO)
		po->file_changed = FALSE;
	else
		return FALSE;
	return TRUE;
}

/*
 * The "Go to" functions.
 */
static void goto_dlg_clicked(GnomeDialog * dialog, gint button,
			     gpointer adjustment)
{
	guint number = GTK_ADJUSTMENT(adjustment)->value - 1;
	if (button == GNOME_OK)
		goto_nth_msg(GTK_WIDGET(dialog), GUINT_TO_POINTER(number));
	gnome_dialog_close(dialog);
}

void goto_dlg(GtkWidget * widget, gpointer useless)
{
	static GtkWidget *dialog = NULL;
	static GtkObject *adjustment;
	GtkWidget *spin, *label;

	if (dialog)
		gtk_adjustment_set_value(GTK_ADJUSTMENT(adjustment),
					 g_list_position(po->messages,
							 po->current) + 1);
	raise_and_return_if_exists(dialog);
	dialog = gnome_dialog_new(_("gtranslator -- go to"), _("Go!"),
			     GNOME_STOCK_BUTTON_CANCEL, NULL);
	/*
	 * We want the "Go!" button to be the default.
	 */
	gnome_dialog_set_default(GNOME_DIALOG(dialog), 0);
	label = gtk_label_new(_("Go to message number:"));
	
	/*
	 * Display current message number and let it change from first to last .
	 */
	adjustment =
	    gtk_adjustment_new(g_list_position(po->messages, po->current) + 1,
	    		       1, po->length, 1, 10, 10);
	spin = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1, 0);
	gtk_spin_button_set_update_policy(GTK_SPIN_BUTTON(spin),
					  GTK_UPDATE_IF_VALID);
	gnome_dialog_editable_enters(GNOME_DIALOG(dialog), GTK_EDITABLE(spin));
	
	/*
	 * Pack the label & the Gnome entry into the dialog.
	 */
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), label,
			   FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), spin,
			   FALSE, FALSE, 0);
	
	gtk_signal_connect(GTK_OBJECT(dialog), "clicked",
			   GTK_SIGNAL_FUNC(goto_dlg_clicked), adjustment);
	gtk_window_set_focus(GTK_WINDOW(dialog), spin);
	show_nice_dialog(&dialog, "gtranslator -- goto");
}

static void match_case_toggled(GtkWidget * widget, gpointer useless)
{
	wants.match_case =
	    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	update_flags();
	gtranslator_config_init();
	gtranslator_config_set_bool("find/case_sensitive",
			      wants.match_case);
	gtranslator_config_close();
}

static void find_in_activated(GtkWidget * widget, gpointer which)
{
	wants.find_in = GPOINTER_TO_INT(which);
	gtranslator_config_init();
	gtranslator_config_set_int("find/find_in",
			      wants.find_in);
	gtranslator_config_close();
}

static void find_dlg_clicked(GnomeDialog * dialog, gint button,
			     gpointer findy)
{
	gchar *find_what;
	if (button == GNOME_OK) {
		find_what = gtk_editable_get_chars(
			GTK_EDITABLE(gnome_entry_gtk_entry(findy)), 0, -1);
		find_do(NULL, find_what);
		enable_actions(ACT_FIND_AGAIN, ACT_END);
		return;
	}
	gnome_dialog_close(dialog);
}

void find_dialog(GtkWidget * widget, gpointer useless)
{
	static GtkWidget *dialog = NULL;
	GtkWidget *label, *findy, *match_case;
	GtkWidget *find_in, *menu, *menu_item, *option, *hbox;

	raise_and_return_if_exists(dialog);
	dialog = gnome_dialog_new(_("Find in the po-file"), _("Find"),
				  GNOME_STOCK_BUTTON_CLOSE, NULL);
	
	label = gtk_label_new(_("Enter your desired search string:"));
	findy = gnome_entry_new("FINDY");
	
	match_case = gtk_check_button_new_with_label(_("Case sensitive"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(match_case),
				     wants.match_case);

	menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_label(_("English"));
	gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
			   GTK_SIGNAL_FUNC(find_in_activated),
			   GINT_TO_POINTER(0));
	gtk_menu_append(GTK_MENU(menu), menu_item);
	menu_item = gtk_menu_item_new_with_label(_("Translated"));
	gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
			   GTK_SIGNAL_FUNC(find_in_activated),
			   GINT_TO_POINTER(1));
	gtk_menu_append(GTK_MENU(menu), menu_item);
	menu_item = gtk_menu_item_new_with_label(_("Both"));
	gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
			   GTK_SIGNAL_FUNC(find_in_activated),
			   GINT_TO_POINTER(2));
	gtk_menu_append(GTK_MENU(menu), menu_item);
	gtk_menu_set_active(GTK_MENU(menu), wants.find_in);

	find_in = gtk_label_new(_("Find in: "));
	option = gtk_option_menu_new();
	gtk_option_menu_set_menu(GTK_OPTION_MENU(option), menu);

	hbox = gtk_hbox_new(FALSE, 0);

	/*
	 * Pack the single elements into the dialog.
	 */
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), label,
			   FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), findy,
			   FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), match_case,
			   FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), find_in,
			   FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), option,
			   TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), hbox,
			   FALSE, FALSE, 0);
	
	gtk_signal_connect(GTK_OBJECT(dialog), "clicked",
			   GTK_SIGNAL_FUNC(find_dlg_clicked), findy);
	gtk_signal_connect(GTK_OBJECT(match_case), "toggled",
			   GTK_SIGNAL_FUNC(match_case_toggled), NULL);
	gtk_window_set_focus(GTK_WINDOW(dialog), gnome_entry_gtk_entry(GNOME_ENTRY(findy)));
	show_nice_dialog(&dialog, "gtranslator -- find");
}

/* 
 * TODO: Jump to the message containing first error. Something strange with
 * line/message numbers, maybe we need to convert between them?
 */
void compile_error_dialog(FILE * fs)
{
	gchar buf[2048];
	gint pos[] = { 0 };
	gint len;
	GtkWidget *dialog, *textbox;
	GtkWidget *scroll;

	dialog = gnome_app_error(GNOME_APP(app1),
		_("An error occured while msgfmt was executed:\n"));
	textbox = gtk_text_new(NULL, NULL);
	gtk_text_set_editable(GTK_TEXT(textbox), FALSE);
	while (TRUE) {
		len = fread(buf, 1, sizeof(buf), fs);
		if (len == 0)
			break;
		gtk_editable_insert_text(GTK_EDITABLE(textbox), buf, len, pos);
	}
	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scroll), textbox);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox),
			 scroll, TRUE, TRUE, 0);
	show_nice_dialog(&dialog, NULL);
}

