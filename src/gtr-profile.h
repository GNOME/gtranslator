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
 */

#ifndef __PROFILE_H__
#define __PROFILE_H__

#include <glib.h>
#include <glib-object.h>

/*
 * Utility Macros
 */

#define GTR_TYPE_PROFILE		(gtr_profile_get_type ())
#define GTR_PROFILE(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_PROFILE, GtrProfile))
#define GTR_PROFILE_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_PROFILE, GtrProfileClass))
#define GTR_IS_PROFILE(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_PROFILE))
#define GTR_IS_PROFILE_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_PROFILE))
#define GTR_PROFILE_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_PROFILE, GtrProfileClass))

/*
 * Main object structure
 */
typedef struct _GtrProfile GtrProfile;

struct _GtrProfile
{
  GObject parent_instance;
};

/*
 * Class definition
 */
typedef struct _GtrProfileClass GtrProfileClass;

struct _GtrProfileClass
{
  GObjectClass parent_class;
};

/*
 * Public methods
 */
GType                 gtr_profile_get_type            (void)G_GNUC_CONST;

GtrProfile           *gtr_profile_new                 (void);

const gchar          *gtr_profile_get_name            (GtrProfile  *profile);
void                  gtr_profile_set_name            (GtrProfile  *profile,
                                                       const gchar *data);

const gchar          *gtr_profile_get_auth_token      (GtrProfile  *profile);
void                  gtr_profile_set_auth_token      (GtrProfile  *profile,
                                                       const gchar *data);                                                      

const gchar          *gtr_profile_get_author_name     (GtrProfile  *profile);
void                  gtr_profile_set_author_name     (GtrProfile  *profile,
                                                       const gchar *data);

const gchar          *gtr_profile_get_author_email    (GtrProfile  *profile);
void                  gtr_profile_set_author_email    (GtrProfile  *profile,
                                                       const gchar *data);

const gchar          *gtr_profile_get_language_name   (GtrProfile  *profile);
void                  gtr_profile_set_language_name   (GtrProfile  *profile,
                                                       const gchar *data);

const gchar          *gtr_profile_get_language_code   (GtrProfile  *profile);
void                  gtr_profile_set_language_code   (GtrProfile  *profile,
                                                       const gchar *data);

const gchar          *gtr_profile_get_charset         (GtrProfile  *profile);
void                  gtr_profile_set_charset         (GtrProfile  *profile,
                                                       const gchar *data);

const gchar          *gtr_profile_get_encoding        (GtrProfile  *profile);
void                  gtr_profile_set_encoding        (GtrProfile  *profile,
                                                       const gchar *data);

const gchar          *gtr_profile_get_group_email     (GtrProfile  *profile);
void                  gtr_profile_set_group_email     (GtrProfile  *profile,
                                                       const gchar *data);

const gchar          *gtr_profile_get_plural_forms    (GtrProfile  *profile);
void                  gtr_profile_set_plural_forms    (GtrProfile  *profile,
                                                       const gchar *data);

#endif /* __PROFILE_H__ */
