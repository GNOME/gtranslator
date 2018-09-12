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
  GtkWidget *titlebar;
  GtkWidget *main_box;
  GtkWidget *open_button;

  GtrWindow *main_window;
} GtrProjectsPrivate;

struct _GtrProjects
{
  GtkBin parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrProjects, gtr_projects, GTK_TYPE_BIN)


static void project_add_cb (GtkButton *btn, GtrProjects *self);


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

  gtk_widget_class_bind_template_child_private (widget_class, GtrProjects, open_button);
}

static void
gtr_projects_init (GtrProjects *self)
{
  GtrProjectsPrivate *priv = gtr_projects_get_instance_private (self);
  gtk_widget_init_template (GTK_WIDGET (self));

  priv->main_window = NULL;

  g_signal_connect (priv->open_button,
                    "clicked",
                    G_CALLBACK (project_add_cb),
                    self);
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
project_add_cb (GtkButton   *btn,
                GtrProjects *self)
{
  GtrProjectsPrivate *priv = gtr_projects_get_instance_private (self);
  GtrWindow *window = GTR_WINDOW (priv->main_window);
  gtr_open_file_dialog (NULL, window);
}

