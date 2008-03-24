/*
 * Copyright (C) 2005 - Paolo Borelli
 * 		 2007 - Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, 
 * Boston, MA 02111-1307, USA.
 */
 
#ifndef GTR_STATUSBAR_H
#define GTR_STATUSBAR_H

#include <gtk/gtkhbox.h>

G_BEGIN_DECLS

#define GTR_TYPE_STATUSBAR		(gtranslator_statusbar_get_type ())
#define GTR_STATUSBAR(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_STATUSBAR, GtranslatorStatusbar))
#define GTR_STATUSBAR_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_STATUSBAR, GtranslatorStatusbarClass))
#define GTR_IS_STATUSBAR(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_STATUSBAR))
#define GTR_IS_STATUSBAR_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_STATUSBAR))
#define GTR_STATUSBAR_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_STATUSBAR, GtranslatorStatusbarClass))

typedef struct _GtranslatorStatusbar		GtranslatorStatusbar;
typedef struct _GtranslatorStatusbarPrivate	GtranslatorStatusbarPrivate;
typedef struct _GtranslatorStatusbarClass	GtranslatorStatusbarClass;

struct _GtranslatorStatusbar
{
        GtkHBox parent;

	/* <private/> */
        GtranslatorStatusbarPrivate *priv;
};

struct _GtranslatorStatusbarClass
{
        GtkHBoxClass parent_class;
};

GType		 gtranslator_statusbar_get_type	                (void) G_GNUC_CONST;

GtkWidget	*gtranslator_statusbar_new			(void);

guint            gtranslator_statusbar_push                     (GtranslatorStatusbar *statusbar,
								 guint context_id,
								 const gchar *text);

void             gtranslator_statusbar_pop                      (GtranslatorStatusbar *statusbar,
								 guint context_id);

void		 gtranslator_statusbar_set_overwrite		(GtranslatorStatusbar   *statusbar,
								 gboolean          overwrite);

void		 gtranslator_statusbar_clear_overwrite  	(GtranslatorStatusbar   *statusbar);

void		 gtranslator_statusbar_flash_message		(GtranslatorStatusbar   *statusbar,
							 	 guint             context_id,
							 	 const gchar      *format,
							 	 ...) G_GNUC_PRINTF(3, 4);

void             gtranslator_statusbar_update_progress_bar      (GtranslatorStatusbar *statusbar,
								 gdouble translated_count,
								 gdouble messages_count);

G_END_DECLS

#endif
