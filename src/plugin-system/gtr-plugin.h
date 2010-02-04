/*
 * plugin.h
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
 * $Id: plugin.h 6448 2008-08-25 10:28:33Z icq $
 */

#ifndef __GTR_PLUGIN_H__
#define __GTR_PLUGIN_H__

#include <glib-object.h>

#include "gtr-window.h"
#include "gtr-debug.h"

/* TODO: add a .h file that includes all the .h files normally needed to
 * develop a plugin */

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_PLUGIN              (gtranslator_plugin_get_type())
#define GTR_PLUGIN(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), GTR_TYPE_PLUGIN, GtrPlugin))
#define GTR_PLUGIN_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GTR_TYPE_PLUGIN, GtrPluginClass))
#define GTR_IS_PLUGIN(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTR_TYPE_PLUGIN))
#define GTR_IS_PLUGIN_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_TYPE_PLUGIN))
#define GTR_PLUGIN_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GTR_TYPE_PLUGIN, GtrPluginClass))
/*
 * Main object structure
 */
typedef struct _GtrPlugin GtrPlugin;

struct _GtrPlugin
{
  GObject parent;
};

/*
 * Class definition
 */
typedef struct _GtrPluginClass GtrPluginClass;

struct _GtrPluginClass
{
  GObjectClass parent_class;

  /* Virtual public methods */

  void (*activate) (GtrPlugin * plugin, GtrWindow * window);
  void (*deactivate) (GtrPlugin * plugin, GtrWindow * window);

  void (*update_ui) (GtrPlugin * plugin, GtrWindow * window);

  GtkWidget *(*create_configure_dialog) (GtrPlugin * plugin);

  /* Plugins should not override this, it's handled automatically by
     the GtrPluginClass */
    gboolean (*is_configurable) (GtrPlugin * plugin);

  /* Padding for future expansion */
  void (*_gtranslator_reserved1) (void);
  void (*_gtranslator_reserved2) (void);
  void (*_gtranslator_reserved3) (void);
  void (*_gtranslator_reserved4) (void);
};

/*
 * Public methods
 */
GType
gtranslator_plugin_get_type (void)
  G_GNUC_CONST;

     void gtranslator_plugin_activate (GtrPlugin * plugin,
				       GtrWindow * window);
     void gtranslator_plugin_deactivate (GtrPlugin * plugin,
					 GtrWindow * window);

     void gtranslator_plugin_update_ui (GtrPlugin * plugin,
					GtrWindow * window);

     gboolean gtranslator_plugin_is_configurable (GtrPlugin * plugin);
     GtkWidget *gtranslator_plugin_create_configure_dialog
       (GtrPlugin * plugin);

/**
 * GTR_PLUGIN_REGISTER_TYPE_WITH_CODE(PluginName, plugin_name, CODE):
 *
 * Utility macro used to register plugins with additional code.
 */
#define GTR_PLUGIN_REGISTER_TYPE_WITH_CODE(PluginName, plugin_name, CODE)	\
										\
static GType plugin_name##_type = 0;						\
										\
GType										\
plugin_name##_get_type (void)							\
{										\
	return plugin_name##_type;						\
}										\
										\
static void     plugin_name##_init              (PluginName        *self);	\
static void     plugin_name##_class_init        (PluginName##Class *klass);	\
static gpointer plugin_name##_parent_class = NULL;				\
static void     plugin_name##_class_intern_init (gpointer klass)		\
{										\
	plugin_name##_parent_class = g_type_class_peek_parent (klass);		\
	plugin_name##_class_init ((PluginName##Class *) klass);			\
}										\
										\
G_MODULE_EXPORT GType								\
register_gtranslator_plugin (GTypeModule *module)					\
{										\
	static const GTypeInfo our_info =					\
	{									\
		sizeof (PluginName##Class),					\
		NULL, /* base_init */						\
		NULL, /* base_finalize */					\
		(GClassInitFunc) plugin_name##_class_intern_init,		\
		NULL,								\
		NULL, /* class_data */						\
		sizeof (PluginName),						\
		0, /* n_preallocs */						\
		(GInstanceInitFunc) plugin_name##_init				\
	};									\
										\
	DEBUG_PRINT ( "Registering " #PluginName);	\
										\
	/* Initialise the i18n stuff */						\
	bindtextdomain (GETTEXT_PACKAGE, GTR_LOCALEDIR);			\
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");			\
										\
	plugin_name##_type = g_type_module_register_type (module,		\
					    GTR_TYPE_PLUGIN,			\
					    #PluginName,			\
					    &our_info,				\
					    0);					\
										\
	CODE									\
										\
	return plugin_name##_type;						\
}

/**
 * GTR_PLUGIN_REGISTER_TYPE(PluginName, plugin_name):
 * 
 * Utility macro used to register plugins.
 */
#define GTR_PLUGIN_REGISTER_TYPE(PluginName, plugin_name)			\
	GTR_PLUGIN_REGISTER_TYPE_WITH_CODE(PluginName, plugin_name, ;)

/**
 * GTR_PLUGIN_DEFINE_TYPE_WITH_CODE(ObjectName, object_name, PARENT_TYPE, CODE):
 *
 * Utility macro used to register gobject types in plugins with additional code.
 */
#define GTR_PLUGIN_DEFINE_TYPE_WITH_CODE(ObjectName, object_name, PARENT_TYPE, CODE)	\
										\
static GType g_define_type_id = 0;						\
										\
GType										\
object_name##_get_type (void)							\
{										\
	return g_define_type_id;						\
}										\
										\
static void     object_name##_init              (ObjectName        *self);	\
static void     object_name##_class_init        (ObjectName##Class *klass);	\
static gpointer object_name##_parent_class = NULL;				\
static void     object_name##_class_intern_init (gpointer klass)		\
{										\
	object_name##_parent_class = g_type_class_peek_parent (klass);		\
	object_name##_class_init ((ObjectName##Class *) klass);			\
}										\
										\
GType										\
object_name##_register_type (GTypeModule *module)					\
{										\
	static const GTypeInfo our_info =					\
	{									\
		sizeof (ObjectName##Class),					\
		NULL, /* base_init */						\
		NULL, /* base_finalize */					\
		(GClassInitFunc) object_name##_class_intern_init,		\
		NULL,								\
		NULL, /* class_data */						\
		sizeof (ObjectName),						\
		0, /* n_preallocs */						\
		(GInstanceInitFunc) object_name##_init				\
	};									\
										\
	DEBUG_PRINT ( "Registering " #ObjectName);	\
										\
	g_define_type_id = g_type_module_register_type (module,			\
					   	        PARENT_TYPE,		\
					                #ObjectName,		\
					                &our_info,		\
					                0);			\
										\
	CODE									\
										\
	return g_define_type_id;						\
}

/**
 * GTR_PLUGIN_DEFINE_TYPE(ObjectName, object_name, PARENT_TYPE):
 *
 * Utility macro used to register gobject types in plugins.
 */
#define GTR_PLUGIN_DEFINE_TYPE(ObjectName, object_name, PARENT_TYPE)		\
	GTR_PLUGIN_DEFINE_TYPE_WITH_CODE(ObjectName, object_name, PARENT_TYPE, ;)

G_END_DECLS
#endif /* __GTR_PLUGIN_H__ */
