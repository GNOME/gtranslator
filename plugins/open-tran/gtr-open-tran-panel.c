/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *               2010  Yaron Sheffer <yaronf@gmx.com>
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

#include "gtr-open-tran-panel.h"
#include "gtr-application.h"
#include "gtr-window.h"
#include "gtr-header.h"
#include "gtr-dirs.h"

#include <string.h>

#include <glib.h>
#include <glib/gi18n-lib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <json-glib/json-glib.h>
#include <libxml/nanohttp.h>
#include <libxml/uri.h>

#define JSON_LENGTH 65536 /* max length of returned JSON structure.
                             Only recent versions of the library allow
                             for streaming. */

#define GTR_OPEN_TRAN_PANEL_GET_PRIVATE(object) (G_TYPE_INSTANCE_GET_PRIVATE \
						 ((object),		\
						  GTR_TYPE_OPEN_TRAN_PANEL, \
						  GtrOpenTranPanelPrivate))

#define GNOME_ICON       "gnome.png"
#define KDE_ICON         "kde.ico"
#define MOZILLA_ICON     "mozilla.png"
#define DEBIAN_ICON      "debian.png"
#define SUSE_ICON        "suse.png"
#define XFCE_ICON        "xfce.png"
#define INKSCAPE_ICON    "inkscape.png"
#define OPEN_OFFICE_ICON "oo-logo.png"
#define FEDORA_ICON      "fedora.png"
#define MANDRIVA_ICON    "mandriva.png"

G_DEFINE_DYNAMIC_TYPE (GtrOpenTranPanel, gtr_open_tran_panel, GTK_TYPE_BOX)

struct _GtrOpenTranPanelPrivate
{
  GSettings *settings;
  
  GtkWidget *treeview;
  GtkTreeStore *store;
  
  GtkWidget *entry;

  GtkWidget *window;

  gchar *text;
};

enum
{
  ICON_COLUMN,
  TEXT_COLUMN,
  TOOLTIP_COLUMN,
  N_COLUMNS
};

static void
show_error_dialog (GtkWidget * parent,
                   const gchar * message_format, ...)
{
  gchar *msg = NULL;
  va_list args;
  GtkWidget *dialog;

  va_start (args, message_format);
  msg = g_strdup_vprintf (message_format, args);
  va_end (args);

  dialog = gtk_message_dialog_new (GTK_WINDOW (parent),
                                   GTK_DIALOG_DESTROY_WITH_PARENT,
                                   GTK_MESSAGE_ERROR,
                                   GTK_BUTTONS_CLOSE, "%s", msg);
  g_free (msg);

  g_signal_connect (dialog, "response",
                    G_CALLBACK (gtk_widget_destroy), &dialog);
  gtk_widget_show (dialog);
}

static gchar *
get_service_url (gboolean use_mirror_server, gchar *mirror_server_url,
                 const gchar * search_text,
                 const gchar * search_code, const gchar * own_code)
{
  const gchar *base_url;
  gchar *url, *full_url, *escaped_text;
  const gchar * open_tran_url = "http://%s.%s.open-tran.eu/json/suggest/";
  /* URL placeholders: source lang, target lang. Search string is appended. */

  if (!use_mirror_server)
    base_url = open_tran_url;
  else
    base_url = mirror_server_url;

  escaped_text = (gchar *) xmlURIEscapeStr ((const xmlChar *) search_text, (const xmlChar *) "");
  url = g_strdup_printf (base_url, search_code, own_code);
  full_url = g_strconcat (url, escaped_text, NULL);

  g_free (url);
  if (escaped_text)
    xmlFree (escaped_text);

  return full_url;
}

static GdkPixbuf *
create_pixbuf (const gchar *filename)
{
  GdkPixbuf *icon;
  gchar *path;
  GError *error = NULL;

  path = g_build_filename (gtr_dirs_get_gtr_pixmaps_dir (),
                           filename,
                           NULL);
  icon = gdk_pixbuf_new_from_file (path, &error);
  g_free (path);

  if (error)
    {
      g_warning ("Could not load icon: %s", error->message);
      g_error_free (error);
      return NULL;
    }

  return icon;
}

static void
print_string_to_tree_view (const gchar *iconname, const gchar * str,
                           const gchar *tooltip,
                           GtkTreeIter *parent, GtrOpenTranPanel * panel)
{
  GdkPixbuf *icon;
  GtkTreeIter iter;

  /* Text value */
  if (strcmp ("GNOME", iconname) == 0)
    icon = create_pixbuf (GNOME_ICON);
  else if (strcmp ("KDE", iconname) == 0)
    icon = create_pixbuf (KDE_ICON);
  else if (strcmp ("MOZILLA", iconname) == 0)
    icon = create_pixbuf (MOZILLA_ICON);
  else if (strcmp ("Mozilla", iconname) == 0)
    icon = create_pixbuf (MOZILLA_ICON);
  else if (strcmp ("DEBIAN", iconname) == 0)
    icon = create_pixbuf (DEBIAN_ICON);
  else if (strcmp ("Debian Installer", iconname) == 0)
    icon = create_pixbuf (DEBIAN_ICON);
  else if (strcmp ("SUSE", iconname) == 0)
    icon = create_pixbuf (SUSE_ICON);
  else if (strcmp ("XFCE", iconname) == 0)
    icon = create_pixbuf (XFCE_ICON);
  else if (strcmp ("Inkscape", iconname) == 0)
    icon = create_pixbuf (INKSCAPE_ICON);
  else if (strcmp ("OpenOffice.org", iconname) == 0)
    icon = create_pixbuf (OPEN_OFFICE_ICON);
  else if (strcmp ("Fedora", iconname) == 0)
    icon = create_pixbuf (FEDORA_ICON);
  else if (strcmp ("Mandriva", iconname) == 0)
    icon = create_pixbuf (MANDRIVA_ICON);
  else
    icon = NULL;

  gtk_tree_store_append (panel->priv->store, &iter, parent);
  gtk_tree_store_set (panel->priv->store, &iter,
                      ICON_COLUMN, icon, TEXT_COLUMN, str,
                      TOOLTIP_COLUMN, tooltip,
                      -1);

  if (icon)
    g_object_unref (icon);
}

static void
open_connection (GtrOpenTranPanel * panel,
		 const gchar *service_url,
                 const gchar * text)
{
  GError *err = NULL;
  GtkTreeIter treeiter;
  void * ctx;
  unsigned int offset = 0;
  int ret;
  gchar * json_buf;
  JsonParser *parser;
  JsonNode *root;
  JsonArray *array;
  int idx;

  ctx = xmlNanoHTTPOpen (service_url, NULL);
  if (!ctx)
    {
      show_error_dialog (panel->priv->window,
                         _("ERROR: Cannot access %s\n"), service_url);
      return;
    }

  json_buf = g_malloc (JSON_LENGTH);

  do
    {
      ret = xmlNanoHTTPRead (ctx, json_buf, JSON_LENGTH-offset);
      if (ret > 0)
        offset += ret;
    } while (ret > 0);

  if (ret == -1)
    {
      show_error_dialog (panel->priv->window,
                         _("Error in server response, GET failed\n"));
      g_free (json_buf);
      return;
    }

  xmlNanoHTTPClose (ctx);

  parser = json_parser_new ();
  json_parser_load_from_data (parser, json_buf, offset, &err);
  if (err)
    {
      show_error_dialog (panel->priv->window,
                         _("Cannot parse server response, %s\n"),
                         err->message);
      g_error_free (err);
      g_free (json_buf);
      if (parser)
        g_object_unref (parser);
      return;
    }

  root = json_parser_get_root (parser);
  array = json_node_get_array (root);
  if (!array)
    {
      show_error_dialog (panel->priv->window,
                         _("Cannot parse server response, not an array? %s\n"),
                         err->message);
      g_error_free (err);
      g_free (json_buf);
      if (parser)
        g_object_unref (parser);
      return;
    }

  for (idx = 0; idx < json_array_get_length (array); idx++)
    {
      JsonNode *elem;
      JsonObject *obj;
      const gchar *text;
      GtkTreeIter iter;
      GtkTreeIter project_tree_iter;
      JsonArray *projects;
      int j;

      elem = json_array_get_element (array, idx);
      if (!elem)
        {
          show_error_dialog (panel->priv->window,
                             _("WRONG! Can't get result element %d\n"),
                             idx + 1);
          break;
        }

      obj = json_node_get_object (elem);

      if (!obj)
        {
          show_error_dialog (panel->priv->window,
                             _("WRONG! Can't parse result element %d as object\n"),
                             idx + 1);
          break;
        }

      text = json_object_get_string_member (obj, "text");
      gtk_tree_store_append (panel->priv->store, &iter, NULL);
      gtk_tree_store_set (panel->priv->store, &iter,
                      TEXT_COLUMN, text, -1);
      project_tree_iter = iter;
      projects = json_object_get_array_member (obj, "projects");
      if (!projects)
        {
          show_error_dialog (panel->priv->window,
                             _("WRONG! Can't read projects for result element %d\n"),
                             idx + 1);
          break;
        }

      for (j = 0; j < json_array_get_length (projects); j++)
        {
          JsonObject *project = json_array_get_object_element (projects, j);

          const gchar *orig_phrase = json_object_get_string_member (project, "orig_phrase");
          const gchar *project_name = json_object_get_string_member (project, "name");
          const int count = json_object_get_int_member (project, "count");

          const gchar *path_name = json_object_get_string_member (project, "path");
          /* format: X/subproject, where X stands for the project, e.g. G for GNOME */
          gchar *tooltip;

          if (path_name != NULL)
            {
              path_name = g_strrstr (path_name, "/");
              if (path_name != NULL)
                path_name++;
            }

          tooltip = g_strdup_printf (_("%s[%s] Count:%d"),
                                     project_name, path_name, count);

          print_string_to_tree_view (project_name, orig_phrase, tooltip,
                                     &project_tree_iter, panel);
          g_free (tooltip);

          if (!project || !orig_phrase || !project_name)
            {
              show_error_dialog (panel->priv->window,
                                 _("WRONG! Malformed project: %d\n"),j + 1);
              break;
            }
        }
    }


  /* We have to check if we didn't find any text */
  if (!gtk_tree_model_get_iter_first (GTK_TREE_MODEL (panel->priv->store),
                                      &treeiter))
    {
      gtk_tree_store_append (panel->priv->store, &treeiter, NULL);
      gtk_tree_store_set (panel->priv->store, &treeiter,
                          ICON_COLUMN, NULL,
                          TEXT_COLUMN, _("Phrase not found"), -1);
    }

  g_free (json_buf);
  if (parser)
    g_object_unref (parser);
}

static void
entry_activate_cb (GtkEntry * entry, GtrOpenTranPanel * panel)
{
  const gchar *entry_text;
  gchar *search_code = NULL;
  gchar *own_code = NULL;
  gboolean use_mirror_server;
  gchar *mirror_server_url = NULL;
  gchar *service_url = NULL;

  gtk_tree_store_clear (panel->priv->store);

  entry_text = gtk_entry_get_text (GTK_ENTRY (panel->priv->entry));
  if (!entry_text || strlen (entry_text) == 0)
    {
      show_error_dialog (panel->priv->window,
                         _("You have to provide a phrase to search"));
      goto cleanup;
    }

  search_code = g_settings_get_string (panel->priv->settings,
                                       GTR_SETTINGS_SEARCH_CODE);
  if (!search_code || strlen (search_code) == 0)
    {
      show_error_dialog (panel->priv->window,
                         _("You have to provide a search language code in the plugin configuration"));
      goto cleanup;
    }

  own_code = g_settings_get_string (panel->priv->settings,
                                    GTR_SETTINGS_OWN_CODE);

  if (!own_code || strlen (own_code) == 0)
    {
      show_error_dialog (panel->priv->window,
                         _("You have to provide a language code for your language in the plugin configuration"));
      goto cleanup;
    }

  use_mirror_server = g_settings_get_boolean (panel->priv->settings,
                                       GTR_SETTINGS_USE_MIRROR_SERVER);
  mirror_server_url = g_settings_get_string (panel->priv->settings,
                                       GTR_SETTINGS_MIRROR_SERVER_URL);
  
  if (use_mirror_server && (!mirror_server_url || strlen (mirror_server_url) == 0))
    {
      /* Note: the two "%s" in the next string are not interpolated */
      show_error_dialog (panel->priv->window,
                         _("Either use the main open-tran.eu server,"
			   " or enter a server URL in the plugin configuration,"));
      goto cleanup;
    }

  service_url = get_service_url (use_mirror_server, mirror_server_url,
				 entry_text,
				 search_code, own_code);
  open_connection (panel, service_url, entry_text);

 cleanup:
  g_free (search_code);
  g_free (own_code);
  g_free (service_url);
  g_free (mirror_server_url);
}

static void
gtr_open_tran_panel_draw_treeview (GtrOpenTranPanel * panel)
{
  GtkTreeViewColumn *column;
  GtkCellRenderer *renderer;

  GtrOpenTranPanelPrivate *priv = panel->priv;

  priv->store = gtk_tree_store_new (N_COLUMNS,
                                    GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING);

  priv->treeview =
    gtk_tree_view_new_with_model (GTK_TREE_MODEL (priv->store));
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (priv->treeview), FALSE);

  /*
   * Icon column
   */
  column = gtk_tree_view_column_new ();

  renderer = gtk_cell_renderer_pixbuf_new ();
  column = gtk_tree_view_column_new_with_attributes (_("Type"),
                                                     renderer, "pixbuf",
                                                     ICON_COLUMN, NULL);
  gtk_tree_view_column_set_resizable (column, FALSE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (priv->treeview), column);

  /*
   * Text column
   */
  column = gtk_tree_view_column_new ();
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes (_("Open-Tran.eu"),
                                                     renderer,
                                                     "text", TEXT_COLUMN,
                                                     NULL);
  /*
   * Row-wide tooltip
   */
  gtk_tree_view_set_tooltip_column (GTK_TREE_VIEW (priv->treeview), TOOLTIP_COLUMN);

  gtk_tree_view_column_set_resizable (column, FALSE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (priv->treeview), column);
}

static void
gtr_open_tran_panel_init (GtrOpenTranPanel * panel)
{
  GtkWidget *scrolledwindow;
  GtkWidget *button;
  GtkWidget *hbox;

  panel->priv = GTR_OPEN_TRAN_PANEL_GET_PRIVATE (panel);

  panel->priv->settings = g_settings_new ("org.gnome.gtranslator.plugins.open-tran");

  gtk_orientable_set_orientation (GTK_ORIENTABLE (panel),
                                  GTK_ORIENTATION_VERTICAL);

  /*
   * Set up the scrolling window
   */
  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow),
                                       GTK_SHADOW_IN);
  gtk_box_pack_start (GTK_BOX (panel), scrolledwindow, TRUE, TRUE, 0);

  /*
   * TreeView
   */
  gtr_open_tran_panel_draw_treeview (panel);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), panel->priv->treeview);

  /*
   * Entry
   */
  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);

  button = gtk_button_new_with_label (_("Look for:"));
  gtk_button_set_relief (GTK_BUTTON (button), GTK_RELIEF_HALF);
  g_signal_connect (button, "clicked", G_CALLBACK (entry_activate_cb), panel);

  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);

  panel->priv->entry = gtk_entry_new ();
  gtk_box_pack_start (GTK_BOX (hbox), panel->priv->entry, TRUE, TRUE, 0);
  g_signal_connect (panel->priv->entry, "activate",
                    G_CALLBACK (entry_activate_cb), panel);

  gtk_box_pack_start (GTK_BOX (panel), hbox, FALSE, TRUE, 0);

}

static void
gtr_open_tran_panel_dispose (GObject * object)
{
  GtrOpenTranPanel *panel = GTR_OPEN_TRAN_PANEL (object);

  g_clear_object (&panel->priv->settings);

  G_OBJECT_CLASS (gtr_open_tran_panel_parent_class)->dispose (object);
}

static void
gtr_open_tran_panel_class_init (GtrOpenTranPanelClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrOpenTranPanelPrivate));

  object_class->dispose = gtr_open_tran_panel_dispose;
}

static void
gtr_open_tran_panel_class_finalize (GtrOpenTranPanelClass *klass)
{
}

GtkWidget *
gtr_open_tran_panel_new (GtrTab *tab)
{
  GtrOpenTranPanel *panel;

  panel = g_object_new (GTR_TYPE_OPEN_TRAN_PANEL, NULL);

  panel->priv->window = gtk_widget_get_toplevel (GTK_WIDGET (tab));

  return GTK_WIDGET (panel);
}

void
_gtr_open_tran_panel_register_type (GTypeModule *type_module)
{
  gtr_open_tran_panel_register_type (type_module);
}
