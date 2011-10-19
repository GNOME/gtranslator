/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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
 */

#ifndef __GTR_CODE_VIEW_PLUGIN_H__
#define __GTR_CODE_VIEW_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>
#include <libpeas/peas-extension-base.h>
#include <libpeas/peas-object-module.h>

G_BEGIN_DECLS

#define GTR_TYPE_CODE_VIEW_PLUGIN		(gtr_code_view_plugin_get_type ())
#define GTR_CODE_VIEW_PLUGIN(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_CODE_VIEW_PLUGIN, GtrCodeViewPlugin))
#define GTR_CODE_VIEW_PLUGIN_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_CODE_VIEW_PLUGIN, GtrCodeViewPluginClass))
#define GTR_IS_CODE_VIEW_PLUGIN(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_CODE_VIEW_PLUGIN))
#define GTR_IS_CODE_VIEW_PLUGIN_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_CODE_VIEW_PLUGIN))
#define GTR_CODE_VIEW_PLUGIN_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_CODE_VIEW_PLUGIN_PLUGIN, GtrCodeViewPluginClass))

typedef struct _GtrCodeViewPlugin        GtrCodeViewPlugin;
typedef struct _GtrCodeViewPluginPrivate GtrCodeViewPluginPrivate;
typedef struct _GtrCodeViewPluginClass   GtrCodeViewPluginClass;

struct _GtrCodeViewPlugin
{
  PeasExtensionBase parent_instance;

  /* private */
  GtrCodeViewPluginPrivate *priv;
};

struct _GtrCodeViewPluginClass
{
  PeasExtensionBaseClass parent_class;
};

GType gtr_code_view_plugin_get_type (void) G_GNUC_CONST;

G_MODULE_EXPORT void peas_register_types (PeasObjectModule *module);

G_END_DECLS
#endif /* __GTR_CODE_VIEW_PLUGIN_H__ */
