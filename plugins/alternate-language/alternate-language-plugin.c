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

#include "alternate-language-plugin.h"
#include "alternate-language-panel.h"
#include "notebook.h"
#include "window.h"

#include <glib/gi18n-lib.h>

#define WINDOW_DATA_KEY	"GtranslatorAlternateLangPluginWindowData"

#define GTR_MESSAGE_TABLE_GET_PRIVATE(object) \
				(G_TYPE_INSTANCE_GET_PRIVATE ((object),	\
				GTR_TYPE_MESSAGE_TABLE,		\
				GtranslatorAlternateLangPluginPrivate))

GTR_PLUGIN_REGISTER_TYPE_WITH_CODE (GtranslatorAlternateLangPlugin, gtranslator_alternate_lang_plugin,
		gtranslator_alternate_lang_panel_register_type (module);
)


static void
gtranslator_alternate_lang_plugin_init (GtranslatorAlternateLangPlugin *message_table)
{
}

static void
gtranslator_alternate_lang_plugin_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_alternate_lang_plugin_parent_class)->finalize (object);
}


static void
create_alternate_lang_plugin_panel (GtkNotebook *notebook,
			     GtkWidget   *child,
			     guint        page_num,
			     GtranslatorWindow *window)
{
	GtkWidget *alternatelang;
	GtranslatorPo *po;
	
	po = gtranslator_tab_get_po (GTR_TAB (child));
	
	g_return_if_fail (po != NULL);
	
	alternatelang = gtranslator_alternate_lang_panel_new (child);
	gtk_widget_show (alternatelang);
	
	gtranslator_tab_add_widget_to_lateral_panel (GTR_TAB(child),
						     alternatelang,
						     _("Alternate Language"));
	
	g_object_set_data (G_OBJECT (child),
			   WINDOW_DATA_KEY,
			   alternatelang);
}

static void
impl_activate (GtranslatorPlugin *plugin,
	       GtranslatorWindow *window)
{
	GtranslatorNotebook *notebook;
	GList *tabs = NULL;
	
	notebook = gtranslator_window_get_notebook(window);
	
	g_signal_connect(GTK_NOTEBOOK(notebook),
			 "page-added",
			 G_CALLBACK(create_alternate_lang_plugin_panel), window);
			 
	tabs = gtranslator_window_get_all_tabs(window);
	
	if(tabs == NULL)
		return;
	do{
		create_alternate_lang_plugin_panel(GTK_NOTEBOOK(notebook),
						   tabs->data,
						   0, window);
	}while((tabs = g_list_next(tabs)));

}

static void
impl_deactivate(GtranslatorPlugin *plugin,
	        GtranslatorWindow *window)
{
	GtkWidget *panel;
	GtranslatorNotebook *notebook;
	GtkWidget *alternatelang;
	GList *tabs;
	gint page_num;
	
	tabs = gtranslator_window_get_all_tabs(window);
	notebook = gtranslator_window_get_notebook(window);

	if (tabs != NULL)
	{
		do{
			alternatelang = g_object_get_data(G_OBJECT(tabs->data), WINDOW_DATA_KEY);
			panel = gtranslator_tab_get_panel (GTR_TAB(tabs->data));
		
			page_num = gtk_notebook_page_num (GTK_NOTEBOOK (panel),
							  alternatelang);
			gtk_notebook_remove_page (GTK_NOTEBOOK (panel),
						  page_num);

			g_object_set_data (G_OBJECT (tabs->data), WINDOW_DATA_KEY, NULL);
		
		}while((tabs = g_list_next(tabs)));
	}
	
	g_signal_handlers_disconnect_by_func(notebook,
					     create_alternate_lang_plugin_panel,
					     window);
}

static void
gtranslator_alternate_lang_plugin_class_init (GtranslatorAlternateLangPluginClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtranslatorPluginClass *plugin_class = GTR_PLUGIN_CLASS (klass);

	object_class->finalize = gtranslator_alternate_lang_plugin_finalize;

	plugin_class->activate = impl_activate;
	plugin_class->deactivate = impl_deactivate;
}
