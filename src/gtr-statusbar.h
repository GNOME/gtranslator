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

#include <gtk/gtk.h>

G_BEGIN_DECLS
#define GTR_TYPE_STATUSBAR		(gtr_statusbar_get_type ())
#define GTR_STATUSBAR(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_STATUSBAR, GtrStatusbar))
#define GTR_STATUSBAR_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_STATUSBAR, GtrStatusbarClass))
#define GTR_IS_STATUSBAR(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_STATUSBAR))
#define GTR_IS_STATUSBAR_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_STATUSBAR))
#define GTR_STATUSBAR_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_STATUSBAR, GtrStatusbarClass))
typedef struct _GtrStatusbar GtrStatusbar;
typedef struct _GtrStatusbarPrivate GtrStatusbarPrivate;
typedef struct _GtrStatusbarClass GtrStatusbarClass;

struct _GtrStatusbar
{
  GtkBox parent;

  /* <private/> */
  GtrStatusbarPrivate *priv;
};

struct _GtrStatusbarClass
{
  GtkBoxClass parent_class;
};

GType
gtr_statusbar_get_type (void)
  G_GNUC_CONST;

     GtkWidget *gtr_statusbar_new (void);

     void gtr_statusbar_push_default (GtrStatusbar *
                                      statusbar, const gchar * text);

     void gtr_statusbar_pop_default (GtrStatusbar * statusbar);

     guint gtr_statusbar_push (GtrStatusbar * statusbar,
                               guint context_id, const gchar * text);

     void gtr_statusbar_pop (GtrStatusbar * statusbar, guint context_id);

     guint gtr_statusbar_get_context_id (GtrStatusbar *
                                         statusbar,
                                         const gchar * context_description);

     void gtr_statusbar_set_overwrite (GtrStatusbar *
                                       statusbar, gboolean overwrite);

     void gtr_statusbar_clear_overwrite (GtrStatusbar * statusbar);

     void gtr_statusbar_flash_message (GtrStatusbar *
                                       statusbar, guint context_id,
                                       const gchar * format,
                                       ...) G_GNUC_PRINTF (3, 4);

     void gtr_statusbar_update_progress_bar (GtrStatusbar *
                                             statusbar,
                                             gdouble translated_count,
                                             gdouble messages_count);

     void gtr_statusbar_clear_progress_bar (GtrStatusbar * statusbar);

G_END_DECLS
#endif
