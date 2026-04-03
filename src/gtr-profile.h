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

#pragma once

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define GTR_TYPE_PROFILE (gtr_profile_get_type ())

G_DECLARE_FINAL_TYPE (GtrProfile, gtr_profile, GTR, PROFILE, GObject);

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

G_END_DECLS
