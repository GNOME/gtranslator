/*
 * Copyright (C) 2005 - Paolo Borelli
 * 		 2007 - Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "statusbar.h"

#define GTR_STATUSBAR_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object),\
					  GTR_TYPE_STATUSBAR, GtranslatorStatusbarPrivate))

struct _GtranslatorStatusbarPrivate
{
	GtkWidget *statusbar;
	GtkWidget *progress_bar;
	
	GtkWidget     *overwrite_mode_label;

	/* tmp flash timeout data */
	guint          flash_timeout;
	guint          flash_context_id;
	guint          flash_message_id;
};

G_DEFINE_TYPE(GtranslatorStatusbar, gtranslator_statusbar, GTK_TYPE_HBOX)

static void
gtranslator_statusbar_finalize (GObject *object)
{
	GtranslatorStatusbar *statusbar = GTR_STATUSBAR (object);

	if (statusbar->priv->flash_timeout > 0)
		g_source_remove (statusbar->priv->flash_timeout);

	G_OBJECT_CLASS (gtranslator_statusbar_parent_class)->finalize (object);
}

static void
gtranslator_statusbar_class_init (GtranslatorStatusbarClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = gtranslator_statusbar_finalize;

	g_type_class_add_private (object_class, sizeof (GtranslatorStatusbarPrivate));
}

static void
gtranslator_statusbar_init (GtranslatorStatusbar *statusbar)
{
	GtkWidget *frame;
	GtkShadowType shadow_type;

	statusbar->priv = GTR_STATUSBAR_GET_PRIVATE (statusbar);
	
	/*
	 * Statusbar
	 */
	statusbar->priv->statusbar = gtk_statusbar_new ();
	gtk_widget_show (statusbar->priv->statusbar);
	gtk_box_pack_end (GTK_BOX (statusbar), statusbar->priv->statusbar,
			  TRUE, TRUE, 0);
	
	/*
	 * Progress bar
	 */
	statusbar->priv->progress_bar = gtk_progress_bar_new ();
	gtk_widget_show (statusbar->priv->progress_bar);
	gtk_box_pack_start (GTK_BOX (statusbar), statusbar->priv->progress_bar,
			    FALSE, FALSE, 0);
	
	/*
	 * Ins/Ovr stuff
	 */
	gtk_widget_style_get (GTK_WIDGET (statusbar->priv->statusbar),
			      "shadow-type", &shadow_type, NULL);
	
	frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), shadow_type);
	gtk_widget_show (frame);
	
	statusbar->priv->overwrite_mode_label = gtk_label_new ("");
	gtk_label_set_single_line_mode (GTK_LABEL (statusbar->priv->overwrite_mode_label), TRUE);
	gtk_misc_set_alignment (GTK_MISC (statusbar->priv->overwrite_mode_label), 0.0, 0.5);
	gtk_label_set_width_chars (GTK_LABEL (statusbar->priv->overwrite_mode_label),
				  MAX (g_utf8_strlen(_("INS"), -1)+1, g_utf8_strlen (_("OVR"), -1)+1));
						    
	
	gtk_container_add (GTK_CONTAINER (frame), statusbar->priv->overwrite_mode_label);
	gtk_widget_show (statusbar->priv->overwrite_mode_label);
	
	gtk_box_pack_start (GTK_BOX (statusbar->priv->statusbar), frame,
			    FALSE, FALSE, 0);
}

/**
 * gtranslator_statusbar_new:
 *
 * Creates a new #GtranslatorStatusbar.
 *
 * Return value: the new #GtranslatorStatusbar object
 **/
GtkWidget *
gtranslator_statusbar_new (void)
{
	return GTK_WIDGET (g_object_new (GTR_TYPE_STATUSBAR, NULL));
}

guint
gtranslator_statusbar_push (GtranslatorStatusbar *statusbar,
			    guint context_id,
			    const gchar *text)
{
	g_return_if_fail (GTR_IS_STATUSBAR (statusbar));
	
	return gtk_statusbar_push (GTK_STATUSBAR (statusbar->priv->statusbar),
				   context_id, text);
}

void
gtranslator_statusbar_pop (GtranslatorStatusbar *statusbar,
			   guint context_id)
{
	g_return_if_fail (GTR_IS_STATUSBAR (statusbar));
	
	gtk_statusbar_pop (GTK_STATUSBAR (statusbar->priv->statusbar),
			   context_id);
}

/**
 * gtranslator_statusbar_set_overwrite:
 * @statusbar: a #GtranslatorStatusbar
 * @overwrite: if the overwrite mode is set
 *
 * Sets the overwrite mode on the statusbar.
 **/
void
gtranslator_statusbar_set_overwrite (GtranslatorStatusbar *statusbar,
				     gboolean        overwrite)
{
	g_return_if_fail (GTR_IS_STATUSBAR (statusbar));

	if (overwrite)
		gtk_label_set_text(GTK_LABEL(statusbar->priv->overwrite_mode_label), _("OVR"));
	else gtk_label_set_text(GTK_LABEL(statusbar->priv->overwrite_mode_label), _("INS"));
}

void
gtranslator_statusbar_clear_overwrite (GtranslatorStatusbar *statusbar)
{
	g_return_if_fail (GTR_IS_STATUSBAR (statusbar));

	gtk_label_set_text(GTK_LABEL(statusbar->priv->overwrite_mode_label), "");
}

static gboolean
remove_message_timeout (GtranslatorStatusbar *statusbar)
{
	gtk_statusbar_remove (GTK_STATUSBAR (statusbar->priv->statusbar),
			      statusbar->priv->flash_context_id,
			      statusbar->priv->flash_message_id);

	/* remove the timeout */
	statusbar->priv->flash_timeout = 0;
  	return FALSE;
}

/**
 * gtranslator_statusbar_flash_message:
 * @statusbar: a #GtranslatorStatusbar
 * @context_id: message context_id
 * @format: message to flash on the statusbar
 *
 * Flash a temporary message on the statusbar.
 */
void
gtranslator_statusbar_flash_message (GtranslatorStatusbar *statusbar,
				     guint           context_id,
				     const gchar    *format, ...)
{
	const guint32 flash_length = 3000; /* three seconds */
	va_list args;
	gchar *msg;

	g_return_if_fail (GTR_IS_STATUSBAR (statusbar));
	g_return_if_fail (format != NULL);

	va_start (args, format);
	msg = g_strdup_vprintf (format, args);
	va_end (args);

	/* remove a currently ongoing flash message */
	if (statusbar->priv->flash_timeout > 0)
	{
		g_source_remove (statusbar->priv->flash_timeout);
		statusbar->priv->flash_timeout = 0;

		gtk_statusbar_remove (GTK_STATUSBAR (statusbar),
				      statusbar->priv->flash_context_id,
				      statusbar->priv->flash_message_id);
	}

	statusbar->priv->flash_context_id = context_id;
	statusbar->priv->flash_message_id = gtk_statusbar_push (GTK_STATUSBAR (statusbar->priv->statusbar),
								context_id,
								msg);

	statusbar->priv->flash_timeout = g_timeout_add (flash_length,
							(GtkFunction) remove_message_timeout,
							statusbar);

	g_free (msg);
}

/*
 * Update the progress bar
 */
void
gtranslator_statusbar_update_progress_bar (GtranslatorStatusbar *statusbar,
					   gdouble translated_count,
					   gdouble messages_count)
{
	gdouble percentage;
	
	g_return_if_fail (GTR_IS_STATUSBAR (statusbar));
	
	/*
	 * Calculate the percentage.
	 */
	percentage = translated_count / messages_count;
	
	/*
	 * Set the progress only if the values are reasonable.
	 */
	if(percentage > 0.0 || percentage < 1.0)
	{
		/*
		 * Set the progressbar status.
		 */
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (statusbar->priv->progress_bar),
					       percentage);
	}
}
