/*
 * gtr-window-activatable.h
 * This file is part of gtr
 *
 * Copyright (C) 2010 Steve Frécinaux
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Library General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-window-activatable.h"
#include "gtr-window.h"

/**
 * SECTION:gtr-window-activatable
 * @short_description: Interface for activatable extensions on windows
 * @see_also: #PeasExtensionSet
 *
 * #GtrWindowActivatable is an interface which should be implemented by
 * extensions that should be activated on a gtr main window.
 **/

G_DEFINE_INTERFACE (GtrWindowActivatable, gtr_window_activatable,
                    G_TYPE_OBJECT)
     void gtr_window_activatable_default_init (GtrWindowActivatableInterface *
                                               iface)
{
  static gboolean initialized = FALSE;

  if (!initialized)
    {
                /**
		 * GtrWindowActivatable:window:
		 *
		 * The window property contains the gtr window for this
		 * #GtrWindowActivatable instance.
		 */
      g_object_interface_install_property (iface,
                                           g_param_spec_object ("window",
                                                                "Window",
                                                                "The gtranslator window",
                                                                GTR_TYPE_WINDOW,
                                                                G_PARAM_READWRITE
                                                                |
                                                                G_PARAM_CONSTRUCT_ONLY
                                                                |
                                                                G_PARAM_STATIC_STRINGS));

      initialized = TRUE;
    }
}

/**
 * gtr_window_activatable_activate:
 * @activatable: A #GtrWindowActivatable.
 *
 * Activates the extension on the window property.
 */
void
gtr_window_activatable_activate (GtrWindowActivatable * activatable)
{
  GtrWindowActivatableInterface *iface;

  g_return_if_fail (GTR_IS_WINDOW_ACTIVATABLE (activatable));

  iface = GTR_WINDOW_ACTIVATABLE_GET_IFACE (activatable);
  if (iface->activate != NULL)
    {
      iface->activate (activatable);
    }
}

/**
 * gtr_window_activatable_deactivate:
 * @activatable: A #GtrWindowActivatable.
 *
 * Deactivates the extension on the window property.
 */
void
gtr_window_activatable_deactivate (GtrWindowActivatable * activatable)
{
  GtrWindowActivatableInterface *iface;

  g_return_if_fail (GTR_IS_WINDOW_ACTIVATABLE (activatable));

  iface = GTR_WINDOW_ACTIVATABLE_GET_IFACE (activatable);
  if (iface->deactivate != NULL)
    {
      iface->deactivate (activatable);
    }
}

/**
 * gtr_window_activatable_update_state:
 * @activatable: A #GtrWindowActivatable.
 *
 * Triggers an update of the extension internal state to take into account
 * state changes in the window, due to some event or user action.
 */
void
gtr_window_activatable_update_state (GtrWindowActivatable * activatable)
{
  GtrWindowActivatableInterface *iface;

  g_return_if_fail (GTR_IS_WINDOW_ACTIVATABLE (activatable));

  iface = GTR_WINDOW_ACTIVATABLE_GET_IFACE (activatable);
  if (iface->update_state != NULL)
    {
      iface->update_state (activatable);
    }
}
