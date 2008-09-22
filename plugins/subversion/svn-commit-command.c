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

#include "svn-commit-command.h"

struct _SvnCommitCommandPriv
{
	GList *paths;
	gchar *log_message;
	gboolean recursive;
};

G_DEFINE_TYPE (SvnCommitCommand, svn_commit_command, SVN_TYPE_COMMAND);

static svn_error_t*
on_log_callback (const char **log_msg,
				 const char **tmp_file,
				 apr_array_header_t *commit_items,
				 void *baton,
				 apr_pool_t *pool)
{
	
	SvnCommitCommand *self;
	
	self = SVN_COMMIT_COMMAND (baton);
	
	*log_msg = self->priv->log_message;
	*tmp_file = NULL;
	
	return SVN_NO_ERROR;
}

static void
svn_commit_command_init (SvnCommitCommand *self)
{
	svn_client_ctx_t *client_context;
	
	self->priv = g_new0 (SvnCommitCommandPriv, 1);
	client_context = svn_command_get_client_context (SVN_COMMAND (self));
	
	client_context->log_msg_func = on_log_callback;
	client_context->log_msg_baton = self;
	
}

static void
svn_commit_command_finalize (GObject *object)
{
	SvnCommitCommand *self;
	
	self = SVN_COMMIT_COMMAND (object);
	
	svn_command_free_path_list (self->priv->paths);
	g_free (self->priv->log_message);
	g_free (self->priv);

	G_OBJECT_CLASS (svn_commit_command_parent_class)->finalize (object);
}

static guint
svn_commit_command_run (GtranslatorCommand *command)
{
	SvnCommitCommand *self;
	SvnCommand *svn_command;
	GList *current_path;
	apr_array_header_t *commit_paths;
	gchar *revision_message;
	svn_error_t *error;
	svn_commit_info_t *commit_info;
	svn_depth_t depth;
	
	self = SVN_COMMIT_COMMAND (command);
	svn_command = SVN_COMMAND (command);
	current_path = self->priv->paths;
	commit_paths = apr_array_make (svn_command_get_pool (svn_command),
								   g_list_length (self->priv->paths), 
								   sizeof (char *));
	
	while (current_path)
	{
		/* I just copied this so don't blame me... */
		(*((const char **) apr_array_push (commit_paths))) = current_path->data;
		current_path = g_list_next (current_path);
	}
	
	if (self->priv->recursive == TRUE)
		depth = svn_depth_infinity;
	else depth = svn_depth_files;
	
	error = svn_client_commit4 (&commit_info, 
								commit_paths, 
								depth,
								TRUE,
								FALSE,
								NULL,
								NULL,
								svn_command_get_client_context (svn_command), 
								svn_command_get_pool (svn_command));

	if (error)
	{
		svn_command_set_error (svn_command, error);
		return 1;
	}
	
	if (commit_info)
	{
		revision_message = g_strdup_printf ("Committed revision %ld.", 
											commit_info->revision);
		svn_command_push_info (SVN_COMMAND (command), revision_message);
		g_free (revision_message);
	}
	
	return 0;
}

static void
svn_commit_command_class_init (SvnCommitCommandClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtranslatorCommandClass *command_class = GTR_COMMAND_CLASS (klass);

	object_class->finalize = svn_commit_command_finalize;
	command_class->run = svn_commit_command_run;
}

SvnCommitCommand *
svn_commit_command_new (GList *paths, gchar *log_message, 
						gboolean recursive)
{
	SvnCommitCommand *self;
	
	self = g_object_new (SVN_TYPE_COMMIT_COMMAND, NULL);
	self->priv->paths = svn_command_copy_path_list (paths);
	self->priv->log_message = g_strdup (log_message);
	self->priv->recursive = recursive;
	
	return self;
}

void
svn_commit_command_destroy (SvnCommitCommand *self)
{
	g_object_unref (self);
}
