/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *               2008  Igalia
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
 * Authors:
 *   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *   Pablo Sanxiao <psanxiao@gmail.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-actions.h"
#include "gtr-application.h"
#include "gtr-dirs.h"
#include "gtr-header.h"
#include "gtr-msg.h"
#include "gtr-tab.h"
#include "gtr-po.h"
#include "gtr-projects.h"
#include "gtr-dl-teams.h"
#include "gtr-settings.h"
#include "gtr-utils.h"
#include "gtr-window.h"
#include "gtr-profile-manager.h"
#include "gtr-greeter.h"

#include "translation-memory/gtr-translation-memory.h"
#include "translation-memory/gtr-translation-memory-dialog.h"
#include "translation-memory/gda/gtr-gda.h"

#include "codeview/gtr-codeview.h"

#include <adwaita.h>
#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

#define GTR_STOCK_FUZZY_NEXT "gtranslator-fuzzy-next"
#define GTR_STOCK_FUZZY_PREV "gtranslator-fuzzy-prev"
#define GTR_STOCK_UNTRANS_NEXT "gtranslator-untranslated-next"
#define GTR_STOCK_UNTRANS_PREV "gtranslator-untranslated-prev"
#define GTR_STOCK_FUZZY_UNTRANS_NEXT "gtranslator-fuzzy-untranslated-next"
#define GTR_STOCK_FUZZY_UNTRANS_PREV "gtranslator-fuzzy-untranslated-prev"

#define PROFILE_DATA "GtrWindowProfileData"

typedef struct
{
  GSettings *state_settings;
  GSettings *ui_settings;
  GSettings *tm_settings;
  GtrTranslationMemory *translation_memory;

  GtrCodeView *codeview;

  GtkWidget *projects;
  GtkWidget *dlteams;
  GtkWidget *greeter;

  AdwNavigationView *navigation_view;

  GtkWidget *toast_overlay;

  GtrTab *active_tab;

  gint width;
  gint height;

  GtrProfileManager *prof_manager;

  gboolean search_bar_shown;
} GtrWindowPrivate;

struct _GtrWindow
{
  AdwApplicationWindow parent_instance;
};

G_DEFINE_FINAL_TYPE_WITH_PRIVATE (GtrWindow, gtr_window, ADW_TYPE_APPLICATION_WINDOW)

static void update_saved_state (GtrPo *po, GParamSpec *param, gpointer window);

static void
free_match (gpointer data)
{
  GtrTranslationMemoryMatch *match = (GtrTranslationMemoryMatch *) data;

  g_free (match->match);
  g_slice_free (GtrTranslationMemoryMatch, match);
}

/*
 * gtr_window_update_statusbar_message_count:
 * 
 * This func is used to show the global status of the message list
 * in the statusbar widget.
 */
static void
gtr_window_update_statusbar_message_count (GtrTab * tab,
                                           GtrMsg * message,
                                           GtrWindow * window)
{
  GtrTab *active_tab;
  GtrPo *po;
  gint translated, fuzzy, untranslated;

  po = gtr_tab_get_po (tab);

  translated = gtr_po_get_translated_count (po);
  fuzzy = gtr_po_get_fuzzy_count (po);
  untranslated = gtr_po_get_untranslated_count (po);

  active_tab = gtr_window_get_active_tab (window);
  gtr_tab_set_progress (GTR_TAB (active_tab),
                        translated, untranslated, fuzzy);
}

/* Handle drops on the GtrWindow */
static gboolean
drag_data_received_cb (GtkDropTarget * drop_target,
                       const GValue * value,
                       gdouble x,
                       gdouble y,
                       gpointer data)
{
  GtrWindow * window = GTR_WINDOW (data);
  g_autoptr (GError) error = NULL;
  g_autoptr (GtrPo) po = NULL;
  GFile *location;

  if (!G_VALUE_HOLDS (value, G_TYPE_FILE))
    return FALSE;

  location = g_value_get_object (value);

  po = gtr_po_new_from_file (location, &error);
  if (error != NULL)
    {
      AdwDialog *dialog = adw_alert_dialog_new (NULL, error->message);
      adw_alert_dialog_add_response (ADW_ALERT_DIALOG (dialog), "ok", _("OK"));
      adw_alert_dialog_set_default_response (ADW_ALERT_DIALOG (dialog), "ok");
      adw_dialog_present (ADW_DIALOG (dialog), GTK_WIDGET (window));
      return FALSE;
    }
  gtr_window_set_po (window, po);

  return TRUE;
}

static void
set_window_title (GtrWindow * window, gboolean with_path)
{
  GtrPo *po;
  GtrPoState state;
  GtrTab *active_tab;
  GFile *file;
  g_autofree gchar *title = NULL;

  if (with_path)
    {
      gchar *basename;

      active_tab = gtr_window_get_active_tab (window);
      po = gtr_tab_get_po (active_tab);
      state = gtr_po_get_state (gtr_tab_get_po (active_tab));
      file = gtr_po_get_location (po);
      basename = g_file_get_basename (file);

      if (state == GTR_PO_STATE_MODIFIED)
        {
          /* Translators: this is the title of the window with a modified document */
          title = g_strdup_printf (_("*%s — Translation Editor"), basename);
          gtk_widget_action_set_enabled (GTK_WIDGET (window), "win.save",
                                         TRUE);
        }
      else
        {
          /* Translators: this is the title of the window with a document opened */
          title = g_strdup_printf (_("%s — Translation Editor"), basename);
          gtk_widget_action_set_enabled (GTK_WIDGET (window), "win.save",
                                         FALSE);
        }

      g_free (basename);
      g_object_unref (file);
    }
  else
    {
      title = g_strdup (_("Translation Editor"));
    }

  gtk_window_set_title (GTK_WINDOW (window), title);
}

static void
update_saved_state (GtrPo *po,
                    GParamSpec *param,
                    gpointer window)
{
  set_window_title (GTR_WINDOW (window), TRUE);
  GtrTab * tab = gtr_window_get_active_tab(GTR_WINDOW(window));
  gtr_tab_enable_upload (tab, gtr_po_can_dl_upload (po));
}

static void
gtr_window_init (GtrWindow *window)
{
  //GtkTargetList *tl;
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  GAction *sort_action;
  GAction *show_id_column_action;

  priv->search_bar_shown = FALSE;
  priv->state_settings = g_settings_new ("org.gnome.gtranslator.state.window");
  priv->ui_settings = g_settings_new ("org.gnome.gtranslator.preferences.ui");
  priv->active_tab = NULL;

  /* loading custom styles */
  if (g_strrstr (PACKAGE_APPID, "Devel") != NULL)
    gtk_widget_add_css_class (GTK_WIDGET (window), "devel");

  gtk_widget_init_template (GTK_WIDGET (window));

  /* Profile manager */
  priv->prof_manager = gtr_profile_manager_get_default ();

  /* Drag and drop */
  GtkDropTarget * drop_target = gtk_drop_target_new (G_TYPE_FILE, GDK_ACTION_COPY);
  g_signal_connect (drop_target, "drop", G_CALLBACK (drag_data_received_cb), window);
  gtk_widget_add_controller (GTK_WIDGET (window), GTK_EVENT_CONTROLLER (drop_target));

  // project selection
  priv->projects = GTK_WIDGET (gtr_projects_new (window));

  // DL team selection
  priv->dlteams = GTK_WIDGET (gtr_dl_teams_new (window));

  // Greeter, First launch view
  priv->greeter = GTK_WIDGET (gtr_greeter_new (window));

  // translation memory
  priv->translation_memory = GTR_TRANSLATION_MEMORY (gtr_gda_new());
  priv->tm_settings = g_settings_new ("org.gnome.gtranslator.plugins.translation-memory");
  gtr_translation_memory_set_max_omits (priv->translation_memory,
                                        g_settings_get_int (priv->tm_settings,
                                                            "max-missing-words"));
  gtr_translation_memory_set_max_delta (priv->translation_memory,
                                        g_settings_get_int (priv->tm_settings,
                                                            "max-length-diff"));
  gtr_translation_memory_set_max_items (priv->translation_memory, 10);

  sort_action
    = g_settings_create_action (priv->ui_settings, GTR_SETTINGS_SORT_ORDER);
  g_action_map_add_action (G_ACTION_MAP (window), sort_action);

  show_id_column_action
      = g_settings_create_action (priv->ui_settings, GTR_SETTINGS_SHOW_ID_COLUMN);
  g_action_map_add_action (G_ACTION_MAP (window), show_id_column_action);
}

static void
save_window_state (GtrWindow * window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);

  gtk_window_get_default_size (GTK_WINDOW (window), &priv->width, &priv->height);
  gboolean is_maximized = gtk_window_is_maximized (GTK_WINDOW (window));

  g_settings_set (priv->state_settings, GTR_SETTINGS_WINDOW_SIZE,
                  "(ii)", priv->width, priv->height);
  g_settings_set_boolean (priv->state_settings, GTR_SETTINGS_WINDOW_MAXIMIZED,
                          is_maximized);
}

static void
gtr_window_dispose (GObject * object)
{
  GtrWindow *window = GTR_WINDOW (object);
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);

  g_debug ("Dispose window");

  g_clear_object (&priv->state_settings);
  g_clear_object (&priv->ui_settings);
  g_clear_object (&priv->prof_manager);
  g_clear_object (&priv->translation_memory);
  g_clear_object (&priv->tm_settings);
  g_clear_object (&priv->codeview);

  G_OBJECT_CLASS (gtr_window_parent_class)->dispose (object);
}

static void
gtr_window_finalize (GObject * object)
{
  G_OBJECT_CLASS (gtr_window_parent_class)->finalize (object);
}

static gboolean
gtr_window_close_request (GtkWindow *window)
{
  gtr_file_quit (GTR_WINDOW (window));

  save_window_state (GTR_WINDOW (window));

  // Window is removed in gtr_file_quit, after confirmation of save
  // dialog, return TRUE will avoid the close of the window and let
  // the gtr_file_quit handle it with the close confirmation dialog
  return TRUE;
}

void
gtr_window_save_current_tab (GtrWindow *self)
{
  g_autoptr (GError) error = NULL;
  GtrTab *current;
  GtrPo *po;
  AdwToast *toast;

  current = gtr_window_get_active_tab (self);
  po = gtr_tab_get_po (current);

  gtr_po_save_file (po, &error);

  if (error)
    {
      AdwAlertDialog *dialog;
      dialog = ADW_ALERT_DIALOG (adw_alert_dialog_new (
          _("Could not Save"), error->message));
      adw_alert_dialog_add_response (dialog, "ok", _("OK"));
      adw_alert_dialog_set_default_response (dialog, "ok");
      adw_alert_dialog_set_close_response (dialog, "ok");
      adw_alert_dialog_choose (
          dialog, GTK_WIDGET (self), NULL,
          (GAsyncReadyCallback)adw_alert_dialog_choose_finish, NULL);
      return;
    }

  /* We have to change the state of the tab */
  gtr_po_set_state (po, GTR_PO_STATE_SAVED);
  toast = adw_toast_new_format ("%s", _("File saved"));
  adw_toast_set_timeout (toast, 1);
  gtr_window_add_toast (self, toast);
}

static void
on_save_action (GtkWidget *widget, const char *action_name,
                GVariant *parameter)
{
  GtrWindow *self = GTR_WINDOW (widget);

  gtr_window_save_current_tab (self);
}

static void
gtr_window_class_init (GtrWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWindowClass *window_class = GTK_WINDOW_CLASS (klass);

  object_class->finalize = gtr_window_finalize;
  object_class->dispose = gtr_window_dispose;

  window_class->close_request = gtr_window_close_request;

  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (klass),
                                               "/org/gnome/translator/gtr-window.ui");

  /* Main layout widgets */
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), GtrWindow, toast_overlay);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass),
                                                GtrWindow, navigation_view);

  gtk_widget_class_install_action (GTK_WIDGET_CLASS (klass), "win.save", NULL,
                                   on_save_action);
}

static void
searchbar_toggled (GtrTab * tab, gboolean revealed, GtrWindow *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private (window);
  gtr_tab_enable_find_button (GTR_TAB (priv->active_tab), revealed);
  priv->search_bar_shown = revealed;
}

/***************************** Public funcs ***********************************/
void
gtr_window_remove_tab (GtrWindow * window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private (window);
  if (priv->active_tab != NULL)
  {
    AdwNavigationPage *page;
    page = adw_navigation_view_find_page (priv->navigation_view, "poeditor");
    if (page)
      adw_navigation_view_remove (priv->navigation_view, page);

    priv->active_tab = NULL;
    priv->search_bar_shown = FALSE;
  }
}

/**
 * gtr_window_create_tab:
 * @window: a #GtrWindow
 * @po: a #GtrPo
 * 
 * Adds a new #GtrTab to the #GtrNotebook and returns the
 * #GtrTab.
 * 
 * Returns: (transfer none): a new #GtrTab object
 */
GtrTab *
gtr_window_create_tab (GtrWindow * window, GtrPo * po)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private (window);
  GtrTab *tab;
  AdwNavigationPage *page;

  gtr_window_remove_tab (window);
  tab = gtr_tab_new (po, GTK_WINDOW (window));
  priv->active_tab = tab;

  g_signal_connect_after (tab,
                          "message-changed",
                          G_CALLBACK
                          (gtr_window_update_statusbar_message_count),
                          window);

  gtr_window_update_statusbar_message_count(priv->active_tab,NULL, window);

  page = adw_navigation_view_find_page (priv->navigation_view, "poeditor");
  if (!page)
    {
      page = adw_navigation_page_new (GTK_WIDGET (tab), _("Translation Editor"));
      adw_navigation_page_set_can_pop (page, FALSE);
      adw_navigation_page_set_tag (page, "poeditor");
      adw_navigation_view_add (priv->navigation_view, page);
    }
  else
    {
      adw_navigation_page_set_child (page, GTK_WIDGET (tab));
      adw_navigation_view_pop_to_tag (priv->navigation_view, "poeditor");
    }

  // code view
  priv->codeview = gtr_code_view_new (window);

  g_signal_connect_after (po,
                          "notify::state",
                          G_CALLBACK
                          (update_saved_state),
                          window);

  g_signal_connect (tab, "searchbar-toggled", G_CALLBACK (searchbar_toggled), window);

  set_window_title (window, TRUE);
  return tab;
}

void
gtr_window_set_po (GtrWindow *window, GtrPo *po)
{
  GtrTab *tab;
  GList *current;
  GtrView *active_view;
  GtrHeader *header;
  g_autofree gchar *dl_vcs_web  = NULL;
  g_autofree gchar *dl_lang  = NULL;
  g_autofree gchar *dl_module  = NULL;
  g_autofree gchar *dl_branch  = NULL;
  g_autofree gchar *dl_domain  = NULL;
  g_autofree gchar *dl_module_state  = NULL;

  header = gtr_po_get_header (po);
  dl_lang = gtr_header_get_dl_lang (header);
  dl_module = gtr_header_get_dl_module (header);
  dl_branch = gtr_header_get_dl_branch (header);
  dl_domain = gtr_header_get_dl_domain (header);
  dl_module_state = gtr_header_get_dl_state (header);
  dl_vcs_web = gtr_header_get_dl_vcs_web (header);

  /*
   * Set Damned Lies info when a po file is opened locally
   */
  gtr_po_set_dl_info (po,
                      dl_lang,
                      dl_module,
                      dl_branch,
                      dl_domain,
                      dl_module_state,
                      dl_vcs_web);

  /*
   * Create a page to add to our list of open files
   */
  gtr_window_remove_tab (window);
  tab = gtr_window_create_tab (window, po);

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
}

/**
 * gtr_window_get_active_tab:
 * @window: a #GtrWindow
 * 
 * Gets the active #GtrTab of the @window.
 *
 * Returns: (transfer none): the active #GtrTab of the @window.
 */
GtrTab *
gtr_window_get_active_tab (GtrWindow * window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private (window);
  g_return_val_if_fail (priv != NULL, NULL);
  return priv->active_tab;
}

/**
 * gtr_window_get_all_tabs:
 * @window: a #GtrWindow
 *
 * Gets a list of all tabs in the @window or NULL if there is no tab opened.
 *
 * Returns: (transfer container) (element-type Gtranslator.Tab):
 * a list of all tabs in the @window or NULL if there is no tab opened.
 */
GList *
gtr_window_get_all_tabs (GtrWindow * window)
{
  GList *toret = NULL;
  GtrTab *tab = gtr_window_get_active_tab(window);
  if (tab != NULL) {
    toret = g_list_append (toret,
                          GTK_WIDGET(
                            gtr_window_get_active_tab(window)));
  }
  return toret;
}

/**
 * gtr_window_get_header_from_active_tab:
 * @window: a #GtrWindow
 *
 * Gets the #GtrHeader of the #GtrPo of in the active
 * #GtrTab.
 *
 * Returns: (transfer none): the #GtrHeader of the #GtrPo of in the active
 * #GtrTab
 */
GtrHeader *
gtr_window_get_header_from_active_tab (GtrWindow * window)
{
  GtrTab *current_page;
  GtrPo *po;
  GtrHeader *header;

  g_return_val_if_fail (GTR_IS_WINDOW (window), NULL);

  current_page = gtr_window_get_active_tab (window);
  if (!current_page)
    return NULL;

  po = gtr_tab_get_po (current_page);
  header = gtr_po_get_header (po);

  return header;
}

/**
 * gtr_window_get_active_view:
 * @window: a #GtranslationWindow
 *
 * Gets the active translation view in the #GtranslationWindow or
 * NULL if there is not tab opened.
 *
 * Returns: (transfer none): the active translation view in the
 *          #GtranslationWindow or %NULL if there is not tab opened.
 **/
GtrView *
gtr_window_get_active_view (GtrWindow * window)
{
  GtrTab *current_tab;
  current_tab = gtr_window_get_active_tab (window);

  if (!current_tab)
    return NULL;

  return gtr_tab_get_active_view (current_tab);
}

/**
 * gtr_window_get_all_views:
 * @window: the #GtranslationWindow
 * @original: TRUE if you want original TextViews.
 * @translated: TRUE if you want transtated TextViews.
 *
 * Returns all the views currently present in #GtranslationWindow
 *
 * Return value: (transfer container) (element-type Gtranslator.View):
 * a newly allocated list of #GtranslationWindow objects
 **/
GList *
gtr_window_get_all_views (GtrWindow * window,
                          gboolean original, gboolean translated)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  GList *views = NULL;
  g_return_val_if_fail (GTR_IS_WINDOW (window), NULL);

  views = g_list_concat (views,
                        gtr_tab_get_all_views(GTR_TAB(priv->active_tab), original, translated));

  return views;
}

/**
 * gtr_window_get_tab_from_location:
 * @window: a #GtrWindow
 * @location: the GFile of the po file of the #GtrTab
 *
 * Gets the #GtrTab of the #GtrWindows that matches with the
 * @location.
 *
 * Returns: (transfer none): the #GtrTab which @location matches with its po file.
 */
GtkWidget *
gtr_window_get_tab_from_location (GtrWindow * window, GFile * location)
{
  GList *tabs, *l;
  GtrPo *po;
  GFile *po_location;

  g_return_val_if_fail (GTR_IS_WINDOW (window), NULL);

  tabs = gtr_window_get_all_tabs (window);

  for (l = tabs; l != NULL; l = g_list_next (l))
    {
      po = gtr_tab_get_po (GTR_TAB (l->data));

      po_location = gtr_po_get_location (po);

      if (g_file_equal (location, po_location) == TRUE)
        {
          g_object_unref (po_location);

          return l->data;
        }
      g_object_unref (po_location);
    }

  return NULL;
}

void
gtr_window_show_projects (GtrWindow *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);

  adw_navigation_view_push (priv->navigation_view,
                            ADW_NAVIGATION_PAGE (priv->projects));
}

void
gtr_window_show_poeditor (GtrWindow *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  AdwNavigationPage *page;

  page = adw_navigation_view_get_visible_page (priv->navigation_view);
  if (g_strcmp0 ("poeditor", adw_navigation_page_get_tag (page)) != 0)
    adw_navigation_view_push_by_tag (priv->navigation_view, "poeditor");
}

void
gtr_window_show_dlteams (GtrWindow *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);

  if (!adw_navigation_view_find_page (priv->navigation_view, "dlteams"))
    {
      adw_navigation_view_add (priv->navigation_view,
                               ADW_NAVIGATION_PAGE (priv->dlteams));

      /* Load teams and modules automatically */
      gtr_dl_teams_load_json (GTR_DL_TEAMS (priv->dlteams));
      adw_navigation_view_push (priv->navigation_view,
                                ADW_NAVIGATION_PAGE (priv->dlteams));
    }
  else
    {
      AdwNavigationPage *current = adw_navigation_view_get_visible_page (priv->navigation_view);
      adw_navigation_view_replace_with_tags (priv->navigation_view, (const char *[2]) {
        adw_navigation_page_get_tag (current),
        "dlteams",
      }, 2);
    }
}

void
gtr_window_show_greeter (GtrWindow *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private (window);

  adw_navigation_view_push (priv->navigation_view,
                            ADW_NAVIGATION_PAGE (priv->greeter));
}

void
gtr_window_show_tm_dialog (GtrWindow *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  GtkWidget *dlg;

  dlg = gtr_translation_memory_dialog_new (priv->translation_memory);

  adw_dialog_present (ADW_DIALOG (dlg), GTK_WIDGET (window));
}

GtrTranslationMemory *
gtr_window_get_tm (GtrWindow *window) {
  GtrWindowPrivate *priv = gtr_window_get_instance_private (window);
  return priv->translation_memory;
}

void
gtr_window_tm_keybind (GtrWindow *window,
                       GSimpleAction *action)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private (window);
  GtrTranslationMemory *tm = priv->translation_memory;
  GList *tm_list;
  const gchar *msgid;
  GtrTab *tab = gtr_window_get_active_tab (window);
  GtrMsg *msg;
  const gchar *action_name;
  GtrPo *po;
  GtrView *view;
  GtkTextBuffer *buffer;
  gint index = 0;
  GtrTranslationMemoryMatch *match = NULL;

  if (!tab)
    return;

  view = gtr_tab_get_active_view (tab);
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
  po = gtr_tab_get_po (tab);
  msg = gtr_tab_get_msg (tab);
  msgid = gtr_msg_get_msgid (msg);
  tm_list = gtr_translation_memory_lookup (tm, msgid);

  action_name = g_action_get_name (G_ACTION (action));
  if (g_strcmp0 (action_name, "tm_1") == 0)
    index = 0;
  else if (g_strcmp0 (action_name, "tm_2") == 0)
    index = 1;
  else if (g_strcmp0 (action_name, "tm_2") == 0)
    index = 1;
  else if (g_strcmp0 (action_name, "tm_3") == 0)
    index = 2;
  else if (g_strcmp0 (action_name, "tm_4") == 0)
    index = 3;
  else if (g_strcmp0 (action_name, "tm_5") == 0)
    index = 4;
  else if (g_strcmp0 (action_name, "tm_6") == 0)
    index = 5;
  else if (g_strcmp0 (action_name, "tm_7") == 0)
    index = 6;
  else if (g_strcmp0 (action_name, "tm_8") == 0)
    index = 7;
  else if (g_strcmp0 (action_name, "tm_9") == 0)
    index = 8;

  match = (GtrTranslationMemoryMatch *) g_list_nth_data (tm_list, index);
  if (match)
    {
      gtk_text_buffer_begin_user_action (buffer);
      gtr_msg_set_msgstr (msg, match->match);
      gtk_text_buffer_set_text (buffer, match->match, -1);
      gtr_po_set_state (po, GTR_PO_STATE_MODIFIED);
      gtk_text_buffer_end_user_action (buffer);
    }

  g_list_free_full (tm_list, free_match);
}

static void
gtr_window_show_search_bar (GtrWindow *window,
                            gboolean show)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private (window);
  GtrTab *tab = gtr_window_get_active_tab (window);

  if (!tab)
    return;

  gtr_tab_show_hide_search_bar (tab, show);
  gtr_tab_enable_find_button (tab, show);

  priv->search_bar_shown = show;
}

// Shortcut for find now calls this
void
gtr_window_show_focus_search_bar (GtrWindow *window,
                                  gboolean show)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private (window);
  GtrTab *tab = gtr_window_get_active_tab (window);

  // don't need to show if already shown but need to focus on entry
  if (tab != NULL && priv->search_bar_shown == show)
    gtr_tab_focus_search_bar (tab);
  else
    gtr_window_show_search_bar (window, show);
}

void
gtr_window_toggle_search_bar (GtrWindow *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private (window);

  gtr_window_show_search_bar (window, !priv->search_bar_shown);
}

void
gtr_window_add_toast (GtrWindow *window, AdwToast *toast)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private (window);
  adw_toast_overlay_add_toast (ADW_TOAST_OVERLAY (priv->toast_overlay), toast);
}

void
gtr_window_add_toast_msg (GtrWindow *window,
                          const char *message)
{
  AdwToast *toast = adw_toast_new_format ("%s", message);
  adw_toast_set_timeout (toast, 5);
  gtr_window_add_toast (window, toast);
}

static void
on_launch (GObject *object, GAsyncResult *result, gpointer user_data)
{
  g_autoptr (GError) error = NULL;
  gtk_uri_launcher_launch_finish (GTK_URI_LAUNCHER (object), result, &error);

  if (error)
    g_error ("Could not open uri: %s", error->message);
}

void
gtr_window_open_file_in_browser (GtrWindow  *self,
                                 const char *vcs_web,
                                 const char *module,
                                 const char *file,
                                 const char *branch_name,
                                 int         line_number)
{
  g_autoptr(GtkUriLauncher) launcher = NULL;
  g_autofree char *module_endpoint = NULL;
  g_autofree char *uri = NULL;
  g_autofree char *file_with_line = NULL;

  file_with_line = g_strdup_printf ("%s#L%d", file, line_number);
  uri = g_build_path ("/", vcs_web, "/-/blob/", branch_name, file_with_line, NULL);

  g_debug ("Opening %s", uri);

  launcher = gtk_uri_launcher_new (uri);
  gtk_uri_launcher_launch (launcher, GTK_WINDOW (self), NULL, on_launch, NULL);
}
