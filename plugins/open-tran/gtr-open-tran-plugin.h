/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANPOILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __GTR_OPEN_TRAN_PLUGIN_H__
#define __GTR_OPEN_TRAN_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>
#include <libpeas/peas-extension-base.h>
#include <libpeas/peas-object-module.h>

G_BEGIN_DECLS

#define GTR_TYPE_OPEN_TRAN_PLUGIN		(gtr_open_tran_plugin_get_type ())
#define GTR_OPEN_TRAN_PLUGIN(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_OPEN_TRAN_PLUGIN, GtrOpenTranPlugin))
#define GTR_OPEN_TRAN_PLUGIN_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_OPEN_TRAN_PLUGIN, GtrOpenTranPluginClass))
#define GTR_IS_OPEN_TRAN_PLUGIN(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_OPEN_TRAN_PLUGIN))
#define GTR_IS_OPEN_TRAN_PLUGIN_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_OPEN_TRAN_PLUGIN))
#define GTR_OPEN_TRAN_PLUGIN_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_OPEN_TRAN_PLUGIN, GtrOpenTranPluginClass))

typedef struct _GtrOpenTranPlugin        GtrOpenTranPlugin;
typedef struct _GtrOpenTranPluginPrivate GtrOpenTranPluginPrivate;
typedef struct _GtrOpenTranPluginClass   GtrOpenTranPluginClass;

struct _GtrOpenTranPlugin
{
  PeasExtensionBase parent_instance;

  /*< private >*/
  GtrOpenTranPluginPrivate *priv;
};

struct _GtrOpenTranPluginClass
{
  PeasExtensionBaseClass parent_class;
};

GType gtr_open_tran_plugin_get_type (void) G_GNUC_CONST;

G_MODULE_EXPORT void peas_register_types (PeasObjectModule *module);

G_END_DECLS
#endif /* __GTR_OPEN_TRAN_PLUGIN_H__ */
