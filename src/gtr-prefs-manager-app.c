/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * prefs-manager-app.c
 * This file is part of gtranslator
 *
 * Copyright (C) 2002-2005  Paolo Maggi 
 *               2007       Ignacio Casal Quinteiro
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, 
 * Boston, MA 02111-1307, USA. 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#ifdef G_OS_WIN32
#include <gdl/libgdltypebuiltins.h>
#else
#include <gdl/gdl.h>
#endif

#include "gtr-dirs.h"
#include "gtr-prefs-manager.h"
#include "gtr-prefs-manager-private.h"
#include "gtr-prefs-manager-app.h"
#include "gtr-application.h"
#include "gtr-utils.h"
#include "gtr-view.h"
#include "gtr-window.h"

static void gtr_prefs_manager_editor_font_changed (GConfClient *
							   client,
							   guint cnxn_id,
							   GConfEntry * entry,
							   gpointer
							   user_data);

static void gtr_prefs_manager_spellcheck_changed (GConfClient *
							  client,
							  guint cnxn_id,
							  GConfEntry * entry,
							  gpointer user_data);

static void gtr_prefs_manager_highlight_changed (GConfClient * client,
							 guint cnxn_id,
							 GConfEntry * entry,
							 gpointer user_data);

static void gtr_prefs_manager_visible_whitespace_changed (GConfClient
								  * client,
								  guint
								  cnxn_id,
								  GConfEntry *
								  entry,
								  gpointer
								  user_data);

static void gtr_prefs_manager_gdl_style_changed (GConfClient * client,
							 guint cnxn_id,
							 GConfEntry * entry,
							 gpointer user_data);

static void gtr_prefs_manager_autosave_changed (GConfClient * client,
							guint cnxn_id,
							GConfEntry * entry,
							gpointer user_data);

static void gtr_prefs_manager_scheme_color_changed (GConfClient *
							    client,
							    guint cnxn_id,
							    GConfEntry *
							    entry,
							    gpointer
							    user_data);

/* GUI state is serialized to a .desktop file, not in gconf */

#define GTR_STATE_DEFAULT_WINDOW_STATE		0
#define GTR_STATE_DEFAULT_WINDOW_WIDTH		775
#define GTR_STATE_DEFAULT_WINDOW_HEIGHT		500
#define GTR_STATE_DEFAULT_CONTENT_PANE_POS	325
#define GTR_STATE_DEFAULT_COMMENT_PANE_POS	525

#define GTR_STATE_FILE_NAME "gtranslator.config"

#define GTR_STATE_WINDOW_GROUP "window"
#define GTR_STATE_WINDOW_STATE "state"
#define GTR_STATE_WINDOW_HEIGHT "height"
#define GTR_STATE_WINDOW_WIDTH "width"
#define GTR_STATE_CONTENT_PANE_POS "content_pane_pos"
#define GTR_STATE_COMMENT_PANE_POS "comment_pane_pos"

static gint window_state = -1;
static gint window_height = -1;
static gint window_width = -1;
static gint content_pane_pos = -1;
static gint comment_pane_pos = -1;

static GKeyFile *
get_gtr_state_file ()
{
  static GKeyFile *state_file = NULL;

  if (state_file == NULL)
    {
      gchar *config_folder;
      gchar *path;
      GError *err = NULL;

      state_file = g_key_file_new ();

      config_folder = gtr_dirs_get_user_config_dir ();
      path = g_build_filename (config_folder, GTR_STATE_FILE_NAME, NULL);
      g_free (config_folder);

      if (!g_key_file_load_from_file (state_file,
				      path, G_KEY_FILE_NONE, &err))
	{
	  if (err->domain != G_FILE_ERROR || err->code != G_FILE_ERROR_NOENT)
	    {
	      g_warning ("Could not load gtranslator state file: %s\n",
			 err->message);
	    }
	  g_error_free (err);
	}
      g_free (path);
    }

  g_return_val_if_fail (state_file != NULL, NULL);
  return state_file;
}

static void
gtr_state_get_int (const gchar * group,
			   const gchar * key, gint defval, gint * result)
{
  GKeyFile *state_file;
  gint res;
  GError *err = NULL;

  state_file = get_gtr_state_file ();
  res = g_key_file_get_integer (state_file, group, key, &err);

  if (err != NULL)
    {
      if ((err->domain != G_KEY_FILE_ERROR) ||
	  ((err->code != G_KEY_FILE_ERROR_GROUP_NOT_FOUND &&
	    err->code != G_KEY_FILE_ERROR_KEY_NOT_FOUND)))
	{
	  g_warning ("Could not get state value %s::%s : %s\n",
		     group, key, err->message);
	}

      if (defval > 0)
	*result = defval;

      g_error_free (err);
    }
  else
    {
      *result = res;
    }
}

static void
gtr_state_set_int (const gchar * group, const gchar * key, gint value)
{
  GKeyFile *state_file;

  state_file = get_gtr_state_file ();
  g_key_file_set_integer (state_file, group, key, value);
}

static gboolean
gtr_state_file_sync ()
{
  GKeyFile *state_file;
  gchar *config_folder;
  gchar *path;
  gchar *content;
  gsize length;
  GError *err = NULL;
  gboolean ret = FALSE;

  state_file = get_gtr_state_file ();
  g_return_val_if_fail (state_file != NULL, FALSE);

  config_folder = gtr_dirs_get_user_config_dir ();
  path = g_build_filename (config_folder, GTR_STATE_FILE_NAME, NULL);
  g_free (config_folder);

  content = g_key_file_to_data (state_file, &length, &err);

  if (err != NULL)
    {
      g_warning ("Could not get data from state file: %s\n", err->message);
      goto out;
    }

  if ((content != NULL) &&
      (!g_file_set_contents (path, content, length, &err)))
    {
      g_warning ("Could not write gtr state file: %s\n",
		 err->message);
      goto out;
    }

  ret = TRUE;

out:
  if (err != NULL)
    g_error_free (err);

  g_free (content);
  g_free (path);

  return TRUE;
}

/* Window state */
gint
gtr_prefs_manager_get_window_state (void)
{
  if (window_state == -1)
    {
      gtr_state_get_int (GTR_STATE_WINDOW_GROUP,
				 GTR_STATE_WINDOW_STATE,
				 GTR_STATE_DEFAULT_WINDOW_STATE,
				 &window_state);
    }

  return window_state;
}

void
gtr_prefs_manager_set_window_state (gint ws)
{
  g_return_if_fail (ws > -1);

  window_state = ws;

  gtr_state_set_int (GTR_STATE_WINDOW_GROUP,
			     GTR_STATE_WINDOW_STATE, ws);
}

gboolean
gtr_prefs_manager_window_state_can_set (void)
{
  return TRUE;
}

/* Window size */
void
gtr_prefs_manager_get_window_size (gint * width, gint * height)
{
  g_return_if_fail (width != NULL && height != NULL);

  if (window_width == -1)
    {
      gtr_state_get_int (GTR_STATE_WINDOW_GROUP,
				 GTR_STATE_WINDOW_WIDTH,
				 GTR_STATE_DEFAULT_WINDOW_WIDTH,
				 &window_width);
    }

  if (window_height == -1)
    {
      gtr_state_get_int (GTR_STATE_WINDOW_GROUP,
				 GTR_STATE_WINDOW_HEIGHT,
				 GTR_STATE_DEFAULT_WINDOW_HEIGHT,
				 &window_height);
    }

  *width = window_width;
  *height = window_height;
}

void
gtr_prefs_manager_get_default_window_size (gint * width,
						   gint * height)
{
  g_return_if_fail (width != NULL && height != NULL);

  *width = GTR_STATE_DEFAULT_WINDOW_WIDTH;
  *height = GTR_STATE_DEFAULT_WINDOW_HEIGHT;
}

void
gtr_prefs_manager_set_window_size (gint width, gint height)
{
  g_return_if_fail (width > -1 && height > -1);

  window_width = width;
  window_height = height;

  gtr_state_set_int (GTR_STATE_WINDOW_GROUP,
			     GTR_STATE_WINDOW_WIDTH, width);
  gtr_state_set_int (GTR_STATE_WINDOW_GROUP,
			     GTR_STATE_WINDOW_HEIGHT, height);
}

gboolean
gtr_prefs_manager_window_size_can_set (void)
{
  return TRUE;
}

/* Content pane */
gint
gtr_prefs_manager_get_content_pane_pos (void)
{
  if (content_pane_pos == -1)
    {
      gtr_state_get_int (GTR_STATE_WINDOW_GROUP,
				 GTR_STATE_CONTENT_PANE_POS,
				 GTR_STATE_DEFAULT_CONTENT_PANE_POS,
				 &content_pane_pos);
    }

  return content_pane_pos;
}

gint
gtr_prefs_manager_get_default_content_pane_pos (void)
{
  return GTR_STATE_DEFAULT_CONTENT_PANE_POS;
}

void
gtr_prefs_manager_set_content_pane_pos (gint new_pane_pos)
{
  g_return_if_fail (new_pane_pos > -1);

  if (content_pane_pos == new_pane_pos)
    return;

  content_pane_pos = new_pane_pos;
  gtr_state_set_int (GTR_STATE_WINDOW_GROUP,
			     GTR_STATE_CONTENT_PANE_POS, new_pane_pos);
}

/* Comment pane */
gint
gtr_prefs_manager_get_comment_pane_pos (void)
{
  if (comment_pane_pos == -1)
    {
      gtr_state_get_int (GTR_STATE_WINDOW_GROUP,
				 GTR_STATE_COMMENT_PANE_POS,
				 GTR_STATE_DEFAULT_COMMENT_PANE_POS,
				 &comment_pane_pos);
    }

  return comment_pane_pos;
}

gint
gtr_prefs_manager_get_default_comment_pane_pos (void)
{
  return GTR_STATE_DEFAULT_COMMENT_PANE_POS;
}

void
gtr_prefs_manager_set_comment_pane_pos (gint new_pane_pos)
{
  g_return_if_fail (new_pane_pos > -1);

  if (comment_pane_pos == new_pane_pos)
    return;

  comment_pane_pos = new_pane_pos;
  gtr_state_set_int (GTR_STATE_WINDOW_GROUP,
			     GTR_STATE_COMMENT_PANE_POS, new_pane_pos);
}

/* Normal prefs are stored in GConf */

gboolean
gtr_prefs_manager_app_init (void)
{

  g_return_val_if_fail (gtr_prefs_manager == NULL, FALSE);

  gtr_prefs_manager_init ();

  if (gtr_prefs_manager != NULL)
    {
      /* TODO: notify, add dirs */
      gconf_client_add_dir (gtr_prefs_manager->gconf_client,
			    GPM_PREFS_DIR,
			    GCONF_CLIENT_PRELOAD_RECURSIVE, NULL);

      gconf_client_notify_add (gtr_prefs_manager->gconf_client,
			       GPM_EDITOR_FONT,
			       gtr_prefs_manager_editor_font_changed,
			       NULL, NULL, NULL);

      gconf_client_notify_add (gtr_prefs_manager->gconf_client,
			       GPM_SPELLCHECK,
			       gtr_prefs_manager_spellcheck_changed,
			       NULL, NULL, NULL);

      gconf_client_notify_add (gtr_prefs_manager->gconf_client,
			       GPM_HIGHLIGHT_SYNTAX,
			       gtr_prefs_manager_highlight_changed,
			       NULL, NULL, NULL);

      gconf_client_notify_add (gtr_prefs_manager->gconf_client,
			       GPM_VISIBLE_WHITESPACE,
			       gtr_prefs_manager_visible_whitespace_changed,
			       NULL, NULL, NULL);

      gconf_client_notify_add (gtr_prefs_manager->gconf_client,
			       GPM_PANE_SWITCHER_STYLE,
			       gtr_prefs_manager_gdl_style_changed,
			       NULL, NULL, NULL);

      gconf_client_notify_add (gtr_prefs_manager->gconf_client,
			       GPM_AUTOSAVE,
			       gtr_prefs_manager_autosave_changed,
			       NULL, NULL, NULL);

      gconf_client_notify_add (gtr_prefs_manager->gconf_client,
			       GPM_COLOR_SCHEME,
			       gtr_prefs_manager_scheme_color_changed,
			       NULL, NULL, NULL);
    }

  return gtr_prefs_manager != NULL;
}

/* This function must be called before exiting gtr */
void
gtr_prefs_manager_app_shutdown ()
{
  gtr_prefs_manager_shutdown ();

  gtr_state_file_sync ();
}


static void
gtr_prefs_manager_editor_font_changed (GConfClient * client,
					       guint cnxn_id,
					       GConfEntry * entry,
					       gpointer user_data)
{
  GList *views;
  GList *l;
  gchar *font = NULL;
  gboolean def = TRUE;

  g_return_if_fail (entry->key != NULL);
  g_return_if_fail (entry->value != NULL);

  if (strcmp (entry->key, GPM_USE_CUSTOM_FONT) == 0)
    {
      def = gconf_value_get_bool (entry->value);

      if (!def)
	font = g_strdup ("Sans 10");	// Fix to use system font
      else
	font = g_strdup (gtr_prefs_manager_get_editor_font ());
    }
  else if (strcmp (entry->key, GPM_EDITOR_FONT) == 0)
    {
      font = g_strdup (gconf_value_get_string (entry->value));

      def = gtr_prefs_manager_get_use_custom_font ();
    }
  else
    return;

  g_return_if_fail (font != NULL);

  views = gtr_application_get_views (GTR_APP, TRUE, TRUE);
  l = views;

  while (l != NULL)
    {
      /* Note: we use def=FALSE to avoid GtrView to query gconf */
      gtr_view_set_font (GTR_VIEW (l->data), FALSE, font);
      l = l->next;
    }

  g_list_free (views);
  g_free (font);
}


static void
gtr_prefs_manager_spellcheck_changed (GConfClient * client,
					      guint cnxn_id,
					      GConfEntry * entry,
					      gpointer user_data)
{
  GList *l;
  GList *views;

  g_return_if_fail (entry->key != NULL);
  g_return_if_fail (entry->value != NULL);

  l = views = gtr_application_get_views (GTR_APP, FALSE, TRUE);

  while (l != NULL)
    {
      gtr_view_enable_spellcheck (GTR_VIEW (l->data),
					  gtr_prefs_manager_get_spellcheck
					  ());
      l = l->next;
    }
  g_list_free (views);
}


static void
gtr_prefs_manager_highlight_changed (GConfClient * client,
					     guint cnxn_id,
					     GConfEntry * entry,
					     gpointer user_data)
{
  g_return_if_fail (entry->key != NULL);
  g_return_if_fail (entry->value != NULL);

  if (strcmp (entry->key, GPM_HIGHLIGHT_SYNTAX) == 0)
    {
      gboolean enable;
      GList *views;
      GList *l;
      GtkSourceBuffer *buf;

      enable = gconf_value_get_bool (entry->value);

      views = gtr_application_get_views (GTR_APP, TRUE, TRUE);
      l = views;

      while (l != NULL)
	{
	  buf =
	    GTK_SOURCE_BUFFER (gtk_text_view_get_buffer
			       (GTK_TEXT_VIEW (l->data)));
	  g_return_if_fail (GTK_IS_SOURCE_BUFFER (buf));

	  gtk_source_buffer_set_highlight_syntax (buf, enable);

	  l = l->next;
	}

      g_list_free (views);
    }
}

static void
gtr_prefs_manager_visible_whitespace_changed (GConfClient * client,
						      guint cnxn_id,
						      GConfEntry * entry,
						      gpointer user_data)
{
  g_return_if_fail (entry->key != NULL);
  g_return_if_fail (entry->value != NULL);

  if (strcmp (entry->key, GPM_VISIBLE_WHITESPACE) == 0)
    {
      gboolean enable;
      GList *views;
      GList *l;

      enable = gconf_value_get_bool (entry->value);

      views = gtr_application_get_views (GTR_APP, TRUE, TRUE);
      l = views;

      while (l != NULL)
	{
	  gtr_view_enable_visible_whitespace (GTR_VIEW (l->data),
						      enable);

	  l = l->next;
	}

      g_list_free (views);
    }
}

static void
gtr_prefs_manager_gdl_style_changed (GConfClient * client,
					     guint cnxn_id,
					     GConfEntry * entry,
					     gpointer user_data)
{
  GtrWindow *window;
  GdlSwitcherStyle style;
  GdlDockLayout *layout_manager;

  window = gtr_application_get_active_window (GTR_APP);

  style = gtr_prefs_manager_get_pane_switcher_style ();

  layout_manager =
    GDL_DOCK_LAYOUT (_gtr_window_get_layout_manager (window));

  g_object_set (G_OBJECT (layout_manager->master),
		"switcher-style", style, NULL);
}

static void
gtr_prefs_manager_autosave_changed (GConfClient * client,
					    guint cnxn_id,
					    GConfEntry * entry,
					    gpointer user_data)
{
  GList *tabs;
  GList *l;
  GtrWindow *window;

  g_return_if_fail (entry->key != NULL);
  g_return_if_fail (entry->value != NULL);

  window = gtr_application_get_active_window (GTR_APP);

  if (strcmp (entry->key, GPM_AUTOSAVE) == 0)
    {
      gboolean autosave;

      autosave = gconf_value_get_bool (entry->value);

      tabs = gtr_window_get_all_tabs (window);

      for (l = tabs; l != NULL; l = g_list_next (l))
	{
	  GtrTab *tab = GTR_TAB (l->data);

	  gtr_tab_set_autosave_enabled (tab, autosave);
	}

      g_list_free (tabs);
    }
  else if (strcmp (entry->key, GPM_AUTOSAVE_INTERVAL) == 0)
    {
      gint autosave_interval;

      autosave_interval = gconf_value_get_int (entry->value);

      if (autosave_interval <= 0)
	autosave_interval = 1;

      tabs = gtr_window_get_all_tabs (window);

      for (l = tabs; l != NULL; l = g_list_next (l))
	{
	  GtrTab *tab = GTR_TAB (l->data);

	  gtr_tab_set_autosave_interval (tab, autosave_interval);
	}

      g_list_free (tabs);
    }
}

static void
gtr_prefs_manager_scheme_color_changed (GConfClient * client,
						guint cnxn_id,
						GConfEntry * entry,
						gpointer user_data)
{
  GList *views, *l;

  views = gtr_application_get_views (GTR_APP, TRUE, TRUE);

  for (l = views; l != NULL; l = g_list_next (l))
    {
      gtr_view_reload_scheme_color (GTR_VIEW (l->data));
    }
}
