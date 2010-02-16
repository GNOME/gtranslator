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
#include "gtr-profile.h"
#include "gtr-dirs.h"

#include <libxml/xmlreader.h>
#include <libxml/tree.h>

#define GTR_PROFILE_MANAGER_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE((object), \
                                                GTR_TYPE_PROFILE_MANAGER, \
                                                GtrProfileManagerPrivate))

struct _GtrProfileManagerPrivate
{
  GSList     *profiles;
  GtrProfile *active_profile;
};

G_DEFINE_TYPE (GtrProfileManager, gtr_profile_manager, G_TYPE_OBJECT)

static gchar *
get_profile_filename ()
{
  gchar *user_dir;
  gchar *file_name;

  user_dir = gtr_dirs_get_user_config_dir ();
  file_name = g_build_filename (user_dir, "profiles.xml", NULL);
  g_free (user_dir);

  return file_name;
}

static void
gtr_profile_manager_dispose (GObject *object)
{
  GtrProfileManager *manager = GTR_PROFILE_MANAGER (object);

  if (manager->priv->profiles != NULL)
    {
      g_slist_foreach (manager->priv->profiles, (GFunc) g_object_unref, NULL);
      g_slist_free (manager->priv->profiles);
      manager->priv->profiles = NULL;
    }

  G_OBJECT_CLASS (gtr_profile_manager_parent_class)->dispose (object);
}

static void
gtr_profile_manager_class_init (GtrProfileManagerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gtr_profile_manager_dispose;

  g_type_class_add_private (object_class, sizeof (GtrProfileManagerPrivate));
}

static void
parse_profile (GtrProfileManager *manager,
               xmlDocPtr          doc,
               xmlNodePtr         cur)
{
  GtrProfile *profile;

  if (xmlStrcmp (cur->name, (const xmlChar *)"profile") != 0)
    return;

  profile = gtr_profile_new ();

  if (xmlHasProp (cur, (const xmlChar *)"active"))
    manager->priv->active_profile = profile;

  cur = cur->xmlChildrenNode;

  while (cur != NULL)
    {
      xmlChar *content = NULL;

      if (xmlStrcmp (cur->name, (const xmlChar *)"profile_name") == 0)
        {
          content = xmlNodeGetContent (cur);
          gtr_profile_set_name (profile, (const gchar *)content);
        }
      else if (xmlStrcmp (cur->name, (const xmlChar *)"author_name") == 0)
        {
          content = xmlNodeGetContent (cur);
          gtr_profile_set_author_name (profile, (const gchar *)content);
        }
      else if (xmlStrcmp (cur->name, (const xmlChar *)"author_email") == 0)
        {
          content = xmlNodeGetContent (cur);
          gtr_profile_set_author_email (profile, (const gchar *)content);
        }
      else if (xmlStrcmp (cur->name, (const xmlChar *)"language_name") == 0)
        {
          content = xmlNodeGetContent (cur);
          gtr_profile_set_language_name (profile, (const gchar *)content);
        }
      else if (xmlStrcmp (cur->name, (const xmlChar *)"language_code") == 0)
        {
          content = xmlNodeGetContent (cur);
          gtr_profile_set_language_code (profile, (const gchar *)content);
        }
      else if (xmlStrcmp (cur->name, (const xmlChar *)"charset") == 0)
        {
          content = xmlNodeGetContent (cur);
          gtr_profile_set_charset (profile, (const gchar *)content);
        }
      else if (xmlStrcmp (cur->name, (const xmlChar *)"encoding") == 0)
        {
          content = xmlNodeGetContent (cur);
          gtr_profile_set_encoding (profile, (const gchar *)content);
        }
      else if (xmlStrcmp (cur->name, (const xmlChar *)"group_email") == 0)
        {
          content = xmlNodeGetContent (cur);
          gtr_profile_set_group_email (profile, (const gchar *)content);
        }
      else if (xmlStrcmp (cur->name, (const xmlChar *)"plural_forms") == 0)
        {
          content = xmlNodeGetContent (cur);
          gtr_profile_set_plural_forms (profile, (const gchar *)content);
        }

      if (content != NULL)
        xmlFree (content);

      cur = cur->next;
    }

  manager->priv->profiles = g_slist_append (manager->priv->profiles,
                                            profile);
}

static gboolean
load_profiles (GtrProfileManager *manager)
{
  gchar *file_name;
  xmlDocPtr doc;
  xmlNodePtr cur;

  xmlKeepBlanksDefault (0);

  file_name = get_profile_filename ();
  if ((file_name == NULL) ||
      (!g_file_test (file_name, G_FILE_TEST_EXISTS)))
    {
      g_free (file_name);
      return TRUE;
    }

  doc = xmlParseFile (file_name);
  g_free (file_name);

  if (doc == NULL)
    {
      return FALSE;
    }

  cur = xmlDocGetRootElement (doc);
  if (cur == NULL)
    {
      g_message ("The profiles file is empty");
      xmlFreeDoc (doc);

      return FALSE;
    }

  if (xmlStrcmp (cur->name, (const xmlChar *) "profiles"))
    {
      g_message ("Profiles file is of the wrong type");
      xmlFreeDoc (doc);

      return FALSE;
    }

  cur = xmlDocGetRootElement (doc);
  cur = cur->xmlChildrenNode;

  while (cur != NULL)
    {
      parse_profile (manager, doc, cur);

      cur = cur->next;
    }

  xmlFreeDoc (doc);

  return TRUE;
}

static void
save_profile (GtrProfileManager *manager,
              GtrProfile        *profile,
              xmlNodePtr         parent)
{
  xmlNodePtr profile_node;

  profile_node = xmlNewChild (parent, NULL, (const xmlChar *)"profile", NULL);

  if (manager->priv->active_profile == profile)
    xmlSetProp (profile_node, (const xmlChar *)"active", (const xmlChar *)"TRUE");

  xmlNewTextChild (profile_node, NULL, (const xmlChar *)"profile_name",
                   (const xmlChar *)gtr_profile_get_name (profile));
  xmlNewTextChild (profile_node, NULL, (const xmlChar *)"author_name",
                   (const xmlChar *)gtr_profile_get_author_name (profile));
  xmlNewTextChild (profile_node, NULL, (const xmlChar *)"author_email",
                   (const xmlChar *)gtr_profile_get_author_email (profile));
  xmlNewTextChild (profile_node, NULL, (const xmlChar *)"language_name",
                   (const xmlChar *)gtr_profile_get_language_name (profile));
  xmlNewTextChild (profile_node, NULL, (const xmlChar *)"language_code",
                   (const xmlChar *)gtr_profile_get_language_code (profile));
  xmlNewTextChild (profile_node, NULL, (const xmlChar *)"charset",
                   (const xmlChar *)gtr_profile_get_charset (profile));
  xmlNewTextChild (profile_node, NULL, (const xmlChar *)"encoding",
                   (const xmlChar *)gtr_profile_get_encoding (profile));
  xmlNewTextChild (profile_node, NULL, (const xmlChar *)"group_email",
                   (const xmlChar *)gtr_profile_get_group_email (profile));
  xmlNewTextChild (profile_node, NULL, (const xmlChar *)"plural_forms",
                   (const xmlChar *)gtr_profile_get_plural_forms (profile));
}

static void
save_profiles (GtrProfileManager *manager)
{
  xmlDocPtr  doc;
  xmlNodePtr root;
  gchar *file_name;
  GSList *l;

  g_return_if_fail (GTR_IS_PROFILE_MANAGER (manager));

  xmlIndentTreeOutput = TRUE;

  doc = xmlNewDoc ((const xmlChar *)"1.0");
  if (doc == NULL)
    return;

  /* Create metadata root */
  root = xmlNewDocNode (doc, NULL, (const xmlChar *)"profiles", NULL);
  xmlDocSetRootElement (doc, root);

  for (l = manager->priv->profiles; l != NULL; l = g_slist_next (l))
    save_profile (manager, GTR_PROFILE (l->data), root);

  file_name = get_profile_filename ();
  if (file_name != NULL)
    {
      gchar *config_dir;
      int res;

      /* make sure the config dir exists */
      config_dir = gtr_dirs_get_user_config_dir ();
      res = g_mkdir_with_parents (config_dir, 0755);
      if (res != -1)
        xmlSaveFormatFile (file_name, doc, 1);

      g_free (config_dir);
      g_free (file_name);
    }

  xmlFreeDoc (doc);
}

static void
gtr_profile_manager_init (GtrProfileManager *manager)
{
  manager->priv = GTR_PROFILE_MANAGER_GET_PRIVATE (manager);

  manager->priv->profiles = NULL;
  manager->priv->active_profile = NULL;

  load_profiles (manager);
}

GtrProfileManager *
gtr_profile_manager_get_default ()
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
  g_return_val_if_fail (GTR_IS_PROFILE_MANAGER (manager), NULL);

  return manager->priv->profiles;
}

GtrProfile *
gtr_profile_manager_get_active_profile (GtrProfileManager *manager)
{
  g_return_val_if_fail (GTR_IS_PROFILE_MANAGER (manager), NULL);

  return manager->priv->active_profile;
}

void
gtr_profile_manager_set_active_profile (GtrProfileManager *manager,
                                        GtrProfile        *profile)
{
  g_return_if_fail (GTR_IS_PROFILE_MANAGER (manager));
  g_return_if_fail (GTR_IS_PROFILE (profile));

  manager->priv->active_profile = profile;
  save_profiles (manager);
}

void
gtr_profile_manager_add_profile (GtrProfileManager *manager,
                                 GtrProfile        *profile)
{
  g_return_if_fail (GTR_IS_PROFILE_MANAGER (manager));
  g_return_if_fail (profile != NULL);

  if (manager->priv->profiles == NULL)
    manager->priv->active_profile = profile;

  manager->priv->profiles = g_slist_append (manager->priv->profiles,
                                            profile);

  save_profiles (manager);
}

void
gtr_profile_manager_remove_profile (GtrProfileManager *manager,
                                    GtrProfile        *profile)
{
  g_return_if_fail (GTR_IS_PROFILE_MANAGER (manager));
  g_return_if_fail (profile != NULL);

  manager->priv->profiles = g_slist_remove (manager->priv->profiles,
                                            profile);

  save_profiles (manager);
}

void
gtr_profile_manager_modify_profile (GtrProfileManager *manager,
                                    GtrProfile        *old_profile,
                                    GtrProfile        *new_profile)
{
  GSList *p;

  g_return_if_fail (GTR_IS_PROFILE_MANAGER (manager));
  g_return_if_fail (old_profile != NULL);
  g_return_if_fail (new_profile != NULL);

  p = g_slist_find (manager->priv->profiles,
                    old_profile);

  p->data = new_profile;

  if (manager->priv->active_profile == old_profile)
    manager->priv->active_profile = new_profile;

  g_object_unref (old_profile);

  save_profiles (manager);
}
