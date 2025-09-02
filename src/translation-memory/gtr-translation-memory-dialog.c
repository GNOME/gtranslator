/*
 * Copyright (C) 2012 Ignacio Casal Quinteiro <icq@gnome.org>
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "gtr-translation-memory-dialog.h"
#include "gtr-profile-manager.h"
#include "gtr-translation-memory-utils.h"
#include "gtr-po.h"
#include "gtr-utils.h"

#include <glib/gi18n.h>


typedef struct
{
  GSettings *tm_settings;

  GtkWidget *directory;
  GtkWidget *configuration;

  GtkWidget *add_database_button;
  GtkWidget *add_database_progressbar;
  GtkWidget *tm_lang_entry;
  GtkWidget *use_lang_profile_in_tm;

  GtrTranslationMemory *translation_memory;
} GtrTranslationMemoryDialogPrivate;

struct _GtrTranslationMemoryDialog
{
  AdwDialog parent;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrTranslationMemoryDialog,
                            gtr_translation_memory_dialog, ADW_TYPE_DIALOG)

static void
select_folder_cb (GtkFileDialog *dialog, GAsyncResult *res, gpointer user_data)
{
  GtrTranslationMemoryDialog *dlg;
  GtrTranslationMemoryDialogPrivate *priv;

  g_autofree char *filename = NULL;
  g_autoptr (GFile) file = NULL;

  dlg = GTR_TRANSLATION_MEMORY_DIALOG (user_data);
  priv = gtr_translation_memory_dialog_get_instance_private (dlg);

  file = gtk_file_dialog_select_folder_finish (dialog, res, NULL);

  if (!file) return;

  filename = g_file_get_path (file);
  adw_action_row_set_subtitle (ADW_ACTION_ROW (priv->directory), filename);
  g_settings_set_string (priv->tm_settings, "po-directory", filename);
}

static void
select_directory_activated_cb (GtrTranslationMemoryDialog *dlg)
{
  g_autoptr(GtkFileDialog) dialog = NULL;

  dialog = gtk_file_dialog_new ();
  gtk_file_dialog_set_title (dialog, _("Select PO directory"));
  gtk_file_dialog_set_modal (dialog, TRUE);

  gtk_file_dialog_select_folder (
    dialog,
    GTK_WINDOW (gtk_widget_get_root (GTK_WIDGET (dlg))),
    NULL,
    (GAsyncReadyCallback) (select_folder_cb),
    dlg
  );
}

static void
gtr_translation_memory_dialog_finalize (GObject *object)
{
  G_OBJECT_CLASS (gtr_translation_memory_dialog_parent_class)->finalize (object);
}

static void
gtr_translation_memory_dialog_dispose (GObject *object)
{
  GtrTranslationMemoryDialog *dlg = GTR_TRANSLATION_MEMORY_DIALOG (object);
  GtrTranslationMemoryDialogPrivate *priv = gtr_translation_memory_dialog_get_instance_private (dlg);

  g_clear_object (&priv->tm_settings);

  G_OBJECT_CLASS (gtr_translation_memory_dialog_parent_class)->dispose (object);
}

static void
gtr_translation_memory_dialog_class_init (GtrTranslationMemoryDialogClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gtr_translation_memory_dialog_finalize;
  object_class->dispose = gtr_translation_memory_dialog_dispose;

  gtk_widget_class_set_template_from_resource (
    widget_class,
    "/org/gnome/gtranslator/plugins/translation-memory/ui/gtr-translation-memory-dialog.ui"
  );

  gtk_widget_class_bind_template_child_private (widget_class, GtrTranslationMemoryDialog, directory);
  gtk_widget_class_bind_template_child_private (widget_class, GtrTranslationMemoryDialog, configuration);
  gtk_widget_class_bind_template_child_private (widget_class, GtrTranslationMemoryDialog, add_database_button);
  gtk_widget_class_bind_template_child_private (widget_class, GtrTranslationMemoryDialog, add_database_progressbar);
  gtk_widget_class_bind_template_child_private (widget_class, GtrTranslationMemoryDialog, tm_lang_entry);
  gtk_widget_class_bind_template_child_private (widget_class, GtrTranslationMemoryDialog, use_lang_profile_in_tm);

  gtk_widget_class_bind_template_callback (widget_class, select_directory_activated_cb);
}

/***************Translation Memory pages****************/

typedef struct _IdleData
{
  GSList *list;
  GtkProgressBar *progress;
  GtrTranslationMemory *tm;
  GtkWidget *add_database_button;
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
      g_autoptr(GError) error = NULL;
      g_autoptr(GtrPo) po = NULL;

      location = (GFile *) l->data;
      po = gtr_po_new_from_file (location, &error);
      if (error)
        return TRUE;

      msg_list = gtr_po_get_messages (po);

      gtr_translation_memory_store_list (data->tm, msg_list);
    }
  else
    {
      gtk_widget_set_sensitive (data->add_database_button, TRUE);
      return FALSE;
    }

  percentage =
    (gdouble) g_slist_position (data->list,
                                l) / (gdouble) g_slist_length (data->list);

  /*
   * Set the progress only if the values are reasonable.
   */
  if (percentage > 0.0 && percentage < 1.0)
    {
      /* Set the progressbar status. */
      gtk_progress_bar_set_fraction (data->progress, percentage);
    }

  return TRUE;
}

static void
destroy_idle_data (gpointer data)
{
  IdleData *d = (IdleData *) data;

  gtk_widget_set_visible (GTK_WIDGET (d->progress), FALSE);

  g_slist_free_full (d->list, g_object_unref);
  g_free (d);
}

typedef struct
{
  GFile *dir;
  gchar *restriction;
} ScanDirTaskData;

static void
task_data_destroy (ScanDirTaskData *data)
{
  if (data->restriction)
    g_free (data->restriction);
  g_object_unref (data->dir);

  g_free (data);
}

static void
g_slist_free_files (GSList *list) {
  g_slist_free_full (list, g_object_unref);
}

static void
scan_dir_task_func (GTask                      *task,
                    GtrTranslationMemoryDialog *dlg,
                    ScanDirTaskData            *data,
                    GCancellable               *cancellable)
{
  GSList *list = NULL;
  gtr_scan_dir (data->dir, &list, data->restriction);
  g_task_return_pointer (task, list, (GDestroyNotify)g_slist_free_files);
}

static void
scan_dir_task_ready_cb (GtrTranslationMemoryDialog *dlg,
                        GTask                      *task,
                        IdleData                   *data)
{
  data->list = g_task_propagate_pointer (task, NULL);

  g_idle_add_full (G_PRIORITY_HIGH_IDLE + 30,
                   (GSourceFunc) add_to_database,
                   data, (GDestroyNotify) destroy_idle_data);
}

static void
launch_gtr_scan_dir_task (GtrTranslationMemoryDialog *dlg,
                          ScanDirTaskData            *data)
{
  GTask *task;
  IdleData *idata;
  GtrTranslationMemoryDialogPrivate *priv = gtr_translation_memory_dialog_get_instance_private (dlg);

  idata = g_new0 (IdleData, 1);
  idata->list = NULL;
  idata->tm = priv->translation_memory;
  idata->progress = GTK_PROGRESS_BAR (priv->add_database_progressbar);
  idata->add_database_button = priv->add_database_button;

  gtk_progress_bar_pulse (idata->progress);
  gtk_widget_set_visible (priv->add_database_progressbar, TRUE);

  gtk_widget_set_sensitive (priv->add_database_button, FALSE);

  task = g_task_new (dlg, NULL, (GAsyncReadyCallback)scan_dir_task_ready_cb,
                     idata);

  g_task_set_task_data (task, data, (GDestroyNotify)task_data_destroy);

  g_task_run_in_thread (task,
                        (GTaskThreadFunc) scan_dir_task_func);
}

static void
on_add_database_button_clicked (GtkButton                  *button,
                                GtrTranslationMemoryDialog *dlg)
{
  gchar *dir_name;
  GtrTranslationMemoryDialogPrivate *priv = gtr_translation_memory_dialog_get_instance_private (dlg);
  ScanDirTaskData *scan_dir_data;

  dir_name = g_settings_get_string (priv->tm_settings,
                                    "po-directory");

  /* If dir name is empty, show a warning message */
  if (*dir_name == '\0')
    {
      g_free (dir_name);
      return;
    }

  scan_dir_data = g_new0 (ScanDirTaskData, 1);
  scan_dir_data->dir = g_file_new_for_path (dir_name);
  g_free (dir_name);

  if (g_settings_get_boolean (priv->tm_settings,
                              "restrict-to-filename"))
    {
      scan_dir_data->restriction = g_settings_get_string (priv->tm_settings,
                                                          "filename-restriction");
    }
  launch_gtr_scan_dir_task (dlg, scan_dir_data);
}

static void
gtr_translation_memory_dialog_init (GtrTranslationMemoryDialog *dlg)
{
  GtrTranslationMemoryDialogPrivate *priv = gtr_translation_memory_dialog_get_instance_private (dlg);
  GtrProfileManager *prof_manager;
  GtrProfile *profile;
  const gchar *language_code;
  gchar *filename = NULL;

  priv->tm_settings = g_settings_new ("org.gnome.gtranslator.plugins.translation-memory");

  gtk_widget_init_template (GTK_WIDGET (dlg));

  prof_manager = gtr_profile_manager_get_default ();
  profile = gtr_profile_manager_get_active_profile (prof_manager);

  if (profile != NULL)
    {
      language_code = gtr_profile_get_language_code (profile);
      filename = g_strconcat (language_code, ".po", NULL);

      GtkEntryBuffer *entry_buffer = gtk_entry_get_buffer (GTK_ENTRY (priv->tm_lang_entry));
      gtk_entry_buffer_set_text (entry_buffer, filename, -1);
    }
  g_object_unref (prof_manager);

  if (filename != NULL)
    g_settings_set_string (priv->tm_settings,
                           "filename-restriction",
                           filename);

  g_free (filename);

  g_settings_bind (priv->tm_settings,
                   "restrict-to-filename",
                   priv->use_lang_profile_in_tm,
                   "active",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
  g_settings_bind (priv->tm_settings,
                   "po-directory",
                   priv->directory,
                   "subtitle",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
  g_settings_bind (priv->tm_settings,
                   "filename-restriction",
                   priv->tm_lang_entry,
                   "text",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);

  g_signal_connect (GTK_BUTTON (priv->add_database_button), "clicked",
                    G_CALLBACK (on_add_database_button_clicked), dlg);
}

GtkWidget *
gtr_translation_memory_dialog_new (GtrTranslationMemory *translation_memory)
{
  GtrTranslationMemoryDialog *dlg;
  GtrTranslationMemoryDialogPrivate *priv;

  dlg = GTR_TRANSLATION_MEMORY_DIALOG (g_object_new (GTR_TYPE_TRANSLATION_MEMORY_DIALOG, NULL));
  priv = gtr_translation_memory_dialog_get_instance_private (dlg);

  /* FIXME: use a property */
  priv->translation_memory = translation_memory;

  return GTK_WIDGET (dlg);
}
