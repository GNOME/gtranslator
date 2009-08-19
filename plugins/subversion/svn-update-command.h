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

#ifndef _SVN_UPDATE_COMMAND_H_
#define _SVN_UPDATE_COMMAND_H_

#include <config.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include "svn-command.h"

G_BEGIN_DECLS
#define SVN_TYPE_UPDATE_COMMAND             (svn_update_command_get_type ())
#define SVN_UPDATE_COMMAND(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), SVN_TYPE_UPDATE_COMMAND, SvnUpdateCommand))
#define SVN_UPDATE_COMMAND_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), SVN_TYPE_UPDATE_COMMAND, SvnUpdateCommandClass))
#define SVN_IS_UPDATE_COMMAND(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SVN_TYPE_UPDATE_COMMAND))
#define SVN_IS_UPDATE_COMMAND_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), SVN_TYPE_UPDATE_COMMAND))
#define SVN_UPDATE_COMMAND_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), SVN_TYPE_UPDATE_COMMAND, SvnUpdateCommandClass))
typedef struct _SvnUpdateCommandClass SvnUpdateCommandClass;
typedef struct _SvnUpdateCommand SvnUpdateCommand;
typedef struct _SvnUpdateCommandPriv SvnUpdateCommandPriv;

struct _SvnUpdateCommandClass
{
  SvnCommandClass parent_class;
};

struct _SvnUpdateCommand
{
  SvnCommand parent_instance;

  SvnUpdateCommandPriv *priv;
};

GType
svn_update_command_get_type (void)
  G_GNUC_CONST;
     SvnUpdateCommand *svn_update_command_new (const gchar * path,
					       const gchar * revision,
					       gboolean recursive);
     void svn_update_command_destroy (SvnUpdateCommand * self);

G_END_DECLS
#endif /* _SVN_UPDATE_COMMAND_H_ */
