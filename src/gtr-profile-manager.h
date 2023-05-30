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


#ifndef __GTR_PROFILE_MANAGER_H__
#define __GTR_PROFILE_MANAGER_H__

#include <glib-object.h>
#include "gtr-profile.h"

G_BEGIN_DECLS

#define GTR_TYPE_PROFILE_MANAGER		(gtr_profile_manager_get_type ())
#define GTR_PROFILE_MANAGER(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_PROFILE_MANAGER, GtrProfileManager))
#define GTR_PROFILE_MANAGER_CONST(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_PROFILE_MANAGER, GtrProfileManager const))
#define GTR_PROFILE_MANAGER_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GTR_TYPE_PROFILE_MANAGER, GtrProfileManagerClass))
#define GTR_IS_PROFILE_MANAGER(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTR_TYPE_PROFILE_MANAGER))
#define GTR_IS_PROFILE_MANAGER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_TYPE_PROFILE_MANAGER))
#define GTR_PROFILE_MANAGER_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GTR_TYPE_PROFILE_MANAGER, GtrProfileManagerClass))

typedef struct _GtrProfileManager		GtrProfileManager;
typedef struct _GtrProfileManagerClass		GtrProfileManagerClass;

struct _GtrProfileManager
{
  GObject parent;
};

struct _GtrProfileManagerClass
{
  GObjectClass parent_class;

  void (* active_profile_changed) (GtrProfileManager *manager,
                                   GtrProfile        *profile);
  void (* profile_added) (GtrProfileManager *manager,
                          GtrProfile        *profile);
  void (* profile_removed) (GtrProfileManager *manager,
                            GtrProfile        *profile);
  void (* profile_modified) (GtrProfileManager *manager,
                             GtrProfile        *old_profile,
                             GtrProfile        *new_profile);
};

GType                  gtr_profile_manager_get_type           (void) G_GNUC_CONST;

GtrProfileManager     *gtr_profile_manager_get_default        (void);

GSList                *gtr_profile_manager_get_profiles       (GtrProfileManager *manager);

GtrProfile            *gtr_profile_manager_get_active_profile (GtrProfileManager *manager);

void                   gtr_profile_manager_set_active_profile (GtrProfileManager *manager,
                                                               GtrProfile        *profile);

void                   gtr_profile_manager_add_profile        (GtrProfileManager *manager,
                                                               GtrProfile        *profile);

void                   gtr_profile_manager_remove_profile     (GtrProfileManager *manager,
                                                               GtrProfile        *profile);

void                   gtr_profile_manager_modify_profile     (GtrProfileManager *manager,
                                                               GtrProfile        *old_profile,
                                                               GtrProfile        *new_profile);

GtrProfile            *gtr_profile_manager_get_profile        (GtrProfileManager *manager,
                                                               const char        *name);

G_END_DECLS

#endif /* __GTR_PROFILE_MANAGER_H__ */
