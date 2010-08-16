/*
 * gtr-settings.h
 * This file is part of gtranslator
 *
 * Copyright (C) 2009 - Ignacio Casal Quinteiro
 *               2002 - Paolo Maggi
 *
 * gtr is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * gtr is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gtr; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */


#ifndef __GTR_SETTINGS_H__
#define __GTR_SETTINGS_H__

#include <glib-object.h>
#include <glib.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define GTR_TYPE_SETTINGS		(gtr_settings_get_type ())
#define GTR_SETTINGS(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_SETTINGS, GtrSettings))
#define GTR_SETTINGS_CONST(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_SETTINGS, GtrSettings const))
#define GTR_SETTINGS_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GTR_TYPE_SETTINGS, GtrSettingsClass))
#define GTR_IS_SETTINGS(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTR_TYPE_SETTINGS))
#define GTR_IS_SETTINGS_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_TYPE_SETTINGS))
#define GTR_SETTINGS_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GTR_TYPE_SETTINGS, GtrSettingsClass))

typedef struct _GtrSettings		GtrSettings;
typedef struct _GtrSettingsClass	GtrSettingsClass;
typedef struct _GtrSettingsPrivate	GtrSettingsPrivate;

struct _GtrSettings
{
	GSettings parent;

	GtrSettingsPrivate *priv;
};

struct _GtrSettingsClass
{
	GSettingsClass parent_class;
};

GType			 gtr_settings_get_type			(void) G_GNUC_CONST;

GSettings		*gtr_settings_new			(void);

gchar			*gtr_settings_get_system_font		(GtrSettings *gs);

/* Utility functions */
GSList			*gtr_settings_get_list			(GSettings     *settings,
								 const gchar   *key);

void			 gtr_settings_set_list			(GSettings     *settings,
								 const gchar   *key,
								 const GSList  *list);

/* key constants: IMPORTANT: keep them in the same order as the schema */
#define GTR_SETTINGS_WARN_IF_CONTAINS_FUZZY		"warn-if-contains-fuzzy"
#define GTR_SETTINGS_DELETE_COMPILED			"delete-compiled"
#define GTR_SETTINGS_AUTO_SAVE				"auto-save"
#define GTR_SETTINGS_AUTO_SAVE_INTERVAL			"auto-save-interval"
#define GTR_SETTINGS_CREATE_BACKUP			"create-backup"
#define GTR_SETTINGS_USE_PROFILE_VALUES			"use-profile-values"
#define GTR_SETTINGS_HIGHLIGHT_SYNTAX			"highlight-syntax"
#define GTR_SETTINGS_VISIBLE_WHITESPACE			"visible-whitespace"
#define GTR_SETTINGS_USE_CUSTOM_FONT			"use-custom-font"
#define GTR_SETTINGS_EDITOR_FONT			"editor-font"
#define GTR_SETTINGS_UNMARK_FUZZY_WHEN_CHANGED		"unmark-fuzzy-when-changed"
#define GTR_SETTINGS_SPELLCHECK				"spellcheck"
#define GTR_SETTINGS_PO_DIRECTORY			"po-directory"
#define GTR_SETTINGS_RESTRICT_TO_FILENAME		"restrict-to-filename"
#define GTR_SETTINGS_FILENAME_RESTRICTION		"filename-restriction"
#define GTR_SETTINGS_MAX_MISSING_WORDS			"max-missing-words"
#define GTR_SETTINGS_MAX_LENGTH_DIFF			"max-length-diff"
#define GTR_SETTINGS_PANEL_SWITCHER_STYLE		"panel-switcher-style"
#define GTR_SETTINGS_COLOR_SCHEME			"color-scheme"
#define GTR_SETTINGS_SORT_ORDER				"sort-order"
#define GTR_SETTINGS_ACTIVE_PLUGINS			"active-plugins"

/* window state keys */
#define GTR_SETTINGS_WINDOW_STATE			"state"
#define GTR_SETTINGS_WINDOW_SIZE			"size"
#define GTR_SETTINGS_CONTENT_PANEL_SIZE			"content-panel-size"
#define GTR_SETTINGS_CONTEXT_PANEL_SIZE			"context-panel-size"

G_END_DECLS

#endif /* __GTR_SETTINGS_H__ */

/* ex:ts=8:noet: */
