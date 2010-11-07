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

#include "gtr-insert-params-plugin.h"
#include "gtr-msg.h"
#include "gtr-notebook.h"
#include "gtr-window.h"
#include "gtr-window-activatable.h"

#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <string.h>

struct _GtrInsertParamsPluginPrivate
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

G_DEFINE_DYNAMIC_TYPE_EXTENDED (GtrInsertParamsPlugin,
                                gtr_insert_params_plugin,
                                PEAS_TYPE_EXTENSION_BASE,
                                0,
                                G_IMPLEMENT_INTERFACE_DYNAMIC (GTR_TYPE_WINDOW_ACTIVATABLE,
                                                               gtr_window_activatable_iface_init))

static GSList *params = NULL;
static gint param_position;

static const gchar param_regex[] =
  "\\%\\%|\\%" "(?:[1-9][0-9]*\\$)?"  /* argument */
  "[#0\\-\\ \\+\\'I]*"                /* flags */
  "(?:[1-9][0-9]*|\\*)?"              /* width */
  "(?:\\.\\-?(?:[0-9]+|\\*))?"        /* precision */
  "(?:hh|ll|[hlLqjzt])?"              /* length modifier */
  "[diouxXeEfFgGaAcsCSpnm]";          /* conversion specifier */

static void
on_next_tag_activated (GtkAction * action, GtrWindow * window)
{
  GtrView *view;
  GtkTextBuffer *buffer;
  GSList *param;

  if (params == NULL)
    return;

  if (param_position >= g_slist_length (params))
    param_position = 0;

  param = g_slist_nth (params, param_position);

  view = gtr_window_get_active_view (window);

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

  gtk_text_buffer_begin_user_action (buffer);
  gtk_text_buffer_insert_at_cursor (buffer,
                                    (const gchar *) param->data,
                                    strlen (param->data));
  gtk_text_buffer_end_user_action (buffer);

  param_position++;
}

static const GtkActionEntry action_entries[] = {
  {"NextParam", NULL, N_("_Next Param"), "<control><shift>K",
   N_("Insert the next param of the message"),
   G_CALLBACK (on_next_tag_activated)},
  {"InsertParams", NULL, N_("_Insert Params")}
};

static const gchar submenu[] =
  "<ui>"
  "  <menubar name='MainMenu'>"
  "    <menu name='EditMenu' action='Edit'>"
  "      <placeholder name='EditOps_1'>"
  "        <menuitem name='EditNextParam' action='NextParam' />"
  "        <menuitem name='EditInsertParams' action='InsertParams' />"
  "      </placeholder>"
  "    </menu>"
  "  </menubar>"
  "</ui>";

static void
update_ui (GtrInsertParamsPlugin *plugin)
{
  GtrInsertParamsPluginPrivate *priv = plugin->priv;
  GtkTextView *view;
  GtkAction *action;

  view = GTK_TEXT_VIEW (gtr_window_get_active_view (priv->window));

  action = gtk_action_group_get_action (priv->action_group, "InsertParams");
  gtk_action_set_sensitive (action,
                            (view != NULL) &&
                            gtk_text_view_get_editable (view));

  action = gtk_action_group_get_action (priv->action_group, "NextParam");
  gtk_action_set_sensitive (action,
                            (view != NULL) &&
                            gtk_text_view_get_editable (view));
}

static void
gtr_insert_params_plugin_init (GtrInsertParamsPlugin *plugin)
{
  plugin->priv = G_TYPE_INSTANCE_GET_PRIVATE (plugin,
                                              GTR_TYPE_INSERT_PARAMS_PLUGIN,
                                              GtrInsertParamsPluginPrivate);
}

static void
gtr_insert_params_plugin_dispose (GObject *object)
{
  GtrInsertParamsPluginPrivate *priv = GTR_INSERT_PARAMS_PLUGIN (object)->priv;

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

  G_OBJECT_CLASS (gtr_insert_params_plugin_parent_class)->dispose (object);
}

static void
gtr_insert_params_plugin_finalize (GObject *object)
{
  g_slist_free (params);
  params = NULL;

  G_OBJECT_CLASS (gtr_insert_params_plugin_parent_class)->finalize (object);
}

static void
gtr_insert_params_plugin_set_property (GObject      *object,
                                       guint         prop_id,
                                       const GValue *value,
                                       GParamSpec   *pspec)
{
  GtrInsertParamsPluginPrivate *priv = GTR_INSERT_PARAMS_PLUGIN (object)->priv;

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
gtr_insert_params_plugin_get_property (GObject    *object,
                                       guint       prop_id,
                                       GValue     *value,
                                       GParamSpec *pspec)
{
  GtrInsertParamsPluginPrivate *priv = GTR_INSERT_PARAMS_PLUGIN (object)->priv;

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
on_menuitem_activated (GtkMenuItem * item, GtrWindow * window)
{
  const gchar *name;
  GtkWidget *label;
  GtrView *view;
  GtkTextBuffer *buffer;

  label = gtk_bin_get_child (GTK_BIN (item));
  name = gtk_label_get_text (GTK_LABEL (label));

  view = gtr_window_get_active_view (window);

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

  gtk_text_buffer_begin_user_action (buffer);
  gtk_text_buffer_insert_at_cursor (buffer, name, strlen (name));
  gtk_text_buffer_end_user_action (buffer);
}

static void
parse_list (GtrWindow * window)
{
  GtkUIManager *manager;
  GtkWidget *insert_params, *next_param;
  GtkWidget *menuitem;
  GtkWidget *menu;
  GSList *l = params;

  manager = gtr_window_get_ui_manager (window);

  insert_params = gtk_ui_manager_get_widget (manager,
                                             "/MainMenu/EditMenu/EditOps_1/EditInsertParams");
  next_param = gtk_ui_manager_get_widget (manager,
                                          "/MainMenu/EditMenu/EditOps_1/EditNextParam");

  if (params == NULL)
    {
      gtk_menu_item_set_submenu (GTK_MENU_ITEM (insert_params), NULL);
      gtk_widget_set_sensitive (insert_params, FALSE);
      gtk_widget_set_sensitive (next_param, FALSE);
      return;
    }

  gtk_widget_set_sensitive (insert_params, TRUE);
  gtk_widget_set_sensitive (next_param, TRUE);

  menu = gtk_menu_new ();
  do
    {
      menuitem = gtk_menu_item_new_with_label ((const gchar *) l->data);
      gtk_widget_show (menuitem);

      g_signal_connect (menuitem, "activate",
                        G_CALLBACK (on_menuitem_activated), window);

      gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
    }
  while ((l = g_slist_next (l)));

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (insert_params), menu);
}

static void
showed_message_cb (GtrTab * tab, GtrMsg * msg, GtrWindow * window)
{
  const gchar *msgid;
  GRegex *regex;
  GMatchInfo *match_info;
  gchar *word;
  gint i;

  if (params != NULL)
    {
      g_slist_foreach (params, (GFunc) g_free, NULL);
      g_slist_free (params);
      params = NULL;
    }

  /*
   * If we show another message we have to restart the index
   * of the params
   */
  param_position = 0;

  msgid = gtr_msg_get_msgid (msg);

  /*
   * Regular expression
   */
  regex = g_regex_new (param_regex, 0, 0, NULL);
  g_regex_match (regex, msgid, 0, &match_info);
  while (g_match_info_matches (match_info))
    {
      gchar *word_collate;

      word = g_match_info_fetch (match_info, 0);
      word_collate = g_utf8_collate_key (word, -1);
      for (i = 0; i < g_slist_length (params); i++)
        {
          gchar *param_collate;
          gchar *param = g_slist_nth_data (params, i);

          param_collate = g_utf8_collate_key (param, -1);
          if (strcmp (param_collate, word_collate) == 0)
            {
              g_free (word);
              word = NULL;
            }
          g_free (param_collate);
        }
      g_free (word_collate);

      if (word != NULL)
        params = g_slist_append (params, word);
      g_match_info_next (match_info, NULL);
    }
  g_match_info_free (match_info);
  g_regex_unref (regex);

  parse_list (window);
}


static void
page_added_cb (GtkNotebook * notebook,
               GtkWidget * child, guint page_num, GtrWindow * window)
{
  g_signal_connect (child, "showed-message",
                    G_CALLBACK (showed_message_cb), window);
}

static void
gtr_insert_params_plugin_activate (GtrWindowActivatable *activatable)
{
  GtrInsertParamsPluginPrivate *priv = GTR_INSERT_PARAMS_PLUGIN (activatable)->priv;
  GtkUIManager *manager;
  GError *error = NULL;
  GtrNotebook *notebook;
  GList *tabs = NULL;

  manager = gtr_window_get_ui_manager (priv->window);

  priv->action_group = gtk_action_group_new ("GtrInsertParamsPluginActions");
  gtk_action_group_set_translation_domain (priv->action_group,
                                           GETTEXT_PACKAGE);
  gtk_action_group_add_actions (priv->action_group, action_entries,
                                G_N_ELEMENTS (action_entries), priv->window);

  gtk_ui_manager_insert_action_group (manager, priv->action_group, -1);

  priv->ui_id = gtk_ui_manager_add_ui_from_string (manager,
                                                   submenu, -1,
                                                   &error);
  if (error)
    {
      g_warning ("%s", error->message);
      g_error_free (error);
      return;
    }

  update_ui (GTR_INSERT_PARAMS_PLUGIN (activatable));

  /*Adding menuitems */

  notebook = gtr_window_get_notebook (priv->window);

  g_signal_connect (GTK_NOTEBOOK (notebook),
                    "page-added", G_CALLBACK (page_added_cb), priv->window);

  tabs = gtr_window_get_all_tabs (priv->window);

  if (tabs == NULL)
    return;
  do
    {
      g_signal_connect (tabs->data, "showed-message",
                        G_CALLBACK (showed_message_cb), priv->window);
    }
  while ((tabs = g_list_next (tabs)));
}

static void
gtr_insert_params_plugin_deactivate (GtrWindowActivatable *activatable)
{
  GtrInsertParamsPluginPrivate *priv = GTR_INSERT_PARAMS_PLUGIN (activatable)->priv;
  GtrNotebook *notebook;
  GtkUIManager *manager;

  manager = gtr_window_get_ui_manager (priv->window);

  gtk_ui_manager_remove_ui (manager, priv->ui_id);
  gtk_ui_manager_remove_action_group (manager, priv->action_group);

  notebook = gtr_window_get_notebook (priv->window);

  g_signal_handlers_disconnect_by_func (notebook, page_added_cb, priv->window);
}

static void
gtr_insert_params_plugin_update_state (GtrWindowActivatable *activatable)
{
  update_ui (GTR_INSERT_PARAMS_PLUGIN (activatable));
}

static void
gtr_insert_params_plugin_class_init (GtrInsertParamsPluginClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gtr_insert_params_plugin_dispose;
  object_class->finalize = gtr_insert_params_plugin_finalize;
  object_class->set_property = gtr_insert_params_plugin_set_property;
  object_class->get_property = gtr_insert_params_plugin_get_property;

  g_object_class_override_property (object_class, PROP_WINDOW, "window");

  g_type_class_add_private (klass, sizeof (GtrInsertParamsPluginPrivate));
}

static void
gtr_insert_params_plugin_class_finalize (GtrInsertParamsPluginClass * klass)
{
}

static void
gtr_window_activatable_iface_init (GtrWindowActivatableInterface *iface)
{
  iface->activate = gtr_insert_params_plugin_activate;
  iface->deactivate = gtr_insert_params_plugin_deactivate;
  iface->update_state = gtr_insert_params_plugin_update_state;
}

G_MODULE_EXPORT void
peas_register_types (PeasObjectModule *module)
{
  gtr_insert_params_plugin_register_type (G_TYPE_MODULE (module));

  peas_object_module_register_extension_type (module,
                                              GTR_TYPE_WINDOW_ACTIVATABLE,
                                              GTR_TYPE_INSERT_PARAMS_PLUGIN);
}
