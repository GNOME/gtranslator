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

#include "svn-add-command.h"

struct _SvnAddCommandPriv
{
	gchar *path;
	gboolean force;
	gboolean recursive;
};

G_DEFINE_TYPE (SvnAddCommand, svn_add_command, SVN_TYPE_COMMAND);

static void
svn_add_command_init (SvnAddCommand *self)
{
	self->priv = g_new0 (SvnAddCommandPriv, 1);
}

static void
svn_add_command_finalize (GObject *object)
{
	SvnAddCommand *self;
	
	self = SVN_ADD_COMMAND (object);
	
	g_free (self->priv->path);
	g_free (self->priv);

	G_OBJECT_CLASS (svn_add_command_parent_class)->finalize (object);
}

static guint
svn_add_command_run (GtranslatorCommand *command)
{
	SvnAddCommand *self;
	SvnCommand *svn_command;
	svn_error_t *error;
	svn_depth_t depth;
	
	self = SVN_ADD_COMMAND (command);
	svn_command = SVN_COMMAND (command);

	if (self->priv->recursive == TRUE)
		depth = svn_depth_infinity;
	else depth = svn_depth_empty;
	
	error = svn_client_add4 (self->priv->path,
							 depth,
							 self->priv->force,
							 FALSE,
							 FALSE, 
							 svn_command_get_client_context (svn_command), 
							 svn_command_get_pool (svn_command));

	if (error)
	{
		svn_command_set_error (svn_command, error);
		return 1;
	}
	
	return 0;
}

static void
svn_add_command_class_init (SvnAddCommandClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtranslatorCommandClass *command_class = GTR_COMMAND_CLASS (klass);

	object_class->finalize = svn_add_command_finalize;
	command_class->run = svn_add_command_run;
}


SvnAddCommand *
svn_add_command_new (const gchar *path, gboolean force, gboolean recursive)
{
	SvnAddCommand *self;
	
	self = g_object_new (SVN_TYPE_ADD_COMMAND, NULL);
	self->priv->path = svn_command_make_canonical_path (SVN_COMMAND (self),
														path);
	self->priv->force = force;
	self->priv->recursive = recursive;
	
	return self;
}

void
svn_add_command_destroy (SvnAddCommand *self)
{
	g_object_unref (self);
}
