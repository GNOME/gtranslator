/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *               2008  Pablo Sanxiao <psanxiao@gmail.com>
 *                     Igalia
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

#include "gtr-application.h"
#include "gtr-dirs.h"
#include "gtr-preferences-dialog.h"
#include "gtr-settings.h"
#include "gtr-profile.h"
#include "gtr-profile-manager.h"
#include "gtr-utils.h"
#include "gtr-plugin-manager.h"
#include "gtr-profile-dialog.h"
#include "gtr-po.h"
#include "gtr-utils.h"
#include "../translation-memory/gtr-translation-memory.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <string.h>
#include <gtksourceview/gtksourcestyleschememanager.h>
#include <gdl/gdl.h>

#define GTR_PREFERENCES_DIALOG_GET_PRIVATE(object) (G_TYPE_INSTANCE_GET_PRIVATE ( \
                                                    (object), \
                                                    GTR_TYPE_PREFERENCES_DIALOG, \
                                                    GtrPreferencesDialogPrivate))


G_DEFINE_TYPE (GtrPreferencesDialog, gtr_preferences_dialog, GTK_TYPE_DIALOG)

struct _GtrPreferencesDialogPrivate
{
  GSettings *ui_settings;
  GSettings *tm_settings;
  GSettings *editor_settings;
  GSettings *files_settings;

  GtkWidget *notebook;

  /* Files->General */
  GtkWidget *warn_if_contains_fuzzy_checkbutton;
  GtkWidget *delete_compiled_checkbutton;

  /* Files->Autosave */
  GtkWidget *autosave_checkbutton;
  GtkWidget *autosave_interval_spinbutton;
  GtkWidget *autosave_hbox;
  GtkWidget *create_backup_checkbutton;

  /* Editor->Text display */
  GtkWidget *highlight_syntax_checkbutton;
  GtkWidget *visible_whitespace_checkbutton;
  GtkWidget *use_custom_font_checkbutton;
  GtkWidget *editor_font_fontbutton;
  GtkWidget *editor_font_hbox;

  /* Editor->Contents */
  GtkWidget *unmark_fuzzy_when_changed_checkbutton;
  GtkWidget *spellcheck_checkbutton;

  /*Profiles */
  GtkWidget *profile_treeview;
  GtkWidget *add_button;
  GtkWidget *edit_button;
  GtkWidget *delete_button;

  /*Translation Memory */
  GtkWidget *directory_entry;
  GtkWidget *search_button;
  GtkWidget *add_database_button;
  GtkWidget *add_database_progressbar;

  GtkWidget *use_lang_profile_in_tm;
  GtkWidget *tm_lang_entry;
  GtkWidget *missing_words_spinbutton;
  GtkWidget *sentence_length_spinbutton;

  /*PO header->Personal information */
  GtkWidget *name_entry;
  GtkWidget *email_entry;

  /*PO header->Language settings */
  GtkWidget *language_comboentry;
  GtkWidget *langcode_comboentry;
  GtkWidget *charset_comboentry;
  GtkWidget *encoding_comboentry;
  GtkWidget *team_email_comboentry;
  GtkWidget *plurals_entry;
  GtkWidget *number_plurals_spinbutton;

  /*Inteface */
  GtkWidget *gdl_combobox;
  GtkWidget *scheme_color_combobox;

  /*Plugins */
  GtkWidget *plugins_box;
};

enum
{
  PROFILE_NAME_COLUMN,
  ACTIVE_PROFILE_COLUMN,
  PROFILE_COLUMN,
  PROFILE_N_COLUMNS
};

/***************Files pages****************/

static void
setup_files_general_page (GtrPreferencesDialog * dlg)
{
  g_settings_bind (dlg->priv->files_settings,
                   GTR_SETTINGS_WARN_IF_CONTAINS_FUZZY,
                   dlg->priv->warn_if_contains_fuzzy_checkbutton,
                   "active",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
  g_settings_bind (dlg->priv->files_settings,
                   GTR_SETTINGS_DELETE_COMPILED,
                   dlg->priv->delete_compiled_checkbutton,
                   "active",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
}

static void
on_auto_save_changed (GSettings            *settings,
                      const gchar          *key,
                      GtrPreferencesDialog *dlg)
{
  gtk_widget_set_sensitive (dlg->priv->autosave_interval_spinbutton,
                            g_settings_get_boolean (settings, key));
}

static void
setup_files_autosave_page (GtrPreferencesDialog * dlg)
{
  g_settings_bind (dlg->priv->files_settings,
                   GTR_SETTINGS_AUTO_SAVE_INTERVAL,
                   dlg->priv->autosave_interval_spinbutton,
                   "value",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);

  g_settings_bind (dlg->priv->files_settings,
                   GTR_SETTINGS_AUTO_SAVE,
                   dlg->priv->autosave_checkbutton,
                   "active",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
  g_signal_connect (dlg->priv->files_settings,
                    "changed::" GTR_SETTINGS_AUTO_SAVE,
                    G_CALLBACK (on_auto_save_changed),
                    dlg);
  /*Set sensitive */
  on_auto_save_changed (dlg->priv->files_settings,
                        GTR_SETTINGS_AUTO_SAVE,
                        dlg);

  g_settings_bind (dlg->priv->files_settings,
                   GTR_SETTINGS_CREATE_BACKUP,
                   dlg->priv->create_backup_checkbutton,
                   "active",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
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
use_custom_font_changed (GSettings            *settings,
                         const gchar          *key,
                         GtrPreferencesDialog *dlg)
{
  gtk_widget_set_sensitive (dlg->priv->editor_font_hbox,
                            g_settings_get_boolean (settings, key));
}

static void
setup_editor_text_display_page (GtrPreferencesDialog * dlg)
{
  g_settings_bind (dlg->priv->editor_settings,
                   GTR_SETTINGS_HIGHLIGHT_SYNTAX,
                   dlg->priv->highlight_syntax_checkbutton,
                   "active",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
  g_settings_bind (dlg->priv->editor_settings,
                   GTR_SETTINGS_VISIBLE_WHITESPACE,
                   dlg->priv->visible_whitespace_checkbutton,
                   "active",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);

  g_settings_bind (dlg->priv->editor_settings,
                   GTR_SETTINGS_USE_CUSTOM_FONT,
                   dlg->priv->use_custom_font_checkbutton,
                   "active",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
  g_signal_connect (dlg->priv->editor_settings,
                    "changed::" GTR_SETTINGS_USE_CUSTOM_FONT,
                    G_CALLBACK (use_custom_font_changed), dlg);
  /*Set sensitive */
  use_custom_font_changed (dlg->priv->editor_settings,
                           GTR_SETTINGS_USE_CUSTOM_FONT,
                           dlg);

  g_settings_bind (dlg->priv->editor_settings,
                   GTR_SETTINGS_EDITOR_FONT,
                   dlg->priv->editor_font_fontbutton,
                   "font-name",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
}

static void
setup_editor_contents (GtrPreferencesDialog * dlg)
{
  g_settings_bind (dlg->priv->editor_settings,
                   GTR_SETTINGS_UNMARK_FUZZY_WHEN_CHANGED,
                   dlg->priv->unmark_fuzzy_when_changed_checkbutton,
                   "active",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
  g_settings_bind (dlg->priv->editor_settings,
                   GTR_SETTINGS_SPELLCHECK,
                   dlg->priv->spellcheck_checkbutton,
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

/***************Profile pages****************/
static void
on_profile_dialog_response_cb (GtrProfileDialog     *profile_dialog,
                               gint                  response_id,
                               GtrPreferencesDialog *dlg)
{
  GtrProfileManager *prof_manager;
  GtkTreeModel *model;
  GtrProfile *profile;
  GtrProfile *active_profile;
  GtkTreeIter iter;

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (dlg->priv->profile_treeview));
  g_return_if_fail (model != NULL);

  prof_manager = gtr_profile_manager_get_default ();
  profile = gtr_profile_dialog_get_profile (profile_dialog);

  /* add new profile */
  if (response_id == GTK_RESPONSE_ACCEPT)
    {
      gtk_list_store_append (GTK_LIST_STORE (model), &iter);
      gtr_profile_manager_add_profile (prof_manager, profile);

      active_profile = gtr_profile_manager_get_active_profile (prof_manager);

      gtk_list_store_set (GTK_LIST_STORE (model),
                          &iter,
                          PROFILE_NAME_COLUMN, gtr_profile_get_name (profile),
                          ACTIVE_PROFILE_COLUMN, (profile == active_profile),
                          PROFILE_COLUMN, profile,
                          -1);
    }
  /* modify profile */
  else if (response_id == GTK_RESPONSE_YES)
    {
      GtkTreeSelection *selection;

      selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (dlg->priv->profile_treeview));

      if (gtk_tree_selection_get_selected (selection, &model, &iter))
        {
          GtrProfile *old;

          gtk_tree_model_get (model, &iter,
                              PROFILE_COLUMN, &old,
                              -1);

          gtr_profile_manager_modify_profile (prof_manager, old, profile);
          active_profile = gtr_profile_manager_get_active_profile (prof_manager);

          gtk_list_store_set (GTK_LIST_STORE (model),
                              &iter,
                              PROFILE_NAME_COLUMN, gtr_profile_get_name (profile),
                              ACTIVE_PROFILE_COLUMN, (profile == active_profile),
                              PROFILE_COLUMN, profile,
                              -1);
        }
    }

  g_object_unref (prof_manager);
  gtk_widget_destroy (GTK_WIDGET (profile_dialog));
}

static void
add_button_clicked (GtkWidget *button, GtrPreferencesDialog *dlg)
{
  GtrProfileDialog *profile_dialog;

  profile_dialog = gtr_profile_dialog_new (GTK_WIDGET (dlg), NULL);

  g_signal_connect (profile_dialog, "response",
                    G_CALLBACK (on_profile_dialog_response_cb), dlg);

  gtk_widget_show (GTK_WIDGET (profile_dialog));
  gtk_window_present (GTK_WINDOW (profile_dialog));
}

static void
edit_button_clicked (GtkWidget *button, GtrPreferencesDialog *dlg)
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  GtkTreeSelection *selection;
  GtrProfile *profile;

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (dlg->priv->profile_treeview));
  g_return_if_fail (model != NULL);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (dlg->priv->profile_treeview));

  if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
      GtrProfileDialog *profile_dialog;

      gtk_tree_model_get (model, &iter, PROFILE_COLUMN, &profile, -1);

      profile_dialog = gtr_profile_dialog_new (GTK_WIDGET (dlg), profile);

      g_signal_connect (profile_dialog, "response",
                        G_CALLBACK (on_profile_dialog_response_cb), dlg);

      gtk_widget_show (GTK_WIDGET (profile_dialog));
      gtk_window_present (GTK_WINDOW (profile_dialog));
    }
}

static void
delete_confirm_dialog_cb (GtkWidget *dialog,
                          gint response_id, GtrPreferencesDialog *dlg)
{
  if (response_id == GTK_RESPONSE_YES)
    {
      GtkTreeIter iter;
      GtkTreeModel *model;
      GtkTreeSelection *selection;

      model = gtk_tree_view_get_model (GTK_TREE_VIEW (dlg->priv->profile_treeview));
      g_return_if_fail (model != NULL);

      selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (dlg->priv->profile_treeview));

      if (gtk_tree_selection_get_selected (selection, &model, &iter))
        {
          GtrProfileManager *prof_manager;
          GtrProfile *profile;

          gtk_tree_model_get (model, &iter, PROFILE_COLUMN, &profile,
                              -1);

          if (profile != NULL)
            {
              prof_manager = gtr_profile_manager_get_default ();
              gtr_profile_manager_remove_profile (prof_manager, profile);
              g_object_unref (prof_manager);

              gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
            }
        }
    }

  gtk_widget_destroy (dialog);
}

static void
delete_button_clicked (GtkWidget *button, GtrPreferencesDialog *dlg)
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  GtkTreeSelection *selection;
  gboolean active;
  GtkWidget *dialog;

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (dlg->priv->profile_treeview));
  g_return_if_fail (model != NULL);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (dlg->priv->profile_treeview));

  if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
      gtk_tree_model_get (model, &iter, ACTIVE_PROFILE_COLUMN, &active, -1);

      if (active)
        {
          dialog = gtk_message_dialog_new (GTK_WINDOW (dlg),
                                           GTK_DIALOG_MODAL,
                                           GTK_MESSAGE_ERROR,
                                           GTK_BUTTONS_CLOSE, NULL);

          gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (dialog),
                                         _("<span weight=\"bold\" size=\"large\">Impossible to remove the active profile</span>"));

          gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG
                                                    (dialog),
                                                    _("Another profile should be selected as active before"));

          gtk_dialog_run (GTK_DIALOG (dialog));
          gtk_widget_destroy (dialog);
        }
      else
        {
          dialog = gtk_message_dialog_new (GTK_WINDOW (dlg),
                                           GTK_DIALOG_MODAL,
                                           GTK_MESSAGE_QUESTION,
                                           GTK_BUTTONS_NONE, NULL);

          gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (dialog),
                                         _("<span weight=\"bold\" size=\"large\">Are you sure you want to delete this profile?</span>"));

          gtk_dialog_add_button (GTK_DIALOG (dialog),
                                 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);

          gtk_dialog_add_button (GTK_DIALOG (dialog),
                                 GTK_STOCK_DELETE, GTK_RESPONSE_YES);

          gtk_dialog_run (GTK_DIALOG (dialog));

          g_signal_connect (GTK_DIALOG (dialog), "response",
                            G_CALLBACK (delete_confirm_dialog_cb), dlg);
        }
    }
}

static void
active_toggled_cb (GtkCellRendererToggle *cell_renderer,
                   gchar *path_str, GtrPreferencesDialog *dlg)
{
  GtkTreeIter iter, first;
  GtkTreePath *path;
  GtkTreeModel *model;
  GtrProfile *active_profile;

  path = gtk_tree_path_new_from_string (path_str);

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (dlg->priv->profile_treeview));
  g_return_if_fail (model != NULL);

  gtk_tree_model_get_iter (model, &iter, path);

  gtk_tree_model_get (model, &iter, PROFILE_COLUMN, &active_profile, -1);

  if (active_profile != NULL)
    {
      GtrProfileManager *prof_manager;

      prof_manager = gtr_profile_manager_get_default ();

      if (gtr_profile_manager_get_active_profile (prof_manager) != active_profile)
        {
          gtr_profile_manager_set_active_profile (prof_manager, active_profile);

          gtk_tree_model_get_iter_first (model, &first);

          do
          {
            gtk_list_store_set (GTK_LIST_STORE (model),
                                &first,
                                ACTIVE_PROFILE_COLUMN, FALSE,
                                -1);
          } while (gtk_tree_model_iter_next (model, &first));

          gtk_list_store_set (GTK_LIST_STORE (model),
                              &iter,
                              ACTIVE_PROFILE_COLUMN, TRUE,
                              -1);
        }

      g_object_unref (prof_manager);
    }

  gtk_tree_path_free (path);
}

static void
fill_profile_treeview (GtrPreferencesDialog *dlg, GtkTreeModel *model)
{
  GtrProfileManager *prof_manager;
  GtkTreeIter iter;
  GtrProfile *active_profile;
  GSList *l, *profiles;

  gtk_list_store_clear (GTK_LIST_STORE (model));

  prof_manager = gtr_profile_manager_get_default ();
  profiles = gtr_profile_manager_get_profiles (prof_manager);
  active_profile = gtr_profile_manager_get_active_profile (prof_manager);

  for (l = profiles; l != NULL; l = g_slist_next (l))
    {
      GtrProfile *profile = GTR_PROFILE (l->data);
      const gchar *profile_name;

      profile_name = gtr_profile_get_name (profile);
      gtk_list_store_append (GTK_LIST_STORE (model), &iter);

      gtk_list_store_set (GTK_LIST_STORE (model),
                          &iter,
                          PROFILE_NAME_COLUMN, profile_name,
                          ACTIVE_PROFILE_COLUMN, (profile == active_profile),
                          PROFILE_COLUMN, profile,
                          -1);
    }

  g_object_unref (prof_manager);
}

static void
setup_profile_pages (GtrPreferencesDialog *dlg)
{

  GtkTreeViewColumn *name_column, *toggle_column;
  GtkCellRenderer *text_renderer, *toggle_renderer;
  GtkListStore *model;

  model = gtk_list_store_new (PROFILE_N_COLUMNS,
                              G_TYPE_STRING,
                              G_TYPE_BOOLEAN,
                              G_TYPE_POINTER);

  gtk_tree_view_set_model (GTK_TREE_VIEW (dlg->priv->profile_treeview),
                           GTK_TREE_MODEL (model));

  g_object_unref (model);

  text_renderer = gtk_cell_renderer_text_new ();
  toggle_renderer = gtk_cell_renderer_toggle_new ();

  g_signal_connect (toggle_renderer,
                    "toggled", G_CALLBACK (active_toggled_cb), dlg);

  gtk_cell_renderer_toggle_set_activatable (GTK_CELL_RENDERER_TOGGLE (toggle_renderer),
                                            TRUE);
  gtk_cell_renderer_toggle_set_radio (GTK_CELL_RENDERER_TOGGLE (toggle_renderer),
                                      TRUE);

  name_column = gtk_tree_view_column_new_with_attributes (_("Profile"),
                                                          text_renderer,
                                                          "text",
                                                          PROFILE_NAME_COLUMN,
                                                          NULL);

  toggle_column = gtk_tree_view_column_new_with_attributes (_("Active"),
                                                            toggle_renderer,
                                                            "active",
                                                            ACTIVE_PROFILE_COLUMN,
                                                            NULL);

  gtk_tree_view_column_set_resizable (toggle_column, TRUE);
  gtk_tree_view_column_set_resizable (name_column, TRUE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (dlg->priv->profile_treeview),
                               name_column);
  gtk_tree_view_append_column (GTK_TREE_VIEW (dlg->priv->profile_treeview),
                               toggle_column);

  gtk_tree_view_column_set_expand (name_column, TRUE);

  fill_profile_treeview (dlg, GTK_TREE_MODEL (model));

  /* Connect the signals */
  g_signal_connect (dlg->priv->add_button,
                    "clicked", G_CALLBACK (add_button_clicked), dlg);

  g_signal_connect (dlg->priv->delete_button,
                    "clicked", G_CALLBACK (delete_button_clicked), dlg);

  g_signal_connect (dlg->priv->edit_button,
                    "clicked", G_CALLBACK (edit_button_clicked), dlg);
}

/***************Interface pages****************/
static void
scheme_color_changed_cb (GtkComboBoxText * combobox, GtrPreferencesDialog * dlg)
{
  gchar *active_text;

  active_text = gtk_combo_box_text_get_active_text (combobox);

  g_settings_set_string (dlg->priv->ui_settings,
                         GTR_SETTINGS_COLOR_SCHEME,
                         active_text);
  g_free (active_text);
}

static void
on_gdl_combobox_changed (GtkComboBox          *combobox,
                         GtrPreferencesDialog *dlg)
{
  g_settings_set_enum (dlg->priv->ui_settings,
                       GTR_SETTINGS_PANEL_SWITCHER_STYLE,
                       gtk_combo_box_get_active (combobox));
}

static void
setup_interface_pages (GtrPreferencesDialog * dlg)
{
  GtkSourceStyleSchemeManager *manager;
  const gchar *const *scheme_ids;
  gchar *scheme_active;
  gint i = 0;
  GtkListStore *store;
  GtkCellRenderer *cell;
  GdlSwitcherStyle style;

  style = g_settings_get_enum (dlg->priv->ui_settings,
                               GTR_SETTINGS_PANEL_SWITCHER_STYLE);

  gtk_combo_box_set_active (GTK_COMBO_BOX (dlg->priv->gdl_combobox),
                            style);
  g_signal_connect (dlg->priv->gdl_combobox,
                    "changed",
                    G_CALLBACK (on_gdl_combobox_changed),
                    dlg);

  /* Scheme color */
  store = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_combo_box_set_model (GTK_COMBO_BOX (dlg->priv->scheme_color_combobox),
                           GTK_TREE_MODEL (store));
  g_object_unref (store);

  cell = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT
                              (dlg->priv->scheme_color_combobox), cell, TRUE);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT
                                  (dlg->priv->scheme_color_combobox), cell,
                                  "text", 0, NULL);

  manager = gtk_source_style_scheme_manager_get_default ();
  scheme_ids = gtk_source_style_scheme_manager_get_scheme_ids (manager);
  scheme_active = g_settings_get_string (dlg->priv->ui_settings,
                                         GTR_SETTINGS_COLOR_SCHEME);
  while (scheme_ids[i] != NULL)
    {
      gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (dlg->priv->scheme_color_combobox),
                                      scheme_ids[i]);
      if (g_strcmp0 (scheme_ids[i], scheme_active) == 0)
        gtk_combo_box_set_active (GTK_COMBO_BOX
                                  (dlg->priv->scheme_color_combobox), i);
      i++;
    }

  g_free (scheme_active);

  /*Connect signals */
  g_signal_connect (dlg->priv->scheme_color_combobox, "changed",
                    G_CALLBACK (scheme_color_changed_cb), dlg);
}

/***************Translation Memory pages****************/
static void
response_filechooser_cb (GtkDialog * dialog,
                         gint response_id, GtrPreferencesDialog *dlg)
{
  if (response_id == GTK_RESPONSE_YES)
    {
      gchar *filename;

      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      gtk_entry_set_text (GTK_ENTRY (dlg->priv->directory_entry),
                          filename);
      g_settings_set_string (dlg->priv->tm_settings,
                             GTR_SETTINGS_PO_DIRECTORY,
                             filename);
      g_free (filename);
    }

  gtk_widget_destroy (GTK_WIDGET (dialog));
}

static void
on_search_button_clicked (GtkButton * button, GtrPreferencesDialog *dlg)
{
  GtkWidget *filechooser;

  filechooser = gtk_file_chooser_dialog_new ("Select PO directory",
                                             GTK_WINDOW (dlg),
                                             GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                             GTK_STOCK_CANCEL,
                                             GTK_RESPONSE_CANCEL,
                                             GTK_STOCK_OK,
                                             GTK_RESPONSE_YES, NULL);

  g_signal_connect (GTK_DIALOG (filechooser), "response",
                    G_CALLBACK (response_filechooser_cb), dlg);

  gtk_dialog_run (GTK_DIALOG (filechooser));
}

typedef struct _IdleData
{
  GSList *list;
  GtkProgressBar *progress;
  GtrTranslationMemory *tm;
  GtkWindow *parent;
} IdleData;

static gboolean
add_to_database (gpointer data_pointer)
{
  IdleData *data = (IdleData *) data_pointer;
  static GSList *l = NULL;
  gdouble percentage;

  if (l == NULL)
    l = data->list;
  else
    l = g_slist_next (l);

  if (l)
    {
      GList *msg_list = NULL;
      GFile *location;
      GError *error = NULL;
      GtrPo *po;

      po = gtr_po_new ();
      location = (GFile *) l->data;

      gtr_po_parse (po, location, &error);
      if (error)
        return TRUE;

      msg_list = gtr_po_get_messages (po);

      gtr_translation_memory_store_list (data->tm, msg_list);

      g_object_unref (po);
    }
  else
    {
      GtkWidget *dialog;

      gtk_progress_bar_set_fraction (data->progress, 1.0);

      dialog = gtk_message_dialog_new (data->parent,
                                       GTK_DIALOG_DESTROY_WITH_PARENT,
                                       GTK_MESSAGE_INFO,
                                       GTK_BUTTONS_CLOSE, NULL);

      gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (dialog),
                                     _
                                     ("<span weight=\"bold\" size=\"large\">Strings added to database</span>"));

      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);

      return FALSE;
    }

  percentage =
    (gdouble) g_slist_position (data->list,
                                l) / (gdouble) g_slist_length (data->list);

  /*
   * Set the progress only if the values are reasonable.
   */
  if (percentage > 0.0 || percentage < 1.0)
    {
      /*
       * Set the progressbar status.
       */
      gtk_progress_bar_set_fraction (data->progress, percentage);
    }

  return TRUE;
}

static void
destroy_idle_data (gpointer data)
{
  IdleData *d = (IdleData *) data;

  gtk_widget_hide (GTK_WIDGET (d->progress));
  g_slist_foreach (d->list, (GFunc) g_object_unref, NULL);
  g_slist_free (d->list);

  g_free (d);
}

static void
on_add_database_button_clicked (GtkButton * button, GtrPreferencesDialog * dlg)
{
  GFile *dir;
  gchar *dir_name;
  IdleData *data;

  data = g_new0 (IdleData, 1);
  data->list = NULL;

  dir_name = g_settings_get_string (dlg->priv->tm_settings,
                                    GTR_SETTINGS_PO_DIRECTORY);

  dir = g_file_new_for_path (dir_name);
  g_free (dir_name);

  if (g_settings_get_boolean (dlg->priv->tm_settings,
                              GTR_SETTINGS_RESTRICT_TO_FILENAME))
    {
      gchar *restriction;

      restriction = g_settings_get_string (dlg->priv->tm_settings,
                                           GTR_SETTINGS_FILENAME_RESTRICTION);
      gtr_utils_scan_dir (dir, &data->list, restriction);
      g_free (restriction);
    }
  else
    gtr_utils_scan_dir (dir, &data->list, NULL);

  data->tm =
    GTR_TRANSLATION_MEMORY (gtr_application_get_translation_memory (GTR_APP));
  data->progress = GTK_PROGRESS_BAR (dlg->priv->add_database_progressbar);
  data->parent = GTK_WINDOW (dlg);

  gtk_widget_show (dlg->priv->add_database_progressbar);
  g_idle_add_full (G_PRIORITY_HIGH_IDLE + 30,
                   (GSourceFunc) add_to_database,
                   data, (GDestroyNotify) destroy_idle_data);

  g_object_unref (dir);
}

static void
setup_tm_pages (GtrPreferencesDialog * dlg)
{
  GtrProfileManager *prof_manager;
  GtrProfile *profile;
  const gchar *language_code;
  gchar *filename = NULL;

  prof_manager = gtr_profile_manager_get_default ();
  profile = gtr_profile_manager_get_active_profile (prof_manager);

  if (profile != NULL)
    {
      language_code = gtr_profile_get_language_code (profile);
      filename = g_strconcat (language_code, ".po", NULL);

      gtk_entry_set_text (GTK_ENTRY (dlg->priv->tm_lang_entry), filename);
    }
  g_object_unref (prof_manager);

  if (filename != NULL)
    g_settings_set_string (dlg->priv->tm_settings,
                           GTR_SETTINGS_FILENAME_RESTRICTION,
                           filename);

  g_free (filename);

  g_settings_bind (dlg->priv->tm_settings,
                   GTR_SETTINGS_RESTRICT_TO_FILENAME,
                   dlg->priv->use_lang_profile_in_tm,
                   "active",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
  g_settings_bind (dlg->priv->tm_settings,
                   GTR_SETTINGS_MAX_MISSING_WORDS,
                   dlg->priv->missing_words_spinbutton,
                   "value",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
  g_settings_bind (dlg->priv->tm_settings,
                   GTR_SETTINGS_MAX_LENGTH_DIFF,
                   dlg->priv->sentence_length_spinbutton,
                   "value",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
  g_settings_bind (dlg->priv->tm_settings,
                   GTR_SETTINGS_PO_DIRECTORY,
                   dlg->priv->directory_entry,
                   "text",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
  g_settings_bind (dlg->priv->tm_settings,
                   GTR_SETTINGS_FILENAME_RESTRICTION,
                   dlg->priv->tm_lang_entry,
                   "text",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);

  g_signal_connect (GTK_BUTTON (dlg->priv->search_button), "clicked",
                    G_CALLBACK (on_search_button_clicked), dlg);

  g_signal_connect (GTK_BUTTON (dlg->priv->add_database_button), "clicked",
                    G_CALLBACK (on_add_database_button_clicked), dlg);
}

/***************Plugins pages****************/
static void
setup_plugin_pages (GtrPreferencesDialog * dlg)
{
  GtkWidget *alignment;
  GtkWidget *page_content;

  alignment = gtk_alignment_new (0., 0., 1., 1.);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 12, 12, 12, 12);

  page_content = gtr_plugin_manager_new ();
  g_return_if_fail (page_content != NULL);

  gtk_container_add (GTK_CONTAINER (alignment), page_content);

  gtk_box_pack_start (GTK_BOX (dlg->priv->plugins_box),
                      alignment, TRUE, TRUE, 0);

  gtk_widget_show_all (page_content);
}


static void
dialog_response_handler (GtkDialog * dlg, gint res_id)
{
  switch (res_id)
    {
    default:
      gtk_widget_destroy (GTK_WIDGET (dlg));
    }
}

static void
gtr_preferences_dialog_init (GtrPreferencesDialog * dlg)
{
  gboolean ret;
  GtkWidget *error_widget, *action_area;
  GtkBox *content_area;
  gchar *root_objects[] = {
    "notebook",
    "adjustment1",
    "adjustment2",
    "adjustment3",
    "model1",
    NULL
  };
  gchar *path;

  dlg->priv = GTR_PREFERENCES_DIALOG_GET_PRIVATE (dlg);

  dlg->priv->ui_settings = g_settings_new ("org.gnome.gtranslator.preferences.ui");
  dlg->priv->editor_settings = g_settings_new ("org.gnome.gtranslator.preferences.editor");
  dlg->priv->tm_settings = g_settings_new ("org.gnome.gtranslator.preferences.tm");
  dlg->priv->files_settings = g_settings_new ("org.gnome.gtranslator.preferences.files");

  gtk_dialog_add_buttons (GTK_DIALOG (dlg),
                          GTK_STOCK_CLOSE,
                          GTK_RESPONSE_CLOSE,
                          GTK_STOCK_HELP, GTK_RESPONSE_HELP, NULL);

  gtk_window_set_title (GTK_WINDOW (dlg), _("Gtranslator Preferences"));
  gtk_window_set_resizable (GTK_WINDOW (dlg), FALSE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);

  action_area = gtk_dialog_get_action_area (GTK_DIALOG (dlg));
  content_area = GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dlg)));

  /* HIG defaults */
  gtk_container_set_border_width (GTK_CONTAINER (dlg), 5);
  gtk_box_set_spacing (content_area, 2);    /* 2 * 5 + 2 = 12 */
  gtk_container_set_border_width (GTK_CONTAINER (action_area), 5);
  gtk_box_set_spacing (GTK_BOX (action_area), 4);

  g_signal_connect (dlg,
                    "response", G_CALLBACK (dialog_response_handler), NULL);

  /*Glade */

  path = gtr_dirs_get_ui_file ("gtr-preferences-dialog.ui");
  ret = gtr_utils_get_ui_objects (path,
                                  root_objects,
                                  &error_widget,
                                  "notebook", &dlg->priv->notebook,
                                  "warn_if_fuzzy_checkbutton",
                                  &dlg->
                                  priv->warn_if_contains_fuzzy_checkbutton,
                                  "delete_compiled_checkbutton",
                                  &dlg->priv->delete_compiled_checkbutton,
                                  "autosave_checkbutton",
                                  &dlg->priv->autosave_checkbutton,
                                  "autosave_interval_spinbutton",
                                  &dlg->priv->autosave_interval_spinbutton,
                                  "autosave_hbox", &dlg->priv->autosave_hbox,
                                  "create_backup_checkbutton",
                                  &dlg->priv->create_backup_checkbutton,
                                  "highlight_checkbutton",
                                  &dlg->priv->highlight_syntax_checkbutton,
                                  "visible_whitespace_checkbutton",
                                  &dlg->priv->visible_whitespace_checkbutton,
                                  "use_custom_font_checkbutton",
                                  &dlg->priv->use_custom_font_checkbutton,
                                  "editor_font_fontbutton",
                                  &dlg->priv->editor_font_fontbutton,
                                  "editor_font_hbox",
                                  &dlg->priv->editor_font_hbox,
                                  "unmark_fuzzy_checkbutton",
                                  &dlg->
                                  priv->unmark_fuzzy_when_changed_checkbutton,
                                  "spellcheck_checkbutton",
                                  &dlg->priv->spellcheck_checkbutton,
                                  "profile_treeview",
                                  &dlg->priv->profile_treeview, "add_button",
                                  &dlg->priv->add_button, "edit_button",
                                  &dlg->priv->edit_button, "delete_button",
                                  &dlg->priv->delete_button,
                                  "directory_entry",
                                  &dlg->priv->directory_entry,
                                  "search_button", &dlg->priv->search_button,
                                  "add_database_button",
                                  &dlg->priv->add_database_button,
                                  "add_database_progressbar",
                                  &dlg->priv->add_database_progressbar,
                                  "use_lang_profile_in_tm",
                                  &dlg->priv->use_lang_profile_in_tm,
                                  "tm_lang_entry", &dlg->priv->tm_lang_entry,
                                  "missing_words_spinbutton",
                                  &dlg->priv->missing_words_spinbutton,
                                  "sentence_length_spinbutton",
                                  &dlg->priv->sentence_length_spinbutton,
                                  "gdl_combobox", &dlg->priv->gdl_combobox,
                                  "scheme_color_combobox",
                                  &dlg->priv->scheme_color_combobox,
                                  "plugins_box", &dlg->priv->plugins_box,
                                  NULL);
  g_free (path);

  if (!ret)
    {
      gtk_widget_show (error_widget);
      gtk_box_pack_start (content_area, error_widget, TRUE, TRUE, 0);

      return;
    }

  gtk_box_pack_start (content_area, dlg->priv->notebook, FALSE, FALSE, 0);

  gtk_container_set_border_width (GTK_CONTAINER (dlg->priv->notebook), 5);

  setup_files_pages (dlg);
  setup_editor_pages (dlg);
  setup_profile_pages (dlg);
  setup_interface_pages (dlg);
  setup_tm_pages (dlg);
  setup_plugin_pages (dlg);
}

static void
gtr_preferences_dialog_dispose (GObject * object)
{
  GtrPreferencesDialog *dlg = GTR_PREFERENCES_DIALOG (object);

  if (dlg->priv->ui_settings != NULL)
    {
      g_object_unref (dlg->priv->ui_settings);
      dlg->priv->ui_settings = NULL;
    }

  if (dlg->priv->tm_settings != NULL)
    {
      g_object_unref (dlg->priv->tm_settings);
      dlg->priv->tm_settings = NULL;
    }

  if (dlg->priv->editor_settings != NULL)
    {
      g_object_unref (dlg->priv->editor_settings);
      dlg->priv->editor_settings = NULL;
    }

  if (dlg->priv->files_settings != NULL)
    {
      g_object_unref (dlg->priv->files_settings);
      dlg->priv->files_settings = NULL;
    }

  G_OBJECT_CLASS (gtr_preferences_dialog_parent_class)->dispose (object);
}

static void
gtr_preferences_dialog_class_init (GtrPreferencesDialogClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrPreferencesDialogPrivate));

  object_class->dispose = gtr_preferences_dialog_dispose;
}

void
gtr_show_preferences_dialog (GtrWindow * window)
{
  static GtkWidget *dlg = NULL;

  g_return_if_fail (GTR_IS_WINDOW (window));

  if (dlg == NULL)
    {
      dlg = GTK_WIDGET (g_object_new (GTR_TYPE_PREFERENCES_DIALOG, NULL));
      g_signal_connect (dlg,
                        "destroy", G_CALLBACK (gtk_widget_destroyed), &dlg);
      gtk_widget_show_all (dlg);
    }

  if (GTK_WINDOW (window) != gtk_window_get_transient_for (GTK_WINDOW (dlg)))
    {
      gtk_window_set_transient_for (GTK_WINDOW (dlg), GTK_WINDOW (window));
    }

  gtk_window_present (GTK_WINDOW (dlg));
}
