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
  GtkWidget *plural;

  GtkWidget *lang;
  GtkWidget *module;
  GtkWidget *branch;
  GtkWidget *domain;
  GtkWidget *state;

  GtrPo     *po;
} GtrHeaderDialogPrivate;

struct _GtrHeaderDialog
{
  AdwPreferencesDialog parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrHeaderDialog, gtr_header_dialog, ADW_TYPE_PREFERENCES_DIALOG)

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
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = gtr_header_dialog_dispose;

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gnome/translator/gtr-header-dialog.ui");

  gtk_widget_class_bind_template_child_private (widget_class, GtrHeaderDialog, take_my_options);
  gtk_widget_class_bind_template_child_private (widget_class, GtrHeaderDialog, translator);
  gtk_widget_class_bind_template_child_private (widget_class, GtrHeaderDialog, tr_email);
  gtk_widget_class_bind_template_child_private (widget_class, GtrHeaderDialog, language);
  gtk_widget_class_bind_template_child_private (widget_class, GtrHeaderDialog, lang_code);
  gtk_widget_class_bind_template_child_private (widget_class, GtrHeaderDialog, lg_email);
  gtk_widget_class_bind_template_child_private (widget_class, GtrHeaderDialog, charset);
  gtk_widget_class_bind_template_child_private (widget_class, GtrHeaderDialog, encoding);
  gtk_widget_class_bind_template_child_private (widget_class, GtrHeaderDialog, plural);

  gtk_widget_class_bind_template_child_private (widget_class, GtrHeaderDialog, prj_id_version);
  gtk_widget_class_bind_template_child_private (widget_class, GtrHeaderDialog, rmbt);
  gtk_widget_class_bind_template_child_private (widget_class, GtrHeaderDialog, pot_date);
  gtk_widget_class_bind_template_child_private (widget_class, GtrHeaderDialog, po_date);

  gtk_widget_class_bind_template_child_private (widget_class, GtrHeaderDialog, prj_comment);

  gtk_widget_class_bind_template_child_private (widget_class, GtrHeaderDialog, lang);
  gtk_widget_class_bind_template_child_private (widget_class, GtrHeaderDialog, module);
  gtk_widget_class_bind_template_child_private (widget_class, GtrHeaderDialog, branch);
  gtk_widget_class_bind_template_child_private (widget_class, GtrHeaderDialog, domain);
  gtk_widget_class_bind_template_child_private (widget_class, GtrHeaderDialog, state);
}

static void
take_my_options_checkbutton_toggled (AdwSwitchRow *button, GParamSpec *spec,
                                     GtrHeaderDialog *dlg)
{
  gboolean active;
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);

  active = adw_switch_row_get_active (button);

  g_settings_set_boolean (priv->settings, GTR_SETTINGS_USE_PROFILE_VALUES,
                          active);

  gtk_widget_set_sensitive (priv->translator, !active);
  gtk_widget_set_sensitive (priv->tr_email, !active);
  gtk_widget_set_sensitive (priv->language, !active);
  gtk_widget_set_sensitive (priv->lang_code, !active);
  gtk_widget_set_sensitive (priv->lg_email, !active);
  gtk_widget_set_sensitive (priv->encoding, !active);
  gtk_widget_set_sensitive (priv->plural, !active);
}

static void
po_state_set_modified (GtrPo * po)
{
  if (gtr_po_get_state (po) != GTR_PO_STATE_MODIFIED)
    gtr_po_set_state (po, GTR_PO_STATE_MODIFIED);
}

static void
prj_comment_changed (GtkTextBuffer *buffer, GtrHeaderDialog * dlg)
{
  GtkTextIter start, end;
  char *text;
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
prj_id_version_changed (GtkEditable *editable, GtrHeaderDialog * dlg)
{
  const char *text;
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);

  text = gtk_editable_get_text (editable);
  if (text)
    {
      gtr_header_set_prj_id_version (gtr_po_get_header (priv->po), text);
      po_state_set_modified (priv->po);
    }
}

static void
rmbt_changed (GtkEditable *editable, GtrHeaderDialog * dlg)
{
  const char *text;
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);

  text = gtk_editable_get_text (editable);
  if (text)
    {
      gtr_header_set_rmbt (gtr_po_get_header (priv->po), text);
      po_state_set_modified (priv->po);
    }
}

static void
translator_changed (GtkWidget *widget, GtrHeaderDialog * dlg)
{
  const char *name, *email;
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);

  name = gtk_editable_get_text (GTK_EDITABLE (priv->translator));
  email = gtk_editable_get_text (GTK_EDITABLE (priv->tr_email));

  if (name && email)
    {
      gtr_header_set_translator (gtr_po_get_header (priv->po), name, email);
      po_state_set_modified (priv->po);
    }
}

static void
language_changed (GtkWidget * widget, GtrHeaderDialog * dlg)
{
  const char *language, *lg_email, *lang_code;
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);

  language = gtk_editable_get_text (GTK_EDITABLE (priv->language));
  lg_email = gtk_editable_get_text (GTK_EDITABLE (priv->lg_email));
  lang_code = gtk_editable_get_text (GTK_EDITABLE (priv->lang_code));
  if (language && lg_email)
    {
      gtr_header_set_language (gtr_po_get_header (priv->po),
                               language, lang_code,
                               lg_email);
      po_state_set_modified (priv->po);
    }
}

static void
opt_changed (GtkWidget * widget, GtrHeaderDialog * dlg)
{
  const char *encoding, *charset, *plural;
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);

  encoding = gtk_editable_get_text (GTK_EDITABLE (priv->encoding));
  charset = gtk_editable_get_text (GTK_EDITABLE (priv->charset));
  plural = gtk_editable_get_text (GTK_EDITABLE (priv->plural));
  if (encoding)
    gtr_header_set_encoding (gtr_po_get_header (priv->po), encoding);
  if (charset)
    gtr_header_set_charset (gtr_po_get_header (priv->po), charset);
  if (plural)
    gtr_header_set_plural_forms (gtr_po_get_header (priv->po), plural);
  if (encoding || charset || plural)
    po_state_set_modified (priv->po);
}

static void
dl_changed (GtkWidget * widget, GtrHeaderDialog * dlg)
{
  const char *lang, *module, *branch, *domain;
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);

  lang = gtk_editable_get_text (GTK_EDITABLE (priv->lang));
  module = gtk_editable_get_text (GTK_EDITABLE (priv->module));
  branch = gtk_editable_get_text (GTK_EDITABLE (priv->branch));
  domain = gtk_editable_get_text (GTK_EDITABLE (priv->domain));

  gtr_header_set_dl_info (gtr_po_get_header (priv->po),
                          lang, module, branch,
                          domain, NULL);

  po_state_set_modified (priv->po);
}

static void
gtr_header_dialog_fill_from_header (GtrHeaderDialog * dlg)
{
  GtrHeader *header;
  gchar *text;
  GtkTextBuffer *buffer;
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);

  header = gtr_po_get_header (priv->po);

  /* Project Information */
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (priv->prj_comment));
  gtk_text_buffer_set_text (buffer, gtr_header_get_comments (header), -1);

  text = gtr_header_get_prj_id_version (header);
  gtk_editable_set_text (GTK_EDITABLE (priv->prj_id_version), text);
  g_free (text);

  text = gtr_header_get_pot_date (header);
  gtk_editable_set_text (GTK_EDITABLE (priv->pot_date), text);
  g_free (text);

  text = gtr_header_get_po_date (header);
  gtk_editable_set_text (GTK_EDITABLE (priv->po_date), text);
  g_free (text);

  text = gtr_header_get_rmbt (header);
  gtk_editable_set_text (GTK_EDITABLE (priv->rmbt), text);
  g_free (text);

  /* Translator and Language Information */
  text = gtr_header_get_translator (header);
  gtk_editable_set_text (GTK_EDITABLE (priv->translator), text);
  g_free (text);

  text = gtr_header_get_tr_email (header);
  gtk_editable_set_text (GTK_EDITABLE (priv->tr_email), text);
  g_free (text);

  text = gtr_header_get_language (header);
  gtk_editable_set_text (GTK_EDITABLE (priv->language), text);
  g_free (text);

  text = gtr_header_get_language_code (header);
  gtk_editable_set_text (GTK_EDITABLE (priv->lang_code), text);
  g_free (text);

  text = gtr_header_get_lg_email (header);
  gtk_editable_set_text (GTK_EDITABLE (priv->lg_email), text);
  g_free (text);

  text = gtr_header_get_charset (header);
  gtk_editable_set_text (GTK_EDITABLE (priv->charset), text);
  g_free (text);

  text = gtr_header_get_encoding (header);
  gtk_editable_set_text (GTK_EDITABLE (priv->encoding), text);
  g_free (text);

  text = gtr_header_get_plural_forms (header);
  gtk_editable_set_text (GTK_EDITABLE (priv->plural), text);
  g_free (text);

  /* Damned Lies Information */
  text = gtr_header_get_dl_lang (header);
  if (text)
    {
      gtk_editable_set_text (GTK_EDITABLE (priv->lang), text);
      g_free (text);
    }

  text = gtr_header_get_dl_module (header);
  if (text)
    {
      gtk_editable_set_text (GTK_EDITABLE (priv->module), text);
      g_free (text);
    }

  text = gtr_header_get_dl_branch (header);
  if (text)
    {
      gtk_editable_set_text (GTK_EDITABLE (priv->branch), text);
      g_free (text);
    }

  text = gtr_header_get_dl_domain (header);
  if (text)
    {
      gtk_editable_set_text (GTK_EDITABLE (priv->domain), text);
      g_free (text);
    }

  text = gtr_header_get_dl_state (header);
  if (text)
    {
      gtk_editable_set_text (GTK_EDITABLE (priv->state), text);
      g_free (text);
    }
}

static void
gtr_header_dialog_init (GtrHeaderDialog * dlg)
{
  GtrHeaderDialogPrivate *priv = gtr_header_dialog_get_instance_private (dlg);

  priv->settings = g_settings_new ("org.gnome.gtranslator.preferences.files");

  adw_dialog_set_title (ADW_DIALOG (dlg), _("Edit Header"));

  gtk_widget_init_template (GTK_WIDGET (dlg));

  adw_switch_row_set_active (
      ADW_SWITCH_ROW (priv->take_my_options),
      g_settings_get_boolean (priv->settings,
                              GTR_SETTINGS_USE_PROFILE_VALUES));

  gtk_text_view_set_editable (GTK_TEXT_VIEW (priv->prj_comment), TRUE);

  gtk_widget_set_sensitive (priv->pot_date, FALSE);
  gtk_widget_set_sensitive (priv->po_date, FALSE);
  gtk_widget_set_sensitive (priv->charset, FALSE);
  gtk_widget_set_sensitive (priv->state, FALSE);

  if (adw_switch_row_get_active (ADW_SWITCH_ROW (priv->take_my_options)))
    {
      gboolean active;

      active
          = adw_switch_row_get_active (ADW_SWITCH_ROW (priv->take_my_options));

      gtk_widget_set_sensitive (priv->translator, !active);
      gtk_widget_set_sensitive (priv->tr_email, !active);
      gtk_widget_set_sensitive (priv->language, !active);
      gtk_widget_set_sensitive (priv->lang_code, !active);
      gtk_widget_set_sensitive (priv->lg_email, !active);
      gtk_widget_set_sensitive (priv->encoding, !active);
      gtk_widget_set_sensitive (priv->plural, !active);
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
  g_signal_connect (priv->take_my_options, "notify::active",
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

  g_signal_connect (priv->encoding, "changed",
                    G_CALLBACK (opt_changed), dlg);
  g_signal_connect (priv->charset, "changed",
                    G_CALLBACK (opt_changed), dlg);
  g_signal_connect (priv->plural, "changed",
                    G_CALLBACK (opt_changed), dlg);

  g_signal_connect (priv->lang, "changed",
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

  dlg = GTK_WIDGET (g_object_new (GTR_TYPE_HEADER_DIALOG, NULL));
  set_default_values (GTR_HEADER_DIALOG (dlg), window);

  adw_dialog_present (ADW_DIALOG (dlg), GTK_WIDGET (window));
}
