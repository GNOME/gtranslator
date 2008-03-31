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
#include <gio/gio.h>

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
insert_link (GtkTextBuffer *buffer, 
	     GtkTextIter *iter,
	     const gchar *path,
	     gint *line)
{
	GtkTextTag *tag;
	gchar *text;
	GFile  *file;
	gchar *uri;

	file = g_file_new_for_commandline_arg (path);
	uri = g_file_get_uri (file);
	g_object_unref (file);

	tag = gtk_text_buffer_create_tag (buffer, NULL, 
					  "foreground", "blue", 
					  "underline", PANGO_UNDERLINE_SINGLE, 
					  NULL);
	g_object_set_data (G_OBJECT (tag), "path", uri);
	g_object_set_data (G_OBJECT (tag), "line", line);

	text = g_strconcat (path, "\n", NULL);
	gtk_text_buffer_insert_with_tags (buffer, iter, text, -1, tag, NULL);
	g_free (text);
}

static void
show_in_editor (const gchar *path,
		gint line)
{
	gchar *open[4];

	if (g_find_program_in_path ("gedit"))
	{
		open[0] = "gedit";
	}
	else
	{
		GtkWidget *dialog;
		
		dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL,
						 GTK_DIALOG_MODAL,
						 GTK_BUTTONS_CLOSE,
						 _("Please, install gedit to be able to show the file"));
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
	}
	
	open[1] = g_strdup (path);
	open[2] = g_strdup_printf ("+%d", line);
	open[3] = NULL;
					
	gdk_spawn_on_screen (gdk_screen_get_default (),
			     NULL,
			     open,
			     NULL,
			     G_SPAWN_SEARCH_PATH,
			     NULL,
			     NULL, NULL, NULL);
	g_free (open[1]);
	g_free (open[2]);
}

static void
follow_if_link (GtkWidget   *text_view, 
		GtkTextIter *iter)
{
	GSList *tags = NULL, *tagp = NULL;

	tags = gtk_text_iter_get_tags (iter);
	for (tagp = tags;  tagp != NULL;  tagp = tagp->next)
	{
		GtkTextTag *tag = tagp->data;
		gchar *path = g_object_get_data (G_OBJECT (tag), "path");
		gint line = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (tag), "line"));
		
		show_in_editor (path, line);
	}

	if (tags)
		g_slist_free (tags);
}

static gboolean
event_after (GtkWidget *text_view,
	     GdkEvent  *ev)
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

	follow_if_link (text_view, &iter);

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
			gdk_window_set_cursor (gtk_text_view_get_window (text_view, GTK_TEXT_WINDOW_TEXT), hand_cursor);
		else
			gdk_window_set_cursor (gtk_text_view_get_window (text_view, GTK_TEXT_WINDOW_TEXT), regular_cursor);
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
	const gchar *filename = NULL;
	gint i = 0;
	gint *index = NULL;
	gchar *path = NULL;
	GSList *tags = NULL, *tagp = NULL;

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (panel->priv->extracted_comments));
	gtk_text_buffer_set_text (buffer, "", 0);
	gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
	
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
	
	gtk_text_buffer_insert (buffer, &iter, _("Paths:\n"), -1);
	
	filename = gtranslator_msg_get_filename (msg, i);
	while (filename)
	{
		index = gtranslator_msg_get_file_line (msg, i);
		insert_link (buffer, &iter, filename, index);
		i++;
		filename = gtranslator_msg_get_filename (msg, i);
	}

	format = g_strconcat (_("Format: "), gtranslator_msg_get_format (msg), NULL);
    	context = g_strconcat (_("Context:\n"), gtranslator_msg_get_msgctxt (msg), NULL);
    	extracted = g_strconcat (_("Extracted comments:\n"),
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
	 * Cursors
	 */
	hand_cursor = gdk_cursor_new (GDK_HAND2);
	regular_cursor = gdk_cursor_new (GDK_XTERM);
	
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
	g_signal_connect (priv->extracted_comments, "event-after", 
			  G_CALLBACK (event_after), NULL);
	g_signal_connect (priv->extracted_comments, "motion-notify-event", 
			  G_CALLBACK (motion_notify_event), NULL);
	g_signal_connect (priv->extracted_comments, "visibility-notify-event", 
			  G_CALLBACK (visibility_notify_event), NULL);
	
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
