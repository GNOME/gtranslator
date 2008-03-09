/*
 * Copyright (C) 2007  Pablo Sanxiao <psanxiao@gmail.com>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
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

#include "profile-dialog.h"
#include "preferences-dialog.h"
#include "utils.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>


#define GTR_PROFILE_DIALOG_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 	(object),	\
						 	GTR_TYPE_PROFILE_DIALOG,     \
						 	GtranslatorProfileDialogPrivate))
#define PIXMAPSDIR "/usr/local/share/pixmaps/gtranslator"

G_DEFINE_TYPE(GtranslatorProfileDialog, gtranslator_profile_dialog, GTK_TYPE_DIALOG)

struct _GtranslatorProfileDialogPrivate
{
	GtkWidget *main_box;
	
	GtkWidget *profile_name_entry;

	GtkWidget *author_name_entry;
	GtkWidget *author_email_entry;

	GtkWidget *language_name_entry;
	GtkWidget *language_code_entry;
	GtkWidget *charset_entry;
	GtkWidget *encoding_entry;
	GtkWidget *language_email_entry;
	GtkWidget *plurals_number_spinbutton;
	GtkWidget *plurals_forms_entry;
};

static void gtranslator_profile_dialog_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_profile_dialog_parent_class)->finalize (object);
}

static void gtranslator_profile_dialog_class_init (GtranslatorProfileDialogClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorProfileDialogPrivate));

	object_class->finalize = gtranslator_profile_dialog_finalize;
}

static void gtranslator_profile_dialog_init (GtranslatorProfileDialog *dlg)
{
	gboolean ret;
	GtkWidget *error_widget;
	
	dlg->priv = GTR_PROFILE_DIALOG_GET_PRIVATE (dlg);

	gtk_dialog_add_buttons (GTK_DIALOG (dlg),
				GTK_STOCK_OK,
				GTK_RESPONSE_OK,
				NULL);

	gtk_dialog_add_buttons (GTK_DIALOG (dlg),
				GTK_STOCK_CANCEL,
				GTK_RESPONSE_CANCEL,
				NULL);
	
	gtk_window_set_title (GTK_WINDOW (dlg), _("gtranslator Profile"));
	gtk_window_set_resizable (GTK_WINDOW (dlg), FALSE);
	gtk_dialog_set_has_separator (GTK_DIALOG (dlg), FALSE);
	gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);

	gtk_container_set_border_width (GTK_CONTAINER (dlg), 5);
	gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (dlg)->vbox), 2);

	g_signal_connect (dlg,
			  "response",
			  G_CALLBACK (gtk_widget_destroy),
			  NULL);
	
	ret = gtranslator_utils_get_glade_widgets(DATADIR"/profile-dialog.glade",
		"profiles_dialog",
		&error_widget,
		"profiles_dialog", &dlg->priv->main_box,
		"profile_entry", &dlg->priv->profile_name_entry,
		"name_entry", &dlg->priv->author_name_entry,
		"email_entry", &dlg->priv->author_email_entry,
		"language_entry", &dlg->priv->language_name_entry,
		"langcode_entry", &dlg->priv->language_code_entry,
		"charset_entry", &dlg->priv->charset_entry,
		"encoding_entry", &dlg->priv->encoding_entry,
		"team_email_entry", &dlg->priv->language_email_entry,
		"plurals_spinbutton", &dlg->priv->plurals_number_spinbutton,
		"plurals_entry", &dlg->priv->plurals_forms_entry,
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
	gtk_window_set_modal (GTK_WINDOW (dlg), TRUE);
}

void gtranslator_show_profile_dialog (GtranslatorPreferencesDialog *dialog)
{
	
	static GtkWidget *dlg = NULL;

	g_return_if_fail(GTR_IS_PREFERENCES_DIALOG(dialog));
	
	if(dlg == NULL)
	{
		dlg = GTK_WIDGET (g_object_new (GTR_TYPE_PROFILE_DIALOG, NULL));
		g_signal_connect (dlg,
				  "destroy",
				  G_CALLBACK (gtk_widget_destroyed),
				  &dlg);
		gtk_widget_show_all(dlg);
	}
	
	if (GTK_WINDOW (dialog) != gtk_window_get_transient_for (GTK_WINDOW (dlg)))
	{
		gtk_window_set_transient_for (GTK_WINDOW (dlg),
					      GTK_WINDOW (dialog));
	}
	
	gtk_window_present (GTK_WINDOW (dlg));	
}

