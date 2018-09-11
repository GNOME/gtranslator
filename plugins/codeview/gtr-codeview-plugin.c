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

#include "gtr-codeview-plugin.h"
#include "gtr-context.h"
#include "gtr-dirs.h"
#include "gtr-utils.h"
#include "gtr-viewer.h"
#include "gtr-window.h"
#include "gtr-window-activatable.h"

#include <libpeas-gtk/peas-gtk-configurable.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <gio/gio.h>
#include <ctype.h>

#define GTR_SETTINGS_USE_EDITOR "use-editor"
#define GTR_SETTINGS_PROGRAM_CMD "program-cmd"
#define GTR_SETTINGS_LINE_CMD "line-cmd"

#define GTR_CODE_VIEW_PLUGIN_GET_PRIVATE(object) \
				(G_TYPE_INSTANCE_GET_PRIVATE ((object),	\
				GTR_TYPE_CODE_VIEW_PLUGIN,		\
				GtrCodeViewPluginPrivate))

struct _GtrCodeViewPluginPrivate
{
  GSettings *settings;

  GtrWindow *window;

  GSList *tags;
};

typedef struct _GtrCodeViewConfigureWidget GtrCodeViewConfigureWidget;

struct _GtrCodeViewConfigureWidget
{
  GSettings *settings;

  GtkWidget *main_box;
  GtkWidget *use_editor_checkbutton;
  GtkWidget *program_box;
  GtkWidget *program_cmd_entry;
  GtkWidget *line_cmd_entry;
};

enum
{
  PROP_0,
  PROP_WINDOW
};

static void gtr_window_activatable_iface_init (GtrWindowActivatableInterface *iface);
static void peas_gtk_configurable_iface_init (PeasGtkConfigurableInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED (GtrCodeViewPlugin,
                                gtr_code_view_plugin,
                                PEAS_TYPE_EXTENSION_BASE,
                                0,
                                G_IMPLEMENT_INTERFACE_DYNAMIC (GTR_TYPE_WINDOW_ACTIVATABLE,
                                                               gtr_window_activatable_iface_init)
                                G_IMPLEMENT_INTERFACE_DYNAMIC (PEAS_GTK_TYPE_CONFIGURABLE,
                                                               peas_gtk_configurable_iface_init))

static void
insert_link (GtkTextBuffer * buffer, GtkTextIter * iter,
             const gchar * path, gint * line,
             GtrCodeViewPlugin * plugin,
             const gchar * msgid)
{
  GtkTextTag *tag;
  gchar *text;

  tag = gtk_text_buffer_create_tag (buffer, NULL,
                                    "foreground", "blue",
                                    "underline", PANGO_UNDERLINE_SINGLE,
                                    NULL);

  g_object_set_data (G_OBJECT (tag), "line", line);
  g_object_set_data_full (G_OBJECT (tag), "path", g_strdup (path), g_free);
  g_object_set_data_full (G_OBJECT (tag), "msgid", g_strdup (msgid), g_free);

  text = g_strdup_printf ("%s:%d\n", path, GPOINTER_TO_INT (line));
  gtk_text_buffer_insert_with_tags (buffer, iter, text, -1, tag, NULL);
  g_free (text);

  plugin->priv->tags = g_slist_prepend (plugin->priv->tags, tag);
}

static void
show_in_editor (const gchar * program_name,
                const gchar * line_cmd, const gchar * path, gint line)
{
  gchar *open[4];

  if (g_find_program_in_path (program_name))
    {
      open[0] = g_strdup (program_name);
    }
  else
    {
      GtkWidget *dialog;

      dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL,
                                       GTK_DIALOG_MODAL,
                                       GTK_BUTTONS_CLOSE,
                                       _("Please install “%s” to be able to show the file"),
                                       program_name);
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
      return;
    }

  open[1] = g_strdup (path);
  open[2] = g_strdup_printf ("%s%d", line_cmd, line);
  open[3] = NULL;

  g_spawn_async (NULL, open, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, NULL);
  g_free (open[0]);
  g_free (open[1]);
  g_free (open[2]);
}

static void
show_source (GtrCodeViewPlugin * plugin, const gchar * path, gint line)
{
  gboolean use_editor;

  use_editor = g_settings_get_boolean (plugin->priv->settings,
                                       GTR_SETTINGS_USE_EDITOR);

  if (use_editor)
    {
      gchar *program_cmd;
      gchar *line_cmd;

      /* Program cmd */
      program_cmd = g_settings_get_string (plugin->priv->settings,
                                           GTR_SETTINGS_PROGRAM_CMD);

      /* Line cmd */
      line_cmd = g_settings_get_string (plugin->priv->settings,
                                        GTR_SETTINGS_LINE_CMD);

      show_in_editor (program_cmd, line_cmd, path, line);

      g_free (program_cmd);
      g_free (line_cmd);
    }
  else
    gtr_show_viewer (plugin->priv->window, path, line);
}

static gboolean
path_is_fake (const gchar * path)
{
  return g_str_has_suffix (path, ".h")
    && !g_file_test (path, G_FILE_TEST_EXISTS);
}

static gchar *
real_path (const gchar * path)
{
  gchar *result = g_strdup (path);

  if (path_is_fake (path))
    result[strlen (result) - 2] = '\0';

  return result;
}

static int
get_line_for_text (const gchar * path, const gchar * msgid)
{
  gchar *content, *str_found, *i, *escaped;
  int result;

  content = NULL;
  result = 1;
  escaped = g_markup_escape_text (msgid, -1);

  if (!g_file_get_contents (path, &content, NULL, NULL))
    goto out;

  i = content;
  while ((str_found = g_strstr_len (i, -1, escaped)))
    {
      gchar c;

      i = str_found + strlen (escaped);
      c = *i;
      if (!isalpha (c) &&
          !isalpha (*(str_found - 1)) && !(c == ':') && !(c == '_'))
        break;
    }
  if (!str_found)
    goto out;

  for (i = content; i < str_found; i++)
    if (*i == '\n')
      result++;

out:
  g_free (content);
  g_free (escaped);

  return result;
}

static void
follow_if_link (GtrCodeViewPlugin * plugin,
                GtkWidget * text_view, GtkTextIter * iter)
{
  GSList *tags = NULL, *tagp = NULL;
  GtrTab *tab;
  GtrPo *po;
  gchar *fullpath;
  gchar *dirname;
  GFile *location, *parent;

  tab = gtr_window_get_active_tab (plugin->priv->window);

  if (!tab)
    return;
  po = gtr_tab_get_po (tab);

  location = gtr_po_get_location (po);
  parent = g_file_get_parent (location);
  g_object_unref (location);

  dirname = g_file_get_path (parent);
  g_object_unref (parent);

  tags = gtk_text_iter_get_tags (iter);
  for (tagp = tags; tagp != NULL; tagp = tagp->next)
    {
      GtkTextTag *tag = tagp->data;
      gchar *path = g_object_get_data (G_OBJECT (tag), "path");
      gint line =
        GPOINTER_TO_INT (g_object_get_data (G_OBJECT (tag), "line"));

      fullpath = g_build_filename (dirname, path, NULL);

      if (path_is_fake (fullpath))
        {
          gchar *msgid = g_object_get_data (G_OBJECT (tag), "msgid");

          path = g_strdup (fullpath);
          g_free (fullpath);
          fullpath = real_path (path);
          g_free (path);

          line = get_line_for_text (fullpath, msgid);
        }

      show_source (plugin, fullpath, line);

      g_free (fullpath);
    }

  if (tags)
    g_slist_free (tags);

  g_free (dirname);
}

static gboolean
event_after (GtkWidget * text_view,
             GdkEvent * ev, GtrCodeViewPlugin * plugin)
{
  GtkTextIter start, end, iter;
  GtkTextBuffer *buffer;
  GdkEventButton *event;
  gint x, y;

  if (ev->type != GDK_BUTTON_RELEASE)
    return FALSE;

  event = (GdkEventButton *) ev;

  if (event->button != 1)
    return FALSE;

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));

  /* we shouldn't follow a link if the user has selected something */
  gtk_text_buffer_get_selection_bounds (buffer, &start, &end);
  if (gtk_text_iter_get_offset (&start) != gtk_text_iter_get_offset (&end))
    return FALSE;

  gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (text_view),
                                         GTK_TEXT_WINDOW_WIDGET,
                                         event->x, event->y, &x, &y);

  gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (text_view), &iter, x, y);

  follow_if_link (plugin, text_view, &iter);

  return FALSE;
}

static gboolean hovering_over_link = FALSE;
static GdkCursor *hand_cursor = NULL;
static GdkCursor *regular_cursor = NULL;

/* Looks at all tags covering the position (x, y) in the text view, 
 * and if one of them is a link, change the cursor to the "hands" cursor
 * typically used by web browsers.
 */
static void
set_cursor_if_appropriate (GtkTextView * text_view, gint x, gint y)
{
  GSList *tags = NULL, *tagp = NULL;
  GtkTextIter iter;
  gboolean hovering = FALSE;

  gtk_text_view_get_iter_at_location (text_view, &iter, x, y);

  tags = gtk_text_iter_get_tags (&iter);
  for (tagp = tags; tagp != NULL; tagp = tagp->next)
    {
      GtkTextTag *tag = tagp->data;
      gchar *path = g_object_get_data (G_OBJECT (tag), "path");

      if (path)
        {
          hovering = TRUE;
          break;
        }
    }

  if (hovering != hovering_over_link)
    {
      hovering_over_link = hovering;

      if (hovering_over_link)
        gdk_window_set_cursor (gtk_text_view_get_window (text_view,
                                                         GTK_TEXT_WINDOW_TEXT),
                               hand_cursor);
      else
        gdk_window_set_cursor (gtk_text_view_get_window (text_view,
                                                         GTK_TEXT_WINDOW_TEXT),
                               regular_cursor);
    }

  if (tags)
    g_slist_free (tags);
}

/*
 * Update the cursor image if the pointer moved. 
 */
static gboolean
motion_notify_event (GtkWidget * text_view, GdkEventMotion * event)
{
  gint x, y;

  gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (text_view),
                                         GTK_TEXT_WINDOW_WIDGET,
                                         event->x, event->y, &x, &y);

  set_cursor_if_appropriate (GTK_TEXT_VIEW (text_view), x, y);

  return FALSE;
}

/* Also update the cursor image if the window becomes visible
 * (e.g. when a window covering it got iconified).
 */
static gboolean
visibility_notify_event (GtkWidget * text_view, GdkEventVisibility * event)
{
  GdkDeviceManager *device_manager;
  GdkDevice *pointer;
  gint wx, wy, bx, by;

  device_manager = gdk_display_get_device_manager (gtk_widget_get_display (text_view));
  pointer = gdk_device_manager_get_client_pointer (device_manager);
  gdk_window_get_device_position (gtk_widget_get_window (text_view), pointer, &wx, &wy, NULL);

  gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (text_view),
                                         GTK_TEXT_WINDOW_WIDGET,
                                         wx, wy, &bx, &by);

  set_cursor_if_appropriate (GTK_TEXT_VIEW (text_view), bx, by);

  return FALSE;
}

static void
gtr_code_view_plugin_init (GtrCodeViewPlugin *plugin)
{
  plugin->priv = GTR_CODE_VIEW_PLUGIN_GET_PRIVATE (plugin);

  plugin->priv->settings = g_settings_new ("org.gnome.gtranslator.plugins.codeview");
  plugin->priv->tags = NULL;
}

static void
gtr_code_view_plugin_dispose (GObject *object)
{
  GtrCodeViewPluginPrivate *priv = GTR_CODE_VIEW_PLUGIN (object)->priv;

  g_clear_object (&priv->settings);
  g_clear_object (&priv->window);

  G_OBJECT_CLASS (gtr_code_view_plugin_parent_class)->dispose (object);
}

static void
gtr_code_view_plugin_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  GtrCodeViewPluginPrivate *priv = GTR_CODE_VIEW_PLUGIN (object)->priv;

  switch (prop_id)
    {
      case PROP_WINDOW:
        priv->window = GTR_WINDOW (g_value_dup_object (value));
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
gtr_code_view_plugin_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  GtrCodeViewPluginPrivate *priv = GTR_CODE_VIEW_PLUGIN (object)->priv;

  switch (prop_id)
    {
      case PROP_WINDOW:
        g_value_set_object (value, priv->window);
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
showed_message_cb (GtrTab * tab,
                   GtrMsg * msg, GtrCodeViewPlugin * plugin)
{
  const gchar *filename = NULL;
  gint i = 0;
  gint *line = NULL;
  GtkTextIter iter;
  GtkTextBuffer *buffer;
  GtkTextView *view;
  GtrContextPanel *panel;
  GtkTextTag *bold;

  panel = gtr_tab_get_context_panel (tab);
  view = gtr_context_panel_get_context_text_view (panel);

  buffer = gtk_text_view_get_buffer (view);

  gtk_text_buffer_get_end_iter (buffer, &iter);

  /* Create the bold tag for header */
  bold = gtk_text_buffer_create_tag (buffer, NULL, "weight", PANGO_WEIGHT_BOLD,
                                     "weight-set", TRUE, NULL);

  gtk_text_buffer_insert (buffer, &iter, "\n", 1);
  gtk_text_buffer_insert_with_tags (buffer, &iter, _("Paths:"), -1, bold, NULL);
  gtk_text_buffer_insert (buffer, &iter, "\n", 1);

  filename = gtr_msg_get_filename (msg, i);
  while (filename)
    {
      line = gtr_msg_get_file_line (msg, i);
      insert_link (buffer, &iter, filename, line, plugin,
                   gtr_msg_get_msgid (msg));
      i++;
      filename = gtr_msg_get_filename (msg, i);
    }

  /*
   * The tags are managed by buffer, so lets add a reference in the buffer
   */
  g_object_set_data (G_OBJECT (buffer), "link_tags", plugin->priv->tags);
  plugin->priv->tags = NULL;
}

static void
delete_text_and_tags (GtrTab * tab, GtrCodeViewPlugin * plugin)
{
  GSList *tagp = NULL, *tags;
  GtkTextBuffer *buffer;
  GtrContextPanel *panel;
  GtkTextView *view;
  GtkTextIter start, end;
  GtkTextMark *path_start, *path_end;

  panel = gtr_tab_get_context_panel (tab);
  view = gtr_context_panel_get_context_text_view (panel);

  buffer = gtk_text_view_get_buffer (view);
  path_start = gtk_text_buffer_get_mark (buffer, "path_start");

  if (path_start == NULL)
    return;

  path_end = gtk_text_buffer_get_mark (buffer, "path_end");
  tags = g_object_get_data (G_OBJECT (buffer), "link_tags");

  for (tagp = tags; tagp != NULL; tagp = tagp->next)
    {
      GtkTextTag *tag = tagp->data;

      g_object_set_data (G_OBJECT (tag), "path", NULL);
      g_object_set_data (G_OBJECT (tag), "msgid", NULL);
    }
  g_slist_free (tags);

  /*
   * Deleting the text
   */
  gtk_text_buffer_get_iter_at_mark (buffer, &start, path_start);
  gtk_text_buffer_get_iter_at_mark (buffer, &end, path_end);
  gtk_text_buffer_delete (buffer, &start, &end);

  /*
   * Deleting the marks
   */
  gtk_text_buffer_delete_mark (buffer, path_start);
  gtk_text_buffer_delete_mark (buffer, path_end);
}

static void
message_edition_finished_cb (GtrTab * tab,
                             GtrMsg * msg, GtrCodeViewPlugin * plugin)
{
  delete_text_and_tags (tab, plugin);
}

static void
on_context_panel_reloaded (GtrContextPanel         *panel,
                           GtrMsg                  *msg,
                           GtrCodeViewPlugin *plugin)
{
  GtrTab *tab;

  tab = gtr_window_get_active_tab (plugin->priv->window);

  showed_message_cb (tab, msg, plugin);
}

static void
page_added_cb (GtkNotebook * notebook,
               GtkWidget * child,
               guint page_num, GtrCodeViewPlugin * plugin)
{
  GtrContextPanel *panel;
  GtkTextView *view;

  panel = gtr_tab_get_context_panel (GTR_TAB (child));
  view = gtr_context_panel_get_context_text_view (panel);

  g_return_if_fail (GTK_IS_TEXT_VIEW (view));

  g_signal_connect_after (child, "showed-message",
                          G_CALLBACK (showed_message_cb), plugin);
  g_signal_connect (child, "message-edition-finished",
                    G_CALLBACK (message_edition_finished_cb), plugin);

  g_signal_connect (view, "event-after", G_CALLBACK (event_after), plugin);
  g_signal_connect (view, "motion-notify-event",
                    G_CALLBACK (motion_notify_event), NULL);
  g_signal_connect (view, "visibility-notify-event",
                    G_CALLBACK (visibility_notify_event), NULL);
  g_signal_connect (panel, "reloaded",
                    G_CALLBACK (on_context_panel_reloaded), plugin);
}

static void
use_editor_toggled (GSettings                  *settings,
                    const gchar                *key,
                    GtrCodeViewConfigureWidget *widget)
{
  gtk_widget_set_sensitive (widget->program_box,
                            g_settings_get_boolean (settings, key));
}

static void
configure_widget_destroyed (GtkWidget *widget,
			    gpointer   data)
{
  GtrCodeViewConfigureWidget *conf_widget = (GtrCodeViewConfigureWidget *)data;

  g_object_unref (conf_widget->settings);
  g_slice_free (GtrCodeViewConfigureWidget, conf_widget);
}

static GtkWidget *
get_configuration_dialog (GtrCodeViewPlugin *plugin)
{

  GtrCodeViewConfigureWidget *widget;
  GtkBuilder *builder;
  gchar *root_objects[] = {
    "main_box",
    NULL
  };

  widget = g_slice_new (GtrCodeViewConfigureWidget);
  widget->settings = g_object_ref (plugin->priv->settings);

  builder = gtk_builder_new ();
  gtk_builder_add_objects_from_resource (builder, "/org/gnome/gtranslator/plugins/codeview/ui/gtr-codeview-dialog.ui",
					 root_objects, NULL);
  widget->main_box = GTK_WIDGET (gtk_builder_get_object (builder, "main_box"));
  g_object_ref (widget->main_box);
  widget->use_editor_checkbutton = GTK_WIDGET (gtk_builder_get_object (builder, "use_editor"));
  widget->program_box = GTK_WIDGET (gtk_builder_get_object (builder, "program_box"));
  widget->program_cmd_entry = GTK_WIDGET (gtk_builder_get_object (builder, "program_cmd"));
  widget->line_cmd_entry = GTK_WIDGET (gtk_builder_get_object (builder, "line_cmd"));
  g_object_unref (builder);

  /* Use editor */
  gtk_widget_set_sensitive (widget->program_box,
                            g_settings_get_boolean (plugin->priv->settings,
                                                    GTR_SETTINGS_USE_EDITOR));

  g_settings_bind (plugin->priv->settings,
                   GTR_SETTINGS_USE_EDITOR,
                   widget->use_editor_checkbutton,
                   "active",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);

  g_settings_bind (plugin->priv->settings,
                   GTR_SETTINGS_PROGRAM_CMD,
                   widget->program_cmd_entry,
                   "text",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);

  g_settings_bind (plugin->priv->settings,
                   GTR_SETTINGS_LINE_CMD,
                   widget->line_cmd_entry,
                   "text",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);

  g_signal_connect (plugin->priv->settings, "changed::"GTR_SETTINGS_USE_EDITOR,
                    G_CALLBACK (use_editor_toggled), widget);

  g_signal_connect (widget->main_box,
                    "destroy",
                    G_CALLBACK (configure_widget_destroyed),
                    widget);

  return widget->main_box;
}

static void
gtr_code_view_plugin_activate (GtrWindowActivatable *activatable)
{
  GtrCodeViewPluginPrivate *priv = GTR_CODE_VIEW_PLUGIN (activatable)->priv;
  GtkWidget *notebook;
  GList *tabs, *l;

  /*
   * Cursors
   */
  hand_cursor = gdk_cursor_new (GDK_HAND2);
  regular_cursor = gdk_cursor_new (GDK_XTERM);

  notebook = GTK_WIDGET (gtr_window_get_notebook (priv->window));

  g_signal_connect (notebook, "page-added",
                    G_CALLBACK (page_added_cb), activatable);

  /*
   * If we already have tabs opened we have to add them
   */
  tabs = gtr_window_get_all_tabs (priv->window);
  for (l = tabs; l != NULL; l = g_list_next (l))
    {
      GtrPo *po;
      GList *msg;

      page_added_cb (GTK_NOTEBOOK (notebook),
                     l->data, 0, GTR_CODE_VIEW_PLUGIN (activatable));

      po = gtr_tab_get_po (GTR_TAB (l->data));
      msg = gtr_po_get_current_message (po);

      showed_message_cb (GTR_TAB (l->data),
                         msg->data, GTR_CODE_VIEW_PLUGIN (activatable));
    }
}

static void
gtr_code_view_plugin_deactivate (GtrWindowActivatable *activatable)
{
  GtrCodeViewPluginPrivate *priv = GTR_CODE_VIEW_PLUGIN (activatable)->priv;
  GList *tabs, *l;
  GtkTextView *view;
  GtrContextPanel *panel;
  GtkWidget *notebook;

  tabs = gtr_window_get_all_tabs (priv->window);
  notebook = GTK_WIDGET (gtr_window_get_notebook (priv->window));

  for (l = tabs; l != NULL; l = g_list_next (l))
    {
      panel = gtr_tab_get_context_panel (GTR_TAB (l->data));
      view = gtr_context_panel_get_context_text_view (panel);

      delete_text_and_tags (GTR_TAB (l->data),
                            GTR_CODE_VIEW_PLUGIN (activatable));

      g_signal_handlers_disconnect_by_func (l->data,
                                            showed_message_cb, activatable);

      g_signal_handlers_disconnect_by_func (view, event_after, priv->window);
      g_signal_handlers_disconnect_by_func (view, motion_notify_event, NULL);
      g_signal_handlers_disconnect_by_func (view,
                                            visibility_notify_event, NULL);
    }

  g_signal_handlers_disconnect_by_func (notebook, page_added_cb, activatable);
}

static GtkWidget *
gtr_code_view_create_configure_widget (PeasGtkConfigurable *configurable)
{
  return get_configuration_dialog (GTR_CODE_VIEW_PLUGIN (configurable));
}

static void
gtr_code_view_plugin_class_init (GtrCodeViewPluginClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gtr_code_view_plugin_dispose;
  object_class->set_property = gtr_code_view_plugin_set_property;
  object_class->get_property = gtr_code_view_plugin_get_property;

  g_object_class_override_property (object_class, PROP_WINDOW, "window");

  g_type_class_add_private (object_class, sizeof (GtrCodeViewPluginPrivate));
}

static void
gtr_code_view_plugin_class_finalize (GtrCodeViewPluginClass *klass)
{
}

static void
peas_gtk_configurable_iface_init (PeasGtkConfigurableInterface *iface)
{
  iface->create_configure_widget = gtr_code_view_create_configure_widget;
}

static void
gtr_window_activatable_iface_init (GtrWindowActivatableInterface *iface)
{
  iface->activate = gtr_code_view_plugin_activate;
  iface->deactivate = gtr_code_view_plugin_deactivate;
}

G_MODULE_EXPORT void
peas_register_types (PeasObjectModule *module)
{
  gtr_code_view_plugin_register_type (G_TYPE_MODULE (module));

  peas_object_module_register_extension_type (module,
                                              GTR_TYPE_WINDOW_ACTIVATABLE,
                                              GTR_TYPE_CODE_VIEW_PLUGIN);
  peas_object_module_register_extension_type (module,
                                              PEAS_GTK_TYPE_CONFIGURABLE,
                                              GTR_TYPE_CODE_VIEW_PLUGIN);
}
