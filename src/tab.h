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
 *
 */

#ifndef __TAB_H__
#define __TAB_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "comment.h"
#include "msg.h"
#include "po.h"
#include "view.h"

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_TAB		(gtranslator_tab_get_type ())
#define GTR_TAB(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_TAB, GtranslatorTab))
#define GTR_TAB_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_TAB, GtranslatorTabClass))
#define GTR_IS_TAB(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_TAB))
#define GTR_IS_TAB_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_TAB))
#define GTR_TAB_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_TAB, GtranslatorTabClass))

/* Private structure type */
typedef struct _GtranslatorTabPrivate	GtranslatorTabPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorTab		GtranslatorTab;

struct _GtranslatorTab
{
	GtkVBox parent_instance;
	
	/*< private > */
	GtranslatorTabPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorTabClass	GtranslatorTabClass;

struct _GtranslatorTabClass
{
	GtkVBoxClass parent_class;
	
	void (* showed_message)  (GtranslatorTab *tab,
				  GtranslatorMsg *msg);
	void (* message_changed) (GtranslatorTab *tab,
				  GtranslatorMsg *msg);
	void (* message_edition_finished)  (GtranslatorTab *tab,
					    GtranslatorMsg *msg);
};

/*
 * Public methods
 */
GType		       gtranslator_tab_get_type            (void) G_GNUC_CONST;

GType	               gtranslator_tab_register_type       (GTypeModule * module);

GtranslatorTab        *gtranslator_tab_new	           (GtranslatorPo *po);

GtranslatorPo         *gtranslator_tab_get_po              (GtranslatorTab *tab);

GtkWidget             *gtranslator_tab_get_panel           (GtranslatorTab *tab);

gint                   gtranslator_tab_get_active_text_tab (GtranslatorTab *tab);

gint                   gtranslator_tab_get_active_trans_tab(GtranslatorTab *tab);

GtranslatorCommentPanel *gtranslator_tab_get_comment_panel   (GtranslatorTab *tab);

GtranslatorView       *gtranslator_tab_get_active_view     (GtranslatorTab *tab);

GList                 *gtranslator_tab_get_all_views       (GtranslatorTab *tab,
							    gboolean original,
							    gboolean translated);

gchar                 *gtranslator_tab_get_name            (GtranslatorTab *tab);

void                   gtranslator_tab_message_go_to       (GtranslatorTab *tab,
							    GList * to_go,
							    gboolean searching);
							    
GtranslatorTab        *gtranslator_tab_get_from_document   (GtranslatorPo *po);

gboolean               gtranslator_tab_get_autosave_enabled (GtranslatorTab *tab);

void                   gtranslator_tab_set_autosave_enabled (GtranslatorTab *tab, 
							     gboolean enable);

gint                   gtranslator_tab_get_autosave_interval (GtranslatorTab *tab);

void                   gtranslator_tab_set_autosave_interval (GtranslatorTab *tab, 
							      gint interval);
							    
gboolean              _gtranslator_tab_can_close           (GtranslatorTab *tab);

G_END_DECLS

#endif /* __TAB_H__ */
