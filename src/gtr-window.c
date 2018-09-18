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
#include "gtr-debug.h"
#include "gtr-dirs.h"
#include "gtr-header.h"
#include "gtr-msg.h"
#include "gtr-notebook.h"
#include "gtr-tab.h"
#include "gtr-po.h"
#include "gtr-projects.h"
#include "gtr-settings.h"
#include "gtr-statusbar.h"
#include "gtr-utils.h"
#include "gtr-window.h"
#include "gtr-window-activatable.h"
#include "gtr-profile-manager.h"
#include "gtr-status-combo-box.h"

#include "translation-memory/gtr-translation-memory.h"
#include "translation-memory/gtr-translation-memory-dialog.h"
#include "translation-memory/gda/gtr-gda.h"

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

#define PROFILE_DATA "GtrWidnowProfileData"

typedef struct
{
  GSettings *state_settings;
  GSettings *tm_settings;
  GtrTranslationMemory *translation_memory;

  GtkWidget *header_bar;
  GtkWidget *main_box;

  GtkWidget *header_stack;
  GtkWidget *stack;

  GtkWidget *projects;
  GtkWidget *notebook;

  GtrTab *active_tab;

  GtkWidget *statusbar;

  gint width;
  gint height;
  GdkWindowState window_state;

  GtrProfileManager *prof_manager;
  GtkWidget *profile_combo;

  guint dispose_has_run : 1;
} GtrWindowPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(GtrWindow, gtr_window, GTK_TYPE_APPLICATION_WINDOW)

enum
{
  TARGET_URI_LIST = 100
};

static void          profile_combo_changed            (GtrStatusComboBox *combo,
                                                       GtkMenuItem       *item,
                                                       GtrWindow         *window);

static void update_saved_state (GtrPo *po, GParamSpec *param, gpointer window);

static void
update_undo_state (GtrTab     *tab,
                   GtrMsg     *msg,
                   GtrWindow  *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  GtrView *active_view = gtr_window_get_active_view (window);
  gtr_notebook_update_undo_buttons (priv->notebook, active_view);
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
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  GtrPo *po;
  gchar *msg;
  const gchar *status;
  gchar *status_msg;
  gchar *current;
  gchar *total;
  gchar *translated_msg;
  gchar *fuzzy_msg;
  gchar *untranslated_msg;
  gint pos, message_count, translated, fuzzy, untranslated;

  g_return_if_fail (GTR_IS_MSG (message));

  po = gtr_tab_get_po (tab);

  message_count = gtr_po_get_messages_count (po);
  pos = gtr_po_get_message_position (po);
  translated = gtr_po_get_translated_count (po);
  fuzzy = gtr_po_get_fuzzy_count (po);
  untranslated = gtr_po_get_untranslated_count (po);
  status = NULL;

  gtr_notebook_set_progress (GTR_NOTEBOOK (priv->notebook),
                             translated, untranslated, fuzzy);

  switch (gtr_msg_get_status (message))
    {
    case GTR_MSG_STATUS_UNTRANSLATED:
      status = _("Untranslated");
      break;
    case GTR_MSG_STATUS_TRANSLATED:
      status = _("Translated");
      break;
    case GTR_MSG_STATUS_FUZZY:
      status = _("Fuzzy");
      break;
    default:
      break;
    }

  status_msg = g_strdup_printf ("(%s)", status);
  current = g_strdup_printf (_("Current: %d"), pos);
  total = g_strdup_printf (_("Total: %d"), message_count);
  translated_msg = g_strdup_printf (ngettext ("%d translated",
                                              "%d translated",
                                              translated), translated);
  fuzzy_msg = g_strdup_printf (ngettext ("%d fuzzy",
                                         "%d fuzzy", fuzzy), fuzzy);
  untranslated_msg = g_strdup_printf (ngettext ("%d untranslated",
                                                "%d untranslated",
                                                untranslated), untranslated);

  msg = g_strconcat ("    ", current, " ", status_msg, "    ", total,
                     " (", translated_msg, ", ", fuzzy_msg, ", ",
                     untranslated_msg, ")", NULL);

  gtr_statusbar_pop (GTR_STATUSBAR (priv->statusbar), 0);

  gtr_statusbar_push (GTR_STATUSBAR (priv->statusbar), 0, msg);

  g_free (msg);
  g_free (current);
  g_free (status_msg);
  g_free (total);
  g_free (translated_msg);
  g_free (fuzzy_msg);
  g_free (untranslated_msg);

  /* We have to update the progress bar too */
  gtr_statusbar_update_progress_bar (GTR_STATUSBAR
                                     (priv->statusbar),
                                     (gdouble) translated,
                                     (gdouble) message_count);
}

static GtrWindow *
get_drop_window (GtkWidget * widget)
{
  GtkWidget *target_window;

  target_window = gtk_widget_get_toplevel (widget);
  g_return_val_if_fail (GTR_IS_WINDOW (target_window), NULL);

  return GTR_WINDOW (target_window);
}

/* Handle drops on the GtrWindow */
static void
drag_data_received_cb (GtkWidget * widget,
                       GdkDragContext * context,
                       gint x,
                       gint y,
                       GtkSelectionData * selection_data,
                       guint info, guint time, gpointer data)
{
  GtrWindow *window;
  GSList *locations;

  window = get_drop_window (widget);

  if (window == NULL)
    return;

  if (info == TARGET_URI_LIST)
    {
      locations = gtr_utils_drop_get_locations (selection_data);
      gtr_actions_load_locations (window, locations);

      g_slist_free_full (locations, g_object_unref);
    }
}

static void
update_overwrite_mode_statusbar (GtkTextView * view, GtrWindow * window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);

  if (view != GTK_TEXT_VIEW (gtr_window_get_active_view (window)))
    return;

  /* Note that we have to use !gtk_text_view_get_overwrite since we
     are in the in the signal handler of "toggle overwrite" that is
     G_SIGNAL_RUN_LAST
   */
  gtr_statusbar_set_overwrite (GTR_STATUSBAR
                               (priv->statusbar),
                               !gtk_text_view_get_overwrite (view));
}

static void
set_window_title (GtrWindow * window, gboolean with_path)
{
  GtrPo *po;
  GtrPoState state;
  GtrTab *active_tab;
  GFile *file;
  gchar *title;
  gchar *subtitle;
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  GtkHeaderBar *header;

  if (with_path)
    {
      gchar *path;
      gchar *basename;

      active_tab = gtr_window_get_active_tab (window);
      po = gtr_tab_get_po (active_tab);
      state = gtr_po_get_state (gtr_tab_get_po (active_tab));
      po = gtr_tab_get_po (active_tab);
      file = gtr_po_get_location (po);
      path = g_file_get_path (file);
      basename = g_file_get_basename (file);
      subtitle = path;

      if (state == GTR_PO_STATE_MODIFIED)
        {
          /* Translators: this is the title of the window with a modified document */
          title = g_strdup_printf (_("*%s — gtranslator"), basename);
          gtr_notebook_enable_save (GTR_NOTEBOOK (priv->notebook), TRUE);
        }
      else
        {
          /* Translators: this is the title of the window with a document opened */
          title = g_strdup_printf (_("%s — gtranslator"), basename);
          gtr_notebook_enable_save (GTR_NOTEBOOK (priv->notebook), FALSE);
        }

      g_free (basename);
      g_object_unref (file);
    }
  else
    {
      title = g_strdup (_("gtranslator"));
      subtitle = g_strdup ("");
    }

  gtk_window_set_title (GTK_WINDOW (window), title);

  // notebook headerbar
  header = GTK_HEADER_BAR (gtr_notebook_get_header (GTR_NOTEBOOK (priv->notebook)));
  gtk_header_bar_set_title (header, title);
  gtk_header_bar_set_subtitle (header, subtitle);

  g_free (title);
  g_free (subtitle);
}

static void
update_saved_state (GtrPo *po,
                    GParamSpec *param,
                    gpointer window)
{
  set_window_title (GTR_WINDOW (window), TRUE);
}

static void
notebook_switch_page (GtkNotebook * nb,
                      GtkWidget * page,
                      gint page_num, GtrWindow * window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  GtrTab *tab;
  GList *msg;
  GtrView *view;
  GtrPo *po;
  GtrHeader *header;
  GtrProfile *profile;
  gint n_pages;
  GList *profile_items, *l;

  tab = GTR_TAB (gtk_notebook_get_nth_page (nb, page_num));
  if (tab == priv->active_tab)
    return;

  /*
   * Set the window title
   */
  n_pages = gtk_notebook_get_n_pages (nb);
  if (n_pages == 1)
    set_window_title (window, TRUE);
  else
    set_window_title (window, FALSE);

  priv->active_tab = tab;
  view = gtr_tab_get_active_view (tab);

  /* sync the statusbar */
  gtr_statusbar_set_overwrite (GTR_STATUSBAR
                               (priv->statusbar),
                               gtk_text_view_get_overwrite
                               (GTK_TEXT_VIEW (view)));

  po = gtr_tab_get_po (tab);
  msg = gtr_po_get_current_message (po);
  gtr_window_update_statusbar_message_count (tab, msg->data, window);

  header = gtr_po_get_header (po);
  profile = gtr_header_get_profile (header);

  if (profile == NULL)
    profile = gtr_profile_manager_get_active_profile (priv->prof_manager);

  profile_items = gtr_status_combo_box_get_items (GTR_STATUS_COMBO_BOX (priv->profile_combo));

  for (l = profile_items; l != NULL; l = g_list_next (l))
    {
      GtrProfile *item_profile;

      item_profile = GTR_PROFILE (g_object_get_data (G_OBJECT (l->data),
                                                     PROFILE_DATA));

      if (item_profile == profile)
        {
          g_signal_handlers_block_by_func (priv->profile_combo,
                                           profile_combo_changed,
                                           window);
          gtr_status_combo_box_set_item (GTR_STATUS_COMBO_BOX (priv->profile_combo),
                                         GTK_MENU_ITEM (l->data));
          g_signal_handlers_unblock_by_func (priv->profile_combo,
                                             profile_combo_changed,
                                             window);
        }
    }
}

static void
notebook_page_removed (GtkNotebook * notebook,
                       GtkWidget * child, guint page_num, GtrWindow * window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  gint n_pages;

  /* Set the window title */
  n_pages = gtk_notebook_get_n_pages (notebook);
  if (n_pages == 1)
    set_window_title (window, TRUE);
  else
    set_window_title (window, FALSE);

  /* Hide the profile combo */
  if (n_pages == 0)
    gtk_widget_hide (priv->profile_combo);
}

static void
notebook_tab_close_request (GtrNotebook * notebook,
                            GtrTab * tab, GtrWindow * window)
{
  /* Note: we are destroying the tab before the default handler
   * seems to be ok, but we need to keep an eye on this. */
  gtr_close_tab (tab, window);
}


static void
notebook_tab_added (GtkNotebook * notebook,
                    GtkWidget * child, guint page_num, GtrWindow * window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  GList *views;
  GtrTab *tab = GTR_TAB (child);
  gint n_pages;

  g_return_if_fail (GTR_IS_TAB (tab));

  /* Set the window title */
  n_pages = gtk_notebook_get_n_pages (notebook);
  if (n_pages == 1)
    set_window_title (window, TRUE);
  else
    set_window_title (window, FALSE);

  /* Show the profile combo */
  gtk_widget_show (priv->profile_combo);

  views = gtr_tab_get_all_views (tab, FALSE, TRUE);

  while (views)
    {
      g_signal_connect (views->data,
                        "toggle_overwrite",
                        G_CALLBACK (update_overwrite_mode_statusbar), window);

      views = views->next;
    }

  g_signal_connect_after (child,
                          "message_changed",
                          G_CALLBACK
                          (gtr_window_update_statusbar_message_count),
                          window);

  g_signal_connect_after (child,
                          "message_changed",
                          G_CALLBACK (update_undo_state),
                          window);

  g_signal_connect_after (child,
                          "showed-message",
                          G_CALLBACK (update_undo_state),
                          window);

  update_undo_state (NULL, NULL, window);
}

static void
profile_combo_changed (GtrStatusComboBox *combo,
                       GtkMenuItem       *item,
                       GtrWindow         *window)
{
  GtrTab *tab;
  GtrPo *po;
  GtrHeader *header;
  GtrProfile *profile;

  tab = gtr_window_get_active_tab (window);

  if (tab == NULL)
    return;

  po = gtr_tab_get_po (tab);
  header = gtr_po_get_header (po);

  profile = GTR_PROFILE (g_object_get_data (G_OBJECT (item), PROFILE_DATA));

  gtr_header_set_profile (header, profile);
}

static void
fill_profile_combo (GtrWindow *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  GSList *profiles, *l;
  GtkWidget *menu_item;
  const gchar *name;

  profiles = gtr_profile_manager_get_profiles (priv->prof_manager);

  for (l = profiles; l != NULL; l = g_slist_next (l))
    {
      GtrProfile *profile = GTR_PROFILE (l->data);

      name = gtr_profile_get_name (profile);
      menu_item = gtk_menu_item_new_with_label (name);
      gtk_widget_show (menu_item);

      g_object_set_data (G_OBJECT (menu_item), PROFILE_DATA, profile);
      gtr_status_combo_box_add_item (GTR_STATUS_COMBO_BOX (priv->profile_combo),
                                     GTK_MENU_ITEM (menu_item),
                                     name);
    }

  if (profiles == NULL)
    {
      name = _("No profile");

      menu_item = gtk_menu_item_new_with_label (name);

      g_object_set_data (G_OBJECT (menu_item), PROFILE_DATA, NULL);
      gtr_status_combo_box_add_item (GTR_STATUS_COMBO_BOX (priv->profile_combo),
                                     GTK_MENU_ITEM (menu_item),
                                     name);
    }
}

static void
init_statusbar (GtrWindow *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);

  priv->profile_combo = gtr_status_combo_box_new (_("Profile"));
  gtk_widget_set_tooltip_text (priv->profile_combo,
                               _("Profile for the active document"));
#if 0
  gtk_box_pack_start (GTK_BOX (priv->statusbar),
                      priv->profile_combo, FALSE, TRUE, 0);
#endif

  g_signal_connect (priv->profile_combo, "changed",
                    G_CALLBACK (profile_combo_changed), window);

  fill_profile_combo (window);
}

static void
on_active_profile_changed (GtrProfileManager *manager,
                           GtrProfile        *profile,
                           GtrWindow         *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  GList *items, *l;
  GtrTab *tab;
  GtrPo *po;
  GtrHeader *header;

  tab = gtr_window_get_active_tab (window);

  if (tab == NULL)
    return;

  po = gtr_tab_get_po (tab);
  header = gtr_po_get_header (po);

  items = gtr_status_combo_box_get_items (GTR_STATUS_COMBO_BOX (priv->profile_combo));

  for (l = items; l != NULL; l = g_list_next (l))
    {
      GtkMenuItem *menu_item;
      GtrProfile *item_profile;

      menu_item = GTK_MENU_ITEM (l->data);

      item_profile = GTR_PROFILE (g_object_get_data (G_OBJECT (menu_item),
                                  PROFILE_DATA));

      if (item_profile == profile && gtr_header_get_profile (header) == NULL)
        {
          g_signal_handlers_block_by_func (priv->profile_combo,
                                           profile_combo_changed, window);
          gtr_status_combo_box_set_item (GTR_STATUS_COMBO_BOX (priv->profile_combo),
                                         menu_item);
          g_signal_handlers_unblock_by_func (priv->profile_combo,
                                             profile_combo_changed, window);
        }
    }
}

static void
on_profile_added (GtrProfileManager *manager,
                  GtrProfile        *profile,
                  GtrWindow         *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  GtkMenuItem *menu_item;
  GList *items;

  /* check that the item is not a "No profile" item */
  items = gtr_status_combo_box_get_items (GTR_STATUS_COMBO_BOX (priv->profile_combo));

  if (items->next == NULL &&
      (g_object_get_data (G_OBJECT (items->data), PROFILE_DATA) == NULL))
    {
      menu_item = GTK_MENU_ITEM (items->data);

      gtk_menu_item_set_label (menu_item,
                               gtr_profile_get_name (profile));

      g_object_set_data (G_OBJECT (menu_item), PROFILE_DATA, profile);
    }
  else
    {
      const gchar *name;

      name = gtr_profile_get_name (profile);
      menu_item = GTK_MENU_ITEM (gtk_menu_item_new_with_label (name));
      gtk_widget_show (GTK_WIDGET (menu_item));

      g_object_set_data (G_OBJECT (menu_item), PROFILE_DATA, profile);
      gtr_status_combo_box_add_item (GTR_STATUS_COMBO_BOX (priv->profile_combo),
                                     menu_item, name);
    }
}

static void
on_profile_removed (GtrProfileManager *manager,
                    GtrProfile        *profile,
                    GtrWindow         *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  GList *items, *l;

  items = gtr_status_combo_box_get_items (GTR_STATUS_COMBO_BOX (priv->profile_combo));

  for (l = items; l != NULL; l = g_list_next (l))
    {
      GtrProfile *prof;

      prof = GTR_PROFILE (g_object_get_data (G_OBJECT (l->data), PROFILE_DATA));
      if (prof == profile)
        gtr_status_combo_box_remove_item (GTR_STATUS_COMBO_BOX (priv->profile_combo),
                                          GTK_MENU_ITEM (l->data));
    }
}

static void
on_profile_modified (GtrProfileManager *manager,
                     GtrProfile        *old_profile,
                     GtrProfile        *new_profile,
                     GtrWindow         *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  GList *items, *l;

  items = gtr_status_combo_box_get_items (GTR_STATUS_COMBO_BOX (priv->profile_combo));

  for (l = items; l != NULL; l = g_list_next (l))
    {
      GtrProfile *profile;

      profile = GTR_PROFILE (g_object_get_data (G_OBJECT (l->data), PROFILE_DATA));
      if (profile == old_profile)
        {
          gtk_menu_item_set_label (GTK_MENU_ITEM (l->data),
                                   gtr_profile_get_name (new_profile));
          g_object_set_data (G_OBJECT (l->data), PROFILE_DATA, new_profile);

          return;
        }
    }
}

static void
gtr_window_init (GtrWindow *window)
{
  GtkTargetList *tl;
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);

  priv->state_settings = g_settings_new ("org.gnome.gtranslator.state.window");

  gtk_widget_init_template (GTK_WIDGET (window));

  /* Profile manager */
  priv->prof_manager = gtr_profile_manager_get_default ();

  g_signal_connect (priv->prof_manager, "active-profile-changed",
                    G_CALLBACK (on_active_profile_changed), window);
  g_signal_connect (priv->prof_manager, "profile-added",
                    G_CALLBACK (on_profile_added), window);
  g_signal_connect (priv->prof_manager, "profile-removed",
                    G_CALLBACK (on_profile_removed), window);
  g_signal_connect (priv->prof_manager, "profile-modified",
                    G_CALLBACK (on_profile_modified), window);

  /* statusbar & progress bar */
  init_statusbar (window);

  /* Drag and drop support, set targets to NULL because we add the
     default uri_targets below */
  gtk_drag_dest_set (GTK_WIDGET (window),
                     GTK_DEST_DEFAULT_MOTION |
                     GTK_DEST_DEFAULT_HIGHLIGHT |
                     GTK_DEST_DEFAULT_DROP, NULL, 0, GDK_ACTION_COPY);

  /* Add uri targets */
  tl = gtk_drag_dest_get_target_list (GTK_WIDGET (window));

  if (tl == NULL)
    {
      tl = gtk_target_list_new (NULL, 0);
      gtk_drag_dest_set_target_list (GTK_WIDGET (window), tl);
      gtk_target_list_unref (tl);
    }

  gtk_target_list_add_uri_targets (tl, TARGET_URI_LIST);

  /* Connect signals */
  g_signal_connect (window,
                    "drag_data_received",
                    G_CALLBACK (drag_data_received_cb), NULL);

  /**
   * Here we define different widgets that provides to append to the main
   * stack and this widgets can also provide a custom headerbar
   *
   * With this widgets we have different views in the same window
   */

  // poeditor
  priv->notebook = GTK_WIDGET (gtr_notebook_new ());
  gtk_widget_show (priv->notebook);
  g_signal_connect (priv->notebook, "switch-page",
                    G_CALLBACK (notebook_switch_page), window);
  g_signal_connect (priv->notebook, "page-added",
                    G_CALLBACK (notebook_tab_added), window);
  g_signal_connect (priv->notebook, "page-removed",
                    G_CALLBACK (notebook_page_removed), window);
  g_signal_connect (priv->notebook,
                    "tab_close_request",
                    G_CALLBACK (notebook_tab_close_request), window);

  gtk_stack_add_named (GTK_STACK (priv->stack), priv->notebook, "poeditor");
  gtk_stack_add_named (GTK_STACK (priv->header_stack),
                       gtr_notebook_get_header (GTR_NOTEBOOK (priv->notebook)),
                       "poeditor");

  // project selection
  priv->projects = GTK_WIDGET (gtr_projects_new (window));
  gtk_stack_add_named (GTK_STACK (priv->stack), priv->projects, "projects");
  gtk_stack_add_named (GTK_STACK (priv->header_stack),
                       gtr_projects_get_header (GTR_PROJECTS (priv->projects)),
                       "projects");

  gtk_widget_show_all (priv->stack);

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

  gtr_window_show_projects (window);
}

static void
save_panes_state (GtrWindow * window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);

  g_settings_set (priv->state_settings, GTR_SETTINGS_WINDOW_SIZE, "(ii)",
                  priv->width, priv->height);
  g_settings_set_int (priv->state_settings, GTR_SETTINGS_WINDOW_STATE,
                      priv->window_state);
}

static void
gtr_window_dispose (GObject * object)
{
  GtrWindow *window = GTR_WINDOW (object);
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);

  if (!priv->dispose_has_run)
    {
      save_panes_state (window);

      priv->dispose_has_run = TRUE;
    }

  g_clear_object (&priv->state_settings);
  g_clear_object (&priv->prof_manager);
  g_clear_object (&priv->translation_memory);
  g_clear_object (&priv->tm_settings);

  G_OBJECT_CLASS (gtr_window_parent_class)->dispose (object);
}

static void
gtr_window_finalize (GObject * object)
{
  G_OBJECT_CLASS (gtr_window_parent_class)->finalize (object);
}

static gboolean
gtr_window_configure_event (GtkWidget * widget, GdkEventConfigure * event)
{
  GtrWindow *window = GTR_WINDOW (widget);
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);

  priv->width = event->width;
  priv->height = event->height;

  return GTK_WIDGET_CLASS (gtr_window_parent_class)->configure_event (widget,
                                                                      event);
}

static void
gtr_window_class_init (GtrWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gtr_window_finalize;
  object_class->dispose = gtr_window_dispose;

  widget_class->configure_event = gtr_window_configure_event;

  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (klass),
                                               "/org/gnome/translator/gtr-window.ui");

  /* Main layout widgets */
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), GtrWindow, header_bar);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), GtrWindow, main_box);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), GtrWindow, stack);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), GtrWindow, header_stack);
}

/***************************** Public funcs ***********************************/

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
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  GtrTab *tab;

  tab = gtr_tab_new (po, GTK_WINDOW (window));
  gtk_widget_show (GTK_WIDGET (tab));

  gtr_notebook_add_page (GTR_NOTEBOOK (priv->notebook), tab);

  g_signal_connect_after (po,
                          "notify::state",
                          G_CALLBACK
                          (update_saved_state),
                          window);

  return tab;
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
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);

  return gtr_notebook_get_page (GTR_NOTEBOOK (priv->notebook));
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
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  gint num_pages;
  gint i = 0;
  GList *toret = NULL;

  num_pages =
    gtk_notebook_get_n_pages (GTK_NOTEBOOK (priv->notebook));

  while (i < num_pages)
    {
      toret = g_list_append (toret,
                             gtk_notebook_get_nth_page (GTK_NOTEBOOK
                                                        (priv->notebook), i));
      i++;
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
 * gtr_window_get_notebook:
 * @window: a #GtrWindow
 * 
 * Gets the main #GtrNotebook of the @window.
 *
 * Returns: (transfer none): the #GtrNotebook of the @window
 */
GtrNotebook *
gtr_window_get_notebook (GtrWindow * window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);

  return GTR_NOTEBOOK (priv->notebook);
}

/**
 * gtr_window_get_statusbar:
 * @window: a #GtrWindow
 *
 * Gets the statusbar widget of the window.
 *
 * Returns: (transfer none): the statusbar widget of the window
 */
GtkWidget *
gtr_window_get_statusbar (GtrWindow * window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);

  return priv->statusbar;
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
  gint numtabs;
  gint i;
  GList *views = NULL;
  GtkWidget *tab;

  g_return_val_if_fail (GTR_IS_WINDOW (window), NULL);

  numtabs = gtk_notebook_get_n_pages (GTK_NOTEBOOK (priv->notebook));
  i = numtabs - 1;

  while (i >= 0 && numtabs != 0)
    {
      tab = gtk_notebook_get_nth_page (GTK_NOTEBOOK (priv->notebook),
                                       i);
      views =
        g_list_concat (views,
                       gtr_tab_get_all_views (GTR_TAB (tab), original,
                                              translated));
      i--;
    }

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

/**
 * gtr_window_set_active_tab:
 * @window: a #GtrWindow
 * @tab: a #GtrTab
 *
 * Sets the active tab for the @window.
 */
void
gtr_window_set_active_tab (GtrWindow * window, GtkWidget * tab)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  gint page;

  page = gtk_notebook_page_num (GTK_NOTEBOOK (priv->notebook), tab);

  gtk_notebook_set_current_page (GTK_NOTEBOOK (priv->notebook), page);
}

/**
 * _gtr_window_close_tab:
 * @window: a #GtrWindow
 * @tab: a #GtrTab
 *
 * Closes the opened @tab of the @window and sets the right sensitivity of the
 * widgets.
 */
void
_gtr_window_close_tab (GtrWindow * window, GtrTab * tab)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  gint i;

  g_return_if_fail (GTR_IS_TAB (tab));

  i = gtk_notebook_page_num (GTK_NOTEBOOK (priv->notebook),
                             GTK_WIDGET (tab));
  if (i != -1)
    gtr_notebook_remove_page (GTR_NOTEBOOK (priv->notebook), i);

  /*
   * If there is only one file opened, we have to clear the statusbar
   */
  if (i == 0)
    {
      gtr_statusbar_push (GTR_STATUSBAR (priv->statusbar), 0, " ");
      gtr_statusbar_clear_progress_bar (GTR_STATUSBAR
                                        (priv->statusbar));
    }
}

void
gtr_window_show_projects (GtrWindow *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);

  gtk_stack_set_visible_child_name (GTK_STACK (priv->header_stack), "projects");
  gtk_stack_set_visible_child_name (GTK_STACK (priv->stack), "projects");

  gtr_notebook_remove_all_pages (GTR_NOTEBOOK (priv->notebook));
}

void
gtr_window_show_poeditor (GtrWindow *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);

  gtk_stack_set_visible_child_name (GTK_STACK (priv->header_stack), "poeditor");
  gtk_stack_set_visible_child_name (GTK_STACK (priv->stack), "poeditor");
}

void
gtr_window_remove_all_pages (GtrWindow *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  gtr_notebook_remove_all_pages (GTR_NOTEBOOK (priv->notebook));
}

void
gtr_window_show_tm_dialog (GtrWindow *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  static GtkWidget *dlg = NULL;

  if (dlg == NULL)
    {
      dlg = gtr_translation_memory_dialog_new (priv->translation_memory);
      gtk_window_set_transient_for (GTK_WINDOW (dlg), GTK_WINDOW (window));

      g_signal_connect (dlg, "destroy",
                        G_CALLBACK (gtk_widget_destroyed), &dlg);
      gtk_widget_show_all (dlg);
    }

  if (GTK_WINDOW (window) != gtk_window_get_transient_for (GTK_WINDOW (dlg)))
    {
      gtk_window_set_transient_for (GTK_WINDOW (dlg), GTK_WINDOW (window));
    }

  gtk_window_present (GTK_WINDOW (dlg));
}

GtrTranslationMemory *
gtr_window_get_tm (GtrWindow *window) {
  GtrWindowPrivate *priv = gtr_window_get_instance_private (window);
  return priv->translation_memory;
}
