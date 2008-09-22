/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * anjuta
 * Copyright (C) James Liggett 2007 <jrliggett@cox.net>
 * 
 * anjuta is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * anjuta is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with anjuta.  If not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#ifndef _SVN_DIFF_COMMAND_H_
#define _SVN_DIFF_COMMAND_H_

#include <glib-object.h>
#include <svn_utf.h>
#include "svn-command.h"

G_BEGIN_DECLS

#define SVN_TYPE_DIFF_COMMAND             (svn_diff_command_get_type ())
#define SVN_DIFF_COMMAND(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), SVN_TYPE_DIFF_COMMAND, SvnDiffCommand))
#define SVN_DIFF_COMMAND_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), SVN_TYPE_DIFF_COMMAND, SvnDiffCommandClass))
#define SVN_IS_DIFF_COMMAND(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SVN_TYPE_DIFF_COMMAND))
#define SVN_IS_DIFF_COMMAND_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), SVN_TYPE_DIFF_COMMAND))
#define SVN_DIFF_COMMAND_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), SVN_TYPE_DIFF_COMMAND, SvnDiffCommandClass))

typedef struct _SvnDiffCommandClass SvnDiffCommandClass;
typedef struct _SvnDiffCommand SvnDiffCommand;
typedef struct _SvnDiffCommandPriv SvnDiffCommandPriv;

struct _SvnDiffCommandClass
{
	SvnCommandClass parent_class;
};

struct _SvnDiffCommand
{
	SvnCommandClass parent_instance;
	
	SvnDiffCommandPriv *priv;
};

enum
{
	SVN_DIFF_REVISION_PREVIOUS = -1,
	SVN_DIFF_REVISION_NONE = 0
}; 

GType svn_diff_command_get_type (void) G_GNUC_CONST;
SvnDiffCommand *svn_diff_command_new (gchar *path, glong revision1,
									  glong revision2, gboolean recursive);
void svn_diff_command_destroy (SvnDiffCommand *self);

GQueue *svn_diff_command_get_output (SvnDiffCommand *self);

G_END_DECLS

#endif /* _SVN_DIFF_COMMAND_H_ */
