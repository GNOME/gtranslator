/*
 * gtr-charmap-plugin.c - Character map side-pane for gtr
 * 
 * Copyright (C) 2006 Steve Frécinaux
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-charmap-plugin.h"
#include "gtr-charmap-panel.h"

#include <glib/gi18n-lib.h>
#include "gtr-application.h"
#include "gtr-statusbar.h"
#include "gtr-window.h"
#include "gtr-window-activatable.h"

#include <libpeas-gtk/peas-gtk-configurable.h>
#include <gucharmap/gucharmap.h>

#define GTR_CHARMAP_PLUGIN_GET_PRIVATE(object) \
                                (G_TYPE_INSTANCE_GET_PRIVATE ((object), \
                                GTR_TYPE_CHARMAP_PLUGIN,                \
                                GtrCharmapPluginPrivate))

struct _GtrCharmapPluginPrivate
{
  GtrWindow *window;
  GtkWidget *charmap;

  guint      context_id;
};

enum
{
  PROP_0,
  PROP_WINDOW
};

static void gtr_window_activatable_iface_init (GtrWindowActivatableInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED (GtrCharmapPlugin,
                                gtr_charmap_plugin,
                                PEAS_TYPE_EXTENSION_BASE,
                                0,
                                G_IMPLEMENT_INTERFACE_DYNAMIC (GTR_TYPE_WINDOW_ACTIVATABLE,
                                                               gtr_window_activatable_iface_init)                       \
                                                                                                                        \
                                                               _gtr_charmap_panel_register_type (type_module);          \
)

static void
gtr_charmap_plugin_init (GtrCharmapPlugin * plugin)
{

  plugin->priv = GTR_CHARMAP_PLUGIN_GET_PRIVATE (plugin);
}

static void
gtr_charmap_plugin_dispose (GObject * object)
{

  GtrCharmapPlugin *plugin = GTR_CHARMAP_PLUGIN (object);

  if (plugin->priv->window != NULL)
  {
    g_object_unref (plugin->priv->window);
    plugin->priv->window = NULL;
  }

  G_OBJECT_CLASS (gtr_charmap_plugin_parent_class)->dispose (object);
}

static void
gtr_charmap_plugin_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  GtrCharmapPlugin *plugin = GTR_CHARMAP_PLUGIN (object);

  switch (prop_id)
  {
    case PROP_WINDOW:
      plugin->priv->window = GTR_WINDOW (g_value_dup_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gtr_charmap_plugin_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  GtrCharmapPlugin *plugin = GTR_CHARMAP_PLUGIN (object);

  switch (prop_id)
  {
    case PROP_WINDOW:
      g_value_set_object (value, plugin->priv->window);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
on_table_status_message (GucharmapChartable *chartable,
                         const gchar        *message,
                         GtrCharmapPlugin   *plugin)
{
  GtrStatusbar *statusbar;

  statusbar = GTR_STATUSBAR (gtr_window_get_statusbar (plugin->priv->window));

  gtr_statusbar_pop (statusbar, plugin->priv->context_id);

  if (message)
    gtr_statusbar_push (statusbar, plugin->priv->context_id, message);
}

static void
on_table_sync_active_char (GucharmapChartable *chartable,
                           GParamSpec         *psepc,
                           GtrCharmapPlugin   *plugin)
{
  GString *gs;
  const gchar **temps;
  gint i;
  gunichar wc;

  wc = gucharmap_chartable_get_active_character (chartable);

  gs = g_string_new (NULL);
  g_string_append_printf (gs, "U+%4.4X %s", wc,
                          gucharmap_get_unicode_name (wc));

  temps = gucharmap_get_nameslist_equals (wc);
  if (temps)
    {
      g_string_append_printf (gs, "   = %s", temps[0]);
      for (i = 1; temps[i]; i++)
        g_string_append_printf (gs, "; %s", temps[i]);
      g_free (temps);
    }

  temps = gucharmap_get_nameslist_stars (wc);
  if (temps)
    {
      g_string_append_printf (gs, "   \342\200\242 %s", temps[0]);
      for (i = 1; temps[i]; i++)
        g_string_append_printf (gs, "; %s", temps[i]);
      g_free (temps);
    }

  on_table_status_message (chartable, gs->str, plugin);
  g_string_free (gs, TRUE);
}

static gboolean
on_table_focus_out_event (GtkWidget             *drawing_area,
                          GdkEventFocus         *event, 
                          GtrCharmapPlugin      *plugin)
{
  GucharmapChartable *chartable;

  chartable = gtr_charmap_panel_get_chartable (GTR_CHARMAP_PANEL (plugin->priv->charmap));

  on_table_status_message (chartable, NULL, plugin);
  return FALSE;
}

static void
on_table_activate (GucharmapChartable   *chartable,
                   GtrWindow            *window)
{
  GtkTextView *view;
  GtkTextBuffer *document;
  GtkTextIter start, end;
  gchar buffer[6];
  gchar length;
  gunichar wc;

  wc = gucharmap_chartable_get_active_character (chartable);

  g_return_if_fail (gucharmap_unichar_validate (wc));

  view = GTK_TEXT_VIEW (gtr_window_get_active_view (window));

  if (!view || !gtk_text_view_get_editable (view))
    return;

  document = gtk_text_view_get_buffer (view);

  g_return_if_fail (document != NULL);

  length = g_unichar_to_utf8 (wc, buffer);

  gtk_text_buffer_begin_user_action (document);

  gtk_text_buffer_get_selection_bounds (document, &start, &end);

  gtk_text_buffer_delete_interactive (document, &start, &end, TRUE);
  if (gtk_text_iter_editable (&start, TRUE))
    gtk_text_buffer_insert (document, &start, buffer, length);

  gtk_text_buffer_end_user_action (document);
}

static GtkWidget *
create_charmap_panel (GtrCharmapPlugin *plugin)
{
  GtkWidget *panel;
  GucharmapChartable *chartable;

  panel = gtr_charmap_panel_new ();

  chartable = gtr_charmap_panel_get_chartable (GTR_CHARMAP_PANEL (panel));

  g_signal_connect (chartable,
                    "notify::active-character",
                    G_CALLBACK (on_table_sync_active_char),
                    plugin);
  g_signal_connect (chartable,
                    "focus-out-event",
                    G_CALLBACK (on_table_focus_out_event),
                    plugin);
  g_signal_connect (chartable,
                    "status-message",
                    G_CALLBACK (on_table_status_message),
                    plugin);
  g_signal_connect (chartable,
                    "activate", G_CALLBACK (on_table_activate),
                    plugin->priv->window);

  gtk_widget_show_all (panel);

  return panel;
}

static void
gtr_charmap_plugin_activate (GtrWindowActivatable *activatable)
{
  GtrCharmapPluginPrivate *priv;
  GtrStatusbar *statusbar;

  priv = GTR_CHARMAP_PLUGIN (activatable)->priv;

  gtr_application_register_icon (GTR_APP, "gucharmap.ico",
                                 "charmap-plugin-icon");

  priv->charmap = create_charmap_panel (GTR_CHARMAP_PLUGIN (activatable));

  gtr_window_add_widget (priv->window,
                         priv->charmap,
                         "GtrCharmapPlugin",
                         _("Character Map"),
                         "charmap-plugin-icon",
                         GTR_WINDOW_PLACEMENT_LEFT);

  statusbar = GTR_STATUSBAR (gtr_window_get_statusbar (priv->window));
  priv->context_id = gtr_statusbar_get_context_id (statusbar,
                                                   "Character Description");
}

static void
gtr_charmap_plugin_deactivate (GtrWindowActivatable *activatable)
{
  GtrCharmapPluginPrivate *priv;
  GucharmapChartable *chartable;

  priv = GTR_CHARMAP_PLUGIN (activatable)->priv;

  chartable = gtr_charmap_panel_get_chartable (GTR_CHARMAP_PANEL (priv->charmap));
  on_table_status_message (chartable, NULL, 
                           GTR_CHARMAP_PLUGIN (activatable));

  gtr_window_remove_widget (priv->window, priv->charmap);
}

static void
gtr_charmap_plugin_class_init (GtrCharmapPluginClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gtr_charmap_plugin_dispose;
  object_class->set_property = gtr_charmap_plugin_set_property;
  object_class->get_property = gtr_charmap_plugin_get_property;

  g_object_class_override_property (object_class, PROP_WINDOW, "window");

  g_type_class_add_private (object_class, sizeof (GtrCharmapPluginPrivate));
}

static void
gtr_charmap_plugin_class_finalize (GtrCharmapPluginClass *klass)
{
}

static void
gtr_window_activatable_iface_init (GtrWindowActivatableInterface *iface)
{
  iface->activate = gtr_charmap_plugin_activate;
  iface->deactivate = gtr_charmap_plugin_deactivate;
}

G_MODULE_EXPORT void
peas_register_types (PeasObjectModule *module)
{
  gtr_charmap_plugin_register_type (G_TYPE_MODULE (module));

  peas_object_module_register_extension_type (module,
                                              GTR_TYPE_WINDOW_ACTIVATABLE,
                                              GTR_TYPE_CHARMAP_PLUGIN);
}
