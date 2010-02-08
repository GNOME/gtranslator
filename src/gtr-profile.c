/*
 * (C) 2008 	Pablo Sanxiao <psanxiao@gmail.com>
 *		Igalia
 *	
 * gtranslator is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or   
 * (at your option) any later version.
 *    
 * gtranslator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *   Pablo Sanxiao <psanxiao@gmail.com>
 */

#include "gtr-application.h"
#include "gtr-profile.h"
#include "gtr-preferences-dialog.h"
#include "gtr-utils.h"

#include <glib.h>
#include <glib-object.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#ifdef G_OS_WIN32
#define SAVE_DATADIR DATADIR
#undef DATADIR
#include <winsock2.h>
#define DATADIR SAVE_DATADIR
#undef SAVE_DATADIR
#else
#include <sys/socket.h>
#endif


#define GTR_PROFILE_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 	(object),	\
					 	GTR_TYPE_PROFILE,     \
					 	GtrProfilePrivate))

G_DEFINE_TYPE (GtrProfile, gtr_profile, G_TYPE_OBJECT)
     struct _GtrProfilePrivate
     {
       /* 
        * Identify the profile
        */
       gchar *name;

       /*
        * Translator's information
        */
       gchar *author_name;
       gchar *author_email;

       /*
        * Complete language name
        */
       gchar *language_name;

       /*
        * Language code. Example: "en" -> English
        */
       gchar *language_code;

       /*
        * Set of characters. Example: UTF-8
        */
       gchar *charset;

       /*
        * Encoding. Example: 8 bits
        */
       gchar *encoding;

       /*
        * Email of the group of translation
        */
       gchar *group_email;

       /*
        * Plural forms
        */
       gchar *plurals;
     };

     static void gtr_profile_init (GtrProfile * profile)
{
  profile->priv = GTR_PROFILE_GET_PRIVATE (profile);
}

static void
gtr_profile_finalize (GObject * object)
{
  GtrProfile *profile = GTR_PROFILE (object);

  g_free (profile->priv->name);
  g_free (profile->priv->author_name);
  g_free (profile->priv->author_email);
  g_free (profile->priv->language_name);
  g_free (profile->priv->language_code);
  g_free (profile->priv->charset);
  g_free (profile->priv->encoding);
  g_free (profile->priv->group_email);
  g_free (profile->priv->plurals);

  G_OBJECT_CLASS (gtr_profile_parent_class)->finalize (object);
}

static void
gtr_profile_class_init (GtrProfileClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrProfilePrivate));

  object_class->finalize = gtr_profile_finalize;
}

/*
 * Public methods
 */

GtrProfile *
gtr_profile_new (void)
{
  GtrProfile *profile;

  profile = g_object_new (GTR_TYPE_PROFILE, NULL);

  return profile;
}

const gchar *
gtr_profile_get_name (GtrProfile * profile)
{
  return profile->priv->name;
}

void
gtr_profile_set_name (GtrProfile * profile, const gchar * data)
{
  if (profile->priv->name)
    g_free (profile->priv->name);
  profile->priv->name = g_strdup (data);
}

const gchar *
gtr_profile_get_author_name (GtrProfile * profile)
{
  return profile->priv->author_name;
}

void
gtr_profile_set_author_name (GtrProfile * profile, const gchar * data)
{
  if (profile->priv->author_name)
    g_free (profile->priv->author_name);
  profile->priv->author_name = g_strdup (data);
}

const gchar *
gtr_profile_get_author_email (GtrProfile * profile)
{
  return profile->priv->author_email;
}

void
gtr_profile_set_author_email (GtrProfile * profile, const gchar * data)
{
  if (profile->priv->author_email)
    g_free (profile->priv->author_email);
  profile->priv->author_email = g_strdup (data);
}

const gchar *
gtr_profile_get_language_name (GtrProfile * profile)
{
  return profile->priv->language_name;
}

void
gtr_profile_set_language_name (GtrProfile * profile, const gchar * data)
{
  if (profile->priv->language_name)
    g_free (profile->priv->language_name);
  profile->priv->language_name = g_strdup (data);
}

const gchar *
gtr_profile_get_language_code (GtrProfile * profile)
{
  return profile->priv->language_code;
}

void
gtr_profile_set_language_code (GtrProfile * profile, const gchar * data)
{
  if (profile->priv->language_code)
    g_free (profile->priv->language_code);
  profile->priv->language_code = g_strdup (data);
}

const gchar *
gtr_profile_get_charset (GtrProfile * profile)
{
  return profile->priv->charset;
}

void
gtr_profile_set_charset (GtrProfile * profile, const gchar * data)
{
  if (profile->priv->charset)
    g_free (profile->priv->charset);
  profile->priv->charset = g_strdup (data);
}

const gchar *
gtr_profile_get_encoding (GtrProfile * profile)
{
  return profile->priv->encoding;
}

void
gtr_profile_set_encoding (GtrProfile * profile, const gchar * data)
{
  if (profile->priv->encoding)
    g_free (profile->priv->encoding);
  profile->priv->encoding = g_strdup (data);
}

const gchar *
gtr_profile_get_group_email (GtrProfile * profile)
{
  return profile->priv->group_email;
}

void
gtr_profile_set_group_email (GtrProfile * profile, const gchar * data)
{
  if (profile->priv->group_email)
    g_free (profile->priv->group_email);
  profile->priv->group_email = g_strdup (data);
}

const gchar *
gtr_profile_get_plurals (GtrProfile * profile)
{
  return profile->priv->plurals;
}

void
gtr_profile_set_plurals (GtrProfile * profile, const gchar * data)
{
  if (profile->priv->plurals)
    g_free (profile->priv->plurals);
  profile->priv->plurals = g_strdup (data);
}

/**
 * gtr_profile_xml_new_entry:
 * @doc: a #xmlDocPtr.
 * @profile: a #GtrProfile object.
 *
 * This function create a new #xmlNodePtr entry into #xmlDocPtr.
 *
 */
void
gtr_profile_xml_new_entry (xmlDocPtr doc, GtrProfile * profile)
{
  xmlNodePtr root;
  xmlNodePtr profile_node;

  root = xmlDocGetRootElement (doc);
  profile_node = xmlNewChild (root, NULL, (const xmlChar *)"profile", NULL);
  xmlNewTextChild (profile_node, NULL, (const xmlChar *)"profile_name", (const xmlChar *)profile->priv->name);
  xmlNewTextChild (profile_node, NULL, (const xmlChar *)"author_name",
                   (const xmlChar *)profile->priv->author_name);
  xmlNewTextChild (profile_node, NULL, (const xmlChar *)"author_email",
                   (const xmlChar *)profile->priv->author_email);
  xmlNewTextChild (profile_node, NULL, (const xmlChar *)"language_name",
                   (const xmlChar *)profile->priv->language_name);
  xmlNewTextChild (profile_node, NULL, (const xmlChar *)"language_code",
                   (const xmlChar *)profile->priv->language_code);
  xmlNewTextChild (profile_node, NULL, (const xmlChar *)"charset", (const xmlChar *)profile->priv->charset);
  xmlNewTextChild (profile_node, NULL, (const xmlChar *)"encoding", (const xmlChar *)profile->priv->encoding);
  xmlNewTextChild (profile_node, NULL, (const xmlChar *)"group_email",
                   (const xmlChar *)profile->priv->group_email);
  xmlNewTextChild (profile_node, NULL, (const xmlChar *)"plurals", (const xmlChar *)profile->priv->plurals);
}

/**
 * gtr_profile_xml_get_entry:
 * @child: a #xmlNodePtr.
 *
 * This function get the values of the #xmlNodePtr and save them into
 * a #GtrProfile object.
 *
 * Returns: a #GtrProfile object.
 */
GtrProfile *
gtr_profile_xml_get_entry (xmlNodePtr child)
{
  xmlNodePtr node;
  GtrProfile *profile;

  profile = gtr_profile_new ();

  node = child->xmlChildrenNode;

  profile->priv->name = (gchar *)xmlNodeGetContent (node);
  node = node->next;
  profile->priv->author_name = (gchar *)xmlNodeGetContent (node);
  node = node->next;
  profile->priv->author_email = (gchar *)xmlNodeGetContent (node);
  node = node->next;
  profile->priv->language_name = (gchar *)xmlNodeGetContent (node);
  node = node->next;
  profile->priv->language_code = (gchar *)xmlNodeGetContent (node);
  node = node->next;
  profile->priv->charset = (gchar *)xmlNodeGetContent (node);
  node = node->next;
  profile->priv->encoding = (gchar *)xmlNodeGetContent (node);
  node = node->next;
  profile->priv->group_email = (gchar *)xmlNodeGetContent (node);
  node = node->next;
  profile->priv->plurals = (gchar *)xmlNodeGetContent (node);

  return profile;
}

/**
 * gtr_profile_get_profiles_from_xml_file:
 * @filename: a filename path.
 *
 * This function get the profiles saved in a xml file
 * and return a #GList of #GtrProfile objects.
 *
 * returns: a #GList
 */
GList *
gtr_profile_get_profiles_from_xml_file (gchar * filename)
{
  GList *profiles_list = NULL;
  GtrProfile *profile;
  xmlNodePtr root, child, active;
  xmlDocPtr doc;
  gchar *active_profile;
  GList *l;

  doc = gtr_xml_open_file (filename);

  root = xmlDocGetRootElement (doc);
  child = root->xmlChildrenNode;
  active = child->xmlChildrenNode;

  active_profile = (gchar *)xmlNodeGetContent (active);
  child = child->next;

  while (child != NULL)
    {
      profile = gtr_profile_xml_get_entry (child);
      profiles_list = g_list_append (profiles_list, profile);
      child = child->next;
    }

  for (l = profiles_list; l; l = l->next)
    {
      GtrProfile *profile;
      profile = (GtrProfile *) l->data;
      if (!strcmp (gtr_profile_get_name (profile), active_profile))
        gtr_application_set_active_profile (GTR_APP, profile);
    }

  return profiles_list;
}

gint
gtr_profile_save_profiles_in_xml (gchar * filename)
{

  xmlNodePtr root;
  xmlDocPtr doc;
  GList *profiles_list, *l;
  GtrProfile *active_profile;

  doc = gtr_xml_new_doc ("list_of_profiles");

  profiles_list = gtr_application_get_profiles (GTR_APP);
  active_profile = gtr_application_get_active_profile (GTR_APP);

  root = xmlDocGetRootElement (doc);

  xmlNewChild (root, NULL, (const xmlChar *)"active", (const xmlChar *)gtr_profile_get_name (active_profile));

  for (l = profiles_list; l; l = l->next)
    {
      GtrProfile *profile;
      profile = (GtrProfile *) l->data;
      gtr_profile_xml_new_entry (doc, profile);
    }

  return xmlSaveFile (filename, doc);
}
