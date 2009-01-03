/*
 * Copyright (C) 2007  Pablo Sanxiao <psanxiao@gmail.com>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 2 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <string.h>

#include "application.h"
#include "header-dialog.h"
#include "utils.h"
#include "prefs-manager.h"
#include "po.h"

#define GTR_HEADER_DIALOG_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 	(object),	\
						 	GTR_TYPE_HEADER_DIALOG,     \
						 	GtranslatorHeaderDialogPrivate))

G_DEFINE_TYPE(GtranslatorHeaderDialog, gtranslator_header_dialog, GTK_TYPE_DIALOG)

struct _GtranslatorHeaderDialogPrivate
{
	GtkWidget *main_box;
	GtkWidget *notebook;

	GtkWidget *prj_page;
	GtkWidget *lang_page;
	GtkWidget *lang_vbox;

	GtkWidget *prj_id_version;
	GtkWidget *rmbt;
	GtkWidget *prj_comment;
	GtkWidget *take_my_options;

	GtkWidget *translator;
	GtkWidget *tr_email;
	GtkWidget *pot_date;
	GtkWidget *po_date;
	GtkWidget *language;
	GtkWidget *lg_email;
	GtkWidget *charset;
	GtkWidget *encoding;
};

static void gtranslator_header_dialog_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_header_dialog_parent_class)->finalize (object);
}

static void gtranslator_header_dialog_class_init (GtranslatorHeaderDialogClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorHeaderDialogPrivate));

	object_class->finalize = gtranslator_header_dialog_finalize;
}

static void
take_my_options_checkbutton_toggled(GtkToggleButton *button,
				    GtranslatorHeaderDialog *dlg)
{
  g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->take_my_options));

  
  gtranslator_prefs_manager_set_use_profile_values(gtk_toggle_button_get_active(button));
  
  gtk_widget_set_sensitive(dlg->priv->translator, !gtk_toggle_button_get_active(button));
  gtk_widget_set_sensitive(dlg->priv->tr_email, !gtk_toggle_button_get_active(button));
  gtk_widget_set_sensitive(dlg->priv->language, !gtk_toggle_button_get_active(button));
  gtk_widget_set_sensitive(dlg->priv->lg_email, !gtk_toggle_button_get_active(button));
  gtk_widget_set_sensitive(dlg->priv->encoding, !gtk_toggle_button_get_active(button));
  
}

static void
prj_comment_changed (GtkTextBuffer *buffer,
		     GtranslatorHeader *header)
{
        const gchar *text;
	GtkTextIter start, end;
	gchar *text_utf8;
	
	gtranslator_header_set_header_changed (header, TRUE);

	gtk_text_buffer_get_bounds(buffer, &start, &end);
	text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);	

	if (text)
	  gtranslator_header_set_comment(header, g_strdup(text));
}

static void
prj_id_version_changed(GObject    *gobject,
		    GParamSpec *arg1,
		    GtranslatorHeader *header)
{
	const gchar *text;

	gtranslator_header_set_header_changed (header, TRUE);

	text = gtk_entry_get_text(GTK_ENTRY(gobject));

	if (text)
		gtranslator_header_set_prj_id_version(header, g_strdup(text));
}

static void
rmbt_changed(GObject    *gobject,
		    GParamSpec *arg1,
		    GtranslatorHeader *header)
{
	const gchar *text;

	gtranslator_header_set_header_changed (header, TRUE);

	text = gtk_entry_get_text(GTK_ENTRY(gobject));

	if (text)
		gtranslator_header_set_rmbt(header, g_strdup(text));
}

static void
translator_changed(GObject    *gobject,
		    GParamSpec *arg1,
		    GtranslatorHeader *header)
{
	const gchar *text;

	gtranslator_header_set_header_changed (header, TRUE);

	text = gtk_entry_get_text(GTK_ENTRY(gobject));

	if (text)
		gtranslator_header_set_translator(header, g_strdup(text));
}

static void
tr_email_changed(GObject    *gobject,
		    GParamSpec *arg1,
		    GtranslatorHeader *header)
{
	const gchar *text;

	gtranslator_header_set_header_changed (header, TRUE);

	text = gtk_entry_get_text(GTK_ENTRY(gobject));

	if (text)
		gtranslator_header_set_tr_email(header, g_strdup(text));
}

static void
language_changed(GObject    *gobject,
		    GParamSpec *arg1,
		    GtranslatorHeader *header)
{
	const gchar *text;

	gtranslator_header_set_header_changed (header, TRUE);

	text = gtk_entry_get_text(GTK_ENTRY(gobject));

	if (text)
		gtranslator_header_set_language(header, g_strdup(text));
}

static void
lg_email_changed(GObject    *gobject,
		    GParamSpec *arg1,
		    GtranslatorHeader *header)
{
	const gchar *text;

	gtranslator_header_set_header_changed (header, TRUE);

	text = gtk_entry_get_text(GTK_ENTRY(gobject));
	
	if (text)
		gtranslator_header_set_lg_email(header, g_strdup(text));
}


static void 
gtranslator_header_dialog_fill_from_header (GtranslatorHeaderDialog *dlg, GtranslatorHeader *header)
{
	GtkTextBuffer *buffer;

	/*
	 * Project Information
	 */	
	buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(dlg->priv->prj_comment));
	gtk_text_buffer_set_text(buffer, gtranslator_header_get_comment(header), 
				strlen(gtranslator_header_get_comment(header)));

	gtk_entry_set_text(GTK_ENTRY(dlg->priv->prj_id_version), gtranslator_header_get_prj_id_version(header));
	gtk_entry_set_text(GTK_ENTRY(dlg->priv->pot_date), gtranslator_header_get_pot_date(header));
	gtk_entry_set_text(GTK_ENTRY(dlg->priv->po_date), gtranslator_header_get_po_date(header));
	gtk_entry_set_text(GTK_ENTRY(dlg->priv->rmbt), gtranslator_header_get_rmbt(header));

	/*
	 * Translator and Language Information
	 */
	gtk_entry_set_text(GTK_ENTRY(dlg->priv->translator), gtranslator_header_get_translator(header));
	gtk_entry_set_text(GTK_ENTRY(dlg->priv->tr_email), gtranslator_header_get_tr_email(header));
	gtk_entry_set_text(GTK_ENTRY(dlg->priv->language), gtranslator_header_get_language(header));
	gtk_entry_set_text(GTK_ENTRY(dlg->priv->lg_email), gtranslator_header_get_lg_email(header));
	gtk_entry_set_text(GTK_ENTRY(dlg->priv->charset), gtranslator_header_get_charset(header));
	gtk_entry_set_text(GTK_ENTRY(dlg->priv->encoding), gtranslator_header_get_encoding(header));
}

static void save_header(GtranslatorPo *po)
{
        GtranslatorHeader *header;

	header = gtranslator_po_get_header (po);

        if (gtranslator_header_get_header_changed (header))
	  gtranslator_po_set_state (po, GTR_PO_STATE_MODIFIED);
} 

static void gtranslator_header_dialog_init (GtranslatorHeaderDialog *dlg)
{
	gboolean ret;
	GtkWidget *error_widget;
	
	dlg->priv = GTR_HEADER_DIALOG_GET_PRIVATE (dlg);

	gtk_dialog_add_buttons (GTK_DIALOG (dlg),
				GTK_STOCK_CLOSE,
				GTK_RESPONSE_CLOSE,
				NULL);
	
	gtk_window_set_title (GTK_WINDOW (dlg), _("Edit Header"));
	gtk_window_set_resizable (GTK_WINDOW (dlg), FALSE);
	gtk_dialog_set_has_separator (GTK_DIALOG (dlg), FALSE);
	gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);

	gtk_container_set_border_width (GTK_CONTAINER (dlg), 5);
	gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (dlg)->vbox), 2);
	gtk_container_set_border_width (GTK_CONTAINER (GTK_DIALOG (dlg)->action_area), 5);
	gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (dlg)->action_area), 4);

	g_signal_connect (dlg,
			  "response",
			  G_CALLBACK (gtk_widget_destroy),
			  NULL);
	
	ret = gtranslator_utils_get_glade_widgets(PKGDATADIR"/header-dialog.glade",
		"main_box",
		&error_widget,
		"main_box", &dlg->priv->main_box,
		"notebook", &dlg->priv->notebook,
		"lang_vbox", &dlg->priv->lang_vbox,
		"prj_id_version", &dlg->priv->prj_id_version,
		"rmbt", &dlg->priv->rmbt,
		"prj_comment", &dlg->priv->prj_comment,
		"take_my_options", &dlg->priv->take_my_options,
		"tr_name", &dlg->priv->translator,
		"tr_email", &dlg->priv->tr_email,
		"pot_date", &dlg->priv->pot_date,
		"po_date", &dlg->priv->po_date,
		"language_entry", &dlg->priv->language,
		"lg_email_entry", &dlg->priv->lg_email, 
		"charset_entry", &dlg->priv->charset,
		"encoding_entry", &dlg->priv->encoding,
		NULL);

	if(!ret)
	{
		gtk_widget_show(error_widget);
		gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dlg)->vbox),
					     error_widget);
		
		return;
	}

	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dlg)->vbox),
			    dlg->priv->main_box, FALSE, FALSE, 0);
	
	gtk_container_set_border_width (GTK_CONTAINER (dlg->priv->notebook), 5);
	
	if (gtranslator_application_get_profiles (GTR_APP) == NULL)
	  {
	    gtk_widget_set_sensitive (dlg->priv->take_my_options, FALSE);
	  }
	else
	  {
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->take_my_options),
				     gtranslator_prefs_manager_get_use_profile_values());
	  }
	gtk_text_view_set_editable (GTK_TEXT_VIEW (dlg->priv->prj_comment), TRUE);

	gtk_widget_set_sensitive(dlg->priv->pot_date, FALSE);
	gtk_widget_set_sensitive(dlg->priv->po_date, FALSE);
	gtk_widget_set_sensitive(dlg->priv->charset, FALSE);

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (dlg->priv->take_my_options))) {
	  
	  gtk_widget_set_sensitive(dlg->priv->translator, !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (dlg->priv->take_my_options)));
	  gtk_widget_set_sensitive(dlg->priv->tr_email, !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (dlg->priv->take_my_options)));
	  gtk_widget_set_sensitive(dlg->priv->language, !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (dlg->priv->take_my_options)));
	  gtk_widget_set_sensitive(dlg->priv->lg_email, !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (dlg->priv->take_my_options)));
	  gtk_widget_set_sensitive(dlg->priv->encoding, !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (dlg->priv->take_my_options)));
	}

	/*Connect signals*/
	g_signal_connect(dlg->priv->take_my_options, "toggled",
			 G_CALLBACK(take_my_options_checkbutton_toggled),
			 dlg);	
}

void gtranslator_show_header_dialog (GtranslatorWindow *window)
{
	
	static GtkWidget *dlg = NULL;
	
	GtranslatorPo *po;
	GtranslatorTab *tab;
	GtranslatorHeader *header;
	GtkTextBuffer *buffer;

	tab = gtranslator_window_get_active_tab (window);
	po = gtranslator_tab_get_po (tab);

	g_return_if_fail(GTR_IS_WINDOW(window));

	/*
	 * Get header's values from tab in window
	 */
	header = gtranslator_window_get_header_from_active_tab(window);
	
	if(dlg == NULL)
	{
		dlg = GTK_WIDGET (g_object_new (GTR_TYPE_HEADER_DIALOG, NULL));
		g_signal_connect (dlg,
				  "destroy",
				  G_CALLBACK (gtk_widget_destroyed),
				  &dlg);
		gtk_widget_show_all(dlg);
	}

	/*
	 * Connect signal to save header in msg
	 */
	g_signal_connect_swapped (dlg, "destroy",
			 G_CALLBACK(save_header),
			 po);

	/*
	 * Write header's values on Header dialog
	 */	
	gtranslator_header_dialog_fill_from_header(GTR_HEADER_DIALOG(dlg), header);
	
	if (GTK_WINDOW (window) != gtk_window_get_transient_for (GTK_WINDOW (dlg)))
	{
		gtk_window_set_transient_for (GTK_WINDOW (dlg),
					      GTK_WINDOW (window));
	}
	
	gtk_window_present (GTK_WINDOW (dlg));

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (GTR_HEADER_DIALOG (dlg)->priv->prj_comment));

	g_signal_connect(buffer, "changed",
			 G_CALLBACK(prj_comment_changed),
			 header);

	/*
         * Connect signals to edit Project information on Header dialog
         */
	g_signal_connect(GTR_HEADER_DIALOG(dlg)->priv->prj_id_version, "notify::text",
			 G_CALLBACK(prj_id_version_changed),
			 header);

	g_signal_connect(GTR_HEADER_DIALOG(dlg)->priv->rmbt, "notify::text",
			 G_CALLBACK(rmbt_changed),
			 header);
	/*
         * Connect signals to edit Translator and Language information on Header dialog
         */
	g_signal_connect(GTR_HEADER_DIALOG(dlg)->priv->translator, "notify::text",
			 G_CALLBACK(translator_changed),
			 header);

	g_signal_connect(GTR_HEADER_DIALOG(dlg)->priv->tr_email, "notify::text",
			 G_CALLBACK(tr_email_changed),
			 header);

	g_signal_connect(GTR_HEADER_DIALOG(dlg)->priv->language, "notify::text",
			 G_CALLBACK(language_changed),
			 header);

	g_signal_connect(GTR_HEADER_DIALOG(dlg)->priv->lg_email, "notify::text",
			 G_CALLBACK(lg_email_changed),
			 header);	
}

