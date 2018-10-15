/*
 * Copyright (C) 2007  Pablo Sanxiao <psanxiao@gmail.com>
 *               2010  Ignacio Casal Quinteiro <icq@gnome.org>
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

#include "gtr-application.h"
#include "gtr-dirs.h"
#include "gtr-header-dialog.h"
#include "gtr-utils.h"
#include "gtr-settings.h"
#include "gtr-po.h"

typedef struct
{
  GSettings *settings;

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
  GtkWidget *lang_code;
  GtkWidget *lg_email;
  GtkWidget *charset;
  GtkWidget *encoding;

  GtrPo     *po;
} GtrHeaderDialogPrivate;

struct _GtrHeaderDialog
{
  GtkDialog parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrHeaderDialog, gtr_header_dialog, GTK_TYPE_DIALOG)

static void
gtr_header_dialog_dispose (GObject * object)
{
  GtrHeaderDialog *dlg = GTR_HEADER_DIALOG (object);
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);

  g_clear_object (&priv->settings);

  G_OBJECT_CLASS (gtr_header_dialog_parent_class)->dispose (object);
}

static void
gtr_header_dialog_class_init (GtrHeaderDialogClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gtr_header_dialog_dispose;
}

static void
take_my_options_checkbutton_toggled (GtkToggleButton * button,
                                     GtrHeaderDialog * dlg)
{
  gboolean active;
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);

  g_return_if_fail (button == GTK_TOGGLE_BUTTON (priv->take_my_options));

  active = gtk_toggle_button_get_active (button);

  g_settings_set_boolean (priv->settings, GTR_SETTINGS_USE_PROFILE_VALUES,
                          active);

  gtk_widget_set_sensitive (priv->translator, !active);
  gtk_widget_set_sensitive (priv->tr_email, !active);
  gtk_widget_set_sensitive (priv->language, !active);
  gtk_widget_set_sensitive (priv->lang_code, !active);
  gtk_widget_set_sensitive (priv->lg_email, !active);
  gtk_widget_set_sensitive (priv->encoding, !active);
}

static void
po_state_set_modified (GtrPo * po)
{
  if (gtr_po_get_state (po) != GTR_PO_STATE_MODIFIED)
    gtr_po_set_state (po, GTR_PO_STATE_MODIFIED);
}

static void
prj_comment_changed (GtkTextBuffer * buffer, GtrHeaderDialog * dlg)
{
  GtkTextIter start, end;
  gchar *text;
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);

  gtk_text_buffer_get_bounds (buffer, &start, &end);
  text = gtk_text_buffer_get_text (buffer, &start, &end, TRUE);

  if (text)
    {
      gtr_header_set_comments (gtr_po_get_header (priv->po), text);
      g_free (text);
      po_state_set_modified (priv->po);
    }
}

static void
prj_id_version_changed (GtkWidget * widget, GtrHeaderDialog * dlg)
{
  const gchar *text;
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);

  text = gtk_entry_get_text (GTK_ENTRY (widget));

  if (text)
    {
      gtr_header_set_prj_id_version (gtr_po_get_header (priv->po), text);
      po_state_set_modified (priv->po);
    }
}

static void
rmbt_changed (GtkWidget * widget, GtrHeaderDialog * dlg)
{
  const gchar *text;
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);

  text = gtk_entry_get_text (GTK_ENTRY (widget));

  if (text)
    {
      gtr_header_set_rmbt (gtr_po_get_header (priv->po), text);
      po_state_set_modified (priv->po);
    }
}

static void
translator_changed (GtkWidget * widget, GtrHeaderDialog * dlg)
{
  const gchar *name, *email;
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);

  name = gtk_entry_get_text (GTK_ENTRY (priv->translator));
  email = gtk_entry_get_text (GTK_ENTRY (priv->tr_email));

  if (name && email)
    {
      gtr_header_set_translator (gtr_po_get_header (priv->po), name, email);
      po_state_set_modified (priv->po);
    }
}

static void
language_changed (GtkWidget * widget, GtrHeaderDialog * dlg)
{
  const gchar *language, *lg_email, *lang_code;
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);

  language = gtk_entry_get_text (GTK_ENTRY (priv->language));
  lg_email = gtk_entry_get_text (GTK_ENTRY (priv->lg_email));
  lang_code = gtk_entry_get_text (GTK_ENTRY (priv->lang_code));

  if (language && lg_email)
    {
      gtr_header_set_language (gtr_po_get_header (priv->po),
                               language, lang_code,
                               lg_email);
      po_state_set_modified (priv->po);
    }
}

static void
gtr_header_dialog_fill_from_header (GtrHeaderDialog * dlg)
{
  GtrHeader *header;
  GtkTextBuffer *buffer;
  gchar *text;
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);

  header = gtr_po_get_header (priv->po);

  /* Project Information */
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (priv->prj_comment));
  gtk_text_buffer_set_text (buffer, gtr_header_get_comments (header), -1);

  text = gtr_header_get_prj_id_version (header);
  gtk_entry_set_text (GTK_ENTRY (priv->prj_id_version), text);
  g_free (text);

  text = gtr_header_get_pot_date (header);
  gtk_entry_set_text (GTK_ENTRY (priv->pot_date), text);
  g_free (text);

  text = gtr_header_get_po_date (header);
  gtk_entry_set_text (GTK_ENTRY (priv->po_date), text);
  g_free (text);

  text = gtr_header_get_rmbt (header);
  gtk_entry_set_text (GTK_ENTRY (priv->rmbt), text);
  g_free (text);

  /* Translator and Language Information */
  text = gtr_header_get_translator (header);
  gtk_entry_set_text (GTK_ENTRY (priv->translator), text);
  g_free (text);

  text = gtr_header_get_tr_email (header);
  gtk_entry_set_text (GTK_ENTRY (priv->tr_email), text);
  g_free (text);

  text = gtr_header_get_language (header);
  gtk_entry_set_text (GTK_ENTRY (priv->language), text);
  g_free (text);

  text = gtr_header_get_language_code (header);
  gtk_entry_set_text (GTK_ENTRY (priv->lang_code), text);
  g_free (text);

  text = gtr_header_get_lg_email (header);
  gtk_entry_set_text (GTK_ENTRY (priv->lg_email), text);
  g_free (text);

  text = gtr_header_get_charset (header);
  gtk_entry_set_text (GTK_ENTRY (priv->charset), text);
  g_free (text);

  text = gtr_header_get_encoding (header);
  gtk_entry_set_text (GTK_ENTRY (priv->encoding), text);
  g_free (text);
}

static void
gtr_header_dialog_init (GtrHeaderDialog * dlg)
{
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);
  GtkBox *content_area;
  GtkBuilder *builder;
  gchar *root_objects[] = {
    "main_box",
    NULL
  };

  priv->settings = g_settings_new ("org.gnome.gtranslator.preferences.files");

  gtk_dialog_add_buttons (GTK_DIALOG (dlg),
                          _("_Close"), GTK_RESPONSE_CLOSE, NULL);

  gtk_window_set_title (GTK_WINDOW (dlg), _("Edit Header"));
  gtk_window_set_resizable (GTK_WINDOW (dlg), FALSE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);

  content_area = GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dlg)));

  gtk_container_set_border_width (GTK_CONTAINER (dlg), 5);
  gtk_box_set_spacing (content_area, 2);

  g_signal_connect (dlg, "response", G_CALLBACK (gtk_widget_destroy), NULL);

  builder = gtk_builder_new ();
  gtk_builder_add_objects_from_resource (builder, "/org/gnome/translator/gtr-header-dialog.ui",
                                         root_objects, NULL);

  priv->main_box = GTK_WIDGET (gtk_builder_get_object (builder, "main_box"));
  g_object_ref (priv->main_box);
  priv->notebook = GTK_WIDGET (gtk_builder_get_object (builder, "notebook"));
  priv->lang_vbox = GTK_WIDGET (gtk_builder_get_object (builder, "lang_vbox"));
  priv->prj_id_version = GTK_WIDGET (gtk_builder_get_object (builder, "prj_id_version"));
  priv->rmbt = GTK_WIDGET (gtk_builder_get_object (builder, "rmbt"));
  priv->prj_comment = GTK_WIDGET (gtk_builder_get_object (builder, "prj_comment"));
  priv->take_my_options = GTK_WIDGET (gtk_builder_get_object (builder, "take_my_options"));
  priv->translator = GTK_WIDGET (gtk_builder_get_object (builder, "tr_name"));
  priv->tr_email = GTK_WIDGET (gtk_builder_get_object (builder, "tr_email"));
  priv->pot_date = GTK_WIDGET (gtk_builder_get_object (builder, "pot_date"));
  priv->po_date = GTK_WIDGET (gtk_builder_get_object (builder, "po_date"));
  priv->language = GTK_WIDGET (gtk_builder_get_object (builder, "language_entry"));
  priv->lang_code = GTK_WIDGET (gtk_builder_get_object (builder, "language_code_entry"));
  priv->lg_email = GTK_WIDGET (gtk_builder_get_object (builder, "lg_email_entry"));
  priv->charset = GTK_WIDGET (gtk_builder_get_object (builder, "charset_entry"));
  priv->encoding = GTK_WIDGET (gtk_builder_get_object (builder, "encoding_entry"));
  g_object_unref (builder);

  gtk_box_pack_start (content_area, priv->main_box, FALSE, FALSE, 0);

  gtk_container_set_border_width (GTK_CONTAINER (priv->notebook), 5);

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (priv->take_my_options),
                                g_settings_get_boolean (priv->settings,
                                                        GTR_SETTINGS_USE_PROFILE_VALUES));

  gtk_text_view_set_editable (GTK_TEXT_VIEW (priv->prj_comment), TRUE);

  gtk_widget_set_sensitive (priv->pot_date, FALSE);
  gtk_widget_set_sensitive (priv->po_date, FALSE);
  gtk_widget_set_sensitive (priv->charset, FALSE);

  if (gtk_toggle_button_get_active
      (GTK_TOGGLE_BUTTON (priv->take_my_options)))
    {
      gboolean active;

      active =
        gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
                                      (priv->take_my_options));

      gtk_widget_set_sensitive (priv->translator, !active);
      gtk_widget_set_sensitive (priv->tr_email, !active);
      gtk_widget_set_sensitive (priv->language, !active);
      gtk_widget_set_sensitive (priv->lang_code, !active);
      gtk_widget_set_sensitive (priv->lg_email, !active);
      gtk_widget_set_sensitive (priv->encoding, !active);
    }
}

static void
set_default_values (GtrHeaderDialog * dlg, GtrWindow * window)
{
  GtkTextBuffer *buffer;
  GtrTab *tab;
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);

  /* Write header's values on Header dialog */
  tab = gtr_window_get_active_tab (window);
  priv->po = gtr_tab_get_po (tab);
  gtr_header_dialog_fill_from_header (GTR_HEADER_DIALOG (dlg));

  /*Connect signals */
  g_signal_connect (priv->take_my_options, "toggled",
                    G_CALLBACK (take_my_options_checkbutton_toggled), dlg);

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (priv->prj_comment));

  g_signal_connect (buffer, "changed", G_CALLBACK (prj_comment_changed), dlg);

  /* Connect signals to edit Project information on Header dialog */
  g_signal_connect (priv->prj_id_version, "changed",
                    G_CALLBACK (prj_id_version_changed), dlg);

  g_signal_connect (priv->rmbt, "changed",
                    G_CALLBACK (rmbt_changed), dlg);

  /* Connect signals to edit Translator and Language information on Header dialog */
  g_signal_connect (priv->translator, "changed",
                    G_CALLBACK (translator_changed), dlg);

  g_signal_connect (priv->tr_email, "changed",
                    G_CALLBACK (translator_changed), dlg);

  g_signal_connect (priv->language, "changed",
                    G_CALLBACK (language_changed), dlg);

  g_signal_connect (priv->lang_code, "changed",
                    G_CALLBACK (language_changed), dlg);

  g_signal_connect (priv->lg_email, "changed",
                    G_CALLBACK (language_changed), dlg);
}

void
gtr_show_header_dialog (GtrWindow * window)
{
  static GtkWidget *dlg = NULL;
  GtrTab *tab;

  tab = gtr_window_get_active_tab (window);
  g_return_if_fail (tab != NULL);

  g_return_if_fail (GTR_IS_WINDOW (window));

  if (dlg == NULL)
    {
      dlg = GTK_WIDGET (g_object_new (GTR_TYPE_HEADER_DIALOG,
                                      "use-header-bar", TRUE, NULL));

      g_signal_connect (dlg,
                        "destroy", G_CALLBACK (gtk_widget_destroyed), &dlg);

      set_default_values (GTR_HEADER_DIALOG (dlg), window);

      gtk_widget_show (GTK_WIDGET (dlg));
    }

  gtk_window_set_transient_for (GTK_WINDOW (dlg), GTK_WINDOW (window));
  gtk_window_set_type_hint (GTK_WINDOW (dlg), GDK_WINDOW_TYPE_HINT_DIALOG);
  gtk_window_set_modal (GTK_WINDOW (dlg), TRUE);

  gtk_window_present (GTK_WINDOW (dlg));
}
