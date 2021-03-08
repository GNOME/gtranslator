/*
 * (C) 2008 	Pablo Sanxiao <psanxiao@gmail.com>
 *		Igalia
 *
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

typedef struct
{
  /* Identify the profile */
  gchar *name;

  /* Authentication token */
  gchar *auth_token;

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
} GtrProfilePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GtrProfile, gtr_profile, G_TYPE_OBJECT)

static void
gtr_profile_init (GtrProfile *profile)
{
}

static void
gtr_profile_finalize (GObject *object)
{
  GtrProfile *profile = GTR_PROFILE (object);
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);

  g_free (priv->name);
  g_free (priv->auth_token);
  g_free (priv->author_name);
  g_free (priv->author_email);
  g_free (priv->language_name);
  g_free (priv->language_code);
  g_free (priv->charset);
  g_free (priv->encoding);
  g_free (priv->group_email);
  g_free (priv->plural_forms);

  G_OBJECT_CLASS (gtr_profile_parent_class)->finalize (object);
}

static void
gtr_profile_class_init (GtrProfileClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

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
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);
  g_return_val_if_fail (GTR_IS_PROFILE (profile), NULL);

  return priv->name;
}

void
gtr_profile_set_name (GtrProfile  *profile,
                      const gchar *data)
{
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);
  g_return_if_fail (GTR_IS_PROFILE (profile));
  g_return_if_fail (data != NULL);

  g_free (priv->name);
  priv->name = g_strdup (data);
}

const gchar *
gtr_profile_get_auth_token (GtrProfile *profile)
{
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);
  g_return_val_if_fail (GTR_IS_PROFILE (profile), NULL);

  return priv->auth_token;
}

void
gtr_profile_set_auth_token (GtrProfile  *profile,
                            const gchar *data)
{
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);
  g_return_if_fail (GTR_IS_PROFILE (profile));
  g_return_if_fail (data != NULL);

  g_free (priv->auth_token);
  priv->auth_token = g_strdup (data);
}

const gchar *
gtr_profile_get_author_name (GtrProfile *profile)
{
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);
  g_return_val_if_fail (GTR_IS_PROFILE (profile), NULL);

  return priv->author_name;
}

void
gtr_profile_set_author_name (GtrProfile  *profile,
                             const gchar *data)
{
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);
  g_return_if_fail (GTR_IS_PROFILE (profile));
  g_return_if_fail (data != NULL);

  g_free (priv->author_name);
  priv->author_name = g_strdup (data);
}

const gchar *
gtr_profile_get_author_email (GtrProfile *profile)
{
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);
  g_return_val_if_fail (GTR_IS_PROFILE (profile), NULL);

  return priv->author_email;
}

void
gtr_profile_set_author_email (GtrProfile  *profile,
                              const gchar *data)
{
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);
  g_return_if_fail (GTR_IS_PROFILE (profile));
  g_return_if_fail (data != NULL);

  g_free (priv->author_email);
  priv->author_email = g_strdup (data);
}

const gchar *
gtr_profile_get_language_name (GtrProfile *profile)
{
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);
  g_return_val_if_fail (GTR_IS_PROFILE (profile), NULL);

  return priv->language_name;
}

void
gtr_profile_set_language_name (GtrProfile  *profile,
                               const gchar *data)
{
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);
  g_return_if_fail (GTR_IS_PROFILE (profile));
  g_return_if_fail (data != NULL);

  g_free (priv->language_name);
  priv->language_name = g_strdup (data);
}

const gchar *
gtr_profile_get_language_code (GtrProfile *profile)
{
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);
  g_return_val_if_fail (GTR_IS_PROFILE (profile), NULL);

  return priv->language_code;
}

void
gtr_profile_set_language_code (GtrProfile  *profile,
                               const gchar *data)
{
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);
  g_return_if_fail (GTR_IS_PROFILE (profile));
  g_return_if_fail (data != NULL);

  g_free (priv->language_code);
  priv->language_code = g_strdup (data);
}

const gchar *
gtr_profile_get_charset (GtrProfile *profile)
{
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);
  g_return_val_if_fail (GTR_IS_PROFILE (profile), NULL);

  return priv->charset;
}

void
gtr_profile_set_charset (GtrProfile  *profile,
                         const gchar *data)
{
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);
  g_return_if_fail (GTR_IS_PROFILE (profile));
  g_return_if_fail (data != NULL);

  g_free (priv->charset);
  priv->charset = g_strdup (data);
}

const gchar *
gtr_profile_get_encoding (GtrProfile *profile)
{
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);
  g_return_val_if_fail (GTR_IS_PROFILE (profile), NULL);

  return priv->encoding;
}

void
gtr_profile_set_encoding (GtrProfile  *profile,
                          const gchar *data)
{
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);
  g_return_if_fail (GTR_IS_PROFILE (profile));
  g_return_if_fail (data != NULL);

  g_free (priv->encoding);
  priv->encoding = g_strdup (data);
}

const gchar *
gtr_profile_get_group_email (GtrProfile *profile)
{
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);
  g_return_val_if_fail (GTR_IS_PROFILE (profile), NULL);

  return priv->group_email;
}

void
gtr_profile_set_group_email (GtrProfile  *profile,
                             const gchar *data)
{
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);
  g_return_if_fail (GTR_IS_PROFILE (profile));
  g_return_if_fail (data != NULL);

  g_free (priv->group_email);
  priv->group_email = g_strdup (data);
}

const gchar *
gtr_profile_get_plural_forms (GtrProfile *profile)
{
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);
  g_return_val_if_fail (GTR_IS_PROFILE (profile), NULL);

  return priv->plural_forms;
}

void
gtr_profile_set_plural_forms (GtrProfile  *profile,
                              const gchar *data)
{
  GtrProfilePrivate *priv = gtr_profile_get_instance_private (profile);
  g_return_if_fail (GTR_IS_PROFILE (profile));
  g_return_if_fail (data != NULL);

  g_free (priv->plural_forms);
  priv->plural_forms = g_strdup (data);
}