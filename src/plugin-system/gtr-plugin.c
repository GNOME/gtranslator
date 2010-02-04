/*
 * plugin.h
 * This file is part of gtranslator
 *
 * Copyright (C) 2002-2005 Paolo Maggi 
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

/*
 * Modified by the gtranslator Team, 2002-2005. See the AUTHORS file for a 
 * list of people on the gtranslator Team.  
 * See the ChangeLog files for a list of changes. 
 *
 * $Id: plugin.c 6448 2008-08-25 10:28:33Z icq $
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-plugin.h"

G_DEFINE_TYPE (GtrPlugin, gtranslator_plugin, G_TYPE_OBJECT)
     static void
       dummy (GtrPlugin * plugin, GtrWindow * window)
{
  /* Empty */
}

static GtkWidget *
create_configure_dialog (GtrPlugin * plugin)
{
  return NULL;
}

static gboolean
is_configurable (GtrPlugin * plugin)
{
  return (GTR_PLUGIN_GET_CLASS (plugin)->create_configure_dialog !=
	  create_configure_dialog);
}

static void
gtranslator_plugin_class_init (GtrPluginClass * klass)
{
  klass->activate = dummy;
  klass->deactivate = dummy;
  klass->update_ui = dummy;

  klass->create_configure_dialog = create_configure_dialog;
  klass->is_configurable = is_configurable;
}

static void
gtranslator_plugin_init (GtrPlugin * plugin)
{
  /* Empty */
}

/**
 * gtranslator_plugin_activate:
 * @plugin: a #GtrPlugin
 * @window: a #GtrWindow
 * 
 * Activates the plugin.
 */
void
gtranslator_plugin_activate (GtrPlugin * plugin,
			     GtrWindow * window)
{
  g_return_if_fail (GTR_IS_PLUGIN (plugin));
  g_return_if_fail (GTR_IS_WINDOW (window));

  GTR_PLUGIN_GET_CLASS (plugin)->activate (plugin, window);
}

/**
 * gtranslator_plugin_deactivate:
 * @plugin: a #GtrPlugin
 * @window: a #GtrWindow
 * 
 * Deactivates the plugin.
 */
void
gtranslator_plugin_deactivate (GtrPlugin * plugin,
			       GtrWindow * window)
{
  g_return_if_fail (GTR_IS_PLUGIN (plugin));
  g_return_if_fail (GTR_IS_WINDOW (window));

  GTR_PLUGIN_GET_CLASS (plugin)->deactivate (plugin, window);
}

/**
 * gtranslator_plugin_update_ui:
 * @plugin: a #GtrPlugin
 * @window: a #GtrWindow
 *
 * Triggers an update of the user interface to take into account state changes
 * caused by the plugin.
 */
void
gtranslator_plugin_update_ui (GtrPlugin * plugin,
			      GtrWindow * window)
{
  g_return_if_fail (GTR_IS_PLUGIN (plugin));
  g_return_if_fail (GTR_IS_WINDOW (window));

  GTR_PLUGIN_GET_CLASS (plugin)->update_ui (plugin, window);
}

/**
 * gtranslator_plugin_is_configurable:
 * @plugin: a #GtrPlugin
 *
 * Whether the plugin is configurable.
 *
 * Returns: TRUE if the plugin is configurable:
 */
gboolean
gtranslator_plugin_is_configurable (GtrPlugin * plugin)
{
  g_return_val_if_fail (GTR_IS_PLUGIN (plugin), FALSE);

  return GTR_PLUGIN_GET_CLASS (plugin)->is_configurable (plugin);
}

/**
 * gtranslator_plugin_create_configure_dialog:
 * @plugin: a #GtrPlugin
 *
 * Creates the configure dialog widget for the plugin.
 *
 * Returns: the configure dialog widget for the plugin.
 */
GtkWidget *
gtranslator_plugin_create_configure_dialog (GtrPlugin * plugin)
{
  g_return_val_if_fail (GTR_IS_PLUGIN (plugin), NULL);

  return GTR_PLUGIN_GET_CLASS (plugin)->create_configure_dialog (plugin);
}
