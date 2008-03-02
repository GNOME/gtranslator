/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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
 */

#ifndef __GTR_Dict_PLUGIN_H__
#define __GTR_Dict_PLUGIN_H__

#include "plugin.h"

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_DICT_PLUGIN		(gtranslator_dict_plugin_get_type ())
#define GTR_DICT_PLUGIN(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_DICT_PLUGIN, GtranslatorDictPlugin))
#define GTR_DICT_PLUGIN_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_Dict_PLUGIN, GtranslatorDictPluginClass))
#define GTR_IS_DICT_PLUGIN(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_Dict_PLUGIN))
#define GTR_IS_DICT_PLUGIN_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_Dict_PLUGIN))
#define GTR_Dict_PLUGIN_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_Dict_PLUGIN, GtranslatorDictPluginClass))

/* Private structure type */
typedef struct _GtranslatorDictPluginPrivate	GtranslatorDictPluginPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorDictPlugin		GtranslatorDictPlugin;

struct _GtranslatorDictPlugin
{
	GtranslatorPlugin parent_instance;
};

/*
 * Class definition
 */
typedef struct _GtranslatorDictPluginClass	GtranslatorDictPluginClass;

struct _GtranslatorDictPluginClass
{
	GtranslatorPluginClass parent_class;
};

/*
 * Public methods
 */
GType	gtranslator_dict_plugin_get_type	(void) G_GNUC_CONST;

/* All the plugins must implement this function */
G_MODULE_EXPORT GType register_gtranslator_plugin (GTypeModule *module);

G_END_DECLS

#endif /* __GTR_Dict_PLUGIN_H__ */
