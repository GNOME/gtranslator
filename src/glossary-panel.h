/*
 * glossary-panel.h
 * This file is part of gtranslator
 *
 * Copyright (C) 2006 - Steve Frécinaux
 *		 2007 - Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * gtranslator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gtranslator; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#ifndef __GLOSSARY_PANEL_H__
#define __GLOSSARY_PANEL_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_GLOSSARY_PANEL		(gtranslator_glossary_panel_get_type ())
#define GTR_GLOSSARY_PANEL(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_GLOSSARY_PANEL, GtranslatorGlossaryPanel))
#define GTR_GLOSSARY_PANEL_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_GLOSSARY_PANEL, GtranslatorGlossaryPanelClass))
#define GTR_IS_GLOSSARY_PANEL(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_GLOSSARY_PANEL))
#define GTR_IS_GLOSSARY_PANEL_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_GLOSSARY_PANEL))
#define GTR_GLOSSARY_PANEL_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_GLOSSARY_PANEL, GtranslatorGlossaryPanelClass))

/* Private structure type */
typedef struct _GtranslatorGlossaryPanelPrivate	GtranslatorGlossaryPanelPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorGlossaryPanel		GtranslatorGlossaryPanel;

struct _GtranslatorGlossaryPanel
{
	GtkVBox parent_instance;
	
	/*< private > */
	GtranslatorGlossaryPanelPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorGlossaryPanelClass	GtranslatorGlossaryPanelClass;

struct _GtranslatorGlossaryPanelClass
{
	GtkVBoxClass parent_class;
};

/*
 * Public methods
 */
GType		 gtranslator_glossary_panel_get_type	   (void) G_GNUC_CONST;
GType		 gtranslator_glossary_panel_register_type   (GTypeModule * module);
GtkWidget	*gtranslator_glossary_panel_new	           (void);

G_END_DECLS

#endif /* __GLOSSARY_PANEL_H__ */
