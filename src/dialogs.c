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

#include "actions.h"
#include "color-schemes.h"
#include "dialogs.h"
#include "find.h"
#include "gui.h"
#include "message.h"
#include "open-differently.h"
#include "parse.h"
#include "prefs.h"
#include "query.h"
#include "replace.h"
#include "syntax.h"

#include <string.h>
#include <locale.h>
#include <libgnome/gnome-util.h>
#include <libgnomeui/libgnomeui.h>

/*
 * Functions to be used only internally in this file
 */
static void gtranslator_go_to_dialog_clicked(GnomeDialog * dialog, gint button,
			     gpointer adjustment);
static void match_case_toggled(GtkWidget * widget, gpointer useless);
static void find_dlg_clicked(GnomeDialog * dialog, gint button,
	gpointer findy);

/*
 * The open URI dialog signal function:
 */ 
void gtranslator_open_uri_dialog_clicked(GnomeDialog *dialog, gint button,
	gpointer entrydata);

void gtranslator_dialog_show(GtkWidget ** dlg, const gchar * wmname)
{
	if (wmname != NULL)
		gtk_window_set_wmclass(GTK_WINDOW(*dlg), wmname, "gtranslator");
	if (GNOME_IS_DIALOG(*dlg))
		gnome_dialog_set_parent(GNOME_DIALOG(*dlg), GTK_WINDOW(gtranslator_application));
	gtk_signal_connect(GTK_OBJECT(*dlg), "destroy",
			   GTK_SIGNAL_FUNC(gtk_widget_destroyed), dlg);

	gtk_widget_show_all(*dlg);
}

GtkWidget * gtranslator_error(char *format, ...)
{
	char *error;
	va_list ap;
	GtkWidget *w;
	
	va_start(ap, format);
	error = g_strdup_vprintf(format, ap);
	va_end(ap);
	w = gnome_app_error(GNOME_APP(gtranslator_application), error);
	g_free(error);
	return w;
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
	gtranslator_raise_dialog(dialog);
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
	 * Make the dialog transient, gtranslator_dialog_show does not do it
	 *  because it is not a GnomeDialog.
	 */
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(gtranslator_application));
	gtranslator_dialog_show(&dialog, "gtranslator -- open");
}

/*
 * "Save as" dialog.
 */
void gtranslator_save_file_as_dialog(GtkWidget * widget, gpointer useless)
{
	static GtkWidget *dialog = NULL;
	gtranslator_raise_dialog(dialog);

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
			   "clicked", GTK_SIGNAL_FUNC(gtranslator_save_file_dialog),
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
	gtranslator_dialog_show(&dialog, "gtranslator -- ask");
	reply = gnome_dialog_run(GNOME_DIALOG(dialog));
	g_free(question);
	if (reply == GNOME_YES)
		gtranslator_save_current_file_dialog(NULL, NULL);
	else if (reply == GNOME_NO)
		po->file_changed = FALSE;
	else
		return FALSE;
	return TRUE;
}

/*
 * The "Go to" functions.
 */
static void gtranslator_go_to_dialog_clicked(GnomeDialog * dialog, gint button,
			     gpointer adjustment)
{
	guint number = GTK_ADJUSTMENT(adjustment)->value - 1;
	if (button == GNOME_OK)
		gtranslator_message_go_to_no(GTK_WIDGET(dialog), GUINT_TO_POINTER(number));
	gnome_dialog_close(dialog);
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
	gtranslator_raise_dialog(dialog);
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
			   GTK_SIGNAL_FUNC(gtranslator_go_to_dialog_clicked), adjustment);
	gtk_window_set_focus(GTK_WINDOW(dialog), spin);
	gtranslator_dialog_show(&dialog, "gtranslator -- goto");
}

static void match_case_toggled(GtkWidget * widget, gpointer useless)
{
	wants.match_case =
	    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	gtranslator_update_regex_flags();
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
		gtranslator_find(NULL, find_what);
		gtranslator_actions_enable(ACT_FIND_AGAIN, ACT_END);
		return;
	}
	gnome_dialog_close(dialog);
}

void gtranslator_find_dialog(GtkWidget * widget, gpointer useless)
{
	int findMenu=0;
	static GtkWidget *dialog = NULL;
	GtkWidget *label, *findy, *match_case;
	GtkWidget *find_in, *menu, *menu_item, *option, *hbox;

	gtranslator_raise_dialog(dialog);
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
			   GINT_TO_POINTER(findEnglish));
	gtk_menu_append(GTK_MENU(menu), menu_item);
	
	menu_item = gtk_menu_item_new_with_label(_("Translated"));
	gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
			   GTK_SIGNAL_FUNC(find_in_activated),
			   GINT_TO_POINTER(findTranslated));
	gtk_menu_append(GTK_MENU(menu), menu_item);
	
	menu_item = gtk_menu_item_new_with_label(_("Both"));
	gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
			   GTK_SIGNAL_FUNC(find_in_activated),
			   GINT_TO_POINTER(findBoth));
	gtk_menu_append(GTK_MENU(menu), menu_item);
	
	menu_item = gtk_menu_item_new_with_label(_("Comments"));
	gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
			   GTK_SIGNAL_FUNC(find_in_activated),
			   GINT_TO_POINTER(findComment));
	gtk_menu_append(GTK_MENU(menu), menu_item);
	
	menu_item = gtk_menu_item_new_with_label(_("In all strings"));
	gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
			   GTK_SIGNAL_FUNC(find_in_activated),
			   GINT_TO_POINTER(findAll));
	gtk_menu_append(GTK_MENU(menu), menu_item);
	
	switch (wants.find_in) {
	case findEnglish:    findMenu = 0; break;
	case findTranslated: findMenu = 1; break;
	case findBoth:       findMenu = 2; break;
	case findComment:    findMenu = 3; break;
	case findAll:        findMenu = 4; break;
	}
	gtk_menu_set_active(GTK_MENU(menu), findMenu);

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
	gtk_window_set_focus(GTK_WINDOW(dialog), 
		gnome_entry_gtk_entry(GNOME_ENTRY(findy)));
	
	gtranslator_dialog_show(&dialog, "gtranslator -- find");
}

/*
 * The replace dialog -- based on the find dialog.
 */
void gtranslator_replace_dialog(GtkWidget *widget, gpointer useless)
{
	int findMenu=0;
	int reply;
	static GtkWidget *dialog = NULL;
	GtkWidget *label, *sndlabel;
	GtkWidget *findy, *replacy;
	GtkWidget *find_in, *menu, *menu_item, *option, *hbox;

	gtranslator_raise_dialog(dialog);
	dialog=gnome_dialog_new(_("gtranslator -- replace"),
		_("Replace"),
		_("Replace all"),
		GNOME_STOCK_BUTTON_CLOSE, 
		NULL);
	
	label=gtk_label_new(_("String to replace:"));
	findy=gnome_entry_new("REPLACE_THIS");

	sndlabel=gtk_label_new(_("Replace string:"));
	replacy=gnome_entry_new("REPLACE_WITH_THIS");
	
	menu=gtk_menu_new();
	
	menu_item = gtk_menu_item_new_with_label(_("Comments"));
	gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
		GTK_SIGNAL_FUNC(find_in_activated),
		GINT_TO_POINTER(findComment));
	gtk_menu_append(GTK_MENU(menu), menu_item);

	menu_item=gtk_menu_item_new_with_label(_("Translated"));
	gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
		GTK_SIGNAL_FUNC(find_in_activated),
		GINT_TO_POINTER(findTranslated));
	gtk_menu_append(GTK_MENU(menu), menu_item);
	
	menu_item=gtk_menu_item_new_with_label(_("Both"));
	gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
		GTK_SIGNAL_FUNC(find_in_activated),
		GINT_TO_POINTER(findBoth));
	gtk_menu_append(GTK_MENU(menu), menu_item);
	
	switch (wants.find_in) 
	{
		case findComment: 
			findMenu=0;
				break;
			
		case findTranslated:
			findMenu=1;
				break;
		
		case findBoth:
			findMenu=2;
				break;
	}
	
	gtk_menu_set_active(GTK_MENU(menu), findMenu);
	
	find_in=gtk_label_new(_("Replace in:"));
	option=gtk_option_menu_new();
	gtk_option_menu_set_menu(GTK_OPTION_MENU(option), menu);

	hbox=gtk_hbox_new(FALSE, 0);

	/*
	 * Pack the single elements into the dialog.
	 */
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), label,
		FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), 
		findy, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), sndlabel,
		FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), 
		replacy, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), find_in,
		FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), option,
		TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), hbox,
		FALSE, FALSE, 0);
	
	gtk_window_set_focus(GTK_WINDOW(dialog), 
		gnome_entry_gtk_entry(GNOME_ENTRY(findy)));

	gtranslator_dialog_show(&dialog, "gtranslator -- replace");
	
	reply=gnome_dialog_run(GNOME_DIALOG(dialog));

	if(reply==2)
	{
		gnome_dialog_close(GNOME_DIALOG(dialog));
	}
	else
	{
		gchar *findme, *replaceme;
		GtrReplace *rpl;

		findme=gtk_editable_get_chars(GTK_EDITABLE(
			gnome_entry_gtk_entry(GNOME_ENTRY(findy))), 0, -1);

		replaceme=gtk_editable_get_chars(GTK_EDITABLE(
			gnome_entry_gtk_entry(GNOME_ENTRY(replacy))), 0, -1);

		gnome_dialog_close(GNOME_DIALOG(dialog));

		if(reply==1)
		{
			rpl=gtranslator_replace_new(findme, replaceme, TRUE, 0);
		}
		else
		{
			rpl=gtranslator_replace_new(findme, replaceme, FALSE, 
				g_list_position(po->messages, po->current));
		}

		g_free(findme);
		g_free(replaceme);
		
		gtranslator_replace_run(rpl);
	}
	
}

/* 
 * TODO: Jump to the message containing first error. Something strange with
 * line/message numbers, maybe we need to convert between them?
 */
void gtranslator_compile_error_dialog(FILE * fs)
{
	gchar buf[2048];
	gint pos[] = { 0 };
	gint len;
	GtkWidget *dialog, *textbox;
	GtkWidget *scroll;

	dialog = gtranslator_error(_("An error occured while msgfmt was executed:\n"));
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
	gtranslator_dialog_show(&dialog, NULL);
}

/*
 * Requests for an URI to open. Uses pregiven protocol list.
 */ 
void gtranslator_open_uri_dialog(GtkWidget *widget, gpointer useless)
{
	static GtkWidget *dialog=NULL;
	GtkWidget *entry;
	GtkWidget *label;

	gtranslator_raise_dialog(dialog);

	dialog=gnome_dialog_new(_("gtranslator -- open from URI"),
				_("Open"), GNOME_STOCK_BUTTON_CANCEL, NULL);
	/* Make Open button the default */
	gnome_dialog_set_default(GNOME_DIALOG(dialog), 0);

	entry=gnome_entry_new("URI");

	label=gtk_label_new(_("Enter URI:"));
	
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), label,
		FALSE, FALSE, 0);
	
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), entry,
		FALSE, FALSE, 0);
	gtk_window_set_focus(GTK_WINDOW(dialog),
		gnome_entry_gtk_entry(GNOME_ENTRY(entry)));
	gnome_dialog_editable_enters(GNOME_DIALOG(dialog),
		GTK_EDITABLE(gnome_entry_gtk_entry(GNOME_ENTRY(entry))));

	gtk_signal_connect(GTK_OBJECT(dialog), "clicked",
		GTK_SIGNAL_FUNC(gtranslator_open_uri_dialog_clicked), entry);
			
	gtranslator_dialog_show(&dialog, "gtranslator -- open URI");
}

/*
 * Checks the URI before it's passed to the core functions.
 */ 
void gtranslator_open_uri_dialog_clicked(GnomeDialog *dialog, gint button,
	gpointer entrydata)
{
	GString *uri=g_string_new("");

	if(button==GNOME_OK)
	{
		/*
		 * Get the URI data from the GnomeEntry.
		 */ 
		uri=g_string_append(uri, gtk_editable_get_chars(
			GTK_EDITABLE(gnome_entry_gtk_entry(entrydata)),
				0, -1));

		
		if(uri->len <= 0)
		{
			/*
			 * Show an error dialog but don't close down the 
			 *  Open from URI dialog.
			 */  
			gtranslator_error(_("No URI given!"));	
		}
		else
		{
			/*
			 * Check if it's one of our supported URI
			 *  types or if it's a "hidden" http URI.
			 */ 
			if(!strncmp(uri->str, "http", 4)||
				!strncmp(uri->str, "https", 5)||
				!strncmp(uri->str, "ftp", 3)||
				!strncmp(uri->str, "file", 4)||
				!strncmp(uri->str, "www.", 4)||
				!strncmp(uri->str, "ftp.", 4))
			{
				gnome_dialog_close(dialog);
				gtranslator_open_po_file(uri->str);
			}
			else
			{
				gtranslator_error(_("No supported URI protocol (like \"ftp://\") given!"));
			}
		}
	}
	else
	{
		gnome_dialog_close(dialog);
	}

	g_string_free(uri, FALSE);
	
}

/*
 * Possibly rescue the given file.
 */
void gtranslator_rescue_file_dialog(void)
{
	GtkWidget *dialog;
	gchar *recovery_message;
	gchar *original_filename;
	gchar *file;
	gint reply;
	
	/*
	 * Get the original filename for the crash-file from the prefs.
	 */
	gtranslator_config_init();
	original_filename=gtranslator_config_get_string("crash/filename");
	gtranslator_config_close();

	recovery_message=g_strdup_printf(_("Open recovery file for `%s'?\n\
It was saved by gtranslator before gtranslator got closed.\n\
Possibly it contains hard pieces of work?!\n\
Saying \"No\" will delete the crash recovery file."),
		original_filename);
	
	dialog=gnome_message_box_new(recovery_message,
		GNOME_MESSAGE_BOX_WARNING,
		GNOME_STOCK_BUTTON_YES,
		GNOME_STOCK_BUTTON_NO,
		GNOME_STOCK_BUTTON_CANCEL,
		NULL);

	gnome_dialog_set_default(GNOME_DIALOG(dialog), 0);
	
	gtranslator_dialog_show(&dialog, "gtranslator -- ask for crash recovery");
	
	reply=gnome_dialog_run(GNOME_DIALOG(dialog));
	
	g_free(recovery_message);

	file=g_strdup_printf("%s/%s", g_get_home_dir(), 
		".crash-gtranslator.po");
	
	if(reply==GNOME_YES)
	{
		g_message(_("Recovering `%s'..."), original_filename);

		/*
		 * Move the recovery file to the original filename and re-open
		 *  it now again.
		 */ 
		rename(file, original_filename);
		gtranslator_parse_main(original_filename);
	}
	else if(reply==GNOME_NO)
	{
		/*
		 * Remove the crash recovery file.
		 */
		unlink(file);
	}

	g_free(original_filename);
	g_free(file);
}

/*
 * Query for a specific string.
 */
void gtranslator_query_dialog(void)
{
	static GtkWidget *dialog=NULL;
	GtkWidget *innertable;
	GtkWidget *query_entry;
	GtkWidget *query_entry_label;
	GtkWidget *domain;
	GtkWidget *domain_label;
	GtkWidget *label;
	gint reply;
	
	gtranslator_raise_dialog(dialog);

	#define add2Box(x); \
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), x, \
		FALSE, FALSE, 0);
	#define add2Table(x, y, z); \
	gtk_table_attach_defaults(GTK_TABLE(innertable), x, y, y+1, z, z+1);

	if(!domains)
	{
		gtranslator_error(_("Couldn't get list of gettext domains!"));
		return;
	}

	/*
	 * A half-baken dialog for the query functionality.
	 */
	label=gtk_label_new(_("Here you can query existing gettext domains for a translation."));
	
	dialog=gnome_dialog_new(
		_("gtranslator -- query existing gettext domains"),
		_("Query"), GNOME_STOCK_BUTTON_CLOSE, NULL);

	innertable=gtk_table_new(2, 2, FALSE);

	gtk_table_set_row_spacings(GTK_TABLE(innertable), 5);
	gtk_table_set_col_spacings(GTK_TABLE(innertable), 5);
	
	query_entry=gnome_entry_new("QUERY");
	query_entry_label=gtk_label_new(_("Query string:"));

	/*
	 * Setup our loved domains list as the stringlist for the combobox.
	 */
	domain=gtk_combo_new();
	gtk_combo_set_popdown_strings(GTK_COMBO(domain), domains);
	domain_label=gtk_label_new(_("Domain to search the translation in:"));

	/*
	 * Set up the default query domain from the preferences if available.
	 */
	if(wants.defaultdomain)
	{
		gtk_entry_set_text(GTK_ENTRY(
			GTK_COMBO(domain)->entry), wants.defaultdomain);
	}

	/*
	 * Add the widgets to the dialog.
	 */
	add2Box(label);
	add2Box(innertable);

	add2Table(query_entry_label, 0, 0);
	add2Table(query_entry, 1, 0);
	add2Table(domain_label, 0, 1);
	add2Table(domain, 1, 1);

	/*
	 * "Query" should be the default button I guess.
	 */
	gnome_dialog_set_default(GNOME_DIALOG(dialog), 0);
	gtranslator_dialog_show(&dialog, "gtranslator -- query dialog");

	reply=gnome_dialog_run(GNOME_DIALOG(dialog));

	if(reply==GNOME_CANCEL)
	{
		gnome_dialog_close(GNOME_DIALOG(dialog));
	}
	else if(reply==GNOME_YES)
	{
		gchar *query_text;
	
		/*
		 * Get the string to query for from the GnomeEntry.
		 */
		query_text=gtk_editable_get_chars(GTK_EDITABLE(
		gnome_entry_gtk_entry(GNOME_ENTRY(query_entry))), 0, -1);

		if(!query_text || (strlen(query_text) <= 1))
		{
			/*
			 * Bad case in here.
			 */
			gnome_app_warning(GNOME_APP(gtranslator_application),
				_("No query string given!"));

			g_free(query_text);
			gnome_dialog_close(GNOME_DIALOG(dialog));
		}
		else
		{
			GtrQuery *query;
			GtrQuery *result;
			gchar *domainname;

			/*
			 * Get the domain's name from the combobox.
			 */
			domainname=gtk_editable_get_chars(GTK_EDITABLE(
				GTK_COMBO(domain)->entry), 0, -1);

			/*
			 * Build up and run the query.
			 */
			query=gtranslator_new_query(domainname, query_text, lc);
			result=gtranslator_query_simple(query);
			
			/*
			 * Close the open dialog now.
			 */
			gnome_dialog_close(GNOME_DIALOG(dialog));

			if(!result)
			{
				/* 
				 * No results? Close down the dialog.
				 */
				gnome_app_warning(GNOME_APP(gtranslator_application),
				_("Couldn't find any result for the query!"));
			}
			else
			{
				/*
				 * In the other case simply print out the
				 *  found translation with the domain name for 
				 *   now and close/exit then.
				 */
				gchar *resulttext;
				GtkWidget *condialog=NULL;
				gint hehue;

				resulttext=g_strdup_printf(_("Found \"%s\" as a translation for \"%s\" in domain \"%s\".\n\
Would you like to insert it into the translation?"),
					result->message, query->message, result->domain);

				/*
				 * Set the last query result in the prefs for
				 *  persistence.
				 */
				gtranslator_config_init();
				
				gtranslator_config_set_string(
					"query/last_match_filename",
					po->filename);
				
				gtranslator_config_set_int(
					"query/last_match_message_no",
					g_list_position(po->messages, 
						po->current));
				
				gtranslator_config_set_string(
					"query/last_match_domain", 
					result->domain);
				
				gtranslator_config_set_string(
					"query/last_match_message", 
					query->message);
				
				gtranslator_config_set_string(
					"query/last_match_translation", 
					result->message);
				
				gtranslator_config_close();

				gtranslator_free_query(&query);
				

				/*
				 * Build up another dialog and show up the
				 *  possible actions.
				 */
				condialog=gnome_message_box_new(resulttext,
					GNOME_MESSAGE_BOX_INFO,
					GNOME_STOCK_BUTTON_YES,
					GNOME_STOCK_BUTTON_NO,
					NULL);
				
				/*
				 * Run the dialog and switch the action to take
				 *  depending on the user's selection.
				 */
				gtranslator_dialog_show(&condialog, "gtranslator -- query result");
				hehue=gnome_dialog_run_and_close(GNOME_DIALOG(condialog));
				
				g_free(resulttext);
				
				if(hehue==GNOME_YES)
				{
					gchar *content;

					/*
					 * Get the translation box contents.
					 */
					content=gtk_editable_get_chars(
						GTK_EDITABLE(trans_box), 0, -1);

					/*
					 * See if the query result is already in the
					 *  translation box; if so print a warning
					 *   and don't insert the translation.
					 */
					if(content && strcmp(content, result->message))
					{
						/*
						 * Insert the text and update the
						 * status flags for it.
						 */
						gtranslator_syntax_insert_text(trans_box,
						result->message);

						gtranslator_translation_changed(NULL, NULL);

						gtranslator_free_query(&result);
					}
					else
					{
						gnome_app_warning(GNOME_APP(gtranslator_application),
						/*
						 * Translators: This means that the queried string
						 *  is already translated.
						 */
						_("Query's result translation is already there!"));
					}

					g_free(content);
				}
			}
		}
	}
}

/*
 * Asks the user if he/she does really want to use autoaccomplishment
 *  and does it if wished.
 */
void gtranslator_auto_accomplishment_dialog(void)
{
	static GtkWidget *dialog=NULL;
	gint reply;

	gtranslator_raise_dialog(dialog);

	dialog=gnome_message_box_new(
		_("Should gtranslator accomplish all missing strings (if possible)\n\
from your default query domain?"),
		GNOME_MESSAGE_BOX_QUESTION,
		GNOME_STOCK_BUTTON_YES,
		GNOME_STOCK_BUTTON_NO,
		NULL);

	/*
	 * Set the default to "Yes" and show/run the dialog.
	 */
	gnome_dialog_set_default(GNOME_DIALOG(dialog), 0);
	
	gtranslator_dialog_show(&dialog, "gtranslator -- accomplish?");

	reply=gnome_dialog_run(GNOME_DIALOG(dialog));

	/*
	 * Only handle the "Yes" case as we do not think about the "No" case -- the
	 *  user didn't want any accomplishment.
	 */
	if(reply==GNOME_YES)
	{
		/*
		 * Accomplish the missing entries via the new query function:
		 */
		gtranslator_query_accomplish();
	}
}
