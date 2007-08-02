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
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "view.h"

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksourcetag.h>
#include <gtksourceview/gtksourcetagtable.h>

#define GTR_VIEW_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 	(object),	\
						 	GTR_TYPE_VIEW,     \
						 	GtranslatorViewPrivate))

G_DEFINE_TYPE(GtranslatorView, gtranslator_view, GTK_TYPE_SOURCE_VIEW)

struct _GtranslatorViewPrivate
{
	GtkSourceTagTable *table;
	GtkSourceBuffer *buffer;
	GSList		*tags;
};


static void
setup_all_tags(GtranslatorViewPrivate *priv)
{
	GtkTextTag *tag;
	
	
	//		"</[A-Za-z0-9\\n]+>",
	//"<[A-Za-z0-9\\n]+[^>]*/?>",
	/*"http:\\/\\/[a-zA-Z0-9\\.\\-_/~]+",
	"mailto:[a-z0-9\\.\\-_]+@[a-z0-9\\.\\-_]+",
	"<?[a-z0-9\\.\\-_]+@[a-z0-9\\.\\-_]+>?",
	"&[a-z,A-Z,\\-,0-9,#\\.]*;"*/

	
	/*
	 * Now is neccessary create the tags and add them to the table
	 */
	tag = gtk_pattern_tag_new("args-def", "arguments",
				  "%([0-9]+(\\$))?[-+'#0]?[0-9]*(.[0-9]+)?[hlL]?[dioxXucsfeEgGp]");
	
	g_object_set(G_OBJECT(tag), "foreground", "blue", NULL);
	
	priv->tags = g_slist_append(priv->tags, tag);
	
	
	tag = gtk_pattern_tag_new("tags-def", "tags",
				  "</[A-Za-z0-9\\n]+>");
	
	g_object_set(G_OBJECT(tag), "foreground", "green", NULL);
	
	priv->tags = g_slist_append(priv->tags, tag);
	
	tag = gtk_pattern_tag_new("tags2-def", "tags2",
				  "<[A-Za-z0-9\\n]+[^>]*/?>");
	
	g_object_set(G_OBJECT(tag), "foreground", "green", NULL);
	
	priv->tags = g_slist_append(priv->tags, tag);
	
	/*tag = gtk_pattern_tag_new("url-def", "url",
				  "&[a-z,A-Z,\\-,0-9,#\\.]*;");
				  
	g_object_set(G_OBJECT(tag), "foreground", "red", NULL);
	
	priv->tags = g_slist_append(priv->tags, tag);*/
}
	       
static void
gtranslator_view_init (GtranslatorView *view)
{
	
	GtkSourceTagStyle *style;
	
	view->priv = GTR_VIEW_GET_PRIVATE (view);
	
	GtranslatorViewPrivate *priv = view->priv;
	
	priv->table = gtk_source_tag_table_new();
	
	setup_all_tags(priv);
	
	gtk_source_tag_table_add_tags(priv->table, priv->tags);
	
	priv->buffer = gtk_source_buffer_new(priv->table);
	gtk_source_buffer_set_highlight(priv->buffer, TRUE);
	
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(view), GTK_TEXT_BUFFER(priv->buffer));
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD);
/*	gtk_source_view_set_show_line_markers(GTK_SOURCE_VIEW(view), TRUE);
	gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view), TRUE);*/
}

static void
gtranslator_view_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_view_parent_class)->finalize (object);
}

static void
gtranslator_view_class_init (GtranslatorViewClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorViewPrivate));

	object_class->finalize = gtranslator_view_finalize;
}

GtkWidget *
gtranslator_view_new (void)
{
	GtkWidget *view;
	
	view = GTK_WIDGET (g_object_new (GTR_TYPE_VIEW, NULL));
	gtk_widget_show_all(view);
	return view;
}

