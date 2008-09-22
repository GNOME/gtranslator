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

#ifndef _SVN_COMMAND_H_
#define _SVN_COMMAND_H_

#include <glib-object.h>
#include <svn_client.h>
#include <svn_pools.h>
#include <svn_config.h>
#include <svn_path.h>
#include "subversion-plugin.h"
#include "async-command.h"

G_BEGIN_DECLS

#define SVN_TYPE_COMMAND             (svn_command_get_type ())
#define SVN_COMMAND(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), SVN_TYPE_COMMAND, SvnCommand))
#define SVN_COMMAND_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), SVN_TYPE_COMMAND, SvnCommandClass))
#define SVN_IS_COMMAND(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SVN_TYPE_COMMAND))
#define SVN_IS_COMMAND_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), SVN_TYPE_COMMAND))
#define SVN_COMMAND_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), SVN_TYPE_COMMAND, SvnCommandClass))

typedef struct _SvnCommandClass SvnCommandClass;
typedef struct _SvnCommand SvnCommand;
typedef struct _SvnCommandPriv SvnCommandPriv;

struct _SvnCommandClass
{
	GtranslatorAsyncCommandClass parent_class;
};

struct _SvnCommand
{
	GtranslatorAsyncCommand parent_instance;
	
	SvnCommandPriv *priv;
};

GType svn_command_get_type (void) G_GNUC_CONST;

void svn_command_push_info (SvnCommand *self, gchar *message);

GQueue * svn_command_get_info_queue (SvnCommand *self);

void svn_command_set_error (SvnCommand *self, svn_error_t *error);

svn_client_ctx_t *svn_command_get_client_context (SvnCommand *self);

apr_pool_t *svn_command_get_pool (SvnCommand *self);

void svn_command_lock_ui (SvnCommand *self);

void svn_command_unlock_ui (SvnCommand *self);

gchar *svn_command_make_canonical_path (SvnCommand *self, gchar *path);

/* Static methods */
svn_opt_revision_t *svn_command_get_revision (gchar *revision);

GList *svn_command_copy_path_list (GList *list);

void svn_command_free_path_list (GList *list);

G_END_DECLS

#endif /* _SVN_COMMAND_H_ */
