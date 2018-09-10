/*
 * Copyright (C) 2018  Daniel Garcia Moreno <danigm@gnome.org>
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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-actions.h"
#include "gtr-projects.h"
#include "gtr-window.h"
#include "gtr-utils.h"

typedef struct
{
  GtkRecentManager *recent_manager;
  GtkWidget *titlebar;
  GtkWidget *main_box;
  GtkWidget *project_list;
  GtkWidget *open_button;

  GtrWindow *main_window;
} GtrProjectsPrivate;

struct _GtrProjects
{
  GtkBin parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrProjects, gtr_projects, GTK_TYPE_BIN)


static void project_add_cb (GtkButton *btn, GtrProjects *self);
static void init_recent (GtrProjects *self);
static void file_open_cb (GtkListBox *box, GtkListBoxRow *row, gpointer data);


static void
gtr_projects_dispose (GObject *object)
{
  G_OBJECT_CLASS (gtr_projects_parent_class)->dispose (object);
}

static void
gtr_projects_finalize (GObject *object)
{
  G_OBJECT_CLASS (gtr_projects_parent_class)->finalize (object);
}

static void
gtr_projects_class_init (GtrProjectsClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gtr_projects_finalize;
  object_class->dispose = gtr_projects_dispose;

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gnome/translator/gtr-projects.ui");

  gtk_widget_class_bind_template_child_private (widget_class, GtrProjects, titlebar);
  gtk_widget_class_bind_template_child_private (widget_class, GtrProjects, main_box);
  gtk_widget_class_bind_template_child_private (widget_class, GtrProjects, project_list);

  gtk_widget_class_bind_template_child_private (widget_class, GtrProjects, open_button);
}

static void
gtr_projects_init (GtrProjects *self)
{
  GtrProjectsPrivate *priv = gtr_projects_get_instance_private (self);
  GtkListBox *list;
  gtk_widget_init_template (GTK_WIDGET (self));

  list = GTK_LIST_BOX (priv->project_list);
  priv->main_window = NULL;
  priv->recent_manager = gtk_recent_manager_get_default ();

  g_signal_connect (priv->open_button,
                    "clicked",
                    G_CALLBACK (project_add_cb),
                    self);

  g_signal_connect (list, "row-activated", G_CALLBACK (file_open_cb), self);
  init_recent (self);
}

GtrProjects*
gtr_projects_new (GtrWindow *window) {
  GtrProjects *self = g_object_new (GTR_TYPE_PROJECTS, NULL);
  GtrProjectsPrivate *priv = gtr_projects_get_instance_private (self);

  priv->main_window = window;
  return self;
}

GtkWidget *
gtr_projects_get_header (GtrProjects *self)
{
  GtrProjectsPrivate *priv = gtr_projects_get_instance_private (self);
  return priv->titlebar;
}

// static functions
static void
file_open_cb (GtkListBox *box,
              GtkListBoxRow *row,
              gpointer data)
{
  gint index = gtk_list_box_row_get_index (row);
  GtrProjects *self = GTR_PROJECTS (data);
  GtrProjectsPrivate *priv = gtr_projects_get_instance_private (self);

  GList *recents = gtk_recent_manager_get_items (priv->recent_manager);
  GtkRecentInfo *info = g_list_nth_data (recents, index);

  GError *error;
  GFile *file;

  file = g_file_new_for_uri (gtk_recent_info_get_uri (info));
  if (!gtr_open (file, priv->main_window, &error)) {
    g_error_free (error);
    goto out;
  }

  gtr_window_show_poeditor (priv->main_window);

out:
  g_object_unref (file);
  g_list_free_full (recents, (GDestroyNotify)gtk_recent_info_unref);
}

static void
project_add_cb (GtkButton   *btn,
                GtrProjects *self)
{
  GtrProjectsPrivate *priv = gtr_projects_get_instance_private (self);
  GtrWindow *window = GTR_WINDOW (priv->main_window);
  gtr_open_file_dialog (NULL, window);
}

static void
init_recent (GtrProjects *self)
{
  GtrProjectsPrivate *priv = gtr_projects_get_instance_private (self);
  GtkListBox *list = GTK_LIST_BOX (priv->project_list);

  GList *recents = gtk_recent_manager_get_items (priv->recent_manager);
  GList *it = g_list_first (recents);

  GList *children, *iter;

  children = gtk_container_get_children (GTK_CONTAINER (list));
  for(iter = children; iter != NULL; iter = g_list_next (iter))
    gtk_widget_destroy (GTK_WIDGET (iter->data));
  g_list_free (children);

  while (it)
    {
      const gchar *name = gtk_recent_info_get_uri_display (it->data);
      GtkWidget *label = gtk_label_new (name);
      GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);

      gtk_label_set_xalign (GTK_LABEL (label), 0.0);
      gtk_label_set_yalign (GTK_LABEL (label), 0.5);
      gtk_box_pack_start (GTK_BOX (box), label, TRUE, TRUE, 6);

      gtk_widget_show_all (box);
      gtk_list_box_insert (list, box, -1);
      it = g_list_next (it);
    }

  g_list_free_full (recents, (GDestroyNotify)gtk_recent_info_unref);
}

void
gtr_projects_recent_add (GtrProjects *self,
                         GFile *location,
                         gchar *project_id)
{
  GtrProjectsPrivate *priv = gtr_projects_get_instance_private (self);
  GtkRecentData *recent_data;
  gchar *uri;
  gchar *path;
  gchar *display_name;

  uri = g_file_get_uri (location);
  path = g_file_get_path (location);
  display_name = gtr_utils_reduce_path ((const gchar *) path);

  recent_data = g_slice_new (GtkRecentData);

  recent_data->display_name = display_name;
  recent_data->description = NULL;
  recent_data->mime_type = "text/x-gettext-translation";
  recent_data->app_name = (gchar *) g_get_application_name ();
  recent_data->app_exec = g_strjoin (" ", g_get_prgname (), "%u", NULL);
  recent_data->groups = NULL;
  recent_data->is_private = FALSE;

  if (!gtk_recent_manager_add_full (priv->recent_manager,
                                    uri, recent_data))
    {
      g_warning ("Unable to add '%s' to the list of recently used documents",
                 uri);
    } else {
      init_recent (self);
    }

  g_free (uri);
  g_free (path);
  g_free (display_name);
  g_free (recent_data->app_exec);
  g_slice_free (GtkRecentData, recent_data);
}

