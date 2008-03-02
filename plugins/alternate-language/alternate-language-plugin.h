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
 *     MERCHANPOILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     Based on Gedit alternate_lang plugin
 */

#ifndef __GTR_ALTERNATE_LANG_PLUGIN_H__
#define __GTR_ALTERNATE_LANG_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>

#include "plugin.h"

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_ALTERNATE_LANG_PLUGIN		(gtranslator_alternate_lang_get_type ())
#define GTR_ALTERNATE_LANG_PLUGIN(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_MESSAGEALTERNATE_LANG_PLUGIN, GtranslatorAlternateLangPlugin))
#define GTR_ALTERNATE_LANG_PLUGIN_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_ALTERNATE_LANG_PLUGIN, GtranslatorAlternateLangPluginClass))
#define GTR_IS_ALTERNATE_LANG_PLUGIN(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_ALTERNATE_LANG_PLUGIN))
#define GTR_IS_ALTERNATE_LANG_PLUGIN_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_ALTERNATE_LANG_PLUGIN))
#define GTR_ALTERNATE_LANG_PLUGIN_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_ALTERNATE_LANG_PLUGIN_PLUGIN, GtranslatorAlternateLangPluginClass))

/* Private structure type */
typedef struct _GtranslatorAlternateLangPluginPrivate	GtranslatorAlternateLangPluginPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorAlternateLangPlugin		GtranslatorAlternateLangPlugin;

struct _GtranslatorAlternateLangPlugin
{
	GtranslatorPlugin parent_instance;
};

/*
 * Class definition
 */
typedef struct _GtranslatorAlternateLangPluginClass	GtranslatorAlternateLangPluginClass;

struct _GtranslatorAlternateLangPluginClass
{
	GtranslatorPluginClass parent_class;
};

/*
 * Public methods
 */
GType	gtranslator_alternate_lang_plugin_get_type	(void) G_GNUC_CONST;

/* All the plugins must implement this function */
G_MODULE_EXPORT GType register_gtranslator_plugin (GTypeModule *module);

G_END_DECLS

#endif /* __GTR_ALTERNATE_LANG_PLUGIN_H__ */
