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

#pragma once

#include <glib-object.h>
#include <glib.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define GTR_TYPE_SETTINGS (gtr_settings_get_type ())

G_DECLARE_DERIVABLE_TYPE (GtrSettings, gtr_settings, GTR, SETTINGS, GObject)

struct _GtrSettingsClass
{
  GObjectClass parent_class;
};

GSettings *gtr_settings_new (void);

/* key constants: IMPORTANT: keep them in the same order as the schema */
#define GTR_SETTINGS_WARN_IF_CONTAINS_FUZZY		"warn-if-contains-fuzzy"
#define GTR_SETTINGS_REMOVE_OBSOLETE_ENTRIES            "remove-obsolete-entries"
#define GTR_SETTINGS_AUTO_SAVE				"auto-save"
#define GTR_SETTINGS_AUTO_SAVE_INTERVAL			"auto-save-interval"
#define GTR_SETTINGS_CREATE_BACKUP			"create-backup"
#define GTR_SETTINGS_USE_PROFILE_VALUES			"use-profile-values"
#define GTR_SETTINGS_OMIT_HEADER_CREDIT		"omit-header-credit"
#define GTR_SETTINGS_HIGHLIGHT_SYNTAX			"highlight-syntax"
#define GTR_SETTINGS_VISIBLE_WHITESPACE			"visible-whitespace"
#define GTR_SETTINGS_UNMARK_FUZZY_WHEN_CHANGED		"unmark-fuzzy-when-changed"
#define GTR_SETTINGS_SPELLCHECK				"spellcheck"
#define GTR_SETTINGS_FONT				"font"
#define GTR_SETTINGS_SORT_ORDER				"sort-order"
#define GTR_SETTINGS_SHOW_ID_COLUMN		"show-id-column"

/* window state keys */
#define GTR_SETTINGS_WINDOW_MAXIMIZED		"window-maximized"
#define GTR_SETTINGS_WINDOW_SIZE			"size"

G_END_DECLS
