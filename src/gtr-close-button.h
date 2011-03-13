/*
 * gtr-close-button.h
 * This file is part of gtr
 *
 * Copyright (C) 2010 - Paolo Borelli
 *
 * gtr is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * gtr is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __GTR_CLOSE_BUTTON_H__
#define __GTR_CLOSE_BUTTON_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTR_TYPE_CLOSE_BUTTON			(gtr_close_button_get_type ())
#define GTR_CLOSE_BUTTON(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_CLOSE_BUTTON, GtrCloseButton))
#define GTR_CLOSE_BUTTON_CONST(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_CLOSE_BUTTON, GtrCloseButton const))
#define GTR_CLOSE_BUTTON_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), GTR_TYPE_CLOSE_BUTTON, GtrCloseButtonClass))
#define GTR_IS_CLOSE_BUTTON(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTR_TYPE_CLOSE_BUTTON))
#define GTR_IS_CLOSE_BUTTON_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_TYPE_CLOSE_BUTTON))
#define GTR_CLOSE_BUTTON_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GTR_TYPE_CLOSE_BUTTON, GtrCloseButtonClass))

typedef struct _GtrCloseButton		GtrCloseButton;
typedef struct _GtrCloseButtonClass		GtrCloseButtonClass;
typedef struct _GtrCloseButtonClassPrivate	GtrCloseButtonClassPrivate;

struct _GtrCloseButton
{
	GtkButton parent;
};

struct _GtrCloseButtonClass
{
	GtkButtonClass parent_class;

	GtrCloseButtonClassPrivate *priv;
};

GType		  gtr_close_button_get_type (void) G_GNUC_CONST;

GtkWidget	 *gtr_close_button_new      (void);

G_END_DECLS

#endif /* __GTR_CLOSE_BUTTON_H__ */
/* ex:set ts=8 noet: */
