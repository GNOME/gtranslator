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
 
#include "profile.h"
#include "preferences-dialog.h"

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
