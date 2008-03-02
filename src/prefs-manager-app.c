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
#include <gdl/gdl-dock-layout.h>
#include <gdl/gdl-switcher.h>

#include "prefs-manager.h"
#include "prefs-manager-private.h"
#include "prefs-manager-app.h"
#include "application.h"
#include "view.h"
#include "window.h"

static void gtranslator_prefs_manager_editor_font_changed (GConfClient *client,
							 guint        cnxn_id,
							 GConfEntry  *entry,
							 gpointer     user_data);

static void gtranslator_prefs_manager_spellcheck_changed (GConfClient *client,
								  guint        cnxn_id, 
								  GConfEntry  *entry, 
								  gpointer     user_data);

static void gtranslator_prefs_manager_highlight_changed(GConfClient *client,
							       guint        cnxn_id, 
							       GConfEntry  *entry, 
							       gpointer     user_data);

static void gtranslator_prefs_manager_visible_whitespace_changed(GConfClient *client,
							       guint        cnxn_id, 
							       GConfEntry  *entry, 
							       gpointer     user_data);

static void gtranslator_prefs_manager_gdl_style_changed (GConfClient* client, guint id,
							 GConfEntry* entry,
							 gpointer user_data);

/*
static void gtranslator_prefs_manager_system_font_changed	(GConfClient *client,
							 guint        cnxn_id,
							 GConfEntry  *entry,
							 gpointer     user_data);



static void gtranslator_prefs_manager_auto_save_changed	(GConfClient *client,
							 guint        cnxn_id,
							 GConfEntry  *entry,
							 gpointer     user_data);*/


/* GUI state is serialized to a .desktop file, not in gconf */

#define GTR_STATE_DEFAULT_WINDOW_STATE		0
#define GTR_STATE_DEFAULT_WINDOW_WIDTH		775
#define GTR_STATE_DEFAULT_WINDOW_HEIGHT		500
#define GTR_STATE_DEFAULT_CONTENT_PANE_POS	325
#define GTR_STATE_DEFAULT_COMMENT_PANE_POS	525

#define GTR_STATE_FILE_LOCATION ".config/gtranslator"

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
get_gtranslator_state_file ()
{
	static GKeyFile *state_file = NULL;

	if (state_file == NULL)
	{
		const gchar *home;
		gchar *path;
		GError *err = NULL;

		state_file = g_key_file_new ();

		home = g_get_home_dir ();
		if (home == NULL)
		{
			g_warning ("Could not get HOME directory\n");
			goto out;
		}

		path = g_build_filename (home,
					 GTR_STATE_FILE_LOCATION,
					 NULL);

		if (!g_key_file_load_from_file (state_file,
						path,
						G_KEY_FILE_NONE,
						&err))
		{
			if (err->domain != G_FILE_ERROR ||
			    err->code != G_FILE_ERROR_NOENT)
			{
				g_warning ("Could not load gtranslator state file: %s\n",
					   err->message);
			}
			g_error_free (err);
		}
		g_free (path);
	}

 out:
	g_return_val_if_fail (state_file != NULL, NULL);
	return state_file;
}

static void
gtranslator_state_get_int (const gchar *group,
		     const gchar *key,
		     gint         defval,
		     gint        *result)
{
	GKeyFile *state_file;
	gint res;
	GError *err = NULL;

	state_file = get_gtranslator_state_file ();
	res = g_key_file_get_integer (state_file,
				      group,
				      key,
				      &err);

	if (err != NULL)
	{
		if ((err->domain != G_KEY_FILE_ERROR) ||
		    ((err->code != G_KEY_FILE_ERROR_GROUP_NOT_FOUND &&
		      err->code != G_KEY_FILE_ERROR_KEY_NOT_FOUND)))
		{
			g_warning ("Could not get state value %s::%s : %s\n",
				   group,
				   key,
				   err->message);
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
gtranslator_state_set_int (const gchar *group,
		     const gchar *key,
		     gint         value)
{
	GKeyFile *state_file;

	state_file = get_gtranslator_state_file ();
	g_key_file_set_integer (state_file,
				group,
				key,
				value);
}

static gboolean
gtranslator_state_file_sync ()
{
	GKeyFile *state_file;
	const gchar *home;
	gchar *path;
	gchar *content;
	gsize length;
	GError *err = NULL;
	gboolean ret = FALSE;

	state_file = get_gtranslator_state_file ();
	g_return_val_if_fail (state_file != NULL, FALSE);

	home = g_get_home_dir ();
	if (home == NULL)
	{
		g_warning ("Could not get HOME directory\n");
		return ret;
	}

	path = g_build_filename (home,
				 GTR_STATE_FILE_LOCATION,
				 NULL);

	content = g_key_file_to_data (state_file,
				      &length,
				      &err);

	if (err != NULL)
	{
		g_warning ("Could not get data from state file: %s\n",
			   err->message);
		goto out;
	}

	if ((content != NULL) &&
	    (!g_file_set_contents (path,
				   content,
				   length,
				   &err)))
	{
		g_warning ("Could not write gtranslator state file: %s\n",
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
gtranslator_prefs_manager_get_window_state (void)
{
	if (window_state == -1)
	{
		gtranslator_state_get_int (GTR_STATE_WINDOW_GROUP,
					   GTR_STATE_WINDOW_STATE,
					   GTR_STATE_DEFAULT_WINDOW_STATE,
					   &window_state);
	}

	return window_state;
}
			
void
gtranslator_prefs_manager_set_window_state (gint ws)
{
	g_return_if_fail (ws > -1);
	
	window_state = ws;

	gtranslator_state_set_int (GTR_STATE_WINDOW_GROUP,
				   GTR_STATE_WINDOW_STATE,
				   ws);
}

gboolean
gtranslator_prefs_manager_window_state_can_set (void)
{
	return TRUE;
}

/* Window size */
void
gtranslator_prefs_manager_get_window_size (gint *width, gint *height)
{
	g_return_if_fail (width != NULL && height != NULL);

	if (window_width == -1)
	{
		gtranslator_state_get_int (GTR_STATE_WINDOW_GROUP,
				     GTR_STATE_WINDOW_WIDTH,
				     GTR_STATE_DEFAULT_WINDOW_WIDTH,
				     &window_width);
	}

	if (window_height == -1)
	{
		gtranslator_state_get_int (GTR_STATE_WINDOW_GROUP,
				     GTR_STATE_WINDOW_HEIGHT,
				     GTR_STATE_DEFAULT_WINDOW_HEIGHT,
				     &window_height);
	}

	*width = window_width;
	*height = window_height;
}

void
gtranslator_prefs_manager_get_default_window_size (gint *width, gint *height)
{
	g_return_if_fail (width != NULL && height != NULL);

	*width = GTR_STATE_DEFAULT_WINDOW_WIDTH;
	*height = GTR_STATE_DEFAULT_WINDOW_HEIGHT;
}

void
gtranslator_prefs_manager_set_window_size (gint width, gint height)
{
	g_return_if_fail (width > -1 && height > -1);
	
	window_width = width;
	window_height = height;

	gtranslator_state_set_int (GTR_STATE_WINDOW_GROUP,
			     GTR_STATE_WINDOW_WIDTH,
			     width);
	gtranslator_state_set_int (GTR_STATE_WINDOW_GROUP,
			     GTR_STATE_WINDOW_HEIGHT,
			     height);
}

gboolean 
gtranslator_prefs_manager_window_size_can_set (void)
{
	return TRUE;
}

/* Content pane */
gint
gtranslator_prefs_manager_get_content_pane_pos (void)
{
	if (content_pane_pos == -1)
	{
		gtranslator_state_get_int (GTR_STATE_WINDOW_GROUP,
					   GTR_STATE_CONTENT_PANE_POS,
					   GTR_STATE_DEFAULT_CONTENT_PANE_POS,
					   &content_pane_pos);
	}

	return content_pane_pos;
}

gint
gtranslator_prefs_manager_get_default_content_pane_pos (void)
{
	return GTR_STATE_DEFAULT_CONTENT_PANE_POS;
}

void
gtranslator_prefs_manager_set_content_pane_pos (gint new_pane_pos)
{
	g_return_if_fail (new_pane_pos > -1);

	if (content_pane_pos == new_pane_pos)
		return;

	content_pane_pos = new_pane_pos;
	gtranslator_state_set_int (GTR_STATE_WINDOW_GROUP,
				   GTR_STATE_CONTENT_PANE_POS,
				   new_pane_pos);
}

/* Comment pane */
gint
gtranslator_prefs_manager_get_comment_pane_pos (void)
{
	if (comment_pane_pos == -1)
	{
		gtranslator_state_get_int (GTR_STATE_WINDOW_GROUP,
					   GTR_STATE_COMMENT_PANE_POS,
					   GTR_STATE_DEFAULT_COMMENT_PANE_POS,
					   &comment_pane_pos);
	}

	return comment_pane_pos;
}

gint
gtranslator_prefs_manager_get_default_comment_pane_pos (void)
{
	return GTR_STATE_DEFAULT_COMMENT_PANE_POS;
}

void
gtranslator_prefs_manager_set_comment_pane_pos (gint new_pane_pos)
{
	g_return_if_fail (new_pane_pos > -1);

	if (comment_pane_pos == new_pane_pos)
		return;

	comment_pane_pos = new_pane_pos;
	gtranslator_state_set_int (GTR_STATE_WINDOW_GROUP,
				   GTR_STATE_COMMENT_PANE_POS,
				   new_pane_pos);
}

/* Normal prefs are stored in GConf */

gboolean
gtranslator_prefs_manager_app_init (void)
{

	g_return_val_if_fail (gtranslator_prefs_manager == NULL, FALSE);

	gtranslator_prefs_manager_init ();

	if (gtranslator_prefs_manager != NULL)
	{
		/* TODO: notify, add dirs */
		gconf_client_add_dir (gtranslator_prefs_manager->gconf_client,
				      GPM_PREFS_DIR,
				      GCONF_CLIENT_PRELOAD_RECURSIVE,
				      NULL);
		
		gconf_client_notify_add (gtranslator_prefs_manager->gconf_client,
				GPM_FONT_DIR,
				gtranslator_prefs_manager_editor_font_changed,
				NULL, NULL, NULL);
		
		gconf_client_notify_add (gtranslator_prefs_manager->gconf_client,
				GPM_SPELLCHECK,
				gtranslator_prefs_manager_spellcheck_changed,
				NULL, NULL, NULL);
		
		gconf_client_notify_add (gtranslator_prefs_manager->gconf_client,
				GPM_HIGHLIGHT,
				gtranslator_prefs_manager_highlight_changed,
				NULL, NULL, NULL);
		
		gconf_client_notify_add (gtranslator_prefs_manager->gconf_client,
				GPM_VISIBLE_WHITESPACE,
				gtranslator_prefs_manager_visible_whitespace_changed,
				NULL, NULL, NULL);
		
		gconf_client_notify_add (gtranslator_prefs_manager->gconf_client,
				GPM_GDL_STYLE,
				gtranslator_prefs_manager_gdl_style_changed,
				NULL, NULL, NULL);
		
	}

	return gtranslator_prefs_manager != NULL;	
}

/* This function must be called before exiting gtranslator */
void
gtranslator_prefs_manager_app_shutdown ()
{
	gtranslator_prefs_manager_shutdown ();

	gtranslator_state_file_sync ();
}


static void 
gtranslator_prefs_manager_editor_font_changed (GConfClient *client,
					       guint        cnxn_id, 
					       GConfEntry  *entry, 
					       gpointer     user_data)
{
	GList *views;
	GList *l;
	gchar *font = NULL;
	gboolean def = TRUE;
	
	g_return_if_fail (entry->key != NULL);
	g_return_if_fail (entry->value != NULL);

	if (strcmp (entry->key, GPM_USE_CUSTOM_FONT) == 0)
	{
		if (entry->value->type == GCONF_VALUE_BOOL)
			def = gconf_value_get_bool (entry->value);
		else
			def = !GPM_DEFAULT_USE_CUSTOM_FONT;
		
		if (!def)
			font = g_strdup(GPM_DEFAULT_EDITOR_FONT);
		else
			font = g_strdup(gtranslator_prefs_manager_get_editor_font ());
	}
	else if (strcmp (entry->key, GPM_EDITOR_FONT) == 0)
	{
		if (entry->value->type == GCONF_VALUE_STRING)
			font = g_strdup (gconf_value_get_string (entry->value));
		else
			font = g_strdup (GPM_DEFAULT_EDITOR_FONT);
				
		def = gtranslator_prefs_manager_get_use_custom_font ();
	}
	else
		return;

	g_return_if_fail (font != NULL);
	
	views = gtranslator_application_get_views (GTR_APP, TRUE, TRUE);
	l = views;

	while (l != NULL)
	{
		/* Note: we use def=FALSE to avoid GtranslatorView to query gconf */
		gtranslator_view_set_font (GTR_VIEW (l->data), FALSE,  font);
		l = l->next;
	}

	g_list_free (views);
	g_free (font);
}


static void 
gtranslator_prefs_manager_spellcheck_changed (GConfClient *client,
					      guint        cnxn_id, 
					      GConfEntry  *entry, 
					      gpointer     user_data)
{
	GList *l;
	GList *views;
	
	g_return_if_fail (entry->key != NULL);
	g_return_if_fail (entry->value != NULL);
	
	l = views = gtranslator_application_get_views(GTR_APP, FALSE, TRUE);
	
	while(l != NULL)
	{
		gtranslator_view_enable_spellcheck(GTR_VIEW(l->data),
						   gtranslator_prefs_manager_get_spellcheck());
		l = l->next;
	}
	g_list_free(views);
}


static void
gtranslator_prefs_manager_highlight_changed (GConfClient *client,
					     guint        cnxn_id,
					     GConfEntry  *entry,
					     gpointer     user_data)
{
	g_return_if_fail (entry->key != NULL);
	g_return_if_fail (entry->value != NULL);

	if (strcmp (entry->key, GPM_HIGHLIGHT) == 0)
	{
		gboolean enable;
		GList *views;
		GList *l;
		GtkSourceBuffer *buf;

		if (entry->value->type == GCONF_VALUE_BOOL)
			enable = gconf_value_get_bool (entry->value);
		else
			enable = GPM_DEFAULT_HIGHLIGHT;

		views = gtranslator_application_get_views (GTR_APP, TRUE, TRUE);
		l = views;

		while (l != NULL)
		{
			buf = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(l->data)));
			g_return_if_fail (GTK_IS_SOURCE_BUFFER (buf));

			gtk_source_buffer_set_highlight_syntax (buf, enable);

			l = l->next;
		}

		g_list_free (views);
	}
}

static void
gtranslator_prefs_manager_visible_whitespace_changed (GConfClient *client,
						   guint        cnxn_id,
						   GConfEntry  *entry,
						   gpointer     user_data)
{
	g_return_if_fail (entry->key != NULL);
	g_return_if_fail (entry->value != NULL);

	if (strcmp (entry->key, GPM_VISIBLE_WHITESPACE) == 0)
	{
		gboolean enable;
		GList *views;
		GList *l;

		if (entry->value->type == GCONF_VALUE_BOOL)
			enable = gconf_value_get_bool (entry->value);
		else
			enable = GPM_DEFAULT_VISIBLE_WHITESPACE;

		views = gtranslator_application_get_views (GTR_APP, TRUE, TRUE);
		l = views;

		while (l != NULL)
		{
			gtranslator_view_enable_visible_whitespace(GTR_VIEW(l->data), enable);

			l = l->next;
		}

		g_list_free (views);
	}
}

static void
gtranslator_prefs_manager_gdl_style_changed (GConfClient* client, guint id,
					     GConfEntry* entry,
					     gpointer user_data)
{
	GtranslatorWindow *window = GTR_WINDOW (user_data);
	GdlSwitcherStyle style;
	GdlDockLayout *layout_manager;
	
	style = gtranslator_prefs_manager_get_gdl_style ();
	
	layout_manager = GDL_DOCK_LAYOUT (_gtranslator_window_get_layout_manager (window));
	
	g_object_set (G_OBJECT (layout_manager->master),
		      "switcher-style", style, NULL);
}

