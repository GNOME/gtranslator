/*
 * Copyright (C) 2007   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * 			Fatih Demir <kabalak@kabalak.net>
 *			Ross Golder <ross@golder.org>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Pablo Sanxiao <psanxiao@gmail.com>
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
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <string.h>
#include <gio/gio.h>

#include "gtr-profile-manager.h"
#include "gtr-close-confirmation-dialog.h"
#include "gtr-actions.h"
#include "gtr-application.h"
#include "gtr-dirs.h"
#include "gtr-file-dialogs.h"
//#include "gtr-notebook.h"
#include "gtr-po.h"
#include "gtr-tab.h"
#include "gtr-utils.h"
#include "gtr-window.h"
#include "gtr-upload-dialog.h"

#define GTR_TAB_SAVE_AS    "gtr-tab-save-as"
#define GTR_IS_CLOSING_ALL "gtr-is-closing-all"

static void load_file_list (GtrWindow * window, const GSList * uris);
static GList * get_modified_documents (GtrWindow * window);

typedef struct {
  SoupMessage *msg;
  GtkWidget   *dialog;
} UserData;

/*
 * The main file opening function. Checks that the file isn't already open,
 * and if not, opens it in a new tab.
 */
gboolean
gtr_open (GFile * location, GtrWindow * window, GError ** error)
{
  GtrPo *po;
  GtrTab *tab;
  GList *current;
  GtrView *active_view;
  GtrHeader *header;
  gchar *dl_team;
  gchar *dl_module;
  gchar *dl_branch;
  gchar *dl_domain;
  gchar *dl_module_state;

  /*
   * If the filename can't be opened, pass the error back to the caller
   * to handle.
   */

  po = gtr_po_new ();
  if (!gtr_po_parse (po, location, error)) {
    gtr_window_show_projects (window);
    // TODO: show error message
    return FALSE;
  }

  if ((*error != NULL)
      && (((GError *) * error)->code != GTR_PO_ERROR_RECOVERY)) {
    gtr_window_show_projects (window);
    return FALSE;
  }

  header = gtr_po_get_header (po);
  dl_team = gtr_header_get_dl_team (header);
  dl_module = gtr_header_get_dl_module (header);
  dl_branch = gtr_header_get_dl_branch (header);
  dl_domain = gtr_header_get_dl_domain (header);
  dl_module_state = gtr_header_get_dl_state (header);

  /*
   * Set Damned Lies info when a po file is opened locally
   */
  gtr_po_set_dl_info(po,
                     dl_team,
                     dl_module,
                     dl_branch,
                     dl_domain,
                     dl_module_state);

  /*
   * Create a page to add to our list of open files
   */
  tab = gtr_window_create_tab (window, po);
  //gtr_window_set_active_tab (window, GTK_WIDGET (tab));

  /*
   * Activate the upload file icon if the po file is in the appropriate
   * state as on the vertimus workflow
   */
  //active_notebook = gtr_window_get_notebook (window);
  gtr_tab_enable_upload (tab, gtr_po_can_dl_upload (po));

  /*
   * Show the current message.
   */
  current = gtr_po_get_current_message (po);
  gtr_tab_message_go_to (tab, current->data, FALSE, GTR_TAB_MOVE_NONE);

  /*
   * Grab the focus
   */
  active_view = gtr_tab_get_active_view (tab);
  gtk_widget_grab_focus (GTK_WIDGET (active_view));

  gtr_window_show_poeditor (window);

  return TRUE;
}

static void
gtr_po_parse_files_from_dialog (GtkNativeDialog * dialog, GtrWindow * window)
{
  GSList *locations = NULL;
  GFile *file;

  g_autoptr (GFile) parent = NULL;
  g_autofree gchar *uri = NULL;

  // Store the file directory for future openings
  file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog));
  parent = g_file_get_parent (file);
  uri = g_file_get_uri (parent);
  _gtr_application_set_last_dir (GTR_APP, uri);

  /*
   * Open the file via our centralized opening function.
   */
  locations = g_slist_append (locations, file);
  load_file_list (window, (const GSList *) locations);
  g_slist_free_full (locations, g_object_unref);
}


static void
gtr_file_chooser_cb (GtkNativeDialog * dialog, guint reply, gpointer user_data)
{
  GtrWindow *window = GTR_WINDOW (user_data);

  if (reply == GTK_RESPONSE_ACCEPT)
    gtr_po_parse_files_from_dialog (dialog, window);

  gtk_native_dialog_destroy (dialog);
}

static void
handle_dialog_response (GtkNativeDialog *dialog, gint response_id, GtrWindow *window ){
  if (response_id == GTK_RESPONSE_YES)
  {
    gtr_save_current_file_dialog (NULL, window);
  }
  else if (response_id == GTK_RESPONSE_CANCEL)
  {
    //to be implemented
  }
}

gboolean
gtr_want_to_save_current_dialog (GtrWindow * window)
{
  gint res = 0;
  GtrTab *tab;
  GtrPo *po;

  GtkWidget *dialog;
  g_autoptr (GFile) location = NULL;
  g_autofree gchar *filename = NULL;
  g_autofree gchar *markup = NULL;

  GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL;

  tab = gtr_window_get_active_tab (window);
  po = gtr_tab_get_po (tab);
  location = gtr_po_get_location (po);
  filename = g_file_get_path (location);

  dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                   flags,
                                   GTK_MESSAGE_WARNING,
                                   GTK_BUTTONS_NONE, NULL);

  markup = g_strdup_printf (
    _("Do you want to save changes to this file: "
      "<span weight=\"bold\" size=\"large\">%s</span>?"),
    filename);

  gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (dialog), markup);
  gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
    _("If you don't save, all your unsaved changes will be permanently lost."));

  gtk_dialog_add_buttons (GTK_DIALOG (dialog),
                          _("Save and open"), GTK_RESPONSE_YES,
                          _("Cancel"), GTK_RESPONSE_CANCEL,
                          _("Continue without saving"), GTK_RESPONSE_NO,
                          NULL);

  //res = gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
  //gtk_widget_destroy (dialog);

  // Below code can be removed as response signal is connected
  /*if (res == GTK_RESPONSE_CANCEL)
    return FALSE;

  if (res == GTK_RESPONSE_YES)
    gtr_save_current_file_dialog (NULL, window);*/
  g_signal_connect(dialog, "response", G_CALLBACK(handle_dialog_response), window);

  return TRUE;
}

/*
 * The "Open file" dialog.
 */
void
gtr_open_file_dialog (GtrWindow * window)
{
  GtkNativeDialog *dialog;
  g_autoptr (GList) list = NULL;
  list = get_modified_documents (window);
  if (list != NULL)
    {
      if (!gtr_want_to_save_current_dialog (window))
        return;
    }

  dialog = gtr_file_chooser_new (GTK_WINDOW (window),
                                 FILESEL_OPEN,
                                 _("Open file for translation"),
                                 _gtr_application_get_last_dir (GTR_APP));

  g_signal_connect (dialog, "response", G_CALLBACK (gtr_file_chooser_cb), window);
  gtk_native_dialog_show (dialog);
}

static void
save_dialog_response_cb (GtkNativeDialog * dialog,
                         gint response_id, GtrWindow * window)
{
  GError *error = NULL;
  GtrPo *po;
  GtrTab *tab;
  gchar *filename;
  GFile *location;

  tab = gtr_window_get_active_tab (window);

  g_return_if_fail (GTK_IS_FILE_CHOOSER (dialog));

  po = gtr_tab_get_po (tab);

  if (response_id != GTK_RESPONSE_ACCEPT)
    {
      gtk_native_dialog_destroy (dialog);
      return;
    }

  //filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
  GFile * file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER(dialog));
  filename = g_file_get_path(file);
  g_return_if_fail (filename != NULL);

  location = g_file_new_for_path (filename);
  g_free (filename);

  gtk_native_dialog_destroy (dialog);

  if (po != NULL)
    {
      gtr_po_set_location (po, location);

      g_object_unref (location);

      gtr_po_save_file (po, &error);

      if (error)
        {
          GtkWidget *dialog;
          GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL;
          dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                           flags,
                                           GTK_MESSAGE_WARNING,
                                           GTK_BUTTONS_OK,
                                           "%s", error->message);
          g_signal_connect (dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
          gtk_window_present (GTK_WINDOW (dialog));
          g_clear_error (&error);
          return;
        }

      /* We have to change the state of the tab */
      gtr_po_set_state (po, GTR_PO_STATE_SAVED);
    }
  else
    {
      g_object_unref (location);
    }
}

static void
_upload_file_callback (GObject      *object,
                       GAsyncResult *result,
                       gpointer      user_data)
{
  UserData *ud = user_data;
  g_autoptr(GInputStream) stream = NULL;
  GtkWidget *dialog;
  GtrTab *active_tab;
  GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL;
  //GtrNotebook *active_notebook;

  GtkWidget *upload_dialog = ud->dialog;
  GtkWidget *window = gtr_upload_dialog_get_parent (GTR_UPLOAD_DIALOG (upload_dialog));
  SoupSession *session = SOUP_SESSION (object);
  SoupStatus status_code = soup_message_get_status (ud->msg);

  GError *error = NULL;

  stream = soup_session_send_finish (session, result, &error);

  active_tab = gtr_window_get_active_tab (GTR_WINDOW (window));

  if (error || !SOUP_STATUS_IS_SUCCESSFUL (status_code))
    {
      if (status_code == SOUP_STATUS_FORBIDDEN)
        {
          dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                           flags,
                                           GTK_MESSAGE_INFO,
                                           GTK_BUTTONS_OK,
                                           _("This file has already been uploaded"));
          //gtr_notebook_enable_upload (active_notebook, FALSE);
          gtr_tab_enable_upload (active_tab, FALSE);
          goto end;
        }

      g_autofree gchar *message = NULL;

      if (error)
        {
          message = error->message;
          g_clear_error (&error);
        }
      else
        {
          message = g_strdup (soup_status_get_phrase (status_code));
        }

      dialog = gtk_message_dialog_new_with_markup (
        GTK_WINDOW (window),
        flags,
        GTK_MESSAGE_WARNING,
        GTK_BUTTONS_CLOSE,
        _(
          "An error occurred while uploading the file: %s\n"
          "Maybe you've not configured your <i>l10n.gnome.org</i> "
          "<b>token</b> correctly in your profile or you don't have "
          "permissions to upload this module."
        ),
        message);
      goto end;
    }

  dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                   flags,
                                   GTK_MESSAGE_INFO,
                                   GTK_BUTTONS_OK,
                                   _("The file has been uploaded!"));

  gtr_tab_enable_upload (active_tab, FALSE);

end:
  g_signal_connect (dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
  gtk_window_present (GTK_WINDOW (dialog));
  gtk_window_destroy (GTK_WINDOW(upload_dialog));
  g_free (ud);
}

void
gtr_upload_file (GtkWidget *upload_dialog,
                 int        response_id,
                 gpointer   user_data)
{
  GtrTab *tab;
  GtrPo *po;
  GBytes *bytes;
  GError *error = NULL;
  GtrProfileManager *pmanager = NULL;
  GtrProfile *profile;
  GtrHeader *header;
  g_autoptr (SoupMultipart) mpart = NULL;

  SoupMessage *msg = NULL;
  static SoupSession *session = NULL;

  g_autofree gchar *content = NULL;
  g_autofree gchar *mime_type = NULL;
  g_autofree gchar *filename = NULL;
  g_autofree gchar *upload_endpoint = NULL;
  const char *auth_token = NULL;
  g_autofree char *auth = NULL;
  g_autofree char *upload_comment = NULL;
  gsize size;
  const gchar *selected_team;
  const gchar *selected_module;
  const gchar *selected_branch;
  const gchar *selected_domain;

  GtrWindow * window = GTR_WINDOW (user_data);

  if (response_id != GTK_RESPONSE_ACCEPT)
    {
      gtk_window_destroy (GTK_WINDOW(upload_dialog));
      return;
    }

  upload_comment = gtr_upload_dialog_get_comment (GTR_UPLOAD_DIALOG (upload_dialog));

  /* Get file content */
  tab = gtr_window_get_active_tab (window);
  po = gtr_tab_get_po (tab);
  filename = g_file_get_basename (gtr_po_get_location (po));
  g_file_load_contents (gtr_po_get_location (po), NULL, &content, &size, NULL,
                        &error);
  if (error != NULL) {
    g_warning ("Error opening file %s: %s", filename, (error)->message);
    g_error_free (error);
  }
  bytes = g_bytes_new (content, size);
  header = gtr_po_get_header (po);

  /* Check mimetype */
  mime_type = g_content_type_guess (filename, (const guchar *) content, size, NULL);

  /* Get the authentication token from the user profile */
  pmanager = gtr_profile_manager_get_default ();
  profile = gtr_profile_manager_get_active_profile (pmanager);
  auth_token = gtr_profile_get_auth_token (profile);
  auth = g_strconcat ("Bearer ", auth_token, NULL);

  selected_module = gtr_po_get_dl_module (po);
  if (selected_module == NULL)
    selected_module = gtr_header_get_dl_module (header);

  selected_branch = gtr_po_get_dl_branch (po);
  if (selected_branch == NULL)
    selected_branch = gtr_header_get_dl_branch (header);

  selected_domain = gtr_po_get_dl_domain (po);
  if (selected_domain == NULL)
    selected_domain = gtr_header_get_dl_domain (header);

  selected_team = gtr_po_get_dl_team (po);
  if (selected_team == NULL)
    selected_team = gtr_header_get_dl_team (header);

  /* API endpoint: modules/[module]/branches/[branch]/domains/[domain]/languages/[team]/upload */
  upload_endpoint = g_strconcat((const gchar *)API_URL,
                                "modules/", selected_module,
                                "/branches/", selected_branch,
                                "/domains/", selected_domain,
                                "/languages/", selected_team,
                                "/upload", NULL);

  /* Init multipart container */
  mpart = soup_multipart_new (SOUP_FORM_MIME_TYPE_MULTIPART);
  soup_multipart_append_form_file (mpart, "file", filename, mime_type, bytes);
  g_bytes_unref (bytes);
  if (upload_comment)
    soup_multipart_append_form_string (mpart, "comment", upload_comment);

  /* Get the associated message */
  msg = soup_message_new_from_multipart (upload_endpoint, mpart);
  soup_message_set_flags (msg, SOUP_MESSAGE_NO_REDIRECT);

  /* Append the authentication header*/
  soup_message_headers_append (soup_message_get_request_headers (msg),
                               "Authentication", auth);

  gtr_upload_dialog_set_loading (GTR_UPLOAD_DIALOG (upload_dialog), TRUE);

  if (!session)
    session = soup_session_new ();

  UserData *ud;
  ud = g_new0 (UserData, 1);
  ud->dialog = upload_dialog;
  ud->msg = msg;
  soup_session_send_async (session, msg, G_PRIORITY_DEFAULT, NULL, _upload_file_callback, ud);
}

/*
 * "Upload file" dialog
 *
 */
void
gtr_upload_file_dialog (GtrWindow * window)
{
  GtrUploadDialog *dialog = gtr_upload_dialog_new (GTK_WIDGET (window));

  g_signal_connect (dialog,
                   "response",
                   G_CALLBACK (gtr_upload_file),
                   window);

  gtk_widget_show (GTK_WIDGET (dialog));
}

/*
 * "Save as" dialog.
 */
void
gtr_save_file_as_dialog (GtrWindow * window)
{
  GtkNativeDialog *dialog;

  dialog = gtr_file_chooser_new (GTK_WINDOW (window),
                                 FILESEL_SAVE,
                                 _("Save file as…"),
                                 g_get_home_dir ());

  g_signal_connect (dialog, "response", G_CALLBACK (save_dialog_response_cb),
                    window);
  gtk_native_dialog_show (dialog);
}

/*
 * A callback for Save
 */
void
gtr_save_current_file_dialog (GtkWidget * widget, GtrWindow * window)
{
  GError *error = NULL;
  GtrTab *current;
  GtrPo *po;

  current = gtr_window_get_active_tab (window);
  po = gtr_tab_get_po (current);

  gtr_po_save_file (po, &error);

  if (error)
    {
      GtkWidget *dialog;
      GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL;
      dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                       flags,
                                       GTK_MESSAGE_WARNING,
                                       GTK_BUTTONS_OK, "%s", error->message);
      g_signal_connect (dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
      gtk_window_present (GTK_WINDOW (dialog));
      g_clear_error (&error);
      return;
    }

  /* We have to change the state of the tab */
  gtr_po_set_state (po, GTR_PO_STATE_SAVED);
}

static gboolean
is_duplicated_location (const GSList * locations, GFile * u)
{
  GSList *l;

  for (l = (GSList *) locations; l != NULL; l = g_slist_next (l))
    {
      if (g_file_equal (u, l->data))
        return TRUE;
    }

  return FALSE;
}

static void
load_file_list (GtrWindow * window, const GSList * locations)
{
  GSList *locations_to_load = NULL;
  GError *error = NULL;

  g_return_if_fail ((locations != NULL) && (locations->data != NULL));

  gtr_window_remove_tab (window);

  locations_to_load = g_slist_reverse (locations);

  while (locations_to_load != NULL)
    {
      g_return_if_fail (locations_to_load->data != NULL);

      if (!gtr_open (locations_to_load->data, window, &error))
        break;

      locations_to_load = g_slist_next (locations_to_load);
    }

  /*
   * Now if there are any error we have to manage it
   * and free the path
   */
  if (error != NULL)
    {
      GtkWidget *dialog;
      GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL;
      /*
       * We have to show the error in a dialog
       */
      dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                       flags,
                                       GTK_MESSAGE_ERROR,
                                       GTK_BUTTONS_CLOSE,
                                       "%s", error->message);
      g_signal_connect (dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
      gtk_window_present (GTK_WINDOW (dialog));
      g_error_free (error);
    }

  g_slist_free ((GSList *) locations_to_load);
}


/**
 * gtr_actions_load_uris:
 *
 * Ignore non-existing URIs 
 */
void
gtr_actions_load_locations (GtrWindow * window, const GSList * locations)
{
  g_return_if_fail (GTR_IS_WINDOW (window));
  g_return_if_fail ((locations != NULL) && (locations->data != NULL));

  load_file_list (window, locations);
}

static void
save_and_close_document (GtrPo * po, GtrWindow * window)
{
  GtrTab *tab;

  gtr_save_current_file_dialog (NULL, window);

  //tab = gtr_tab_get_from_document (po);

  //_gtr_window_close_tab (window, tab);
  gtr_window_remove_tab (window);
}

static void
close_all_tabs (GtrWindow * window)
{
  gtr_window_remove_tab(window);
  /*GtrNotebook *nb;

  nb = gtr_window_get_notebook (window);
  gtr_notebook_remove_all_pages (nb);

  //FIXME: This has to change once we add the close all documents menuitem*/
  gtk_window_destroy (GTK_WINDOW (window));
}

static void
save_and_close_all_documents (GList * unsaved_documents, GtrWindow * window)
{
  GtrTab *tab;
  //GList *l;
  GError *error = NULL;

  /*for (l = unsaved_documents; l != NULL; l = g_list_next (l))
    {
      gtr_po_save_file (l->data, &error);

      if (error)
        {
          GtkWidget *dialog;
          GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL;
          dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                           flags,
                                           GTK_MESSAGE_WARNING,
                                           GTK_BUTTONS_OK,
                                           "%s", error->message);
          g_signal_connect (dialog, "response", G_CALLBACK (gtk_widget_destroy), NULL);
          gtk_window_present (GTK_WINDOW (dialog));
          g_clear_error (&error);

          return;
        }

      tab = gtr_tab_get_from_document (l->data);

      _gtr_window_close_tab (window, tab);
    }*/

  if(unsaved_documents == NULL)
    return;
  gtr_po_save_file(unsaved_documents->data,&error);

  if(error)
  {
    GtkWidget *dialog;
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL;
    dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                     flags,
                                     GTK_MESSAGE_WARNING,
                                     GTK_BUTTONS_OK,
                                     "%s", error->message);
    g_signal_connect (dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
    gtk_window_present (GTK_WINDOW (dialog));
    g_clear_error (&error);
    return;
  }

  gtr_window_remove_tab (window);
  gtk_window_destroy (GTK_WINDOW (window));
}

static void
close_confirmation_dialog_response_handler (GtrCloseConfirmationDialog
                                            * dlg, gint response_id,
                                            GtrWindow * window)
{
  GList *selected_documents;
  gboolean is_closing_all;

  gtk_widget_hide (GTK_WIDGET (dlg));

  is_closing_all = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (window),
                                                       GTR_IS_CLOSING_ALL));

  switch (response_id)
    {
    case GTK_RESPONSE_YES:     /* Save and Close */
      selected_documents =
        gtr_close_confirmation_dialog_get_selected_documents (dlg);
      if (selected_documents == NULL)
        {
          if (is_closing_all)
            {
              gtk_window_destroy (GTK_WINDOW (dlg));

              close_all_tabs (window);

              return;
            }
          else
            g_return_if_reached ();
        }
      else
        {
          if (is_closing_all)
            {
              save_and_close_all_documents (selected_documents, window);
            }
          else
            {
              save_and_close_document (selected_documents->data, window);
            }
        }

      g_list_free (selected_documents);

      break;

    case GTK_RESPONSE_NO:      /* Close without Saving */
      if (is_closing_all)
        {
          gtk_window_destroy (GTK_WINDOW (dlg));

          close_all_tabs (window);

          return;
        }
      else
        {
          // right now is_closing_all is always true so this won't run therefore no need to consider this in port from gtk3 to gtk4
          /*const GList *unsaved_documents;

          unsaved_documents =
            gtr_close_confirmation_dialog_get_unsaved_documents (dlg);
          g_return_if_fail (unsaved_documents->next == NULL);

          _gtr_window_close_tab (window,
                                 gtr_tab_get_from_document
                                 (unsaved_documents->data));*/
        }

      break;
    default:                   /* Do not close */
      break;
    }

  gtk_window_destroy (GTK_WINDOW (dlg));
}

void
gtr_close_tab (GtrTab * tab, GtrWindow * window)
{
  g_object_set_data (G_OBJECT (window),
                     GTR_IS_CLOSING_ALL, GINT_TO_POINTER (0));

  if (!_gtr_tab_can_close (tab))
    {
      GtkWidget *dlg;

      dlg =
        gtr_close_confirmation_dialog_new_single (GTK_WINDOW (window),
                                                  gtr_tab_get_po
                                                  (tab), FALSE);

      g_signal_connect (dlg,
                        "response",
                        G_CALLBACK
                        (close_confirmation_dialog_response_handler), window);

      gtk_widget_show (dlg);
    }
  else
    //_gtr_window_close_tab (window, tab);
    gtr_window_remove_tab(window);
}

void
gtr_file_close (GtrWindow * window)
{
  GtrTab *tab;

  tab = gtr_window_get_active_tab (window);

  gtr_close_tab (tab, window);
}

static GList *
get_modified_documents (GtrWindow * window)
{
  //GtrNotebook *nb;
  GtrTab *tab;
  GtrPo *po;
  GList *list = NULL;

  /*nb = gtr_window_get_notebook (window);
  pages = gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb));

  while (pages > 0)
    {
      tab = GTR_TAB (gtk_notebook_get_nth_page (GTK_NOTEBOOK (nb),
                                                pages - 1));

      po = gtr_tab_get_po (tab);
      if (gtr_po_get_state (po) == GTR_PO_STATE_MODIFIED)
        list = g_list_prepend (list, po);

      pages--;
    }*/

  tab = gtr_window_get_active_tab(window);
  if (tab != NULL) {
    po = gtr_tab_get_po (tab);
    if (gtr_po_get_state (po) == GTR_PO_STATE_MODIFIED)
      list = g_list_prepend (list, po);
  }
  return list;
}

static void
close_all_documents (GtrWindow * window, gboolean logout_mode)
{
  GList *list;

  list = get_modified_documents (window);

  if (list != NULL)
    {
      GtkWidget *dlg;

      dlg = gtr_close_confirmation_dialog_new (GTK_WINDOW (window),
                                               list, logout_mode);

      g_signal_connect (dlg,
                        "response",
                        G_CALLBACK
                        (close_confirmation_dialog_response_handler), window);

      g_list_free (list);

      gtk_widget_show (dlg);
    }
  else
    {
      close_all_tabs (window);

      if (logout_mode)
        {
          gtk_window_destroy (GTK_WINDOW (window));
        }
    }
}

void
gtr_file_quit (GtrWindow * window)
{
  g_object_set_data (G_OBJECT (window),
                     GTR_IS_CLOSING_ALL, GINT_TO_POINTER (1));

  close_all_documents (window, TRUE);
}

void
_gtr_actions_file_close_all (GtrWindow * window)
{
  close_all_documents (window, FALSE);
}

void
_gtr_actions_file_save_all (GtrWindow * window)
{
  GList *list, *l;

  list = get_modified_documents (window);

  for (l = list; l != NULL; l = g_list_next (l))
    {
      GError *error = NULL;

      gtr_po_save_file (GTR_PO (l->data), &error);

      if (error)
        {
          GtkWidget *dialog;
          GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL;

          dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                           flags,
                                           GTK_MESSAGE_WARNING,
                                           GTK_BUTTONS_OK,
                                           "%s", error->message);
          g_signal_connect (dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
          gtk_window_present (GTK_WINDOW (dialog));
          g_clear_error (&error);

          return;
        }

      /* We have to change the state of the tab */
      gtr_po_set_state (GTR_PO (l->data), GTR_PO_STATE_SAVED);
    }

  g_list_free (list);
}
