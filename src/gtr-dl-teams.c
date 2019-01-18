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
  GtkWidget *domains_combobox;
  GtkWidget *branches_combobox;

  GtkListStore *teams_store;
  GtkListStore *modules_store;
  GtkListStore *domains_store;
  GtkListStore *branches_store;

  gchar *selected_team;
  gchar *selected_module;
  gchar *selected_branch;
  gchar *selected_domain;

  GtrWindow *main_window;
} GtrDlTeamsPrivate;

struct _GtrDlTeams
{
  GtkBin parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrDlTeams, gtr_dl_teams, GTK_TYPE_BIN)

static void team_add_cb (GtkButton *btn, GtrDlTeams *self);
static void gtr_dl_teams_save_combo_selected (GtkComboBox *combo, GtrDlTeams *self);

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
          json_object_get_string_member (object, "name"),
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

  gtk_list_store_insert_with_values(data, NULL, -1,
        0, json_object_get_string_member (object, "name"),
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

      /* Enable selection */
      gtk_widget_set_sensitive (priv->teams_combobox, TRUE);
    }
}

static void
gtr_dl_teams_load_module_details_json (GtkComboBox *combo,
                      GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  SoupMessage *msg;
  SoupSession *session;
  gchar *module_endpoint;
  guint status;
  gint i;
  GError *error = NULL;
  JsonNode *node = NULL;

  /* Get module details JSON from DL API */
  module_endpoint = g_strconcat ((const gchar *)"https://l10n.gnome.org/api/v1/modules/", priv->selected_module, NULL);
  msg = soup_message_new ("GET", module_endpoint);
  session = soup_session_new ();
  status = soup_session_send_message (session, msg);

  g_autoptr(JsonParser) parser = json_parser_new ();

  /* Load response body and fill branches and domains */
  /* TODO: empty/remove existing combo boxes to avoid duplicating widgets */
  json_parser_load_from_data (parser, msg->response_body->data, msg->response_body->length, &error);
  node = json_parser_get_root (parser);

  JsonObject *object = json_node_get_object(node);

  /* branches */
  JsonNode *branchesNode = json_object_get_member (object, "branches");
  JsonArray *branchesArray = json_node_get_array (branchesNode);

  priv->branches_combobox = gtk_combo_box_text_new ();
  gtk_widget_set_name (priv->branches_combobox, "combo_branches");

  for (i=0; i < json_array_get_length (branchesArray); i++) {
    JsonNode *branch_element = json_array_get_element (branchesArray, i);
    JsonObject *branch_object = json_node_get_object (branch_element);
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (priv->branches_combobox),
                               json_object_get_string_member (branch_object, "name"),
                               json_object_get_string_member (branch_object, "name")
                               );
  }
  gtk_container_add (GTK_CONTAINER (priv->main_box), priv->branches_combobox);

  g_signal_connect (priv->branches_combobox,
                    "changed",
                    G_CALLBACK (gtr_dl_teams_save_combo_selected),
                    self);

  gtk_widget_show (priv->branches_combobox);

  /* domains */
  JsonNode *domainsNode = json_object_get_member (object, "domains");
  JsonArray *domains_array = json_node_get_array (domainsNode);

  priv->domains_combobox = gtk_combo_box_text_new ();
  gtk_widget_set_name (priv->domains_combobox, "combo_domains");

  for (i=0; i < json_array_get_length (domains_array); i++) {
    JsonNode *domain_element = json_array_get_element (domains_array, i);
    JsonObject *domain_object = json_node_get_object (domain_element);
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (priv->domains_combobox),
                               json_object_get_string_member (domain_object, "name"),
                               json_object_get_string_member (domain_object, "description")
                               );
  }
  gtk_container_add (GTK_CONTAINER (priv->main_box), priv->domains_combobox);

  g_signal_connect (priv->domains_combobox,
                    "changed",
                    G_CALLBACK (gtr_dl_teams_save_combo_selected),
                    self);

  gtk_widget_show (priv->domains_combobox);
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

      /* Load module details when module is selected */
      g_signal_connect (priv->modules_combobox,
                    "changed",
                    G_CALLBACK (gtr_dl_teams_load_module_details_json),
                    user_data);
    }
}

static void
gtr_dl_teams_load_json (GtkButton *btn,
                        GtrDlTeams *self)
{
  /* Get team list JSON from DL */
  g_autoptr(SoupSession) session = soup_session_new ();
  g_autoptr(SoupMessage) message = soup_message_new ("GET", "https://l10n.gnome.org/api/v1/teams");
  soup_session_send_async (session, message, NULL, gtr_dl_teams_parse_teams_json, self);

  /* Get module list JSON from DL */
  g_autoptr(SoupSession) session_modules = soup_session_new ();
  g_autoptr(SoupMessage) message_modules = soup_message_new ("GET", "https://l10n.gnome.org/api/v1/modules");
  soup_session_send_async (session_modules, message_modules, NULL, gtr_dl_teams_parse_modules_json, self);
}

void gtr_dl_teams_verify_and_load (GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  /*printf("%s, %s, %s, %s\n",
         priv->selected_team,
         priv->selected_module,
         priv->selected_branch,
         priv->selected_domain
         );*/
  if (priv->selected_team != NULL &&
      priv->selected_module != NULL &&
      priv->selected_branch != NULL &&
      priv->selected_domain != NULL)
    {
      // TODO: load PO file, get path from DL API first
      //printf("all good, PO can be loaded now\n");
    }
  else
    {
      //printf("not yet!\n");
    }
}

static void
gtr_dl_teams_save_combo_selected (GtkComboBox *combo,
                                  GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  GtkTreeIter iter;
  GtkTreePath *path;

  path = gtk_tree_path_new_from_indices (gtk_combo_box_get_active (combo), -1);

  /* Save selected combo option */
  const gchar *name = gtk_widget_get_name (GTK_WIDGET (combo));
  printf("%s\n", name);
  if (strcmp(name, "combo_modules") == 0)
    {
      gtk_tree_model_get_iter (GTK_TREE_MODEL (priv->modules_store), &iter, path);
      gtk_tree_model_get (GTK_TREE_MODEL (priv->modules_store), &iter, 0, &priv->selected_module, -1);
    }
  else if (strcmp(name, "combo_teams") == 0)
    {
      gtk_tree_model_get_iter (GTK_TREE_MODEL (priv->teams_store), &iter, path);
      gtk_tree_model_get (GTK_TREE_MODEL (priv->teams_store), &iter, 0, &priv->selected_team, -1);
    }
  else if (strcmp(name, "combo_branches") == 0)
    {
      priv->selected_branch = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT (combo));
    }
  else if (strcmp(name, "combo_domains") == 0)
    {
      priv->selected_domain = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT (combo));
    }

  /* Check if all four required values have been selected to proceed with loading PO file */
  gtr_dl_teams_verify_and_load (self);
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
  gtk_widget_set_name (priv->teams_combobox, "combo_teams");

  column = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(priv->teams_combobox), column, TRUE);

  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT(priv->teams_combobox), column,
                                 "text", 0,
                                 NULL);

  gtk_container_add (GTK_CONTAINER (priv->main_box), priv->teams_combobox);
  gtk_widget_set_sensitive (priv->teams_combobox, FALSE);

  priv->modules_combobox = gtk_combo_box_new ();
  gtk_widget_set_name (priv->modules_combobox, "combo_modules");

  column = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(priv->modules_combobox), column, TRUE);

  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT(priv->modules_combobox), column,
                                 "text", 0,
                                 NULL);

  gtk_container_add (GTK_CONTAINER (priv->main_box), priv->modules_combobox);
  gtk_widget_set_sensitive (priv->modules_combobox, FALSE);

  g_signal_connect (priv->open_button,
                    "clicked",
                    G_CALLBACK (team_add_cb),
                    self);

  /* Load teams and modules on click */
  g_signal_connect (priv->dl_button,
                    "clicked",
                    G_CALLBACK (gtr_dl_teams_load_json),
                    self);

  /* Connect "changed" to teams and modules combo boxes */
  g_signal_connect (priv->teams_combobox,
                    "changed",
                    G_CALLBACK (gtr_dl_teams_save_combo_selected),
                    self);
  g_signal_connect (priv->modules_combobox,
                    "changed",
                    G_CALLBACK (gtr_dl_teams_save_combo_selected),
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

