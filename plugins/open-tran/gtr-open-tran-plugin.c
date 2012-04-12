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
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
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
#include "gtr-tab.h"
#include "gtr-tab-activatable.h"
#include "gtr-utils.h"

#include <libpeas-gtk/peas-gtk-configurable.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <libxml/nanohttp.h>

#define OPEN_TRAN_PLUGIN_ICON "open-tran.png"

#define GTR_OPEN_TRAN_PLUGIN_GET_PRIVATE(object) \
				(G_TYPE_INSTANCE_GET_PRIVATE ((object),	\
				GTR_TYPE_OPEN_TRAN_PLUGIN,		\
				GtrOpenTranPluginPrivate))

struct _GtrOpenTranPluginPrivate
{
  GSettings *settings;

  GtrTab *tab;
  GtkWidget *opentran;
};

typedef struct _GtrOpenTranConfigureDialog
{
  GSettings *settings;

  GtkWidget *main_box;
  GtkWidget *search_code_entry;
  GtkWidget *own_code_entry;
  GtkWidget *use_mirror_server_entry;
  GtkWidget *mirror_server_url_entry;
  GtkWidget *mirror_server_frame_entry;
} GtrOpenTranConfigureDialog;

enum
{
  PROP_0,
  PROP_TAB
};

static void gtr_tab_activatable_iface_init (GtrTabActivatableInterface *iface);
static void peas_gtk_configurable_iface_init (PeasGtkConfigurableInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED (GtrOpenTranPlugin,
                                gtr_open_tran_plugin,
                                PEAS_TYPE_EXTENSION_BASE,
                                0,
                                G_IMPLEMENT_INTERFACE_DYNAMIC (GTR_TYPE_TAB_ACTIVATABLE,
                                                               gtr_tab_activatable_iface_init)
                                G_IMPLEMENT_INTERFACE_DYNAMIC (PEAS_GTK_TYPE_CONFIGURABLE,
                                                               peas_gtk_configurable_iface_init) \
                                                                                                 \
                                _gtr_open_tran_panel_register_type (type_module)                 \
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
  GtrOpenTranPluginPrivate *priv = GTR_OPEN_TRAN_PLUGIN (object)->priv;

  xmlNanoHTTPCleanup ();

  if (priv->settings)
    {
      g_object_unref (priv->settings);
      priv->settings = NULL;
    }

  if (priv->tab != NULL)
    {
      g_object_unref (priv->tab);
      priv->tab = NULL;
    }

  G_OBJECT_CLASS (gtr_open_tran_plugin_parent_class)->dispose (object);
}

static void
gtr_open_tran_plugin_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  GtrOpenTranPluginPrivate *priv = GTR_OPEN_TRAN_PLUGIN (object)->priv;

  switch (prop_id)
    {
      case PROP_TAB:
        priv->tab = GTR_TAB (g_value_dup_object (value));
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
gtr_open_tran_plugin_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  GtrOpenTranPluginPrivate *priv = GTR_OPEN_TRAN_PLUGIN (object)->priv;

  switch (prop_id)
    {
      case PROP_TAB:
        g_value_set_object (value, priv->tab);
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
gtr_open_tran_plugin_activate (GtrTabActivatable *activatable)
{
  GtrOpenTranPluginPrivate *priv = GTR_OPEN_TRAN_PLUGIN (activatable)->priv;

  gtr_application_register_icon (GTR_APP, "open-tran.png",
                                 "open-tran-plugin-icon");

  priv->opentran = gtr_open_tran_panel_new (priv->tab);
  gtk_widget_show_all (priv->opentran);

  gtr_tab_add_widget (priv->tab,
                      priv->opentran,
                      "GtrOpenTranPlugin",
                      _("Open Tran"),
                      "open-tran-plugin-icon",
                      GTR_TAB_PLACEMENT_LEFT);
}

static void
gtr_open_tran_plugin_deactivate (GtrTabActivatable *activatable)
{
  GtrOpenTranPluginPrivate *priv = GTR_OPEN_TRAN_PLUGIN (activatable)->priv;

  gtr_tab_remove_widget (priv->tab, priv->opentran);
}

static void
use_mirror_server_toggled_cb (GtkWidget                  *widget,
                              GtrOpenTranConfigureDialog *dlg)
{
  gtk_widget_set_sensitive (dlg->mirror_server_frame_entry,
                            gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)));
}

static void
configure_widget_destroyed (GtkWidget *widget,
                            gpointer   data)
{
  GtrOpenTranConfigureDialog *dlg = (GtrOpenTranConfigureDialog *)data;

  g_object_unref (dlg->settings);
  g_slice_free (GtrOpenTranConfigureDialog, data);
}

static GtkWidget *
get_configuration_dialog (GtrOpenTranPlugin * plugin)
{
  GtrOpenTranPluginPrivate *priv = plugin->priv;
  GtrOpenTranConfigureDialog *dlg;
  GtkBuilder *builder;
  gchar *root_objects[] = {
    "main_box",
    NULL
  };

  dlg = g_slice_new (GtrOpenTranConfigureDialog);
  dlg->settings = g_object_ref (priv->settings);

  builder = gtk_builder_new ();
  gtk_builder_add_objects_from_resource (builder, "/org/gnome/gtranslator/plugins/open-tran/ui/gtr-open-tran-dialog.ui",
  root_objects, NULL);
  dlg->main_box = GTK_WIDGET (gtk_builder_get_object (builder, "main_box"));
  g_object_ref (dlg->main_box);
  dlg->search_code_entry = GTK_WIDGET (gtk_builder_get_object (builder, "search_code"));
  dlg->own_code_entry = GTK_WIDGET (gtk_builder_get_object (builder, "own_code"));
  dlg->use_mirror_server_entry = GTK_WIDGET (gtk_builder_get_object (builder, "use_mirror_server"));
  dlg->mirror_server_url_entry = GTK_WIDGET (gtk_builder_get_object (builder, "mirror_server_url"));
  dlg->mirror_server_frame_entry = GTK_WIDGET (gtk_builder_get_object (builder, "mirror_server_frame"));
  g_object_unref (builder);

  g_settings_bind (dlg->settings,
                   GTR_SETTINGS_OWN_CODE,
                   dlg->own_code_entry,
                   "text",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);

  g_settings_bind (dlg->settings,
                   GTR_SETTINGS_SEARCH_CODE,
                   dlg->search_code_entry,
                   "text",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);

  g_settings_bind (dlg->settings,
                   GTR_SETTINGS_USE_MIRROR_SERVER,
                   dlg->use_mirror_server_entry,
                   "active",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);

  g_settings_bind (dlg->settings,
                   GTR_SETTINGS_MIRROR_SERVER_URL,
                   dlg->mirror_server_url_entry,
                   "text",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);

  /* Set the sensitivity of the mirror server area */
  gtk_widget_set_sensitive (dlg->mirror_server_frame_entry,
                            gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dlg->use_mirror_server_entry)));

  g_signal_connect (dlg->main_box,
                    "destroy",
                    G_CALLBACK (configure_widget_destroyed),
                    dlg);
  g_signal_connect (dlg->use_mirror_server_entry,
                    "toggled",
                    G_CALLBACK (use_mirror_server_toggled_cb),
                    dlg);

  return dlg->main_box;
}

static GtkWidget *
gtr_open_tran_create_configure_widget (PeasGtkConfigurable *configurable)
{
  return get_configuration_dialog (GTR_OPEN_TRAN_PLUGIN (configurable));
}

static void
gtr_open_tran_plugin_class_init (GtrOpenTranPluginClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gtr_open_tran_plugin_dispose;
  object_class->set_property = gtr_open_tran_plugin_set_property;
  object_class->get_property = gtr_open_tran_plugin_get_property;

  g_object_class_override_property (object_class, PROP_TAB, "tab");

  g_type_class_add_private (object_class, sizeof (GtrOpenTranPluginPrivate));
}

static void
gtr_open_tran_plugin_class_finalize (GtrOpenTranPluginClass *klass)
{
}

static void
peas_gtk_configurable_iface_init (PeasGtkConfigurableInterface *iface)
{
  iface->create_configure_widget = gtr_open_tran_create_configure_widget;
}

static void
gtr_tab_activatable_iface_init (GtrTabActivatableInterface *iface)
{
  iface->activate = gtr_open_tran_plugin_activate;
  iface->deactivate = gtr_open_tran_plugin_deactivate;
}

G_MODULE_EXPORT void
peas_register_types (PeasObjectModule *module)
{
  gtr_open_tran_plugin_register_type (G_TYPE_MODULE (module));

  peas_object_module_register_extension_type (module,
                                              GTR_TYPE_TAB_ACTIVATABLE,
                                              GTR_TYPE_OPEN_TRAN_PLUGIN);
  peas_object_module_register_extension_type (module,
                                              PEAS_GTK_TYPE_CONFIGURABLE,
                                              GTR_TYPE_OPEN_TRAN_PLUGIN);
}
