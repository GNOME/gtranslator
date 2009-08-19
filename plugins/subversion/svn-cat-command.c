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

#include "svn-cat-command.h"

struct _SvnCatCommandPriv
{
  gchar *path;
  glong revision;
  GQueue *output;
};

G_DEFINE_TYPE (SvnCatCommand, svn_cat_command, SVN_TYPE_COMMAND);

static void
svn_cat_command_init (SvnCatCommand * self)
{
  self->priv = g_new0 (SvnCatCommandPriv, 1);
}

static void
svn_cat_command_finalize (GObject * object)
{
  SvnCatCommand *self;
  GList *current_output;

  self = SVN_CAT_COMMAND (object);

  g_free (self->priv->path);

  current_output = self->priv->output->head;

  while (current_output)
    {
      g_free (current_output->data);
      current_output = g_list_next (current_output);
    }

  g_queue_free (self->priv->output);

  g_free (self->priv);

  G_OBJECT_CLASS (svn_cat_command_parent_class)->finalize (object);
}

static guint
svn_cat_command_run (GtranslatorCommand * command)
{
  SvnCatCommand *self;
  SvnCommand *svn_command;
  svn_opt_revision_t revision;
  svn_opt_revision_t peg_revision;
  svn_stream_t *cat_stream;
  apr_file_t *cat_input;
  apr_file_t *cat_output;
  apr_size_t read_size;
  gchar *line;
  svn_error_t *error;
  apr_status_t apr_error;

  self = SVN_CAT_COMMAND (command);
  svn_command = SVN_COMMAND (command);

  apr_file_pipe_create (&cat_output, &cat_input,
			svn_command_get_pool (svn_command));
  apr_file_pipe_timeout_set (cat_output, 0);
  apr_file_pipe_timeout_set (cat_input, 0);
  cat_stream = svn_stream_from_aprfile2 (cat_input, FALSE,
					 svn_command_get_pool (svn_command));

  revision.kind = svn_opt_revision_base;
  revision.value.number = self->priv->revision;
  peg_revision.kind = svn_opt_revision_unspecified;

  error = svn_client_cat2 (cat_stream,
			   self->priv->path,
			   &peg_revision,
			   &revision,
			   svn_command_get_client_context (svn_command),
			   svn_command_get_pool (svn_command));

  if (error)
    {
      svn_command_set_error (svn_command, error);
      return 1;
    }

  while (apr_file_eof (cat_output) != APR_EOF)
    {
      read_size = 80;
      line = g_new0 (gchar, (read_size + 1));

      apr_error = apr_file_read (cat_output, line, &read_size);

      if (apr_error)
	break;

      if (strlen (line))
	{
	  gtranslator_async_command_lock (GTR_ASYNC_COMMAND (command));
	  g_queue_push_tail (self->priv->output, g_strdup (line));
	  gtranslator_async_command_unlock (GTR_ASYNC_COMMAND (command));

	  g_free (line);

	  gtranslator_command_notify_data_arrived (command);
	}
    }

  return 0;
}

static void
svn_cat_command_class_init (SvnCatCommandClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtranslatorCommandClass *command_class = GTR_COMMAND_CLASS (klass);

  object_class->finalize = svn_cat_command_finalize;
  command_class->run = svn_cat_command_run;
}


SvnCatCommand *
svn_cat_command_new (const gchar * path, glong revision)
{
  SvnCatCommand *self;

  self = g_object_new (SVN_TYPE_CAT_COMMAND, NULL);
  self->priv->path = svn_command_make_canonical_path (SVN_COMMAND (self),
						      path);
  self->priv->revision = revision;
  self->priv->output = g_queue_new ();

  return self;
}

void
svn_cat_command_destroy (SvnCatCommand * self)
{
  g_object_unref (self);
}

GQueue *
svn_cat_command_get_output (SvnCatCommand * self)
{
  return self->priv->output;
}
