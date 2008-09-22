/*
 * (C) 2008 	Pablo Sanxiao <psanxiao@gmail.com>
 *			
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *    
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "application.h" 
#include "profile.h"
#include "preferences-dialog.h"
#include "utils.h"

#include <glib.h>
#include <glib-object.h>

#define GTR_PROFILE_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 	(object),	\
					 	GTR_TYPE_PROFILE,     \
					 	GtranslatorProfilePrivate))

G_DEFINE_TYPE(GtranslatorProfile, gtranslator_profile, G_TYPE_OBJECT)

struct _GtranslatorProfilePrivate
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

static void gtranslator_profile_init (GtranslatorProfile *profile)
{
	profile->priv = GTR_PROFILE_GET_PRIVATE (profile);
}

static void gtranslator_profile_finalize (GObject *object)
{
	GtranslatorProfile *profile = GTR_PROFILE(object);
	
	g_free (profile->priv->name);
	g_free (profile->priv->author_name);
	g_free (profile->priv->author_email);
	g_free (profile->priv->language_name);
	g_free (profile->priv->language_code);
	g_free (profile->priv->charset);
	g_free (profile->priv->encoding);
	g_free (profile->priv->group_email);
	g_free (profile->priv->plurals);
	
	G_OBJECT_CLASS (gtranslator_profile_parent_class)->finalize (object);
}

static void gtranslator_profile_class_init (GtranslatorProfileClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorProfilePrivate));

	object_class->finalize = gtranslator_profile_finalize;
}

/*
 * Public methods
 */

GtranslatorProfile *gtranslator_profile_new (void)
{
	GtranslatorProfile *profile;
	
	profile = g_object_new (GTR_TYPE_PROFILE, NULL);
	
	return profile;
}

gchar *gtranslator_profile_get_name (GtranslatorProfile *profile)
{
	return profile->priv->name;
}

void gtranslator_profile_set_name (GtranslatorProfile *profile, gchar *data)
{
	if (profile->priv->name)
		g_free (profile->priv->name);
	profile->priv->name = g_strdup (data);
}

gchar *gtranslator_profile_get_author_name (GtranslatorProfile *profile)
{
	return profile->priv->author_name;
}

void gtranslator_profile_set_author_name (GtranslatorProfile *profile, gchar *data)
{
	if (profile->priv->author_name)
		g_free (profile->priv->author_name);
	profile->priv->author_name = g_strdup (data);
}

gchar *gtranslator_profile_get_author_email (GtranslatorProfile *profile)
{
	return profile->priv->author_email;
}

void gtranslator_profile_set_author_email (GtranslatorProfile *profile, gchar *data)
{
	if (profile->priv->author_email)
		g_free (profile->priv->author_email);
	profile->priv->author_email = g_strdup (data);
}

gchar *gtranslator_profile_get_language_name (GtranslatorProfile *profile)
{
	return profile->priv->language_name;
}

void gtranslator_profile_set_language_name (GtranslatorProfile *profile, gchar *data)
{
	if (profile->priv->language_name)
		g_free (profile->priv->language_name);
	profile->priv->language_name = g_strdup (data);
}

gchar *gtranslator_profile_get_language_code (GtranslatorProfile *profile)
{
	return profile->priv->language_code;
}

void gtranslator_profile_set_language_code (GtranslatorProfile *profile, gchar *data)
{
	if (profile->priv->language_code)
		g_free (profile->priv->language_code);
	profile->priv->language_code = g_strdup (data);
}

gchar *gtranslator_profile_get_charset (GtranslatorProfile *profile)
{
	return profile->priv->charset;
}

void gtranslator_profile_set_charset (GtranslatorProfile *profile, gchar *data)
{
	if (profile->priv->charset)
		g_free (profile->priv->charset);
	profile->priv->charset = g_strdup (data);
}

gchar *gtranslator_profile_get_encoding (GtranslatorProfile *profile)
{
	return profile->priv->encoding;
}

void gtranslator_profile_set_encoding (GtranslatorProfile *profile, gchar *data)
{
	if (profile->priv->encoding)
		g_free (profile->priv->encoding);
	profile->priv->encoding = g_strdup (data);
}

gchar *gtranslator_profile_get_group_email (GtranslatorProfile *profile)
{
	return profile->priv->group_email;
}

void gtranslator_profile_set_group_email (GtranslatorProfile *profile, gchar *data)
{
	if (profile->priv->group_email)
		g_free (profile->priv->group_email);
	profile->priv->group_email = g_strdup (data);
}

gchar *gtranslator_profile_get_plurals (GtranslatorProfile *profile)
{
	return profile->priv->plurals;
}

void gtranslator_profile_set_plurals (GtranslatorProfile *profile, gchar *data)
{
	if (profile->priv->plurals)
		g_free (profile->priv->plurals);
	profile->priv->plurals = g_strdup (data);
}

/**
 * gtranslator_profile_xml_new_entry:
 * @doc: a #xmlDocPtr.
 * @profile: a #GtranslatorProfile object.
 *
 * This function create a new #xmlNodePtr entry into #xmlDocPtr.
 *
 */
void gtranslator_profile_xml_new_entry (xmlDocPtr doc, GtranslatorProfile *profile)
{
	xmlNodePtr root;
	xmlNodePtr profile_node;
	
	root = xmlDocGetRootElement (doc);
	profile_node = xmlNewChild (root, NULL, "profile", NULL);
	xmlNewTextChild (profile_node, NULL, "profile_name", profile->priv->name);
	xmlNewTextChild (profile_node, NULL, "author_name", profile->priv->author_name);
	xmlNewTextChild (profile_node, NULL, "author_email", profile->priv->author_email);
	xmlNewTextChild (profile_node, NULL, "language_name", profile->priv->language_name);
	xmlNewTextChild (profile_node, NULL, "language_code", profile->priv->language_code);
	xmlNewTextChild (profile_node, NULL, "charset", profile->priv->charset);
	xmlNewTextChild (profile_node, NULL, "encoding", profile->priv->encoding);
	xmlNewTextChild (profile_node, NULL, "group_email", profile->priv->group_email);
	xmlNewTextChild (profile_node, NULL, "plurals", profile->priv->plurals);
}

/**
 * gtranslator_profile_xml_get_entry:
 * @child: a #xmlNodePtr.
 *
 * This function get the values of the #xmlNodePtr and save them into
 * a #GtranslatorProfile object.
 *
 * Returns: a #GtranslatorProfile object.
 */ 
GtranslatorProfile *gtranslator_profile_xml_get_entry (xmlNodePtr child)
{
  xmlNodePtr node;
  GtranslatorProfile *profile;

  profile = gtranslator_profile_new ();

  node = child->xmlChildrenNode;

  profile->priv->name = xmlNodeGetContent (node);
  node = node->next;
  profile->priv->author_name = xmlNodeGetContent (node);
  node = node->next;
  profile->priv->author_email = xmlNodeGetContent (node);
  node = node->next;
  profile->priv->language_name = xmlNodeGetContent (node);
  node = node->next;
  profile->priv->language_code = xmlNodeGetContent (node);
  node = node->next;
  profile->priv->charset = xmlNodeGetContent (node);
  node = node->next;
  profile->priv->encoding = xmlNodeGetContent (node);
  node = node->next;
  profile->priv->group_email = xmlNodeGetContent (node);
  node = node->next;
  profile->priv->plurals = xmlNodeGetContent (node);

  return profile;
}

/**
 * gtranslator_profile_get_profiles_from_xml_file:
 * @filename: a filename path.
 *
 * This function get the profiles saved in a xml file
 * and return a #GList of #GtranslatorProfile objects.
 *
 * returns: a #GList
 */  
GList *gtranslator_profile_get_profiles_from_xml_file (gchar *filename)
{
  GList *profiles_list = NULL;
  GtranslatorProfile *profile;
  xmlNodePtr root, child, active;
  xmlDocPtr doc;
  gchar *active_profile;
  GList *l;

  doc = gtranslator_xml_open_file (filename);

  root = xmlDocGetRootElement (doc);
  child = root->xmlChildrenNode;
  active = child->xmlChildrenNode;

  active_profile = xmlNodeGetContent (active);
  child = child->next;

  while (child != NULL) {
    profile = gtranslator_profile_xml_get_entry (child);
    profiles_list = g_list_append (profiles_list, profile);
    child = child->next;
  }
  
  for (l = profiles_list; l; l = l->next) {
    GtranslatorProfile *profile;
    profile = (GtranslatorProfile *)l->data;
    if (!strcmp(gtranslator_profile_get_name (profile), active_profile))
      gtranslator_application_set_active_profile (GTR_APP, profile);
  }
  
  return profiles_list;
}

void gtranslator_profile_save_profiles_in_xml (gchar *filename) {
  
  xmlNodePtr root, child, active;
  xmlDocPtr doc;
  GList *profiles_list, *l;
  GtranslatorProfile *active_profile;

  doc = gtranslator_xml_new_doc ("list_of_profiles");
  
  profiles_list = gtranslator_application_get_profiles (GTR_APP);
  active_profile = gtranslator_application_get_active_profile (GTR_APP);

  root = xmlDocGetRootElement (doc);
  child = root->xmlChildrenNode;
  active = child->xmlChildrenNode;

  xmlNewChild (root, NULL, "active", gtranslator_profile_get_name (active_profile));
  
  for (l = profiles_list; l; l = l->next) {
    GtranslatorProfile *profile;
    profile = (GtranslatorProfile *)l->data;
    gtranslator_profile_xml_new_entry (doc, profile);
  }
  
  xmlSaveFile (filename, doc);
}
