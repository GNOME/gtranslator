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
#include "gtr-dl-teams.h"
#include "gtr-settings.h"
#include "gtr-utils.h"
#include "gtr-window.h"
#include "gtr-window-activatable.h"
#include "gtr-profile-manager.h"

#include "translation-memory/gtr-translation-memory.h"
#include "translation-memory/gtr-translation-memory-dialog.h"
#include "translation-memory/gda/gtr-gda.h"

#include "codeview/gtr-codeview.h"

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

  GtrCodeView *codeview;

  GtkWidget *header_bar;
  GtkWidget *main_box;

  GtkWidget *header_stack;
  GtkWidget *stack;

  GtkWidget *projects;
  GtkWidget *notebook;
  GtkWidget *dlteams;

  GtrTab *active_tab;

  gint width;
  gint height;
  GdkWindowState window_state;

  GtrProfileManager *prof_manager;

  gboolean search_bar_shown;

  guint dispose_has_run : 1;
} GtrWindowPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(GtrWindow, gtr_window, GTK_TYPE_APPLICATION_WINDOW)

enum
{
  TARGET_URI_LIST = 100
};

static void update_saved_state (GtrPo *po, GParamSpec *param, gpointer window);

static void
free_match (gpointer data)
{
  GtrTranslationMemoryMatch *match = (GtrTranslationMemoryMatch *) data;

  g_free (match->match);
  g_slice_free (GtrTranslationMemoryMatch, match);
}

static void
update_undo_state (GtrTab     *tab,
                   GtrMsg     *msg,
                   GtrWindow  *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  GtrView *active_view = gtr_window_get_active_view (window);
  gtr_notebook_update_undo_buttons (GTR_NOTEBOOK (priv->notebook), active_view);
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

  g_return_if_fail (GTR_IS_MSG (message));

  po = gtr_tab_get_po (tab);

  translated = gtr_po_get_translated_count (po);
  fuzzy = gtr_po_get_fuzzy_count (po);
  untranslated = gtr_po_get_untranslated_count (po);

  active_tab = gtr_window_get_active_tab (window);
  gtr_tab_set_progress (GTR_TAB (active_tab),
                        translated, untranslated, fuzzy);
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
set_window_title (GtrWindow * window, gboolean with_path)
{
  GtrPo *po;
  GtrPoState state;
  GtrTab *active_tab;
  GFile *file;
  gchar *title;
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);
  GtkHeaderBar *header;

  if (with_path)
    {
      gchar *basename;

      active_tab = gtr_window_get_active_tab (window);
      po = gtr_tab_get_po (active_tab);
      state = gtr_po_get_state (gtr_tab_get_po (active_tab));
      po = gtr_tab_get_po (active_tab);
      file = gtr_po_get_location (po);
      basename = g_file_get_basename (file);

      if (state == GTR_PO_STATE_MODIFIED)
        {
          /* Translators: this is the title of the window with a modified document */
          title = g_strdup_printf (_("*%s — Translation Editor"), basename);
          gtr_notebook_enable_save (GTR_NOTEBOOK (priv->notebook), TRUE);
        }
      else
        {
          /* Translators: this is the title of the window with a document opened */
          title = g_strdup_printf (_("%s — Translation Editor"), basename);
          gtr_notebook_enable_save (GTR_NOTEBOOK (priv->notebook), FALSE);
        }

      g_free (basename);
      g_object_unref (file);
    }
  else
    {
      title = g_strdup (_("Translation Editor"));
    }

  gtk_window_set_title (GTK_WINDOW (window), title);

  // notebook headerbar
  header = GTK_HEADER_BAR (gtr_notebook_get_header (GTR_NOTEBOOK (priv->notebook)));
  gtk_header_bar_set_title (header, title);

  g_free (title);
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
  GtrPo *po;
  gint n_pages;

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

  po = gtr_tab_get_po (tab);
  msg = gtr_po_get_current_message (po);
  gtr_window_update_statusbar_message_count (tab, msg->data, window);
}

static void
notebook_page_removed (GtkNotebook * notebook,
                       GtkWidget * child, guint page_num, GtrWindow * window)
{
  gint n_pages;

  /* Set the window title */
  n_pages = gtk_notebook_get_n_pages (notebook);
  if (n_pages == 1)
    set_window_title (window, TRUE);
  else
    set_window_title (window, FALSE);
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
  GtrTab *tab = GTR_TAB (child);
  gint n_pages;

  g_return_if_fail (GTR_IS_TAB (tab));

  /* Set the window title */
  n_pages = gtk_notebook_get_n_pages (notebook);
  if (n_pages == 1)
    set_window_title (window, TRUE);
  else
    set_window_title (window, FALSE);

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
gtr_window_init (GtrWindow *window)
{
  GtkTargetList *tl;
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);

  priv->search_bar_shown = FALSE;
  priv->state_settings = g_settings_new ("org.gnome.gtranslator.state.window");

  gtk_widget_init_template (GTK_WIDGET (window));

  /* Profile manager */
  priv->prof_manager = gtr_profile_manager_get_default ();

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

  // DL team selection
  priv->dlteams = GTK_WIDGET (gtr_dl_teams_new (window));
  gtk_stack_add_named (GTK_STACK (priv->stack), priv->dlteams, "dlteams");
  gtk_stack_add_named (GTK_STACK (priv->header_stack),
                       gtr_dl_teams_get_header (GTR_DL_TEAMS (priv->dlteams)),
                       "dlteams");

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

  // code view
  priv->codeview = gtr_code_view_new (window);

  gtr_window_show_projects (window);
}

static void
save_window_state (GtrWindow * window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);

  if ((priv->window_state &
	 (GDK_WINDOW_STATE_MAXIMIZED | GDK_WINDOW_STATE_FULLSCREEN)) == 0)
    {
      gtk_window_get_size (GTK_WINDOW (window), &priv->width, &priv->height);
      g_settings_set (priv->state_settings, GTR_SETTINGS_WINDOW_SIZE,
				"(ii)", priv->width, priv->height);
    }
}

static void
gtr_window_dispose (GObject * object)
{
  GtrWindow *window = GTR_WINDOW (object);
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);

  if (!priv->dispose_has_run)
    {
      save_window_state (window);
      priv->dispose_has_run = TRUE;
    }

  g_clear_object (&priv->state_settings);
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
  gtr_notebook_reset_sort (GTR_NOTEBOOK (priv->notebook));

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
  GtrWindowPrivate *priv = gtr_window_get_instance_private (window);
  g_return_val_if_fail (priv != NULL, NULL);
  g_return_val_if_fail (priv->notebook != NULL, NULL);

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
gtr_window_show_dlteams (GtrWindow *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private(window);

  gtk_stack_set_visible_child_name (GTK_STACK (priv->header_stack), "dlteams");
  gtk_stack_set_visible_child_name (GTK_STACK (priv->stack), "dlteams");
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

void
gtr_window_hide_sort_menu (GtrWindow *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private (window);

  gtr_notebook_hide_sort_menu (GTR_NOTEBOOK (priv->notebook));
}

void
gtr_window_show_search_bar (GtrWindow *window,
                            gboolean show)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private (window);
  GtrNotebook *notebook = GTR_NOTEBOOK (priv->notebook);
  GtrTab *tab = gtr_window_get_active_tab (window);

  gtr_tab_show_hide_search_bar (tab, show);
  gtr_notebook_enable_find_button(notebook, show);

  priv->search_bar_shown = show;
}

void
gtr_window_toggle_search_bar (GtrWindow *window)
{
  GtrWindowPrivate *priv = gtr_window_get_instance_private (window);

  gtr_window_show_search_bar(window, !priv->search_bar_shown);
}
