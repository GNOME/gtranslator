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

#include "prefs-manager.h"
#include "prefs-manager-private.h"

#define DEFINE_BOOL_PREF(name, key, def) gboolean 			\
gtranslator_prefs_manager_get_ ## name (void)					\
{									\
									\
	return gtranslator_prefs_manager_get_bool (key,			\
					     (def));			\
}									\
									\
void 									\
gtranslator_prefs_manager_set_ ## name (gboolean v)				\
{									\
									\
	gtranslator_prefs_manager_set_bool (key,				\
				      v);				\
}


#define DEFINE_INT_PREF(name, key, def) gint	 			\
gtranslator_prefs_manager_get_ ## name (void)			 		\
{									\
									\
	return gtranslator_prefs_manager_get_int (key,			\
					    (def));			\
}									\
									\
void 									\
gtranslator_prefs_manager_set_ ## name (gint v)				\
{									\
									\
	gtranslator_prefs_manager_set_int (key,				\
				     v);				\
}

#define DEFINE_STRING_PREF(name, key, def) const gchar*	 		\
gtranslator_prefs_manager_get_ ## name (void)			 		\
{									\
									\
	return gtranslator_prefs_manager_get_string (key,			\
					       def);			\
}									\
									\
void 									\
gtranslator_prefs_manager_set_ ## name (const gchar* v)			\
{									\
									\
	gtranslator_prefs_manager_set_string (key,				\
				        v);				\
}	


GtranslatorPrefsManager *gtranslator_prefs_manager = NULL;


static GtkWrapMode 	 get_wrap_mode_from_string 		(const gchar* str);

static gboolean 	 gconf_client_get_bool_with_default 	(GConfClient* client, 
								 const gchar* key, 
								 gboolean def, 
								 GError** err);

static gchar		*gconf_client_get_string_with_default 	(GConfClient* client, 
								 const gchar* key,
								 const gchar* def, 
								 GError** err);

static gint		 gconf_client_get_int_with_default 	(GConfClient* client, 
								 const gchar* key,
								 gint def, 
								 GError** err);

static gboolean		 gtranslator_prefs_manager_get_bool		(const gchar* key, 
								 gboolean def);

static gint		 gtranslator_prefs_manager_get_int		(const gchar* key, 
								 gint def);

static gchar		*gtranslator_prefs_manager_get_string		(const gchar* key, 
								 const gchar* def);


gboolean
gtranslator_prefs_manager_init (void)
{
	//gtranslator_debug (DEBUG_PREFS);

	if (gtranslator_prefs_manager == NULL)
	{
		GConfClient *gconf_client;

		gconf_client = gconf_client_get_default ();
		if (gconf_client == NULL)
		{
			g_warning (_("Cannot initialize preferences manager."));
			return FALSE;
		}

		gtranslator_prefs_manager = g_new0 (GtranslatorPrefsManager, 1);

		gtranslator_prefs_manager->gconf_client = gconf_client;
	}

	if (gtranslator_prefs_manager->gconf_client == NULL)
	{
		g_free (gtranslator_prefs_manager);
		gtranslator_prefs_manager = NULL;
	}

	return gtranslator_prefs_manager != NULL;
	
}

void
gtranslator_prefs_manager_shutdown ()
{
	//gtranslator_debug (DEBUG_PREFS);

	g_return_if_fail (gtranslator_prefs_manager != NULL);

	g_object_unref (gtranslator_prefs_manager->gconf_client);
	gtranslator_prefs_manager->gconf_client = NULL;
}

static gboolean		 
gtranslator_prefs_manager_get_bool (const gchar* key, gboolean def)
{
	//gtranslator_debug (DEBUG_PREFS);

	g_return_val_if_fail (gtranslator_prefs_manager != NULL, def);
	g_return_val_if_fail (gtranslator_prefs_manager->gconf_client != NULL, def);

	return gconf_client_get_bool_with_default (gtranslator_prefs_manager->gconf_client,
						   key,
						   def,
						   NULL);
}

static gint 
gtranslator_prefs_manager_get_int (const gchar* key, gint def)
{
	//gtranslator_debug (DEBUG_PREFS);

	g_return_val_if_fail (gtranslator_prefs_manager != NULL, def);
	g_return_val_if_fail (gtranslator_prefs_manager->gconf_client != NULL, def);

	return gconf_client_get_int_with_default (gtranslator_prefs_manager->gconf_client,
						  key,
						  def,
						  NULL);
}	

static gchar *
gtranslator_prefs_manager_get_string (const gchar* key, const gchar* def)
{
	//gtranslator_debug (DEBUG_PREFS);

	g_return_val_if_fail (gtranslator_prefs_manager != NULL, 
			      def ? g_strdup (def) : NULL);
	g_return_val_if_fail (gtranslator_prefs_manager->gconf_client != NULL, 
			      def ? g_strdup (def) : NULL);

	return gconf_client_get_string_with_default (gtranslator_prefs_manager->gconf_client,
						     key,
						     def,
						     NULL);
}	

static void		 
gtranslator_prefs_manager_set_bool (const gchar* key, gboolean value)
{
	//gtranslator_debug (DEBUG_PREFS);

	g_return_if_fail (gtranslator_prefs_manager != NULL);
	g_return_if_fail (gtranslator_prefs_manager->gconf_client != NULL);
	g_return_if_fail (gconf_client_key_is_writable (
				gtranslator_prefs_manager->gconf_client, key, NULL));
			
	gconf_client_set_bool (gtranslator_prefs_manager->gconf_client, key, value, NULL);
}

static void		 
gtranslator_prefs_manager_set_int (const gchar* key, gint value)
{
	//gtranslator_debug (DEBUG_PREFS);

	g_return_if_fail (gtranslator_prefs_manager != NULL);
	g_return_if_fail (gtranslator_prefs_manager->gconf_client != NULL);
	g_return_if_fail (gconf_client_key_is_writable (
				gtranslator_prefs_manager->gconf_client, key, NULL));
			
	gconf_client_set_int (gtranslator_prefs_manager->gconf_client, key, value, NULL);
}

static void		 
gtranslator_prefs_manager_set_string (const gchar* key, const gchar* value)
{
	//gtranslator_debug (DEBUG_PREFS);

	g_return_if_fail (value != NULL);
	
	g_return_if_fail (gtranslator_prefs_manager != NULL);
	g_return_if_fail (gtranslator_prefs_manager->gconf_client != NULL);
	g_return_if_fail (gconf_client_key_is_writable (
				gtranslator_prefs_manager->gconf_client, key, NULL));
			
	gconf_client_set_string (gtranslator_prefs_manager->gconf_client, key, value, NULL);
}

static gboolean 
gtranslator_prefs_manager_key_is_writable (const gchar* key)
{
	//gtranslator_debug (DEBUG_PREFS);

	g_return_val_if_fail (gtranslator_prefs_manager != NULL, FALSE);
	g_return_val_if_fail (gtranslator_prefs_manager->gconf_client != NULL, FALSE);

	return gconf_client_key_is_writable (gtranslator_prefs_manager->gconf_client, key, NULL);
}

/* Edit Header */
DEFINE_BOOL_PREF(take_my_options,
		 GPM_TAKE_MY_OPTIONS,
		 GPM_DEFAULT_TAKE_MY_OPTIONS)

/* Files */
DEFINE_BOOL_PREF(warn_if_fuzzy,
		 GPM_WARN_IF_FUZZY,
		 GPM_DEFAULT_WARN_IF_FUZZY)

DEFINE_BOOL_PREF(delete_compiled,
		 GPM_DELETE_COMPILED,
		 GPM_DEFAULT_DELETE_COMPILED)

DEFINE_BOOL_PREF(autosave,
		 GPM_AUTOSAVE,
		 GPM_DEFAULT_AUTOSAVE)

DEFINE_INT_PREF(autosave_interval,
		GPM_AUTOSAVE_INTERVAL,
		GPM_DEFAULT_AUTOSAVE_INTERVAL)

DEFINE_BOOL_PREF(create_backup,
		 GPM_CREATE_BACKUP,
		 GPM_DEFAULT_CREATE_BACKUP)

/* Editor */
DEFINE_BOOL_PREF(highlight,
		 GPM_HIGHLIGHT,
		 GPM_DEFAULT_HIGHLIGHT)

DEFINE_BOOL_PREF(visible_whitespace,
		 GPM_VISIBLE_WHITESPACE,
		 GPM_DEFAULT_VISIBLE_WHITESPACE)

DEFINE_BOOL_PREF(use_custom_font,
		 GPM_USE_CUSTOM_FONT,
		 GPM_DEFAULT_USE_CUSTOM_FONT)

DEFINE_STRING_PREF(editor_font,
		   GPM_EDITOR_FONT,
		   GPM_DEFAULT_EDITOR_FONT)

DEFINE_BOOL_PREF(unmark_fuzzy,
		 GPM_UNMARK_FUZZY,
		 GPM_DEFAULT_UNMARK_FUZZY)

DEFINE_BOOL_PREF(spellcheck,
		 GPM_SPELLCHECK,
		 GPM_DEFAULT_SPELLCHECK)

/* PO header */
DEFINE_STRING_PREF(name,
		   GPM_NAME,
		   GPM_DEFAULT_NAME)

DEFINE_STRING_PREF(email,
		   GPM_EMAIL,
		   GPM_DEFAULT_EMAIL)

DEFINE_STRING_PREF(plural_form,
		   GPM_PLURAL_FORM,
		   GPM_DEFAULT_PLURAL_FORM)

/* Interface */
DEFINE_INT_PREF (gdl_style,
		 GPM_GDL_STYLE,
		 GPM_DEFAULT_GDL_STYLE)

DEFINE_STRING_PREF (scheme_color,
		    GPM_SCHEME_COLOR,
		    GPM_DEFAULT_SCHEME_COLOR)

/* Translation Memory */
DEFINE_STRING_PREF (tm_dir,
		    GPM_TM_DIR,
		    GPM_DEFAULT_TM_DIR)

DEFINE_INT_PREF (use_lang_profile,
		    GPM_TM_USE_LANG_PROFILE,
		    GPM_DEFAULT_TM_USE_LANG_PROFILE)

DEFINE_STRING_PREF (tm_lang_entry,
		    GPM_TM_LANG_ENTRY,
		    GPM_DEFAULT_TM_LANG_ENTRY)

DEFINE_INT_PREF (show_tm_options,
		 GPM_TM_SHOW_TM_OPTIONS,
		 GPM_DEFAULT_TM_SHOW_TM_OPTIONS)

DEFINE_INT_PREF (missing_words,
		 GPM_TM_MISSING_WORDS,
		 GPM_DEFAULT_TM_MISSING_WORDS)

DEFINE_INT_PREF (sentence_length,
		 GPM_TM_SENTENCE_LENGTH,
		 GPM_DEFAULT_TM_SENTENCE_LENGTH)

/* The following functions are taken from gconf-client.c 
 * and partially modified. 
 * The licensing terms on these is: 
 *
 * 
 * GConf
 * Copyright (C) 1999, 2000, 2000 Red Hat Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */


static const gchar* 
gconf_value_type_to_string(GConfValueType type)
{
  switch (type)
    {
    case GCONF_VALUE_INT:
      return "int";
      break;
    case GCONF_VALUE_STRING:
      return "string";
      break;
    case GCONF_VALUE_FLOAT:
      return "float";
      break;
    case GCONF_VALUE_BOOL:
      return "bool";
      break;
    case GCONF_VALUE_SCHEMA:
      return "schema";
      break;
    case GCONF_VALUE_LIST:
      return "list";
      break;
    case GCONF_VALUE_PAIR:
      return "pair";
      break;
    case GCONF_VALUE_INVALID:
      return "*invalid*";
      break;
    default:
      g_return_val_if_reached (NULL);
      break;
    }
}

/* Emit the proper signals for the error, and fill in err */
static gboolean
handle_error (GConfClient* client, GError* error, GError** err)
{
  if (error != NULL)
    {
      gconf_client_error(client, error);
      
      if (err == NULL)
        {
          gconf_client_unreturned_error(client, error);

          g_error_free(error);
        }
      else
        *err = error;

      return TRUE;
    }
  else
    return FALSE;
}

static gboolean
check_type (const gchar* key, GConfValue* val, GConfValueType t, GError** err)
{
  if (val->type != t)
    {
      /*
      gconf_set_error(err, GCONF_ERROR_TYPE_MISMATCH,
                      _("Expected `%s' got `%s' for key %s"),
                      gconf_value_type_to_string(t),
                      gconf_value_type_to_string(val->type),
                      key);
      */
      g_set_error (err, GCONF_ERROR, GCONF_ERROR_TYPE_MISMATCH,
	  	   _("Expected `%s' got `%s' for key %s"),
                   gconf_value_type_to_string(t),
                   gconf_value_type_to_string(val->type),
                   key);
	      
      return FALSE;
    }
  else
    return TRUE;
}

static gboolean
gconf_client_get_bool_with_default (GConfClient* client, const gchar* key,
                        	    gboolean def, GError** err)
{
  GError* error = NULL;
  GConfValue* val;

  g_return_val_if_fail (err == NULL || *err == NULL, def);

  val = gconf_client_get (client, key, &error);

  if (val != NULL)
    {
      gboolean retval = def;

      g_return_val_if_fail (error == NULL, retval);
      
      if (check_type (key, val, GCONF_VALUE_BOOL, &error))
        retval = gconf_value_get_bool (val);
      else
        handle_error (client, error, err);

      gconf_value_free (val);

      return retval;
    }
  else
    {
      if (error != NULL)
        handle_error (client, error, err);
      return def;
    }
}

static gchar*
gconf_client_get_string_with_default (GConfClient* client, const gchar* key,
                        	      const gchar* def, GError** err)
{
  GError* error = NULL;
  gchar* val;

  g_return_val_if_fail (err == NULL || *err == NULL, def ? g_strdup (def) : NULL);

  val = gconf_client_get_string (client, key, &error);

  if (val != NULL)
    {
      g_return_val_if_fail (error == NULL, def ? g_strdup (def) : NULL);
      
      return val;
    }
  else
    {
      if (error != NULL)
        handle_error (client, error, err);
      return def ? g_strdup (def) : NULL;
    }
}

static gint
gconf_client_get_int_with_default (GConfClient* client, const gchar* key,
                        	   gint def, GError** err)
{
  GError* error = NULL;
  GConfValue* val;

  g_return_val_if_fail (err == NULL || *err == NULL, def);

  val = gconf_client_get (client, key, &error);

  if (val != NULL)
    {
      gint retval = def;

      g_return_val_if_fail (error == NULL, def);
      
      if (check_type (key, val, GCONF_VALUE_INT, &error))
        retval = gconf_value_get_int(val);
      else
        handle_error (client, error, err);

      gconf_value_free (val);

      return retval;
    }
  else
    {
      if (error != NULL)
        handle_error (client, error, err);
      return def;
    }
}

