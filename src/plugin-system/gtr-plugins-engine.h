/*
 * plugins-engine.h
 * This file is part of gtranslator
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
 * Modified by the gtranslator Team, 2002-2005. See the AUTHORS file for a 
 * list of people on the gtranslator Team.  
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
#define GTR_TYPE_PLUGINS_ENGINE              (gtranslator_plugins_engine_get_type ())
#define GTR_PLUGINS_ENGINE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), GTR_TYPE_PLUGINS_ENGINE, GtranslatorPluginsEngine))
#define GTR_PLUGINS_ENGINE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GTR_TYPE_PLUGINS_ENGINE, GtranslatorPluginsEngineClass))
#define GTR_IS_PLUGINS_ENGINE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTR_TYPE_PLUGINS_ENGINE))
#define GTR_IS_PLUGINS_ENGINE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_TYPE_PLUGINS_ENGINE))
#define GTR_PLUGINS_ENGINE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GTR_TYPE_PLUGINS_ENGINE, GtranslatorPluginsEngineClass))
typedef struct _GtranslatorPluginsEngine GtranslatorPluginsEngine;
typedef struct _GtranslatorPluginsEnginePrivate
  GtranslatorPluginsEnginePrivate;

struct _GtranslatorPluginsEngine
{
  GObject parent;
  GtranslatorPluginsEnginePrivate *priv;
};

typedef struct _GtranslatorPluginsEngineClass GtranslatorPluginsEngineClass;

struct _GtranslatorPluginsEngineClass
{
  GObjectClass parent_class;

  void (*activate_plugin) (GtranslatorPluginsEngine * engine,
			   GtranslatorPluginInfo * info);

  void (*deactivate_plugin) (GtranslatorPluginsEngine * engine,
			     GtranslatorPluginInfo * info);
};

GType
gtranslator_plugins_engine_get_type (void)
  G_GNUC_CONST;

     GtranslatorPluginsEngine *gtranslator_plugins_engine_get_default (void);

     void gtranslator_plugins_engine_garbage_collect (GtranslatorPluginsEngine
						      * engine);

     const GList
       *gtranslator_plugins_engine_get_plugin_list (GtranslatorPluginsEngine *
						    engine);

     GtranslatorPluginInfo
       *gtranslator_plugins_engine_get_plugin_info (GtranslatorPluginsEngine *
						    engine,
						    const gchar * name);

     gboolean
       gtranslator_plugins_engine_activate_plugin (GtranslatorPluginsEngine *
						   engine,
						   GtranslatorPluginInfo *
						   info);
     gboolean
       gtranslator_plugins_engine_deactivate_plugin (GtranslatorPluginsEngine
						     * engine,
						     GtranslatorPluginInfo *
						     info);

     void
       gtranslator_plugins_engine_configure_plugin (GtranslatorPluginsEngine *
						    engine,
						    GtranslatorPluginInfo *
						    info, GtkWindow * parent);

/* 
 * new_window == TRUE if this function is called because a new top window
 * has been created
 */
     void
       gtranslator_plugins_engine_update_plugins_ui (GtranslatorPluginsEngine
						     * engine,
						     GtranslatorWindow *
						     window,
						     gboolean new_window);

G_END_DECLS
#endif /* __GTR_PLUGINS_ENGINE_H__ */
