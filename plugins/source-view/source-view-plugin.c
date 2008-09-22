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

#include "source-view-plugin.h"
#include "comment.h"
#include "utils.h"
#include "viewer.h"
#include "window.h"

#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <gio/gio.h>
#include <gconf/gconf-client.h>

/* Gconf keys */
#define SOURCE_VIEW_BASE_KEY "/apps/gtranslator/plugins/source-view"
#define USE_EDITOR_KEY SOURCE_VIEW_BASE_KEY "/use_editor"
#define PROGRAM_CMD_KEY SOURCE_VIEW_BASE_KEY "/program_cmd"
#define LINE_CMD_KEY SOURCE_VIEW_BASE_KEY "/line_cmd"

/* Glade */
#define GLADE_FILE DATADIR"/source-view-dialog.glade"

#define GTR_SOURCE_VIEW_PLUGIN_GET_PRIVATE(object) \
				(G_TYPE_INSTANCE_GET_PRIVATE ((object),	\
				GTR_TYPE_SOURCE_VIEW_PLUGIN,		\
				GtranslatorSourceViewPluginPrivate))

struct _GtranslatorSourceViewPluginPrivate
{
	GConfClient *gconf_client;
	
	/* Dialog stuff */
	GtkWidget *dialog;
	
	GtkWidget *main_box;
	GtkWidget *use_editor_checkbutton;
	GtkWidget *program_box;
	GtkWidget *program_cmd_entry;
	GtkWidget *line_cmd_entry;
	
	GtranslatorWindow *window;

	gint inserted_lines;
};

GTR_PLUGIN_REGISTER_TYPE(GtranslatorSourceViewPlugin, gtranslator_source_view_plugin)

static void 
insert_link (GtkTextBuffer *buffer, 
	     GtkTextIter *iter,
	     const gchar *path,
	     gint *line)
{
	GtkTextTag *tag;
	gchar *text;

	tag = gtk_text_buffer_create_tag (buffer, NULL, 
					  "foreground", "blue", 
					  "underline", PANGO_UNDERLINE_SINGLE, 
					  NULL);
	g_object_set_data (G_OBJECT (tag), "path", g_strdup (path));
	g_object_set_data (G_OBJECT (tag), "line", line);

	text = g_strconcat (path, "\n", NULL);
	gtk_text_buffer_insert_with_tags (buffer, iter, text, -1, tag, NULL);
	g_free (text);
}

static void
show_in_editor (const gchar *program_name,
		const gchar *line_cmd,
		const gchar *path,
		gint line)
{
	gchar *open[4];

	if (g_find_program_in_path (program_name))
	{
		open[0] = g_strdup (program_name);
	}
	else
	{
		GtkWidget *dialog;
		
		dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL,
						 GTK_DIALOG_MODAL,
						 GTK_BUTTONS_CLOSE,
						 _("Please, install %s to be able to show the file"),
						 program_name);
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
	}
	
	open[1] = g_strdup (path);
	open[2] = g_strdup_printf ("%s%d",line_cmd, line);
	open[3] = NULL;
					
	gdk_spawn_on_screen (gdk_screen_get_default (),
			     NULL,
			     open,
			     NULL,
			     G_SPAWN_SEARCH_PATH,
			     NULL,
			     NULL, NULL, NULL);
	g_free (open[0]);
	g_free (open[1]);
	g_free (open[2]);
}

static void
show_source (GtranslatorSourceViewPlugin *plugin,
	     const gchar *path,
	     gint line)
{
	gboolean use_editor;
	
	use_editor = gconf_client_get_bool (plugin->priv->gconf_client,
					    USE_EDITOR_KEY,
					    NULL);

	if (use_editor)
	{
		gchar *program_cmd;
		gchar *line_cmd;
		
		//Program cmd
		program_cmd = gconf_client_get_string (plugin->priv->gconf_client,
						       PROGRAM_CMD_KEY,
						       NULL);
	
		//Line cmd
		line_cmd = gconf_client_get_string (plugin->priv->gconf_client,
						    LINE_CMD_KEY,
						    NULL);
		
		show_in_editor (program_cmd,
				line_cmd,
				path, line);
		
		g_free (program_cmd);
		g_free (line_cmd);
	}
	else gtranslator_show_viewer (plugin->priv->window,
				      path, line);
}

static void
follow_if_link (GtranslatorSourceViewPlugin *plugin,
		GtkWidget   *text_view, 
		GtkTextIter *iter)
{
	GSList *tags = NULL, *tagp = NULL;
	GtranslatorTab *tab;
	GtranslatorPo *po;
	const gchar *po_path;
	gchar *fullpath;
	gchar *dirname;
	
	tab = gtranslator_window_get_active_tab (plugin->priv->window);

	if (!tab)
		return;
	po = gtranslator_tab_get_po (tab);

	po_path = gtranslator_po_get_filename (po);
	dirname = g_path_get_dirname (po_path);
	
	tags = gtk_text_iter_get_tags (iter);
	for (tagp = tags;  tagp != NULL;  tagp = tagp->next)
	{
		GtkTextTag *tag = tagp->data;
		gchar *path = g_object_get_data (G_OBJECT (tag), "path");
		gint line = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (tag), "line"));

		fullpath = g_build_filename (dirname, path, NULL);
		show_source (plugin, fullpath, line);
		
		g_free (fullpath);
	}

	if (tags)
		g_slist_free (tags);
	
	g_free (dirname);
}

static gboolean
event_after (GtkWidget *text_view,
	     GdkEvent  *ev,
	     GtranslatorSourceViewPlugin *plugin)
{
	GtkTextIter start, end, iter;
	GtkTextBuffer *buffer;
	GdkEventButton *event;
	gint x, y;

	if (ev->type != GDK_BUTTON_RELEASE)
		return FALSE;

	event = (GdkEventButton *)ev;

	if (event->button != 1)
		return FALSE;

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));

	/* we shouldn't follow a link if the user has selected something */
	gtk_text_buffer_get_selection_bounds (buffer, &start, &end);
	if (gtk_text_iter_get_offset (&start) != gtk_text_iter_get_offset (&end))
		return FALSE;

	gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (text_view), 
					       GTK_TEXT_WINDOW_WIDGET,
					       event->x, event->y, &x, &y);
	
	gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (text_view), &iter, x, y);

	follow_if_link (plugin, text_view, &iter);

	return FALSE;
}

static gboolean hovering_over_link = FALSE;
static GdkCursor *hand_cursor = NULL;
static GdkCursor *regular_cursor = NULL;

/* Looks at all tags covering the position (x, y) in the text view, 
 * and if one of them is a link, change the cursor to the "hands" cursor
 * typically used by web browsers.
 */
static void
set_cursor_if_appropriate (GtkTextView    *text_view,
                           gint            x,
                           gint            y)
{
	GSList *tags = NULL, *tagp = NULL;
	GtkTextIter iter;
	gboolean hovering = FALSE;

	gtk_text_view_get_iter_at_location (text_view, &iter, x, y);
  
	tags = gtk_text_iter_get_tags (&iter);
	for (tagp = tags;  tagp != NULL;  tagp = tagp->next)
	{
		GtkTextTag *tag = tagp->data;
		gchar *path = g_object_get_data (G_OBJECT (tag), "path");

		if (path) 
		{
			hovering = TRUE;
			break;
		}
	}

	if (hovering != hovering_over_link)
	{
		hovering_over_link = hovering;

		if (hovering_over_link)
			gdk_window_set_cursor (gtk_text_view_get_window (text_view,
									 GTK_TEXT_WINDOW_TEXT),
					       hand_cursor);
		else
			gdk_window_set_cursor (gtk_text_view_get_window (text_view,
									 GTK_TEXT_WINDOW_TEXT),
					       regular_cursor);
	}
	
	if (tags) 
		g_slist_free (tags);
}

/*
 * Update the cursor image if the pointer moved. 
 */
static gboolean
motion_notify_event (GtkWidget      *text_view,
		     GdkEventMotion *event)
{
	gint x, y;

	gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (text_view), 
					       GTK_TEXT_WINDOW_WIDGET,
					       event->x, event->y, &x, &y);

	set_cursor_if_appropriate (GTK_TEXT_VIEW (text_view), x, y);

	gdk_window_get_pointer (text_view->window, NULL, NULL, NULL);
	return FALSE;
}

/* Also update the cursor image if the window becomes visible
 * (e.g. when a window covering it got iconified).
 */
static gboolean
visibility_notify_event (GtkWidget          *text_view,
			 GdkEventVisibility *event)
{
	gint wx, wy, bx, by;
  
	gdk_window_get_pointer (text_view->window, &wx, &wy, NULL);
  
	gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (text_view), 
					       GTK_TEXT_WINDOW_WIDGET,
					       wx, wy, &bx, &by);

	set_cursor_if_appropriate (GTK_TEXT_VIEW (text_view), bx, by);

	return FALSE;
}

static void
gtranslator_source_view_plugin_init (GtranslatorSourceViewPlugin *plugin)
{
	plugin->priv = GTR_SOURCE_VIEW_PLUGIN_GET_PRIVATE (plugin);

	plugin->priv->gconf_client = gconf_client_get_default ();

	gconf_client_add_dir (plugin->priv->gconf_client,
			      SOURCE_VIEW_BASE_KEY,
			      GCONF_CLIENT_PRELOAD_ONELEVEL,
			      NULL);
}

static void
gtranslator_source_view_plugin_finalize (GObject *object)
{
	GtranslatorSourceViewPlugin *plugin = GTR_SOURCE_VIEW_PLUGIN (object);
	
	gconf_client_suggest_sync (plugin->priv->gconf_client, NULL);

	g_object_unref (G_OBJECT (plugin->priv->gconf_client));
	
	G_OBJECT_CLASS (gtranslator_source_view_plugin_parent_class)->finalize (object);
}

static void
showed_message_cb (GtranslatorTab *tab,
		   GtranslatorMsg *msg,
		   GtranslatorSourceViewPlugin *plugin)
{
	const gchar *filename = NULL;
	gint i = 0;
	gint *line = NULL;
	GtkTextIter iter;
	GtkTextBuffer *buffer;
	GtkTextView *view;
	GtranslatorCommentPanel *panel;

	panel = gtranslator_tab_get_comment_panel (tab);
	view = gtranslator_comment_panel_get_extracted_text_view (panel);
	
	buffer = gtk_text_view_get_buffer (view);
	
	gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
	
	gtk_text_buffer_insert (buffer, &iter, _("Paths:\n"), -1);
	
	filename = gtranslator_msg_get_filename (msg, i);
	while (filename)
	{
		line = gtranslator_msg_get_file_line (msg, i);
		insert_link (buffer, &iter, filename, line);
		i++;
		filename = gtranslator_msg_get_filename (msg, i);
	}

	plugin->priv->inserted_lines = i;
}

static void
message_edition_finished_cb (GtranslatorTab *tab,
			     GtranslatorMsg *msg,
			     GtranslatorSourceViewPlugin *plugin)
{
	GSList *tags = NULL, *tagp = NULL;
	GtkTextIter iter;
	GtkTextBuffer *buffer;
	gint i = 0;
	GtranslatorCommentPanel *panel;
	GtkTextView *view;

	panel = gtranslator_tab_get_comment_panel (tab);
	view = gtranslator_comment_panel_get_extracted_text_view (panel);

	buffer = gtk_text_view_get_buffer (view);
	
	gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);

	while (i < plugin->priv->inserted_lines)
	{	
		/*
		 * Let's free the path
		 */
		tags = gtk_text_iter_get_tags (&iter);
		for (tagp = tags;  tagp != NULL;  tagp = tagp->next)
		{
			GtkTextTag *tag = tagp->data;
			gchar *path = g_object_get_data (G_OBJECT (tag), "path");

			if (path) 
			{
				g_free (path);
			}
		}
		g_slist_free (tags);
		gtk_text_iter_forward_line (&iter);
		i++;
	}
}

static void
page_added_cb (GtkNotebook *notebook,
	       GtkWidget *child,
	       guint page_num,
	       GtranslatorSourceViewPlugin *plugin)
{	
	GtranslatorCommentPanel *panel;
	GtkTextView *view;

	panel = gtranslator_tab_get_comment_panel (GTR_TAB (child));
	view = gtranslator_comment_panel_get_extracted_text_view (panel);

	g_signal_connect_after (child, "showed-message",
				G_CALLBACK (showed_message_cb), plugin);
	/*g_signal_connect (child, "message-edition-finished,
			  G_CALLBACK (message_edition_finished_cb), plugin);*/
	
	g_signal_connect (view, "event-after", 
			  G_CALLBACK (event_after), plugin);
	g_signal_connect (view, "motion-notify-event", 
			  G_CALLBACK (motion_notify_event), NULL);
	g_signal_connect (view, "visibility-notify-event", 
			  G_CALLBACK (visibility_notify_event), NULL);
}

static void
use_editor_toggled (GtkToggleButton *button,
		    GtranslatorSourceViewPlugin *plugin)
{
	gtk_widget_set_sensitive (plugin->priv->program_box,
				  gtk_toggle_button_get_active (button));
}

static GtkWidget *
get_configuration_dialog (GtranslatorSourceViewPlugin *plugin)
{

	gboolean ret;
	GtkWidget *error_widget;
	gchar *value;
	gboolean use_editor;
	
	ret = gtranslator_utils_get_glade_widgets (GLADE_FILE,
						  "dialog",
						  &error_widget,
						  "dialog", &plugin->priv->dialog,
						  "main_box", &plugin->priv->main_box,
						  "use_editor", &plugin->priv->use_editor_checkbutton,
						  "program_box", &plugin->priv->program_box,
						  "program_cmd", &plugin->priv->program_cmd_entry,
						  "line_cmd", &plugin->priv->line_cmd_entry,
						  NULL);

	if(!ret)
	{
		//FIXME: We have to show a dialog
	}

	/* Set default values */
	
	//Use editor
	use_editor = gconf_client_get_bool (plugin->priv->gconf_client,
					    USE_EDITOR_KEY,
					    NULL);
	
	g_signal_connect (plugin->priv->use_editor_checkbutton, "toggled",
			  G_CALLBACK (use_editor_toggled), plugin);
	
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (plugin->priv->use_editor_checkbutton),
				      use_editor);
	
	use_editor_toggled (GTK_TOGGLE_BUTTON (plugin->priv->use_editor_checkbutton),
			    plugin);
	
	//Program cmd
	value = gconf_client_get_string (plugin->priv->gconf_client,
					 PROGRAM_CMD_KEY,
					 NULL);

	gtk_entry_set_text (GTK_ENTRY (plugin->priv->program_cmd_entry),
			    value);
	
	g_free (value);
	
	//Line cmd
	value = gconf_client_get_string (plugin->priv->gconf_client,
					 LINE_CMD_KEY,
					 NULL);
	
	gtk_entry_set_text (GTK_ENTRY (plugin->priv->line_cmd_entry),
			    value);

	g_free (value);
	
	return plugin->priv->dialog;
}

static void
impl_activate (GtranslatorPlugin *plugin,
	       GtranslatorWindow *window)
{	
	GtkWidget *notebook;
	GtranslatorSourceViewPlugin *source_view = GTR_SOURCE_VIEW_PLUGIN (plugin);
	GList *tabs, *l;

	/*
	 * Cursors
	 */
	hand_cursor = gdk_cursor_new (GDK_HAND2);
	regular_cursor = gdk_cursor_new (GDK_XTERM);
	
	notebook = GTK_WIDGET (gtranslator_window_get_notebook (window));
	
	source_view->priv->window = window;
	
	g_signal_connect (notebook, "page-added",
			  G_CALLBACK (page_added_cb), plugin);

	/*
	 * If we already have tabs opened we have to add them
	 */
	tabs = gtranslator_window_get_all_tabs (window);
	for (l = tabs; l != NULL; l = g_list_next (l))
	{
		page_added_cb (GTK_NOTEBOOK (notebook),
			       l->data, 0, GTR_SOURCE_VIEW_PLUGIN (plugin));
	}
}

static void
impl_deactivate(GtranslatorPlugin *plugin,
	        GtranslatorWindow *window)
{
	GList *tabs, *l;
	GtkTextView *view;
	GtranslatorCommentPanel *panel;
	GtkWidget *notebook;
	
	tabs = gtranslator_window_get_all_tabs (window);
	notebook = GTK_WIDGET (gtranslator_window_get_notebook (window));
	
	for (l = tabs; l != NULL; l = g_list_next (l))
	{
		panel = gtranslator_tab_get_comment_panel (GTR_TAB (l->data));
		view = gtranslator_comment_panel_get_extracted_text_view (panel);
		
		g_signal_handlers_disconnect_by_func (l->data,
						      showed_message_cb,
						      plugin);

		g_signal_handlers_disconnect_by_func (view,
						      event_after,
						      window);
		g_signal_handlers_disconnect_by_func (view,
						      motion_notify_event,
						      NULL);
		g_signal_handlers_disconnect_by_func (view,
						      visibility_notify_event,
						      NULL);
	}
	
	g_signal_handlers_disconnect_by_func (notebook, 
					      page_added_cb,
					      plugin);
}

static void
ok_button_pressed (GtranslatorSourceViewPlugin *plugin)
{
	const gchar *program_cmd;
	const gchar *line_cmd;
	gboolean use_editor;
	
	/* We have to get the text from the entries */
	use_editor = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (plugin->priv->use_editor_checkbutton));
	program_cmd = gtk_entry_get_text (GTK_ENTRY (plugin->priv->program_cmd_entry));
	line_cmd = gtk_entry_get_text (GTK_ENTRY (plugin->priv->line_cmd_entry));
	
	/* Now we store the data in gconf */
	if (!gconf_client_key_is_writable (plugin->priv->gconf_client,
					   USE_EDITOR_KEY,
					   NULL))
		return;

	gconf_client_set_bool (plugin->priv->gconf_client,
			       USE_EDITOR_KEY,
			       use_editor,
			       NULL);
	
	if (!gconf_client_key_is_writable (plugin->priv->gconf_client,
					   PROGRAM_CMD_KEY,
					   NULL))
		return;

	gconf_client_set_string (plugin->priv->gconf_client,
				 PROGRAM_CMD_KEY,
		       		 program_cmd,
		       		 NULL);
	
	if (!gconf_client_key_is_writable (plugin->priv->gconf_client,
					   LINE_CMD_KEY,
					   NULL))
		return;

	gconf_client_set_string (plugin->priv->gconf_client,
				 LINE_CMD_KEY,
		       		 line_cmd,
		       		 NULL);
}

static void
configure_dialog_response_cb (GtkWidget           *widget,
			      gint                 response,
			      GtranslatorSourceViewPlugin *plugin)
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
	
	dialog = get_configuration_dialog (GTR_SOURCE_VIEW_PLUGIN (plugin));
	
	g_signal_connect (dialog,
			  "response",
			  G_CALLBACK (configure_dialog_response_cb),
			  GTR_SOURCE_VIEW_PLUGIN (plugin));
	g_signal_connect (dialog,
			  "destroy",
			  G_CALLBACK (gtk_widget_destroy),
			  &dialog);
	
	return dialog;
}

static void
gtranslator_source_view_plugin_class_init (GtranslatorSourceViewPluginClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtranslatorPluginClass *plugin_class = GTR_PLUGIN_CLASS (klass);

	object_class->finalize = gtranslator_source_view_plugin_finalize;

	plugin_class->activate = impl_activate;
	plugin_class->deactivate = impl_deactivate;
	plugin_class->create_configure_dialog = impl_create_configure_dialog;
	
	g_type_class_add_private (object_class, sizeof (GtranslatorSourceViewPluginPrivate));
}
