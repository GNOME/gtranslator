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

#include <glib/gi18n-lib.h>

#define WINDOW_DATA_KEY	"GtrAlternateLangPluginWindowData"
#define TAB_DATA_KEY    "GtrAlternateLangPluginTabData"
#define MENU_PATH "/MainMenu/ViewMenu/ViewOps_2"

#define GTR_MESSAGE_TABLE_GET_PRIVATE(object) \
				(G_TYPE_INSTANCE_GET_PRIVATE ((object),	\
				GTR_TYPE_MESSAGE_TABLE,		\
				GtrAlternateLangPluginPrivate))

GTR_PLUGIN_REGISTER_TYPE_WITH_CODE (GtrAlternateLangPlugin,
                                    gtr_alternate_lang_plugin,
                                    gtr_alternate_lang_panel_register_type
                                    (module);
  )
     static void on_alternate_lang_activated (GtkAction * action,
                                              GtrWindow * window)
{
  GtrTab *tab;
  GtkWidget *alternatelang;

  tab = gtr_window_get_active_tab (window);
  alternatelang = g_object_get_data (G_OBJECT (tab), TAB_DATA_KEY);

  gtr_tab_show_lateral_panel_widget (GTR_TAB (tab), alternatelang);
}

static const GtkActionEntry action_entries[] = {
  {"AlternateLang", NULL, N_("_Alternate Language"), "<control>L",
   N_("Show the Alternate Language panel"),
   G_CALLBACK (on_alternate_lang_activated)},
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
update_ui_real (GtrWindow * window, WindowData * data)
{
  GtrTab *tab;
  GtkAction *action;

  tab = gtr_window_get_active_tab (window);

  action = gtk_action_group_get_action (data->action_group, "AlternateLang");
  gtk_action_set_sensitive (action, (tab != NULL));
}

static void
gtr_alternate_lang_plugin_init (GtrAlternateLangPlugin * message_table)
{
}

static void
gtr_alternate_lang_plugin_finalize (GObject * object)
{
  G_OBJECT_CLASS (gtr_alternate_lang_plugin_parent_class)->finalize (object);
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
impl_activate (GtrPlugin * plugin, GtrWindow * window)
{
  GtrNotebook *notebook;
  GList *tabs = NULL;
  GtkUIManager *manager;
  WindowData *data;

  data = g_new (WindowData, 1);

  manager = gtr_window_get_ui_manager (window);

  data->action_group = gtk_action_group_new ("GtrAlternateLangPluginActions");
  gtk_action_group_set_translation_domain (data->action_group,
                                           GETTEXT_PACKAGE);
  gtk_action_group_add_actions (data->action_group, action_entries,
                                G_N_ELEMENTS (action_entries), window);

  gtk_ui_manager_insert_action_group (manager, data->action_group, -1);

  data->ui_id = gtk_ui_manager_new_merge_id (manager);

  g_object_set_data_full (G_OBJECT (window),
                          WINDOW_DATA_KEY,
                          data, (GDestroyNotify) free_window_data);

  gtk_ui_manager_add_ui (manager,
                         data->ui_id,
                         MENU_PATH,
                         "AlternateLang",
                         "AlternateLang", GTK_UI_MANAGER_MENUITEM, FALSE);

  notebook = gtr_window_get_notebook (window);

  g_signal_connect (GTK_NOTEBOOK (notebook),
                    "page-added",
                    G_CALLBACK (create_alternate_lang_plugin_panel), window);

  tabs = gtr_window_get_all_tabs (window);

  if (tabs == NULL)
    return;
  do
    {
      create_alternate_lang_plugin_panel (GTK_NOTEBOOK (notebook),
                                          tabs->data, 0, window);
    }
  while ((tabs = g_list_next (tabs)));
}

static void
impl_deactivate (GtrPlugin * plugin, GtrWindow * window)
{
  GtrNotebook *notebook;
  GtkWidget *alternatelang;
  GList *tabs;
  GtkUIManager *manager;
  WindowData *data;

  tabs = gtr_window_get_all_tabs (window);
  notebook = gtr_window_get_notebook (window);

  if (tabs != NULL)
    {
      do
        {
          alternatelang =
            g_object_get_data (G_OBJECT (tabs->data), TAB_DATA_KEY);
          gtr_tab_remove_widget_from_lateral_panel (GTR_TAB
                                                    (tabs->data),
                                                    alternatelang);

          g_object_set_data (G_OBJECT (tabs->data), WINDOW_DATA_KEY, NULL);
        }
      while ((tabs = g_list_next (tabs)));
    }

  g_signal_handlers_disconnect_by_func (notebook,
                                        create_alternate_lang_plugin_panel,
                                        window);

  /* Remove menuitem */
  manager = gtr_window_get_ui_manager (window);

  data =
    (WindowData *) g_object_get_data (G_OBJECT (window), WINDOW_DATA_KEY);
  g_return_if_fail (data != NULL);

  gtk_ui_manager_remove_ui (manager, data->ui_id);
  gtk_ui_manager_remove_action_group (manager, data->action_group);

  g_object_set_data (G_OBJECT (window), WINDOW_DATA_KEY, NULL);
}

static void
impl_update_ui (GtrPlugin * plugin, GtrWindow * window)
{
  WindowData *data;

  data =
    (WindowData *) g_object_get_data (G_OBJECT (window), WINDOW_DATA_KEY);
  g_return_if_fail (data != NULL);

  update_ui_real (window, data);
}

static void
  gtr_alternate_lang_plugin_class_init (GtrAlternateLangPluginClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtrPluginClass *plugin_class = GTR_PLUGIN_CLASS (klass);

  object_class->finalize = gtr_alternate_lang_plugin_finalize;

  plugin_class->activate = impl_activate;
  plugin_class->deactivate = impl_deactivate;
  plugin_class->update_ui = impl_update_ui;
}
