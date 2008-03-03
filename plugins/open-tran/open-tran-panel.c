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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "open-tran-panel.h"
#include "application.h"
#include "window.h"
#include "header.h"
#include "plugin.h"

#include <string.h>

#include <glib.h>
#include <glib/gi18n-lib.h>
#include <glib-object.h>
#include <gconf/gconf-client.h>
#include <gtk/gtk.h>
#include <libsoup/soup.h>

#ifdef LIBSOUP_2_2
#include <libsoup/soup-xmlrpc-message.h>
#include <libsoup/soup-xmlrpc-response.h>
#endif

#define GTR_OPEN_TRAN_PANEL_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),		       \
						 GTR_TYPE_OPEN_TRAN_PANEL,     \
						 GtranslatorOpenTranPanelPrivate))

#define GNOME_ICON   PIXMAPSDIR"/gnome.png"
#define KDE_ICON     PIXMAPSDIR"/kde.ico"
#define MOZILLA_ICON PIXMAPSDIR"/mozilla.png"
#define DEBIAN_ICON  PIXMAPSDIR"/debian.png"

GTR_PLUGIN_DEFINE_TYPE(GtranslatorOpenTranPanel, gtranslator_open_tran_panel, GTK_TYPE_VBOX)

struct _GtranslatorOpenTranPanelPrivate
{
	GConfClient *gconf_client;
	
	GtkWidget *treeview;
	GtkListStore *store;
	
	GtkWidget *entry;
	
	SoupSession *session;
	
	GtranslatorWindow *window;
	
	gchar *text;
};

enum
{
	ICON_COLUMN,
	TEXT_COLUMN,
	N_COLUMNS
};

static void
show_error_dialog (GtranslatorWindow *parent,
		   const gchar *message_format,
		   ...)
{
	gchar *msg = NULL;
	va_list args;
	GtkWidget *dialog;
	
	va_start (args, message_format);
	msg = g_strdup_vprintf (message_format, args);
	va_end (args);
	
	dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
				       GTK_DIALOG_DESTROY_WITH_PARENT,
				       GTK_MESSAGE_ERROR,
				       GTK_BUTTONS_CLOSE,
				       msg);
	g_free(msg);
	
	g_signal_connect(dialog, "response",
			 G_CALLBACK(gtk_widget_destroy),
			 &dialog);
	gtk_widget_show(dialog);
}

static GdkPixbuf *
create_pixbuf(const gchar *path)
{
	GdkPixbuf *icon;
	GError *error = NULL;
	
	icon = gdk_pixbuf_new_from_file(path, &error);
	
	if (error)
	{
		g_warning ("Could not load icon: %s\n", error->message);
		g_error_free(error);
		return NULL;
	}
	
	return icon;
}

static void
print_struct_to_tree_view(gpointer value,
			  GtranslatorOpenTranPanel *panel)
{
#ifdef LIBSOUP_2_2
	gchar *str;
#elif LIBSOUP_2_4
	const gchar *str;
#endif
	GdkPixbuf *icon;
	GtkTreeIter iter;

	/*
	 * Text value
	 */
#ifdef LIBSOUP_2_2	 
	if (soup_xmlrpc_value_get_string (value, &str))
	{
#elif LIBSOUP_2_4
	if (G_VALUE_HOLDS_STRING (value))
	{
		str = g_value_get_string (value);
#endif
		/*
		 * We have to parse the first character of str:
		 * G - Gnome
		 * K - KDE
		 * M - Mozilla
		 * D - Debian Installer
		 * F - Frysian dictionary
		 */
		switch(*str)
		{
			case 'G': icon = create_pixbuf(GNOME_ICON);
				break;
			case 'K': icon = create_pixbuf(KDE_ICON);
				break;
			case 'M': icon = create_pixbuf(MOZILLA_ICON);
				break;
			case 'D': icon = create_pixbuf(DEBIAN_ICON);
				break;
			default: icon = NULL;
				break;
		}
		
		gtk_list_store_append(panel->priv->store, &iter);
		gtk_list_store_set(panel->priv->store, &iter,
				   ICON_COLUMN, icon,
				   TEXT_COLUMN, panel->priv->text,
				   -1);
				   
		g_free (panel->priv->text);
	}
}

/*
 * To see the protocol spec:
 *  http://bugzilla.gnome.org/show_bug.cgi?id=506469
 */
static void
print_struct_field (gpointer key,
		    gpointer value,
		    gpointer data)
{
	GtranslatorOpenTranPanel *panel = GTR_OPEN_TRAN_PANEL(data);
	GHashTable *hash;
	GList *values;
#ifdef LIBSOUP_2_2
	gchar *str;
	SoupXmlrpcValue *result;
	SoupXmlrpcValueArrayIterator *ar_iter;
#elif LIBSOUP_2_4
	GValueArray *array;
	const gchar *str;
#endif
	

#ifdef LIBSOUP_2_2	
	/*
	 * Text value
	 */
	if (soup_xmlrpc_value_get_string (value, &str))
	{
		panel->priv->text = str;
	}
	
	/*
	 * Now we have to detect the type of the text (Gnome, Kde...)
	 */
	if (soup_xmlrpc_value_array_get_iterator (value, &ar_iter)) 
	{
		if(!soup_xmlrpc_value_array_iterator_get_value(ar_iter, &result))
		{
			show_error_dialog(panel->priv->window,
					  _("WRONG! Can't get result element 1\n"));
			return;
		}
		if (!soup_xmlrpc_value_get_struct (result, &hash)) {
			show_error_dialog(panel->priv->window,
					  _("WRONG! Result element 1 is not a struct"));
			return;
		}
	
		values = g_hash_table_get_values(hash);
		if((values = g_list_next(values)))
			print_struct_to_tree_view(values->data, panel);
	}
#elif LIBSOUP_2_4
	if (G_VALUE_HOLDS_STRING (value))
	{
		str = g_value_get_string (value);
		panel->priv->text = g_strdup (str);
	}
			
	if (G_VALUE_HOLDS (value, G_TYPE_VALUE_ARRAY))
	{
		array = g_value_get_boxed (value);

		if (G_VALUE_HOLDS (array->values, G_TYPE_HASH_TABLE))
		{
			hash = g_value_get_boxed (array->values);
			g_value_array_free (array);
		}
		else return;
			
		values = g_hash_table_get_values(hash);
		if((values = g_list_next(values)))
			print_struct_to_tree_view(values->data, panel);
	}
#endif
}

#ifdef LIBSOUP_2_2
static void
got_response (SoupMessage *msg,
	      gpointer data)
{
	GtranslatorOpenTranPanel *panel = GTR_OPEN_TRAN_PANEL(data);
	SoupXmlrpcResponse *response;
	SoupXmlrpcValue *value;
	GHashTable *hash;
	SoupXmlrpcValueArrayIterator *iter;
	SoupXmlrpcValue *result;
	GtkTreeIter treeiter;
	gint i;
	
	if (!SOUP_STATUS_IS_SUCCESSFUL (msg->status_code)) {
		show_error_dialog(panel->priv->window,
				  _("Error: %d %s\n"),
				  msg->status_code,
				  msg->reason_phrase);
		return;
	}

	response = soup_xmlrpc_message_parse_response (SOUP_XMLRPC_MESSAGE (msg));
	if (!response) {
		show_error_dialog(panel->priv->window,
				 /*
				  * Translators: XMLRPC is the name of the protocol
				  */
				  _("Could not parse XMLRPC response\n"));
		return;
	}

	value = soup_xmlrpc_response_get_value (response);
	if (!value) {
		show_error_dialog(panel->priv->window,
				 /*
				  * Translators: XMLRPC is the name of the protocol
				  */
				  _("No response value in XMLRPC response\n"));
		return;
	}

	if (!soup_xmlrpc_value_array_get_iterator (value, &iter)) {
		show_error_dialog(panel->priv->window,
				 /*
				  * Translators: XMLRPC is the name of the protocol
				  */
				  _("Could not extract result from XMLRPC response\n"));
		return;
	}

	i = 0;
	while(iter)
	{
		if(!soup_xmlrpc_value_array_iterator_get_value(iter, &result))
		{
			show_error_dialog(panel->priv->window,
					  _("WRONG! Can't get result element %d\n"), i + 1);
			g_object_unref (response);
			return;
		}
		if (!soup_xmlrpc_value_get_struct (result, &hash)) {
			show_error_dialog(panel->priv->window,
					  _("WRONG! Result element %d is not a struct"), i + 1);
			g_object_unref (response);
			return;
		}
		
		g_hash_table_foreach(hash, print_struct_field, data);
		iter = soup_xmlrpc_value_array_iterator_next(iter);
		i++;
	}

	g_object_unref (response);
	
	/*
	 * We have to check if we didn't find any text
	 */
	if(!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(panel->priv->store),
                                          &treeiter))
	{
		gtk_list_store_append(panel->priv->store, &treeiter);
		gtk_list_store_set(panel->priv->store, &treeiter,
				   ICON_COLUMN, NULL,
				   TEXT_COLUMN, _("Phrase not found"),
				   -1);
	}
}
#endif

#ifdef LIBSOUP_2_4
static void
check_xmlrpc (GValue *value, GType type, ...)
{
	va_list args;

	if (!G_VALUE_HOLDS (value, type)) {
		g_warning (_("ERROR: could not parse response\n"));
		g_value_unset (value);
		return;
	}

	va_start (args, type);
	SOUP_VALUE_GETV (value, type, args);
	va_end (args);
}
#endif

static void
open_connection(GtranslatorOpenTranPanel *panel,
		const gchar *text,
		const gchar *search_code,
		const gchar *own_code)
{
	const gchar *uri = "http://open-tran.eu/RPC2";

#ifdef LIBSOUP_2_2
	SoupXmlrpcMessage *msg;
	SoupUri *proxy = NULL; //This can be useful in a future

	panel->priv->session = soup_session_async_new_with_options (
		SOUP_SESSION_PROXY_URI, proxy,
		NULL);

	msg = soup_xmlrpc_message_new (uri);
	if (!msg) {
		show_error_dialog(panel->priv->window,
				  _("Could not create web service request to '%s'\n"), uri);
		return;
	}

	soup_xmlrpc_message_start_call (msg, "suggest2");
	soup_xmlrpc_message_start_param (msg);
	soup_xmlrpc_message_write_string (msg, text);
	soup_xmlrpc_message_write_string (msg, search_code);
	soup_xmlrpc_message_write_string (msg, own_code);
	soup_xmlrpc_message_end_param (msg);
	soup_xmlrpc_message_end_call (msg);

	soup_xmlrpc_message_persist (msg);
	soup_session_queue_message (panel->priv->session, SOUP_MESSAGE (msg),
				    got_response, panel);
#elif LIBSOUP_2_4
	SoupMessage *msg;
	gchar *body;
	GValueArray *array;
	GError *err = NULL;
	GValue retval;
	GHashTable *result;
	GtkTreeIter treeiter;
	gint i;
	
	panel->priv->session = soup_session_async_new ();
	
	array = soup_value_array_new_with_vals (G_TYPE_STRING, text,
						G_TYPE_STRING, search_code,
						G_TYPE_STRING, own_code,
						G_TYPE_INVALID);
	
	body = soup_xmlrpc_build_method_call ("suggest2", array->values,
					      array->n_values);
					      
	g_value_array_free (array);
	
	if (!body)
		return;

	msg = soup_message_new ("POST", uri);
	soup_message_set_request (msg, "text/xml", SOUP_MEMORY_TAKE,
				  body, strlen (body));
	soup_session_send_message (panel->priv->session, msg);
	
	if (!SOUP_STATUS_IS_SUCCESSFUL (msg->status_code)) {
		show_error_dialog (panel->priv->window,
				   _("ERROR: %d %s\n"), msg->status_code,
				   msg->reason_phrase);
		g_object_unref (msg);
		return;
	}

	if (!soup_xmlrpc_parse_method_response (msg->response_body->data,
						msg->response_body->length,
						&retval, &err)) {
		if (err) {
			show_error_dialog (panel->priv->window, 
					   _("FAULT: %d %s\n"), err->code,
					   err->message);
			g_error_free (err);
		} else
			show_error_dialog (panel->priv->window,
					   _("ERROR: could not parse response\n"));
	}
	g_object_unref (msg);
	
	check_xmlrpc (&retval, G_TYPE_VALUE_ARRAY, &array);
	
	for (i = 0; i < array->n_values; i++)
	{
		if (!soup_value_array_get_nth (array, i, G_TYPE_HASH_TABLE, &result))
		{
			show_error_dialog(panel->priv->window,
					  _("WRONG! Can't get result element %d\n"), i + 1);
			break;
		}
		
		g_hash_table_foreach(result, print_struct_field, panel);
	}
	
	/*
	 * We have to check if we didn't find any text
	 */
	if(!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(panel->priv->store),
                                          &treeiter))
	{
		gtk_list_store_append(panel->priv->store, &treeiter);
		gtk_list_store_set(panel->priv->store, &treeiter,
				   ICON_COLUMN, NULL,
				   TEXT_COLUMN, _("Phrase not found"),
				   -1);
	}
	
	soup_session_abort (panel->priv->session);
	g_object_unref (panel->priv->session);
#endif
}

static void
entry_activate_cb (GtkEntry *entry,
		   GtranslatorOpenTranPanel *panel)
{
	const gchar *entry_text = NULL;
	const gchar *search_code = NULL;
	const gchar *own_code = NULL;
	
	gtk_list_store_clear(panel->priv->store);
	
	entry_text = gtk_entry_get_text(GTK_ENTRY(panel->priv->entry));
	if(!entry_text)
	{
		show_error_dialog(panel->priv->window,
				  _("You have to provide a phrase to search"));
		return;
	}
	
	search_code = gconf_client_get_string (panel->priv->gconf_client,
					       SEARCH_CODE_KEY,
					       NULL);
	if(!search_code)
	{
		show_error_dialog(panel->priv->window,
				  _("You have to provide a search language code"));
		return;
	}
	
	own_code = gconf_client_get_string (panel->priv->gconf_client,
					    OWN_CODE_KEY,
					    NULL);
	if(!own_code)
	{
		show_error_dialog(panel->priv->window,
				  _("You have to provide a language code for your language"));
		return;
	}
	
	open_connection(panel, entry_text, search_code, own_code);
}

static void
gtranslator_open_tran_panel_draw_treeview (GtranslatorOpenTranPanel *panel)
{
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;	
	
	GtranslatorOpenTranPanelPrivate *priv = panel->priv;
	
	priv->store = gtk_list_store_new(N_COLUMNS,
					 GDK_TYPE_PIXBUF,
					 G_TYPE_STRING);
					 
	priv->treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(priv->store));
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(priv->treeview),
					  FALSE);
	
	/*
	 * Icon column
	 */
	column = gtk_tree_view_column_new();
	
	renderer = gtk_cell_renderer_pixbuf_new();
	column = gtk_tree_view_column_new_with_attributes(_("Type"),
							  renderer, "pixbuf",
							  ICON_COLUMN, NULL);
	gtk_tree_view_column_set_resizable(column, FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(priv->treeview), column);
	
	/*
	 * Text column
	 */
	column = gtk_tree_view_column_new();
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Open-Tran.eu"),
							  renderer,
							  "text", TEXT_COLUMN,
							  NULL);

	gtk_tree_view_column_set_resizable(column, FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(priv->treeview), column);
}

static void
gtranslator_open_tran_panel_draw (GtranslatorOpenTranPanel *panel)
{
	GtkWidget *scrolledwindow;
	GtkWidget *button;
	GtkWidget *hbox;
	
	/*
	 * Set up the scrolling window for the extracted comments display
	 */	
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow),
					    GTK_SHADOW_IN);
	gtk_box_pack_start(GTK_BOX(panel), scrolledwindow, TRUE, TRUE, 0);
	
	/*
	 * TreeView
	 */
	gtranslator_open_tran_panel_draw_treeview (panel);
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  panel->priv->treeview);
			  
	/*
	 * Entry
	 */
	hbox = gtk_hbox_new(FALSE, 6);
	
	button = gtk_button_new_with_label(_("Look for:"));
	gtk_button_set_relief(GTK_BUTTON(button),
			      GTK_RELIEF_NONE);
	g_signal_connect(button, "clicked",
			 G_CALLBACK(entry_activate_cb), panel);
			      
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	
	panel->priv->entry = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbox), panel->priv->entry, TRUE, TRUE, 0);
	g_signal_connect(panel->priv->entry, "activate",
			 G_CALLBACK(entry_activate_cb), panel);
	
	gtk_box_pack_start(GTK_BOX(panel), hbox, FALSE, TRUE, 0);
}

static void
gtranslator_open_tran_panel_init (GtranslatorOpenTranPanel *panel)
{
	
	panel->priv = GTR_OPEN_TRAN_PANEL_GET_PRIVATE (panel);

	panel->priv->gconf_client = gconf_client_get_default ();
	
	gtranslator_open_tran_panel_draw(panel);
}

static void
gtranslator_open_tran_panel_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_open_tran_panel_parent_class)->finalize (object);
}

static void
gtranslator_open_tran_panel_class_init (GtranslatorOpenTranPanelClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorOpenTranPanelPrivate));

	object_class->finalize = gtranslator_open_tran_panel_finalize;
}

GtkWidget *
gtranslator_open_tran_panel_new (GtranslatorWindow *window)
{
	GtranslatorOpenTranPanel *panel;
	
	panel = g_object_new (GTR_TYPE_OPEN_TRAN_PANEL, NULL);
	
	panel->priv->window = window;

	return GTK_WIDGET(panel);
}

