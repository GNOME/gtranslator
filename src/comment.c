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

#include "comment.h"
#include "tab.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define GTR_COMMENT_PANEL_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),		       \
						 GTR_TYPE_COMMENT_PANEL,     \
						 GtranslatorCommentPanelPrivate))

G_DEFINE_TYPE(GtranslatorCommentPanel, gtranslator_comment_panel, GTK_TYPE_VBOX)


struct _GtranslatorCommentPanelPrivate
{
	GtkWidget *extracted_comments;
	GtkWidget *comments;

	GtranslatorTab *tab;
};

static void
showed_message_cb (GtranslatorTab *tab,
		   GtranslatorMsg *msg,
		   GtranslatorCommentPanel *panel)
{
	GtkTextBuffer *buffer;
	GtkTextIter iter;
	gchar *extracted;
	gchar *context;
	gchar *format;
    	gchar *toset;

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (panel->priv->extracted_comments));
	gtk_text_buffer_set_text (buffer, "", 0);
	gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);

	format = g_strconcat (_("Format: "), gtranslator_msg_get_format (msg), NULL);
    	context = g_strconcat (_("Context:"), gtranslator_msg_get_msgctxt (msg), NULL);
    	extracted = g_strconcat (_("Extracted comments:"),
				 gtranslator_msg_get_extracted_comments(msg), NULL);
    
   	toset = g_strdup_printf("%s\n%s\n%s", format, context, extracted);
    
    	g_free (format);
    	g_free (context);
    	g_free (extracted);

	gtk_text_buffer_insert (buffer, &iter, toset, -1);
    
    	g_free (toset);

	gtranslator_comment_panel_set_comments (panel,
					        gtranslator_msg_get_comment(msg));
}

static void
gtranslator_comment_panel_draw(GtranslatorCommentPanel *panel)
{
	GtranslatorCommentPanelPrivate *priv = panel->priv;
	GtkWidget *comments_scrolled_window;
	
	/*
	 * Set up the scrolling window for the extracted comments display
	 */	
	comments_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(comments_scrolled_window),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(comments_scrolled_window),
					    GTK_SHADOW_IN);
	gtk_box_pack_start(GTK_BOX(panel), comments_scrolled_window, TRUE, TRUE, 0);
	gtk_widget_show (comments_scrolled_window);

	/*
	 * Extracted comments
	 */	
	priv->extracted_comments = gtk_text_view_new();
	
	gtk_text_view_set_editable(GTK_TEXT_VIEW(priv->extracted_comments), FALSE);
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (priv->extracted_comments),
				     GTK_WRAP_WORD);
	gtk_container_add(GTK_CONTAINER(comments_scrolled_window), GTK_WIDGET(priv->extracted_comments));
	gtk_widget_show (priv->extracted_comments);

	/*
	 * Set up the scrolling window for the extracted comments display
	 */	
	comments_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(comments_scrolled_window),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(comments_scrolled_window),
					    GTK_SHADOW_IN);
	gtk_box_pack_start(GTK_BOX(panel), comments_scrolled_window, TRUE, TRUE, 0);
	gtk_widget_show (comments_scrolled_window);

	/*
	 * Comments
	 */
	priv->comments = gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(priv->comments), FALSE);
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (priv->comments),
				     GTK_WRAP_WORD);
	gtk_container_add(GTK_CONTAINER(comments_scrolled_window),
			  GTK_WIDGET(priv->comments));
	gtk_widget_show (priv->comments);
}


static void
gtranslator_comment_panel_init (GtranslatorCommentPanel *panel)
{
	panel->priv = GTR_COMMENT_PANEL_GET_PRIVATE (panel);
	
	gtranslator_comment_panel_draw(panel);
}

static void
gtranslator_comment_panel_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_comment_panel_parent_class)->finalize (object);
}

static void
gtranslator_comment_panel_class_init (GtranslatorCommentPanelClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorCommentPanelPrivate));

	object_class->finalize = gtranslator_comment_panel_finalize;
}

GtkWidget *
gtranslator_comment_panel_new (GtkWidget *tab)
{
	GtranslatorCommentPanel *comment;
	comment = g_object_new (GTR_TYPE_COMMENT_PANEL, NULL);
	
	comment->priv->tab = GTR_TAB(tab);
	g_signal_connect(tab,
			 "showed-message",
			 G_CALLBACK(showed_message_cb),
			 comment);
	
	return GTK_WIDGET(comment);
}

void
gtranslator_comment_panel_set_comments(GtranslatorCommentPanel *panel,
				       const gchar *comments)
{
	GtkTextBuffer *buf;

	buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->priv->comments));
	
	gtk_text_buffer_set_text(buf, comments, -1);
}

GtkTextView *
gtranslator_comment_panel_get_extracted_text_view (GtranslatorCommentPanel *panel)
{
	g_return_if_fail (GTR_IS_COMMENT_PANEL (panel));
	
	return GTK_TEXT_VIEW (panel->priv->extracted_comments);
}
							    
