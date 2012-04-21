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

#include "gtr-fullscreen-plugin.h"
#include "gtr-window.h"
#include "gtr-window-activatable.h"

#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <string.h>

#define MENU_PATH "/MainMenu/ViewMenu/ViewOps_1"

struct _GtrFullscreenPluginPrivate
{
  GtrWindow      *window;

  GtkActionGroup *action_group;
  guint           ui_id;
};

enum
{
  PROP_0,
  PROP_WINDOW
};

static void gtr_window_activatable_iface_init (GtrWindowActivatableInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED (GtrFullscreenPlugin,
                                gtr_fullscreen_plugin,
                                PEAS_TYPE_EXTENSION_BASE,
                                0,
                                G_IMPLEMENT_INTERFACE_DYNAMIC (GTR_TYPE_WINDOW_ACTIVATABLE,
                                                               gtr_window_activatable_iface_init))

static void
on_fullscreen_activated (GtkToggleAction *action,
                         GtrWindow       *window)
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

static void
gtr_fullscreen_plugin_init (GtrFullscreenPlugin *plugin)
{
  plugin->priv = G_TYPE_INSTANCE_GET_PRIVATE (plugin,
                                              GTR_TYPE_FULLSCREEN_PLUGIN,
                                              GtrFullscreenPluginPrivate);
}

static void
gtr_fullscreen_plugin_dispose (GObject *object)
{
  GtrFullscreenPluginPrivate *priv = GTR_FULLSCREEN_PLUGIN (object)->priv;

  g_clear_object (&priv->window);
  g_clear_object (&priv->action_group);

  G_OBJECT_CLASS (gtr_fullscreen_plugin_parent_class)->dispose (object);
}

static void
gtr_fullscreen_plugin_set_property (GObject      *object,
                                    guint         prop_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
  GtrFullscreenPluginPrivate *priv = GTR_FULLSCREEN_PLUGIN (object)->priv;

  switch (prop_id)
    {
      case PROP_WINDOW:
        priv->window = GTR_WINDOW (g_value_dup_object (value));
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
gtr_fullscreen_plugin_get_property (GObject    *object,
                                    guint       prop_id,
                                    GValue     *value,
                                    GParamSpec *pspec)
{
  GtrFullscreenPluginPrivate *priv = GTR_FULLSCREEN_PLUGIN (object)->priv;

  switch (prop_id)
    {
      case PROP_WINDOW:
        g_value_set_object (value, priv->window);
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
gtr_fullscreen_plugin_activate (GtrWindowActivatable *activatable)
{
  GtrFullscreenPluginPrivate *priv = GTR_FULLSCREEN_PLUGIN (activatable)->priv;
  GtkUIManager *manager;

  manager = gtr_window_get_ui_manager (priv->window);

  priv->action_group = gtk_action_group_new ("GtrFullscreenPluginActions");
  gtk_action_group_set_translation_domain (priv->action_group,
                                           GETTEXT_PACKAGE);
  gtk_action_group_add_toggle_actions (priv->action_group, action_entries,
                                       G_N_ELEMENTS (action_entries),
                                       priv->window);

  gtk_ui_manager_insert_action_group (manager, priv->action_group, -1);

  priv->ui_id = gtk_ui_manager_new_merge_id (manager);

  if (priv->ui_id == 0)
    {
      g_warning ("Couldn not get a new merge id for fullscreen plugin");
      return;
    }

  gtk_ui_manager_add_ui (manager,
                         priv->ui_id,
                         MENU_PATH,
                         "Fullscreen",
                         "Fullscreen", GTK_UI_MANAGER_MENUITEM, FALSE);
}

static void
gtr_fullscreen_plugin_deactivate (GtrWindowActivatable *activatable)
{
  GtrFullscreenPluginPrivate *priv = GTR_FULLSCREEN_PLUGIN (activatable)->priv;
  GtkUIManager *manager;

  manager = gtr_window_get_ui_manager (priv->window);

  gtk_ui_manager_remove_ui (manager, priv->ui_id);
  gtk_ui_manager_remove_action_group (manager, priv->action_group);
}

static void
gtr_fullscreen_plugin_class_init (GtrFullscreenPluginClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gtr_fullscreen_plugin_dispose;
  object_class->set_property = gtr_fullscreen_plugin_set_property;
  object_class->get_property = gtr_fullscreen_plugin_get_property;

  g_object_class_override_property (object_class, PROP_WINDOW, "window");

  g_type_class_add_private (klass, sizeof (GtrFullscreenPluginPrivate));
}

static void
gtr_fullscreen_plugin_class_finalize (GtrFullscreenPluginClass *klass)
{
}

static void
gtr_window_activatable_iface_init (GtrWindowActivatableInterface *iface)
{
  iface->activate = gtr_fullscreen_plugin_activate;
  iface->deactivate = gtr_fullscreen_plugin_deactivate;
}

G_MODULE_EXPORT void
peas_register_types (PeasObjectModule *module)
{
  gtr_fullscreen_plugin_register_type (G_TYPE_MODULE (module));

  peas_object_module_register_extension_type (module,
                                              GTR_TYPE_WINDOW_ACTIVATABLE,
                                              GTR_TYPE_FULLSCREEN_PLUGIN);
}
