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

#include "svn-resolve-command.h"

struct _SvnResolveCommandPriv
{
	GList *paths;
	gboolean recursive;
};

G_DEFINE_TYPE (SvnResolveCommand, svn_resolve_command, SVN_TYPE_COMMAND);

static void
svn_resolve_command_init (SvnResolveCommand *self)
{	
	self->priv = g_new0 (SvnResolveCommandPriv, 1);
}

static void
svn_resolve_command_finalize (GObject *object)
{
	SvnResolveCommand *self;
	
	self = SVN_RESOLVE_COMMAND (object);
	
	svn_command_free_path_list (self->priv->paths);
	g_free (self->priv);

	G_OBJECT_CLASS (svn_resolve_command_parent_class)->finalize (object);
}

static guint
svn_resolve_command_run (GtranslatorCommand *command)
{
	SvnResolveCommand *self;
	SvnCommand *svn_command;
	GList *current_path;
	svn_error_t *error;
	
	self = SVN_RESOLVE_COMMAND (command);
	svn_command = SVN_COMMAND (command);
	current_path = self->priv->paths;
	
	while (current_path)
	{
		error = svn_client_resolved (current_path->data, 
									 self->priv->recursive,
									 svn_command_get_client_context (svn_command), 
									 svn_command_get_pool (svn_command));

		if (error)
		{
			svn_command_set_error (svn_command, error);
			return 1;
		}
		
		current_path = g_list_next (current_path);
	}
	
	return 0;
}

static void
svn_resolve_command_class_init (SvnResolveCommandClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtranslatorCommandClass *command_class = GTR_COMMAND_CLASS (klass);

	object_class->finalize = svn_resolve_command_finalize;
	command_class->run = svn_resolve_command_run;
}


SvnResolveCommand *
svn_resolve_command_new (GList *paths, gboolean recursive)
{
	SvnResolveCommand *self;
	
	self = g_object_new (SVN_TYPE_RESOLVE_COMMAND, NULL);
	self->priv->paths = svn_command_copy_path_list (paths);
	self->priv->recursive = recursive;
	
	return self;
}

void
svn_resolve_command_destroy (SvnResolveCommand *self)
{
	g_object_unref (self);
}
