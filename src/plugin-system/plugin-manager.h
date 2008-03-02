/*
 * plugin-manager.h
 * This file is part of gtranslator
 *
 * Copyright (C) 2002-2005 Paolo Maggi
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
 * $Id: plugin-manager.h 5666 2007-06-29 19:52:25Z sfre $
 */

#ifndef __GTR_PLUGIN_MANAGER_H__
#define __GTR_PLUGIN_MANAGER_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_PLUGIN_MANAGER              (gtranslator_plugin_manager_get_type())
#define GTR_PLUGIN_MANAGER(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), GTR_TYPE_PLUGIN_MANAGER, GtranslatorPluginManager))
#define GTR_PLUGIN_MANAGER_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GTR_TYPE_PLUGIN_MANAGER, GtranslatorPluginManagerClass))
#define GTR_IS_PLUGIN_MANAGER(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTR_TYPE_PLUGIN_MANAGER))
#define GTR_IS_PLUGIN_MANAGER_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_TYPE_PLUGIN_MANAGER))
#define GTR_PLUGIN_MANAGER_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GTR_TYPE_PLUGIN_MANAGER, GtranslatorPluginManagerClass))

/* Private structure type */
typedef struct _GtranslatorPluginManagerPrivate GtranslatorPluginManagerPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorPluginManager GtranslatorPluginManager;

struct _GtranslatorPluginManager 
{
	GtkVBox vbox;

	/*< private > */
	GtranslatorPluginManagerPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorPluginManagerClass GtranslatorPluginManagerClass;

struct _GtranslatorPluginManagerClass 
{
	GtkVBoxClass parent_class;
};

/*
 * Public methods
 */
GType		 gtranslator_plugin_manager_get_type		(void) G_GNUC_CONST;

GtkWidget	*gtranslator_plugin_manager_new		(void);
   
G_END_DECLS

#endif  /* __GTR_PLUGIN_MANAGER_H__  */
