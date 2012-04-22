/*
 * Copyright (C) 2012  Carlos Garnacho  <carlosg@gnome.org>
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
 *   Carlos Garnacho <carlosg@gnome.org>
 */

#ifndef __MESSAGE_CONTAINER_H__
#define __MESSAGE_CONTAINER_H__

#include <glib.h>
#include <glib-object.h>
#include "gtr-msg.h"

G_BEGIN_DECLS

#define GTR_TYPE_MESSAGE_CONTAINER           (gtr_message_container_get_type ())
#define GTR_MESSAGE_CONTAINER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_MESSAGE_CONTAINER, GtrMessageContainer))
#define GTR_IS_MESSAGE_CONTAINER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTR_TYPE_MESSAGE_CONTAINER))
#define GTR_MESSAGE_CONTAINER_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GTR_TYPE_MESSAGE_CONTAINER, GtrMessageContainerInterface))

typedef struct _GtrMessageContainer GtrMessageContainer;
typedef struct _GtrMessageContainerInterface GtrMessageContainerInterface;

struct _GtrMessageContainerInterface
{
  GTypeInterface g_iface;

  GtrMsg * (* get_message)        (GtrMessageContainer *container,
                                   gint                 number);
  gint     (* get_message_number) (GtrMessageContainer *container,
                                   GtrMsg *msg);
  gint     (* get_count)          (GtrMessageContainer *container);
};

GType    gtr_message_container_get_type    (void) G_GNUC_CONST;
GtrMsg * gtr_message_container_get_message (GtrMessageContainer * container,
                                            gint number);
gint     gtr_message_container_get_message_number (GtrMessageContainer * container,
                                                   GtrMsg * msg);
gint     gtr_message_container_get_count   (GtrMessageContainer * container);


G_END_DECLS

#endif /* __MESSAGE_CONTAINER_H__ */
