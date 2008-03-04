/*
 * module.c
 * This file is part of gtranslator
 *
 * Copyright (C) 2005 - Paolo Maggi 
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
 
/* This is a modified version of ephy-module.c from Epiphany source code.
 * Here the original copyright assignment:
 *
 *  Copyright (C) 2003 Marco Pesenti Gritti
 *  Copyright (C) 2003, 2004 Christian Persch
 *
 */

/*
 * Modified by the gtranslator Team, 2005. See the AUTHORS file for a 
 * list of people on the gtranslator Team.  
 * See the ChangeLog files for a list of changes. 
 *
 * $Id: module.c 5367 2006-12-17 14:29:49Z pborelli $
 */

#include "config.h"

#include "module.h"
//#include "gtranslator-debug.h"

#include <gmodule.h>

typedef struct _GtranslatorModuleClass GtranslatorModuleClass;

struct _GtranslatorModuleClass
{
	GTypeModuleClass parent_class;
};

struct _GtranslatorModule
{
	GTypeModule parent_instance;

	GModule *library;

	gchar *path;
	GType type;
};

typedef GType (*GtranslatorModuleRegisterFunc) (GTypeModule *);

static void gtranslator_module_init		(GtranslatorModule *action);
static void gtranslator_module_class_init	(GtranslatorModuleClass *class);

static GObjectClass *parent_class = NULL;

GType
gtranslator_module_get_type (void)
{
	static GType type = 0;

	if (G_UNLIKELY (type == 0))
	{
		static const GTypeInfo type_info =
		{
			sizeof (GtranslatorModuleClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) gtranslator_module_class_init,
			(GClassFinalizeFunc) NULL,
			NULL,
			sizeof (GtranslatorModule),
			0, /* n_preallocs */
			(GInstanceInitFunc) gtranslator_module_init,
		};

		type = g_type_register_static (G_TYPE_TYPE_MODULE,
					       "GtranslatorModule",
					       &type_info, 0);
	}

	return type;
}

static gboolean
gtranslator_module_load (GTypeModule *gmodule)
{
	GtranslatorModule *module = GTR_MODULE (gmodule);
	GtranslatorModuleRegisterFunc register_func;

	g_message( "Loading %s", module->path);

	module->library = g_module_open (module->path, 0);

	if (module->library == NULL)
	{
		g_warning (g_module_error());

		return FALSE;
	}

	/* extract symbols from the lib */
	if (!g_module_symbol (module->library, "register_gtranslator_plugin",
			      (void *) &register_func))
	{
		g_warning (g_module_error());
		g_module_close (module->library);

		return FALSE;
	}

	/* symbol can still be NULL even though g_module_symbol
	 * returned TRUE */
	if (register_func == NULL)
	{
		g_warning ("Symbol 'register_gtranslator_plugin' should not be NULL");
		g_module_close (module->library);

		return FALSE;
	}

	module->type = register_func (gmodule);

	if (module->type == 0)
	{
		g_warning ("Invalid gtranslator plugin contained by module %s", module->path);
		return FALSE;
	}

	return TRUE;
}

static void
gtranslator_module_unload (GTypeModule *gmodule)
{
	GtranslatorModule *module = GTR_MODULE (gmodule);

	g_message( "Unloading %s", module->path);

	g_module_close (module->library);

	module->library = NULL;
	module->type = 0;
}

const gchar *
gtranslator_module_get_path (GtranslatorModule *module)
{
	g_return_val_if_fail (GTR_IS_MODULE (module), NULL);

	return module->path;
}

GObject *
gtranslator_module_new_object (GtranslatorModule *module)
{
	g_message( "Creating object of type %s", g_type_name (module->type));

	if (module->type == 0)
	{
		return NULL;
	}

	return g_object_new (module->type, NULL);
}

static void
gtranslator_module_init (GtranslatorModule *module)
{
	g_message( "GtranslatorModule %p initialising", module);
}

static void
gtranslator_module_finalize (GObject *object)
{
	GtranslatorModule *module = GTR_MODULE (object);

	g_message( "GtranslatorModule %p finalising", module);

	g_free (module->path);

	G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtranslator_module_class_init (GtranslatorModuleClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);
	GTypeModuleClass *module_class = G_TYPE_MODULE_CLASS (class);

	parent_class = (GObjectClass *) g_type_class_peek_parent (class);

	object_class->finalize = gtranslator_module_finalize;

	module_class->load = gtranslator_module_load;
	module_class->unload = gtranslator_module_unload;
}

GtranslatorModule *
gtranslator_module_new (const gchar *path)
{
	GtranslatorModule *result;

	if (path == NULL || path[0] == '\0')
	{
		return NULL;
	}

	result = g_object_new (GTR_TYPE_MODULE, NULL);

	g_type_module_set_name (G_TYPE_MODULE (result), path);
	result->path = g_strdup (path);

	return result;
}
