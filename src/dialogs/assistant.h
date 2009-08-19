/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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

#ifndef __ASSISTANT_H__
#define __ASSISTANT_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include "window.h"

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_ASSISTANT		(gtranslator_assistant_get_type ())
#define GTR_ASSISTANT(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_ASSISTANT, GtranslatorAssistant))
#define GTR_ASSISTANT_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_ASSISTANT, GtranslatorAssistantClass))
#define GTR_IS_ASSISTANT(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_ASSISTANT))
#define GTR_IS_ASSISTANT_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_ASSISTANT))
#define GTR_ASSISTANT_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_ASSISTANT, GtranslatorAssistantClass))
/* Private structure type */
typedef struct _GtranslatorAssistantPrivate GtranslatorAssistantPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorAssistant GtranslatorAssistant;

struct _GtranslatorAssistant
{
  GtkAssistant parent_instance;

  /*< private > */
  GtranslatorAssistantPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorAssistantClass GtranslatorAssistantClass;

struct _GtranslatorAssistantClass
{
  GtkAssistantClass parent_class;
};

/*
 * Public methods
 */
GType
gtranslator_assistant_get_type (void)
  G_GNUC_CONST;

     GType gtranslator_assistant_register_type (GTypeModule * module);

     void gtranslator_show_assistant (GtranslatorWindow * window);

G_END_DECLS
#endif /* __ASSISTANT_H__ */
