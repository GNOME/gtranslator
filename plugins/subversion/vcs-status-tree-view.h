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

#ifndef _GTR_VCS_STATUS_TREE_VIEW_H_
#define _GTR_VCS_STATUS_TREE_VIEW_H_

#include <glib-object.h>
#include <gtk/gtk.h>
#include "subversion-enum-types.h"

G_BEGIN_DECLS

#define GTR_VCS_TYPE_STATUS_TREE_VIEW             (gtranslator_vcs_status_tree_view_get_type ())
#define GTR_VCS_STATUS_TREE_VIEW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_VCS_TYPE_STATUS_TREE_VIEW, GtranslatorVcsStatusTreeView))
#define GTR_VCS_STATUS_TREE_VIEW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GTR_VCS_TYPE_STATUS_TREE_VIEW, GtranslatorVcsStatusTreeViewClass))
#define GTR_VCS_IS_STATUS_TREE_VIEW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTR_VCS_TYPE_STATUS_TREE_VIEW))
#define GTR_VCS_IS_STATUS_TREE_VIEW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_VCS_TYPE_STATUS_TREE_VIEW))
#define GTR_VCS_STATUS_TREE_VIEW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GTR_VCS_TYPE_STATUS_TREE_VIEW, GtranslatorVcsStatusTreeViewClass))

/* Show default status codes: Modified, Added, Deleted, and Conflicted */
#define GTR_VCS_DEFAULT_STATUS_CODES (GTR_VCS_STATUS_MODIFIED | \
										 GTR_VCS_STATUS_ADDED |    \
										 GTR_VCS_STATUS_DELETED |  \
										 GTR_VCS_STATUS_CONFLICTED)

typedef struct _GtranslatorVcsStatusTreeViewClass GtranslatorVcsStatusTreeViewClass;
typedef struct _GtranslatorVcsStatusTreeView GtranslatorVcsStatusTreeView;
typedef struct _GtranslatorVcsStatusTreeViewPriv GtranslatorVcsStatusTreeViewPriv;

struct _GtranslatorVcsStatusTreeViewClass
{
	GtkTreeViewClass parent_class;
};

struct _GtranslatorVcsStatusTreeView
{
	GtkTreeView parent_instance;
	
	GtranslatorVcsStatusTreeViewPriv *priv;
};

typedef enum
{
	/* Unversioned, ignored, or uninteresting items */
	GTR_VCS_STATUS_NONE = 0, /*< skip >*/
	
	GTR_VCS_STATUS_MODIFIED = 1 << 0,
	GTR_VCS_STATUS_ADDED = 1 << 1,
	GTR_VCS_STATUS_DELETED = 1 << 2,
	GTR_VCS_STATUS_CONFLICTED = 1 << 3,
	GTR_VCS_STATUS_MISSING = 1 << 4,
} GtranslatorVcsStatus;

GType gtranslator_vcs_status_tree_view_get_type (void) G_GNUC_CONST;
GtkWidget *gtranslator_vcs_status_tree_view_new (void);
void gtranslator_vcs_status_tree_view_destroy (GtranslatorVcsStatusTreeView *self);
void gtranslator_vcs_status_tree_view_add (GtranslatorVcsStatusTreeView *self, 
									  gchar *path, 
									  GtranslatorVcsStatus status, 
									  gboolean selected);
void gtranslator_vsc_status_tree_view_clear (GtranslatorVcsStatusTreeView *self);
void gtranslator_vcs_status_tree_view_select_all (GtranslatorVcsStatusTreeView *self);
void gtranslator_vcs_status_tree_view_unselect_all (GtranslatorVcsStatusTreeView *self);
GList *gtranslator_vcs_status_tree_view_get_selected (GtranslatorVcsStatusTreeView *self);

G_END_DECLS

#endif /* _GTR_VCS_STATUS_TREE_VIEW_H_ */
