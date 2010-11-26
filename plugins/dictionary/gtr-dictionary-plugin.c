/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-application.h"
#include "gtr-dictionary-plugin.h"
#include "gtr-dict-panel.h"
#include "gtr-window.h"
#include "gtr-window-activatable.h"

#include <libpeas-gtk/peas-gtk-configurable.h>
#include <glib/gi18n-lib.h>

#define GTR_DICT_PLUGIN_GET_PRIVATE(object) \
				(G_TYPE_INSTANCE_GET_PRIVATE ((object),	\
				GTR_TYPE_DICT_PLUGIN,		\
				GtrDictPluginPrivate))


struct _GtrDictPluginPrivate
{
  GtrWindow *window;
  GtkWidget *dict;
};

enum
{
  PROP_0,
  PROP_WINDOW
};

static void gtr_window_activatable_iface_init (GtrWindowActivatableInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED (GtrDictPlugin,
                                gtr_dict_plugin,
                                PEAS_TYPE_EXTENSION_BASE,
                                0,
                                G_IMPLEMENT_INTERFACE_DYNAMIC
                                (GTR_TYPE_WINDOW_ACTIVATABLE,
                                 gtr_window_activatable_iface_init)
                                _gtr_dict_panel_register_type (type_module))

static GtkWidget *
create_dict_panel (GtrWindow * window)
{
  GtkWidget *panel;

  panel = gtr_dict_panel_new (window);

  gtk_widget_show (panel);

  return panel;
}


static void
gtr_dict_plugin_set_property (GObject * object,
                              guint prop_id,
                              const GValue * value,
                              GParamSpec * pspec)
{
  GtrDictPluginPrivate *priv = GTR_DICT_PLUGIN (object)->priv;

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
gtr_dict_plugin_get_property (GObject * object,
                              guint prop_id,
                              GValue * value, GParamSpec * pspec)
{
  GtrDictPluginPrivate *priv = GTR_DICT_PLUGIN (object)->priv;

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
gtr_dict_plugin_init (GtrDictPlugin * plugin)
{
  plugin->priv = GTR_DICT_PLUGIN_GET_PRIVATE (plugin);
}

static void
gtr_dict_plugin_dispose (GObject * object)
{
  GtrDictPluginPrivate *priv = GTR_DICT_PLUGIN (object)->priv;

  if (priv->window != NULL)
    {
      g_object_unref (priv->window);
      priv->window = NULL;
    }

  G_OBJECT_CLASS (gtr_dict_plugin_parent_class)->dispose (object);
}

static void
gtr_dict_plugin_activate (GtrWindowActivatable * activatable)
{
  GtrDictPluginPrivate *priv = GTR_DICT_PLUGIN (activatable)->priv;

  gtr_application_register_icon (GTR_APP, "gnome-dictionary.png",
                                 "dictionary-icon");

  priv->dict = create_dict_panel (priv->window);

  gtr_window_add_widget (priv->window,
                         priv->dict,
                         "GtrDictionaryPlugin",
                         _("Dictionary"),
                         "dictionary-icon",
                         GTR_WINDOW_PLACEMENT_LEFT);
}

static void
gtr_dict_plugin_deactivate (GtrWindowActivatable * activatable)
{
  GtrDictPluginPrivate *priv = GTR_DICT_PLUGIN (activatable)->priv;

  gtr_window_remove_widget (priv->window, priv->dict);
}

static void
gtr_dict_plugin_class_init (GtrDictPluginClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gtr_dict_plugin_dispose;
  object_class->set_property = gtr_dict_plugin_set_property;
  object_class->get_property = gtr_dict_plugin_get_property;

  g_object_class_override_property (object_class, PROP_WINDOW, "window");
  g_type_class_add_private (object_class, sizeof (GtrDictPluginPrivate));
}

static void
gtr_dict_plugin_class_finalize (GtrDictPluginClass * klass)
{
}

static void
gtr_window_activatable_iface_init (GtrWindowActivatableInterface * iface)
{
  iface->activate = gtr_dict_plugin_activate;
  iface->deactivate = gtr_dict_plugin_deactivate;
}

G_MODULE_EXPORT void
peas_register_types (PeasObjectModule * module)
{
  gtr_dict_plugin_register_type (G_TYPE_MODULE (module));

  peas_object_module_register_extension_type (module,
      GTR_TYPE_WINDOW_ACTIVATABLE,
      GTR_TYPE_DICT_PLUGIN);
}
