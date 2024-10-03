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
#include "gtr-window.h"

#include <glib.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>

static GArray * msg_queue_arr = NULL;

typedef struct
{
  gchar *primary_text;
  gchar *secondary_text;
} message_struct;

static void show_info_bar (GtrTab * tab);

static void
msg_queue_element_clear (message_struct *msg_struct_temp)
{
  g_clear_pointer (&msg_struct_temp->primary_text, g_free);
  g_clear_pointer (&msg_struct_temp->secondary_text, g_free);
}

/*
 * Callback func called when warning button is clicked
 */
static void
handle_info_bar_response (AdwToast *toast,
                          gpointer user_data)
{
  GtrTab *tab = GTR_TAB (user_data);

  if (msg_queue_arr->len > 0)
    msg_queue_arr = g_array_remove_index (msg_queue_arr, 0);

  if (msg_queue_arr->len > 0)
    {
      show_info_bar (tab);
    }
  g_object_unref (tab);
}

static void
show_info_bar (GtrTab * tab)
{
  AdwToast *toast;
  message_struct msg_struct_temp = g_array_index (msg_queue_arr, message_struct, 0);
  const gchar * primary_text = msg_struct_temp.primary_text;
  const gchar * secondary_text = msg_struct_temp.secondary_text;

  if (secondary_text)
    toast = adw_toast_new_format ("%s: %s", primary_text, secondary_text);
  else
    toast = adw_toast_new_format ("%s", primary_text);

  adw_toast_set_timeout (toast, 0);
  // Increase reference for tab, to avoid segfault on dismissed if tab
  // is removed
  g_object_ref (tab);
  g_signal_connect (G_OBJECT (toast), "dismissed",
                    G_CALLBACK (handle_info_bar_response), tab);
  gtr_window_add_toast (GTR_WINDOW (gtr_tab_get_window (tab)), toast);
}

void
create_info_info_bar (const gchar * primary_text,
                      const gchar * secondary_text,
                      GtrTab * tab)
{
  message_struct msg_struct_temp = {
    g_strdup (primary_text),
    g_strdup (secondary_text),
  };

  if (msg_queue_arr == NULL)
    {
      msg_queue_arr = g_array_new (FALSE, FALSE, sizeof (message_struct));
      g_array_set_clear_func (msg_queue_arr,
                              (GDestroyNotify)msg_queue_element_clear);
    }

  g_array_append_val (msg_queue_arr, msg_struct_temp);
  show_info_bar (tab);
}
