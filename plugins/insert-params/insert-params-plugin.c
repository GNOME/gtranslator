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

#include "insert-params-plugin.h"
#include "msg.h"
#include "notebook.h"
#include "window.h"

#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <string.h>

#define WINDOW_DATA_KEY "GtranslatorInsertParamsPluginWindowData"

GTR_PLUGIN_REGISTER_TYPE (GtranslatorInsertParamsPlugin,
			  gtranslator_insert_params_plugin)
     static GSList *params = NULL;
     static gint param_position;

     static const gchar param_regex[] = "\\%\\%|\\%" "(?:[1-9][0-9]*\\$)?"	// argument
       "[#0\\-\\ \\+\\'I]*"	// flags
       "(?:[1-9][0-9]*|\\*)?"	// width
       "(?:\\.\\-?(?:[0-9]+|\\*))?"	// precision
       "(?:hh|ll|[hlLqjzt])?"	// length modifier
       "[diouxXeEfFgGaAcsCSpnm]";	// conversion specifier

     static void
       on_next_tag_activated (GtkAction * action, GtranslatorWindow * window)
{
  GtranslatorView *view;
  GtkTextBuffer *buffer;
  GSList *param;

  if (params == NULL)
    return;

  if (param_position >= g_slist_length (params))
    param_position = 0;

  param = g_slist_nth (params, param_position);

  view = gtranslator_window_get_active_view (window);

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
  "      </placeholder>" "    </menu>" "  </menubar>" "</ui>";

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
update_ui_real (GtranslatorWindow * window, WindowData * data)
{
  GtkTextView *view;
  GtkAction *action;

  view = GTK_TEXT_VIEW (gtranslator_window_get_active_view (window));

  action = gtk_action_group_get_action (data->action_group, "InsertParams");
  gtk_action_set_sensitive (action,
			    (view != NULL) &&
			    gtk_text_view_get_editable (view));

  action = gtk_action_group_get_action (data->action_group, "NextParam");
  gtk_action_set_sensitive (action,
			    (view != NULL) &&
			    gtk_text_view_get_editable (view));
}

static void
gtranslator_insert_params_plugin_init (GtranslatorInsertParamsPlugin *
				       message_table)
{
}

static void
gtranslator_insert_params_plugin_finalize (GObject * object)
{
  if (params != NULL)
    {
      g_slist_free (params);
      params = NULL;
    }

  G_OBJECT_CLASS (gtranslator_insert_params_plugin_parent_class)->
    finalize (object);
}

static void
on_menuitem_activated (GtkMenuItem * item, GtranslatorWindow * window)
{
  const gchar *name;
  GtkWidget *label;
  GtranslatorView *view;
  GtkTextBuffer *buffer;

  label = gtk_bin_get_child (GTK_BIN (item));
  name = gtk_label_get_text (GTK_LABEL (label));

  view = gtranslator_window_get_active_view (window);

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

  gtk_text_buffer_begin_user_action (buffer);
  gtk_text_buffer_insert_at_cursor (buffer, name, strlen (name));
  gtk_text_buffer_end_user_action (buffer);
}

static void
parse_list (GtranslatorWindow * window)
{
  GtkUIManager *manager;
  GtkWidget *insert_params, *next_param;
  GtkWidget *menuitem;
  GtkWidget *menu;
  GSList *l = params;

  manager = gtranslator_window_get_ui_manager (window);

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
showed_message_cb (GtranslatorTab * tab,
		   GtranslatorMsg * msg, GtranslatorWindow * window)
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

  msgid = gtranslator_msg_get_msgid (msg);

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
	       GtkWidget * child, guint page_num, GtranslatorWindow * window)
{
  g_signal_connect (child, "showed-message",
		    G_CALLBACK (showed_message_cb), window);
}

static void
impl_activate (GtranslatorPlugin * plugin, GtranslatorWindow * window)
{
  GtkUIManager *manager;
  WindowData *data;
  GError *error = NULL;
  GtranslatorNotebook *notebook;
  GList *tabs = NULL;

  g_return_if_fail (GTR_IS_WINDOW (window));

  data = g_new (WindowData, 1);

  manager = gtranslator_window_get_ui_manager (window);

  data->action_group =
    gtk_action_group_new ("GtranslatorInsertParamsPluginActions");
  gtk_action_group_set_translation_domain (data->action_group,
					   GETTEXT_PACKAGE);
  gtk_action_group_add_actions (data->action_group, action_entries,
				G_N_ELEMENTS (action_entries), window);

  gtk_ui_manager_insert_action_group (manager, data->action_group, -1);

  data->ui_id = gtk_ui_manager_add_ui_from_string (manager,
						   submenu, -1, &error);
  if (error)
    {
      g_warning ("%s", error->message);
      g_error_free (error);
      g_free (data);
      return;
    }

  g_object_set_data_full (G_OBJECT (window),
			  WINDOW_DATA_KEY,
			  data, (GDestroyNotify) free_window_data);

  update_ui_real (window, data);

  /*Adding menuitems */

  notebook = gtranslator_window_get_notebook (window);

  g_signal_connect (GTK_NOTEBOOK (notebook),
		    "page-added", G_CALLBACK (page_added_cb), window);

  tabs = gtranslator_window_get_all_tabs (window);

  if (tabs == NULL)
    return;
  do
    {
      g_signal_connect (tabs->data, "showed-message",
			G_CALLBACK (showed_message_cb), window);
    }
  while ((tabs = g_list_next (tabs)));
}

static void
impl_deactivate (GtranslatorPlugin * plugin, GtranslatorWindow * window)
{
  GtranslatorNotebook *notebook;
  GtkUIManager *manager;
  WindowData *data;

  manager = gtranslator_window_get_ui_manager (window);

  data =
    (WindowData *) g_object_get_data (G_OBJECT (window), WINDOW_DATA_KEY);
  g_return_if_fail (data != NULL);

  gtk_ui_manager_remove_ui (manager, data->ui_id);
  gtk_ui_manager_remove_action_group (manager, data->action_group);

  g_object_set_data (G_OBJECT (window), WINDOW_DATA_KEY, NULL);

  notebook = gtranslator_window_get_notebook (window);

  g_signal_handlers_disconnect_by_func (notebook, page_added_cb, window);

  if (params != NULL)
    {
      g_slist_free (params);
      params = NULL;
    }
}

static void
impl_update_ui (GtranslatorPlugin * plugin, GtranslatorWindow * window)
{
  WindowData *data;

  data =
    (WindowData *) g_object_get_data (G_OBJECT (window), WINDOW_DATA_KEY);
  g_return_if_fail (data != NULL);

  update_ui_real (window, data);
}

static void
gtranslator_insert_params_plugin_class_init
  (GtranslatorInsertParamsPluginClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtranslatorPluginClass *plugin_class = GTR_PLUGIN_CLASS (klass);

  object_class->finalize = gtranslator_insert_params_plugin_finalize;

  plugin_class->activate = impl_activate;
  plugin_class->deactivate = impl_deactivate;
  plugin_class->update_ui = impl_update_ui;
}
