/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * 		 2005  Paolo Maggi 
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANMSGILITY or FITNESS FOR A PARTICULAR PURMSGSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "gtr-io-error-info-bar.h"

#include <glib.h>
#include <gtk/gtk.h>

static void
set_contents (GtkInfoBar * infobar, GtkWidget * contents)
{
  GtkWidget *content_area;

  content_area = gtk_info_bar_get_content_area (infobar);
  gtk_container_add (GTK_CONTAINER (content_area), contents);
}

/*
 * Message area funcs
 */
static void
set_info_bar_text_and_icon (GtkInfoBar * infobar,
                            const gchar * icon_stock_id,
                            const gchar * primary_text,
                            const gchar * secondary_text)
{
  GtkWidget *hbox_content;
  GtkWidget *image;
  GtkWidget *vbox;
  gchar *primary_markup;
  gchar *secondary_markup;
  GtkWidget *primary_label;
  GtkWidget *secondary_label;

  hbox_content = gtk_hbox_new (FALSE, 8);
  gtk_widget_show (hbox_content);

  image = gtk_image_new_from_stock (icon_stock_id, GTK_ICON_SIZE_DIALOG);
  gtk_widget_show (image);
  gtk_box_pack_start (GTK_BOX (hbox_content), image, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (image), 0.5, 0);

  vbox = gtk_vbox_new (FALSE, 6);
  gtk_widget_show (vbox);
  gtk_box_pack_start (GTK_BOX (hbox_content), vbox, TRUE, TRUE, 0);

  primary_markup = g_strdup_printf ("<b>%s</b>", primary_text);
  primary_label = gtk_label_new (primary_markup);
  g_free (primary_markup);

  gtk_widget_show (primary_label);

  gtk_box_pack_start (GTK_BOX (vbox), primary_label, TRUE, TRUE, 0);
  gtk_label_set_use_markup (GTK_LABEL (primary_label), TRUE);
  gtk_label_set_line_wrap (GTK_LABEL (primary_label), FALSE);
  gtk_misc_set_alignment (GTK_MISC (primary_label), 0, 0.5);

  GTK_WIDGET_SET_FLAGS (primary_label, GTK_CAN_FOCUS);

  gtk_label_set_selectable (GTK_LABEL (primary_label), TRUE);

  if (secondary_text != NULL)
    {
      secondary_markup = g_strdup_printf ("<small>%s</small>",
                                          secondary_text);
      secondary_label = gtk_label_new (secondary_markup);
      g_free (secondary_markup);

      gtk_widget_show (secondary_label);

      gtk_box_pack_start (GTK_BOX (vbox), secondary_label, TRUE, TRUE, 0);

      GTK_WIDGET_SET_FLAGS (secondary_label, GTK_CAN_FOCUS);

      gtk_label_set_use_markup (GTK_LABEL (secondary_label), TRUE);
      gtk_label_set_line_wrap (GTK_LABEL (secondary_label), TRUE);
      gtk_label_set_selectable (GTK_LABEL (secondary_label), TRUE);
      gtk_misc_set_alignment (GTK_MISC (secondary_label), 0, 0.5);
    }

  set_contents (infobar, hbox_content);
}

/*
 * Callback func called when warning button is clicked
 */
void
warning_message_button_clicked (GtkWidget * widget,
                                gint response_id, gpointer data)
{
  if (response_id == GTK_RESPONSE_CLOSE)
    {
      gtk_widget_hide (GTK_WIDGET (widget));
    }
}

GtkWidget *
create_error_info_bar (const gchar * primary_text,
                       const gchar * secondary_text)
{
  GtkWidget *infobar;

  infobar = gtk_info_bar_new_with_buttons (GTK_STOCK_CLOSE,
                                           GTK_RESPONSE_CLOSE, NULL);

  gtk_info_bar_set_message_type (GTK_INFO_BAR (infobar), GTK_MESSAGE_WARNING);

  set_info_bar_text_and_icon (GTK_INFO_BAR (infobar),
                              GTK_STOCK_DIALOG_ERROR,
                              primary_text, secondary_text);

  g_signal_connect (G_OBJECT (infobar), "response",
                    G_CALLBACK (warning_message_button_clicked), NULL);

  gtk_widget_show (infobar);

  return infobar;
}
