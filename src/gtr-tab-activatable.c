/*
 * gtr-tab-activatable.h
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

#include "gtr-tab-activatable.h"
#include "gtr-tab.h"

/**
 * SECTION:gtr-tab-activatable
 * @short_description: Interface for activatable extensions on tabs
 * @see_also: #PeasExtensionSet
 *
 * #GtrTabActivatable is an interface which should be implemented by
 * extensions that should be activated on a gtr main tab.
 **/

G_DEFINE_INTERFACE (GtrTabActivatable, gtr_tab_activatable, G_TYPE_OBJECT)

void
gtr_tab_activatable_default_init (GtrTabActivatableInterface * iface)
{
  static gboolean initialized = FALSE;

  if (!initialized)
    {
      /**
       * GtrTabActivatable:tab:
       *
       * The tab property contains the gtr tab for this
       * #GtrTabActivatable instance.
       */
      g_object_interface_install_property (iface,
                                           g_param_spec_object ("tab",
                                                                "Tab",
                                                                "The gtranslator tab",
                                                                GTR_TYPE_TAB,
                                                                G_PARAM_READWRITE |
                                                                G_PARAM_CONSTRUCT_ONLY |
                                                                G_PARAM_STATIC_STRINGS));

      initialized = TRUE;
    }
}

/**
 * gtr_tab_activatable_activate:
 * @activatable: A #GtrTabActivatable.
 *
 * Activates the extension on the tab property.
 */
void
gtr_tab_activatable_activate (GtrTabActivatable * activatable)
{
  GtrTabActivatableInterface *iface;

  g_return_if_fail (GTR_IS_TAB_ACTIVATABLE (activatable));

  iface = GTR_TAB_ACTIVATABLE_GET_IFACE (activatable);
  if (iface->activate != NULL)
    {
      iface->activate (activatable);
    }
}

/**
 * gtr_tab_activatable_deactivate:
 * @activatable: A #GtrTabActivatable.
 *
 * Deactivates the extension on the tab property.
 */
void
gtr_tab_activatable_deactivate (GtrTabActivatable * activatable)
{
  GtrTabActivatableInterface *iface;

  g_return_if_fail (GTR_IS_TAB_ACTIVATABLE (activatable));

  iface = GTR_TAB_ACTIVATABLE_GET_IFACE (activatable);
  if (iface->deactivate != NULL)
    {
      iface->deactivate (activatable);
    }
}

/**
 * gtr_tab_activatable_update_state:
 * @activatable: A #GtrTabActivatable.
 *
 * Triggers an update of the extension internal state to take into account
 * state changes in the tab, due to some event or user action.
 */
void
gtr_tab_activatable_update_state (GtrTabActivatable * activatable)
{
  GtrTabActivatableInterface *iface;

  g_return_if_fail (GTR_IS_TAB_ACTIVATABLE (activatable));

  iface = GTR_TAB_ACTIVATABLE_GET_IFACE (activatable);
  if (iface->update_state != NULL)
    {
      iface->update_state (activatable);
    }
}
