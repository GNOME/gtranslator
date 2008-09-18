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

#include "async-command.h"

/**
 * SECTION: gtranslator-async-command
 * @short_description: #GtranslatorCommand subclass that serves as the base for 
 *					   commands that need to run in another thread.
 * @include: libgtranslator/gtranslator-async-command.h
 *
 * #GtranslatorAsyncCommand provides a simple way for plugins to run tasks that 
 * are synchronous and usually take several seconds or longer to execute in 
 * another thread so that such tasks do no block Gtranslator's user interface.
 *
 * #GtranslatorAsyncCommand automatically runs and manages the thread when the 
 * command starts, and destroys it when the command finishes. Aside from 
 * locking protected data with gtranslator_async_command_lock/unlock, clients, and
 * even commands themselves need not even be concerned that their tasks are 
 * rnning on another thread.
 *
 * For an example of how #GtranslatorAsyncCommand is used, see the Subversion plugin.
 */

struct _GtranslatorAsyncCommandPriv
{
	GMutex *mutex;
	guint return_code;
	gboolean complete;
	gboolean new_data_arrived;
};

G_DEFINE_TYPE (GtranslatorAsyncCommand, gtranslator_async_command, GTR_TYPE_COMMAND);

static void
gtranslator_async_command_init (GtranslatorAsyncCommand *self)
{
	self->priv = g_new0 (GtranslatorAsyncCommandPriv, 1);
	
	self->priv->mutex = g_mutex_new ();
}

static void
gtranslator_async_command_finalize (GObject *object)
{
	GtranslatorAsyncCommand *self;
	
	self = GTR_ASYNC_COMMAND (object);
	
	g_mutex_free (self->priv->mutex);
	g_idle_remove_by_data (self);
	
	g_free (self->priv);

	G_OBJECT_CLASS (gtranslator_async_command_parent_class)->finalize (object);
}

static gboolean
gtranslator_async_command_notification_poll (GtranslatorCommand *command)
{
	GtranslatorAsyncCommand *self;
	
	self = GTR_ASYNC_COMMAND (command);
	
	if (self->priv->new_data_arrived &&
		g_mutex_trylock (self->priv->mutex))
	{
		g_signal_emit_by_name (command, "data-arrived");
		g_mutex_unlock (self->priv->mutex);
		self->priv->new_data_arrived = FALSE;
	}
	
	if (self->priv->complete)
	{
		g_signal_emit_by_name (command, "command-finished", 
							   self->priv->return_code);
		return FALSE;
	}
	else
		return TRUE;
	
}

static gpointer
gtranslator_async_command_thread (GtranslatorCommand *command)
{
	guint return_code;
	
	return_code = GTR_COMMAND_GET_CLASS (command)->run (command);
	gtranslator_command_notify_complete (command, return_code);
	return NULL;
}

static void
start_command (GtranslatorCommand *command)
{
	g_idle_add ((GSourceFunc) gtranslator_async_command_notification_poll, 
				command);
	g_thread_create ((GThreadFunc) gtranslator_async_command_thread, 
					 command, FALSE, NULL);
}

static void
notify_data_arrived (GtranslatorCommand *command)
{
	GtranslatorAsyncCommand *self;
	
	self = GTR_ASYNC_COMMAND (command);
	
	self->priv->new_data_arrived = TRUE;
}

static void
notify_complete (GtranslatorCommand *command, guint return_code)
{
	GtranslatorAsyncCommand *self;
	
	self = GTR_ASYNC_COMMAND (command);
	
	self->priv->complete = TRUE;
	self->priv->return_code = return_code;
}

static void
gtranslator_async_command_class_init (GtranslatorAsyncCommandClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);
	GtranslatorCommandClass* parent_class = GTR_COMMAND_CLASS (klass);

	object_class->finalize = gtranslator_async_command_finalize;
	
	parent_class->start = start_command;
	parent_class->notify_data_arrived = notify_data_arrived;
	parent_class->notify_complete = notify_complete;
}

void
gtranslator_async_command_set_error_message (GtranslatorCommand *command, 
										gchar *error_message)
{
	gtranslator_async_command_lock (GTR_ASYNC_COMMAND (command));
	GTR_COMMAND_GET_CLASS (command)->set_error_message (command, 
														   error_message);
	gtranslator_async_command_unlock (GTR_ASYNC_COMMAND (command));
}

gchar *
gtranslator_async_command_get_error_message (GtranslatorCommand *command)
{
	gchar *error_message;
	
	gtranslator_async_command_lock (GTR_ASYNC_COMMAND (command));
	error_message = GTR_COMMAND_GET_CLASS (command)->get_error_message (command);
	gtranslator_async_command_unlock (GTR_ASYNC_COMMAND (command));
	
	return error_message;
}

/**
 * gtranslator_async_command_lock:
 * @self: GtranslatorAsyncCommand object.
 *
 * Locks the command's built-in mutex.
 */
void
gtranslator_async_command_lock (GtranslatorAsyncCommand *self)
{
	g_mutex_lock (self->priv->mutex);
}

/**
 * gtranslator_async_command_unlock:
 * @self: GtranslatorAsyncCommand object.
 *
 * Unlocks the command's built-in mutex.
 */
void
gtranslator_async_command_unlock (GtranslatorAsyncCommand *self)
{
	g_mutex_unlock (self->priv->mutex);
}
