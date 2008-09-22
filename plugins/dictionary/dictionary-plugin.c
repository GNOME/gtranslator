/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dictionary-plugin.h"
#include "dict-panel.h"
#include "window.h"

#include <glib/gi18n-lib.h>
#include <gconf/gconf-client.h>
//#include <gtranslator/gtranslator-debug.h>

#define WINDOW_DATA_KEY	"GtranslatorDictPluginWindowData"
#define PANEL_KEY "/apps/gtranslator/plugins/dictionary"

#define GTR_DICT_PLUGIN_GET_PRIVATE(object) \
				(G_TYPE_INSTANCE_GET_PRIVATE ((object),	\
				GTR_TYPE_DICT_PLUGIN,		\
				GtranslatorDictPluginPrivate))

typedef struct
{
	GtkWidget	*panel;
	guint		 context_id;
} WindowData;

GTR_PLUGIN_REGISTER_TYPE_WITH_CODE (GtranslatorDictPlugin, gtranslator_dict_plugin,
		gtranslator_dict_panel_register_type (module);
)

static void
gtranslator_dict_plugin_init (GtranslatorDictPlugin *plugin)
{
	//gtranslator_debug_message (DEBUG_PLUGINS, "GtranslatorDictPlugin initializing");
}

static void
gtranslator_dict_plugin_finalize (GObject *object)
{
	//gtranslator_debug_message (DEBUG_PLUGINS, "GtranslatorDictPlugin finalizing");

	G_OBJECT_CLASS (gtranslator_dict_plugin_parent_class)->finalize (object);
}

static void
free_window_data (WindowData *data)
{
	g_return_if_fail (data != NULL);
	
	g_free (data);
}


static GtkWidget *
create_dict_panel (GtranslatorWindow *window)
{
	GtkWidget      *panel;

	panel = gtranslator_dict_panel_new ();

	gtk_widget_show_all (panel);

	return panel;
}

static void
restore_position(GtranslatorDictPanel *panel)
{
	GConfClient *client;
	gint position;
	
	client = gconf_client_get_default();
	position = gconf_client_get_int(client, PANEL_KEY "/panel_position", NULL);
	gtranslator_dict_panel_set_position(panel, position);
	
	g_object_unref(client);
}

static void
impl_activate (GtranslatorPlugin *plugin,
	       GtranslatorWindow *window)
{
	/*GtkWidget *image;
	GtkIconTheme *theme;*/
	WindowData *data;

	//gtranslator_debug (DEBUG_PLUGINS);

	data = g_new (WindowData, 1);

	/*theme = gtk_icon_theme_get_default ();
	
	if (gtk_icon_theme_has_icon (theme, "accessories-dictionary"))
		image = gtk_image_new_from_icon_name ("accessories-dictionary",
						      GTK_ICON_SIZE_MENU);
	else
		image = gtk_image_new_from_icon_name ("gdict",
						      GTK_ICON_SIZE_MENU);*/

	data->panel = create_dict_panel (window);
	
	restore_position(GTR_DICT_PANEL(data->panel));
	
	gtranslator_window_add_widget (window,
				       data->panel,
				       "GtranslatorDictionaryPlugin",
				       _("Dictionary"),
				       NULL,
				       GTR_WINDOW_PLACEMENT_LEFT);

	//gtk_object_sink (GTK_OBJECT (image));

	g_object_set_data_full (G_OBJECT (window),
				WINDOW_DATA_KEY,
				data,
				(GDestroyNotify) free_window_data);
}

static void
impl_deactivate	(GtranslatorPlugin *plugin,
		 GtranslatorWindow *window)
{
	WindowData *data;

	data = (WindowData *) g_object_get_data (G_OBJECT (window),
						 WINDOW_DATA_KEY);
	g_return_if_fail (data != NULL);

	gtranslator_window_remove_widget (window, data->panel);

	g_object_set_data (G_OBJECT (window), WINDOW_DATA_KEY, NULL);
}

static void
gtranslator_dict_plugin_class_init (GtranslatorDictPluginClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtranslatorPluginClass *plugin_class = GTR_PLUGIN_CLASS (klass);

	object_class->finalize = gtranslator_dict_plugin_finalize;

	plugin_class->activate = impl_activate;
	plugin_class->deactivate = impl_deactivate;
}
