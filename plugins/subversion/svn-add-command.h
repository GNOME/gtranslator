/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * gtranslator
 * Copyright (C) James Liggett 2007 <jrliggett@cox.net>
 *
 * Portions based on the original Subversion plugin 
 * Copyright (C) Johannes Schmid 2005 
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

#ifndef _SVN_ADD_COMMAND_H_
#define _SVN_ADD_COMMAND_H_

#include <glib-object.h>
#include "svn-command.h"

G_BEGIN_DECLS

#define SVN_TYPE_ADD_COMMAND             (svn_add_command_get_type ())
#define SVN_ADD_COMMAND(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), SVN_TYPE_ADD_COMMAND, SvnAddCommand))
#define SVN_ADD_COMMAND_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), SVN_TYPE_ADD_COMMAND, SvnAddCommandClass))
#define SVN_IS_ADD_COMMAND(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SVN_TYPE_ADD_COMMAND))
#define SVN_IS_ADD_COMMAND_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), SVN_TYPE_ADD_COMMAND))
#define SVN_ADD_COMMAND_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), SVN_TYPE_ADD_COMMAND, SvnAddCommandClass))

typedef struct _SvnAddCommandClass SvnAddCommandClass;
typedef struct _SvnAddCommand SvnAddCommand;
typedef struct _SvnAddCommandPriv SvnAddCommandPriv;

struct _SvnAddCommandClass
{
	SvnCommandClass parent_class;
};

struct _SvnAddCommand
{
	SvnCommand parent_instance;
	
	SvnAddCommandPriv *priv;
};

GType svn_add_command_get_type (void) G_GNUC_CONST;
SvnAddCommand * svn_add_command_new (const gchar *path, gboolean force, gboolean recursive);
void svn_add_command_destroy (SvnAddCommand *self);

G_END_DECLS

#endif /* _SVN_ADD_COMMAND_H_ */
