/*
 * gtr-charmap-panel.c
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

#include "gtr-charmap-panel.h"

#include <gucharmap/gucharmap.h>

#define GTR_CHARMAP_PANEL_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),		       \
						 GTR_TYPE_CHARMAP_PANEL,     \
						 GtrCharmapPanelPrivate))

struct _GtrCharmapPanelPrivate
{
  GucharmapChaptersView *chapters_view;
  GucharmapChartable *chartable;
};

G_DEFINE_DYNAMIC_TYPE (GtrCharmapPanel, gtr_charmap_panel, GTK_TYPE_VBOX)

static void
on_chapter_view_selection_changed (GtkTreeSelection *selection,
                                   GtrCharmapPanel  *panel)
{
  GtrCharmapPanelPrivate *priv = panel->priv;
  GucharmapCodepointList *codepoint_list;
  GtkTreeIter iter;

  if (!gtk_tree_selection_get_selected (selection, NULL, &iter))
    return;

  codepoint_list = gucharmap_chapters_view_get_codepoint_list (priv->chapters_view);
  gucharmap_chartable_set_codepoint_list (priv->chartable, codepoint_list);
  g_object_unref (codepoint_list);
}

static void
gtr_charmap_panel_init (GtrCharmapPanel *panel)
{
  GtrCharmapPanelPrivate *priv;
  GtkPaned *paned;
  GtkWidget *scrolled_window, *view, *chartable;
  GtkTreeSelection *selection;
  GucharmapChaptersModel *model;

  priv = panel->priv = GTR_CHARMAP_PANEL_GET_PRIVATE (panel);

  paned = GTK_PANED (gtk_paned_new (GTK_ORIENTATION_VERTICAL));

  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                  GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolled_window),
                                       GTK_SHADOW_ETCHED_IN);

  view = gucharmap_chapters_view_new ();
  priv->chapters_view = GUCHARMAP_CHAPTERS_VIEW (view);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (view), FALSE);

  model = gucharmap_script_chapters_model_new ();
  gucharmap_chapters_view_set_model (priv->chapters_view, model);
  g_object_unref (model);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (view));
  g_signal_connect (selection, "changed",
                    G_CALLBACK (on_chapter_view_selection_changed), panel);

  gtk_container_add (GTK_CONTAINER (scrolled_window), view);
  gtk_widget_show (view);

  gtk_paned_pack1 (paned, scrolled_window, FALSE, TRUE);
  gtk_widget_show (scrolled_window);

  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                  GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolled_window),
                                       GTK_SHADOW_ETCHED_IN);

  chartable = gucharmap_chartable_new ();
  priv->chartable = GUCHARMAP_CHARTABLE (chartable);
  gtk_container_add (GTK_CONTAINER (scrolled_window), chartable);
  gtk_widget_show (chartable);

  gtk_paned_pack2 (paned, scrolled_window, TRUE, TRUE);
  gtk_widget_show (scrolled_window);

  gucharmap_chapters_view_select_locale (priv->chapters_view);

  gtk_paned_set_position (paned, 150);

  gtk_box_pack_start (GTK_BOX (panel), GTK_WIDGET (paned), TRUE, TRUE, 0);
}

static void
gtr_charmap_panel_finalize (GObject * object)
{
  G_OBJECT_CLASS (gtr_charmap_panel_parent_class)->finalize (object);
}

static void
gtr_charmap_panel_class_init (GtrCharmapPanelClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrCharmapPanelPrivate));

  object_class->finalize = gtr_charmap_panel_finalize;
}

static void
gtr_charmap_panel_class_finalize (GtrCharmapPanelClass *klass)
{
}

GtkWidget *
gtr_charmap_panel_new (void)
{
  return GTK_WIDGET (g_object_new (GTR_TYPE_CHARMAP_PANEL, NULL));
}

GucharmapChartable *
gtr_charmap_panel_get_chartable (GtrCharmapPanel * panel)
{
  return panel->priv->chartable;
}

void
_gtr_charmap_panel_register_type (GTypeModule * type_module)
{
  gtr_charmap_panel_register_type (type_module);
}
