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

#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gi18n.h>

#include "gtr-header-dialog.h"
#include "gtr-actions.h"
#include "gtr-msg.h"
#include "gtr-po.h"
#include "gtr-tab.h"
#include "gtr-window.h"

void
gtr_actions_edit_undo (GtkAction * action, GtrWindow * window)
{
  GtrView *active_view;
  GtkSourceBuffer *active_document;

  active_view = gtr_window_get_active_view (window);
  g_return_if_fail (active_view);

  active_document =
    GTK_SOURCE_BUFFER (gtk_text_view_get_buffer
                       (GTK_TEXT_VIEW (active_view)));

  gtk_text_buffer_begin_user_action (GTK_TEXT_BUFFER (active_document));
  gtk_source_buffer_undo (active_document);
  gtk_text_buffer_end_user_action (GTK_TEXT_BUFFER (active_document));

  gtk_widget_grab_focus (GTK_WIDGET (active_view));
}

void
gtr_actions_edit_redo (GtkAction * action, GtrWindow * window)
{
  GtrView *active_view;
  GtkSourceBuffer *active_document;

  active_view = gtr_window_get_active_view (window);
  g_return_if_fail (active_view);

  active_document =
    GTK_SOURCE_BUFFER (gtk_text_view_get_buffer
                       (GTK_TEXT_VIEW (active_view)));

  gtk_text_buffer_begin_user_action (GTK_TEXT_BUFFER (active_document));
  gtk_source_buffer_redo (active_document);
  gtk_text_buffer_end_user_action (GTK_TEXT_BUFFER (active_document));

  gtk_widget_grab_focus (GTK_WIDGET (active_view));
}

void
gtr_actions_edit_cut (GtkAction * action, GtrWindow * window)
{
  GtrView *active_view;

  active_view = gtr_window_get_active_view (window);
  g_return_if_fail (active_view);

  gtr_view_cut_clipboard (active_view);

  gtk_widget_grab_focus (GTK_WIDGET (active_view));
}

void
gtr_actions_edit_copy (GtkAction * action, GtrWindow * window)
{
  GtrView *active_view;

  active_view = gtr_window_get_active_view (window);
  g_return_if_fail (active_view);

  gtr_view_copy_clipboard (active_view);

  gtk_widget_grab_focus (GTK_WIDGET (active_view));
}

void
gtr_actions_edit_paste (GtkAction * action, GtrWindow * window)
{
  GtrView *active_view;

  active_view = gtr_window_get_active_view (window);
  g_return_if_fail (active_view);

  gtr_view_paste_clipboard (active_view);

  gtk_widget_grab_focus (GTK_WIDGET (active_view));
}


/*
 * Use the untranslated message as the translation.
 */
void
gtr_message_copy_to_translation (GtkAction * action, GtrWindow * window)
{
  GtrTab *current;

  current = gtr_window_get_active_tab (window);

  gtr_tab_copy_to_translation (current);
}

/*
 * Toggle the sticky status
 */
void
gtr_message_status_toggle_fuzzy (GtkAction * action, GtrWindow * window)
{
  GtrTab *current;
  GtrPo *po;
  GList *msg;

  current = gtr_window_get_active_tab (window);
  po = gtr_tab_get_po (current);
  msg = gtr_po_get_current_message (po);

  if (gtr_msg_is_fuzzy (msg->data))
    gtr_msg_set_fuzzy (msg->data, FALSE);
  else
    gtr_msg_set_fuzzy (msg->data, TRUE);

  /*
   * Emit that message was changed.
   */
  g_signal_emit_by_name (current, "message_changed", msg->data);
}

void
gtr_actions_edit_header (GtkAction * action, GtrWindow * window)
{
  gtr_show_header_dialog (window);
}

void
gtr_actions_edit_clear (GtkAction * action, GtrWindow * window)
{
  GtrTab *tab;

  g_return_if_fail (GTR_IS_WINDOW (window));

  tab = gtr_window_get_active_tab (window);

  gtr_tab_clear_msgstr_views (tab);
}
