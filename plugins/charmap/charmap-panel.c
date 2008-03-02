/*
 * gtranslator-charmap-panel.c
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

#include "plugin.h"
#include "charmap-panel.h"
#include <gucharmap/gucharmap-script-chapters.h>

#define GTR_CHARMAP_PANEL_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),		       \
						 GTR_TYPE_CHARMAP_PANEL,     \
						 GtranslatorCharmapPanelPrivate))

struct _GtranslatorCharmapPanelPrivate
{
	GtkWidget	*table;
	GtkWidget	*chapters;
};

GTR_PLUGIN_DEFINE_TYPE(GtranslatorCharmapPanel, gtranslator_charmap_panel, GTK_TYPE_VBOX)

static void
on_chapter_changed (GucharmapChapters *chapters,
		    GtranslatorCharmapPanel *panel)
{
	gucharmap_table_set_codepoint_list (GUCHARMAP_TABLE (panel->priv->table),
					    gucharmap_chapters_get_codepoint_list (chapters));
}

static void
gtranslator_charmap_panel_init (GtranslatorCharmapPanel *panel)
{
	GucharmapCodepointList *codepoint_list;
	GtkPaned *paned;
	
	panel->priv = GTR_CHARMAP_PANEL_GET_PRIVATE (panel);
	
	panel->priv->chapters = gucharmap_script_chapters_new ();
	
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (GUCHARMAP_CHAPTERS (panel->priv->chapters)->tree_view),
					   FALSE);

	codepoint_list = gucharmap_chapters_get_codepoint_list 
				(GUCHARMAP_CHAPTERS (panel->priv->chapters));
	
	panel->priv->table = gucharmap_table_new ();

	gucharmap_table_set_codepoint_list (GUCHARMAP_TABLE (panel->priv->table),
					    codepoint_list);
	
	paned = GTK_PANED (gtk_vpaned_new ());
	gtk_paned_pack1 (paned, panel->priv->chapters, FALSE, TRUE);
	gtk_paned_pack2 (paned, panel->priv->table, TRUE, TRUE);
	gtk_paned_set_position (paned, 150);
	
	gtk_box_pack_start (GTK_BOX (panel), GTK_WIDGET (paned), TRUE, TRUE, 0);

	g_signal_connect (panel->priv->chapters,
			  "changed",
			  G_CALLBACK (on_chapter_changed),
			  panel);
}

static void
gtranslator_charmap_panel_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_charmap_panel_parent_class)->finalize (object);
}

static void
gtranslator_charmap_panel_class_init (GtranslatorCharmapPanelClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorCharmapPanelPrivate));

	object_class->finalize = gtranslator_charmap_panel_finalize;
}

GtkWidget *
gtranslator_charmap_panel_new (void)
{
	return GTK_WIDGET (g_object_new (GTR_TYPE_CHARMAP_PANEL, NULL));
}

GucharmapTable *
gtranslator_charmap_panel_get_table (GtranslatorCharmapPanel *panel)
{
	return GUCHARMAP_TABLE (panel->priv->table);
}

