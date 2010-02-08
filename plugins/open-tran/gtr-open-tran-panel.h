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
 *     MERCHANPOILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __OPEN_TRAN_PANEL_H__
#define __OPEN_TRAN_PANEL_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "gtr-window.h"

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_OPEN_TRAN_PANEL		(gtr_open_tran_panel_get_type ())
#define GTR_OPEN_TRAN_PANEL(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_OPEN_TRAN_PANEL, GtrOpenTranPanel))
#define GTR_OPEN_TRAN_PANEL_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_OPEN_TRAN_PANEL, GtrOpenTranPanelClass))
#define GTR_IS_OPEN_TRAN_PANEL(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_OPEN_TRAN_PANEL))
#define GTR_IS_OPEN_TRAN_PANEL_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_OPEN_TRAN_PANEL))
#define GTR_OPEN_TRAN_PANEL_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_OPEN_TRAN_PANEL, GtrOpenTranPanelClass))
/* Gconf keys */
#define OPEN_TRAN_BASE_KEY "/apps/gtranslator/plugins/open-tran"
#define SEARCH_CODE_KEY OPEN_TRAN_BASE_KEY "/search_code"
#define OWN_CODE_KEY OPEN_TRAN_BASE_KEY "/own_code"
/* Private structure type */
typedef struct _GtrOpenTranPanelPrivate GtrOpenTranPanelPrivate;

/*
 * Main object structure
 */
typedef struct _GtrOpenTranPanel GtrOpenTranPanel;

struct _GtrOpenTranPanel
{
  GtkVBox parent_instance;

  /*< private > */
  GtrOpenTranPanelPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtrOpenTranPanelClass GtrOpenTranPanelClass;

struct _GtrOpenTranPanelClass
{
  GtkVBoxClass parent_class;
};

/*
 * Public methods
 */
GType
gtr_open_tran_panel_get_type (void)
  G_GNUC_CONST;
     GType gtr_open_tran_panel_register_type (GTypeModule * module);
     GtkWidget *gtr_open_tran_panel_new (GtrWindow * window);

G_END_DECLS
#endif /* __OPEN_TRAN_PANEL_H__ */
