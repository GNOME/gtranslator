/**
 * Fatih Demir [ kabalak@gmx.net ]
 *
 * (C) 2000 Published under GNU GPL V 2.0+
 *
 * Here has the preferences box got his own
 *  home file ...
 *
 * -- the source
 **/

#include "prefs.h"

void prefs_box(GtkWidget *widget,gpointer useless)
{
        GtkWidget *propertybox1;
        GtkWidget *notebook1;
        GtkWidget *vbox3;
        GtkWidget *warn_for_errors;
        GtkWidget *saving_unchanged;
        GtkWidget *start_at_last_file;
        GtkWidget *label1;
        GtkWidget *vbox2;
        GSList *verbose_group = NULL;
        GtkWidget *verbose_yes;
        GtkWidget *verbose_no;
        GtkWidget *label2;

	propertybox1 = gnome_property_box_new ();

	notebook1 = GNOME_PROPERTY_BOX (propertybox1)->notebook;
	gtk_widget_show (notebook1);

	vbox3 = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox3);
	gtk_container_add (GTK_CONTAINER (notebook1), vbox3);

	warn_for_errors = gtk_check_button_new_with_label (_("Warn me if there are errors in the .po-file"));
	gtk_widget_show (warn_for_errors);
	gtk_box_pack_start (GTK_BOX (vbox3), warn_for_errors, FALSE, FALSE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (warn_for_errors), TRUE);

	saving_unchanged = gtk_check_button_new_with_label (_("Ask for saving unchanged .po-files "));
	gtk_widget_show (saving_unchanged);
	gtk_box_pack_start (GTK_BOX (vbox3), saving_unchanged, FALSE, FALSE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (saving_unchanged), TRUE);

	start_at_last_file = gtk_check_button_new_with_label (_("gtranslator should start at the last edited file"));
	gtk_widget_show (start_at_last_file);
	gtk_box_pack_start (GTK_BOX (vbox3), start_at_last_file, FALSE, FALSE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (start_at_last_file), TRUE);

	label1 = gtk_label_new (_("Common settings "));
	gtk_widget_show (label1);
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label1);

	vbox2 = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox2);
	gtk_container_add (GTK_CONTAINER (notebook1), vbox2);

	verbose_yes = gtk_radio_button_new_with_label (verbose_group, _("Show me the output of msgfmt --statistics"));
	verbose_group = gtk_radio_button_group (GTK_RADIO_BUTTON (verbose_yes));
	gtk_widget_show (verbose_yes);
	gtk_box_pack_start (GTK_BOX (vbox2), verbose_yes, FALSE, FALSE, 0);

        verbose_no = gtk_radio_button_new_with_label (verbose_group, _("Don't  show me any messages of msgfmt "));
        verbose_group = gtk_radio_button_group (GTK_RADIO_BUTTON (verbose_no));
        gtk_widget_show (verbose_no);
        gtk_box_pack_start (GTK_BOX (vbox2), verbose_no, FALSE, FALSE, 0);
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (verbose_no), TRUE);

        label2 = gtk_label_new (_("Compiling  points ..."));
        gtk_widget_show (label2);
        gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label2);

	gtk_signal_connect (GTK_OBJECT (propertybox1), "apply",
                            GTK_SIGNAL_FUNC (on_propertybox1_apply),
                            NULL);
	gtk_signal_connect (GTK_OBJECT (propertybox1), "help",
                            GTK_SIGNAL_FUNC (on_propertybox1_help),
                            propertybox1);
	gtk_signal_connect (GTK_OBJECT (propertybox1), "close",
                            GTK_SIGNAL_FUNC (on_propertybox1_close),
                            propertybox1);
	gtk_signal_connect (GTK_OBJECT (warn_for_errors), "toggled",
                            GTK_SIGNAL_FUNC (on_warn_for_errors_toggled),
                            propertybox1);
	gtk_signal_connect (GTK_OBJECT (saving_unchanged), "toggled",
                            GTK_SIGNAL_FUNC (on_saving_unchanged_toggled),
                            propertybox1);
	gtk_signal_connect (GTK_OBJECT (start_at_last_file), "toggled",
                            GTK_SIGNAL_FUNC (on_start_at_last_file_toggled),
                            propertybox1);
	gtk_signal_connect (GTK_OBJECT (verbose_yes), "toggled",
                            GTK_SIGNAL_FUNC (on_verbose_yes_toggled),
                            propertybox1);
	gtk_signal_connect (GTK_OBJECT (verbose_no), "toggled",
                            GTK_SIGNAL_FUNC (on_verbose_no_toggled),
                            propertybox1);

	return propertybox1;
}
