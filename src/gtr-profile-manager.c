/*
 * gtr-profile-manager.c
 * This file is part of gtranslator
 *
 * Copyright (C) 2010 - Ignacio Casal Quinteiro
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gtr-profile-manager.h"

#include "gtr-dirs.h"
#include "gtr-marshal.h"
#include "gtr-marshal.h"
#include "gtr-profile.h"

typedef struct
{
  GSList     *profiles;
  GtrProfile *active_profile;
} GtrProfileManagerPrivate;

struct _GtrProfileManager
{
  GObject parent_instance;
};

G_DEFINE_FINAL_TYPE_WITH_PRIVATE (GtrProfileManager, gtr_profile_manager, G_TYPE_OBJECT);

enum
{
  ACTIVE_PROFILE_CHANGED,
  PROFILE_ADDED,
  PROFILE_REMOVED,
  PROFILE_MODIFIED,
  LAST_SIGNAL
};

static unsigned int signals[LAST_SIGNAL];

static char *
get_profile_filename (void)
{
  const char *user_dir;
  char *file_name;

  user_dir = gtr_dirs_get_user_config_dir ();
  file_name = g_build_filename (user_dir, "profiles.ini", NULL);

  return file_name;
}

static void
gtr_profile_manager_finalize (GObject *object)
{
  GtrProfileManager *manager = GTR_PROFILE_MANAGER (object);
  GtrProfileManagerPrivate *priv = gtr_profile_manager_get_instance_private (manager);

  g_slist_free_full (priv->profiles, g_object_unref);

  G_OBJECT_CLASS (gtr_profile_manager_parent_class)->finalize (object);
}

static void
gtr_profile_manager_class_init (GtrProfileManagerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gtr_profile_manager_finalize;

  /* Signals */
  signals[ACTIVE_PROFILE_CHANGED] =
    g_signal_new ("active-profile-changed",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL,
                  gtr_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1, GTR_TYPE_PROFILE);
  g_signal_set_va_marshaller (signals[ACTIVE_PROFILE_CHANGED],
                              G_TYPE_FROM_CLASS (klass),
                              gtr_marshal_VOID__OBJECTv);

  signals[PROFILE_ADDED] =
    g_signal_new ("profile-added",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL,
                  gtr_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1, GTR_TYPE_PROFILE);
  g_signal_set_va_marshaller (signals[PROFILE_ADDED],
                              G_TYPE_FROM_CLASS (klass),
                              gtr_marshal_VOID__OBJECTv);

  signals[PROFILE_REMOVED] =
    g_signal_new ("profile-removed",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL,
                  gtr_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1, GTR_TYPE_PROFILE);
  g_signal_set_va_marshaller (signals[PROFILE_REMOVED],
                              G_TYPE_FROM_CLASS (klass),
                              gtr_marshal_VOID__OBJECTv);

  signals[PROFILE_MODIFIED] =
    g_signal_new ("profile-modified",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL,
                  gtr_marshal_VOID__OBJECT_OBJECT,
                  G_TYPE_NONE,
                  2, GTR_TYPE_PROFILE, GTR_TYPE_PROFILE);
  g_signal_set_va_marshaller (signals[PROFILE_MODIFIED],
                              G_TYPE_FROM_CLASS (klass),
                              gtr_marshal_VOID__OBJECT_OBJECTv);
}

static void
parse_profile (GtrProfileManager *manager,
               GKeyFile          *key_file,
               const char        *group)
{
  GtrProfileManagerPrivate *priv = gtr_profile_manager_get_instance_private (manager);
  g_autoptr (GtrProfile) profile = NULL;
  g_autoptr (GError) error = NULL;
  g_autofree char *profile_name = NULL;
  g_autofree char *auth_token = NULL;
  g_autofree char *author_name = NULL;
  g_autofree char *author_email = NULL;
  g_autofree char *language_name = NULL;
  g_autofree char *language_code = NULL;
  g_autofree char *charset = NULL;
  g_autofree char *encoding = NULL;
  g_autofree char *group_email = NULL;
  g_autofree char *plural_forms = NULL;

  g_return_if_fail (group && g_strcmp0 (group, "") != 0);

  profile = gtr_profile_new ();

  gtr_profile_set_name (profile, group);

  if (g_key_file_get_boolean (key_file, group, "active", &error))
    priv->active_profile = profile;

  if (error &&!g_error_matches (error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND))
    return g_warning ("Could not parse profile %s's 'active' key: %s", group, error->message);

  g_clear_error (&error);

  auth_token = g_key_file_get_string (key_file, group, "auth_token", &error);
  if (auth_token && g_strcmp0 (auth_token, "") != 0)
    gtr_profile_set_auth_token (profile, auth_token);

  if (error &&!g_error_matches (error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND))
    return g_warning ("Could not parse profile %s's 'auth_token' key: %s", group, error->message);

  g_clear_error (&error);

  author_name = g_key_file_get_string (key_file, group, "author_name", &error);
  if (author_name && g_strcmp0 (author_name, "") != 0)
    gtr_profile_set_author_name (profile, author_name);

  if (error &&!g_error_matches (error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND))
    return g_warning ("Could not parse profile %s's 'author_name' key: %s", group, error->message);

  g_clear_error (&error);

  author_email = g_key_file_get_string (key_file, group, "author_email", &error);
  if (author_email && g_strcmp0 (author_email, "") != 0)
    gtr_profile_set_author_email (profile, author_email);

  if (error &&!g_error_matches (error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND))
    return g_warning ("Could not parse profile %s's 'author_email' key: %s", group, error->message);

  g_clear_error (&error);

  language_name = g_key_file_get_string (key_file, group, "language_name", &error);
  if (language_name && g_strcmp0 (language_name, "") != 0)
    gtr_profile_set_language_name (profile, language_name);

  if (error &&!g_error_matches (error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND))
    return g_warning ("Could not parse profile %s's 'language_name' key: %s", group, error->message);

  g_clear_error (&error);

  language_code = g_key_file_get_string (key_file, group, "language_code", &error);
  if (language_code && g_strcmp0 (language_code, "") != 0)
    gtr_profile_set_language_code (profile, language_code);

  if (error &&!g_error_matches (error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND))
    return g_warning ("Could not parse profile %s's 'language_code' key: %s", group, error->message);

  g_clear_error (&error);

  charset = g_key_file_get_string (key_file, group, "charset", &error);
  if (charset && g_strcmp0 (charset, "") != 0)
    gtr_profile_set_charset (profile, charset);

  if (error &&!g_error_matches (error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND))
    return g_warning ("Could not parse profile %s's 'charset' key: %s", group, error->message);

  g_clear_error (&error);

  encoding = g_key_file_get_string (key_file, group, "encoding", &error);
  if (encoding && g_strcmp0 (encoding, "") != 0)
    gtr_profile_set_encoding (profile, encoding);

  if (error &&!g_error_matches (error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND))
    return g_warning ("Could not parse profile %s's 'encoding' key: %s", group, error->message);

  g_clear_error (&error);

  group_email = g_key_file_get_string (key_file, group, "group_email", &error);
  if (group_email && g_strcmp0 (group_email, "") != 0)
    gtr_profile_set_group_email (profile, group_email);

  if (error &&!g_error_matches (error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND))
    return g_warning ("Could not parse profile %s's 'group_email' key: %s", group, error->message);

  g_clear_error (&error);

  plural_forms = g_key_file_get_string (key_file, group, "plural_forms", &error);
  if (plural_forms && g_strcmp0 (plural_forms, "") != 0)
    gtr_profile_set_plural_forms (profile, plural_forms);

  if (error &&!g_error_matches (error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND))
    return g_warning ("Could not parse profile %s's 'plural_forms' key: %s", group, error->message);

  priv->profiles = g_slist_append (priv->profiles, g_steal_pointer (&profile));
}

static gboolean
load_profiles (GtrProfileManager *manager)
{
  g_autofree char *file_name = NULL;
  g_auto (GStrv) groups = NULL;
  size_t n_groups = 0;

  g_autoptr(GError) error = NULL;
  g_autoptr(GKeyFile) key_file = g_key_file_new ();

  file_name = get_profile_filename ();
  if ((file_name == NULL) ||
      (!g_file_test (file_name, G_FILE_TEST_EXISTS)))
    return TRUE;

  if (!g_key_file_load_from_file (key_file, file_name, G_KEY_FILE_NONE, &error))  {
    if (!g_error_matches (error, G_FILE_ERROR, G_FILE_ERROR_NOENT))
      g_warning ("Error loading key file: %s", error->message);
    return FALSE;
  }

  groups = g_key_file_get_groups (key_file, &n_groups);

  for (int i = 0; i < n_groups; i++)
    parse_profile (manager, key_file, groups[i]);

  return TRUE;
}

static void
save_profile (GtrProfileManager *manager,
              GKeyFile          *key_file,
              GtrProfile        *profile)
{
  GtrProfileManagerPrivate *priv = gtr_profile_manager_get_instance_private (manager);
  const char *group = gtr_profile_get_name (profile);

  if (priv->active_profile == profile)
    g_key_file_set_boolean (key_file, group, "active", true);

  const char *auth_token = gtr_profile_get_auth_token (profile);
  if (auth_token && g_strcmp0 (auth_token, "") != 0)
    g_key_file_set_string (key_file, group, "auth_token", auth_token);

  const char *author_name = gtr_profile_get_author_name (profile);
  if (author_name && g_strcmp0 (author_name, "") != 0)
    g_key_file_set_string (key_file, group, "author_name", author_name);

  const char *author_email = gtr_profile_get_author_email (profile);
  if (author_email && g_strcmp0 (author_email, "") != 0)
    g_key_file_set_string (key_file, group, "author_email", author_email);

  const char *language_name = gtr_profile_get_language_name (profile);
  if (language_name && g_strcmp0 (language_name, "") != 0)
    g_key_file_set_string (key_file, group, "language_name", language_name);

  const char *language_code = gtr_profile_get_language_code (profile);
  if (language_code && g_strcmp0 (language_code, "") != 0)
    g_key_file_set_string (key_file, group, "language_code", language_code);

  const char *charset = gtr_profile_get_charset (profile);
  if (charset && g_strcmp0 (charset, "") != 0)
    g_key_file_set_string (key_file, group, "charset", charset);

  const char *encoding = gtr_profile_get_encoding (profile);
  if (encoding && g_strcmp0 (encoding, "") != 0)
    g_key_file_set_string (key_file, group, "encoding", encoding);

  const char *group_email = gtr_profile_get_group_email (profile);
  if (group_email && g_strcmp0 (group_email, "") != 0)
    g_key_file_set_string (key_file, group, "group_email", group_email);

  const char *plural_forms = gtr_profile_get_plural_forms (profile);
  if (plural_forms && g_strcmp0 (plural_forms, "") != 0)
    g_key_file_set_string (key_file, group, "plural_forms", plural_forms);
}

static void
save_profiles (GtrProfileManager *manager)
{
  g_autofree char *file_name = NULL;
  GSList *l;
  GtrProfileManagerPrivate *priv = gtr_profile_manager_get_instance_private (manager);
  g_autoptr (GKeyFile) key_file = g_key_file_new ();
  g_autoptr (GError) error = NULL;

  g_return_if_fail (GTR_IS_PROFILE_MANAGER (manager));

  for (l = priv->profiles; l != NULL; l = g_slist_next (l))
    save_profile (manager, key_file, GTR_PROFILE (l->data));

  file_name = get_profile_filename ();
  if (file_name != NULL && !g_key_file_save_to_file (key_file, file_name, &error))
    g_warning ("Could not save profiles.ini: %s", error->message);
}

static void
gtr_profile_manager_init (GtrProfileManager *manager)
{
  GtrProfileManagerPrivate *priv = gtr_profile_manager_get_instance_private (manager);

  priv->profiles = NULL;
  priv->active_profile = NULL;

  load_profiles (manager);
}

GtrProfileManager *
gtr_profile_manager_get_default (void)
{
  static GtrProfileManager *manager = NULL;

  if (manager == NULL)
    {
      manager = g_object_new (GTR_TYPE_PROFILE_MANAGER, NULL);
      return manager;
    }

  return g_object_ref (manager);
}

GSList *
gtr_profile_manager_get_profiles (GtrProfileManager *manager)
{
  GtrProfileManagerPrivate *priv = gtr_profile_manager_get_instance_private (manager);
  g_return_val_if_fail (GTR_IS_PROFILE_MANAGER (manager), NULL);

  return priv->profiles;
}

GtrProfile *
gtr_profile_manager_get_active_profile (GtrProfileManager *manager)
{
  GtrProfileManagerPrivate *priv = gtr_profile_manager_get_instance_private (manager);
  g_return_val_if_fail (GTR_IS_PROFILE_MANAGER (manager), NULL);

  return priv->active_profile;
}

void
gtr_profile_manager_set_active_profile (GtrProfileManager *manager,
                                        GtrProfile        *profile)
{
  GtrProfileManagerPrivate *priv = gtr_profile_manager_get_instance_private (manager);
  g_return_if_fail (GTR_IS_PROFILE_MANAGER (manager));
  g_return_if_fail (GTR_IS_PROFILE (profile));

  priv->active_profile = profile;

  g_signal_emit (G_OBJECT (manager), signals[ACTIVE_PROFILE_CHANGED], 0, profile);
  save_profiles (manager);
}

void
gtr_profile_manager_add_profile (GtrProfileManager *manager,
                                 GtrProfile        *profile)
{
  GtrProfileManagerPrivate *priv = gtr_profile_manager_get_instance_private (manager);
  g_return_if_fail (GTR_IS_PROFILE_MANAGER (manager));
  g_return_if_fail (profile != NULL);

  if (priv->profiles == NULL)
    priv->active_profile = profile;

  priv->profiles = g_slist_append (priv->profiles,
                                            profile);

  g_signal_emit (G_OBJECT (manager), signals[PROFILE_ADDED], 0, profile);

  save_profiles (manager);
}

void
gtr_profile_manager_remove_profile (GtrProfileManager *manager,
                                    GtrProfile        *profile)
{
  GtrProfileManagerPrivate *priv = gtr_profile_manager_get_instance_private (manager);
  g_return_if_fail (GTR_IS_PROFILE_MANAGER (manager));
  g_return_if_fail (profile != NULL);

  priv->profiles = g_slist_remove (priv->profiles,
                                            profile);

  g_signal_emit (G_OBJECT (manager), signals[PROFILE_REMOVED], 0, profile);
  g_object_unref (profile);

  save_profiles (manager);
}

void
gtr_profile_manager_modify_profile (GtrProfileManager *manager,
                                    GtrProfile        *old_profile,
                                    GtrProfile        *new_profile)
{
  GtrProfileManagerPrivate *priv = gtr_profile_manager_get_instance_private (manager);
  GSList *p;

  g_return_if_fail (GTR_IS_PROFILE_MANAGER (manager));
  g_return_if_fail (old_profile != NULL);
  g_return_if_fail (new_profile != NULL);

  p = g_slist_find (priv->profiles,
                    old_profile);

  p->data = new_profile;

  if (priv->active_profile == old_profile)
    priv->active_profile = new_profile;

  g_signal_emit (G_OBJECT (manager), signals[PROFILE_MODIFIED], 0,
                 old_profile, new_profile);

  g_object_unref (old_profile);

  save_profiles (manager);
}

GtrProfile *
gtr_profile_manager_get_profile (GtrProfileManager *manager,
                                 const char        *name)
{
  GSList *l = NULL;

  GtrProfileManagerPrivate *priv = gtr_profile_manager_get_instance_private (manager);

  for (l = priv->profiles; l != NULL; l = g_slist_next (l))
    {
      GtrProfile *p = GTR_PROFILE (l->data);
      const char *pname = gtr_profile_get_name (p);
      if (g_strcmp0 (name, pname) == 0)
        return p;
    }

  return NULL;
}
