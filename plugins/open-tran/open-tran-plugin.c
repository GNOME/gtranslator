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

#include "open-tran-plugin.h"
#include "open-tran-panel.h"

#include <glib/gi18n-lib.h>
#include <gconf/gconf-client.h>
#include <gtk/gtk.h>

#include "application.h"
#include "window.h"
#include "utils.h"

#define OPEN_TRAN_PLUGIN_ICON "open-tran.png"
#define WINDOW_DATA_KEY	"GtranslatorOpenTranPluginWindowData"
#define UI_FILE DATADIR"/open-tran-dialog.ui"

#define GTR_OPEN_TRAN_PLUGIN_GET_PRIVATE(object) \
				(G_TYPE_INSTANCE_GET_PRIVATE ((object),	\
				GTR_TYPE_OPEN_TRAN_PLUGIN,		\
				GtranslatorOpenTranPluginPrivate))

struct _GtranslatorOpenTranPluginPrivate
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

GTR_PLUGIN_REGISTER_TYPE_WITH_CODE (GtranslatorOpenTranPlugin,
				    gtranslator_open_tran_plugin,
				    gtranslator_open_tran_panel_register_type
				    (module);)
     static void gtranslator_open_tran_plugin_init (GtranslatorOpenTranPlugin
						    * plugin)
{
  plugin->priv = GTR_OPEN_TRAN_PLUGIN_GET_PRIVATE (plugin);

  plugin->priv->gconf_client = gconf_client_get_default ();

  gconf_client_add_dir (plugin->priv->gconf_client,
			OPEN_TRAN_BASE_KEY,
			GCONF_CLIENT_PRELOAD_ONELEVEL, NULL);
}

static void
gtranslator_open_tran_plugin_finalize (GObject * object)
{
  GtranslatorOpenTranPlugin *plugin = GTR_OPEN_TRAN_PLUGIN (object);

  gconf_client_suggest_sync (plugin->priv->gconf_client, NULL);

  g_object_unref (G_OBJECT (plugin->priv->gconf_client));

  G_OBJECT_CLASS (gtranslator_open_tran_plugin_parent_class)->
    finalize (object);
}


static void
impl_activate (GtranslatorPlugin * plugin, GtranslatorWindow * window)
{
  GtkWidget *opentran;

  gtranslator_application_register_icon (GTR_APP, "open-tran.png",
					 "open-tran-plugin-icon");

  opentran = gtranslator_open_tran_panel_new (window);
  gtk_widget_show (opentran);

  gtranslator_window_add_widget (window,
				 opentran,
				 "GtranslatorOpenTranPlugin",
				 _("Open Tran"),
				 "open-tran-plugin-icon",
				 GTR_WINDOW_PLACEMENT_LEFT);

  g_object_set_data (G_OBJECT (window), WINDOW_DATA_KEY, opentran);
}

static void
impl_deactivate (GtranslatorPlugin * plugin, GtranslatorWindow * window)
{
  GtkWidget *opentran;

  opentran = (GtkWidget *) g_object_get_data (G_OBJECT (window),
					      WINDOW_DATA_KEY);
  g_return_if_fail (opentran != NULL);

  gtranslator_window_remove_widget (window, opentran);

  g_object_set_data (G_OBJECT (window), WINDOW_DATA_KEY, NULL);
}

static void
get_custom_code (GtranslatorOpenTranPlugin * plugin, gboolean own_code)
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
    code = g_strdup ("gl");	//Why gl? Just because i want.

  if (!own_code)
    gtk_entry_set_text (GTK_ENTRY (plugin->priv->search_code_entry), code);
  else
    gtk_entry_set_text (GTK_ENTRY (plugin->priv->own_code_entry), code);

  g_free (code);
}

static GtkWidget *
get_configuration_dialog (GtranslatorOpenTranPlugin * plugin)
{

  gboolean ret;
  GtkWidget *error_widget;
  gchar *root_objects[] = {
    "dialog",
    NULL
  };

  ret = gtranslator_utils_get_ui_objects (UI_FILE,
					  root_objects,
					  &error_widget,
					  "dialog", &plugin->priv->dialog,
					  "main_box", &plugin->priv->main_box,
					  "search_code",
					  &plugin->priv->search_code_entry,
					  "own_code",
					  &plugin->priv->own_code_entry,
					  NULL);

  if (!ret)
    {
      //FIXME: We have to show a dialog
    }

  get_custom_code (plugin, FALSE);
  get_custom_code (plugin, TRUE);

  return plugin->priv->dialog;
}

static void
ok_button_pressed (GtranslatorOpenTranPlugin * plugin)
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
			      gint response,
			      GtranslatorOpenTranPlugin * plugin)
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
impl_create_configure_dialog (GtranslatorPlugin * plugin)
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
gtranslator_open_tran_plugin_class_init (GtranslatorOpenTranPluginClass *
					 klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtranslatorPluginClass *plugin_class = GTR_PLUGIN_CLASS (klass);

  object_class->finalize = gtranslator_open_tran_plugin_finalize;

  plugin_class->activate = impl_activate;
  plugin_class->deactivate = impl_deactivate;
  plugin_class->create_configure_dialog = impl_create_configure_dialog;

  g_type_class_add_private (object_class,
			    sizeof (GtranslatorOpenTranPluginPrivate));
}
