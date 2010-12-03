/*
 * gtr-charmap-plugin.h - Character map side-pane for gtr
 * 
 * Copyright (C) 2006 Steve Frécinaux
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * $Id: gtr-charmap-plugin.h 137 2006-04-23 15:13:27Z sfre $
 */

#ifndef __GTR_CHARMAP_PLUGIN_H__
#define __GTR_CHARMAP_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>
#include <libpeas/peas-extension-base.h>
#include <libpeas/peas-object-module.h>

G_BEGIN_DECLS

#define GTR_TYPE_CHARMAP_PLUGIN         (gtr_charmap_plugin_get_type ())
#define GTR_CHARMAP_PLUGIN(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_CHARMAP_PLUGIN, GtrCharmapPlugin))
#define GTR_CHARMAP_PLUGIN_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_CHARMAP_PLUGIN, GtrCharmapPluginClass))
#define GTR_IS_CHARMAP_PLUGIN(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_CHARMAP_PLUGIN))
#define GTR_IS_CHARMAP_PLUGIN_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_CHARMAP_PLUGIN))
#define GTR_CHARMAP_PLUGIN_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_CHARMAP_PLUGIN, GtrCharmapPluginClass))


typedef struct _GtrCharmapPlugin        GtrCharmapPlugin;
typedef struct _GtrCharmapPluginPrivate GtrCharmapPluginPrivate;
typedef struct _GtrCharmapPluginClass   GtrCharmapPluginClass;

struct _GtrCharmapPlugin
{
  PeasExtensionBase parent_instance;

  /*< private >*/
  GtrCharmapPluginPrivate *priv;
};


struct _GtrCharmapPluginClass
{
  PeasExtensionBaseClass parent_class;
};

GType                   gtr_charmap_plugin_get_type     (void) G_GNUC_CONST;

G_MODULE_EXPORT void    peas_register_types             (PeasObjectModule *module);

G_END_DECLS

#endif /* __GTR_CHARMAP_PLUGIN_H__ */
