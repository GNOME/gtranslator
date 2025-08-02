/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *               2008  Pablo Sanxiao <psanxiao@gmail.com>
 *                     Igalia
 *               2022 Daniel Garcia <danigm@gnome.org>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *   Pablo Sanxiao <psanxiao@gmail.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-actions-app.h"
#include "gtr-application.h"
#include "gtr-dirs.h"
#include "gtr-preferences-dialog.h"
#include "gtr-settings.h"
#include "gtr-profile.h"
#include "gtr-profile-manager.h"
#include "gtr-utils.h"
#include "gtr-profile-dialog.h"
#include "gtr-po.h"
#include "gtr-utils.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <string.h>
#include <gtksourceview/gtksource.h>

typedef struct
{
  GSettings *ui_settings;
  GSettings *editor_settings;
  GSettings *files_settings;

  GtkWidget *notebook;

  /* Files->General */
  AdwSwitchRow *warn_if_contains_fuzzy;
  AdwSwitchRow *remove_obsolete_entries;

  /* Files->Autosave */
  GtkWidget *autosave_checkbutton;
  GtkWidget *autosave_interval_spinbutton;
  GtkWidget *create_backup;

  /* Editor->Text display */
  GtkWidget *highlight_syntax_checkbutton;
  GtkWidget *visible_whitespace_checkbutton;
  GtkWidget *font_button;

  /* Editor->Contents */
  GtkWidget *unmark_fuzzy_when_changed_checkbutton;
  GtkWidget *spellcheck_checkbutton;

  /*Profiles */
  GtkWidget *check_group;
  GtkWidget *profiles;
  GtkWidget *add_profile;

  GtrProfile *editing_profile;
} GtrPreferencesDialogPrivate;

struct _GtrPreferencesDialog
{
  AdwPreferencesDialog parent_instance;
};


G_DEFINE_TYPE_WITH_PRIVATE (GtrPreferencesDialog, gtr_preferences_dialog, ADW_TYPE_PREFERENCES_DIALOG)

static void fill_profile_listbox (GtrPreferencesDialog *dlg);

/***************Files pages****************/

static void
setup_files_general_page (GtrPreferencesDialog * dlg)
{
  GtrPreferencesDialogPrivate *priv = gtr_preferences_dialog_get_instance_private (dlg);
  g_settings_bind (priv->files_settings,
                   GTR_SETTINGS_WARN_IF_CONTAINS_FUZZY,
                   priv->warn_if_contains_fuzzy,
                   "active",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
  g_settings_bind (priv->files_settings,
                   GTR_SETTINGS_REMOVE_OBSOLETE_ENTRIES,
                   priv->remove_obsolete_entries,
                   "active",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
}

static void
setup_files_autosave_page (GtrPreferencesDialog * dlg)
{
  GtrPreferencesDialogPrivate *priv = gtr_preferences_dialog_get_instance_private (dlg);
  g_settings_bind (priv->files_settings,
                   GTR_SETTINGS_AUTO_SAVE_INTERVAL,
                   priv->autosave_interval_spinbutton,
                   "value",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);

  g_settings_bind (priv->files_settings,
                   GTR_SETTINGS_AUTO_SAVE,
                   priv->autosave_checkbutton,
                   "active",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
  g_settings_bind (priv->files_settings,
                   GTR_SETTINGS_AUTO_SAVE,
                   priv->autosave_interval_spinbutton,
                   "sensitive",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);

  g_settings_bind (priv->files_settings,
                   GTR_SETTINGS_CREATE_BACKUP,
                   priv->create_backup,
                   "active",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
}

int
gtr_prefs_get_remove_obsolete (void)
{
  GSettings *files_settings = g_settings_new ("org.gnome.gtranslator.preferences.files");
  gboolean remove_obsolete_entries_bool = g_settings_get_boolean (files_settings, GTR_SETTINGS_REMOVE_OBSOLETE_ENTRIES);
  return remove_obsolete_entries_bool;
}

static void
setup_files_pages (GtrPreferencesDialog * dlg)
{
  /*Children */
  setup_files_general_page (dlg);
  setup_files_autosave_page (dlg);
}


/***************Editor pages****************/

static void
setup_editor_text_display_page (GtrPreferencesDialog * dlg)
{
  GtrPreferencesDialogPrivate *priv = gtr_preferences_dialog_get_instance_private (dlg);
  g_settings_bind (priv->editor_settings,
                   GTR_SETTINGS_HIGHLIGHT_SYNTAX,
                   priv->highlight_syntax_checkbutton,
                   "active",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
  g_settings_bind (priv->editor_settings,
                   GTR_SETTINGS_VISIBLE_WHITESPACE,
                   priv->visible_whitespace_checkbutton,
                   "active",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
}

static void
setup_editor_contents (GtrPreferencesDialog * dlg)
{
  GtrPreferencesDialogPrivate *priv = gtr_preferences_dialog_get_instance_private (dlg);
  g_settings_bind (priv->editor_settings,
                   GTR_SETTINGS_UNMARK_FUZZY_WHEN_CHANGED,
                   priv->unmark_fuzzy_when_changed_checkbutton,
                   "active",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
  g_settings_bind (priv->editor_settings,
                   GTR_SETTINGS_SPELLCHECK,
                   priv->spellcheck_checkbutton,
                   "active",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
}

static void
setup_editor_pages (GtrPreferencesDialog * dlg)
{
  /*Children */
  setup_editor_text_display_page (dlg);
  setup_editor_contents (dlg);
}

static void
on_font_set (GtkWidget *widget, GParamSpec *spec, GtrPreferencesDialog *dlg)
{
  GtrPreferencesDialogPrivate *priv = gtr_preferences_dialog_get_instance_private (dlg);
  PangoFontDescription *pango_font = NULL;
  g_autofree char *font = NULL;

  pango_font = gtk_font_dialog_button_get_font_desc (GTK_FONT_DIALOG_BUTTON (priv->font_button));
  font = pango_font_description_to_string (pango_font);
  g_settings_set_string (priv->editor_settings, GTR_SETTINGS_FONT, font);
}

static char *
get_default_font (void) {
  g_autoptr(GSettings) settings = NULL;
  g_autoptr(GSettingsSchema) schema = NULL;
  GSettingsSchemaSource *source = g_settings_schema_source_get_default ();
  char *font = NULL;

  schema = g_settings_schema_source_lookup (source, "org.gnome.desktop.interface", TRUE);
  if (!schema || !g_settings_schema_has_key (schema, "monospace-font-name"))
    return NULL;

  settings = g_settings_new ("org.gnome.desktop.interface");
  font = g_settings_get_string (settings, "monospace-font-name");
  return font;
}

static void
on_profile_selection_change (GtkCheckButton *button, GtrProfile *profile)
{
  GtrProfileManager *prof_manager = gtr_profile_manager_get_default ();

  if (gtk_check_button_get_active (button))
    {
      gtr_profile_manager_set_active_profile (prof_manager, profile);
    }
  // TODO: check if there's only one profile and it's unchecked

  g_object_unref (prof_manager);
}

static GtkWidget *
create_profile_row (GtrPreferencesDialog *dlg,
                    GtrProfile           *profile,
                    GtrProfile           *active_profile)
{
  const gchar *profile_name;
  GtkWidget *row = NULL;
  GtkWidget *check = NULL;
  GtkWidget *menu = NULL;
  GMenu *gmenu = NULL;
  GMenuItem *gitem = NULL;

  GtrPreferencesDialogPrivate *priv = gtr_preferences_dialog_get_instance_private (dlg);

  // Profile row widget creation
  // CheckButton, label, menu -> [edit, delete]
  profile_name = gtr_profile_get_name (profile);
  row = adw_action_row_new ();
  adw_preferences_row_set_title (ADW_PREFERENCES_ROW (row), profile_name);

  // CheckButton
  check = gtk_check_button_new ();
  gtk_widget_set_valign (check, GTK_ALIGN_CENTER);
  gtk_widget_add_css_class (check, "selection-mode");
  adw_action_row_add_prefix (ADW_ACTION_ROW (row), check);

  // MenuButton
  gmenu = g_menu_new ();
  gitem = g_menu_item_new (_("Edit"), "profile.edit");
  g_menu_item_set_action_and_target (gitem, "profile.edit", "s", profile_name);
  g_menu_append_item (gmenu, gitem);
  g_object_unref (gitem);
  gitem = g_menu_item_new (_("Delete"), "profile.delete");
  g_menu_item_set_action_and_target (gitem, "profile.delete", "s", profile_name);
  g_menu_append_item (gmenu, gitem);
  g_object_unref (gitem);

  menu = gtk_menu_button_new ();
  gtk_widget_set_valign (menu, GTK_ALIGN_CENTER);
  gtk_menu_button_set_menu_model (GTK_MENU_BUTTON (menu), G_MENU_MODEL (gmenu));
  gtk_menu_button_set_icon_name (GTK_MENU_BUTTON (menu), "view-more-symbolic");
  adw_action_row_add_suffix (ADW_ACTION_ROW (row), menu);
  gtk_widget_add_css_class (menu, "flat");

  adw_action_row_set_activatable_widget (ADW_ACTION_ROW (row), check);

  if (profile == active_profile)
    {
      gtk_check_button_set_active (GTK_CHECK_BUTTON (check), true);
    }

  if (!priv->check_group)
    priv->check_group = check;
  else
    {
      gtk_check_button_set_group (GTK_CHECK_BUTTON (check),
                                  GTK_CHECK_BUTTON (priv->check_group));
    }

  g_signal_connect (check, "toggled",
                    G_CALLBACK (on_profile_selection_change), profile);


  return row;
}

/***************Profile pages****************/
static void
on_profile_dialog_response_cb (GtrProfileDialog     *profile_dialog,
                               GtrPreferencesDialog *dlg)
{
  GtrProfileManager *prof_manager;
  GtrProfile *profile;
  GtrProfile *active_profile;
  GSList *profiles = NULL;
  GtrPreferencesDialogPrivate *priv = gtr_preferences_dialog_get_instance_private (dlg);

  prof_manager = gtr_profile_manager_get_default ();
  profile = gtr_profile_dialog_get_profile (profile_dialog);
  profiles = gtr_profile_manager_get_profiles (prof_manager);
  active_profile = gtr_profile_manager_get_active_profile (prof_manager);

  /* add new profile */
  if (!gtr_profile_dialog_get_editing (profile_dialog))
    {
      unsigned int n = g_slist_length (profiles);
      GtkWidget *row = create_profile_row (dlg, profile, active_profile);
      gtk_list_box_insert (GTK_LIST_BOX (priv->profiles), row, n);
      gtr_profile_manager_add_profile (prof_manager, profile);
    }
  /* modify profile */
  else
    {
      gtr_profile_manager_modify_profile (prof_manager, priv->editing_profile, profile);
      priv->editing_profile = NULL;
      fill_profile_listbox (dlg);
    }

  g_object_unref (prof_manager);
  adw_dialog_close (ADW_DIALOG (profile_dialog));
}

static void
add_button_clicked (GtrPreferencesDialog *dlg)
{
  GtrProfileDialog *profile_dialog;

  profile_dialog = gtr_profile_dialog_new (NULL);
  g_signal_connect (profile_dialog, "response",
                    G_CALLBACK (on_profile_dialog_response_cb), dlg);
  adw_dialog_present (ADW_DIALOG (profile_dialog), GTK_WIDGET (dlg));
}

static void
on_profile_row_cb (GtkListBox *profiles, GtkListBoxRow *row, GtrPreferencesDialog *dlg)
{
  GtrPreferencesDialogPrivate *priv = gtr_preferences_dialog_get_instance_private (dlg);

  if (GTK_WIDGET (row) == priv->add_profile)
    add_button_clicked (dlg);
}

static void
edit_button_clicked (GtkWidget            *widget,
                     const char           *action_name,
                     GVariant             *parameter)
{
  char *profile_name = NULL;
  GtrProfileManager *prof_manager;
  GtrProfile *profile = NULL;
  GtrPreferencesDialog *dlg = GTR_PREFERENCES_DIALOG (widget);
  GtrPreferencesDialogPrivate *priv = gtr_preferences_dialog_get_instance_private (dlg);


  prof_manager = gtr_profile_manager_get_default ();
  profile_name = (char *) g_variant_get_string (parameter, NULL);
  profile = gtr_profile_manager_get_profile (prof_manager, profile_name);

  if (profile)
    {
      priv->editing_profile = profile;
      GtrProfileDialog *profile_dialog;
      profile_dialog = gtr_profile_dialog_new (profile);
      g_signal_connect (profile_dialog, "response",
                        G_CALLBACK (on_profile_dialog_response_cb), dlg);
      adw_dialog_present (ADW_DIALOG (profile_dialog), GTK_WIDGET (dlg));
    }

  g_object_unref (prof_manager);
}

static void
delete_confirm_dialog_cb (GtkWidget *dialog, char *response, GtrPreferencesDialog *dlg)
{
  GtrPreferencesDialogPrivate *priv = gtr_preferences_dialog_get_instance_private (dlg);
  GtrProfileManager *prof_manager;
  GtrProfile *profile = priv->editing_profile;

  priv->editing_profile = NULL;
  adw_dialog_close (ADW_DIALOG (dialog));

  if (g_strcmp0 (response, "cancel") == 0)
    return;

  prof_manager = gtr_profile_manager_get_default ();
  gtr_profile_manager_remove_profile (prof_manager, profile);
  g_object_unref (prof_manager);

  fill_profile_listbox (dlg);
}

static void
delete_button_clicked (GtkWidget            *widget,
                       const char           *action_name,
                       GVariant             *parameter)
{
  char *profile_name = NULL;
  GtrProfileManager *prof_manager;
  GtrProfile *profile = NULL;
  GtrProfile *active_profile = NULL;
  GtrPreferencesDialog *dlg = GTR_PREFERENCES_DIALOG (widget);
  GtrPreferencesDialogPrivate *priv = gtr_preferences_dialog_get_instance_private (dlg);

  prof_manager = gtr_profile_manager_get_default ();
  profile_name = (char *) g_variant_get_string (parameter, NULL);
  profile = gtr_profile_manager_get_profile (prof_manager, profile_name);
  active_profile = gtr_profile_manager_get_active_profile (prof_manager);

  if (profile)
    {
      if (active_profile == profile)
        {
          AdwDialog *dialog = adw_alert_dialog_new (
            _("Impossible to remove the active profile"),
            _("Another profile should be selected as active before")
          );

          adw_alert_dialog_add_response (ADW_ALERT_DIALOG (dialog), "ok", _("_Ok"));
          adw_alert_dialog_set_default_response (ADW_ALERT_DIALOG (dialog), "ok");

          adw_dialog_present (dialog, GTK_WIDGET (dlg));
        }
      else
        {
          AdwDialog *dialog = adw_alert_dialog_new (
            _("Are you sure you want to delete this profile?"),
            NULL
          );

          adw_alert_dialog_add_responses (ADW_ALERT_DIALOG (dialog),
                                          "cancel",  _("_Cancel"),
                                          "delete", _("_Delete"),
                                          NULL);

          priv->editing_profile = profile;
          g_signal_connect (dialog, "response",
                            G_CALLBACK (delete_confirm_dialog_cb), dlg);
          adw_dialog_present (dialog, GTK_WIDGET (dlg));
        }
    }

  g_object_unref (prof_manager);
}

static void
clear_profile_listbox (GtrPreferencesDialog *dlg)
{
  GtrPreferencesDialogPrivate *priv = gtr_preferences_dialog_get_instance_private (dlg);
  GtkWidget *widget = GTK_WIDGET (gtk_list_box_get_row_at_index (GTK_LIST_BOX (priv->profiles), 0));

  while (widget != priv->add_profile)
    {
      gtk_list_box_remove (GTK_LIST_BOX (priv->profiles), widget);
      widget = GTK_WIDGET (gtk_list_box_get_row_at_index (GTK_LIST_BOX (priv->profiles), 0));
    }
}

static void
fill_profile_listbox (GtrPreferencesDialog *dlg)
{
  GtrProfileManager *prof_manager;
  GtrProfile *active_profile;
  GSList *l, *profiles;
  unsigned int n = 0;
  GtrPreferencesDialogPrivate *priv = gtr_preferences_dialog_get_instance_private (dlg);

  prof_manager = gtr_profile_manager_get_default ();
  profiles = gtr_profile_manager_get_profiles (prof_manager);
  active_profile = gtr_profile_manager_get_active_profile (prof_manager);

  clear_profile_listbox (dlg);

  priv->check_group = NULL;
  for (l = profiles; l != NULL; l = g_slist_next (l), n++)
    {
      GtrProfile *profile = GTR_PROFILE (l->data);
      GtkWidget *row = create_profile_row (dlg, profile, active_profile);
      gtk_list_box_insert (GTK_LIST_BOX (priv->profiles), row, n);
    }
  g_object_unref (prof_manager);
}

static void
setup_profile_pages (GtrPreferencesDialog *dlg)
{
  GtrPreferencesDialogPrivate *priv = gtr_preferences_dialog_get_instance_private (dlg);
  fill_profile_listbox (dlg);

  g_signal_connect (priv->profiles, "row-activated", G_CALLBACK (on_profile_row_cb), dlg);
}

static void
gtr_preferences_dialog_init (GtrPreferencesDialog * dlg)
{
  GtrPreferencesDialogPrivate *priv = gtr_preferences_dialog_get_instance_private (dlg);
  g_autofree char *font = NULL;
  g_autoptr(PangoFontDescription) pango_font = NULL;

  gtk_widget_init_template (GTK_WIDGET (dlg));

  priv->editing_profile = NULL;
  priv->ui_settings = g_settings_new ("org.gnome.gtranslator.preferences.ui");
  priv->editor_settings = g_settings_new ("org.gnome.gtranslator.preferences.editor");
  priv->files_settings = g_settings_new ("org.gnome.gtranslator.preferences.files");
  priv->check_group = NULL;

  setup_profile_pages (dlg);
  setup_files_pages (dlg);
  setup_editor_pages (dlg);

  font = g_settings_get_string (priv->editor_settings, GTR_SETTINGS_FONT);
  if (!strlen (font))
    font = get_default_font ();
  pango_font = pango_font_description_from_string (font);
  gtk_font_dialog_button_set_font_desc (GTK_FONT_DIALOG_BUTTON (priv->font_button), pango_font);
  g_signal_connect (priv->font_button, "notify::font-desc", G_CALLBACK (on_font_set), dlg);
}

static void
gtr_preferences_dialog_dispose (GObject * object)
{
  GtrPreferencesDialog *dlg = GTR_PREFERENCES_DIALOG (object);
  GtrPreferencesDialogPrivate *priv = gtr_preferences_dialog_get_instance_private (dlg);

  g_clear_object (&priv->ui_settings);
  g_clear_object (&priv->editor_settings);
  g_clear_object (&priv->files_settings);

  G_OBJECT_CLASS (gtr_preferences_dialog_parent_class)->dispose (object);
}

static void
gtr_preferences_dialog_class_init (GtrPreferencesDialogClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = gtr_preferences_dialog_dispose;

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gnome/translator/gtr-preferences-dialog.ui");

  gtk_widget_class_bind_template_child_private (widget_class, GtrPreferencesDialog, warn_if_contains_fuzzy);
  gtk_widget_class_bind_template_child_private (widget_class, GtrPreferencesDialog, remove_obsolete_entries);
  gtk_widget_class_bind_template_child_private (widget_class, GtrPreferencesDialog, create_backup);
  gtk_widget_class_bind_template_child_private (widget_class, GtrPreferencesDialog, autosave_checkbutton);
  gtk_widget_class_bind_template_child_private (widget_class, GtrPreferencesDialog, autosave_interval_spinbutton);
  gtk_widget_class_bind_template_child_private (widget_class, GtrPreferencesDialog, highlight_syntax_checkbutton);
  gtk_widget_class_bind_template_child_private (widget_class, GtrPreferencesDialog, visible_whitespace_checkbutton);
  gtk_widget_class_bind_template_child_private (widget_class, GtrPreferencesDialog, font_button);
  gtk_widget_class_bind_template_child_private (widget_class, GtrPreferencesDialog, unmark_fuzzy_when_changed_checkbutton);
  gtk_widget_class_bind_template_child_private (widget_class, GtrPreferencesDialog, spellcheck_checkbutton);

  gtk_widget_class_bind_template_child_private (widget_class, GtrPreferencesDialog, profiles);
  gtk_widget_class_bind_template_child_private (widget_class, GtrPreferencesDialog, add_profile);

  gtk_widget_class_install_action (widget_class, "profile.edit", "s", edit_button_clicked);
  gtk_widget_class_install_action (widget_class, "profile.delete", "s", delete_button_clicked);
}

void
gtr_show_preferences_dialog (GtrWindow * window)
{
  GtkWidget *dlg = NULL;
  dlg = GTK_WIDGET (g_object_new (GTR_TYPE_PREFERENCES_DIALOG,
                                   NULL));

  adw_dialog_present (ADW_DIALOG (dlg), GTK_WIDGET (window));
}
