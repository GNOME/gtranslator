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
#include "gtr-profile-manager.h"
#include "gtr-drop-down-option.h"
#include <libsoup/soup.h>

#include <json-glib/json-glib.h>
#include <json-glib/json-gobject.h>

#include <glib/gi18n.h>

/* From https://gnome.pages.gitlab.gnome.org/libadwaita/doc/main/css-variables.html */
#define GTR_ERROR_DARK "#ff938c"
#define GTR_WARNING_DARK "#ffc252"
#define GTR_SUCCESS_DARK "#78e9ab"
#define GTR_ERROR_LIGHT "#c30000"
#define GTR_WARNING_LIGHT "#905400"
#define GTR_SUCCESS_LIGHT "#007c3d"

typedef struct
{
  GtkWidget *load_button;
  GtkWidget *reserve_button;
  GtkWidget *stats_row;
  GtkWidget *module_state_row;
  GtkWidget *file_row;

  GtkWidget *langs_comborow;
  GListStore *langs_model;
  GtkWidget *modules_comborow;
  GListStore *modules_model;
  GtkWidget *domains_comborow;
  GtkWidget *branches_comborow;

  GtkStringList *branches_model;
  GListStore *domains_model;

  gchar *selected_lang;
  gchar *selected_module;
  gchar *selected_branch;
  gchar *selected_domain;
  gchar *file_path;
  gchar *module_state;
  gchar *vcs_web;

  GtrWindow *main_window;

  SoupSession *soup_session;
} GtrDlTeamsPrivate;

struct _GtrDlTeams
{
  AdwNavigationPage parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrDlTeams, gtr_dl_teams, ADW_TYPE_NAVIGATION_PAGE)

typedef struct
{
  char *name;
  char *description;
} GtrDlTeamsDomainPrivate;

struct _GtrDlTeamsDomain
{
  GObject parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrDlTeamsDomain, gtr_dl_teams_domain, G_TYPE_OBJECT)

static void gtr_dl_teams_load_po_file (GtkButton *button, GtrDlTeams *self);
static void gtr_dl_teams_get_file_info (GtrDlTeams *self);
static void gtr_dl_teams_reserve_for_translation (GtrDlTeams *self);

struct _StringObject {
  GObject parent_instance;
  const char * string;
};

static void
gtr_dl_teams_combobox_add (JsonArray   *array,
                           int          index,
                           JsonNode    *element,
                           AdwComboRow *combo)
{
  JsonObject *object = json_node_get_object (element);
  GListStore *model = G_LIST_STORE (adw_combo_row_get_model (ADW_COMBO_ROW (combo)));
  GtrDropDownOption *option = NULL;

  const char *name = json_object_get_string_member (object, "locale");
  const char *desc = json_object_get_string_member (object, "name");

  option = gtr_drop_down_option_new (name, desc);
  g_list_store_append (model, option);
  g_object_unref (option);
}

static void
gtr_dl_modules_combobox_add (JsonArray   *array,
                             int          index,
                             JsonNode    *element,
                             AdwComboRow *combo)
{
  JsonObject *object = json_node_get_object (element);
  GListStore *model = G_LIST_STORE (adw_combo_row_get_model (ADW_COMBO_ROW (combo)));
  GtrDropDownOption *option = NULL;

  const char *name = json_object_get_string_member (object, "name");
  option = gtr_drop_down_option_new (name, NULL);
  g_list_store_append (model, option);
  g_object_unref (option);
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
  g_autoptr (GError) error = NULL;
  JsonNode *node = NULL;
  JsonArray *array = NULL;

  GtrDlTeams *widget = GTR_DL_TEAMS (user_data);
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (widget);

  AdwDialog *dialog;

  /* Parse JSON */
  stream = soup_session_send_finish (SOUP_SESSION (object), result, &error);

  if (error)
    {
      dialog = adw_alert_dialog_new (NULL, error->message);
      adw_alert_dialog_add_response (ADW_ALERT_DIALOG (dialog), "ok", _("OK"));
      adw_alert_dialog_set_default_response (ADW_ALERT_DIALOG (dialog), "ok");
      adw_dialog_present (ADW_DIALOG (dialog), GTK_WIDGET (priv->main_window));
      return;
    }

  json_parser_load_from_stream (parser, stream, NULL, NULL);

  node = json_parser_get_root (parser);
  array = json_node_get_array (node);

  pmanager = gtr_profile_manager_get_default ();
  profile = gtr_profile_manager_get_active_profile (pmanager);
  if (profile)
    def_lang = gtr_profile_get_language_code (profile);

  json_array_foreach_element (
    array,
    (JsonArrayForeach)gtr_dl_teams_combobox_add,
    priv->langs_comborow
  );

  option = gtr_drop_down_option_new (def_lang, NULL);
  g_list_store_find_with_equal_func (
    priv->langs_model,
    option,
    (GEqualFunc)gtr_drop_down_option_equal,
    &def_lang_pos
  );
  adw_combo_row_set_selected (ADW_COMBO_ROW (priv->langs_comborow), def_lang_pos);

  /* Enable selection */
  gtk_widget_set_sensitive (priv->langs_comborow, TRUE);
  g_object_unref (pmanager);
}

static void
gtr_dl_teams_parse_module_details (GObject *object, GAsyncResult *result, gpointer user_data)
{
  GtrDlTeams *self = GTR_DL_TEAMS (user_data);
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  gint i;
  JsonNode *node = NULL;
  JsonObject *jobject;
  JsonNode *branchesNode;
  JsonNode *domainsNode;
  JsonNode *vcsWebNode;
  AdwDialog *dialog;
  SoupStatus status_code;
  SoupMessage *msg = NULL;

  g_autoptr (GError) error = NULL;
  g_autoptr(JsonParser) parser = NULL;
  g_autoptr(GInputStream) stream = NULL;

  stream = soup_session_send_finish (SOUP_SESSION (object), result, &error);
  msg = soup_session_get_async_result_message (SOUP_SESSION (object), result);
  status_code = soup_message_get_status (msg);
  if (error || !SOUP_STATUS_IS_SUCCESSFUL (status_code))
    {
      const char *m = error ? error->message : soup_message_get_reason_phrase (msg);
      dialog = adw_alert_dialog_new (_("Error loading module info"), m);
      adw_alert_dialog_add_response (ADW_ALERT_DIALOG (dialog), "ok", _("OK"));
      adw_alert_dialog_set_default_response (ADW_ALERT_DIALOG (dialog), "ok");
      adw_dialog_present (dialog, GTK_WIDGET (priv->main_window));
      return;
    }

  gtk_string_list_splice (
    priv->branches_model,
    0,
    g_list_model_get_n_items (G_LIST_MODEL (priv->branches_model)),
    NULL
  );
  g_list_store_remove_all (priv->domains_model);
  parser = json_parser_new ();

  /* Load response body and fill branches and domains, then show widgets */
  json_parser_load_from_stream (parser, stream, NULL, &error);
  node = json_parser_get_root (parser);
  jobject = json_node_get_object (node);

  vcsWebNode = json_object_get_member (jobject, "vcs_web");
  if (vcsWebNode)
    g_set_str (&priv->vcs_web, json_node_get_string (vcsWebNode));

  /* branches */
  branchesNode = json_object_get_member (jobject, "branches");

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
          gtk_string_list_append (priv->branches_model, name);
        }
      adw_combo_row_set_selected (ADW_COMBO_ROW (priv->branches_comborow), 0);
      gtk_widget_set_sensitive (priv->branches_comborow, TRUE);
    }
  // TODO: check why there are no branches, display notification to user

  /* domains */
  domainsNode = json_object_get_member (jobject, "domains");

  if (domainsNode != NULL)
    {
      JsonArray *domains_array = json_node_get_array (domainsNode);
      JsonNode *domain_element;
      JsonObject *domain_object;

      for (i=0; i < json_array_get_length (domains_array); i++)
        {
          g_autoptr(GtrDlTeamsDomain) teams_domain = NULL;

          domain_element = json_array_get_element (domains_array, i);
          domain_object = json_node_get_object (domain_element);
          teams_domain = gtr_dl_teams_domain_new (
            json_object_get_string_member (domain_object, "name"),
            json_object_get_string_member (domain_object, "description")
          );

          g_list_store_append (priv->domains_model, teams_domain);
        }

      adw_combo_row_set_selected (ADW_COMBO_ROW (priv->domains_comborow), 0);
      gtk_widget_set_sensitive (priv->domains_comborow, TRUE);
    }
  // TODO: check why there are no domains and display notification to user
}

static void
gtr_dl_teams_load_module_details_json (GtkWidget  *widget,
                                       GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);

  g_autoptr(SoupMessage) msg = NULL;
  g_autofree gchar *module_endpoint = NULL;

  adw_action_row_set_subtitle (ADW_ACTION_ROW (priv->stats_row), "");

  /* Disable (down)load button */
  gtk_widget_set_sensitive (priv->branches_comborow, FALSE);
  gtk_widget_set_sensitive (priv->domains_comborow, FALSE);
  gtk_widget_set_sensitive (priv->load_button, FALSE);

  /* Get module details JSON from DL API */
  module_endpoint = g_strconcat ((const gchar *)API_URL, "modules/", priv->selected_module, NULL);
  msg = soup_message_new ("GET", module_endpoint);
  soup_session_send_async (priv->soup_session, msg, G_PRIORITY_DEFAULT, NULL, gtr_dl_teams_parse_module_details, self);
}

static void
gtr_dl_teams_parse_modules_json (GObject *object,
                                 GAsyncResult *result,
                                 gpointer user_data)
{
  g_autoptr(JsonParser) parser = json_parser_new ();
  g_autoptr(GInputStream) stream = NULL;
  g_autoptr (GError) error = NULL;

  JsonNode *node = NULL;
  JsonArray *array = NULL;

  GtrDlTeams *widget = GTR_DL_TEAMS (user_data);
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (widget);

  AdwDialog *dialog;

  /* Parse JSON */
  stream = soup_session_send_finish (SOUP_SESSION (object), result, &error);
  if (error)
    {
      dialog = adw_alert_dialog_new (NULL, error->message);
      adw_alert_dialog_add_response (ADW_ALERT_DIALOG (dialog), "ok", _("OK"));
      adw_alert_dialog_set_default_response (ADW_ALERT_DIALOG (dialog), "ok");
      adw_dialog_present (ADW_DIALOG (dialog), GTK_WIDGET (priv->main_window));
      return;
    }

  json_parser_load_from_stream (parser, stream, NULL, NULL);

  node = json_parser_get_root (parser);
  array = json_node_get_array (node);

  json_array_foreach_element (
    array,
    (JsonArrayForeach)gtr_dl_modules_combobox_add,
    priv->modules_comborow
  );

  gtk_widget_set_sensitive (priv->modules_comborow, TRUE);
}

void
gtr_dl_teams_load_json (GtrDlTeams *self)
{
  /* Get team list JSON from DL */
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  SoupMessage *message = NULL;
  char *url = NULL;

  url = g_strconcat ((const gchar *)API_URL, "languages", NULL);
  message = soup_message_new ("GET", url);
  soup_session_send_async (priv->soup_session, message, G_PRIORITY_DEFAULT, NULL, gtr_dl_teams_parse_teams_json, self);


  g_object_unref (message);
  g_free (url);

  /* Get module list JSON from DL */
  url = g_strconcat ((const gchar *)API_URL, "modules", NULL);
  message = soup_message_new ("GET", url);
  soup_session_send_async (priv->soup_session, message, G_PRIORITY_DEFAULT, NULL, gtr_dl_teams_parse_modules_json, self);

  g_object_unref (message);
  g_free (url);
}

static void gtr_dl_teams_verify_and_load (GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);

  if (priv->selected_lang != NULL &&
      priv->selected_module != NULL &&
      priv->selected_branch != NULL &&
      priv->selected_domain != NULL)
    {
      // get stats and path from DL API and enable (down)load button
      gtr_dl_teams_get_file_info (self);
    }
}

static void
gtr_dl_teams_parse_file_info (GObject *object, GAsyncResult *result, gpointer user_data)
{
  GtrDlTeams *self = GTR_DL_TEAMS (user_data);
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  JsonNode *node = NULL;
  JsonObject *jobject;
  JsonNode *stats_node;
  JsonObject *stats_object;
  char *markup;
  AdwDialog *dialog;
  SoupStatus status_code;
  SoupMessage *msg = NULL;
  AdwStyleManager *style_manager;
  const char *error_color;
  const char *success_color;
  const char *warning_color;

  g_autoptr(JsonParser) parser = NULL;
  g_autoptr (GError) error = NULL;
  g_autoptr(GInputStream) stream = NULL;

  stream = soup_session_send_finish (SOUP_SESSION (object), result, &error);
  msg = soup_session_get_async_result_message (SOUP_SESSION (object), result);
  status_code = soup_message_get_status (msg);
  if (error || !SOUP_STATUS_IS_SUCCESSFUL (status_code))
    {
      const char *m = error ? error->message : soup_message_get_reason_phrase (msg);
      dialog = adw_alert_dialog_new (_("Error loading file info"), m);
      adw_alert_dialog_add_response (ADW_ALERT_DIALOG (dialog), "ok", _("OK"));
      adw_alert_dialog_set_default_response (ADW_ALERT_DIALOG (dialog), "ok");
      adw_dialog_present (dialog, GTK_WIDGET (priv->main_window));
      return;
    }

  parser = json_parser_new ();
  /* Load response body and get path to PO file */
  json_parser_load_from_stream (parser, stream, NULL, &error);
  node = json_parser_get_root (parser);
  jobject = json_node_get_object(node);

  /* Save file path; escape the string - slashes inside! */
  g_clear_pointer (&priv->file_path, g_free);
  priv->file_path = g_strescape (json_object_get_string_member (jobject, "po_file"), "");

  if (!priv->file_path)
    {
      adw_action_row_set_subtitle (ADW_ACTION_ROW (priv->file_row), _("No file found"));
      return;
    }

  if (priv->module_state)
    g_free (priv->module_state);
  priv->module_state = g_strdup (json_object_get_string_member (jobject, "state"));

  if (!priv->module_state)
    adw_action_row_set_subtitle (ADW_ACTION_ROW (priv->module_state_row), _("No module state found."));

  /* Get file statistics and show them to the user */
  stats_node = json_object_get_member (jobject, "statistics");
  stats_object = json_node_get_object (stats_node);

  style_manager = adw_style_manager_get_default ();
  if (adw_style_manager_get_dark (style_manager))
    {
      error_color = GTR_ERROR_DARK;
      success_color = GTR_SUCCESS_DARK;
      warning_color = GTR_WARNING_DARK;
    }
  else
    {
      error_color = GTR_ERROR_LIGHT;
      success_color = GTR_SUCCESS_LIGHT;
      warning_color = GTR_WARNING_LIGHT;
    }

  markup = g_markup_printf_escaped (
    "<span color=\"%s\">\%ld translated</span>, <span color=\"%s\">\%ld fuzzy</span>, <span color=\"%s\">\%ld untranslated</span>",
    success_color,
    json_object_get_int_member (stats_object, "trans"),
    warning_color,
    json_object_get_int_member (stats_object, "fuzzy"),
    error_color,
    json_object_get_int_member (stats_object, "untrans"));

  adw_action_row_set_subtitle (ADW_ACTION_ROW (priv->stats_row), markup);
  adw_action_row_set_subtitle (ADW_ACTION_ROW (priv->file_row), strrchr (priv->file_path, '/') + 1);
  adw_action_row_set_subtitle (ADW_ACTION_ROW (priv->module_state_row), priv->module_state);
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
gtr_dl_teams_get_file_info (GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  g_autofree char *stats_endpoint = NULL;
  g_autoptr(SoupMessage) msg = NULL;

  /* API endpoint: modules/[module]/branches/[branch]/domains/[domain]/languages/[team] */
  stats_endpoint = g_strconcat ((const gchar *)API_URL,
                                 "modules/",
                                 priv->selected_module,
                                 "/branches/",
                                 priv->selected_branch,
                                 "/domains/",
                                 priv->selected_domain,
                                 "/languages/",
                                 priv->selected_lang,
                                 NULL);

  msg = soup_message_new ("GET", stats_endpoint);
  soup_session_send_async (priv->soup_session,
                           msg,
                           G_PRIORITY_DEFAULT,
                           NULL,
                           gtr_dl_teams_parse_file_info,
                           self);
}

static void
gtr_dl_teams_download_file_done (GObject *object, GAsyncResult *result, gpointer user_data)
{
  GtrDlTeams *self = GTR_DL_TEAMS (user_data);
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);

  int file_index = 0;
  const char *dest_dir = g_get_user_special_dir (G_USER_DIRECTORY_DOWNLOAD);
  gboolean ret = FALSE;

  AdwDialog *dialog;

  GOutputStream *output = NULL;
  SoupMessage *msg = NULL;
  SoupStatus status_code;

  g_autoptr (GFile) tmp_file = NULL;
  g_autoptr(GFile) dest_file = NULL;
  g_autoptr (GError) error = NULL;
  g_autoptr (GBytes) bytes = NULL;
  g_autoptr (GFileIOStream) iostream = NULL;
  g_autofree char *basename = NULL;
  g_autofree char *og_basename = NULL;
  g_autofree char *file_path = NULL;

  bytes = soup_session_send_and_read_finish (SOUP_SESSION (object), result, &error);
  msg = soup_session_get_async_result_message (SOUP_SESSION (object), result);
  status_code = soup_message_get_status (msg);
  if (error || !SOUP_STATUS_IS_SUCCESSFUL (status_code))
    {
      const char *m = error ? error->message : soup_message_get_reason_phrase (msg);
      dialog = adw_alert_dialog_new (_("Error loading file"), m);
      adw_alert_dialog_add_response (ADW_ALERT_DIALOG (dialog), "ok", _("OK"));
      adw_alert_dialog_set_default_response (ADW_ALERT_DIALOG (dialog), "ok");
      adw_dialog_present (dialog, GTK_WIDGET (priv->main_window));
      return;
    }

  tmp_file = g_file_new_tmp (NULL, &iostream, &error);
  if (error != NULL)
    {
      dialog = adw_alert_dialog_new (_("Error creating tmp file"),
                                     error->message);
      adw_alert_dialog_add_response (ADW_ALERT_DIALOG (dialog), "ok", _("OK"));
      adw_alert_dialog_set_default_response (ADW_ALERT_DIALOG (dialog), "ok");
      adw_dialog_present (dialog, GTK_WIDGET (priv->main_window));
      return;
    }

  output = g_io_stream_get_output_stream (G_IO_STREAM (iostream));
  g_output_stream_write_bytes (output, bytes, NULL, &error);
  if (error != NULL)
    {
      dialog = adw_alert_dialog_new (_("Error writing stream"), error->message);
      adw_alert_dialog_add_response (ADW_ALERT_DIALOG (dialog), "ok", _("OK"));
      adw_alert_dialog_set_default_response (ADW_ALERT_DIALOG (dialog), "ok");
      adw_dialog_present (dialog, GTK_WIDGET (priv->main_window));
      return;
    }

  /* Save file to Downloads; file basename is the part from last / character on */
  basename = g_path_get_basename (priv->file_path);
  og_basename = g_strdup (basename);
  // Remove the extension
  file_path = g_strconcat ("file://", dest_dir, "/", basename, NULL);
  dest_file = g_file_new_for_uri (file_path);

  ret = g_file_copy (tmp_file, dest_file, G_FILE_COPY_NONE, NULL, NULL, NULL, &error);
  while (!ret && g_error_matches (error, G_IO_ERROR, G_IO_ERROR_EXISTS))
    {
      g_autofree char *tmpname = gtr_utils_get_filename (og_basename);
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
      dialog = adw_alert_dialog_new (_("Error creating tmp file"), error->message);
      adw_alert_dialog_add_response (ADW_ALERT_DIALOG (dialog), "ok", _("OK"));
      adw_alert_dialog_set_default_response (ADW_ALERT_DIALOG (dialog), "ok");
      adw_dialog_present (dialog, GTK_WIDGET (priv->main_window));
      return;
    }

  g_autoptr(GtrPo) po = NULL;
  po = gtr_po_new_from_file (dest_file, &error);
  if (error)
    {
      dialog = adw_alert_dialog_new (NULL, error->message);
      adw_alert_dialog_add_response (ADW_ALERT_DIALOG (dialog), "ok", _("OK"));
      adw_alert_dialog_set_default_response (ADW_ALERT_DIALOG (dialog), "ok");
      adw_dialog_present (dialog, GTK_WIDGET (priv->main_window));
      return;
    }
  else
    {
      GtrTab *tab;
      g_autoptr (GError) po_error = NULL;

      gtr_po_set_dl_info (po, priv->selected_lang, priv->selected_module,
                          priv->selected_branch, priv->selected_domain,
                          priv->module_state, priv->vcs_web);
      // Save to update the headers
      gtr_po_save_file (po, &po_error);
      if (po_error)
        g_error ("Could not save po file %s", po_error->message);

      gtr_window_set_po (priv->main_window, po);
      tab = gtr_window_get_active_tab (priv->main_window);
      gtr_tab_set_info (tab, _("The file has been saved in your Downloads folder"), NULL);
    }
}

static void
gtr_dl_teams_download_file (GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  g_autoptr(SoupMessage) msg = NULL;
  g_autofree gchar *api_endpoint = NULL;
  g_autofree gchar *escaped_file_path = NULL;

  /* Load the file, save as temp; path to file is https://l10n.gnome.org/[priv->file_path] */
  escaped_file_path = g_strcompress (priv->file_path);
  api_endpoint = g_strconcat (DL_SERVER, escaped_file_path, NULL);
  msg = soup_message_new ("GET", api_endpoint);
  soup_session_send_and_read_async (priv->soup_session, msg, G_PRIORITY_DEFAULT, NULL,
                                    (GAsyncReadyCallback)gtr_dl_teams_download_file_done,
                                    self);
}

static void
gtr_dl_teams_load_po_file (GtkButton *button, GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  gboolean reserve_first = FALSE;

  // reserve for translation first
  reserve_first = adw_switch_row_get_active (ADW_SWITCH_ROW (priv->reserve_button));
  if (reserve_first)
    gtr_dl_teams_reserve_for_translation (self);
  else
    gtr_dl_teams_download_file (self);
}

static void
gtr_dl_teams_reserve_for_translation_done (GObject *object, GAsyncResult *result, gpointer user_data)
{
  GtrDlTeams *self = GTR_DL_TEAMS (user_data);
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  AdwDialog *dialog;
  SoupStatus status_code;
  SoupMessage *msg = NULL;

  g_autoptr (GError) error = NULL;

  soup_session_send_finish (SOUP_SESSION (object), result, &error);
  msg = soup_session_get_async_result_message (SOUP_SESSION (object), result);
  status_code = soup_message_get_status (msg);
  if (error || !SOUP_STATUS_IS_SUCCESSFUL (status_code))
    {
      dialog = adw_alert_dialog_new (_("Could not reserve module"), NULL);
      adw_alert_dialog_add_response (ADW_ALERT_DIALOG (dialog), "ok", _("OK"));
      adw_alert_dialog_set_default_response (ADW_ALERT_DIALOG (dialog), "ok");
      adw_alert_dialog_format_body (ADW_ALERT_DIALOG (dialog),
        _(
          "%s\n"
          "Maybe you've not configured your l10n.gnome.org "
          "token correctly in your profile or you don't have "
          "permissions to reserve this module."
        ),
        error ? error->message : soup_message_get_reason_phrase (msg));
      adw_dialog_present (dialog, GTK_WIDGET (priv->main_window));
      return;
   }

  gtk_widget_set_sensitive (priv->reserve_button, FALSE);
  gtr_window_add_toast_msg (GTR_WINDOW (priv->main_window),
                            _("The file has been successfully reserved"));

  // The reserve was successful, so we should change the module_state to
  // "Translating"
  if (priv->module_state)
    g_free (priv->module_state);
  priv->module_state = g_strdup ("Translating");

  // After reserve we always download
  gtr_dl_teams_download_file (self);
}

/* Reserve for translation */
static void
gtr_dl_teams_reserve_for_translation (GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  GtrProfileManager *pmanager = NULL;
  GtrProfile *profile = NULL;
  const char *auth_token = NULL;

  g_autoptr (SoupMessage) msg = NULL;
  g_autofree char *auth = NULL;
  g_autofree gchar *reserve_endpoint = NULL;

  pmanager = gtr_profile_manager_get_default ();
  profile = gtr_profile_manager_get_active_profile (pmanager);
  auth_token = gtr_profile_get_auth_token (profile);
  auth = g_strconcat ("Bearer ", auth_token, NULL);

  /* API endpoint: modules/[module]/branches/[branch]/domains/[domain]/languages/[team]/reserve */
  reserve_endpoint = g_strconcat ((const gchar *)API_URL,
                                  "modules/", priv->selected_module,
                                  "/branches/", priv->selected_branch,
                                  "/domains/", priv->selected_domain,
                                  "/languages/", priv->selected_lang,
                                  "/reserve", NULL);

  msg = soup_message_new ("POST", reserve_endpoint);
  soup_message_set_flags (msg, SOUP_MESSAGE_NO_REDIRECT);
  soup_message_headers_append (soup_message_get_request_headers (msg),
                               "Authentication", auth);
  soup_session_send_async (priv->soup_session, msg, G_PRIORITY_DEFAULT, NULL, gtr_dl_teams_reserve_for_translation_done, self);
}

static void
gtr_dl_teams_langs_combo_selected_notify (GtkWidget  *widget,
                                          GParamSpec *spec,
                                          GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  GtrDropDownOption *opt = GTR_DROP_DOWN_OPTION (
    adw_combo_row_get_selected_item (ADW_COMBO_ROW (priv->langs_comborow))
  );
  if (priv->selected_lang)
    g_free (priv->selected_lang);
  if (opt)
    priv->selected_lang = g_strdup (gtr_drop_down_option_get_name (opt));
  else
    priv->selected_lang = NULL;

  gtr_dl_teams_verify_and_load (self);
}

static void
gtr_dl_teams_modules_combo_selected_notify (GtkWidget  *widget,
                                            GParamSpec *spec,
                                            GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  GtrDropDownOption *opt = GTR_DROP_DOWN_OPTION (
    adw_combo_row_get_selected_item (ADW_COMBO_ROW (priv->modules_comborow))
  );

  g_clear_pointer (&priv->selected_module, g_free);
  g_clear_pointer (&priv->selected_branch, g_free);
  g_clear_pointer (&priv->selected_domain, g_free);

  if (opt)
    priv->selected_module = g_strdup (gtr_drop_down_option_get_name (opt));

  /* Reload module details on module change */
  gtr_dl_teams_load_module_details_json (widget, self);

  gtr_dl_teams_verify_and_load (self);
}

static void
gtr_dl_teams_domains_combo_selected_notify (GtkWidget  *widget,
                                            GParamSpec *spec,
                                            GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  GObject *domain = adw_combo_row_get_selected_item (ADW_COMBO_ROW (priv->domains_comborow));

  if (priv->selected_domain)
    g_free (priv->selected_domain);
  if (domain)
    priv->selected_domain = g_strdup (gtr_dl_teams_domain_name (GTR_DL_TEAMS_DOMAIN (domain)));
  else
    priv->selected_domain = NULL;

  gtr_dl_teams_verify_and_load (self);
}

static void
gtr_dl_teams_branches_combo_selected_notify (GtkWidget  *widget,
                                             GParamSpec *spec,
                                             GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);

  int selected = adw_combo_row_get_selected (ADW_COMBO_ROW (widget));
  if (priv->selected_branch)
    g_free (priv->selected_branch);
  if (selected != GTK_INVALID_LIST_POSITION)
    priv->selected_branch = g_strdup (gtk_string_list_get_string (priv->branches_model, selected));
  else
    priv->selected_branch = NULL;

  /* Check if all four required values have been selected to proceed with loading PO file */
  gtr_dl_teams_verify_and_load (self);
}

static void
gtr_dl_teams_dispose (GObject *object)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (GTR_DL_TEAMS (object));

  g_clear_pointer (&priv->vcs_web, g_free);
  if (priv->selected_lang)
    {
      g_free (priv->selected_lang);
      priv->selected_lang = NULL;
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
  if (priv->soup_session)
    {
      g_object_unref (priv->soup_session);
      priv->soup_session = NULL;
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

  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, file_row);
  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, stats_row);
  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, module_state_row);
  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, load_button);
  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, reserve_button);

  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, langs_comborow);
  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, modules_comborow);
  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, domains_comborow);
  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, branches_comborow);

  gtk_widget_class_bind_template_child_private (widget_class, GtrDlTeams, branches_model);
}

static char *
domains_expression (GtrDlTeamsDomain *domain)
{
  g_assert (GTR_IS_DL_TEAMS_DOMAIN (domain));
  return g_strdup (gtr_dl_teams_domain_description (domain));
}

static void
gtr_dl_teams_init (GtrDlTeams *self)
{
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);
  gtk_widget_init_template (GTK_WIDGET (self));
  GtkExpression *expression = NULL;
  AdwStyleManager *style_manager;

  style_manager = adw_style_manager_get_default ();

  priv->soup_session = soup_session_new ();

  priv->main_window = NULL;
  priv->selected_lang = NULL;
  priv->selected_module = NULL;
  priv->selected_branch = NULL;
  priv->selected_domain = NULL;
  priv->file_path = NULL;
  priv->module_state = NULL;
  priv->vcs_web = NULL;

  priv->domains_model = g_list_store_new (GTR_TYPE_DL_TEAMS_DOMAIN);
  adw_combo_row_set_model (ADW_COMBO_ROW (priv->domains_comborow),
                           G_LIST_MODEL (priv->domains_model));

  gtk_widget_set_sensitive (priv->load_button, FALSE);
  gtk_widget_set_sensitive (priv->reserve_button, FALSE);

  /* Add combo boxes for DL teams and modules */
  expression = gtk_property_expression_new (GTR_TYPE_DROP_DOWN_OPTION, NULL, "description");
  priv->langs_model = g_list_store_new (GTR_TYPE_DROP_DOWN_OPTION);
  adw_combo_row_set_model (
    ADW_COMBO_ROW (priv->langs_comborow),
    G_LIST_MODEL (priv->langs_model)
  );
  adw_combo_row_set_expression (ADW_COMBO_ROW (priv->langs_comborow), expression);
  gtk_expression_unref (expression);
  adw_combo_row_set_enable_search (ADW_COMBO_ROW (priv->langs_comborow), TRUE);
  gtk_widget_set_sensitive (priv->langs_comborow, FALSE);

  expression = gtk_property_expression_new (GTR_TYPE_DROP_DOWN_OPTION, NULL, "name");
  priv->modules_model = g_list_store_new (GTR_TYPE_DROP_DOWN_OPTION);
  adw_combo_row_set_model (
    ADW_COMBO_ROW (priv->modules_comborow),
    G_LIST_MODEL (priv->modules_model)
  );
  adw_combo_row_set_expression (ADW_COMBO_ROW (priv->modules_comborow), expression);
  gtk_expression_unref (expression);
  adw_combo_row_set_enable_search (ADW_COMBO_ROW (priv->modules_comborow), TRUE);
  adw_combo_row_set_search_match_mode (ADW_COMBO_ROW (priv->modules_comborow),
                                       GTK_STRING_FILTER_MATCH_MODE_SUBSTRING);
  gtk_widget_set_sensitive (priv->modules_comborow, FALSE);

  /* Connect "changed" to all combo boxes */
  g_signal_connect (priv->langs_comborow,
                    "notify::selected-item",
                    G_CALLBACK (gtr_dl_teams_langs_combo_selected_notify),
                    self);
  g_signal_connect (priv->modules_comborow,
                    "notify::selected-item",
                    G_CALLBACK (gtr_dl_teams_modules_combo_selected_notify),
                    self);
  g_signal_connect (priv->domains_comborow,
                    "notify::selected-item",
                    G_CALLBACK (gtr_dl_teams_domains_combo_selected_notify),
                    self);
  g_signal_connect (priv->branches_comborow,
                    "notify::selected",
                    G_CALLBACK (gtr_dl_teams_branches_combo_selected_notify),
                    self);

  /* Connect "click" on file button to load PO file */
  g_signal_connect (priv->load_button,
                    "clicked",
                    G_CALLBACK (gtr_dl_teams_load_po_file),
                    self);

  g_signal_connect_swapped (style_manager,
                            "notify::dark",
                            G_CALLBACK (gtr_dl_teams_verify_and_load),
                            self);

  expression = gtk_cclosure_expression_new (G_TYPE_STRING,
                                            NULL,
                                            0, NULL,
                                            G_CALLBACK (domains_expression),
                                            NULL, NULL);
  adw_combo_row_set_expression (ADW_COMBO_ROW (priv->domains_comborow), expression);
  gtk_expression_unref (expression);
}

GtrDlTeams*
gtr_dl_teams_new (GtrWindow *window)
{
  GtrDlTeams *self = g_object_new (GTR_TYPE_DL_TEAMS, NULL);
  GtrDlTeamsPrivate *priv = gtr_dl_teams_get_instance_private (self);

  priv->main_window = window;
  return self;
}

// Domains GObject
GtrDlTeamsDomain*
gtr_dl_teams_domain_new (const char *name, const char *description)
{
  GtrDlTeamsDomain *self = g_object_new (GTR_TYPE_DL_TEAMS_DOMAIN, NULL);
  GtrDlTeamsDomainPrivate *priv = gtr_dl_teams_domain_get_instance_private (self);

  priv->name = g_strdup (name);
  priv->description = g_strdup (description);

  return self;
}

const char*
gtr_dl_teams_domain_name (GtrDlTeamsDomain *domain)
{
  GtrDlTeamsDomainPrivate *priv = gtr_dl_teams_domain_get_instance_private (domain);
  return (const char*) priv->name;
}

const char*
gtr_dl_teams_domain_description (GtrDlTeamsDomain *domain)
{
  GtrDlTeamsDomainPrivate *priv = gtr_dl_teams_domain_get_instance_private (domain);
  return (const char*) priv->description;
}

static void
gtr_dl_teams_domain_dispose (GObject *object)
{
  GtrDlTeamsDomainPrivate *priv = gtr_dl_teams_domain_get_instance_private (GTR_DL_TEAMS_DOMAIN (object));
  if (priv->name)
    g_free (priv->name);
  if (priv->description)
    g_free (priv->description);

  G_OBJECT_CLASS (gtr_dl_teams_domain_parent_class)->dispose (object);
}

static void
gtr_dl_teams_domain_class_init (GtrDlTeamsDomainClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->dispose = gtr_dl_teams_domain_dispose;
}

static void
gtr_dl_teams_domain_init (GtrDlTeamsDomain *self)
{
  GtrDlTeamsDomainPrivate *priv = gtr_dl_teams_domain_get_instance_private (self);

  priv->name = NULL;
  priv->description = NULL;
}
