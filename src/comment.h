/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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
 */

#ifndef __COMMENT_PANEL_H__
#define __COMMENT_PANEL_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_COMMENT_PANEL		(gtranslator_comment_panel_get_type ())
#define GTR_COMMENT_PANEL(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_COMMENT_PANEL, GtranslatorCommentPanel))
#define GTR_COMMENT_PANEL_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_COMMENT_PANEL, GtranslatorCommentPanelClass))
#define GTR_IS_COMMENT_PANEL(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_COMMENT_PANEL))
#define GTR_IS_COMMENT_PANEL_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_COMMENT_PANEL))
#define GTR_COMMENT_PANEL_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_COMMENT_PANEL, GtranslatorCommentPanelClass))

/* Private structure type */
typedef struct _GtranslatorCommentPanelPrivate	GtranslatorCommentPanelPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorCommentPanel		GtranslatorCommentPanel;

struct _GtranslatorCommentPanel
{
	GtkVBox parent_instance;
	
	/*< private > */
	GtranslatorCommentPanelPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorCommentPanelClass	GtranslatorCommentPanelClass;

struct _GtranslatorCommentPanelClass
{
	GtkVBoxClass parent_class;
};

/*
 * Public methods
 */
GType		 gtranslator_comment_panel_get_type	   (void) G_GNUC_CONST;

GType		 gtranslator_comment_panel_register_type   (GTypeModule * module);

GtkWidget	*gtranslator_comment_panel_new	           (GtkWidget *tab);

void             gtranslator_comment_panel_set_comments    (GtranslatorCommentPanel *panel,
							    const gchar *comments);

void             gtranslator_comment_panel_set_extracted_comments
							   (GtranslatorCommentPanel *panel,
						            const gchar *extracted_comments);

G_END_DECLS

#endif /* __COMMENT_PANEL_H__ */
