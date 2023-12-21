/*
 * gtr-settings.h
 * This file is part of gtranslator
 *
 * Copyright (C) 2009  Ignacio Casal Quinteiro
 *               2002  Paolo Maggi
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

struct _GtrSettings
{
  GObject parent;
};

struct _GtrSettingsClass
{
  GObjectClass parent_class;
};

GType                    gtr_settings_get_type                  (void) G_GNUC_CONST;

GSettings               *gtr_settings_new                       (void);

/* key constants: IMPORTANT: keep them in the same order as the schema */
#define GTR_SETTINGS_WARN_IF_CONTAINS_FUZZY		"warn-if-contains-fuzzy"
#define GTR_SETTINGS_REMOVE_OBSOLETE_ENTRIES            "remove-obsolete-entries"
#define GTR_SETTINGS_AUTO_SAVE				"auto-save"
#define GTR_SETTINGS_AUTO_SAVE_INTERVAL			"auto-save-interval"
#define GTR_SETTINGS_CREATE_BACKUP			"create-backup"
#define GTR_SETTINGS_USE_PROFILE_VALUES			"use-profile-values"
#define GTR_SETTINGS_HIGHLIGHT_SYNTAX			"highlight-syntax"
#define GTR_SETTINGS_VISIBLE_WHITESPACE			"visible-whitespace"
#define GTR_SETTINGS_UNMARK_FUZZY_WHEN_CHANGED		"unmark-fuzzy-when-changed"
#define GTR_SETTINGS_SPELLCHECK				"spellcheck"
#define GTR_SETTINGS_FONT				"font"
#define GTR_SETTINGS_SORT_ORDER				"sort-order"

/* window state keys */
#define GTR_SETTINGS_WINDOW_STATE			"state"
#define GTR_SETTINGS_WINDOW_SIZE			"size"

G_END_DECLS

#endif /* __GTR_SETTINGS_H__ */

/* ex:ts=8:noet: */
