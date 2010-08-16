/*
 * gtr-settings.c
 * This file is part of gtranslator
 *
 * Copyright (C) 2002-2005 - Paolo Maggi
 *               2009 - Ignacio Casal Quinteiro
 *
 * gtranslator is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * gtranslator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gtr; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#include <string.h>
#include <gdl/gdl.h>

#include "gtr-settings.h"
#include "gtr-application.h"
#include "gtr-debug.h"
#include "gtr-view.h"
#include "gtr-window.h"
#include "gtr-dirs.h"
#include "gtr-utils.h"

#define GTR_SETTINGS_SYSTEM_FONT "monospace-font-name"

#define GTR_SETTINGS_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE((object), GTR_TYPE_SETTINGS, GtrSettingsPrivate))

struct _GtrSettingsPrivate
{
  GSettings *interface;
  GSettings *editor;
  GSettings *ui;
  GSettings *files;

  gchar *old_scheme;
};

G_DEFINE_TYPE (GtrSettings, gtr_settings, G_TYPE_SETTINGS)

static void
gtr_settings_finalize (GObject * object)
{
  GtrSettings *gs = GTR_SETTINGS (object);

  g_free (gs->priv->old_scheme);

  G_OBJECT_CLASS (gtr_settings_parent_class)->finalize (object);
}

static void
gtr_settings_dispose (GObject * object)
{
  GtrSettings *gs = GTR_SETTINGS (object);

  if (gs->priv->interface != NULL)
    {
      g_object_unref (gs->priv->interface);
      gs->priv->interface = NULL;
    }

  if (gs->priv->editor != NULL)
    {
      g_object_unref (gs->priv->editor);
      gs->priv->editor = NULL;
    }

  if (gs->priv->ui != NULL)
    {
      g_object_unref (gs->priv->ui);
      gs->priv->ui = NULL;
    }

  G_OBJECT_CLASS (gtr_settings_parent_class)->dispose (object);
}

static void
set_font (GtrSettings * gs, const gchar * font)
{
  GList *views, *l;

  views = gtr_application_get_views (GTR_APP, TRUE, TRUE);

  for (l = views; l != NULL; l = g_list_next (l))
    {
      /* Note: we use def=FALSE to avoid GtrView to query gconf */
      gtr_view_set_font (GTR_VIEW (l->data), FALSE, font);
    }

  g_list_free (views);
}

static void
on_system_font_changed (GSettings * settings,
                        const gchar * key, GtrSettings * gs)
{

  gboolean use_custom_font;
  gchar *font;

  use_custom_font = g_settings_get_boolean (gs->priv->editor,
                                            GTR_SETTINGS_USE_CUSTOM_FONT);
  if (!use_custom_font)
    return;

  font = g_settings_get_string (settings, key);

  set_font (gs, font);

  g_free (font);
}

static void
on_use_custom_font_changed (GSettings * settings,
                            const gchar * key, GtrSettings * gs)
{
  gboolean def;
  gchar *font;

  def = g_settings_get_boolean (settings, key);

  if (!def)
    {
      font = g_settings_get_string (gs->priv->interface,
                                    GTR_SETTINGS_SYSTEM_FONT);
    }
  else
    {
      font = g_settings_get_string (gs->priv->editor,
                                    GTR_SETTINGS_EDITOR_FONT);
    }

  set_font (gs, font);

  g_free (font);
}

static void
on_editor_font_changed (GSettings * settings,
                        const gchar * key, GtrSettings * gs)
{
  gboolean use_custom_font;
  gchar *font;

  use_custom_font = g_settings_get_boolean (gs->priv->editor,
                                            GTR_SETTINGS_USE_CUSTOM_FONT);
  if (!use_custom_font)
    return;

  font = g_settings_get_string (settings, key);

  set_font (gs, font);

  g_free (font);
}

static void
on_auto_save_changed (GSettings * settings,
                      const gchar * key, GtrSettings * gs)
{
  const GList *windows, *l;
  gboolean auto_save;

  auto_save = g_settings_get_boolean (settings, key);

  windows = gtr_application_get_windows (GTR_APP);

  for (l = windows; l != NULL; l = g_list_next (l))
    {
      GList *tabs, *t;

      tabs = gtr_window_get_all_tabs (GTR_WINDOW (l->data));

      for (t = tabs; t != NULL; t = g_list_next (t))
        {
          gtr_tab_set_autosave_enabled (GTR_TAB (t), auto_save);
        }

      g_list_free (tabs);
    }
}

static void
on_auto_save_interval_changed (GSettings * settings,
                               const gchar * key, GtrSettings * gs)
{
  const GList *windows, *l;
  gint auto_save_interval;

  g_settings_get (settings, key, "u", &auto_save_interval);

  windows = gtr_application_get_windows (GTR_APP);

  for (l = windows; l != NULL; l = g_list_next (l))
    {
      GList *tabs, *t;

      tabs = gtr_window_get_all_tabs (GTR_WINDOW (l->data));

      for (t = tabs; t != NULL; t = g_list_next (t))
        {
          gtr_tab_set_autosave_interval (GTR_TAB (t->data), auto_save_interval);
        }

      g_list_free (tabs);
    }
}

static void
on_syntax_highlighting_changed (GSettings * settings,
                                const gchar * key, GtrSettings * gs)
{
  GList *views, *l;
  gboolean enable;

  enable = g_settings_get_boolean (settings, key);

  views = gtr_application_get_views (GTR_APP, TRUE, TRUE);

  for (l = views; l != NULL; l = g_list_next (l))
    {
      GtkSourceBuffer *buf;

      buf = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (l->data)));
      gtk_source_buffer_set_highlight_syntax (buf, enable);
    }

  g_list_free (views);
}

static void
on_visible_whitespace_changed (GSettings * settings,
                               const gchar * key, GtrSettings * gs)
{
  GList *views, *l;
  gboolean enable;

  enable = g_settings_get_boolean (settings, key);

  views = gtr_application_get_views (GTR_APP, TRUE, TRUE);

  for (l = views; l != NULL; l = g_list_next (l))
    {
      gtr_view_enable_visible_whitespace (GTR_VIEW (l->data), enable);
    }

  g_list_free (views);
}

static void
on_spellcheck_changed (GSettings * settings,
                       const gchar * key, GtrSettings * gs)
{
  GList *views, *l;
  gboolean enable;

  enable = g_settings_get_boolean (settings, key);

  views = gtr_application_get_views (GTR_APP, TRUE, TRUE);

  for (l = views; l != NULL; l = g_list_next (l))
    {
      gtr_view_enable_spellcheck (GTR_VIEW (l->data), enable);
    }

  g_list_free (views);
}

static void
on_pane_switcher_style_changed (GSettings * settings, const gchar * key,
                                GtrSettings * gs)
{
  const GList *windows, *l;
  GdlSwitcherStyle style;

  style = g_settings_get_enum (settings, key);

  windows = gtr_application_get_windows (GTR_APP);

  for (l = windows; l != NULL; l = g_list_next (l))
    {
      GdlDockLayout *layout_manager;

      layout_manager = GDL_DOCK_LAYOUT (_gtr_window_get_layout_manager (GTR_WINDOW (l->data)));

      g_object_set (G_OBJECT (layout_manager->master),
                    "switcher-style", style, NULL);
    }
}

static void
on_scheme_changed (GSettings * settings, const gchar * key, GtrSettings * gs)
{
  GList *views, *l;

  views = gtr_application_get_views (GTR_APP, TRUE, TRUE);

  for (l = views; l != NULL; l = g_list_next (l))
    {
      gtr_view_reload_scheme_color (GTR_VIEW (l->data));
    }

  g_list_free (views);
}

static void
gtr_settings_init (GtrSettings * gs)
{
  gs->priv = GTR_SETTINGS_GET_PRIVATE (gs);

  gs->priv->old_scheme = NULL;
}

static void
initialize (GtrSettings * gs)
{
  GSettings *prefs;

  prefs = g_settings_get_child (G_SETTINGS (gs), "preferences");
  gs->priv->files = g_settings_get_child (prefs, "files");
  gs->priv->editor = g_settings_get_child (prefs, "editor");
  gs->priv->ui = g_settings_get_child (prefs, "ui");
  g_object_unref (prefs);

  /* Load settings */
  gs->priv->interface = g_settings_new ("org.gnome.desktop.interface");

  g_signal_connect (gs->priv->interface,
                    "changed::monospace-font-name",
                    G_CALLBACK (on_system_font_changed), gs);

  /* editor changes */
  g_signal_connect (gs->priv->files,
                    "changed::autosave",
                    G_CALLBACK (on_auto_save_changed), gs);
  g_signal_connect (gs->priv->files,
                    "changed::autosave-interval",
                    G_CALLBACK (on_auto_save_interval_changed), gs);
  g_signal_connect (gs->priv->editor,
                    "changed::use-custom-font",
                    G_CALLBACK (on_use_custom_font_changed), gs);
  g_signal_connect (gs->priv->editor,
                    "changed::editor-font",
                    G_CALLBACK (on_editor_font_changed), gs);
  g_signal_connect (gs->priv->editor,
                    "changed::highlight-syntax",
                    G_CALLBACK (on_syntax_highlighting_changed), gs);
  g_signal_connect (gs->priv->editor,
                    "changed::visible-whitespace",
                    G_CALLBACK (on_visible_whitespace_changed), gs);
  g_signal_connect (gs->priv->editor,
                    "changed::spellcheck",
                    G_CALLBACK (on_spellcheck_changed), gs);
  g_signal_connect (gs->priv->ui,
                    "changed::pane-switcher-style",
                    G_CALLBACK (on_pane_switcher_style_changed), gs);
  g_signal_connect (gs->priv->ui,
                    "changed::color-scheme",
                    G_CALLBACK (on_scheme_changed), gs);
}

static void
gtr_settings_class_init (GtrSettingsClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gtr_settings_finalize;
  object_class->dispose = gtr_settings_dispose;

  g_type_class_add_private (object_class, sizeof (GtrSettingsPrivate));
}

GSettings *
gtr_settings_new ()
{
  GtrSettings *settings;

  settings = g_object_new (GTR_TYPE_SETTINGS,
                           "schema", "org.gnome.gtranslator", NULL);

  initialize (settings);

  return G_SETTINGS (settings);
}

gchar *
gtr_settings_get_system_font (GtrSettings * gs)
{
  gchar *system_font;

  g_return_val_if_fail (GTR_IS_SETTINGS (gs), NULL);

  system_font = g_settings_get_string (gs->priv->interface,
                                       "monospace-font-name");

  return system_font;
}

GSList *
gtr_settings_get_list (GSettings * settings, const gchar * key)
{
  GSList *list = NULL;
  gchar **values;
  gsize i;

  g_return_val_if_fail (G_IS_SETTINGS (settings), NULL);
  g_return_val_if_fail (key != NULL, NULL);

  values = g_settings_get_strv (settings, key);
  i = 0;

  while (values[i] != NULL)
    {
      list = g_slist_prepend (list, values[i]);
      i++;
    }

  g_free (values);

  return g_slist_reverse (list);
}

void
gtr_settings_set_list (GSettings * settings,
                       const gchar * key, const GSList * list)
{
  gchar **values = NULL;
  const GSList *l;

  g_return_if_fail (G_IS_SETTINGS (settings));
  g_return_if_fail (key != NULL);

  if (list != NULL)
    {
      gint i, len;

      len = g_slist_length ((GSList *) list);
      values = g_new (gchar *, len + 1);

      for (l = list, i = 0; l != NULL; l = g_slist_next (l), i++)
        {
          values[i] = l->data;
        }
      values[i] = NULL;
    }

  g_settings_set_strv (settings, key, (const gchar * const *) values);
  g_free (values);
}

/* ex:ts=8:noet: */
