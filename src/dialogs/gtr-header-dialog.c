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

#define GTR_HEADER_DIALOG_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),	\
						 GTR_TYPE_HEADER_DIALOG,     \
						 GtrHeaderDialogPrivate))

G_DEFINE_TYPE (GtrHeaderDialog, gtr_header_dialog, GTK_TYPE_DIALOG)

struct _GtrHeaderDialogPrivate
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
  GtkWidget *lg_email;
  GtkWidget *charset;
  GtkWidget *encoding;

  GtrHeader *header;
};

static void
gtr_header_dialog_dispose (GObject * object)
{
  GtrHeaderDialog *dlg = GTR_HEADER_DIALOG (object);

  if (dlg->priv->settings != NULL)
    {
      g_object_unref (dlg->priv->settings);
      dlg->priv->settings = NULL;
    }

  G_OBJECT_CLASS (gtr_header_dialog_parent_class)->dispose (object);
}

static void
gtr_header_dialog_class_init (GtrHeaderDialogClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrHeaderDialogPrivate));

  object_class->dispose = gtr_header_dialog_dispose;
}

static void
take_my_options_checkbutton_toggled (GtkToggleButton * button,
                                     GtrHeaderDialog * dlg)
{
  gboolean active;

  g_return_if_fail (button == GTK_TOGGLE_BUTTON (dlg->priv->take_my_options));

  active = gtk_toggle_button_get_active (button);

  g_settings_set_boolean (dlg->priv->settings, GTR_SETTINGS_USE_PROFILE_VALUES,
                          active);

  gtk_widget_set_sensitive (dlg->priv->translator, !active);
  gtk_widget_set_sensitive (dlg->priv->tr_email, !active);
  gtk_widget_set_sensitive (dlg->priv->language, !active);
  gtk_widget_set_sensitive (dlg->priv->lg_email, !active);
  gtk_widget_set_sensitive (dlg->priv->encoding, !active);
}

static void
prj_comment_changed (GtkTextBuffer * buffer, GtrHeaderDialog * dlg)
{
  GtkTextIter start, end;
  gchar *text;

  gtk_text_buffer_get_bounds (buffer, &start, &end);
  text = gtk_text_buffer_get_text (buffer, &start, &end, TRUE);

  if (text)
    {
      gtr_header_set_comments (dlg->priv->header, text);
      g_free (text);
    }
}

static void
prj_id_version_changed (GtkWidget * widget, GtrHeaderDialog * dlg)
{
  const gchar *text;

  text = gtk_entry_get_text (GTK_ENTRY (widget));

  if (text)
    gtr_header_set_prj_id_version (dlg->priv->header, text);
}

static void
rmbt_changed (GtkWidget * widget, GtrHeaderDialog * dlg)
{
  const gchar *text;

  text = gtk_entry_get_text (GTK_ENTRY (widget));

  if (text)
    gtr_header_set_rmbt (dlg->priv->header, text);
}

static void
translator_changed (GtkWidget * widget, GtrHeaderDialog * dlg)
{
  const gchar *name, *email;

  name = gtk_entry_get_text (GTK_ENTRY (dlg->priv->translator));
  email = gtk_entry_get_text (GTK_ENTRY (dlg->priv->tr_email));

  if (name && email)
    gtr_header_set_translator (dlg->priv->header, name, email);
}

static void
language_changed (GtkWidget * widget, GtrHeaderDialog * dlg)
{
  const gchar *language, *lg_email;

  language = gtk_entry_get_text (GTK_ENTRY (dlg->priv->language));
  lg_email = gtk_entry_get_text (GTK_ENTRY (dlg->priv->lg_email));

  if (language && lg_email)
    gtr_header_set_language (dlg->priv->header, language, lg_email);
}

static void
gtr_header_dialog_fill_from_header (GtrHeaderDialog * dlg)
{
  GtrHeader *header;
  GtkTextBuffer *buffer;
  gchar *text;

  header = dlg->priv->header;

  /* Project Information */
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (dlg->priv->prj_comment));
  gtk_text_buffer_set_text (buffer, gtr_header_get_comments (header), -1);

  text = gtr_header_get_prj_id_version (header);
  gtk_entry_set_text (GTK_ENTRY (dlg->priv->prj_id_version), text);
  g_free (text);

  text = gtr_header_get_pot_date (header);
  gtk_entry_set_text (GTK_ENTRY (dlg->priv->pot_date), text);
  g_free (text);

  text = gtr_header_get_po_date (header);
  gtk_entry_set_text (GTK_ENTRY (dlg->priv->po_date), text);
  g_free (text);

  text = gtr_header_get_rmbt (header);
  gtk_entry_set_text (GTK_ENTRY (dlg->priv->rmbt), text);
  g_free (text);

  /* Translator and Language Information */
  text = gtr_header_get_translator (header);
  gtk_entry_set_text (GTK_ENTRY (dlg->priv->translator), text);
  g_free (text);

  text = gtr_header_get_tr_email (header);
  gtk_entry_set_text (GTK_ENTRY (dlg->priv->tr_email), text);
  g_free (text);

  text = gtr_header_get_language (header);
  gtk_entry_set_text (GTK_ENTRY (dlg->priv->language), text);
  g_free (text);

  text = gtr_header_get_lg_email (header);
  gtk_entry_set_text (GTK_ENTRY (dlg->priv->lg_email), text);
  g_free (text);

  text = gtr_header_get_charset (header);
  gtk_entry_set_text (GTK_ENTRY (dlg->priv->charset), text);
  g_free (text);

  text = gtr_header_get_encoding (header);
  gtk_entry_set_text (GTK_ENTRY (dlg->priv->encoding), text);
  g_free (text);
}

static void
gtr_header_dialog_init (GtrHeaderDialog * dlg)
{
  gboolean ret;
  GtkWidget *error_widget, *action_area;
  GtkBox *content_area;
  gchar *path;
  gchar *root_objects[] = {
    "main_box",
    NULL
  };

  dlg->priv = GTR_HEADER_DIALOG_GET_PRIVATE (dlg);

  dlg->priv->settings = g_settings_new ("org.gnome.gtranslator.preferences.files");

  gtk_dialog_add_buttons (GTK_DIALOG (dlg),
                          GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE, NULL);

  gtk_window_set_title (GTK_WINDOW (dlg), _("Edit Header"));
  gtk_window_set_resizable (GTK_WINDOW (dlg), FALSE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);

  action_area = gtk_dialog_get_action_area (GTK_DIALOG (dlg));
  content_area = GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dlg)));

  gtk_container_set_border_width (GTK_CONTAINER (dlg), 5);
  gtk_box_set_spacing (content_area, 2);
  gtk_container_set_border_width (GTK_CONTAINER (action_area), 5);
  gtk_box_set_spacing (GTK_BOX (action_area), 4);

  g_signal_connect (dlg, "response", G_CALLBACK (gtk_widget_destroy), NULL);

  path = gtr_dirs_get_ui_file ("gtr-header-dialog.ui");
  ret = gtr_utils_get_ui_objects (path,
                                  root_objects,
                                  &error_widget,
                                  "main_box", &dlg->priv->main_box,
                                  "notebook", &dlg->priv->notebook,
                                  "lang_vbox", &dlg->priv->lang_vbox,
                                  "prj_id_version",
                                  &dlg->priv->prj_id_version, "rmbt",
                                  &dlg->priv->rmbt, "prj_comment",
                                  &dlg->priv->prj_comment,
                                  "take_my_options",
                                  &dlg->priv->take_my_options,
                                  "tr_name", &dlg->priv->translator,
                                  "tr_email", &dlg->priv->tr_email,
                                  "pot_date", &dlg->priv->pot_date,
                                  "po_date", &dlg->priv->po_date,
                                  "language_entry",
                                  &dlg->priv->language,
                                  "lg_email_entry",
                                  &dlg->priv->lg_email,
                                  "charset_entry",
                                  &dlg->priv->charset,
                                  "encoding_entry",
                                  &dlg->priv->encoding, NULL);
  g_free (path);

  if (!ret)
    {
      gtk_widget_show (error_widget);
      gtk_box_pack_start (content_area, error_widget, TRUE, TRUE, 0);

      return;
    }

  gtk_box_pack_start (content_area, dlg->priv->main_box, FALSE, FALSE, 0);

  gtk_container_set_border_width (GTK_CONTAINER (dlg->priv->notebook), 5);

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dlg->priv->take_my_options),
                                g_settings_get_boolean (dlg->priv->settings,
                                                        GTR_SETTINGS_USE_PROFILE_VALUES));

  gtk_text_view_set_editable (GTK_TEXT_VIEW (dlg->priv->prj_comment), TRUE);

  gtk_widget_set_sensitive (dlg->priv->pot_date, FALSE);
  gtk_widget_set_sensitive (dlg->priv->po_date, FALSE);
  gtk_widget_set_sensitive (dlg->priv->charset, FALSE);

  if (gtk_toggle_button_get_active
      (GTK_TOGGLE_BUTTON (dlg->priv->take_my_options)))
    {
      gboolean active;

      active =
        gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
                                      (dlg->priv->take_my_options));

      gtk_widget_set_sensitive (dlg->priv->translator, !active);
      gtk_widget_set_sensitive (dlg->priv->tr_email, !active);
      gtk_widget_set_sensitive (dlg->priv->language, !active);
      gtk_widget_set_sensitive (dlg->priv->lg_email, !active);
      gtk_widget_set_sensitive (dlg->priv->encoding, !active);
    }
}

static void
set_default_values (GtrHeaderDialog * dlg, GtrWindow * window)
{
  GtkTextBuffer *buffer;

  /* Write header's values on Header dialog */
  dlg->priv->header = gtr_window_get_header_from_active_tab (window);
  gtr_header_dialog_fill_from_header (GTR_HEADER_DIALOG (dlg));

  /*Connect signals */
  g_signal_connect (dlg->priv->take_my_options, "toggled",
                    G_CALLBACK (take_my_options_checkbutton_toggled), dlg);

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (dlg->priv->prj_comment));

  g_signal_connect (buffer, "changed", G_CALLBACK (prj_comment_changed), dlg);

  /* Connect signals to edit Project information on Header dialog */
  g_signal_connect (dlg->priv->prj_id_version, "changed",
                    G_CALLBACK (prj_id_version_changed), dlg);

  g_signal_connect (dlg->priv->rmbt, "changed",
                    G_CALLBACK (rmbt_changed), dlg);

  /* Connect signals to edit Translator and Language information on Header dialog */
  g_signal_connect (dlg->priv->translator, "changed",
                    G_CALLBACK (translator_changed), dlg);

  g_signal_connect (dlg->priv->tr_email, "changed",
                    G_CALLBACK (translator_changed), dlg);

  g_signal_connect (dlg->priv->language, "changed",
                    G_CALLBACK (language_changed), dlg);

  g_signal_connect (dlg->priv->lg_email, "changed",
                    G_CALLBACK (language_changed), dlg);
}

void
gtr_show_header_dialog (GtrWindow * window)
{
  static GtrHeaderDialog *dlg = NULL;
  GtrPo *po;
  GtrTab *tab;

  tab = gtr_window_get_active_tab (window);
  g_return_if_fail (tab != NULL);
  po = gtr_tab_get_po (tab);

  g_return_if_fail (GTR_IS_WINDOW (window));

  if (dlg == NULL)
    {
      dlg = g_object_new (GTR_TYPE_HEADER_DIALOG, NULL);

      g_signal_connect (dlg,
                        "destroy", G_CALLBACK (gtk_widget_destroyed), &dlg);

      set_default_values (dlg, window);

      gtk_widget_show (GTK_WIDGET (dlg));
    }

  if (GTK_WINDOW (window) != gtk_window_get_transient_for (GTK_WINDOW (dlg)))
    {
      gtk_window_set_transient_for (GTK_WINDOW (dlg), GTK_WINDOW (window));
    }

  gtk_window_present (GTK_WINDOW (dlg));
}
