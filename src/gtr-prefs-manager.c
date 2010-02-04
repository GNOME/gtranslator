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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include <glib/gi18n.h>
#include <gconf/gconf-value.h>
#include <gconf/gconf-client.h>

#include <gtk/gtk.h>

#include "gtr-prefs-manager.h"
#include "gtr-prefs-manager-private.h"

#define DEFINE_BOOL_PREF(name, key) gboolean 			\
gtranslator_prefs_manager_get_ ## name (void)					\
{									\
									\
	return gtranslator_prefs_manager_get_bool (key);			\
}									\
									\
void 									\
gtranslator_prefs_manager_set_ ## name (gboolean v)				\
{									\
									\
	gtranslator_prefs_manager_set_bool (key,				\
				      v);				\
}


#define DEFINE_INT_PREF(name, key) gint	 			\
gtranslator_prefs_manager_get_ ## name (void)			 		\
{									\
									\
	return gtranslator_prefs_manager_get_int (key);			\
}									\
									\
void 									\
gtranslator_prefs_manager_set_ ## name (gint v)				\
{									\
									\
	gtranslator_prefs_manager_set_int (key,				\
				     v);				\
}

#define DEFINE_STRING_PREF(name, key) const gchar*	 		\
gtranslator_prefs_manager_get_ ## name (void)			 		\
{									\
									\
	return gtranslator_prefs_manager_get_string (key);			\
}									\
									\
void 									\
gtranslator_prefs_manager_set_ ## name (const gchar* v)			\
{									\
									\
	gtranslator_prefs_manager_set_string (key,				\
				        v);				\
}


GtrPrefsManager *gtranslator_prefs_manager = NULL;

static gboolean gtranslator_prefs_manager_get_bool (const gchar * key);

static gint gtranslator_prefs_manager_get_int (const gchar * key);

static gchar *gtranslator_prefs_manager_get_string (const gchar * key);


void
gtranslator_prefs_manager_init (void)
{
  //gtranslator_debug (DEBUG_PREFS);

  if (gtranslator_prefs_manager == NULL)
    {
      GConfClient *gconf_client;

      gconf_client = gconf_client_get_default ();
      if (gconf_client == NULL)
	g_error ("Failed to initialize preferences manager");

      gtranslator_prefs_manager = g_new0 (GtrPrefsManager, 1);

      gtranslator_prefs_manager->gconf_client = gconf_client;
    }
}

void
gtranslator_prefs_manager_shutdown ()
{
  //gtranslator_debug (DEBUG_PREFS);

  g_object_unref (gtranslator_prefs_manager->gconf_client);
  gtranslator_prefs_manager->gconf_client = NULL;
}

static gboolean
gtranslator_prefs_manager_get_bool (const gchar * key)
{
  //gtranslator_debug (DEBUG_PREFS);

  return gconf_client_get_bool (gtranslator_prefs_manager->gconf_client,
				key, NULL);
}

static gint
gtranslator_prefs_manager_get_int (const gchar * key)
{
  //gtranslator_debug (DEBUG_PREFS);

  return gconf_client_get_int (gtranslator_prefs_manager->gconf_client,
			       key, NULL);
}

static gchar *
gtranslator_prefs_manager_get_string (const gchar * key)
{
  //gtranslator_debug (DEBUG_PREFS);

  return gconf_client_get_string (gtranslator_prefs_manager->gconf_client,
				  key, NULL);
}

static void
gtranslator_prefs_manager_set_bool (const gchar * key, gboolean value)
{
  //gtranslator_debug (DEBUG_PREFS);

  g_return_if_fail (gconf_client_key_is_writable
		    (gtranslator_prefs_manager->gconf_client, key, NULL));

  gconf_client_set_bool (gtranslator_prefs_manager->gconf_client, key, value,
			 NULL);
}

static void
gtranslator_prefs_manager_set_int (const gchar * key, gint value)
{
  //gtranslator_debug (DEBUG_PREFS);

  g_return_if_fail (gconf_client_key_is_writable
		    (gtranslator_prefs_manager->gconf_client, key, NULL));

  gconf_client_set_int (gtranslator_prefs_manager->gconf_client, key, value,
			NULL);
}

static void
gtranslator_prefs_manager_set_string (const gchar * key, const gchar * value)
{
  //gtranslator_debug (DEBUG_PREFS);

  g_return_if_fail (value != NULL);

  g_return_if_fail (gconf_client_key_is_writable
		    (gtranslator_prefs_manager->gconf_client, key, NULL));

  gconf_client_set_string (gtranslator_prefs_manager->gconf_client, key,
			   value, NULL);
}

static gboolean
gtranslator_prefs_manager_key_is_writable (const gchar * key)
{
  //gtranslator_debug (DEBUG_PREFS);

  return gconf_client_key_is_writable (gtranslator_prefs_manager->
				       gconf_client, key, NULL);
}

/* Files */
DEFINE_BOOL_PREF (warn_if_contains_fuzzy,
		  GPM_WARN_IF_CONTAINS_FUZZY)
DEFINE_BOOL_PREF (delete_compiled,
		  GPM_DELETE_COMPILED)
DEFINE_BOOL_PREF (autosave,
		  GPM_AUTOSAVE)
DEFINE_INT_PREF (autosave_interval,
		 GPM_AUTOSAVE_INTERVAL)
DEFINE_BOOL_PREF (create_backup,
		  GPM_CREATE_BACKUP)
DEFINE_BOOL_PREF (use_profile_values, GPM_USE_PROFILE_VALUES)
/* Editor */
  DEFINE_BOOL_PREF (highlight_syntax,
		  GPM_HIGHLIGHT_SYNTAX)
DEFINE_BOOL_PREF (visible_whitespace,
		  GPM_VISIBLE_WHITESPACE)
DEFINE_BOOL_PREF (use_custom_font,
		  GPM_USE_CUSTOM_FONT)
DEFINE_STRING_PREF (editor_font,
		    GPM_EDITOR_FONT)
DEFINE_BOOL_PREF (unmark_fuzzy_when_changed,
		  GPM_UNMARK_FUZZY_WHEN_CHANGED)
DEFINE_BOOL_PREF (spellcheck, GPM_SPELLCHECK)
/* Translation Memory */
  DEFINE_STRING_PREF (po_directory,
		    GPM_PO_DIRECTORY)
DEFINE_BOOL_PREF (restrict_to_filename,
		  GPM_RESTRICT_TO_FILENAME)
DEFINE_STRING_PREF (filename_restriction,
		    GPM_FILENAME_RESTRICTION)
DEFINE_INT_PREF (max_missing_words,
		 GPM_MAX_MISSING_WORDS)
DEFINE_INT_PREF (max_length_diff, GPM_MAX_LENGTH_DIFF)
/* UI */
/* pane_switcher_style requires custom functions to deal with conversion from a
   string to an enum */
  GdlSwitcherStyle
gtranslator_prefs_manager_get_pane_switcher_style (void)
{
  GdlSwitcherStyle pane_switcher_style;
  const gchar *v;

  v = gtranslator_prefs_manager_get_string (GPM_PANE_SWITCHER_STYLE);
  if (!g_strcmp0 (v, "icons"))
    pane_switcher_style = GDL_SWITCHER_STYLE_ICON;
  else if (!g_strcmp0 (v, "text"))
    pane_switcher_style = GDL_SWITCHER_STYLE_TEXT;
  else if (!g_strcmp0 (v, "icons_and_text"))
    pane_switcher_style = GDL_SWITCHER_STYLE_BOTH;
  else if (!g_strcmp0 (v, "tabs"))
    pane_switcher_style = GDL_SWITCHER_STYLE_TABS;
  else if (!g_strcmp0 (v, "system"))
    pane_switcher_style = GDL_SWITCHER_STYLE_TOOLBAR;
  else
    pane_switcher_style = GDL_SWITCHER_STYLE_BOTH;

  return pane_switcher_style;
}

void
gtranslator_prefs_manager_set_pane_switcher_style (GdlSwitcherStyle
						   pane_switcher_style)
{
  const gchar *v;

  switch (pane_switcher_style)
    {
    case GDL_SWITCHER_STYLE_ICON:
      v = "icons";
      break;

    case GDL_SWITCHER_STYLE_TEXT:
      v = "text";
      break;

    case GDL_SWITCHER_STYLE_TABS:
      v = "tabs";
      break;

    case GDL_SWITCHER_STYLE_TOOLBAR:
      v = "system";
      break;

    case GDL_SWITCHER_STYLE_BOTH:
    default:
      v = "icons_and_text";
    }
  gtranslator_prefs_manager_set_string (GPM_PANE_SWITCHER_STYLE, v);
}

DEFINE_STRING_PREF (color_scheme, GPM_COLOR_SCHEME)
/* sort_order requires custom functions to deal with conversion from a string to
   an enum */
  GtrSortOrder
gtranslator_prefs_manager_get_sort_order (void)
{
  GtrSortOrder sort_order;
  const gchar *v;

  v = gtranslator_prefs_manager_get_string (GPM_SORT_ORDER);
  if (!g_strcmp0 (v, "status"))
    sort_order = GTR_SORT_ORDER_STATUS;
  else if (!g_strcmp0 (v, "id"))
    sort_order = GTR_SORT_ORDER_ID;
  else if (!g_strcmp0 (v, "original_text"))
    sort_order = GTR_SORT_ORDER_ORIGINAL_TEXT;
  else if (!g_strcmp0 (v, "translated_text"))
    sort_order = GTR_SORT_ORDER_TRANSLATED_TEXT;
  else
    sort_order = GTR_SORT_ORDER_ID;

  return sort_order;
}

void
gtranslator_prefs_manager_set_sort_order (GtrSortOrder sort_order)
{
  const gchar *v;

  switch (sort_order)
    {
    case GTR_SORT_ORDER_STATUS:
      v = "status";
      break;

    case GTR_SORT_ORDER_ORIGINAL_TEXT:
      v = "original_text";
      break;

    case GTR_SORT_ORDER_TRANSLATED_TEXT:
      v = "translated_text";
      break;

    case GTR_SORT_ORDER_ID:
    default:
      v = "id";
    }
  gtranslator_prefs_manager_set_string (GPM_SORT_ORDER, v);
}
