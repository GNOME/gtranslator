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

#define GTR_STATUSBAR_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GTR_TYPE_STATUSBAR, GtranslatorStatusbarPrivate))

struct _GtranslatorStatusbarPrivate
{
	GtkWidget     *overwrite_mode_label;

	/* tmp flash timeout data */
	guint          flash_timeout;
	guint          flash_context_id;
	guint          flash_message_id;
};

G_DEFINE_TYPE(GtranslatorStatusbar, gtranslator_statusbar, GTK_TYPE_STATUSBAR)

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
	
	gtk_widget_style_get (GTK_WIDGET (statusbar), "shadow-type", &shadow_type, NULL);
	
	frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME(frame), shadow_type);
	gtk_widget_show (frame);
	
	statusbar->priv->overwrite_mode_label = gtk_label_new ("");
	gtk_label_set_single_line_mode (GTK_LABEL (statusbar->priv->overwrite_mode_label), TRUE);
	gtk_misc_set_alignment (GTK_MISC (statusbar->priv->overwrite_mode_label), 0.0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(statusbar->priv->overwrite_mode_label),
				  MAX(g_utf8_strlen(_("INS"), -1)+1, g_utf8_strlen(_("OVR"), -1)+1));
						    
	
	gtk_container_add (GTK_CONTAINER (frame), statusbar->priv->overwrite_mode_label);
	gtk_widget_show (statusbar->priv->overwrite_mode_label);
	
	gtk_box_pack_start(GTK_BOX(statusbar), frame, FALSE, FALSE, 0);
	
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
	gtk_statusbar_remove (GTK_STATUSBAR (statusbar),
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
	statusbar->priv->flash_message_id = gtk_statusbar_push (GTK_STATUSBAR (statusbar),
								context_id,
								msg);

	statusbar->priv->flash_timeout = g_timeout_add (flash_length,
							(GtkFunction) remove_message_timeout,
							statusbar);

	g_free (msg);
}
