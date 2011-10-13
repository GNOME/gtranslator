/*
 * gtr-window-activatable.h
 * This file is part of gtr
 *
 * Copyright (C) 2010 - Steve Frécinaux
 *
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANMSGILITY or FITNESS FOR A PARTICULAR PURMSGSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GTR_WINDOW_ACTIVATABLE_H__
#define __GTR_WINDOW_ACTIVATABLE_H__

#include <glib-object.h>

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_WINDOW_ACTIVATABLE		(gtr_window_activatable_get_type ())
#define GTR_WINDOW_ACTIVATABLE(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_WINDOW_ACTIVATABLE, GtrWindowActivatable))
#define GTR_WINDOW_ACTIVATABLE_IFACE(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GTR_TYPE_WINDOW_ACTIVATABLE, GtrWindowActivatableInterface))
#define GTR_IS_WINDOW_ACTIVATABLE(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTR_TYPE_WINDOW_ACTIVATABLE))
#define GTR_WINDOW_ACTIVATABLE_GET_IFACE(obj)	(G_TYPE_INSTANCE_GET_INTERFACE ((obj), GTR_TYPE_WINDOW_ACTIVATABLE, GtrWindowActivatableInterface))
typedef struct _GtrWindowActivatable GtrWindowActivatable;      /* dummy typedef */
typedef struct _GtrWindowActivatableInterface GtrWindowActivatableInterface;

struct _GtrWindowActivatableInterface
{
  GTypeInterface g_iface;

  /* Virtual public methods */
  void (*activate) (GtrWindowActivatable * activatable);
  void (*deactivate) (GtrWindowActivatable * activatable);
  void (*update_state) (GtrWindowActivatable * activatable);
};

/*
 * Public methods
 */
GType
gtr_window_activatable_get_type (void)
  G_GNUC_CONST;

     void gtr_window_activatable_activate (GtrWindowActivatable *
                                           activatable);
     void gtr_window_activatable_deactivate (GtrWindowActivatable *
                                             activatable);
     void gtr_window_activatable_update_state (GtrWindowActivatable *
                                               activatable);

G_END_DECLS
#endif /* __GTR_WINDOW_ACTIVATABLE_H__ */
