/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * gtranslator
 * Copyright (C) James Liggett 2007 <jrliggett@cox.net>
 * 
 * gtranslator is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * gtranslator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with gtranslator.  If not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#ifndef _SVN_CAT_COMMAND_H_
#define _SVN_CAT_COMMAND_H_

#include <glib-object.h>
#include "svn-command.h"

G_BEGIN_DECLS
#define SVN_TYPE_CAT_COMMAND             (svn_cat_command_get_type ())
#define SVN_CAT_COMMAND(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), SVN_TYPE_CAT_COMMAND, SvnCatCommand))
#define SVN_CAT_COMMAND_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), SVN_TYPE_CAT_COMMAND, SvnCatCommandClass))
#define SVN_IS_CAT_COMMAND(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SVN_TYPE_CAT_COMMAND))
#define SVN_IS_CAT_COMMAND_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), SVN_TYPE_CAT_COMMAND))
#define SVN_CAT_COMMAND_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), SVN_TYPE_CAT_COMMAND, SvnCatCommandClass))
typedef struct _SvnCatCommandClass SvnCatCommandClass;
typedef struct _SvnCatCommand SvnCatCommand;
typedef struct _SvnCatCommandPriv SvnCatCommandPriv;

struct _SvnCatCommandClass
{
  SvnCommandClass parent_class;
};

struct _SvnCatCommand
{
  SvnCommand parent_instance;

  SvnCatCommandPriv *priv;
};

GType
svn_cat_command_get_type (void)
  G_GNUC_CONST;
     SvnCatCommand *svn_cat_command_new (const gchar * path, glong revision);
     void svn_cat_command_destroy (SvnCatCommand * self);
     GQueue *svn_cat_command_get_output (SvnCatCommand * self);

G_END_DECLS
#endif /* _SVN_CAT_COMMAND_H_ */
