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

#ifndef _GTR_COMMAND_H_
#define _GTR_COMMAND_H_

#include <glib-object.h>

G_BEGIN_DECLS

#define GTR_TYPE_COMMAND             (gtranslator_command_get_type ())
#define GTR_COMMAND(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_COMMAND, GtranslatorCommand))
#define GTR_COMMAND_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GTR_TYPE_COMMAND, GtranslatorCommandClass))
#define GTR_IS_COMMAND(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTR_TYPE_COMMAND))
#define GTR_IS_COMMAND_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_TYPE_COMMAND))
#define GTR_COMMAND_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GTR_TYPE_COMMAND, GtranslatorCommandClass))

typedef struct _GtranslatorCommandClass GtranslatorCommandClass;
typedef struct _GtranslatorCommand GtranslatorCommand;
typedef struct _GtranslatorCommandPriv GtranslatorCommandPriv;

struct _GtranslatorCommandClass
{
	GObjectClass parent_class;
	
	/* Virtual Methods */
	guint (*run) (GtranslatorCommand *self);
	void (*start) (GtranslatorCommand *self);
	void (*notify_data_arrived) (GtranslatorCommand *self);
	void (*notify_complete) (GtranslatorCommand *self, guint return_code);
	void (*set_error_message) (GtranslatorCommand *self, gchar *error_message);
	gchar * (*get_error_message) (GtranslatorCommand *self);

};

struct _GtranslatorCommand
{
	GObject parent_instance;
	
	GtranslatorCommandPriv *priv;
};

GType gtranslator_command_get_type (void) G_GNUC_CONST;

void gtranslator_command_start (GtranslatorCommand *self);
void gtranslator_command_notify_data_arrived (GtranslatorCommand *self);
void gtranslator_command_notify_complete (GtranslatorCommand *self, guint return_code);

void gtranslator_command_set_error_message (GtranslatorCommand *self, gchar *error_message);
gchar *gtranslator_command_get_error_message (GtranslatorCommand *self);

G_END_DECLS

#endif /* _GTR_COMMAND_H_ */
