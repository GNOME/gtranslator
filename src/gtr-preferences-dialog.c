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

#define GTR_PREFERENCES_DIALOG_GET_PRIVATE(object) (G_TYPE_INSTANCE_GET_PRIVATE ( \
                                                    (object), \
                                                    GTR_TYPE_PREFERENCES_DIALOG, \
                                                    GtrPreferencesDialogPrivate))


G_DEFINE_TYPE (GtrPreferencesDialog, gtr_preferences_dialog, GTK_TYPE_DIALOG)

struct _GtrPreferencesDialogPrivate
{
  GSettings *ui_settings;
  GSettings *editor_settings;
  GSettings *files_settings;

  GtkWidget *notebook;

  /* Files->General */
  GtkWidget *warn_if_contains_fuzzy_checkbutton;

  /* Files->Autosave */
  GtkWidget *autosave_checkbutton;
  GtkWidget *autosave_interval_spinbutton;
  GtkWidget *autosave_grid;
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
  gchar *markup;

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

          markup = g_strdup_printf("<span weight=\"bold\" size=\"large\">%s</span>",
                                   _("Impossible to remove the active profile"));
          gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (dialog), markup);
          g_free(markup);

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

          markup = g_strdup_printf("<span weight=\"bold\" size=\"large\">%s</span>",
                                   _("Are you sure you want to delete this profile?"));
          gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (dialog), markup);
          g_free(markup);

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
  GtrPreferencesDialogPrivate *priv;
  GtkWidget *action_area;
  GtkWidget *profiles_toolbar;
  GtkWidget *profiles_scrolled_window;
  GtkBuilder *builder;
  GtkBox *content_area;
  GtkStyleContext *context;
  gchar *root_objects[] = {
    "notebook",
    "adjustment1",
    "adjustment2",
    "adjustment3",
    "model1",
    NULL
  };

  dlg->priv = GTR_PREFERENCES_DIALOG_GET_PRIVATE (dlg);
  priv = dlg->priv;

  priv->ui_settings = g_settings_new ("org.gnome.gtranslator.preferences.ui");
  priv->editor_settings = g_settings_new ("org.gnome.gtranslator.preferences.editor");
  priv->files_settings = g_settings_new ("org.gnome.gtranslator.preferences.files");

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

  builder = gtk_builder_new ();
  gtk_builder_add_objects_from_resource (builder, "/org/gnome/translator/gtr-preferences-dialog.ui",
                                         root_objects, NULL);
  priv->notebook = GTK_WIDGET (gtk_builder_get_object (builder, "notebook"));
  g_object_ref (priv->notebook);
  priv->warn_if_contains_fuzzy_checkbutton = GTK_WIDGET (gtk_builder_get_object (builder, "warn_if_fuzzy_checkbutton"));
  priv->autosave_checkbutton = GTK_WIDGET (gtk_builder_get_object (builder, "autosave_checkbutton"));
  priv->autosave_interval_spinbutton = GTK_WIDGET (gtk_builder_get_object (builder, "autosave_interval_spinbutton"));
  priv->autosave_grid = GTK_WIDGET (gtk_builder_get_object (builder, "autosave_grid"));
  priv->create_backup_checkbutton = GTK_WIDGET (gtk_builder_get_object (builder, "create_backup_checkbutton"));
  priv->highlight_syntax_checkbutton = GTK_WIDGET (gtk_builder_get_object (builder, "highlight_checkbutton"));
  priv->visible_whitespace_checkbutton = GTK_WIDGET (gtk_builder_get_object (builder, "visible_whitespace_checkbutton"));
  priv->use_custom_font_checkbutton = GTK_WIDGET (gtk_builder_get_object (builder, "use_custom_font_checkbutton"));
  priv->editor_font_fontbutton = GTK_WIDGET (gtk_builder_get_object (builder, "editor_font_fontbutton"));
  priv->editor_font_hbox = GTK_WIDGET (gtk_builder_get_object (builder, "editor_font_hbox"));
  priv->unmark_fuzzy_when_changed_checkbutton = GTK_WIDGET (gtk_builder_get_object (builder, "unmark_fuzzy_checkbutton"));
  priv->spellcheck_checkbutton = GTK_WIDGET (gtk_builder_get_object (builder, "spellcheck_checkbutton"));
  priv->profile_treeview = GTK_WIDGET (gtk_builder_get_object (builder, "profile_treeview"));
  priv->add_button = GTK_WIDGET (gtk_builder_get_object (builder, "add-button"));
  priv->edit_button = GTK_WIDGET (gtk_builder_get_object (builder, "edit-button"));
  priv->delete_button = GTK_WIDGET (gtk_builder_get_object (builder, "delete-button"));
  profiles_toolbar = GTK_WIDGET (gtk_builder_get_object (builder, "profiles-toolbar"));
  profiles_scrolled_window = GTK_WIDGET (gtk_builder_get_object (builder, "profiles-scrolledwindow"));
  g_object_unref (builder);

  gtk_box_pack_start (content_area, priv->notebook, FALSE, FALSE, 0);

  gtk_container_set_border_width (GTK_CONTAINER (priv->notebook), 5);

  context = gtk_widget_get_style_context (profiles_scrolled_window);
  gtk_style_context_set_junction_sides (context, GTK_JUNCTION_BOTTOM);

  context = gtk_widget_get_style_context (profiles_toolbar);
  gtk_style_context_set_junction_sides (context, GTK_JUNCTION_TOP);

  setup_files_pages (dlg);
  setup_editor_pages (dlg);
  setup_profile_pages (dlg);
}

static void
gtr_preferences_dialog_dispose (GObject * object)
{
  GtrPreferencesDialog *dlg = GTR_PREFERENCES_DIALOG (object);

  g_clear_object (&dlg->priv->ui_settings);
  g_clear_object (&dlg->priv->editor_settings);
  g_clear_object (&dlg->priv->files_settings);

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
