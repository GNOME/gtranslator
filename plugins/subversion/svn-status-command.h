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

#ifndef _SVN_STATUS_COMMAND_H_
#define _SVN_STATUS_COMMAND_H_

#include <glib-object.h>
#include "svn-command.h"
#include "svn-status.h"

G_BEGIN_DECLS

#define SVN_TYPE_STATUS_COMMAND             (svn_status_command_get_type ())
#define SVN_STATUS_COMMAND(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), SVN_TYPE_STATUS_COMMAND, SvnStatusCommand))
#define SVN_STATUS_COMMAND_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), SVN_TYPE_STATUS_COMMAND, SvnStatusCommandClass))
#define SVN_IS_STATUS_COMMAND(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SVN_TYPE_STATUS_COMMAND))
#define SVN_IS_STATUS_COMMAND_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), SVN_TYPE_STATUS_COMMAND))
#define SVN_STATUS_COMMAND_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), SVN_TYPE_STATUS_COMMAND, SvnStatusCommandClass))

typedef struct _SvnStatusCommandClass SvnStatusCommandClass;
typedef struct _SvnStatusCommand SvnStatusCommand;
typedef struct _SvnStatusCommandPriv SvnStatusCommandPriv;

struct _SvnStatusCommandClass
{
	SvnCommandClass parent_class;
};

struct _SvnStatusCommand
{
	SvnCommand parent_instance;
	
	SvnStatusCommandPriv *priv;
};

GType svn_status_command_get_type (void) G_GNUC_CONST;
SvnStatusCommand *svn_status_command_new (gchar *path, gboolean recursive, 
										  gboolean get_all_items);
void svn_status_command_destroy (SvnStatusCommand *self);
GQueue *svn_status_command_get_status_queue (SvnStatusCommand *self);

G_END_DECLS

#endif /* _SVN_STATUS_COMMAND_H_ */
