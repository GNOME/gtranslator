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
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "gtr-io-error-info-bar.h"
#include "gtr-utils.h"
#include "gtr-tab.h"

#include <glib.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>

static GArray * msg_queue_arr = NULL;

enum msg_type
{
  INFO_MSG,
  ERROR_MSG
};

typedef struct
{
  const gchar * primary_text;
  const gchar * secondary_text;
  enum msg_type message_type;
} message_struct;

static void show_info_bar (GtrTab * tab);

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

  hbox_content = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_widget_show (hbox_content);

  image = gtk_image_new_from_icon_name (icon_stock_id, GTK_ICON_SIZE_DIALOG);
  gtk_widget_show (image);
  gtk_box_append (GTK_BOX (hbox_content), image);
  gtk_widget_set_halign (image, 0.5);
  gtk_widget_set_valign (image, 0.0);

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
  gtk_widget_show (vbox);
  gtk_widget_set_hexpand (vbox, TRUE);
  gtk_box_append (GTK_BOX (hbox_content), vbox);

  primary_markup = g_strdup_printf ("<b>%s</b>", primary_text);
  primary_label = gtk_label_new (primary_markup);
  g_free (primary_markup);

  gtk_widget_show (primary_label);

  gtk_widget_set_vexpand (primary_label, TRUE);
  gtk_box_append (GTK_BOX (vbox), primary_label);
  gtk_label_set_use_markup (GTK_LABEL (primary_label), TRUE);
  gtk_label_set_line_wrap (GTK_LABEL (primary_label), FALSE);
  gtk_label_set_xalign (GTK_LABEL (primary_label), 0.0);
  gtk_label_set_yalign (GTK_LABEL (primary_label), 0.5);

  gtk_widget_set_can_focus (primary_label, TRUE);

  gtk_label_set_selectable (GTK_LABEL (primary_label), TRUE);

  if (secondary_text != NULL)
    {
      secondary_markup = g_strdup_printf ("<small>%s</small>",
                                          secondary_text);
      secondary_label = gtk_label_new (secondary_markup);
      g_free (secondary_markup);

      gtk_widget_show (secondary_label);

      gtk_widget_set_vexpand (secondary_label, TRUE);
      gtk_box_append (GTK_BOX (vbox), secondary_label);

      gtk_widget_set_can_focus (secondary_label, TRUE);

      gtk_label_set_use_markup (GTK_LABEL (secondary_label), TRUE);
      gtk_label_set_line_wrap (GTK_LABEL (secondary_label), TRUE);
      gtk_label_set_selectable (GTK_LABEL (secondary_label), TRUE);
      gtk_label_set_xalign (GTK_LABEL (secondary_label), 0.0);
      gtk_label_set_yalign (GTK_LABEL (secondary_label), 0.5);
    }

  set_contents (infobar, hbox_content);
}

/*
 * Callback func called when warning button is clicked
 */
static void
handle_info_bar_response (GtkWidget * infobar,
                          gint response_id,
                          GtrTab * tab)
{
  if (response_id == GTK_RESPONSE_OK || response_id == GTK_RESPONSE_CLOSE)
  {
    gtk_widget_hide (infobar);
    message_struct msg_struct_temp = g_array_index (msg_queue_arr, message_struct, 0);
    g_free ((gchar *)msg_struct_temp.primary_text);
    g_free ((gchar *)msg_struct_temp.secondary_text);
    msg_queue_arr = g_array_remove_index (msg_queue_arr, 0);

    if (msg_queue_arr->len > 0)
    {
      show_info_bar (tab);
    }
  }
}

static void
show_info_bar (GtrTab * tab)
{
  GtkWidget *infobar;
  message_struct msg_struct_temp = g_array_index (msg_queue_arr, message_struct, 0);
  const gchar * primary_text = msg_struct_temp.primary_text;
  const gchar * secondary_text = msg_struct_temp.secondary_text;
  enum msg_type message_type = msg_struct_temp.message_type;
  gchar * icon_stock_id = NULL;

  if (message_type == INFO_MSG)
  {
    infobar = gtk_info_bar_new_with_buttons (_("_OK"),
                                             GTK_RESPONSE_OK, NULL);

    gtk_info_bar_set_message_type (GTK_INFO_BAR (infobar), GTK_MESSAGE_INFO);

    icon_stock_id = "dialog-information-symbolic";
  }
  else
  {
    infobar = gtk_info_bar_new_with_buttons (_("_Close"),
                                             GTK_RESPONSE_CLOSE, NULL);

    gtk_info_bar_set_message_type (GTK_INFO_BAR (infobar), GTK_MESSAGE_WARNING);

    icon_stock_id = "dialog-error-symbolic";
  }

  set_info_bar_text_and_icon (GTK_INFO_BAR (infobar),
                              icon_stock_id,
                              primary_text, secondary_text);

  g_signal_connect (G_OBJECT (infobar), "response",
                    G_CALLBACK (handle_info_bar_response), tab);

  gtk_widget_show (infobar);

  gtr_tab_set_info_bar (tab, infobar);
}

void
create_error_info_bar (const gchar * primary_text,
                       const gchar * secondary_text,
                       GtrTab * tab)
{
  message_struct msg_struct_temp = {
    g_strdup (primary_text),
    g_strdup (secondary_text),
    ERROR_MSG
  };

  if (msg_queue_arr == NULL)
    msg_queue_arr = g_array_new (FALSE, FALSE, sizeof (message_struct));

  g_array_append_val (msg_queue_arr, msg_struct_temp);
  show_info_bar (tab);
}

void
create_info_info_bar (const gchar * primary_text,
                      const gchar * secondary_text,
                      GtrTab * tab)
{
  message_struct msg_struct_temp = {
    g_strdup(primary_text),
    g_strdup(secondary_text),
    INFO_MSG
  };

  if (msg_queue_arr == NULL)
    msg_queue_arr = g_array_new (FALSE, FALSE, sizeof (message_struct));

  g_array_append_val (msg_queue_arr, msg_struct_temp);
  show_info_bar (tab);
}
