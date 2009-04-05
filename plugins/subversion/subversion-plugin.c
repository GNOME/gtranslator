/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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

#include "subversion-plugin.h"
#include "subversion-utils.h"
#include "svn-add-command.h"
#include "window.h"
#include "statusbar.h"
#include "commit-dialog.h"
#include "update-dialog.h"
#include "diff-dialog.h"
#include "checkout-dialog.h"
#include "utils.h"

#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <gconf/gconf-client.h>

#define WINDOW_DATA_KEY "GtranslatorSubversionPluginWindowData"

#define GTR_SUBVERSION_PLUGIN_GET_PRIVATE(object) \
				(G_TYPE_INSTANCE_GET_PRIVATE ((object),	\
				GTR_TYPE_SUBVERSION_PLUGIN,		\
				GtranslatorSubversionPluginPrivate))

GTR_PLUGIN_REGISTER_TYPE(GtranslatorSubversionPlugin, gtranslator_subversion_plugin)

struct _GtranslatorSubversionPluginPrivate
{
	GConfClient *gconf_client;
	
	/* Dialog stuff */
	GtkWidget *dialog;
	
	GtkWidget *main_box;
	GtkWidget *program_name_entry;
	GtkWidget *line_argument_entry;
};

static void
on_update_activated (GtkAction *action,
		     GtranslatorWindow *window)
{
	gtranslator_show_update_dialog (window);
}

static void
on_commit_activated (GtkAction *action,
		     GtranslatorWindow *window)
{
	gtranslator_show_commit_dialog (window);
}

static void
on_add_command_finished (GtranslatorCommand *command,
			 guint return_code, 
			 GtranslatorWindow *window)
{
	GtranslatorStatusbar *statusbar;
	
	statusbar = GTR_STATUSBAR (gtranslator_window_get_statusbar (window));
	
	gtranslator_statusbar_flash_message (statusbar, 0,
					     _("Subversion: File will be added on next "
					       "commit."));

	subversion_utils_report_errors (window, command, return_code);
	
	svn_add_command_destroy (SVN_ADD_COMMAND (command));
}

static void
on_add_activated (GtkAction *action,
		  GtranslatorWindow *window)
{
	GtranslatorTab *tab;
	GtranslatorPo *po;
	SvnAddCommand *add_command;
	GFile *location;
	gchar *path;
	
	tab = gtranslator_window_get_active_tab (window);
	po = gtranslator_tab_get_po (tab);
	
	location = gtranslator_po_get_location (po);
	path = g_file_get_path (location);
	g_object_unref (location);
	
	add_command = svn_add_command_new (path,
					   FALSE,
					   FALSE);
	
	g_free (path);

	g_signal_connect (G_OBJECT (add_command), "command-finished",
			  G_CALLBACK (on_add_command_finished),
			  window);
	
	gtranslator_command_start (GTR_COMMAND (add_command));
}

static void
on_diff_activated (GtkAction *action,
		   GtranslatorWindow *window)
{
	gtranslator_show_diff_dialog (window);
}

static void
on_checkout_activated (GtkAction *action,
		       GtranslatorWindow *window)
{
	gtranslator_show_checkout_dialog (window);
}

static const GtkActionEntry action_entries[] =
{	
	{ "SubversionUpdate", GTK_STOCK_REFRESH, N_("_Update"), NULL,
	 N_("Sync your local copy with the Subversion tree"),
	 G_CALLBACK (on_update_activated)},
	
	{ "SubversionCommit", GTK_STOCK_YES, N_("_Commit"), NULL,
	 N_("Commit your changes to the Subversion tree"),
	 G_CALLBACK (on_commit_activated)},
	
	{ "SubversionAdd", GTK_STOCK_ADD, N_("_Add"), NULL,
	 N_("Add a new file/directory to the Subversion tree"),
	 G_CALLBACK (on_add_activated)},
	
	{ "SubversionDiff", GTK_STOCK_ZOOM_100, N_("_Diff"), NULL,
	 N_("Diff local PO file with repository PO file"),
	 G_CALLBACK (on_diff_activated)},
	
	{ "SubversionCheckout", GTK_STOCK_SAVE, N_("C_heckout"), "",
	 N_("Get a new repository copy"),
	 G_CALLBACK (on_checkout_activated)},
	
	{ "Subversion", NULL, N_("S_ubversion") }
};

static const gchar submenu[] =
"<ui>"
"  <menubar name=\"MainMenu\">"
"    <placeholder name=\"SubversionPlaceholder\">"
"      <menu name=\"SubversionMenu\" action=\"Subversion\">"
"        <menuitem name=\"SubversionUpdateMenu\" action=\"SubversionUpdate\"/>"
"        <menuitem name=\"SubversionCommitMenu\" action=\"SubversionCommit\"/>"
"        <separator />"
"        <menuitem name=\"SubversionAddMenu\" action=\"SubversionAdd\"/>"
"        <separator />"
"        <menuitem name=\"SubversionDiffMenu\" action=\"SubversionDiff\"/>"
"        <separator />"
"        <menuitem name=\"SubversionCheckoutMenu\" action=\"SubversionCheckout\"/>"
"      </menu>"
"    </placeholder>"
"  </menubar>"
"</ui>";

typedef struct
{
	GtkActionGroup *action_group;
	guint           ui_id;
} WindowData;

static void
free_window_data (WindowData *data)
{
	g_return_if_fail (data != NULL);

	g_free (data);
}

static void
update_ui_real (GtranslatorWindow *window,
		WindowData   *data)
{
	GList *tabs;
	GtkAction *action;

	tabs = gtranslator_window_get_all_tabs (window);

	action = gtk_action_group_get_action (data->action_group,
					      "SubversionAdd");
	gtk_action_set_sensitive (action,
				  tabs != NULL);
	
	action = gtk_action_group_get_action (data->action_group,
					      "SubversionUpdate");
	gtk_action_set_sensitive (action,
				  tabs != NULL);
	
	action = gtk_action_group_get_action (data->action_group,
					      "SubversionCommit");
	gtk_action_set_sensitive (action,
				  tabs != NULL);
	
	action = gtk_action_group_get_action (data->action_group,
					      "SubversionDiff");
	gtk_action_set_sensitive (action,
				  tabs != NULL);
}

static void
gtranslator_subversion_plugin_init (GtranslatorSubversionPlugin *plugin)
{
	plugin->priv = GTR_SUBVERSION_PLUGIN_GET_PRIVATE (plugin);

	plugin->priv->gconf_client = gconf_client_get_default ();

	gconf_client_add_dir (plugin->priv->gconf_client,
			      SUBVERSION_BASE_KEY,
			      GCONF_CLIENT_PRELOAD_ONELEVEL,
			      NULL);
	
	apr_initialize ();
}

static void
gtranslator_subversion_plugin_finalize (GObject *object)
{
	GtranslatorSubversionPlugin *plugin = GTR_SUBVERSION_PLUGIN (object);
	
	gconf_client_suggest_sync (plugin->priv->gconf_client, NULL);

	g_object_unref (G_OBJECT (plugin->priv->gconf_client));
	
	apr_terminate ();
	
	G_OBJECT_CLASS (gtranslator_subversion_plugin_parent_class)->finalize (object);
}

static void
impl_activate (GtranslatorPlugin *plugin,
	       GtranslatorWindow *window)
{
	GtkUIManager *manager;
	WindowData *data;
	GError *error = NULL;
	
	g_return_if_fail (GTR_IS_WINDOW (window));

	data = g_new (WindowData, 1);

	manager = gtranslator_window_get_ui_manager (window);

	data->action_group = gtk_action_group_new ("GtranslatorSubversionPluginActions");
	gtk_action_group_set_translation_domain (data->action_group, 
						 GETTEXT_PACKAGE);
	gtk_action_group_add_actions (data->action_group,
				      action_entries,
				      G_N_ELEMENTS (action_entries), 
				      window);

	gtk_ui_manager_insert_action_group (manager, data->action_group, 0);

	g_object_set_data_full (G_OBJECT (window), 
				WINDOW_DATA_KEY, 
				data,
				(GDestroyNotify) free_window_data);
	
	data->ui_id = gtk_ui_manager_add_ui_from_string (manager,
							 submenu,
							 -1,
							 &error);
	
	if (data->ui_id == 0)
	{
		g_warning (error->message);
		g_error_free (error);
		return;
	}

	gtk_ui_manager_ensure_update (manager);
	update_ui_real (window, data);
}

static void
impl_deactivate (GtranslatorPlugin *plugin,
		 GtranslatorWindow *window)
{
	GtkUIManager *manager;
	WindowData *data;
	
	manager = gtranslator_window_get_ui_manager (window);

	data = (WindowData *) g_object_get_data (G_OBJECT (window), WINDOW_DATA_KEY);
	g_return_if_fail (data != NULL);

	gtk_ui_manager_remove_ui (manager, data->ui_id);
	gtk_ui_manager_remove_action_group (manager, data->action_group);

	g_object_set_data (G_OBJECT (window), WINDOW_DATA_KEY, NULL);	
}

static void
impl_update_ui (GtranslatorPlugin *plugin,
		GtranslatorWindow *window)
{
	WindowData *data;

	data = (WindowData *) g_object_get_data (G_OBJECT (window), WINDOW_DATA_KEY);
	g_return_if_fail (data != NULL);

	update_ui_real (window, data);
}

static void
get_program (GtranslatorSubversionPlugin *plugin)
{
	gchar *data;
	
	/* Program name */
	data = gconf_client_get_string (plugin->priv->gconf_client,
					PROGRAM_NAME_KEY,
					NULL);
	
	if (!data)
		data = g_strdup ("meld");
	
	gtk_entry_set_text (GTK_ENTRY (plugin->priv->program_name_entry), data);
	
	g_free (data);
	
	/* Line argument */
	data = gconf_client_get_string (plugin->priv->gconf_client,
					LINE_ARGUMENT_KEY,
					NULL);
	
	if (!data)
		data = g_strdup ("");
	
	gtk_entry_set_text (GTK_ENTRY (plugin->priv->line_argument_entry), data);
	
	g_free (data);
}

static GtkWidget *
get_configuration_dialog (GtranslatorSubversionPlugin *plugin)
{

	gboolean ret;
	GtkWidget *error_widget;
	gchar *root_objects [] = {
	  "settings_dialog",
	  NULL
	};
	
	ret = gtranslator_utils_get_ui_objects (UI_FILE,
						root_objects,
						&error_widget,
						"settings_dialog", &plugin->priv->dialog,
						"main_box", &plugin->priv->main_box,
						"program_name", &plugin->priv->program_name_entry,
						"line_argument", &plugin->priv->line_argument_entry,
						NULL);

	if(!ret)
	{
		gtk_widget_show (error_widget);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (plugin->priv->dialog)->vbox),
				    error_widget,
				    TRUE,
				    TRUE,
				    0);
		
		return plugin->priv->dialog;
	}

	get_program (plugin);
	
	return plugin->priv->dialog;
}

static void
ok_button_pressed (GtranslatorSubversionPlugin *plugin)
{
	const gchar *program_name;
	const gchar *line_argument;
	
	/* We have to get the text from the entries */
	program_name = gtk_entry_get_text (GTK_ENTRY (plugin->priv->program_name_entry));
	line_argument = gtk_entry_get_text (GTK_ENTRY (plugin->priv->line_argument_entry));
	
	/* Now we store the data in gconf */
	if (!gconf_client_key_is_writable (plugin->priv->gconf_client,
					   PROGRAM_NAME_KEY,
					   NULL))
		return;

	gconf_client_set_string (plugin->priv->gconf_client,
				 PROGRAM_NAME_KEY,
		       		 program_name,
		       		 NULL);
	
	if (!gconf_client_key_is_writable (plugin->priv->gconf_client,
					   LINE_ARGUMENT_KEY,
					   NULL))
		return;

	gconf_client_set_string (plugin->priv->gconf_client,
				 LINE_ARGUMENT_KEY,
		       		 line_argument,
		       		 NULL);
}

static void
configure_dialog_response_cb (GtkWidget           *widget,
			      gint                 response,
			      GtranslatorSubversionPlugin *plugin)
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
impl_create_configure_dialog (GtranslatorPlugin *plugin)
{
	GtkWidget *dialog;
	
	dialog = get_configuration_dialog (GTR_SUBVERSION_PLUGIN (plugin));
	
	g_signal_connect (dialog,
			  "response",
			  G_CALLBACK (configure_dialog_response_cb),
			  GTR_SUBVERSION_PLUGIN (plugin));
	g_signal_connect (dialog,
			  "destroy",
			  G_CALLBACK (gtk_widget_destroy),
			  &dialog);
	
	return dialog;
}

static void
gtranslator_subversion_plugin_class_init (GtranslatorSubversionPluginClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtranslatorPluginClass *plugin_class = GTR_PLUGIN_CLASS (klass);

	object_class->finalize = gtranslator_subversion_plugin_finalize;

	plugin_class->activate = impl_activate;
	plugin_class->deactivate = impl_deactivate;
	plugin_class->update_ui = impl_update_ui;
	plugin_class->create_configure_dialog = impl_create_configure_dialog;
	
	g_type_class_add_private (object_class, sizeof (GtranslatorSubversionPluginPrivate));
}
