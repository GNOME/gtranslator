/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _SVN_CHECKOUT_COMMAND_H_
#define _SVN_CHECKOUT_COMMAND_H_

#include <config.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include "svn-command.h"

G_BEGIN_DECLS

#define SVN_TYPE_CHECKOUT_COMMAND             (svn_checkout_command_get_type ())
#define SVN_CHECKOUT_COMMAND(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), SVN_TYPE_CHECKOUT_COMMAND, SvnCheckoutCommand))
#define SVN_CHECKOUT_COMMAND_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), SVN_TYPE_CHECKOUT_COMMAND, SvnCheckoutCommandClass))
#define SVN_IS_CHECKOUT_COMMAND(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SVN_TYPE_CHECKOUT_COMMAND))
#define SVN_IS_CHECKOUT_COMMAND_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), SVN_TYPE_CHECKOUT_COMMAND))
#define SVN_CHECKOUT_COMMAND_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), SVN_TYPE_CHECKOUT_COMMAND, SvnCheckoutCommandClass))

typedef struct _SvnCheckoutCommandClass SvnCheckoutCommandClass;
typedef struct _SvnCheckoutCommand SvnCheckoutCommand;
typedef struct _SvnCheckoutCommandPriv SvnCheckoutCommandPriv;

struct _SvnCheckoutCommandClass
{
	SvnCommandClass parent_class;
};

struct _SvnCheckoutCommand
{
	SvnCommand parent_instance;
	
	SvnCheckoutCommandPriv *priv;
};

GType svn_checkout_command_get_type (void) G_GNUC_CONST;
SvnCheckoutCommand * svn_checkout_command_new (const gchar *url, const gchar *path);
void svn_checkout_command_destroy (SvnCheckoutCommand *self);

G_END_DECLS

#endif /* _SVN_CHECKOUT_COMMAND_H_ */
