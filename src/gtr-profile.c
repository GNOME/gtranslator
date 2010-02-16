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
 *   Ignacio Casal Quinteiro <icq@gnome.org>
 */

#include "gtr-application.h"
#include "gtr-profile.h"
#include "gtr-preferences-dialog.h"
#include "gtr-utils.h"

#include <glib.h>

#define GTR_PROFILE_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
                                         (object), \
                                         GTR_TYPE_PROFILE, \
                                         GtrProfilePrivate))

G_DEFINE_TYPE (GtrProfile, gtr_profile, G_TYPE_OBJECT)

struct _GtrProfilePrivate
{
  /* Identify the profile */
  gchar *name;

  /* Translator's information */
  gchar *author_name;
  gchar *author_email;

  /* Complete language name */
  gchar *language_name;

  /* Language code. Example: "en" -> English */
  gchar *language_code;

  /* Set of characters. Example: UTF-8 */
  gchar *charset;

  /* Encoding. Example: 8 bits */
  gchar *encoding;

  /* Email of the group of translation */
  gchar *group_email;

  /* Plural forms */
  gchar *plural_forms;
};

static void
gtr_profile_init (GtrProfile *profile)
{
  profile->priv = GTR_PROFILE_GET_PRIVATE (profile);
}

static void
gtr_profile_finalize (GObject *object)
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
  g_free (profile->priv->plural_forms);

  G_OBJECT_CLASS (gtr_profile_parent_class)->finalize (object);
}

static void
gtr_profile_class_init (GtrProfileClass *klass)
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
  return g_object_new (GTR_TYPE_PROFILE, NULL);
}

const gchar *
gtr_profile_get_name (GtrProfile *profile)
{
  g_return_val_if_fail (GTR_IS_PROFILE (profile), NULL);

  return profile->priv->name;
}

void
gtr_profile_set_name (GtrProfile  *profile,
                      const gchar *data)
{
  g_return_if_fail (GTR_IS_PROFILE (profile));
  g_return_if_fail (data != NULL);

  g_free (profile->priv->name);
  profile->priv->name = g_strdup (data);
}

const gchar *
gtr_profile_get_author_name (GtrProfile *profile)
{
  g_return_val_if_fail (GTR_IS_PROFILE (profile), NULL);

  return profile->priv->author_name;
}

void
gtr_profile_set_author_name (GtrProfile  *profile,
                             const gchar *data)
{
  g_return_if_fail (GTR_IS_PROFILE (profile));
  g_return_if_fail (data != NULL);

  g_free (profile->priv->author_name);
  profile->priv->author_name = g_strdup (data);
}

const gchar *
gtr_profile_get_author_email (GtrProfile *profile)
{
  g_return_val_if_fail (GTR_IS_PROFILE (profile), NULL);

  return profile->priv->author_email;
}

void
gtr_profile_set_author_email (GtrProfile  *profile,
                              const gchar *data)
{
  g_return_if_fail (GTR_IS_PROFILE (profile));
  g_return_if_fail (data != NULL);

  g_free (profile->priv->author_email);
  profile->priv->author_email = g_strdup (data);
}

const gchar *
gtr_profile_get_language_name (GtrProfile *profile)
{
  g_return_val_if_fail (GTR_IS_PROFILE (profile), NULL);

  return profile->priv->language_name;
}

void
gtr_profile_set_language_name (GtrProfile  *profile,
                               const gchar *data)
{
  g_return_if_fail (GTR_IS_PROFILE (profile));
  g_return_if_fail (data != NULL);

  g_free (profile->priv->language_name);
  profile->priv->language_name = g_strdup (data);
}

const gchar *
gtr_profile_get_language_code (GtrProfile *profile)
{
  g_return_val_if_fail (GTR_IS_PROFILE (profile), NULL);

  return profile->priv->language_code;
}

void
gtr_profile_set_language_code (GtrProfile  *profile,
                               const gchar *data)
{
  g_return_if_fail (GTR_IS_PROFILE (profile));
  g_return_if_fail (data != NULL);

  g_free (profile->priv->language_code);
  profile->priv->language_code = g_strdup (data);
}

const gchar *
gtr_profile_get_charset (GtrProfile *profile)
{
  g_return_val_if_fail (GTR_IS_PROFILE (profile), NULL);

  return profile->priv->charset;
}

void
gtr_profile_set_charset (GtrProfile  *profile,
                         const gchar *data)
{
  g_return_if_fail (GTR_IS_PROFILE (profile));
  g_return_if_fail (data != NULL);

  g_free (profile->priv->charset);
  profile->priv->charset = g_strdup (data);
}

const gchar *
gtr_profile_get_encoding (GtrProfile *profile)
{
  g_return_val_if_fail (GTR_IS_PROFILE (profile), NULL);

  return profile->priv->encoding;
}

void
gtr_profile_set_encoding (GtrProfile  *profile,
                          const gchar *data)
{
  g_return_if_fail (GTR_IS_PROFILE (profile));
  g_return_if_fail (data != NULL);

  g_free (profile->priv->encoding);
  profile->priv->encoding = g_strdup (data);
}

const gchar *
gtr_profile_get_group_email (GtrProfile *profile)
{
  g_return_val_if_fail (GTR_IS_PROFILE (profile), NULL);

  return profile->priv->group_email;
}

void
gtr_profile_set_group_email (GtrProfile  *profile,
                             const gchar *data)
{
  g_return_if_fail (GTR_IS_PROFILE (profile));
  g_return_if_fail (data != NULL);

  g_free (profile->priv->group_email);
  profile->priv->group_email = g_strdup (data);
}

const gchar *
gtr_profile_get_plural_forms (GtrProfile *profile)
{
  g_return_val_if_fail (GTR_IS_PROFILE (profile), NULL);

  return profile->priv->plural_forms;
}

void
gtr_profile_set_plural_forms (GtrProfile  *profile,
                              const gchar *data)
{
  g_return_if_fail (GTR_IS_PROFILE (profile));
  g_return_if_fail (data != NULL);

  g_free (profile->priv->plural_forms);
  profile->priv->plural_forms = g_strdup (data);
}
