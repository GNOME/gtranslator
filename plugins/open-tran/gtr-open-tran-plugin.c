/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANPOILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-open-tran-plugin.h"
#include "gtr-open-tran-panel.h"
#include "gtr-application.h"
#include "gtr-dirs.h"
#include "gtr-window.h"
#include "gtr-utils.h"

#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <libxml/nanohttp.h>

#define OPEN_TRAN_PLUGIN_ICON "open-tran.png"
#define WINDOW_DATA_KEY "GtrOpenTranPluginWindowData"

#define GTR_OPEN_TRAN_PLUGIN_GET_PRIVATE(object) \
				(G_TYPE_INSTANCE_GET_PRIVATE ((object),	\
				GTR_TYPE_OPEN_TRAN_PLUGIN,		\
				GtrOpenTranPluginPrivate))

struct _GtrOpenTranPluginPrivate
{
  GSettings *settings;

  /* Dialog stuff */
  GtkWidget *dialog;

  GtkWidget *main_box;
  GtkWidget *search_code_entry;
  GtkWidget *own_code_entry;
};

typedef struct
{
  GtkWidget *panel;
  guint context_id;
} WindowData;

GTR_PLUGIN_REGISTER_TYPE_WITH_CODE (GtrOpenTranPlugin,
                                    gtr_open_tran_plugin,
                                    gtr_open_tran_panel_register_type
                                    (module);
  )

static void
gtr_open_tran_plugin_init (GtrOpenTranPlugin * plugin)
{
  plugin->priv = GTR_OPEN_TRAN_PLUGIN_GET_PRIVATE (plugin);

  plugin->priv->settings = g_settings_new ("org.gnome.gtranslator.plugins.open-tran");

  xmlNanoHTTPInit ();
}

static void
gtr_open_tran_plugin_dispose (GObject * object)
{
  GtrOpenTranPlugin *plugin = GTR_OPEN_TRAN_PLUGIN (object);

  xmlNanoHTTPCleanup ();

  if (plugin->priv->settings)
    {
      g_object_unref (plugin->priv->settings);
      plugin->priv->settings = NULL;
    }

  G_OBJECT_CLASS (gtr_open_tran_plugin_parent_class)->dispose (object);
}


static void
impl_activate (GtrPlugin * plugin, GtrWindow * window)
{
  GtkWidget *opentran;

  gtr_application_register_icon (GTR_APP, "open-tran.png",
                                 "open-tran-plugin-icon");

  opentran = gtr_open_tran_panel_new (window);
  gtk_widget_show (opentran);

  gtr_window_add_widget (window,
                         opentran,
                         "GtrOpenTranPlugin",
                         _("Open Tran"),
                         "open-tran-plugin-icon", GTR_WINDOW_PLACEMENT_LEFT);

  g_object_set_data (G_OBJECT (window), WINDOW_DATA_KEY, opentran);
}

static void
impl_deactivate (GtrPlugin * plugin, GtrWindow * window)
{
  GtkWidget *opentran;

  opentran = (GtkWidget *) g_object_get_data (G_OBJECT (window),
                                              WINDOW_DATA_KEY);
  g_return_if_fail (opentran != NULL);

  gtr_window_remove_widget (window, opentran);

  g_object_set_data (G_OBJECT (window), WINDOW_DATA_KEY, NULL);
}

static GtkWidget *
get_configuration_dialog (GtrOpenTranPlugin * plugin)
{

  gboolean ret;
  GtkWidget *error_widget;
  gchar *path;
  gchar *root_objects[] = {
    "dialog",
    NULL
  };

  path = gtr_dirs_get_ui_file ("gtr-open-tran-dialog.ui");
  ret = gtr_utils_get_ui_objects (path,
                                  root_objects,
                                  &error_widget,
                                  "dialog", &plugin->priv->dialog,
                                  "main_box", &plugin->priv->main_box,
                                  "search_code",
                                  &plugin->priv->search_code_entry,
                                  "own_code",
                                  &plugin->priv->own_code_entry, NULL);
  if (!ret)
    {
      g_error (_("Error from configuration dialog %s"), path);
    }

  g_free (path);

  g_settings_bind (plugin->priv->settings,
                   GTR_SETTINGS_OWN_CODE,
                   plugin->priv->own_code_entry,
                   "text",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);

  g_settings_bind (plugin->priv->settings,
                   GTR_SETTINGS_SEARCH_CODE,
                   plugin->priv->search_code_entry,
                   "text",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);

  return plugin->priv->dialog;
}

static void
configure_dialog_response_cb (GtkWidget * widget,
                              gint response, GtrOpenTranPlugin * plugin)
{
  gtk_widget_destroy (plugin->priv->dialog);
}

static GtkWidget *
impl_create_configure_dialog (GtrPlugin * plugin)
{
  GtkWidget *dialog;

  dialog = get_configuration_dialog (GTR_OPEN_TRAN_PLUGIN (plugin));

  g_signal_connect (dialog,
                    "response",
                    G_CALLBACK (configure_dialog_response_cb),
                    GTR_OPEN_TRAN_PLUGIN (plugin));
  g_signal_connect (dialog,
                    "destroy", G_CALLBACK (gtk_widget_destroy), &dialog);

  return dialog;
}

static void
gtr_open_tran_plugin_class_init (GtrOpenTranPluginClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtrPluginClass *plugin_class = GTR_PLUGIN_CLASS (klass);

  object_class->dispose = gtr_open_tran_plugin_dispose;

  plugin_class->activate = impl_activate;
  plugin_class->deactivate = impl_deactivate;
  plugin_class->create_configure_dialog = impl_create_configure_dialog;

  g_type_class_add_private (object_class, sizeof (GtrOpenTranPluginPrivate));
}
