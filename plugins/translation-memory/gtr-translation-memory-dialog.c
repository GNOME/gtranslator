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
#include "gtr-utils.h"
#include "gtr-po.h"

#include <glib/gi18n.h>


#define GTR_TRANSLATION_MEMORY_DIALOG_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE((object), GTR_TYPE_TRANSLATION_MEMORY_DIALOG, GtrTranslationMemoryDialogPrivate))

struct _GtrTranslationMemoryDialogPrivate
{
  GSettings *tm_settings;

  GtkWidget *directory_entry;
  GtkWidget *search_button;
  GtkWidget *add_database_button;
  GtkWidget *add_database_progressbar;
  GtkWidget *tm_lang_entry;
  GtkWidget *use_lang_profile_in_tm;

  GtrTranslationMemory *translation_memory;
};

G_DEFINE_TYPE (GtrTranslationMemoryDialog, gtr_translation_memory_dialog, GTK_TYPE_DIALOG)

static void
gtr_translation_memory_dialog_finalize (GObject *object)
{
  G_OBJECT_CLASS (gtr_translation_memory_dialog_parent_class)->finalize (object);
}

static void
gtr_translation_memory_dialog_dispose (GObject *object)
{
  GtrTranslationMemoryDialogPrivate *priv = GTR_TRANSLATION_MEMORY_DIALOG (object)->priv;

  if (priv->tm_settings != NULL)
    {
      g_object_unref (priv->tm_settings);
      priv->tm_settings = NULL;
    }

  G_OBJECT_CLASS (gtr_translation_memory_dialog_parent_class)->dispose (object);
}

static void
gtr_translation_memory_dialog_class_init (GtrTranslationMemoryDialogClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gtr_translation_memory_dialog_finalize;
  object_class->dispose = gtr_translation_memory_dialog_dispose;

  g_type_class_add_private (object_class, sizeof (GtrTranslationMemoryDialogPrivate));
}

/***************Translation Memory pages****************/
static void
response_filechooser_cb (GtkDialog                  *dialog,
                         gint                        response_id,
                         GtrTranslationMemoryDialog *dlg)
{
  if (response_id == GTK_RESPONSE_YES)
    {
      gchar *filename;

      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      gtk_entry_set_text (GTK_ENTRY (dlg->priv->directory_entry),
                          filename);
      g_settings_set_string (dlg->priv->tm_settings,
                             "po-directory",
                             filename);
      g_free (filename);
    }

  gtk_widget_destroy (GTK_WIDGET (dialog));
}

static void
on_search_button_clicked (GtkButton                  *button,
                          GtrTranslationMemoryDialog *dlg)
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

  gtk_widget_show (filechooser);
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
                                     _("<span weight=\"bold\" size=\"large\">Strings added to database</span>"));

      g_signal_connect (dialog, "response",
                        G_CALLBACK (gtk_widget_destroy), NULL);
      gtk_widget_show (dialog);

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
      /* Set the progressbar status. */
      gtk_progress_bar_set_fraction (data->progress, percentage);
    }

  return TRUE;
}

static void
destroy_idle_data (gpointer data)
{
  IdleData *d = (IdleData *) data;

  gtk_widget_hide (GTK_WIDGET (d->progress));

  g_slist_free_full (d->list, g_object_unref);
  g_free (d);
}

static void
on_add_database_button_clicked (GtkButton                  *button,
                                GtrTranslationMemoryDialog *dlg)
{
  GFile *dir;
  gchar *dir_name;
  IdleData *data;

  dir_name = g_settings_get_string (dlg->priv->tm_settings,
                                    "po-directory");

  /* If dir name is empty, show a warning message */
  if (*dir_name == '\0')
    {
      GtkWidget *dialog;
      dialog = gtk_message_dialog_new (GTK_WINDOW (dlg),
                                       GTK_DIALOG_DESTROY_WITH_PARENT,
                                       GTK_MESSAGE_WARNING,
                                       GTK_BUTTONS_CLOSE,
                                       _("Please specify a valid path to build the translation memory"));

      gtk_widget_show (dialog);
      g_signal_connect (dialog, "response",
                        G_CALLBACK (gtk_widget_destroy), NULL);
      g_free (dir_name);
      return;
    }

  dir = g_file_new_for_path (dir_name);
  g_free (dir_name);

  data = g_new0 (IdleData, 1);
  data->list = NULL;

  if (g_settings_get_boolean (dlg->priv->tm_settings,
                              "restrict-to-filename"))
    {
      gchar *restriction;

      restriction = g_settings_get_string (dlg->priv->tm_settings,
                                           "filename-restriction");
      gtr_utils_scan_dir (dir, &data->list, restriction);
      g_free (restriction);
    }
  else
    gtr_utils_scan_dir (dir, &data->list, NULL);

  data->tm = dlg->priv->translation_memory;
  data->progress = GTK_PROGRESS_BAR (dlg->priv->add_database_progressbar);
  data->parent = GTK_WINDOW (dlg);

  gtk_widget_show (dlg->priv->add_database_progressbar);
  g_idle_add_full (G_PRIORITY_HIGH_IDLE + 30,
                   (GSourceFunc) add_to_database,
                   data, (GDestroyNotify) destroy_idle_data);

  g_object_unref (dir);
}

static void
gtr_translation_memory_dialog_init (GtrTranslationMemoryDialog *dlg)
{
  GtrTranslationMemoryDialogPrivate *priv;
  GtkWidget *action_area;
  GtkWidget *content_area;
  GtkBuilder *builder;
  GtkWidget *content;
  GtrProfileManager *prof_manager;
  GtrProfile *profile;
  const gchar *language_code;
  gchar *filename = NULL;
  GError *error = NULL;
  gchar *root_objects[] = {
    "translation-memory-box",
    NULL
  };

  dlg->priv = GTR_TRANSLATION_MEMORY_DIALOG_GET_PRIVATE (dlg);
  priv = dlg->priv;

  priv->tm_settings = g_settings_new ("org.gnome.gtranslator.plugins.translation-memory");

  gtk_dialog_add_buttons (GTK_DIALOG (dlg),
                          GTK_STOCK_CLOSE,
                          GTK_RESPONSE_CLOSE,
                          NULL);

  gtk_window_set_title (GTK_WINDOW (dlg), _("Gtranslator Translation Memory Manager"));
  gtk_window_set_resizable (GTK_WINDOW (dlg), FALSE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);

  action_area = gtk_dialog_get_action_area (GTK_DIALOG (dlg));
  content_area = gtk_dialog_get_content_area (GTK_DIALOG (dlg));

  /* HIG defaults */
  gtk_container_set_border_width (GTK_CONTAINER (dlg), 5);
  gtk_box_set_spacing (GTK_BOX (content_area), 2);    /* 2 * 5 + 2 = 12 */
  gtk_container_set_border_width (GTK_CONTAINER (action_area), 5);
  gtk_box_set_spacing (GTK_BOX (action_area), 4);

  g_signal_connect (dlg, "response",
                    G_CALLBACK (gtk_widget_destroy), NULL);

  builder = gtk_builder_new ();
  gtk_builder_add_objects_from_resource (builder, "/org/gnome/gtranslator/plugins/translation-memory/ui/gtr-translation-memory-dialog.ui", root_objects, &error);
  content = GTK_WIDGET (gtk_builder_get_object (builder, "translation-memory-box"));
  g_object_ref (content);
  priv->directory_entry = GTK_WIDGET (gtk_builder_get_object (builder, "directory-entry"));
  priv->search_button = GTK_WIDGET (gtk_builder_get_object (builder, "search-button"));
  priv->add_database_button = GTK_WIDGET (gtk_builder_get_object (builder, "add-database-button"));
  priv->add_database_progressbar = GTK_WIDGET (gtk_builder_get_object (builder, "add-database-progressbar"));
  priv->use_lang_profile_in_tm = GTK_WIDGET (gtk_builder_get_object (builder, "use-lang-profile-in-tm"));
  priv->tm_lang_entry = GTK_WIDGET (gtk_builder_get_object (builder, "tm-lang-entry"));
  g_object_unref (builder);

  gtk_box_pack_start (GTK_BOX (content_area), content, TRUE, TRUE, 0);
  g_object_unref (content);

  prof_manager = gtr_profile_manager_get_default ();
  profile = gtr_profile_manager_get_active_profile (prof_manager);

  if (profile != NULL)
    {
      language_code = gtr_profile_get_language_code (profile);
      filename = g_strconcat (language_code, ".po", NULL);

      gtk_entry_set_text (GTK_ENTRY (priv->tm_lang_entry), filename);
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
                   priv->directory_entry,
                   "text",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
  g_settings_bind (priv->tm_settings,
                   "filename-restriction",
                   priv->tm_lang_entry,
                   "text",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);

  g_signal_connect (GTK_BUTTON (priv->search_button), "clicked",
                    G_CALLBACK (on_search_button_clicked), dlg);

  g_signal_connect (GTK_BUTTON (priv->add_database_button), "clicked",
                    G_CALLBACK (on_add_database_button_clicked), dlg);
}

GtkWidget *
gtr_translation_memory_dialog_new (GtrTranslationMemory *translation_memory)
{
  GtrTranslationMemoryDialog *dlg;

  dlg = GTR_TRANSLATION_MEMORY_DIALOG (g_object_new (GTR_TYPE_TRANSLATION_MEMORY_DIALOG, NULL));

  /* FIXME: use a property */
  dlg->priv->translation_memory = translation_memory;

  return GTK_WIDGET (dlg);
}
