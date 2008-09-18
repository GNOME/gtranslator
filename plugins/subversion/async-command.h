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

#ifndef _GTR_ASYNC_COMMAND_H_
#define _GTR_ASYNC_COMMAND_H_

#include <glib-object.h>
#include "command.h"

G_BEGIN_DECLS

#define GTR_TYPE_ASYNC_COMMAND             (gtranslator_async_command_get_type ())
#define GTR_ASYNC_COMMAND(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_ASYNC_COMMAND, GtranslatorAsyncCommand))
#define GTR_ASYNC_COMMAND_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GTR_TYPE_ASYNC_COMMAND, GtranslatorAsyncCommandClass))
#define IS_GTR_ASYNC_COMMAND(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTR_TYPE_ASYNC_COMMAND))
#define IS_GTR_ASYNC_COMMAND_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_TYPE_ASYNC_COMMAND))
#define GTR_ASYNC_COMMAND_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GTR_TYPE_ASYNC_COMMAND, GtranslatorAsyncCommandClass))

typedef struct _GtranslatorAsyncCommandClass GtranslatorAsyncCommandClass;
typedef struct _GtranslatorAsyncCommand GtranslatorAsyncCommand;
typedef struct _GtranslatorAsyncCommandPriv GtranslatorAsyncCommandPriv;

struct _GtranslatorAsyncCommandClass
{
	GtranslatorCommandClass parent_class;
};

struct _GtranslatorAsyncCommand
{
	GtranslatorCommand parent_instance;
	
	GtranslatorAsyncCommandPriv *priv;
};

GType gtranslator_async_command_get_type (void) G_GNUC_CONST;

void gtranslator_async_command_set_error_message (GtranslatorCommand *command, 
											 gchar *error_message);
gchar *gtranslator_async_command_get_error_message (GtranslatorCommand *command);

void gtranslator_async_command_lock (GtranslatorAsyncCommand *self);
void gtranslator_async_command_unlock (GtranslatorAsyncCommand *self);

G_END_DECLS

#endif /* _GTR_ASYNC_COMMAND_H_ */
