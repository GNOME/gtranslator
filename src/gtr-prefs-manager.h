/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * prefs-manager.h
 * This file is part of gtranslator based in gedit
 *
 * Copyright (C) 2002  Paolo Maggi 
 * 		 2007  Ignacio Casal Quinteiro
 *               2008  Igalia
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
 *
 * Authors:
 *   Paolo Maggi
 *   Ignacio Casal Quinteiro
 *   Pablo Sanxiao <psanxiao@gmail.com>
 */

#ifndef __GTR_PREFS_MANAGER_H__
#define __GTR_PREFS_MANAGER_H__

#include <glib.h>

#ifdef G_OS_WIN32
#include <gdl/libgdltypebuiltins.h>
#else
#include <gdl/gdl.h>
#endif

/* Useful enum for sort order pref */
typedef enum
{
  GTR_SORT_ORDER_STATUS,
  GTR_SORT_ORDER_ID,
  GTR_SORT_ORDER_ORIGINAL_TEXT,
  GTR_SORT_ORDER_TRANSLATED_TEXT
} GtrSortOrder;

#define GTR_BASE_KEY			"/apps/gtr"
#define GPM_PREFS_DIR			GTR_BASE_KEY "/preferences"

/* Files */
#define GPM_FILES_DIR			GPM_PREFS_DIR "/files"

#define GPM_WARN_IF_CONTAINS_FUZZY	GPM_FILES_DIR "/warn_if_contains_fuzzy"
#define GPM_DELETE_COMPILED		GPM_FILES_DIR "/delete_compiled"
#define GPM_AUTOSAVE			GPM_FILES_DIR "/autosave"
#define GPM_AUTOSAVE_INTERVAL		GPM_FILES_DIR "/autosave_interval"
#define GPM_CREATE_BACKUP		GPM_FILES_DIR "/create_backup"
#define GPM_USE_PROFILE_VALUES		GPM_FILES_DIR "/use_profile_values"

/* Editor */
#define GPM_EDITOR_DIR			GPM_PREFS_DIR "/editor"

#define GPM_HIGHLIGHT_SYNTAX		GPM_EDITOR_DIR "/highlight_syntax"
#define GPM_VISIBLE_WHITESPACE		GPM_EDITOR_DIR "/visible_whitespace"
#define GPM_USE_CUSTOM_FONT		GPM_EDITOR_DIR "/use_custom_font"
#define GPM_EDITOR_FONT			GPM_EDITOR_DIR "/editor_font"
#define GPM_UNMARK_FUZZY_WHEN_CHANGED	GPM_EDITOR_DIR "/unmark_fuzzy_when_changed"
#define GPM_SPELLCHECK			GPM_EDITOR_DIR "/spellcheck"

/* Translation Memory */
#define GPM_TM_DIR			GPM_PREFS_DIR "/tm"

#define GPM_PO_DIRECTORY		GPM_TM_DIR "/po_directory"
#define GPM_RESTRICT_TO_FILENAME	GPM_TM_DIR "/restrict_to_filename"
#define GPM_FILENAME_RESTRICTION	GPM_TM_DIR "/filename_restriction"
#define GPM_MAX_MISSING_WORDS	GPM_TM_DIR "/max_missing_words"
#define GPM_MAX_LENGTH_DIFF		GPM_TM_DIR "/max_length_diff"

/* User Interface */
#define GPM_UI_DIR			GPM_PREFS_DIR "/ui"

#define GPM_PANE_SWITCHER_STYLE		GPM_UI_DIR "/pane_switcher_style"
#define GPM_COLOR_SCHEME		GPM_UI_DIR "/color_scheme"
#define GPM_SORT_ORDER			GPM_UI_DIR "/sort_order"

/** LIFE CYCLE MANAGEMENT FUNCTIONS **/

void gtr_prefs_manager_init (void);

/* This function must be called before exiting gtranslator */
void gtr_prefs_manager_shutdown (void);


/** PREFS MANAGEMENT FUNCTIONS **/

/* Files */
void gtr_prefs_manager_set_warn_if_contains_fuzzy (gboolean
							   warn_if_contains_fuzzy);
gboolean gtr_prefs_manager_get_warn_if_contains_fuzzy (void);

void gtr_prefs_manager_set_delete_compiled (gboolean delete_compiled);
gboolean gtr_prefs_manager_get_delete_compiled (void);

void gtr_prefs_manager_set_autosave (gboolean autosave);
gboolean gtr_prefs_manager_get_autosave (void);

void gtr_prefs_manager_set_autosave_interval (gint autosave_interval);
gint gtr_prefs_manager_get_autosave_interval (void);

void gtr_prefs_manager_set_create_backup (gboolean create_backup);
gboolean gtr_prefs_manager_get_create_backup (void);

void gtr_prefs_manager_set_use_profile_values (gboolean
						       use_profile_values);
gboolean gtr_prefs_manager_get_use_profile_values (void);

/* Editor */
void gtr_prefs_manager_set_highlight_syntax (gboolean
						     highlight_syntax);
gboolean gtr_prefs_manager_get_highlight_syntax (void);

void gtr_prefs_manager_set_visible_whitespace (gboolean
						       visible_whitespace);
gboolean gtr_prefs_manager_get_visible_whitespace (void);

void gtr_prefs_manager_set_use_custom_font (gboolean use_custom_font);
gboolean gtr_prefs_manager_get_use_custom_font (void);

void gtr_prefs_manager_set_editor_font (const gchar * editor_font);
const gchar *gtr_prefs_manager_get_editor_font (void);

void gtr_prefs_manager_set_unmark_fuzzy_when_changed (gboolean
							      unmark_fuzzy_when_changed);
gboolean gtr_prefs_manager_get_unmark_fuzzy_when_changed (void);

void gtr_prefs_manager_set_spellcheck (gboolean spellcheck);
gboolean gtr_prefs_manager_get_spellcheck (void);

/* Translation memory */
void gtr_prefs_manager_set_po_directory (const gchar * po_directory);
const gchar *gtr_prefs_manager_get_po_directory (void);

void gtr_prefs_manager_set_restrict_to_filename (gboolean
							 restrict_to_filename);
gboolean gtr_prefs_manager_get_restrict_to_filename (void);

void gtr_prefs_manager_set_filename_restriction (const gchar *
							 filename_restriction);
const gchar *gtr_prefs_manager_get_filename_restriction (void);

void gtr_prefs_manager_set_max_missing_words (gint max_missing_words);
gint gtr_prefs_manager_get_max_missing_words (void);

void gtr_prefs_manager_set_max_length_diff (gint max_length_diff);
gint gtr_prefs_manager_get_max_length_diff (void);

/* UI */
void gtr_prefs_manager_set_pane_switcher_style (GdlSwitcherStyle
							pane_switcher_style);
GdlSwitcherStyle gtr_prefs_manager_get_pane_switcher_style (void);

void gtr_prefs_manager_set_color_scheme (const gchar * color_scheme);
const gchar *gtr_prefs_manager_get_color_scheme (void);

void gtr_prefs_manager_set_sort_order (GtrSortOrder
					       sort_order);
GtrSortOrder gtr_prefs_manager_get_sort_order (void);

#endif /* __GTR_PREFS_MANAGER_H__ */
