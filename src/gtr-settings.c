/*
 * gtr-settings.c
 * This file is part of gtranslator
 *
 * Copyright (C) 2002-2005  Paolo Maggi
 *               2009  Ignacio Casal Quinteiro
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <string.h>
#include <gtksourceview/gtksource.h>

#include "gtr-settings.h"
#include "gtr-application.h"
#include "gtr-view.h"
#include "gtr-window.h"
#include "gtr-dirs.h"
#include "gtr-utils.h"

typedef struct
{
  GSettings *interface;
  GSettings *editor;
  GSettings *ui;
  GSettings *files;

  gchar *old_scheme;
} GtrSettingsPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GtrSettings, gtr_settings, G_TYPE_OBJECT)

static void
gtr_settings_finalize (GObject * object)
{
  GtrSettings *gs = GTR_SETTINGS (object);
  GtrSettingsPrivate *priv = gtr_settings_get_instance_private (gs);

  g_free (priv->old_scheme);

  G_OBJECT_CLASS (gtr_settings_parent_class)->finalize (object);
}

static void
gtr_settings_dispose (GObject * object)
{
  GtrSettings *gs = GTR_SETTINGS (object);
  GtrSettingsPrivate *priv = gtr_settings_get_instance_private (gs);

  g_clear_object (&priv->interface);
  g_clear_object (&priv->files);
  g_clear_object (&priv->editor);
  g_clear_object (&priv->ui);

  G_OBJECT_CLASS (gtr_settings_parent_class)->dispose (object);
}

static void
on_auto_save_changed (GSettings * settings,
                      const gchar * key, GtrSettings * gs)
{
  const GList *windows, *l;
  gboolean auto_save;

  auto_save = g_settings_get_boolean (settings, key);

  windows = gtk_application_get_windows (GTK_APPLICATION (GTR_APP));

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

  windows = gtk_application_get_windows (GTK_APPLICATION (GTR_APP));

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
on_font_changed (GSettings * settings,
                 const gchar * key, GtrSettings * gs)
{
  GList *views, *l;
  g_autofree char *font = NULL;

  font = g_settings_get_string (settings, GTR_SETTINGS_FONT);

  views = gtr_application_get_views (GTR_APP, TRUE, TRUE);

  for (l = views; l != NULL; l = g_list_next (l))
    {
      gtr_view_set_font (GTR_VIEW (l->data), font);
    }

  g_list_free (views);
}

static void
gtr_settings_init (GtrSettings * gs)
{
  GtrSettingsPrivate *priv = gtr_settings_get_instance_private (gs);

  priv->old_scheme = NULL;

  priv->files = g_settings_new ("org.gnome.gtranslator.preferences.files");
  priv->editor = g_settings_new ("org.gnome.gtranslator.preferences.editor");
  priv->ui = g_settings_new ("org.gnome.gtranslator.preferences.ui");

  /* Load settings */
  priv->interface = g_settings_new ("org.gnome.desktop.interface");

  /* editor changes */
  g_signal_connect (priv->files,
                    "changed::autosave",
                    G_CALLBACK (on_auto_save_changed), gs);
  g_signal_connect (priv->files,
                    "changed::autosave-interval",
                    G_CALLBACK (on_auto_save_interval_changed), gs);
  g_signal_connect (priv->editor,
                    "changed::highlight-syntax",
                    G_CALLBACK (on_syntax_highlighting_changed), gs);
  g_signal_connect (priv->editor, "changed::visible-whitespace",
                    G_CALLBACK (on_visible_whitespace_changed), gs);
  g_signal_connect (priv->editor,
                    "changed::font",
                    G_CALLBACK (on_font_changed), gs);
}

static void
gtr_settings_class_init (GtrSettingsClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gtr_settings_finalize;
  object_class->dispose = gtr_settings_dispose;
}

GSettings *
gtr_settings_new (void)
{
  return g_object_new (GTR_TYPE_SETTINGS, NULL);
}

/* ex:ts=8:noet: */
