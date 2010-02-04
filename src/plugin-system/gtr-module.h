/*
 * module.h
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

/* This is a modified version of module.h from Epiphany source code.
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
 * $Id: module.h 6263 2008-05-05 10:52:10Z sfre $
 */

#ifndef __GTR_MODULE_H__
#define __GTR_MODULE_H__

#include <glib-object.h>
#include <gmodule.h>

G_BEGIN_DECLS
#define GTR_TYPE_MODULE		(gtranslator_module_get_type ())
#define GTR_MODULE(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_MODULE, GtrModule))
#define GTR_MODULE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GTR_TYPE_MODULE, GtrModuleClass))
#define GTR_IS_MODULE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTR_TYPE_MODULE))
#define GTR_IS_MODULE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_TYPE_MODULE))
#define GTR_MODULE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), GTR_TYPE_MODULE, GtrModuleClass))
typedef struct _GtrModule GtrModule;

struct _GtrModule
{
  GTypeModule parent;

  GModule *library;

  gchar *path;
  gchar *module_name;
  GType type;
};

typedef struct _GtrModuleClass GtrModuleClass;

struct _GtrModuleClass
{
  GTypeModuleClass parent_class;

  /* Virtual class methods */
  void (*garbage_collect) ();
};

GType
gtranslator_module_get_type (void)
  G_GNUC_CONST;

     const gchar *gtranslator_module_get_path (GtrModule * module);

     const gchar *gtranslator_module_get_module_name (GtrModule *
						      module);

     GObject *gtranslator_module_new_object (GtrModule * module);

     void gtranslator_module_class_garbage_collect
       (GtrModuleClass * klass);

G_END_DECLS
#endif
