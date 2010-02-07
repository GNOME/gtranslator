/*
 * plugins-engine.h
 * This file is part of gtr
 *
 * Copyright (C) 2002-2005 - Paolo Maggi 
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
 * Modified by the gtr Team, 2002-2005. See the AUTHORS file for a 
 * list of people on the gtr Team.  
 * See the ChangeLog files for a list of changes. 
 *
 * $Id: plugins-engine.h 6264 2008-05-05 11:00:38Z sfre $
 */

#ifndef __GTR_PLUGINS_ENGINE_H__
#define __GTR_PLUGINS_ENGINE_H__

#include <glib.h>
#include "gtr-window.h"
#include "gtr-plugin-info.h"
#include "gtr-plugin.h"

G_BEGIN_DECLS
#define GTR_TYPE_PLUGINS_ENGINE              (gtr_plugins_engine_get_type ())
#define GTR_PLUGINS_ENGINE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), GTR_TYPE_PLUGINS_ENGINE, GtrPluginsEngine))
#define GTR_PLUGINS_ENGINE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GTR_TYPE_PLUGINS_ENGINE, GtrPluginsEngineClass))
#define GTR_IS_PLUGINS_ENGINE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTR_TYPE_PLUGINS_ENGINE))
#define GTR_IS_PLUGINS_ENGINE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_TYPE_PLUGINS_ENGINE))
#define GTR_PLUGINS_ENGINE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GTR_TYPE_PLUGINS_ENGINE, GtrPluginsEngineClass))
typedef struct _GtrPluginsEngine GtrPluginsEngine;
typedef struct _GtrPluginsEnginePrivate
  GtrPluginsEnginePrivate;

struct _GtrPluginsEngine
{
  GObject parent;
  GtrPluginsEnginePrivate *priv;
};

typedef struct _GtrPluginsEngineClass GtrPluginsEngineClass;

struct _GtrPluginsEngineClass
{
  GObjectClass parent_class;

  void (*activate_plugin) (GtrPluginsEngine * engine,
			   GtrPluginInfo * info);

  void (*deactivate_plugin) (GtrPluginsEngine * engine,
			     GtrPluginInfo * info);
};

GType
gtr_plugins_engine_get_type (void)
  G_GNUC_CONST;

     GtrPluginsEngine *gtr_plugins_engine_get_default (void);

     void gtr_plugins_engine_garbage_collect (GtrPluginsEngine
						      * engine);

     const GList
       *gtr_plugins_engine_get_plugin_list (GtrPluginsEngine *
						    engine);

     GtrPluginInfo
       *gtr_plugins_engine_get_plugin_info (GtrPluginsEngine *
						    engine,
						    const gchar * name);

     gboolean
       gtr_plugins_engine_activate_plugin (GtrPluginsEngine *
						   engine,
						   GtrPluginInfo *
						   info);
     gboolean
       gtr_plugins_engine_deactivate_plugin (GtrPluginsEngine
						     * engine,
						     GtrPluginInfo *
						     info);

     void
       gtr_plugins_engine_configure_plugin (GtrPluginsEngine *
						    engine,
						    GtrPluginInfo *
						    info, GtkWindow * parent);

/* 
 * new_window == TRUE if this function is called because a new top window
 * has been created
 */
     void
       gtr_plugins_engine_update_plugins_ui (GtrPluginsEngine
						     * engine,
						     GtrWindow *
						     window,
						     gboolean new_window);

G_END_DECLS
#endif /* __GTR_PLUGINS_ENGINE_H__ */
