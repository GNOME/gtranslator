/*
 * glossary-panel.h
 * This file is part of gtr
 *
 * Copyright (C) 2006 - Steve Frécinaux
 *               2007 - Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef __GLOSSARY_PANEL_H__
#define __GLOSSARY_PANEL_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTR_TYPE_GLOSSARY_PANEL         (gtr_glossary_panel_get_type ())
#define GTR_GLOSSARY_PANEL(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_GLOSSARY_PANEL, GtrGlossaryPanel))
#define GTR_GLOSSARY_PANEL_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_GLOSSARY_PANEL, GtrGlossaryPanelClass))
#define GTR_IS_GLOSSARY_PANEL(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_GLOSSARY_PANEL))
#define GTR_IS_GLOSSARY_PANEL_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_GLOSSARY_PANEL))
#define GTR_GLOSSARY_PANEL_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_GLOSSARY_PANEL, GtrGlossaryPanelClass))

typedef struct _GtrGlossaryPanel        GtrGlossaryPanel;
typedef struct _GtrGlossaryPanelClass   GtrGlossaryPanelClass;
typedef struct _GtrGlossaryPanelPrivate GtrGlossaryPanelPrivate;

struct _GtrGlossaryPanel
{
  GtkBox parent_instance;

  /*< private > */
  GtrGlossaryPanelPrivate *priv;
};

struct _GtrGlossaryPanelClass
{
  GtkBoxClass parent_class;
};

GType            gtr_glossary_panel_get_type            (void) G_GNUC_CONST;

GType            gtr_glossary_panel_register_type       (GTypeModule *module);

GtkWidget       *gtr_glossary_panel_new                 (void);

G_END_DECLS
#endif /* __GLOSSARY_PANEL_H__ */
