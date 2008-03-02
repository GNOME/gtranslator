/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * prefs-manager.h
 * This file is part of gtranslator based in gedit
 *
 * Copyright (C) 2002  Paolo Maggi 
 * 		 2007  Ignacio Casal Quinteiro
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

#ifndef __GTR_PREFS_MANAGER_H__
#define __GTR_PREFS_MANAGER_H__

#include <glib.h>

#define GTR_BASE_KEY			"/apps/gtranslator"

#define GPM_PREFS_DIR			GTR_BASE_KEY "/preferences"

/* Files */
#define GPM_GENERAL_DIR			GPM_PREFS_DIR "/files/general"
#define GPM_WARN_IF_FUZZY		GPM_GENERAL_DIR "/warn_if_fuzzy"
#define GPM_DELETE_COMPILED		GPM_GENERAL_DIR "/delete_compiled"

#define GPM_AUTOSAVE_DIR		GPM_PREFS_DIR "/files/autosave"
#define GPM_AUTOSAVE			GPM_AUTOSAVE_DIR "/autosave"
#define GPM_AUTOSAVE_INTERVAL		GPM_AUTOSAVE_DIR "/autosave_interval"
#define GPM_APPEND_SUFFIX		GPM_AUTOSAVE_DIR "/append_suffix"
#define GPM_AUTOSAVE_SUFFIX		GPM_AUTOSAVE_DIR "/autosave_suffix"

/* Editor */
#define GPM_TEXT_DISPLAY_DIR		GPM_PREFS_DIR "/editor/text_display"
#define GPM_HIGHLIGHT			GPM_TEXT_DISPLAY_DIR "/highlight"
#define GPM_VISIBLE_WHITESPACE		GPM_TEXT_DISPLAY_DIR "/visible_whitespace"
#define GPM_FONT_DIR			GPM_TEXT_DISPLAY_DIR "/fonts"
#define GPM_USE_CUSTOM_FONT		GPM_FONT_DIR "/use_custom_font"
#define GPM_EDITOR_FONT			GPM_FONT_DIR "/editor_font"

#define GPM_CONTENTS_DIR		GPM_PREFS_DIR "/editor/contents"
#define GPM_UNMARK_FUZZY		GPM_CONTENTS_DIR "/unmark_fuzzy"
#define GPM_KEEP_OBSOLETE		GPM_CONTENTS_DIR "/keep_obsolete"
#define GPM_SPELLCHECK			GPM_CONTENTS_DIR "/spellcheck"

/* Edit Header */
#define GPM_TAKE_MY_OPTIONS		GPM_PREFS_DIR "/take_my_options"

/* PO header */
#define GPM_PERSONAL_INFORMATION_DIR	GPM_PREFS_DIR "/po_header/personal_information"
#define GPM_NAME			GPM_PERSONAL_INFORMATION_DIR "/name"
#define GPM_EMAIL			GPM_PERSONAL_INFORMATION_DIR "/email"

#define GPM_LANGUAGE_SETTINGS_DIR	GPM_PREFS_DIR "/po_header/language_settings"
#define GPM_LANGUAGE			GPM_LANGUAGE_SETTINGS_DIR "/language"
#define GPM_LANGCODE			GPM_LANGUAGE_SETTINGS_DIR "/langcode"
#define GPM_CHARSET			GPM_LANGUAGE_SETTINGS_DIR "/charset"
#define GPM_TRANSFER_ENCODING		GPM_LANGUAGE_SETTINGS_DIR "/transfer_encoding"
#define GPM_TEAM_EMAIL			GPM_LANGUAGE_SETTINGS_DIR "/team_email"
#define GPM_NUMBER_PLURALS		GPM_LANGUAGE_SETTINGS_DIR "/number_plurals"
#define GPM_PLURALS			GPM_LANGUAGE_SETTINGS_DIR "/plurals"

/* Interface */
#define GPM_INTERFACE_DIR		GTR_BASE_KEY "/interface"
#define GPM_GDL_STYLE			GPM_INTERFACE_DIR "/gdl_style"

/* Fallback default values. Keep in sync with gtranslator.schemas */

/* Files */
#define GPM_DEFAULT_WARN_IF_FUZZY	  0 /* FALSE */
#define GPM_DEFAULT_DELETE_COMPILED       0 /* FALSE */

#define GPM_DEFAULT_AUTOSAVE		  0 /* FALSE */
#define GPM_DEFAULT_AUTOSAVE_INTERVAL	  4
#define GPM_DEFAULT_APPEND_SUFFIX         0 /* FALSE */
#define GPM_DEFAULT_AUTOSAVE_SUFFIX	  (const gchar*) "autosave"

/* Editor */
#define GPM_DEFAULT_HIGHLIGHT             1 /* TRUE */
#define GPM_DEFAULT_VISIBLE_WHITESPACE    1 /* TRUE */
#define GPM_DEFAULT_USE_CUSTOM_FONT       0 /* FALSE */
#define GPM_DEFAULT_EDITOR_FONT           (const gchar*) "Sans 10"

#define GPM_DEFAULT_UNMARK_FUZZY          1 /* TRUE */
#define GPM_DEFAULT_KEEP_OBSOLETE         0 /* FALSE */
#define GPM_DEFAULT_SPELLCHECK            1 /* TRUE */

/* Edit Header */
#define GPM_DEFAULT_TAKE_MY_OPTIONS	  0 /*FALSE */

/* PO header */
#define GPM_DEFAULT_NAME		(const gchar*) ""
#define GPM_DEFAULT_EMAIL		(const gchar*) ""

/*#define GPM_AUTHORS_LANGUAGE		GPM_LANGUAGE_SETTINGS_DIR "/authors_language"
#define GPM_LCODE			GPM_LANGUAGE_SETTINGS_DIR "/lcode"
#define GPM_MIME_TYPE			GPM_LANGUAGE_SETTINGS_DIR "/mime_type"
#define GPM_ENCODING			GPM_LANGUAGE_SETTINGS_DIR "/encoding"
#define GPM_LG_EMAIL			GPM_LANGUAGE_SETTINGS_DIR "/lg_email"*/
#define GPM_DEFAULT_NUMBER_PLURALS	2
#define GPM_DEFAULT_PLURALS		(const gchar*) ""

/* Interface */
#define GPM_DEFAULT_GDL_STYLE           2 //Both icons and text

/** LIFE CYCLE MANAGEMENT FUNCTIONS **/

gboolean		 gtranslator_prefs_manager_init                         (void);

/* This function must be called before exiting gtranslator */
void			 gtranslator_prefs_manager_shutdown                     (void);


/** PREFS MANAGEMENT FUNCTIONS **/

/* Use preferences for header */
void			gtranslator_prefs_manager_set_take_my_options		(gboolean take_options);
gboolean		gtranslator_prefs_manager_get_take_my_options		(void);

/* Warn if fuzzy*/
void                     gtranslator_prefs_manager_set_warn_if_fuzzy            (gboolean wif);
gboolean                 gtranslator_prefs_manager_get_warn_if_fuzzy            (void);

/*Delete compiled*/
void                     gtranslator_prefs_manager_set_delete_compiled          (gboolean scf);
gboolean                 gtranslator_prefs_manager_get_delete_compiled          (void);

/*Autosave*/
void                     gtranslator_prefs_manager_set_autosave                 (gboolean autosave);
gboolean                 gtranslator_prefs_manager_get_autosave                 (void);

/*Autosave interval*/
void                     gtranslator_prefs_manager_set_autosave_interval        (gint timeout);
gint                     gtranslator_prefs_manager_get_autosave_interval        (void);

/*Append suffix*/
void                     gtranslator_prefs_manager_set_append_suffix            (gboolean aws);
gboolean                 gtranslator_prefs_manager_get_append_suffix            (void);

/*Autosave suffix*/
void                     gtranslator_prefs_manager_set_autosave_suffix          (const gchar *suffix);
const gchar *            gtranslator_prefs_manager_get_autosave_suffix          (void);

/*Highlight*/
void                     gtranslator_prefs_manager_set_highlight                (gboolean highlight);
gboolean                 gtranslator_prefs_manager_get_highlight                (void);

/*Visible whitespace*/
void                     gtranslator_prefs_manager_set_visible_whitespace       (gboolean udc);
gboolean                 gtranslator_prefs_manager_get_visible_whitespace       (void);

/*Use custom font*/
void                     gtranslator_prefs_manager_set_use_custom_font          (gboolean use_custom_font);
gboolean                 gtranslator_prefs_manager_get_use_custom_font          (void);

/*Editor font*/
void                     gtranslator_prefs_manager_set_editor_font              (const gchar *font);
const gchar *            gtranslator_prefs_manager_get_editor_font              (void);

/*Unmark fuzzy*/
void                     gtranslator_prefs_manager_set_unmark_fuzzy             (gboolean unmark);
gboolean                 gtranslator_prefs_manager_get_unmark_fuzzy             (void);

/*keep obsolete*/
void                     gtranslator_prefs_manager_set_keep_obsolete            (gboolean keep_obsolete);
gboolean                 gtranslator_prefs_manager_get_keep_obsolete            (void);

/*Spellcheck*/
void                     gtranslator_prefs_manager_set_spellcheck               (gboolean spell_checking);
gboolean                 gtranslator_prefs_manager_get_spellcheck               (void);

/*Name*/
void                     gtranslator_prefs_manager_set_name                     (const gchar *name);
const gchar *            gtranslator_prefs_manager_get_name                     (void);

/*Email*/
void                     gtranslator_prefs_manager_set_email                    (const gchar *email);
const gchar *            gtranslator_prefs_manager_get_email                    (void);

/*number plurals*/
void                     gtranslator_prefs_manager_set_number_plurals           (gint plurals);
gint                     gtranslator_prefs_manager_get_number_plurals           (void);

/*gdl style*/
void                     gtranslator_prefs_manager_set_gdl_style                (gint style);
gint                     gtranslator_prefs_manager_get_gdl_style                (void);

#endif  /* __GTR_PREFS_MANAGER_H__ */
