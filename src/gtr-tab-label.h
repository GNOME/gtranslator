/*
 * gtr-tab-label.h
 * This file is part of gtr
 *
 * Copyright (C) 2010 - Paolo Borelli
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, 
 * Boston, MA 02111-1307, USA.
 */

#ifndef __GTR_TAB_LABEL_H__
#define __GTR_TAB_LABEL_H__

#include <gtk/gtk.h>
#include "gtr-tab.h"

G_BEGIN_DECLS

#define GTR_TYPE_TAB_LABEL		(gtr_tab_label_get_type ())
#define GTR_TAB_LABEL(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_TAB_LABEL, GtrTabLabel))
#define GTR_TAB_LABEL_CONST(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_TAB_LABEL, GtrTabLabel const))
#define GTR_TAB_LABEL_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GTR_TYPE_TAB_LABEL, GtrTabLabelClass))
#define GTR_IS_TAB_LABEL(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTR_TYPE_TAB_LABEL))
#define GTR_IS_TAB_LABEL_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_TYPE_TAB_LABEL))
#define GTR_TAB_LABEL_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GTR_TYPE_TAB_LABEL, GtrTabLabelClass))

typedef struct _GtrTabLabel		GtrTabLabel;
typedef struct _GtrTabLabelClass	GtrTabLabelClass;
typedef struct _GtrTabLabelPrivate	GtrTabLabelPrivate;

struct _GtrTabLabel {
	GtkBox parent;
	
	GtrTabLabelPrivate *priv;
};

struct _GtrTabLabelClass {
	GtkBoxClass parent_class;

	void (* close_clicked)  (GtrTabLabel *tab_label);
};

GType		 gtr_tab_label_get_type (void) G_GNUC_CONST;

GtkWidget 	*gtr_tab_label_new (GtrTab *tab);

GtrTab	*gtr_tab_label_get_tab (GtrTabLabel *tab_label);

void		gtr_tab_label_set_close_button_sensitive (GtrTabLabel *tab_label,
							    gboolean       sensitive);

G_END_DECLS

#endif /* __GTR_TAB_LABEL_H__ */
