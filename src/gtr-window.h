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

#ifndef __GTR_WINDOW_H__
#define __GTR_WINDOW_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "gtr-header.h"
#include "gtr-notebook.h"
#include "gtr-tab.h"
#include "gtr-view.h"

G_BEGIN_DECLS

#define GTR_TYPE_WINDOW		(gtr_window_get_type ())
#define GTR_WINDOW(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_WINDOW, GtrWindow))
#define GTR_WINDOW_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_WINDOW, GtrWindowClass))
#define GTR_IS_WINDOW(o)	(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_WINDOW))
#define GTR_IS_WINDOW_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_WINDOW))
#define GTR_WINDOW_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_WINDOW, GtrWindowClass))

typedef struct _GtrWindow        GtrWindow;
typedef struct _GtrWindowClass   GtrWindowClass;

struct _GtrWindow
{
  GtkApplicationWindow parent_instance;
};

struct _GtrWindowClass
{
  GtkApplicationWindowClass parent_class;
};

GType gtr_window_get_type (void) G_GNUC_CONST;

GType gtr_window_register_type (GTypeModule * module);

GtrTab *gtr_window_create_tab (GtrWindow * window, GtrPo * po);

GtrTab *gtr_window_get_active_tab (GtrWindow * window);

GList *gtr_window_get_all_tabs (GtrWindow * window);

GtrNotebook *gtr_window_get_notebook (GtrWindow * window);

GtrHeader * gtr_window_get_header_from_active_tab (GtrWindow * window);

GtkWidget *gtr_window_get_statusbar (GtrWindow * window);

GtrView *gtr_window_get_active_view (GtrWindow * window);

GList *gtr_window_get_all_views (GtrWindow * window,
                               gboolean original, gboolean translated);

GtkWidget *gtr_window_get_tab_from_location (GtrWindow * window, GFile * location);

void gtr_window_set_active_tab (GtrWindow * window, GtkWidget * tab);

void _gtr_window_close_tab (GtrWindow * window, GtrTab * tab);

GtrTranslationMemory * gtr_window_get_tm (GtrWindow *window);
void gtr_window_show_tm_dialog (GtrWindow *window);

void gtr_window_remove_all_pages (GtrWindow *window);

void gtr_window_tm_keybind (GtrWindow *window, GSimpleAction *action);

void gtr_window_hide_sort_menu (GtrWindow *window);

/** stack app states **/
void gtr_window_show_projects (GtrWindow *window);
void gtr_window_show_poeditor (GtrWindow *window);
void gtr_window_show_dlteams (GtrWindow *window);

G_END_DECLS
#endif /* __GTR_WINDOW_H__ */
