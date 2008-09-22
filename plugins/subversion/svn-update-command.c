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

#include "svn-update-command.h"

struct _SvnUpdateCommandPriv
{
	gchar *path;
	gchar *revision;
	gboolean recursive;
};

G_DEFINE_TYPE (SvnUpdateCommand, svn_update_command, SVN_TYPE_COMMAND);

static void
svn_update_command_init (SvnUpdateCommand *self)
{	
	self->priv = g_new0 (SvnUpdateCommandPriv, 1);
}

static void
svn_update_command_finalize (GObject *object)
{
	SvnUpdateCommand *self;
	
	self = SVN_UPDATE_COMMAND (object);
	
	g_free (self->priv->path);
	g_free (self->priv->revision);
	g_free (self->priv);

	G_OBJECT_CLASS (svn_update_command_parent_class)->finalize (object);
}

static guint
svn_update_command_run (GtranslatorCommand *command)
{
	SvnUpdateCommand *self;
	SvnCommand *svn_command;
	svn_opt_revision_t *revision;
	apr_array_header_t *update_paths;
	apr_array_header_t *update_revisions;
	svn_error_t *error;
	svn_revnum_t revision_number;
	gchar *revision_message;
	svn_depth_t depth;
	
	self = SVN_UPDATE_COMMAND (command);
	svn_command = SVN_COMMAND (command);
	revision = svn_command_get_revision (self->priv->revision);
	
	/* I just copied this so don't blame me... */
	update_paths = apr_array_make (svn_command_get_pool (svn_command), 1,
								   sizeof (char *));
	(*((const char **) apr_array_push (update_paths))) = self->priv->path;
	
	if (self->priv->recursive == TRUE)
		depth = svn_depth_infinity;
	else depth = svn_depth_files;
	
	error = svn_client_update3 (&update_revisions, 
								update_paths, 
								revision, 
								depth,
								FALSE,
								FALSE,
								FALSE,
								svn_command_get_client_context (svn_command),
								svn_command_get_pool (svn_command));
	
	if (error)
	{
		svn_command_set_error (svn_command, error);
		return 1;
	}
	
	revision_number = (*(svn_revnum_t *) apr_array_pop (update_revisions));
	
	revision_message = g_strdup_printf (_("Updated to revision %ld."), 
										(glong) revision_number);
	svn_command_push_info (SVN_COMMAND (command), revision_message);
	g_free (revision_message);
	
	return 0;
}

static void
svn_update_command_class_init (SvnUpdateCommandClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtranslatorCommandClass *command_class = GTR_COMMAND_CLASS (klass);

	object_class->finalize = svn_update_command_finalize;
	command_class->run = svn_update_command_run;
}


SvnUpdateCommand *
svn_update_command_new (const gchar *path, const gchar *revision, gboolean recursive)
{
	SvnUpdateCommand *self;
	
	self = g_object_new (SVN_TYPE_UPDATE_COMMAND, NULL);
	
	self->priv->path = svn_command_make_canonical_path (SVN_COMMAND (self),
														path);
	self->priv->revision = g_strdup (revision);
	self->priv->recursive = recursive;
	
	return self;
}

void
svn_update_command_destroy (SvnUpdateCommand *self)
{
	g_object_unref (self);
}
