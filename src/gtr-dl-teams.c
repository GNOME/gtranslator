/*
 * Copyright (C) 2018  Teja Cetinski <teja@cetinski.eu>
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
#include "gtr-dl-teams.h"
#include "gtr-window.h"
#include "gtr-utils.h"
#include <libsoup/soup.h>

#include <json-glib/json-glib.h>
#include <json-glib/json-gobject.h>

typedef struct
{
  GtkWidget *titlebar;
  GtkWidget *main_box;
  GtkWidget *open_button;
  GtkWidget *dl_button;
  GtkWidget *teams_combobox;
  GtkWidget *modules_combobox;

  GtkListStore *teams_store;
  GtkListStore *modules_store;

  GtrWindow *main_window;
} GtrDlTeamsPrivate;

struct _GtrDlTeams
{
  GtkBin parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrDlTeams, gtr_dl_teams, GTK_TYPE_BIN)

static void team_add_cb (GtkButton *btn, GtrDlTeams *self);

static void
gtr_dl_teams_list_add (JsonArray *array,
                       guint      index,
                       JsonNode  *element,
                       gpointer   data)
{
  JsonObject *object = json_node_get_object (element);

  gtk_list_store_insert_with_values(data, NULL, -1,
        0, json_object_get_string_member (object, "description"),
        -1);
  /*printf ("%s: %s\n",
          json_object_get_string_member (object, "id"),
          json_object_get_string_member (object, "description")
          );*/
}

static void
gtr_dl_modules_list_add (JsonArray *array,
                         guint      index,
                         JsonNode  *element,
                         gpointer   data)
{
  JsonObject *object = json_node_get_object (element);
  JsonObject *fieldsNode = json_object_get_object_member (object, "fields");

  gtk_list_store_insert_with_values(data, NULL, -1,0, json_object_get_string_member (fieldsNode, "name"),
        -1);
}

static void
gtr_dl_teams_parse_teams_json (GObject *object,
                               GAsyncResult *result,
                               gpointer user_data)
{
  g_autoptr(JsonParser) parser = json_parser_new ();
  g_autoptr(GInputStream) stream;
  GError *error = NULL;
  JsonNode *node = NULL;
  JsonArray *array = NULL;

  GtrDlTeams *widget = GTR_DL_TEAMS (user_data);
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (widget);

  /* Parse JSON */
  stream = soup_session_send_finish (SOUP_SESSION (object), result, &error);

  if (error)
    {
      // todo: display text in UI
      //printf("error! code: %d, message: %s\n", error->code, error->message);
    }
  else
    {
      json_parser_load_from_stream (parser, stream, NULL, NULL);

      node = json_parser_get_root (parser);
      array = json_node_get_array (node);

      /* Fill teams list store with values from JSON and set store as combo box model */
      json_array_foreach_element (array, gtr_dl_teams_list_add, GTK_TREE_MODEL (priv->teams_store));

      gtk_combo_box_set_model (GTK_COMBO_BOX (priv->teams_combobox), GTK_TREE_MODEL (priv->teams_store));
    }
}

static void
gtr_dl_teams_parse_modules_json (GObject *object,
                                 GAsyncResult *result,
                                 gpointer user_data)
{
  g_autoptr(JsonParser) parser = json_parser_new ();
  g_autoptr(GInputStream) stream;
	GError *error = NULL;

  JsonNode *node = NULL;
  JsonArray *array = NULL;

  GtrDlTeams *widget = GTR_DL_TEAMS (user_data);
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (widget);

  /* Parse JSON */
  stream = soup_session_send_finish (SOUP_SESSION (object), result, &error);
  if (error)
    {
      // todo: display text in UI
      //printf("error! code: %d, message: %s\n", error->code, error->message);
    }
  else
    {
      json_parser_load_from_stream (parser, stream, NULL, NULL);

      node = json_parser_get_root (parser);
      array = json_node_get_array (node);

      /* Fill modules list store with values from JSON and set store as combo box model */
      json_array_foreach_element (array, gtr_dl_modules_list_add, GTK_TREE_MODEL (priv->modules_store));

      gtk_combo_box_set_model (GTK_COMBO_BOX (priv->modules_combobox), GTK_TREE_MODEL (priv->modules_store));

      gtk_widget_set_sensitive (priv->modules_combobox, TRUE);
    }
}

static void
gtr_dl_teams_load_json (GtkButton *btn,
                        GtrDlTeams *self)
{
  /* Get team list JSON from DL */
  g_autoptr(SoupSession) session = soup_session_new ();
  g_autoptr(SoupMessage) message = soup_message_new ("GET", "https://l10n.gnome.org/teams/json");
  soup_session_send_async (session, message, NULL, gtr_dl_teams_parse_teams_json, self);

  /* Get module list JSON from DL */
  g_autoptr(SoupSession) session_modules = soup_session_new ();
  g_autoptr(SoupMessage) message_modules = soup_message_new ("GET", "https://l10n.gnome.org/module/json");
  soup_session_send_async (session_modules, message_modules, NULL, gtr_dl_teams_parse_modules_json, self);
}

static void
gtr_dl_teams_dispose (GObject *object)
{
  G_OBJECT_CLASS (gtr_dl_teams_parent_class)->dispose (object);
}

static void
gtr_dl_teams_finalize (GObject *object)
{
  G_OBJECT_CLASS (gtr_dl_teams_parent_class)->finalize (object);
}

static void
gtr_dl_teams_class_init (GtrDlTeamsClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gtr_dl_teams_finalize;
  object_class->dispose = gtr_dl_teams_dispose;

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gnome/translator/gtr-dl-teams.ui");

  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, titlebar);
  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, main_box);

  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, open_button);
  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, dl_button);

}

static void
gtr_dl_teams_init (GtrDlTeams *self)
{
  GtkCellRenderer *column;
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  gtk_widget_init_template (GTK_WIDGET (self));

  priv->main_window = NULL;

  /* Init teams and modules list stores */
  priv->teams_store = gtk_list_store_new (1, G_TYPE_STRING);
  priv->modules_store = gtk_list_store_new (1, G_TYPE_STRING);

  /* Add combo boxes for DL teams and modules */
  priv->teams_combobox = gtk_combo_box_new ();

  column = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(priv->teams_combobox), column, TRUE);

  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT(priv->teams_combobox), column,
                                 "text", 0,
                                 //"text", 1,
                                 NULL);

  gtk_container_add (GTK_CONTAINER (priv->main_box), priv->teams_combobox);

  priv->modules_combobox = gtk_combo_box_new ();

  column = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(priv->modules_combobox), column, TRUE);

  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT(priv->modules_combobox), column,
                                 "text", 0,
                                 //"text", 1,
                                 NULL);

  gtk_container_add (GTK_CONTAINER (priv->main_box), priv->modules_combobox);
  gtk_widget_set_sensitive (priv->modules_combobox, FALSE);

  g_signal_connect (priv->open_button,
                    "clicked",
                    G_CALLBACK (team_add_cb),
                    self);
  /* Load JSON on click */
  g_signal_connect (priv->dl_button,
                    "clicked",
                    G_CALLBACK (gtr_dl_teams_load_json),
                    self);
}

GtrDlTeams*
gtr_dl_teams_new (GtrWindow *window) {
  GtrDlTeams *self = g_object_new (GTR_TYPE_DL_TEAMS, NULL);
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);

  priv->main_window = window;
  return self;
}

GtkWidget *
gtr_dl_teams_get_header (GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  return priv->titlebar;
}

// static functions
static void
team_add_cb (GtkButton   *btn,
                GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  GtrWindow *window = GTR_WINDOW (priv->main_window);
  gtr_open_file_dialog (NULL, window);
}

