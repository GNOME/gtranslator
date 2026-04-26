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

#pragma once

#include <glib-object.h>
#include <glib.h>

#include "gtr-msg.h"

G_BEGIN_DECLS

#define GTR_TYPE_MESSAGE_CONTAINER (gtr_message_container_get_type ())

G_DECLARE_INTERFACE (GtrMessageContainer, gtr_message_container, GTR, MESSAGE_CONTAINER, GObject);

struct _GtrMessageContainerInterface
{
  GTypeInterface g_iface;

  GtrMsg * (* get_message)        (GtrMessageContainer *container,
                                   gint                 number);
  gint     (* get_message_number) (GtrMessageContainer *container,
                                   GtrMsg *msg);
  gint     (* get_count)          (GtrMessageContainer *container);
};

GtrMsg * gtr_message_container_get_message (GtrMessageContainer * container,
                                            gint number);
gint     gtr_message_container_get_message_number (GtrMessageContainer * container,
                                                   GtrMsg * msg);
gint     gtr_message_container_get_count   (GtrMessageContainer * container);


G_END_DECLS
