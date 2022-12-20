/*
 * Copyright (C) 2007  Pablo Sanxiao <psanxiao@gmail.com>
 *               2010  Ignacio Casal Quinteiro <icq@gnome.org>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

  GtkWidget *team;
  GtkWidget *module;
  GtkWidget *branch;
  GtkWidget *domain;
  GtkWidget *state;

  GtrPo     *po;
} GtrHeaderDialogPrivate;

struct _GtrHeaderDialog
{
  GtkWindow parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrHeaderDialog, gtr_header_dialog, GTK_TYPE_WINDOW)

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
take_my_options_checkbutton_toggled (GtkCheckButton * button,
                                     GtrHeaderDialog * dlg)
{
  gboolean active;
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);

  g_return_if_fail (button == GTK_CHECK_BUTTON (priv->take_my_options));

  active = gtk_check_button_get_active (button);

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

  //text = gtk_entry_get_text (GTK_ENTRY (widget));
  GtkEntryBuffer *buffer = gtk_entry_get_buffer (GTK_ENTRY(widget));
  text = gtk_entry_buffer_get_text (buffer);

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

  //text = gtk_entry_get_text (GTK_ENTRY (widget));
  GtkEntryBuffer *buffer = gtk_entry_get_buffer (GTK_ENTRY(widget));
  text = gtk_entry_buffer_get_text (buffer);

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

  GtkEntryBuffer *buffer = gtk_entry_get_buffer (GTK_ENTRY(widget));
  name = gtk_entry_buffer_get_text (buffer);
  buffer = gtk_entry_get_buffer (GTK_ENTRY(widget));
  email = gtk_entry_buffer_get_text (buffer);

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

  // language = gtk_entry_get_text (GTK_ENTRY (priv->language));
  language = gtk_entry_buffer_get_text (gtk_entry_get_buffer (GTK_ENTRY (priv->language)));

  // lg_email = gtk_entry_get_text (GTK_ENTRY (priv->lg_email));
  lg_email = gtk_entry_buffer_get_text (gtk_entry_get_buffer (GTK_ENTRY (priv->lg_email)));

  // lang_code = gtk_entry_get_text (GTK_ENTRY (priv->lang_code));
  lang_code = gtk_entry_buffer_get_text (gtk_entry_get_buffer (GTK_ENTRY (priv->lang_code)));

  if (language && lg_email)
    {
      gtr_header_set_language (gtr_po_get_header (priv->po),
                               language, lang_code,
                               lg_email);
      po_state_set_modified (priv->po);
    }
}

static void
dl_changed (GtkWidget * widget, GtrHeaderDialog * dlg)
{
  const gchar *team, *module, *branch, *domain;
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);

  //team = gtk_entry_get_text (GTK_ENTRY (priv->team));
  GtkEntryBuffer *buffer = gtk_entry_get_buffer (GTK_ENTRY(widget));
  team = gtk_entry_buffer_get_text (buffer);
  //module = gtk_entry_get_text (GTK_ENTRY (priv->module));
  buffer = gtk_entry_get_buffer (GTK_ENTRY(widget));
  module = gtk_entry_buffer_get_text (buffer);
  //branch = gtk_entry_get_text (GTK_ENTRY (priv->branch));
  buffer = gtk_entry_get_buffer (GTK_ENTRY(widget));
  branch = gtk_entry_buffer_get_text (buffer);
  //domain = gtk_entry_get_text (GTK_ENTRY (priv->domain));
  buffer = gtk_entry_get_buffer (GTK_ENTRY(widget));
  domain = gtk_entry_buffer_get_text (buffer);

  gtr_header_set_dl_info (gtr_po_get_header (priv->po),
                          team, module, branch,
                          domain);

  po_state_set_modified (priv->po);
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
  //gtk_entry_set_text (GTK_ENTRY (priv->prj_id_version), text);
  GtkEntryBuffer *entry_buffer = gtk_entry_get_buffer (GTK_ENTRY(priv->prj_id_version));
  gtk_entry_buffer_set_text (entry_buffer, text, -1);
  g_free (text);

  text = gtr_header_get_pot_date (header);
  //gtk_entry_set_text (GTK_ENTRY (priv->pot_date), text);
  entry_buffer = gtk_entry_get_buffer (GTK_ENTRY(priv->pot_date));
  gtk_entry_buffer_set_text (entry_buffer, text, -1);
  g_free (text);

  text = gtr_header_get_po_date (header);
  //gtk_entry_set_text (GTK_ENTRY (priv->po_date), text);
  entry_buffer = gtk_entry_get_buffer (GTK_ENTRY(priv->po_date));
  gtk_entry_buffer_set_text (entry_buffer, text, -1);
  g_free (text);

  text = gtr_header_get_rmbt (header);
  //gtk_entry_set_text (GTK_ENTRY (priv->rmbt), text);
  entry_buffer = gtk_entry_get_buffer (GTK_ENTRY(priv->rmbt));
  gtk_entry_buffer_set_text (entry_buffer, text, -1);
  g_free (text);

  /* Translator and Language Information */
  text = gtr_header_get_translator (header);
  //gtk_entry_set_text (GTK_ENTRY (priv->translator), text);
  entry_buffer = gtk_entry_get_buffer (GTK_ENTRY(priv->translator));
  gtk_entry_buffer_set_text (entry_buffer, text, -1);
  g_free (text);

  text = gtr_header_get_tr_email (header);
  //gtk_entry_set_text (GTK_ENTRY (priv->tr_email), text);
  entry_buffer = gtk_entry_get_buffer (GTK_ENTRY(priv->tr_email));
  gtk_entry_buffer_set_text (entry_buffer, text, -1);
  g_free (text);

  text = gtr_header_get_language (header);
  //gtk_entry_set_text (GTK_ENTRY (priv->language), text);
  entry_buffer = gtk_entry_get_buffer (GTK_ENTRY(priv->language));
  gtk_entry_buffer_set_text (entry_buffer, text, -1);
  g_free (text);

  text = gtr_header_get_language_code (header);
  //gtk_entry_set_text (GTK_ENTRY (priv->lang_code), text);
  entry_buffer = gtk_entry_get_buffer (GTK_ENTRY(priv->lang_code));
  gtk_entry_buffer_set_text (entry_buffer, text, -1);
  g_free (text);

  text = gtr_header_get_lg_email (header);
  //gtk_entry_set_text (GTK_ENTRY (priv->lg_email), text);
  entry_buffer = gtk_entry_get_buffer (GTK_ENTRY(priv->lg_email));
  gtk_entry_buffer_set_text (entry_buffer, text, -1);
  g_free (text);

  text = gtr_header_get_charset (header);
  //gtk_entry_set_text (GTK_ENTRY (priv->charset), text);
  entry_buffer = gtk_entry_get_buffer (GTK_ENTRY(priv->charset));
  gtk_entry_buffer_set_text (entry_buffer, text, -1);
  g_free (text);

  text = gtr_header_get_encoding (header);
  //gtk_entry_set_text (GTK_ENTRY (priv->encoding), text);
  entry_buffer = gtk_entry_get_buffer (GTK_ENTRY(priv->encoding));
  gtk_entry_buffer_set_text (entry_buffer, text, -1);
  g_free (text);

  /* Damned Lies Information */
  text = gtr_header_get_dl_team (header);
  if (text)
    {
      entry_buffer = gtk_entry_get_buffer (GTK_ENTRY(priv->team));
      gtk_entry_buffer_set_text (entry_buffer, text, -1);
      g_free (text);
    }

  text = gtr_header_get_dl_module (header);
  if (text)
    {
      entry_buffer = gtk_entry_get_buffer (GTK_ENTRY(priv->module));
      gtk_entry_buffer_set_text (entry_buffer, text, -1);
      g_free (text);
    }

  text = gtr_header_get_dl_branch (header);
  if (text)
    {
      entry_buffer = gtk_entry_get_buffer (GTK_ENTRY(priv->branch));
      gtk_entry_buffer_set_text (entry_buffer, text, -1);
      g_free (text);
    }

  text = gtr_header_get_dl_domain (header);
  if (text)
    {
      entry_buffer = gtk_entry_get_buffer (GTK_ENTRY(priv->domain));
      gtk_entry_buffer_set_text (entry_buffer, text, -1);
      g_free (text);
    }

  text = gtr_header_get_dl_state (header);
  if (text)
    {
      entry_buffer = gtk_entry_get_buffer (GTK_ENTRY(priv->state));
      gtk_entry_buffer_set_text (entry_buffer, text, -1);
      g_free (text);
    }
}

static void
gtr_header_dialog_init (GtrHeaderDialog * dlg)
{
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);
  GtkBox *content_area;
  GtkBuilder *builder;

  GError *error = NULL;
  const char *root_objects[] = {
    "main_box",
    NULL
  };

  priv->settings = g_settings_new ("org.gnome.gtranslator.preferences.files");

  gtk_window_set_title (GTK_WINDOW (dlg), _("Edit Header"));
  gtk_window_set_resizable (GTK_WINDOW (dlg), FALSE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);

  content_area = GTK_BOX (gtk_box_new (GTK_ORIENTATION_VERTICAL, 6));
  gtk_window_set_child (GTK_WINDOW (dlg), GTK_WIDGET (content_area));

  builder = gtk_builder_new ();
  gtk_builder_add_objects_from_resource (
    builder, "/org/gnome/translator/gtr-header-dialog.ui",
    root_objects,
    &error
  );

  if (error != NULL)
    {
      g_warning ("Error parsing gtr-header-dialog.ui: %s", (error)->message);
      g_error_free (error);
    }

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
  priv->team = GTK_WIDGET (gtk_builder_get_object (builder, "team"));
  priv->module = GTK_WIDGET (gtk_builder_get_object (builder, "module"));
  priv->branch = GTK_WIDGET (gtk_builder_get_object (builder, "branch"));
  priv->domain = GTK_WIDGET (gtk_builder_get_object (builder, "domain"));
  priv->state = GTK_WIDGET (gtk_builder_get_object (builder, "state"));

  g_object_unref (builder);

  gtk_box_append (content_area, priv->main_box);

  gtk_widget_set_margin_start (priv->notebook, 6);
  gtk_widget_set_margin_end (priv->notebook, 6);
  gtk_widget_set_margin_top (priv->notebook, 6);
  gtk_widget_set_margin_bottom (priv->notebook, 6);

  gtk_check_button_set_active (GTK_CHECK_BUTTON (priv->take_my_options),
                               g_settings_get_boolean (priv->settings,
                                                       GTR_SETTINGS_USE_PROFILE_VALUES));

  gtk_text_view_set_editable (GTK_TEXT_VIEW (priv->prj_comment), TRUE);

  gtk_widget_set_sensitive (priv->pot_date, FALSE);
  gtk_widget_set_sensitive (priv->po_date, FALSE);
  gtk_widget_set_sensitive (priv->charset, FALSE);
  gtk_widget_set_sensitive (priv->state, FALSE);

  if (gtk_check_button_get_active
      (GTK_CHECK_BUTTON (priv->take_my_options)))
    {
      gboolean active;

      active =
        gtk_check_button_get_active (GTK_CHECK_BUTTON
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

  gtk_window_set_transient_for (GTK_WINDOW (dlg), GTK_WINDOW (window));

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

  g_signal_connect (priv->team, "changed",
                    G_CALLBACK (dl_changed), dlg);

  g_signal_connect (priv->module, "changed",
                    G_CALLBACK (dl_changed), dlg);

  g_signal_connect (priv->branch, "changed",
                    G_CALLBACK (dl_changed), dlg);

  g_signal_connect (priv->domain, "changed",
                    G_CALLBACK (dl_changed), dlg);
}

void
gtr_show_header_dialog (GtrWindow * window)
{
  GtkWidget *dlg = NULL;
  GtrTab *tab = NULL;

  g_return_if_fail (GTR_IS_WINDOW (window));
  tab = gtr_window_get_active_tab (window);
  g_return_if_fail (tab != NULL);

  dlg = GTK_WIDGET (g_object_new (GTR_TYPE_HEADER_DIALOG,
                                  "use-header-bar", TRUE, NULL));
  set_default_values (GTR_HEADER_DIALOG (dlg), window);

  gtk_window_set_modal (GTK_WINDOW (dlg), TRUE);
  gtk_window_present (GTK_WINDOW (dlg));
}
