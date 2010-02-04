/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *               2008  Igalia
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *   Pablo Sanxiao <psanxiao@gmail.com>
 */

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "gtr-header.h"
#include "gtr-notebook.h"
#include "gtr-tab.h"
#include "gtr-view.h"

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_WINDOW		(gtranslator_window_get_type ())
#define GTR_WINDOW(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_WINDOW, GtrWindow))
#define GTR_WINDOW_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_WINDOW, GtrWindowClass))
#define GTR_IS_WINDOW(o)	(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_WINDOW))
#define GTR_IS_WINDOW_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_WINDOW))
#define GTR_WINDOW_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_WINDOW, GtrWindowClass))
/* Private structure type */
typedef struct _GtrWindowPrivate GtrWindowPrivate;

/*
 * Main object structure
 */
typedef struct _GtrWindow GtrWindow;

struct _GtrWindow
{
  GtkWindow parent_instance;

  /*< private > */
  GtrWindowPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtrWindowClass GtrWindowClass;

struct _GtrWindowClass
{
  GtkWindowClass parent_class;
};

typedef enum
{
  GTR_WINDOW_PLACEMENT_NONE = 0,
  GTR_WINDOW_PLACEMENT_TOP,
  GTR_WINDOW_PLACEMENT_BOTTOM,
  GTR_WINDOW_PLACEMENT_RIGHT,
  GTR_WINDOW_PLACEMENT_LEFT,
  GTR_WINDOW_PLACEMENT_CENTER,
  GTR_WINDOW_PLACEMENT_FLOATING
} GtrWindowPlacement;

/*
 * Public methods
 */
GType
gtranslator_window_get_type (void)
  G_GNUC_CONST;

     GType gtranslator_window_register_type (GTypeModule * module);

     GtrTab *gtranslator_window_create_tab (GtrWindow *
						    window,
						    GtrPo * po);

     GtrTab *gtranslator_window_get_active_tab (GtrWindow *
							window);

     GList *gtranslator_window_get_all_tabs (GtrWindow * window);

     GtrNotebook *gtranslator_window_get_notebook (GtrWindow *
							   window);
     GtrHeader
       *gtranslator_window_get_header_from_active_tab (GtrWindow *
						       window);

     GtkWidget *gtranslator_window_get_statusbar (GtrWindow * window);

     GtkUIManager *gtranslator_window_get_ui_manager (GtrWindow *
						      window);

     GtrView *gtranslator_window_get_active_view (GtrWindow *
							  window);

     GList *gtranslator_window_get_all_views (GtrWindow * window,
					      gboolean original,
					      gboolean translated);

     void set_sensitive_according_to_message (GtrWindow * window,
					      GtrPo * po);

     void set_sensitive_according_to_window (GtrWindow * window);

     void gtranslator_window_add_widget (GtrWindow * window,
					 GtkWidget * widget,
					 const gchar * name,
					 const gchar * title,
					 const gchar * stock_id,
					 GtrWindowPlacement
					 placement);

     void gtranslator_window_remove_widget (GtrWindow * window,
					    GtkWidget * widget);

     GObject *_gtranslator_window_get_layout_manager
       (GtrWindow * window);

     GtkWidget *gtranslator_window_get_tab_from_location
       (GtrWindow * window, GFile * location);

     void gtranslator_window_set_active_tab (GtrWindow * window,
					     GtkWidget * tab);

     void _gtranslator_window_close_tab (GtrWindow * window,
					 GtrTab * tab);

     GtkWidget *gtranslator_window_get_tm_menu (GtrWindow * window);

G_END_DECLS
#endif /* __WINDOW_H__ */
