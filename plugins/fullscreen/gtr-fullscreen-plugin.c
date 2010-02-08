/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANPOILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-fullscreen-plugin.h"
#include "gtr-window.h"

#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <string.h>

#define WINDOW_DATA_KEY "GtrFullscreenPluginWindowData"
#define MENU_PATH "/MainMenu/ViewMenu/ViewOps_1"

GTR_PLUGIN_REGISTER_TYPE (GtrFullscreenPlugin, gtr_fullscreen_plugin)
     static void on_fullscreen_activated (GtkToggleAction * action,
                                          GtrWindow * window)
{
  if (gtk_toggle_action_get_active (action))
    gtk_window_fullscreen (GTK_WINDOW (window));
  else
    gtk_window_unfullscreen (GTK_WINDOW (window));
}

static const GtkToggleActionEntry action_entries[] = {
  {"Fullscreen", NULL, N_("_Fullscreen"), "F11",
   N_("Place window on fullscreen state"),
   G_CALLBACK (on_fullscreen_activated)},
};

typedef struct
{
  GtkActionGroup *action_group;
  guint ui_id;
} WindowData;

static void
free_window_data (WindowData * data)
{
  g_return_if_fail (data != NULL);

  g_free (data);
}

static void
gtr_fullscreen_plugin_init (GtrFullscreenPlugin * message_table)
{
}

static void
gtr_fullscreen_plugin_finalize (GObject * object)
{
  G_OBJECT_CLASS (gtr_fullscreen_plugin_parent_class)->finalize (object);
}

static void
impl_activate (GtrPlugin * plugin, GtrWindow * window)
{
  GtkUIManager *manager;
  WindowData *data;
  GError *error = NULL;

  g_return_if_fail (GTR_IS_WINDOW (window));

  data = g_new (WindowData, 1);

  manager = gtr_window_get_ui_manager (window);

  data->action_group = gtk_action_group_new ("GtrFullscreenPluginActions");
  gtk_action_group_set_translation_domain (data->action_group,
                                           GETTEXT_PACKAGE);
  gtk_action_group_add_toggle_actions (data->action_group, action_entries,
                                       G_N_ELEMENTS (action_entries), window);

  gtk_ui_manager_insert_action_group (manager, data->action_group, -1);

  data->ui_id = gtk_ui_manager_new_merge_id (manager);

  if (data->ui_id == 0)
    {
      g_warning ("%s", error->message);
      g_error_free (error);
      g_free (data);
      return;
    }

  g_object_set_data_full (G_OBJECT (window),
                          WINDOW_DATA_KEY,
                          data, (GDestroyNotify) free_window_data);

  gtk_ui_manager_add_ui (manager,
                         data->ui_id,
                         MENU_PATH,
                         "Fullscreen",
                         "Fullscreen", GTK_UI_MANAGER_MENUITEM, FALSE);
}

static void
impl_deactivate (GtrPlugin * plugin, GtrWindow * window)
{
  GtkUIManager *manager;
  WindowData *data;

  manager = gtr_window_get_ui_manager (window);

  data =
    (WindowData *) g_object_get_data (G_OBJECT (window), WINDOW_DATA_KEY);
  g_return_if_fail (data != NULL);

  gtk_ui_manager_remove_ui (manager, data->ui_id);
  gtk_ui_manager_remove_action_group (manager, data->action_group);

  g_object_set_data (G_OBJECT (window), WINDOW_DATA_KEY, NULL);
}

static void
gtr_fullscreen_plugin_class_init (GtrFullscreenPluginClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtrPluginClass *plugin_class = GTR_PLUGIN_CLASS (klass);

  object_class->finalize = gtr_fullscreen_plugin_finalize;

  plugin_class->activate = impl_activate;
  plugin_class->deactivate = impl_deactivate;
}
