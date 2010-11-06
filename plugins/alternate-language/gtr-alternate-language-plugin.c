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

#include "gtr-alternate-language-plugin.h"
#include "gtr-alternate-language-panel.h"
#include "gtr-notebook.h"
#include "gtr-window.h"
#include "gtr-window-activatable.h"

#include <glib/gi18n.h>

#define TAB_DATA_KEY    "GtrAlternateLangPluginTabData"
#define MENU_PATH "/MainMenu/ViewMenu/ViewOps_2"

struct _GtrAlternateLangPluginPrivate
{
  GtrWindow *window;

  GtkActionGroup *action_group;
  guint ui_id;
};

enum
{
  PROP_0,
  PROP_WINDOW
};

static void gtr_window_activatable_iface_init (GtrWindowActivatableInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED (GtrAlternateLangPlugin,
                                gtr_alternate_lang_plugin,
                                PEAS_TYPE_EXTENSION_BASE,
                                0,
                                G_IMPLEMENT_INTERFACE_DYNAMIC (GTR_TYPE_WINDOW_ACTIVATABLE,
                                                               gtr_window_activatable_iface_init) \
                                                                                                  \
                                _gtr_alternate_lang_panel_register_type (type_module);            \
)

static void
on_alternate_lang_activated (GtkAction *action,
                             GtrWindow * window)
{
  GtrTab *tab;
  GtkWidget *alternatelang;

  tab = gtr_window_get_active_tab (window);
  alternatelang = g_object_get_data (G_OBJECT (tab), TAB_DATA_KEY);

  gtr_tab_show_lateral_panel_widget (GTR_TAB (tab), alternatelang);
}

static const GtkActionEntry action_entries[] = {
  {"AlternateLang", NULL, N_("_Alternate Language"), "<control>K",
   N_("Show the Alternate Language panel"),
   G_CALLBACK (on_alternate_lang_activated)},
};

static void
gtr_alternate_lang_plugin_init (GtrAlternateLangPlugin *plugin)
{
  plugin->priv = G_TYPE_INSTANCE_GET_PRIVATE (plugin,
                                              GTR_TYPE_ALTERNATE_LANG_PLUGIN,
                                              GtrAlternateLangPluginPrivate);
}

static void
gtr_alternate_lang_plugin_dispose (GObject *object)
{
  GtrAlternateLangPluginPrivate *priv = GTR_ALTERNATE_LANG_PLUGIN (object)->priv;

  if (priv->window != NULL)
    {
      g_object_unref (priv->window);
      priv->window = NULL;
    }

  if (priv->action_group != NULL)
    {
      g_object_unref (priv->action_group);
      priv->action_group = NULL;
    }

  G_OBJECT_CLASS (gtr_alternate_lang_plugin_parent_class)->dispose (object);
}

static void
gtr_alternate_lang_plugin_set_property (GObject      *object,
                                        guint         prop_id,
                                        const GValue *value,
                                        GParamSpec   *pspec)
{
  GtrAlternateLangPluginPrivate *priv = GTR_ALTERNATE_LANG_PLUGIN (object)->priv;

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
gtr_alternate_lang_plugin_get_property (GObject    *object,
                                        guint       prop_id,
                                        GValue     *value,
                                        GParamSpec *pspec)
{
  GtrAlternateLangPluginPrivate *priv = GTR_ALTERNATE_LANG_PLUGIN (object)->priv;

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
update_ui (GtrAlternateLangPlugin *plugin)
{
  GtrTab *tab;
  GtkAction *action;

  tab = gtr_window_get_active_tab (plugin->priv->window);

  action = gtk_action_group_get_action (plugin->priv->action_group, "AlternateLang");
  gtk_action_set_sensitive (action, (tab != NULL));
}

static void
create_alternate_lang_plugin_panel (GtkNotebook * notebook,
                                    GtkWidget * child,
                                    guint page_num, GtrWindow * window)
{
  GtkWidget *alternatelang;
  GtrPo *po;

  po = gtr_tab_get_po (GTR_TAB (child));

  g_return_if_fail (po != NULL);

  alternatelang = gtr_alternate_lang_panel_new (child);
  gtk_widget_show (alternatelang);

  gtr_tab_add_widget_to_lateral_panel (GTR_TAB (child),
                                       alternatelang,
                                       _("Alternate Language"));

  g_object_set_data (G_OBJECT (child), TAB_DATA_KEY, alternatelang);
}

static void
gtr_alternate_lang_plugin_activate (GtrWindowActivatable *activatable)
{
  GtrAlternateLangPluginPrivate *priv = GTR_ALTERNATE_LANG_PLUGIN (activatable)->priv;
  GtrNotebook *notebook;
  GtkUIManager *manager;
  GList *tabs = NULL;

  manager = gtr_window_get_ui_manager (priv->window);

  priv->action_group = gtk_action_group_new ("GtrAlternateLangPluginActions");
  gtk_action_group_set_translation_domain (priv->action_group,
                                           GETTEXT_PACKAGE);
  gtk_action_group_add_actions (priv->action_group, action_entries,
                                G_N_ELEMENTS (action_entries), priv->window);

  gtk_ui_manager_insert_action_group (manager, priv->action_group, -1);

  priv->ui_id = gtk_ui_manager_new_merge_id (manager);

  gtk_ui_manager_add_ui (manager,
                         priv->ui_id,
                         MENU_PATH,
                         "AlternateLang",
                         "AlternateLang", GTK_UI_MANAGER_MENUITEM, FALSE);

  notebook = gtr_window_get_notebook (priv->window);

  g_signal_connect (GTK_NOTEBOOK (notebook),
                    "page-added",
                    G_CALLBACK (create_alternate_lang_plugin_panel),
                    priv->window);

  tabs = gtr_window_get_all_tabs (priv->window);

  if (tabs == NULL)
    return;
  do
    {
      create_alternate_lang_plugin_panel (GTK_NOTEBOOK (notebook),
                                          tabs->data, 0, priv->window);
    }
  while ((tabs = g_list_next (tabs)));
}

static void
gtr_alternate_lang_plugin_deactivate (GtrWindowActivatable *activatable)
{
  GtrAlternateLangPluginPrivate *priv = GTR_ALTERNATE_LANG_PLUGIN (activatable)->priv;
  GtrNotebook *notebook;
  GtkWidget *alternatelang;
  GList *tabs;
  GtkUIManager *manager;

  tabs = gtr_window_get_all_tabs (priv->window);
  notebook = gtr_window_get_notebook (priv->window);

  if (tabs != NULL)
    {
      do
        {
          alternatelang =
            g_object_get_data (G_OBJECT (tabs->data), TAB_DATA_KEY);
          gtr_tab_remove_widget_from_lateral_panel (GTR_TAB
                                                    (tabs->data),
                                                    alternatelang);

          g_object_set_data (G_OBJECT (tabs->data), TAB_DATA_KEY, NULL);
        }
      while ((tabs = g_list_next (tabs)));
    }

  g_signal_handlers_disconnect_by_func (notebook,
                                        create_alternate_lang_plugin_panel,
                                        priv->window);

  /* Remove menuitem */
  manager = gtr_window_get_ui_manager (priv->window);

  gtk_ui_manager_remove_ui (manager, priv->ui_id);
  gtk_ui_manager_remove_action_group (manager, priv->action_group);
}

static void
gtr_alternate_lang_plugin_update_state (GtrWindowActivatable *activatable)
{
  update_ui (GTR_ALTERNATE_LANG_PLUGIN (activatable));
}

static void
gtr_alternate_lang_plugin_class_init (GtrAlternateLangPluginClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gtr_alternate_lang_plugin_dispose;
  object_class->set_property = gtr_alternate_lang_plugin_set_property;
  object_class->get_property = gtr_alternate_lang_plugin_get_property;

  g_object_class_override_property (object_class, PROP_WINDOW, "window");

  g_type_class_add_private (klass, sizeof (GtrAlternateLangPluginPrivate));
}

static void
gtr_alternate_lang_plugin_class_finalize (GtrAlternateLangPluginClass *klass)
{
}

static void
gtr_window_activatable_iface_init (GtrWindowActivatableInterface *iface)
{
  iface->activate = gtr_alternate_lang_plugin_activate;
  iface->deactivate = gtr_alternate_lang_plugin_deactivate;
  iface->update_state = gtr_alternate_lang_plugin_update_state;
}

G_MODULE_EXPORT void
peas_register_types (PeasObjectModule *module)
{
  gtr_alternate_lang_plugin_register_type (G_TYPE_MODULE (module));

  peas_object_module_register_extension_type (module,
                                              GTR_TYPE_WINDOW_ACTIVATABLE,
                                              GTR_TYPE_ALTERNATE_LANG_PLUGIN);
}
