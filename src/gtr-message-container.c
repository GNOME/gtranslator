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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-message-container.h"

G_DEFINE_INTERFACE (GtrMessageContainer, gtr_message_container, G_TYPE_OBJECT)

static void
gtr_message_container_default_init (GtrMessageContainerInterface * iface)
{
}

/**
 * gtr_message_container_get_message:
 * @container:
 * @number:
 *
 * Returns: (transfer none):
 */
GtrMsg *
gtr_message_container_get_message (GtrMessageContainer * container,
                                   gint number)
{
  g_return_val_if_fail (GTR_IS_MESSAGE_CONTAINER (container), NULL);

  return GTR_MESSAGE_CONTAINER_GET_IFACE (container)->get_message (container,
                                                                   number);
}

gint
gtr_message_container_get_message_number (GtrMessageContainer * container,
                                          GtrMsg * msg)
{
  g_return_val_if_fail (GTR_IS_MESSAGE_CONTAINER (container), -1);
  g_return_val_if_fail (GTR_IS_MSG (msg), -1);

  return GTR_MESSAGE_CONTAINER_GET_IFACE (container)->get_message_number (container, msg);
}


gint
gtr_message_container_get_count (GtrMessageContainer * container)
{
  g_return_val_if_fail (GTR_IS_MESSAGE_CONTAINER (container), 0);

  return GTR_MESSAGE_CONTAINER_GET_IFACE (container)->get_count (container);
}
