/**
* Fatih Demir [ kabalak@gmx.net ]
* Gediminas Paulauskas <menesis@delfi.lt>
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* This is the general place for all dialogs
* used in gtranslator ...
*
* -- the source ...
**/

#include "dialogs.h"
#include "find.h"
#include "msg_db.h"

/* Functions to be used only internally in this file */
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

/**
* The Open file dialog
**/
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
	/* gtk_file_selection_complete(GTK_FILE_SELECTION(dialog),"*.po*"); */
	/* Connect the signals */
	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(dialog)->ok_button),
			   "clicked", GTK_SIGNAL_FUNC(parse_the_file),
			   (gpointer) dialog);
	gtk_signal_connect_object(GTK_OBJECT
				  (GTK_FILE_SELECTION(dialog)->cancel_button),
		"clicked",
		GTK_SIGNAL_FUNC(gtk_widget_destroy),
		GTK_OBJECT(dialog));
	/* Make the dialog transient, show_nice_dialog does not do it */
	/* because it is not a GnomeDialog */
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(app1));
	show_nice_dialog(&dialog, "gtranslator -- open");
}

/**
* Save file as-dialog
**/
void save_file_as(GtkWidget * widget, gpointer useless)
{
	static GtkWidget *dialog = NULL;
	raise_and_return_if_exists(dialog);
	dialog = gtk_file_selection_new(_("gtranslator -- save file as .."));
	/* gtk_file_selection_complete(GTK_FILE_SELECTION(dialog),"*.po"); */
	/* Connect the signals */
	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(dialog)->ok_button),
			   "clicked", GTK_SIGNAL_FUNC(save_the_file),
			   (gpointer) dialog);
	gtk_signal_connect_object(GTK_OBJECT
				  (GTK_FILE_SELECTION(dialog)->cancel_button),
		"clicked",
		GTK_SIGNAL_FUNC(gtk_widget_destroy),
		GTK_OBJECT(dialog));
	/* Make the dialog transient */
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(app1));
	show_nice_dialog(&dialog, "gtranslator -- save");
}

/* If file was changed, asks the user, what to do, according to response, saves
 * the file or not, and returns TRUE. If neither of YES and NO was pressed,
 * returns FALSE */
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

/**
* The GoTo methods
**/
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
	/* We want Go! button to be the default */
	gnome_dialog_set_default(GNOME_DIALOG(dialog), 0);
	label = gtk_label_new(_("Go to message number:"));
	/* Display current message number and let it change from first to
	   last */
	adjustment =
	    gtk_adjustment_new(g_list_position(po->messages, po->current) + 1,
	    		       1, po->length, 1, 10, 10);
	spin = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1, 0);
	gtk_spin_button_set_update_policy(GTK_SPIN_BUTTON(spin), 
					  GTK_UPDATE_IF_VALID);
	gnome_dialog_editable_enters(GNOME_DIALOG(dialog), GTK_EDITABLE(spin));
	gtk_window_set_focus(GTK_WINDOW(dialog), spin);
	/**
	* Add'em to the dialog
	**/
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), label,
			   FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), spin,
			   FALSE, FALSE, 0);
	/**
	* Connect the signals
	**/
	gtk_signal_connect(GTK_OBJECT(dialog), "clicked",
			   GTK_SIGNAL_FUNC(goto_dlg_clicked), adjustment);
	show_nice_dialog(&dialog, "gtranslator -- goto");
}

static void match_case_toggled(GtkWidget * widget, gpointer useless)
{
	wants.match_case =
	    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	update_flags();
	gnome_config_set_bool("/gtranslator/Find/Case sensitive",
			      wants.match_case);
	gnome_config_sync();
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
	GtkWidget *label, *findy, *match_case, *find_in, *menu, *option;

	raise_and_return_if_exists(dialog);
	dialog = gnome_dialog_new(_("Find in the po-file"), _("Find"),
				  GNOME_STOCK_BUTTON_CLOSE, NULL);
	/**
	* Create the widgets.
	**/
	label = gtk_label_new(_("Enter your desired search string:"));
	findy = gnome_entry_new("FINDY");
	
	match_case = gtk_check_button_new_with_label(_("Case sensitive"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(match_case),
				     wants.match_case);

	menu = gtk_menu_new();
	gtk_menu_append(GTK_MENU(menu), 
		gtk_menu_item_new_with_label(_("English")));
	gtk_menu_append(GTK_MENU(menu), 
		gtk_menu_item_new_with_label(_("Translated")));
	gtk_menu_append(GTK_MENU(menu), 
		gtk_menu_item_new_with_label(_("Both")));
	gtk_menu_set_active(GTK_MENU(menu), wants.find_in);

	find_in = gtk_label_new(_("Find in: "));
	option = gtk_option_menu_new();
	gtk_option_menu_set_menu(GTK_OPTION_MENU(option), menu);

	/**
	* Add the elements to the table.
	**/
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), label,
			   FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), findy,
			   FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), match_case,
			   FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), find_in,
			   FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), option,
			   FALSE, FALSE, 0);
	/**
	* Connect the signals
	**/
	gtk_signal_connect(GTK_OBJECT(dialog), "clicked",
			   GTK_SIGNAL_FUNC(find_dlg_clicked), findy);
	gtk_signal_connect(GTK_OBJECT(match_case), "toggled",
			   GTK_SIGNAL_FUNC(match_case_toggled), NULL);
	show_nice_dialog(&dialog, "gtranslator -- find");
}

static GtkWidget *q_dlg, *q_dlg_cancel, *q_dlg_query, *q_entry, *q_dlg_spin_button;
static GtkWidget *r_window_dlg, *r_window_apply, *r_window_cancel;

void query_dialog_create()
{
	GtkWidget *q_dlg_label;
	/**
	* The default challenge length.
	**/
	gint default_challen;
	/**
	* The GtkAdjustment for the spinbutton.
	**/
	GtkObject *fot;
	/**
	* The default challen is 2.
	**/
	default_challen = 2;
	/**
	* Create the dialog.
	**/
	q_dlg = gtk_dialog_new();
	/**
	* Create a helping label.
	**/
	q_dlg_label = gtk_label_new(_("With this query window you can query\n\
the message db for an existing entry\n\
with your query string."));
	/**
	* Get the adjustment.
	**/
	fot = gtk_adjustment_new(default_challen, 1, 10, 1, 2, 2);
	/**
	* Create the spinbutton.
	**/
	q_dlg_spin_button = gtk_spin_button_new(GTK_ADJUSTMENT(fot), 1, 0);
	/**
	* Add another GNOME entry.
	**/
	q_entry = gnome_entry_new("QUERY");
	/**
	* Create the buttons.
	**/
	q_dlg_cancel = gnome_stock_button(GNOME_STOCK_BUTTON_CANCEL);
	q_dlg_query = gtk_button_new_with_label(_("Query"));
	/**
	* Set the size.
	**/
	gtk_widget_set_usize(q_dlg, 260, 150);
	/**
	* Add the widgets to the dialog.
	**/
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(q_dlg)->vbox),
			  q_dlg_label);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(q_dlg)->vbox), q_entry);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(q_dlg)->vbox),
			  q_dlg_spin_button);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(q_dlg)->action_area),
			  q_dlg_query);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(q_dlg)->action_area),
			  q_dlg_cancel);
	/**
	* Connect the signals.
	**/
	gtk_signal_connect(GTK_OBJECT(q_dlg_query), "clicked",
			   GTK_SIGNAL_FUNC(r_window), NULL);
	gtk_signal_connect(GTK_OBJECT(q_dlg_cancel), "clicked",
			   GTK_SIGNAL_FUNC(query_dialog_hide), NULL);
	/**
	* Show the inner-widgets.
	**/
	gtk_widget_show(q_dlg_label);
	gtk_widget_show(q_dlg_spin_button);
	gtk_widget_show(q_entry);
	gtk_widget_show(q_dlg_cancel);
	gtk_widget_show(q_dlg_query);
}

void query_dialog(GtkWidget * widget, gpointer useless)
{
	/**
	* Create the dialog.
	**/
	query_dialog_create();
	/**
	* Set the window-icon.
	**/
#ifdef USE_WINDOW_ICON
	gnome_window_icon_set_from_file(GTK_WINDOW(q_dlg), WINDOW_ICON);
#endif
	/**
	* Set the window-properties.
	**/
	gtk_window_set_title(GTK_WINDOW(q_dlg),
			     _("gtranslator -- query the message db"));
	gtk_window_set_wmclass(GTK_WINDOW(q_dlg), "gtranslator -- query",
			       "gtranslator");
	gtk_widget_show(q_dlg);
}

void query_dialog_hide(GtkWidget * widget, gpointer useless)
{
	/**
	* Hide the dialog.
	**/
	gtk_widget_hide(q_dlg);
}

void r_window(GtkWidget * widget, gpointer useless)
{
	GtkWidget *r_label;
	 /**
        * The gchar holding the result.
        **/
        gchar *result;
	gchar result_string[256];
	result_string[0] = '\0';
	/**
	* Set the challenge-length as specified in the query-dialog.
	**/
	set_challenge_length(gtk_spin_button_get_value_as_int
			     (GTK_SPIN_BUTTON(q_dlg_spin_button)));
	/**
	* Get the result.
	*
	* Note: This is really ugly but it avoids sparing many lines for this.
	**/
	result = get_from_msg_db(gtk_entry_get_text(GTK_ENTRY(gnome_entry_gtk_entry(GNOME_ENTRY(q_entry)))));
	g_snprintf(result_string, 256, _("Result: `%s'"), result);
	/**
	* Hide the previous query dialog.
	**/
	gtk_widget_hide(q_dlg);
	/**
	* Create the dialog.
	**/
	r_window_dlg = gtk_dialog_new();
	/**
	* An explaining label in front of the result.
	**/
	r_label = gtk_label_new(result_string);
	/**
	* Create the buttons.
	**/
	r_window_cancel = gnome_stock_button(GNOME_STOCK_BUTTON_CANCEL);
	r_window_apply = gtk_button_new_with_label(_("Insert"));
	/**
	* Set the window properties.
	**/
	gtk_window_set_title(GTK_WINDOW(r_window_dlg),
			     _("gtranslator -- result for your query"));
	gtk_window_set_wmclass(GTK_WINDOW(r_window_dlg),
			       "gtranslator -- result", "gtranslator");
	/**
	* Place the other widgets.
	**/
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(r_window_dlg)->vbox),
			  r_label);
	gtk_container_add(GTK_CONTAINER
			  (GTK_DIALOG(r_window_dlg)->action_area),
			  r_window_apply);
	gtk_container_add(GTK_CONTAINER
			  (GTK_DIALOG(r_window_dlg)->action_area),
			  r_window_cancel);
	/**
	* Show again the inner widgte first.
	**/
	gtk_widget_show(r_label);
	gtk_widget_show(r_window_apply);
	gtk_widget_show(r_window_cancel);
	/**
        * Set the window-icon.
        **/
#ifdef USE_WINDOW_ICON
	gnome_window_icon_set_from_file(GTK_WINDOW(q_dlg), WINDOW_ICON);
#endif
	/**
	* Set the signals.
	**/
	gtk_signal_connect(GTK_OBJECT(r_window_cancel), "clicked",
			   GTK_SIGNAL_FUNC(r_window_hide), NULL);
	/**
	* Show it now.
	**/
	gtk_widget_show(r_window_dlg);
}

void r_window_hide(GtkWidget * widget, gpointer useless)
{
	/**
	* Hide the result window.
	**/
	gtk_widget_hide(r_window_dlg);
}

/* TODO jump to the message containing first error. Something strange with
 * line/message numbers, maybe we need to convert between them? */
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

