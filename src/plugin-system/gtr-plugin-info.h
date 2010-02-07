/*
 * plugin-info.h
 * This file is part of gtr
 *
 * Copyright (C) 2002-2005 - Paolo Maggi 
 * Copyright (C) 2007 - Paolo Maggi, Steve Frécinaux
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the gtr Team, 2002-2007. See the AUTHORS file for a
 * list of people on the gtr Team.
 * See the ChangeLog files for a list of changes.
 *
 * $Id$
 */

#ifndef __GTR_PLUGIN_INFO_H__
#define __GTR_PLUGIN_INFO_H__

#include <glib-object.h>

#include "gtr-module.h"

G_BEGIN_DECLS
#define GTR_TYPE_PLUGIN_INFO			(gtr_plugin_info_get_type ())
#define GTR_PLUGIN_INFO(obj)			((GtrPluginInfo *) (obj))
typedef struct _GtrPluginInfo GtrPluginInfo;

GType
gtr_plugin_info_get_type (void)
  G_GNUC_CONST;

     gboolean gtr_plugin_info_is_active (GtrPluginInfo * info);
     gboolean gtr_plugin_info_is_available (GtrPluginInfo * info);
     gboolean gtr_plugin_info_is_configurable (GtrPluginInfo * info);

     const gchar *gtr_plugin_info_get_name (GtrPluginInfo * info);
     const gchar * gtr_plugin_info_get_description (GtrPluginInfo * info);
     const gchar *gtr_plugin_info_get_icon_name (GtrPluginInfo * info);
     const gchar **gtr_plugin_info_get_authors (GtrPluginInfo * info);
     const gchar *gtr_plugin_info_get_website (GtrPluginInfo * info);
     const gchar *gtr_plugin_info_get_copyright (GtrPluginInfo * info);
     const gchar *gtr_plugin_info_get_license (GtrPluginInfo * info);

G_END_DECLS
#endif /* __GTR_PLUGIN_INFO_H__ */
