/*
 * gtr-charmap-plugin.c - Character map side-pane for gtr
 * 
 * Copyright (C) 2006 Steve Frécinaux
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib/gi18n-lib.h>

#include "gtr-charmap-plugin.h"
#include "gtr-charmap-panel.h"
#include "gtr-debug.h"
#include "gtr-application.h"
#include "gtr-statusbar.h"
#include "gtr-window.h"

#ifdef HAVE_GUCHARMAP_2
#include <gucharmap/gucharmap.h>
#else
#include <gucharmap/gucharmap-table.h>
#include <gucharmap/gucharmap-unicode-info.h>
#endif

#define WINDOW_DATA_KEY	"GtrCharmapPluginWindowData"

#define GTR_CHARMAP_PLUGIN_GET_PRIVATE(object) \
				(G_TYPE_INSTANCE_GET_PRIVATE ((object),	\
				GTR_TYPE_CHARMAP_PLUGIN,		\
				GtrCharmapPluginPrivate))

typedef struct
{
  GtkWidget *panel;
  guint context_id;
} WindowData;

GTR_PLUGIN_REGISTER_TYPE_WITH_CODE (GtrCharmapPlugin,
				    gtr_charmap_plugin,
				    gtr_charmap_panel_register_type
				    (module);)
     static void gtr_charmap_plugin_init (GtrCharmapPlugin *
						  plugin)
{
  //gtr_debug_message (DEBUG_PLUGINS, "GtrCharmapPlugin initializing");
}

static void
gtr_charmap_plugin_finalize (GObject * object)
{
  //gtr_debug_message (DEBUG_PLUGINS, "GtrCharmapPlugin finalizing");

  G_OBJECT_CLASS (gtr_charmap_plugin_parent_class)->finalize (object);
}

static void
free_window_data (WindowData * data)
{
  g_slice_free (WindowData, data);
}

static void
#ifdef HAVE_GUCHARMAP_2
  on_table_status_message (GucharmapChartable * chartable,
#else
on_table_status_message (GucharmapTable * chartable,
#endif
			 const gchar * message, GtrWindow * window)
{
  GtrStatusbar *statusbar;
  WindowData *data;

  statusbar = GTR_STATUSBAR (gtr_window_get_statusbar (window));
  data = (WindowData *) g_object_get_data (G_OBJECT (window),
					   WINDOW_DATA_KEY);
  g_return_if_fail (data != NULL);

  gtr_statusbar_pop (statusbar, data->context_id);

  if (message)
    gtr_statusbar_push (statusbar, data->context_id, message);
}

static void
#ifdef HAVE_GUCHARMAP_2
on_table_sync_active_char (GucharmapChartable * chartable,
			   GParamSpec * psepc, GtrWindow * window)
#else
on_table_set_active_char (GucharmapTable * chartable,
			  gunichar wc, GtrWindow * window)
#endif
{
  GString *gs;
  const gchar **temps;
  gint i;
#ifdef HAVE_GUCHARMAP_2
  gunichar wc;

  wc = gucharmap_chartable_get_active_character (chartable);
#endif

  gs = g_string_new (NULL);
  g_string_append_printf (gs, "U+%4.4X %s", wc,
			  gucharmap_get_unicode_name (wc));

  temps = gucharmap_get_nameslist_equals (wc);
  if (temps)
    {
      g_string_append_printf (gs, "   = %s", temps[0]);
      for (i = 1; temps[i]; i++)
	g_string_append_printf (gs, "; %s", temps[i]);
      g_free (temps);
    }

  temps = gucharmap_get_nameslist_stars (wc);
  if (temps)
    {
      g_string_append_printf (gs, "   \342\200\242 %s", temps[0]);
      for (i = 1; temps[i]; i++)
	g_string_append_printf (gs, "; %s", temps[i]);
      g_free (temps);
    }

  on_table_status_message (chartable, gs->str, window);
  g_string_free (gs, TRUE);
}

static gboolean
on_table_focus_out_event (GtkWidget * drawing_area,
			  GdkEventFocus * event, GtrWindow * window)
{
#ifdef HAVE_GUCHARMAP_2
  GucharmapChartable *chartable;
#else
  GucharmapTable *chartable;
#endif
  WindowData *data;

  data = (WindowData *) g_object_get_data (G_OBJECT (window),
					   WINDOW_DATA_KEY);
  g_return_val_if_fail (data != NULL, FALSE);

#ifdef HAVE_GUCHARMAP_2
  chartable = gtr_charmap_panel_get_chartable
    (GTR_CHARMAP_PANEL (data->panel));
#else
  chartable = gtr_charmap_panel_get_table
    (GTR_CHARMAP_PANEL (data->panel));
#endif

  on_table_status_message (chartable, NULL, window);
  return FALSE;
}

#ifdef HAVE_GUCHARMAP_2
static void
on_table_activate (GucharmapChartable * chartable, GtrWindow * window)
#else
static void
on_table_activate (GucharmapTable * chartable,
		   gunichar wc, GtrWindow * window)
#endif
{
  GtkTextView *view;
  GtkTextBuffer *document;
  GtkTextIter start, end;
  gchar buffer[6];
  gchar length;
#ifdef HAVE_GUCHARMAP_2
  gunichar wc;

  wc = gucharmap_chartable_get_active_character (chartable);
#endif

  g_return_if_fail (gucharmap_unichar_validate (wc));

  view = GTK_TEXT_VIEW (gtr_window_get_active_view (window));

  if (!view || !gtk_text_view_get_editable (view))
    return;

  document = gtk_text_view_get_buffer (view);

  g_return_if_fail (document != NULL);

  length = g_unichar_to_utf8 (wc, buffer);

  gtk_text_buffer_begin_user_action (document);

  gtk_text_buffer_get_selection_bounds (document, &start, &end);

  gtk_text_buffer_delete_interactive (document, &start, &end, TRUE);
  if (gtk_text_iter_editable (&start, TRUE))
    gtk_text_buffer_insert (document, &start, buffer, length);

  gtk_text_buffer_end_user_action (document);
}

static GtkWidget *
create_charmap_panel (GtrWindow * window)
{
  GtkWidget *panel;
#ifdef HAVE_GUCHARMAP_2
  GucharmapChartable *chartable;
#else
  GucharmapTable *table;
#endif

  panel = gtr_charmap_panel_new ();

#ifdef HAVE_GUCHARMAP_2
  chartable =
    gtr_charmap_panel_get_chartable (GTR_CHARMAP_PANEL (panel));
#else
  table = gtr_charmap_panel_get_table (GTR_CHARMAP_PANEL (panel));
#endif

#ifdef HAVE_GUCHARMAP_2
  g_signal_connect (chartable,
		    "notify::active-character",
		    G_CALLBACK (on_table_sync_active_char), window);
  g_signal_connect (chartable,
		    "focus-out-event",
		    G_CALLBACK (on_table_focus_out_event), window);
  g_signal_connect (chartable,
		    "status-message",
		    G_CALLBACK (on_table_status_message), window);
  g_signal_connect (chartable,
		    "activate", G_CALLBACK (on_table_activate), window);

#else
  g_signal_connect (table,
		    "set-active-char",
		    G_CALLBACK (on_table_set_active_char), window);
  /* Note: GucharmapTable does not provide focus-out-event ... */
  g_signal_connect (table->drawing_area,
		    "focus-out-event",
		    G_CALLBACK (on_table_focus_out_event), window);
  g_signal_connect (table,
		    "status-message",
		    G_CALLBACK (on_table_status_message), window);
  g_signal_connect (table,
		    "activate", G_CALLBACK (on_table_activate), window);
#endif /* HAVE_GUCHARMAP_2 */

  gtk_widget_show_all (panel);

  return panel;
}

static void
impl_activate (GtrPlugin * plugin, GtrWindow * window)
{
  GtrStatusbar *statusbar;
  WindowData *data;

  data = g_new (WindowData, 1);

  gtr_application_register_icon (GTR_APP, "gucharmap.ico",
					 "charmap-plugin-icon");

  data->panel = create_charmap_panel (window);

  gtr_window_add_widget (window,
				 data->panel,
				 "GtrCharmapPlugin",
				 _("Character Map"),
				 "charmap-plugin-icon",
				 GTR_WINDOW_PLACEMENT_LEFT);

  statusbar = GTR_STATUSBAR (gtr_window_get_statusbar (window));
  data->context_id = gtr_statusbar_get_context_id (statusbar,
							   "Character Description");

  g_object_set_data_full (G_OBJECT (window),
			  WINDOW_DATA_KEY,
			  data, (GDestroyNotify) free_window_data);
}

static void
impl_deactivate (GtrPlugin * plugin, GtrWindow * window)
{
  WindowData *data;
#ifdef HAVE_GUCHARMAP_2
  GucharmapChartable *chartable;
#else
  GucharmapTable *chartable;
#endif

  data = (WindowData *) g_object_get_data (G_OBJECT (window),
					   WINDOW_DATA_KEY);
  g_return_if_fail (data != NULL);

#ifdef HAVE_GUCHARMAP_2
  chartable = gtr_charmap_panel_get_chartable
    (GTR_CHARMAP_PANEL (data->panel));
#else
  chartable = gtr_charmap_panel_get_table
    (GTR_CHARMAP_PANEL (data->panel));
#endif
  on_table_status_message (chartable, NULL, window);

  gtr_window_remove_widget (window, data->panel);

  g_object_set_data (G_OBJECT (window), WINDOW_DATA_KEY, NULL);

}

static void
gtr_charmap_plugin_class_init (GtrCharmapPluginClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtrPluginClass *plugin_class = GTR_PLUGIN_CLASS (klass);

  object_class->finalize = gtr_charmap_plugin_finalize;

  plugin_class->activate = impl_activate;
  plugin_class->deactivate = impl_deactivate;
}
