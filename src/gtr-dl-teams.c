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
#include "gtr-filter-selection.h"
#include "gtr-window.h"
#include "gtr-utils.h"
#include "gtr-profile-manager.h"
#include <libsoup/soup.h>

#include <json-glib/json-glib.h>
#include <json-glib/json-gobject.h>

#include <glib/gi18n.h>


#define GTR_TYPE_DROP_DOWN_OPTION (gtr_drop_down_option_get_type ())
G_DECLARE_FINAL_TYPE (GtrDropDownOption, gtr_drop_down_option, GTR, DROP_DOWN_OPTION, GObject)

enum {
  PROP_0,
  PROP_NAME,
  PROP_DESCRIPTION,
  N_PROPERTIES
};

static GParamSpec *option_properties[N_PROPERTIES] = { NULL, };

struct _GtrDropDownOption {
  GObject parent_instance;
  char *name;
  char *description;
};

G_DEFINE_TYPE (GtrDropDownOption, gtr_drop_down_option, G_TYPE_OBJECT);

static void
gtr_drop_down_option_init (GtrDropDownOption *option)
{
  option->name = NULL;
  option->description = NULL;
}

static void
gtr_drop_down_option_finalize (GObject *object)
{
  GtrDropDownOption *option = GTR_DROP_DOWN_OPTION (object);

  g_free (option->name);
  g_free (option->description);

  G_OBJECT_CLASS (gtr_drop_down_option_parent_class)->finalize (object);
}

static void
gtr_drop_down_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  GtrDropDownOption *self = GTR_DROP_DOWN_OPTION (object);

  switch (property_id)
    {
    case PROP_NAME:
      if (self->name)
        g_free (self->name);
      self->name = g_value_dup_string (value);
      break;
    case PROP_DESCRIPTION:
      if (self->description)
        g_free (self->description);
      self->description = g_value_dup_string (value);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gtr_drop_down_get_property (GObject    *object,
                            guint       property_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  GtrDropDownOption *self = GTR_DROP_DOWN_OPTION (object);

  switch (property_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;
    case PROP_DESCRIPTION:
      g_value_set_string (value, self->description);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gtr_drop_down_option_class_init (GtrDropDownOptionClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->finalize = gtr_drop_down_option_finalize;
  object_class->set_property = gtr_drop_down_set_property;
  object_class->get_property = gtr_drop_down_get_property;

  option_properties[PROP_NAME] =
    g_param_spec_string ("name", "Name", "Name",
                      NULL,
                      G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

  option_properties[PROP_DESCRIPTION] =
    g_param_spec_string ("description", "Description", "Description",
                      NULL,
                      G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     option_properties);
}

static GtrDropDownOption *
gtr_drop_down_option_new (const char *name, const char *description)
{
  GtrDropDownOption *option = g_object_new (GTR_TYPE_DROP_DOWN_OPTION,
                                            "name", name,
                                            "description", description,
                                            NULL);
  return option;
}

static gboolean
gtr_drop_down_option_equal (GtrDropDownOption *opt1, GtrDropDownOption *opt2)
{
    if (strcmp (opt1->name, opt2->name))
        return FALSE;
    return TRUE;
}

typedef struct
{
  GtkWidget *titlebar;
  GtkWidget *main_box;
  GtkWidget *select_box;
  GtkWidget *open_button;
  GtkWidget *load_button;
  GtkWidget *reserve_button;
  GtkWidget *stats_label;
  GtkWidget *module_state_label;
  GtkWidget *file_label;
  GtkWidget *instructions;

  GtkWidget *teams_combobox;
  GListStore *teams_model;
  GtkWidget *modules_combobox;
  GtkWidget *domains_combobox;
  GtkWidget *branches_combobox;

  gchar *selected_team;
  gchar *selected_module;
  gchar *selected_branch;
  gchar *selected_domain;
  gchar *file_path;
  gchar *module_state;

  GtrWindow *main_window;
} GtrDlTeamsPrivate;

struct _GtrDlTeams
{
  AdwBin parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrDlTeams, gtr_dl_teams, ADW_TYPE_BIN)

static void team_add_cb (GtkButton *btn, GtrDlTeams *self);
static void gtr_dl_teams_save_combo_selected (GtkWidget *widget, GtrDlTeams *self);
static void gtr_dl_teams_load_po_file (GtkButton *button, GtrDlTeams *self);
static void gtr_dl_teams_get_file_info (GtrDlTeams *self);
static gboolean gtr_dl_teams_reserve_for_translation (GtkWidget *button, GtrDlTeams *self);

struct _StringObject {
  GObject parent_instance;
  const char * string;
};

static void
gtr_dl_teams_combobox_add (JsonArray *array,
                           int index,
                           JsonNode *element,
                           GtkDropDown *combo)
{
  JsonObject *object = json_node_get_object (element);
  GListStore *model = G_LIST_STORE (gtk_drop_down_get_model (GTK_DROP_DOWN (combo)));
  GtrDropDownOption *option = NULL;

  const char *name = json_object_get_string_member (object, "name");
  const char *desc = json_object_get_string_member (object, "description");

  option = gtr_drop_down_option_new (name, desc);
  g_list_store_append (model, option);
  g_object_unref (option);
}

static void
gtr_dl_modules_list_add (JsonArray *array,
                         guint      index,
                         JsonNode  *element,
                         gpointer   data)
{
  JsonObject *object = json_node_get_object (element);
  GSList **list = data;
  const char *name = json_object_get_string_member (object, "name");
  GtrFilterOption *opt = gtr_filter_option_new (name, name);

  *list = g_slist_append (*list, opt);
}

static void
gtr_dl_teams_parse_teams_json (GObject *object,
                               GAsyncResult *result,
                               gpointer user_data)
{
  g_autoptr(JsonParser) parser = json_parser_new ();
  g_autoptr(GInputStream) stream = NULL;
  g_autoptr(GtrDropDownOption) option = NULL;
  GtrProfileManager * pmanager = NULL;
  const char *def_lang = NULL;
  unsigned int def_lang_pos = 0;
  GtrProfile *profile = NULL;
  GError *error = NULL;
  JsonNode *node = NULL;
  JsonArray *array = NULL;

  GtrDlTeams *widget = GTR_DL_TEAMS (user_data);
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (widget);

  GtkWidget *dialog;

  /* Parse JSON */
  stream = soup_session_send_finish (SOUP_SESSION (object), result, &error);

  if (error)
    {
      dialog = gtk_message_dialog_new (GTK_WINDOW (priv->main_window),
                                             GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_WARNING,
                                             GTK_BUTTONS_CLOSE,
                                             "%s",
                                             error->message);
      g_signal_connect (dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
      gtk_window_present (GTK_WINDOW (dialog));
      return;
    }

  json_parser_load_from_stream (parser, stream, NULL, NULL);

  node = json_parser_get_root (parser);
  array = json_node_get_array (node);

  pmanager = gtr_profile_manager_get_default ();
  profile = gtr_profile_manager_get_active_profile (pmanager);
  def_lang = gtr_profile_get_language_code (profile);

  json_array_foreach_element (
    array,
    (JsonArrayForeach)gtr_dl_teams_combobox_add,
    priv->teams_combobox
  );

  option = gtr_drop_down_option_new (def_lang, NULL);
  g_list_store_find_with_equal_func (
    priv->teams_model,
    option,
    (GEqualFunc)gtr_drop_down_option_equal,
    &def_lang_pos
  );
  gtk_drop_down_set_selected (GTK_DROP_DOWN (priv->teams_combobox), def_lang_pos);

  /* Enable selection */
  gtk_widget_set_sensitive (priv->teams_combobox, TRUE);
  g_object_unref (pmanager);
}

static void
gtr_dl_teams_load_module_details_json (GtkWidget  *widget,
                                       GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  g_autoptr(SoupMessage) msg = NULL;
  g_autoptr(SoupSession) session = NULL;
  g_autofree gchar *module_endpoint;
  g_autoptr(JsonParser) parser = NULL;
  gint i;
  GError *error = NULL;
  JsonNode *node = NULL;
  JsonObject *object;
  JsonNode *branchesNode;
  JsonNode *domainsNode;
  GtkWidget *dialog;
  SoupStatus status_code;
  g_autoptr(GInputStream) stream = NULL;

  gtk_widget_hide (priv->file_label);
  gtk_widget_hide (priv->module_state_label);
  gtk_widget_show (priv->instructions);
  gtk_label_set_text (GTK_LABEL (priv->stats_label), "");

  /* Disable (down)load button */
  gtk_widget_set_sensitive (priv->branches_combobox, FALSE);
  gtk_widget_set_sensitive (priv->domains_combobox, FALSE);
  gtk_widget_set_sensitive (priv->load_button, FALSE);

  gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (priv->branches_combobox));
  gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (priv->domains_combobox));

  /* Get module details JSON from DL API */
  module_endpoint = g_strconcat ((const gchar *)API_URL, "modules/", priv->selected_module, NULL);
  msg = soup_message_new ("GET", module_endpoint);
  session = soup_session_new ();
  stream = soup_session_send (session, msg, NULL, &error);
  status_code = soup_message_get_status (msg);

  if (error || !SOUP_STATUS_IS_SUCCESSFUL (status_code))
    {
      g_autofree gchar *message = NULL;

      if (error)
        {
          message = error->message;
          g_clear_error (&error);
        }
      else
        {
          message = g_strdup (soup_message_get_reason_phrase (msg));
        }

      dialog = gtk_message_dialog_new (GTK_WINDOW (priv->main_window),
                                       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                       GTK_MESSAGE_WARNING,
                                       GTK_BUTTONS_CLOSE,
                                       "Error loading module info: %s",
                                       message);
      g_signal_connect (dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
      gtk_window_present (GTK_WINDOW (dialog));
      return;
    }

  parser = json_parser_new ();

  /* Load response body and fill branches and domains, then show widgets */
  json_parser_load_from_stream (parser, stream, NULL, &error);
  node = json_parser_get_root (parser);

  object = json_node_get_object(node);

  /* branches */
  branchesNode = json_object_get_member (object, "branches");

  if (branchesNode != NULL)
    {
      JsonArray *branchesArray = json_node_get_array (branchesNode);
      JsonNode *branch_element;
      JsonObject *branch_object;

      for (i=0; i < json_array_get_length (branchesArray); i++)
        {
          const char *name = NULL;
          branch_element = json_array_get_element (branchesArray, i);
          branch_object = json_node_get_object (branch_element);
          name = json_object_get_string_member (branch_object, "name"),

          gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (priv->branches_combobox),
                                     name, name);
        }
      gtk_combo_box_set_active (GTK_COMBO_BOX (priv->branches_combobox), 0);
      gtk_widget_set_sensitive (priv->branches_combobox, TRUE);
    }
  // TODO: check why there are no branches, display notification to user

  /* domains */
  domainsNode = json_object_get_member (object, "domains");

  if (domainsNode != NULL)
    {
      JsonArray *domains_array = json_node_get_array (domainsNode);
      JsonNode *domain_element;
      JsonObject *domain_object;

      for (i=0; i < json_array_get_length (domains_array); i++)
        {
          domain_element = json_array_get_element (domains_array, i);
          domain_object = json_node_get_object (domain_element);
          gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (priv->domains_combobox),
                                     json_object_get_string_member (domain_object, "name"),
                                     json_object_get_string_member (domain_object, "description"));
        }

      gtk_combo_box_set_active (GTK_COMBO_BOX (priv->domains_combobox), 0);
      gtk_widget_set_sensitive (priv->domains_combobox, TRUE);
    }
  // TODO: check why there are no domains and display notification to user
}

static void
gtr_dl_teams_parse_modules_json (GObject *object,
                                 GAsyncResult *result,
                                 gpointer user_data)
{
  g_autoptr(JsonParser) parser = json_parser_new ();
  g_autoptr(GInputStream) stream = NULL;
	GError *error = NULL;

  JsonNode *node = NULL;
  JsonArray *array = NULL;

  GtrDlTeams *widget = GTR_DL_TEAMS (user_data);
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (widget);

  GtkWidget *dialog;

  GSList *options = NULL;

  /* Parse JSON */
  stream = soup_session_send_finish (SOUP_SESSION (object), result, &error);
  if (error)
    {
      dialog = gtk_message_dialog_new (GTK_WINDOW (priv->main_window),
                                             GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_WARNING,
                                             GTK_BUTTONS_CLOSE,
                                             "%s",
                                             error->message);
      g_signal_connect (dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
      gtk_window_present (GTK_WINDOW (dialog));
      return;
    }

  json_parser_load_from_stream (parser, stream, NULL, NULL);

  node = json_parser_get_root (parser);
  array = json_node_get_array (node);

  /* Fill modules list store with values from JSON and set store as combo box model */
  json_array_foreach_element (array, gtr_dl_modules_list_add, &options);
  gtr_filter_selection_set_options_full (GTR_FILTER_SELECTION (priv->modules_combobox), options);

  gtk_widget_set_sensitive (priv->modules_combobox, TRUE);
}

static void
gtr_dl_teams_load_json (GtrDlTeams *self)
{
  /* Get team list JSON from DL */
  g_autoptr(SoupSession) session = soup_session_new ();
  g_autoptr(SoupMessage) message = soup_message_new ("GET", g_strconcat ((const gchar *)API_URL, "teams", NULL));
  soup_session_send_async (session, message, G_PRIORITY_DEFAULT, NULL, gtr_dl_teams_parse_teams_json, self);

  /* Get module list JSON from DL */
  g_autoptr(SoupSession) session_modules = soup_session_new ();
  g_autoptr(SoupMessage) message_modules = soup_message_new ("GET", g_strconcat ((const gchar *)API_URL, "modules", NULL));
  soup_session_send_async (session_modules, message_modules, G_PRIORITY_DEFAULT, NULL, gtr_dl_teams_parse_modules_json, self);
}

void gtr_dl_teams_verify_and_load (GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);

  if (priv->selected_team != NULL &&
      priv->selected_module != NULL &&
      priv->selected_branch != NULL &&
      priv->selected_domain != NULL)
    {
      gtk_widget_hide (priv->instructions);
      // get stats and path from DL API and enable (down)load button
      gtr_dl_teams_get_file_info (self);
    }
  else
    {
      gtk_widget_show (priv->instructions);
    }
}

static void
gtr_dl_teams_get_file_info (GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  gchar *stats_endpoint;
  JsonNode *node = NULL;
  g_autoptr(JsonParser) parser = NULL;
  JsonObject *object;
  SoupMessage *msg;
  SoupSession *session;
  GError *error = NULL;
  JsonNode *stats_node;
  JsonObject *stats_object;
  const char *format;
  char *markup;
  g_autofree char *module_state = NULL;
  GtkWidget *dialog;
  SoupStatus status_code;
  g_autoptr(GInputStream) stream = NULL;

  /* API endpoint: modules/[module]/branches/[branch]/domains/[domain]/languages/[team] */
  stats_endpoint = g_strconcat ((const gchar *)API_URL,
                                 "modules/",
                                 priv->selected_module,
                                 "/branches/",
                                 priv->selected_branch,
                                 "/domains/",
                                 priv->selected_domain,
                                 "/languages/",
                                 priv->selected_team,
                                 NULL);

  msg = soup_message_new ("GET", stats_endpoint);
  session = soup_session_new ();
  stream = soup_session_send (session, msg, NULL, &error);
  status_code = soup_message_get_status (msg);

  if (error || !SOUP_STATUS_IS_SUCCESSFUL (status_code))
    {
      g_autofree gchar *message = NULL;

      if (error)
        {
          message = error->message;
          g_clear_error (&error);
        }
      else
        {
          message = g_strdup (soup_message_get_reason_phrase (msg));
        }

      dialog = gtk_message_dialog_new (GTK_WINDOW (priv->main_window),
                                       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                       GTK_MESSAGE_WARNING,
                                       GTK_BUTTONS_CLOSE,
                                       "Error loading file info: %s",
                                       message);
      g_signal_connect (dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
      gtk_window_present (GTK_WINDOW (dialog));
      return;
    }

  parser = json_parser_new ();

  /* Load response body and get path to PO file */
  json_parser_load_from_stream (parser, stream, NULL, &error);
  node = json_parser_get_root (parser);

  object = json_node_get_object(node);

  /* Save file path; escape the string - slashes inside! */
  priv->file_path = g_strescape (json_object_get_string_member (object, "po_file"), "");

  if (!priv->file_path)
    {
      gtk_label_set_text (GTK_LABEL (priv->file_label), "No file found.");
      gtk_widget_show (priv->file_label);
      return;
    }

  if (priv->module_state)
    g_free (priv->module_state);
  priv->module_state = g_strdup (json_object_get_string_member (object, "state"));

  if (!priv->module_state)
    {
      gtk_label_set_text (GTK_LABEL (priv->module_state_label), _("No module state found."));
      gtk_widget_show (priv->module_state_label);
    }

  /* Get file statistics and show them to the user */
  stats_node = json_object_get_member (object, "statistics");
  stats_object = json_node_get_object (stats_node);

  format = "<span color=\"green\">\%d translated</span>, <span color=\"orange\">\%d fuzzy</span>, <span color=\"red\">\%d untranslated</span>";
  markup = g_markup_printf_escaped (format,
                                    json_object_get_int_member (stats_object, "trans"),
                                    json_object_get_int_member (stats_object, "fuzzy"),
                                    json_object_get_int_member (stats_object, "untrans"));

  gtk_label_set_markup (GTK_LABEL (priv->stats_label), markup);
  gtk_label_set_text (GTK_LABEL (priv->file_label), g_strconcat("File: ", strrchr (priv->file_path, '/') + 1, NULL));
  gtk_widget_show (priv->file_label);
  module_state = g_strdup_printf (_("The current state is: %s"), priv->module_state);
  gtk_label_set_text (GTK_LABEL (priv->module_state_label), module_state);
  gtk_widget_show (priv->module_state_label);
  /* Enable (down)load button */
  gtk_widget_set_sensitive (priv->load_button, TRUE);

  /* Enable the reserve button if a module's state is either None or Translated or ToReview */
  if (strcmp(priv->module_state, "None") == 0 ||
      strcmp(priv->module_state, "Translated") == 0 ||
      strcmp(priv->module_state, "ToReview") == 0)
    {
      gtk_widget_set_sensitive (priv->reserve_button, TRUE);
    }
  else
    {
      gtk_widget_set_sensitive (priv->reserve_button, FALSE);
    }

  g_free (markup);
}

static void
gtr_dl_teams_load_po_file (GtkButton *button, GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  SoupMessage *msg;
  SoupSession *session;
  GError *error = NULL;
  GFile *tmp_file = NULL;
  GFileIOStream *iostream = NULL;
  GOutputStream *output = NULL;
  GtkWidget *dialog;
  gboolean ret = FALSE;
  int file_index = 0;
  const char *dest_dir = g_get_user_special_dir (G_USER_DIRECTORY_DOWNLOAD);
  g_autofree char *basename = NULL;
  g_autofree char *file_path = NULL;
  g_autoptr(GFile) dest_file = NULL;
  gboolean reserve_first = FALSE;
  SoupStatus status_code;
  GBytes *bytes;

  // reserve for translation first
  reserve_first = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
                                                (priv->reserve_button));
  if (reserve_first)
    {
      ret = gtr_dl_teams_reserve_for_translation (priv->reserve_button, self);
      if (!ret)
        return;

      if (priv->module_state)
        g_free (priv->module_state);

      // The reserve was successful, so we should change the module_state to
      // "Translating"
      priv->module_state = g_strdup ("Translating");
    }

  /* Load the file, save as temp; path to file is https://l10n.gnome.org/[priv->file_path] */
  session = soup_session_new ();
  msg = soup_message_new ("GET", g_strconcat (DL_SERVER, g_strcompress(priv->file_path), NULL));
  bytes = soup_session_send_and_read (session, msg, NULL, &error);
  status_code = soup_message_get_status (msg);

  if (error || !SOUP_STATUS_IS_SUCCESSFUL (status_code))
    {
      g_autofree gchar *message = NULL;

      if (error)
        {
          message = error->message;
          g_clear_error (&error);
        }
      else
        {
          message = g_strdup (soup_message_get_reason_phrase (msg));
        }

      dialog = gtk_message_dialog_new (GTK_WINDOW (priv->main_window),
                                       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                       GTK_MESSAGE_WARNING,
                                       GTK_BUTTONS_CLOSE,
                                       "Error loading file: %s",
                                       message);
      g_signal_connect (dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
      gtk_window_present (GTK_WINDOW (dialog));
      return;
    }

  tmp_file = g_file_new_tmp (NULL, &iostream, &error);

  if (error != NULL)
    {
      dialog = gtk_message_dialog_new (GTK_WINDOW (priv->main_window),
                                       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                       GTK_MESSAGE_WARNING,
                                       GTK_BUTTONS_CLOSE,
                                       "Error creating temp file: %s",
                                       error->message);
      g_signal_connect (dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
      gtk_window_present (GTK_WINDOW (dialog));
      g_error_free (error);
      return;
    }

  output = g_io_stream_get_output_stream (G_IO_STREAM (iostream));
  g_output_stream_write_bytes (output,
                               bytes,
                               NULL,
                               &error);
  g_bytes_unref (bytes);

  if (error != NULL)
    {
      dialog = gtk_message_dialog_new (GTK_WINDOW (priv->main_window),
                                       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                       GTK_MESSAGE_WARNING,
                                       GTK_BUTTONS_CLOSE,
                                       "Error writing stream: %s",
                                       error->message);
      g_error_free (error);
      return;
    }

  /* Save file to Downloads; file basename is the part from last / character on */
  basename = g_path_get_basename (priv->file_path);
  // Remove the extension
  file_path = g_strconcat ("file://", dest_dir, "/", basename, NULL);
  dest_file = g_file_new_for_uri (file_path);

  ret = g_file_copy (tmp_file, dest_file, G_FILE_COPY_NONE, NULL, NULL, NULL, &error);
  while (!ret && g_error_matches (error, G_IO_ERROR, G_IO_ERROR_EXISTS))
    {
      g_autofree char *tmpname = gtr_utils_get_filename (basename);
      g_free (basename);
      g_free (file_path);
      g_object_unref (dest_file);
      g_clear_error (&error);

      basename = g_strdup_printf ("%s (%d).po", tmpname, ++file_index);
      file_path = g_strconcat ("file://", dest_dir, "/", basename, NULL);
      dest_file = g_file_new_for_uri (file_path);
      ret = g_file_copy (tmp_file, dest_file, G_FILE_COPY_NONE, NULL, NULL, NULL, &error);
    }

  if (error != NULL)
    {
      dialog = gtk_message_dialog_new (GTK_WINDOW (priv->main_window),
                                       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                       GTK_MESSAGE_WARNING,
                                       GTK_BUTTONS_CLOSE,
                                       "Error creating tmp file %s",
                                       error->message);
      g_error_free (error);
      return;
    }

  if (gtr_open (dest_file, priv->main_window, &error)) {
    GtrTab *tab = gtr_window_get_active_tab (priv->main_window);
    g_autofree char *info_msg = NULL;
    info_msg = g_strdup_printf (_("The file '%s' has been saved in %s"),
                                basename, dest_dir);
    gtr_tab_set_info (tab, info_msg, NULL);

    GtrPo *po = gtr_tab_get_po(tab);
    GError *po_error = NULL;
    gtr_po_set_dl_info(po,
                       priv->selected_team,
                       priv->selected_module,
                       priv->selected_branch,
                       priv->selected_domain,
                       priv->module_state);
    // Save to update the headers
    gtr_po_save_file(po, &po_error);
  }

  g_object_unref (tmp_file);
}

/* Reserve for translation */
static gboolean
gtr_dl_teams_reserve_for_translation (GtkWidget *button, GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  GtrProfileManager *pmanager = NULL;
  GtrProfile *profile = NULL;
  GtkWidget *dialog, *success_dialog;
  g_autoptr (SoupSession) session = NULL;
  g_autoptr (SoupMessage) msg = NULL;
  const char *auth_token = NULL;
  g_autofree char *auth = NULL;
  g_autofree gchar *reserve_endpoint = NULL;
  SoupStatus status_code;
  g_autoptr(GInputStream) stream = NULL;
  GError *error = NULL;

  pmanager = gtr_profile_manager_get_default ();
  profile = gtr_profile_manager_get_active_profile (pmanager);
  auth_token = gtr_profile_get_auth_token (profile);
  auth = g_strconcat ("Bearer ", auth_token, NULL);

  /* API endpoint: modules/[module]/branches/[branch]/domains/[domain]/languages/[team]/reserve */
  reserve_endpoint = g_strconcat ((const gchar *)API_URL,
                                  "modules/", priv->selected_module,
                                  "/branches/", priv->selected_branch,
                                  "/domains/", priv->selected_domain,
                                  "/languages/", priv->selected_team,
                                  "/reserve", NULL);

  msg = soup_message_new ("POST", reserve_endpoint);
  soup_message_set_flags (msg, SOUP_MESSAGE_NO_REDIRECT);
  soup_message_headers_append (soup_message_get_request_headers (msg),
                               "Authentication", auth);
  session = soup_session_new ();
  stream = soup_session_send (session, msg, NULL, &error);
  status_code = soup_message_get_status (msg);

  if (error || !SOUP_STATUS_IS_SUCCESSFUL (status_code))
  {
    g_autofree gchar *message = NULL;

    if (error)
      {
        message = error->message;
        g_clear_error (&error);
      }
    else
      {
        message = g_strdup (soup_message_get_reason_phrase (msg));
      }

    dialog = gtk_message_dialog_new_with_markup (
      GTK_WINDOW (priv->main_window),
      GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
      GTK_MESSAGE_WARNING,
      GTK_BUTTONS_CLOSE,
      _(
        "An error occurred while reserving this module: %s\n"
        "Maybe you've not configured your <i>l10n.gnome.org</i> "
        "<b>token</b> correctly in your profile or you don't have "
        "permissions to reserve this module."
      ),
      message);
    g_signal_connect (dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
    gtk_window_present (GTK_WINDOW (dialog));

    return FALSE;
  }

  /* Display a message if the reserve for translation operation was successful */
  success_dialog = gtk_message_dialog_new (GTK_WINDOW (priv->main_window),
                                           GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                           GTK_MESSAGE_INFO,
                                           GTK_BUTTONS_OK,
                                           _("The file '%s.%s.%s.%s' has been successfully reserved"),
                                           priv->selected_module,
                                           priv->selected_branch,
                                           priv->selected_team,
                                           priv->selected_domain);

  g_signal_connect (success_dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
  gtk_window_present (GTK_WINDOW (success_dialog));
  gtk_widget_set_sensitive (priv->reserve_button, FALSE);

  return TRUE;
}

static void
gtr_dl_teams_save_combo_selected (GtkWidget  *widget,
                                  GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  const gchar *name;

  /* Save selected combo option */
  name = gtk_widget_get_name (widget);

  if (strcmp(name, "combo_modules") == 0)
    {
      const GtrFilterOption *opt = NULL;
      if (priv->selected_module)
        g_free (priv->selected_module);
      opt = gtr_filter_selection_get_option (GTR_FILTER_SELECTION (priv->modules_combobox));
      priv->selected_module = g_strdup (opt->name);

      /* Reload module details on module change */
      gtr_dl_teams_load_module_details_json (widget, self);
    }
  else if (strcmp(name, "combo_teams") == 0)
    {
      const GtrDropDownOption *opt = GTR_DROP_DOWN_OPTION (
        gtk_drop_down_get_selected_item (GTK_DROP_DOWN (priv->teams_combobox))
      );
      if (priv->selected_team)
        g_free (priv->selected_team);
      priv->selected_team = g_strdup (opt->name);
    }
  else if (strcmp(name, "combo_branches") == 0)
    {
      if (priv->selected_branch)
        g_free (priv->selected_branch);
      priv->selected_branch = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT (widget));
    }
  else if (strcmp(name, "combo_domains") == 0)
    {
      if (priv->selected_domain)
        g_free (priv->selected_domain);
      priv->selected_domain = g_strdup (gtk_combo_box_get_active_id (GTK_COMBO_BOX (widget)));
    }

  /* Check if all four required values have been selected to proceed with loading PO file */
  gtr_dl_teams_verify_and_load (self);
}

static void
gtr_dl_teams_dispose (GObject *object)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (GTR_DL_TEAMS (object));

  if (priv->selected_team)
    {
      g_free (priv->selected_team);
      priv->selected_team = NULL;
    }
  if (priv->selected_module)
    {
      g_free (priv->selected_module);
      priv->selected_module = NULL;
    }
  if (priv->selected_branch)
    {
      g_free (priv->selected_branch);
      priv->selected_branch = NULL;
    }
  if (priv->selected_domain)
    {
      g_free (priv->selected_domain);
      priv->selected_domain = NULL;
    }
  if (priv->module_state)
    {
      g_free (priv->module_state);
      priv->module_state = NULL;
    }
  if (priv->file_path)
    {
      g_free (priv->file_path);
      priv->file_path = NULL;
    }

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
  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, select_box);
  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, file_label);
  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, stats_label);
  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, module_state_label);
  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, load_button);
  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, reserve_button);
  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, instructions);

  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, open_button);
}

static void
gtr_dl_teams_init (GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  gtk_widget_init_template (GTK_WIDGET (self));
  GtkExpression *expression = NULL;

  priv->main_window = NULL;
  priv->selected_team = NULL;
  priv->selected_module = NULL;
  priv->selected_branch = NULL;
  priv->selected_domain = NULL;
  priv->file_path = NULL;
  priv->module_state = NULL;

  gtk_widget_set_sensitive (priv->load_button, FALSE);
  gtk_widget_set_sensitive (priv->reserve_button, FALSE);

  /* Add combo boxes for DL teams and modules */
  expression = gtk_property_expression_new (GTR_TYPE_DROP_DOWN_OPTION, NULL, "description");
  priv->teams_model = g_list_store_new (GTR_TYPE_DROP_DOWN_OPTION);
  priv->teams_combobox = GTK_WIDGET (
    gtk_drop_down_new (G_LIST_MODEL (priv->teams_model), expression)
  );
  gtk_widget_set_name (priv->teams_combobox, "combo_teams");
  gtk_drop_down_set_enable_search (GTK_DROP_DOWN (priv->teams_combobox), TRUE);

  gtk_box_append (GTK_BOX (priv->select_box), priv->teams_combobox);
  gtk_widget_set_sensitive (priv->teams_combobox, FALSE);

  priv->modules_combobox = GTK_WIDGET (gtr_filter_selection_new ());
  gtk_widget_set_name (priv->modules_combobox, "combo_modules");
  gtr_filter_selection_set_text (GTR_FILTER_SELECTION (priv->modules_combobox), _("Module"));

  gtk_box_append (GTK_BOX (priv->select_box), priv->modules_combobox);
  gtk_widget_set_sensitive (priv->modules_combobox, FALSE);

  g_signal_connect (priv->open_button,
                    "clicked",
                    G_CALLBACK (team_add_cb),
                    self);

  /* Add empty combo boxes for DL domains and branches and hide them */
  priv->domains_combobox = gtk_combo_box_text_new ();
  gtk_widget_set_name (priv->domains_combobox, "combo_domains");
  gtk_box_append (GTK_BOX (priv->select_box), priv->domains_combobox);

  priv->branches_combobox = gtk_combo_box_text_new ();
  gtk_widget_set_name (priv->branches_combobox, "combo_branches");
  gtk_box_append (GTK_BOX (priv->select_box), priv->branches_combobox);

  /* Load teams and modules automatically */
  gtr_dl_teams_load_json (self);

  /* Connect "changed" to all combo boxes */
  g_signal_connect (priv->teams_combobox,
                    "changed",
                    G_CALLBACK (gtr_dl_teams_save_combo_selected),
                    self);
  g_signal_connect (priv->modules_combobox,
                    "changed",
                    G_CALLBACK (gtr_dl_teams_save_combo_selected),
                    self);
  g_signal_connect (priv->domains_combobox,
                    "changed",
                    G_CALLBACK (gtr_dl_teams_save_combo_selected),
                    self);
  g_signal_connect (priv->branches_combobox,
                    "changed",
                    G_CALLBACK (gtr_dl_teams_save_combo_selected),
                    self);

  /* Connect "click" on file button to load PO file */
  g_signal_connect (priv->load_button,
                    "clicked",
                    G_CALLBACK (gtr_dl_teams_load_po_file),
                    self);
}

GtrDlTeams*
gtr_dl_teams_new (GtrWindow *window)
{
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
  gtr_open_file_dialog (window);
}


