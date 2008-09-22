/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * anjuta
 * Copyright (C) James Liggett 2007 <jrliggett@cox.net>
 * 
 * anjuta is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * anjuta is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with anjuta.  If not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#ifndef _SVN_RESOLVE_COMMAND_H_
#define _SVN_RESOLVE_COMMAND_H_

#include <glib-object.h>
#include "svn-command.h"

G_BEGIN_DECLS

#define SVN_TYPE_RESOLVE_COMMAND             (svn_resolve_command_get_type ())
#define SVN_RESOLVE_COMMAND(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), SVN_TYPE_RESOLVE_COMMAND, SvnResolveCommand))
#define SVN_RESOLVE_COMMAND_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), SVN_TYPE_RESOLVE_COMMAND, SvnResolveCommandClass))
#define SVN_IS_RESOLVE_COMMAND(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SVN_TYPE_RESOLVE_COMMAND))
#define SVN_IS_RESOLVE_COMMAND_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), SVN_TYPE_RESOLVE_COMMAND))
#define SVN_RESOLVE_COMMAND_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), SVN_TYPE_RESOLVE_COMMAND, SvnResolveCommandClass))

typedef struct _SvnResolveCommandClass SvnResolveCommandClass;
typedef struct _SvnResolveCommand SvnResolveCommand;
typedef struct _SvnResolveCommandPriv SvnResolveCommandPriv;

struct _SvnResolveCommandClass
{
	SvnCommandClass parent_class;
};

struct _SvnResolveCommand
{
	SvnCommand parent_instance;
	
	SvnResolveCommandPriv *priv;
};

GType svn_resolve_command_get_type (void) G_GNUC_CONST;
SvnResolveCommand *svn_resolve_command_new (GList *paths, gboolean recursive);
void svn_resolve_command_destroy (SvnResolveCommand *self);

G_END_DECLS

#endif /* _SVN_RESOLVE_COMMAND_H_ */
