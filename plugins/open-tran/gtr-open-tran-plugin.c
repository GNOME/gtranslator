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
#include <gconf/gconf-client.h>
#include <gtk/gtk.h>

#define OPEN_TRAN_PLUGIN_ICON "open-tran.png"
#define WINDOW_DATA_KEY	"GtrOpenTranPluginWindowData"

#define GTR_OPEN_TRAN_PLUGIN_GET_PRIVATE(object) \
				(G_TYPE_INSTANCE_GET_PRIVATE ((object),	\
				GTR_TYPE_OPEN_TRAN_PLUGIN,		\
				GtrOpenTranPluginPrivate))

struct _GtrOpenTranPluginPrivate
{
  GConfClient *gconf_client;

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
     static void gtr_open_tran_plugin_init (GtrOpenTranPlugin * plugin)
{
  plugin->priv = GTR_OPEN_TRAN_PLUGIN_GET_PRIVATE (plugin);

  plugin->priv->gconf_client = gconf_client_get_default ();

  gconf_client_add_dir (plugin->priv->gconf_client,
                        OPEN_TRAN_BASE_KEY,
                        GCONF_CLIENT_PRELOAD_ONELEVEL, NULL);
}

static void
gtr_open_tran_plugin_finalize (GObject * object)
{
  GtrOpenTranPlugin *plugin = GTR_OPEN_TRAN_PLUGIN (object);

  gconf_client_suggest_sync (plugin->priv->gconf_client, NULL);

  g_object_unref (G_OBJECT (plugin->priv->gconf_client));

  G_OBJECT_CLASS (gtr_open_tran_plugin_parent_class)->finalize (object);
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

static void
get_custom_code (GtrOpenTranPlugin * plugin, gboolean own_code)
{
  gchar *type;
  gchar *code;

  if (own_code)
    type = g_strdup (OWN_CODE_KEY);
  else
    type = g_strdup (SEARCH_CODE_KEY);

  code = gconf_client_get_string (plugin->priv->gconf_client, type, NULL);

  g_free (type);

  if (!code && !own_code)
    code = g_strdup ("en");
  else if (!code && own_code)
    code = g_strdup ("gl");     //Why gl? Just because i want.

  if (!own_code)
    gtk_entry_set_text (GTK_ENTRY (plugin->priv->search_code_entry), code);
  else
    gtk_entry_set_text (GTK_ENTRY (plugin->priv->own_code_entry), code);

  g_free (code);
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
  g_free (path);

  if (!ret)
    {
      //FIXME: We have to show a dialog
    }

  get_custom_code (plugin, FALSE);
  get_custom_code (plugin, TRUE);

  return plugin->priv->dialog;
}

static void
ok_button_pressed (GtrOpenTranPlugin * plugin)
{
  const gchar *search_code;
  const gchar *own_code;

  /* We have to get the text from the entries */
  search_code =
    gtk_entry_get_text (GTK_ENTRY (plugin->priv->search_code_entry));
  own_code = gtk_entry_get_text (GTK_ENTRY (plugin->priv->own_code_entry));

  /* Now we store the data in gconf */
  if (!gconf_client_key_is_writable (plugin->priv->gconf_client,
                                     SEARCH_CODE_KEY, NULL))
    return;

  gconf_client_set_string (plugin->priv->gconf_client,
                           SEARCH_CODE_KEY, search_code, NULL);

  if (!gconf_client_key_is_writable (plugin->priv->gconf_client,
                                     OWN_CODE_KEY, NULL))
    return;

  gconf_client_set_string (plugin->priv->gconf_client,
                           OWN_CODE_KEY, own_code, NULL);
}

static void
configure_dialog_response_cb (GtkWidget * widget,
                              gint response, GtrOpenTranPlugin * plugin)
{
  switch (response)
    {
    case GTK_RESPONSE_OK:
      {
        ok_button_pressed (plugin);

        gtk_widget_destroy (plugin->priv->dialog);
        break;
      }
    case GTK_RESPONSE_CANCEL:
      {
        gtk_widget_destroy (plugin->priv->dialog);
      }
    }
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

  object_class->finalize = gtr_open_tran_plugin_finalize;

  plugin_class->activate = impl_activate;
  plugin_class->deactivate = impl_deactivate;
  plugin_class->create_configure_dialog = impl_create_configure_dialog;

  g_type_class_add_private (object_class, sizeof (GtrOpenTranPluginPrivate));
}
