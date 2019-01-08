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

  GtrWindow *main_window;
} GtrDlTeamsPrivate;

struct _GtrDlTeams
{
  GtkBin parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrDlTeams, gtr_dl_teams, GTK_TYPE_BIN)

static void team_add_cb (GtkButton *btn, GtrDlTeams *self);

static GtkTreeModel *
create_combo_store (void)
{
  const gchar *labels[4] = {
    "Adding",
    "Items",
    "As I",
    "Go",
  };

  //GtkTreeIter iter;
  GtkListStore *store;
  gint i;

  store = gtk_list_store_new (1, G_TYPE_STRING);

  for (i = 0; i < G_N_ELEMENTS (labels); i++)
    {
      //printf( labels[i]);printf("\n");
      gtk_list_store_insert_with_values(store, NULL, -1,
        0, labels[i],
        -1);
      /*
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter,
                          0, "Test",
                          -1);*/
    }

  return GTK_TREE_MODEL (store);
}

static void
add_dl_teams_combo (GtkButton *btn,
                    GtrDlTeams *self)
{
  GtkWidget *combo_box;
  GtkTreeModel *model;
  GtkCellRenderer *column;

  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);

  combo_box = gtk_combo_box_new ();
  //model = create_combo_model_from_json (teams_json_array);
  model = create_combo_store ();
  //combo_box = gtk_combo_box_new_with_model (GTK_TREE_MODEL (model));
  gtk_combo_box_set_model (GTK_COMBO_BOX (combo_box), GTK_TREE_MODEL (model));
  g_object_unref (model);

  column = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo_box), column, TRUE);

  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo_box), column,
                                 "text", 0,
                                 //"text", 1,
                                 NULL);

  gtk_container_add (GTK_CONTAINER (priv->main_box), combo_box);
  gtk_widget_show (combo_box);

}

static void
element_cb (JsonArray *array,
            guint      index,
            JsonNode  *element,
            gpointer   data)
{
  JsonObject *object = json_node_get_object (element);
  /*printf ("%s: %s\n",
          json_object_get_string_member (object, "id"),
          json_object_get_string_member (object, "description")
          );
   * */
}

static void
load_and_parse (GObject *object, GAsyncResult *result, gpointer user_data)
{
  g_autoptr(JsonParser) parser = json_parser_new ();
  g_autoptr(GInputStream) stream;
  JsonNode *node = NULL;
  JsonArray *array = NULL;

  printf("load and parse\n");

  /* Parse JSON */
  stream = soup_session_send_finish (SOUP_SESSION (object), result, NULL);
  json_parser_load_from_stream (parser, stream, NULL, NULL);

  node = json_parser_get_root (parser);
  array = json_node_get_array (node);

  printf("array length: %d\n", json_array_get_length(array));

  // create combo and fill it with options from JSON
  //json_array_foreach_element (array, element_cb, NULL);

  //g_object_unref (session);
  //g_object_unref (message);
  //g_object_unref (parser);

}

static void
gtr_dl_teams_load_json ()
{
  /* Get team list JSON from DL */
  printf("get JSON\n");
  g_autoptr(SoupSession) session = soup_session_new ();
  g_autoptr(SoupMessage) message = soup_message_new ("GET", "https://l10n.gnome.org/teams/json");
  soup_session_send_async (session, message, NULL, load_and_parse, NULL);
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
  //GtkWidget *box, *combo, *entry;
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  gtk_widget_init_template (GTK_WIDGET (self));

  priv->main_window = NULL;

  g_signal_connect (priv->open_button,
                    "clicked",
                    G_CALLBACK (team_add_cb),
                    self);
  g_signal_connect (priv->dl_button,
                    "clicked",
                    G_CALLBACK (add_dl_teams_combo),
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

