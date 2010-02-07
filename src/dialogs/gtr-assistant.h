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

#include "gtr-window.h"

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_ASSISTANT		(gtr_assistant_get_type ())
#define GTR_ASSISTANT(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_ASSISTANT, GtrAssistant))
#define GTR_ASSISTANT_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_ASSISTANT, GtrAssistantClass))
#define GTR_IS_ASSISTANT(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_ASSISTANT))
#define GTR_IS_ASSISTANT_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_ASSISTANT))
#define GTR_ASSISTANT_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_ASSISTANT, GtrAssistantClass))
/* Private structure type */
typedef struct _GtrAssistantPrivate GtrAssistantPrivate;

/*
 * Main object structure
 */
typedef struct _GtrAssistant GtrAssistant;

struct _GtrAssistant
{
  GtkAssistant parent_instance;

  /*< private > */
  GtrAssistantPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtrAssistantClass GtrAssistantClass;

struct _GtrAssistantClass
{
  GtkAssistantClass parent_class;
};

/*
 * Public methods
 */
GType
gtr_assistant_get_type (void)
  G_GNUC_CONST;

     GType gtr_assistant_register_type (GTypeModule * module);

     void gtr_show_assistant (GtrWindow * window);

G_END_DECLS
#endif /* __ASSISTANT_H__ */
