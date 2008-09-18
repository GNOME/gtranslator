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

#ifndef _SVN_STATUS_H_
#define _SVN_STATUS_H_

#include <glib-object.h>
#include <svn_wc.h>
#include "vcs-status-tree-view.h"

G_BEGIN_DECLS

#define SVN_TYPE_STATUS             (svn_status_get_type ())
#define SVN_STATUS(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), SVN_TYPE_STATUS, SvnStatus))
#define SVN_STATUS_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), SVN_TYPE_STATUS, SvnStatusClass))
#define SVN_IS_STATUS(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SVN_TYPE_STATUS))
#define SVN_IS_STATUS_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), SVN_TYPE_STATUS))
#define SVN_STATUS_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), SVN_TYPE_STATUS, SvnStatusClass))

typedef struct _SvnStatusClass SvnStatusClass;
typedef struct _SvnStatus SvnStatus;
typedef struct _SvnStatusPriv SvnStatusPriv;

struct _SvnStatusClass
{
	GObjectClass parent_class;
};

struct _SvnStatus
{
	GObject parent_instance;
	
	SvnStatusPriv *priv;
};

GType svn_status_get_type (void) G_GNUC_CONST;
SvnStatus *svn_status_new (gchar *path, enum svn_wc_status_kind status);
void svn_status_destroy (SvnStatus *self);
gchar * svn_status_get_path (SvnStatus *self);
GtranslatorVcsStatus svn_status_get_vcs_status (SvnStatus *self);

G_END_DECLS

#endif /* _SVN_STATUS_H_ */
