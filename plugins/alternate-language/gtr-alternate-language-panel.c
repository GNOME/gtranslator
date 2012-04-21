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

#include "gtr-alternate-language-panel.h"
#include "gtr-file-dialogs.h"
#include "gtr-msg.h"
#include "gtr-po.h"
#include "gtr-tab.h"
#include "gtr-view.h"
#include "gtr-debug.h"
#include "gtr-utils.h"

#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define FUZZY_ICON		"gtk-dialog-warning"
#define UNTRANSLATED_ICON	"gtk-dialog-error"

#define GTR_ALTERNATE_LANG_PANEL_GET_PRIVATE(object) (G_TYPE_INSTANCE_GET_PRIVATE ( \
                                                      (object), \
                                                      GTR_TYPE_ALTERNATE_LANG_PANEL, \
                                                      GtrAlternateLangPanelPrivate))

G_DEFINE_DYNAMIC_TYPE (GtrAlternateLangPanel, gtr_alternate_lang_panel, GTK_TYPE_BOX)

struct _GtrAlternateLangPanelPrivate
{
  GtkWidget *open_button;
  GtkWidget *close_button;
  GtkWidget *copy_button;
  GtkWidget *textview;

  GtkWidget *status;

  GtrPo *po;
  GtrTab *tab;

  gulong showed_message_id;

  guint text_found : 1;
};

static void
gtr_alternate_lang_panel_set_text (GtrAlternateLangPanel *panel,
                                   const gchar           *text)
{
  GtkTextBuffer *buf;

  buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (panel->priv->textview));

  gtk_text_buffer_set_text (buf, text, -1);
}

static void
showed_message_cb (GtrTab * tab, GtrMsg * msg, GtrAlternateLangPanel * panel)
{
  GList *messages;
  GList *l;
  const gchar *msgid;
  gchar *msgid_collate;
  const gchar *string;
  gchar *string_collate;
  GtrMsgStatus status;

  g_return_if_fail (GTR_IS_MSG (msg));

  msgid = gtr_msg_get_msgid (msg);
  msgid_collate = g_utf8_collate_key (msgid, -1);
  messages = gtr_po_get_messages (panel->priv->po);
  l = messages;

  do
    {
      string = gtr_msg_get_msgid (l->data);
      string_collate = g_utf8_collate_key (string, -1);
      if (strcmp (string_collate, msgid_collate) == 0)
        {
          gtr_alternate_lang_panel_set_text (panel,
                                             gtr_msg_get_msgstr (l->data));
          status = gtr_msg_get_status (GTR_MSG (l->data));
          switch (status)
            {
            case GTR_MSG_STATUS_TRANSLATED:
              gtk_image_clear (GTK_IMAGE (panel->priv->status));
              break;
            case GTR_MSG_STATUS_FUZZY:
              gtk_image_set_from_stock (GTK_IMAGE (panel->priv->status),
                                        FUZZY_ICON,
                                        GTK_ICON_SIZE_SMALL_TOOLBAR);
              break;
            default:
              break;
            }

          g_free (string_collate);
          g_free (msgid_collate);
          panel->priv->text_found = TRUE;
          return;
        }
      g_free (string_collate);
    }
  while ((l = g_list_next (l)));

  g_free (msgid_collate);
  gtr_alternate_lang_panel_set_text (panel, _("Message not found"));
  panel->priv->text_found = FALSE;

  /* If we are here the status is untranslated */
  gtk_image_set_from_stock (GTK_IMAGE (panel->priv->status),
                            UNTRANSLATED_ICON, GTK_ICON_SIZE_SMALL_TOOLBAR);
}

static void
open_file (GtkWidget *dialog, GtrAlternateLangPanel *panel)
{
  GFile *file;
  gchar *po_file;
  GtrMsg *current;
  GtrPo *current_po;
  GList *l;
  GError *error = NULL;

  po_file = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
  file = g_file_new_for_path (po_file);
  g_free (po_file);

  if (panel->priv->po != NULL)
    g_object_unref (panel->priv->po);
  panel->priv->po = gtr_po_new ();
  gtr_po_parse (panel->priv->po, file, &error);

  g_object_unref (file);

  if (error != NULL)
    {
      GtkWidget *erdialog;
      /*
       * FIXME: We have to get the window
       */
      erdialog = gtk_message_dialog_new (NULL,
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_MESSAGE_ERROR,
                                         GTK_BUTTONS_CLOSE,
                                         "%s", error->message);
      gtk_dialog_run (GTK_DIALOG (erdialog));
      gtk_widget_destroy (erdialog);
      g_error_free (error);
      return;
    }

  panel->priv->showed_message_id =
    g_signal_connect (panel->priv->tab, "showed-message",
                      G_CALLBACK (showed_message_cb), panel);

  current_po = gtr_tab_get_po (panel->priv->tab);
  l = gtr_po_get_current_message (current_po);
  current = GTR_MSG (l->data);

  showed_message_cb (panel->priv->tab, current, panel);
  gtk_widget_set_sensitive (panel->priv->textview, TRUE);

  gtk_widget_destroy (dialog);
}


static void
gtr_file_chooser_analyse (gpointer dialog, GtrAlternateLangPanel * panel)
{
  gint reply;

  reply = gtk_dialog_run (GTK_DIALOG (dialog));
  switch (reply)
    {
    case GTK_RESPONSE_ACCEPT:
      open_file (GTK_WIDGET (dialog), panel);
      break;
    case GTK_RESPONSE_CANCEL:
      gtk_widget_hide (GTK_WIDGET (dialog));
      break;
    case GTK_RESPONSE_DELETE_EVENT:
      gtk_widget_hide (GTK_WIDGET (dialog));
      break;
    default:
      break;
    }
}

static void
open_button_clicked_cb (GtkWidget * open_button,
                        GtrAlternateLangPanel * panel)
{
  GtkWidget *dialog = NULL;
  gchar *dir;
  GtrPo *tab_po;
  GFile *location, *parent;

  if (dialog != NULL)
    {
      gtk_window_present (GTK_WINDOW (dialog));
      return;
    }

  /*
   * I need a way here to get the window
   */
  dialog = gtr_file_chooser_new (NULL,
                                 FILESEL_OPEN,
                                 _("Open file for alternate language"), NULL);

  tab_po = gtr_tab_get_po (panel->priv->tab);
  location = gtr_po_get_location (tab_po);
  parent = g_file_get_parent (location);
  g_object_unref (location);

  dir = g_file_get_path (parent);
  g_object_unref (parent);

  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), dir);
  g_free (dir);

  gtk_file_chooser_set_local_only (GTK_FILE_CHOOSER (dialog), TRUE);

  gtr_file_chooser_analyse ((gpointer) dialog, panel);
}

static void
close_button_clicked_cb (GtkWidget             *close_button,
                         GtrAlternateLangPanel *panel)
{
  if (panel->priv->po != NULL)
    {
      gtk_image_clear (GTK_IMAGE (panel->priv->status));
      gtr_alternate_lang_panel_set_text (panel, _("File closed"));

      gtk_widget_set_sensitive (panel->priv->textview, FALSE);

      g_object_unref (panel->priv->po);
      panel->priv->po = NULL;
      panel->priv->text_found = FALSE;
    }

  if (panel->priv->showed_message_id)
    {
      g_signal_handler_disconnect (panel->priv->tab,
                                   panel->priv->showed_message_id);
      panel->priv->showed_message_id = 0;
    }
}

static void
copy_button_clicked_cb (GtkWidget             *copy_button,
                        GtrAlternateLangPanel *panel)
{
  GtkTextBuffer *panel_buf, *buf;
  GtkTextIter start, end;
  GtrView *view;
  gchar *text;

  if (!panel->priv->text_found)
    return;

  panel_buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (panel->priv->textview));
  gtk_text_buffer_get_bounds (panel_buf, &start, &end);

  text = gtk_text_buffer_get_text (panel_buf, &start, &end, FALSE);

  view = gtr_tab_get_active_view (panel->priv->tab);
  buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
  gtk_text_buffer_begin_user_action (buf);
  gtk_text_buffer_set_text (buf, text, -1);
  gtk_text_buffer_end_user_action (buf);

  g_free (text);
}

static void
gtr_alternate_lang_panel_draw (GtrAlternateLangPanel * panel)
{
  GtkWidget *hbox;
  GtkWidget *buttonbox;
  GtkWidget *scroll;

  /* Hbox */
  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_widget_show (hbox);
  gtk_box_pack_start (GTK_BOX (panel), hbox, FALSE, TRUE, 0);

  /* Button box */
  buttonbox = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (buttonbox), GTK_BUTTONBOX_START);
  gtk_widget_show (buttonbox);

  panel->priv->open_button = gtr_gtk_button_new_with_stock_icon (C_("alternate lang", "_Open"),
                                                                 GTK_STOCK_OPEN);
  g_signal_connect (panel->priv->open_button,
                    "clicked", G_CALLBACK (open_button_clicked_cb), panel);
  gtk_widget_show (panel->priv->open_button);

  panel->priv->close_button = gtr_gtk_button_new_with_stock_icon (C_("alternate lang", "_Close"),
                                                                  GTK_STOCK_CLOSE);
  g_signal_connect (panel->priv->close_button,
                    "clicked", G_CALLBACK (close_button_clicked_cb), panel);
  gtk_widget_show (panel->priv->close_button);

  panel->priv->copy_button = gtr_gtk_button_new_with_stock_icon (C_("alternate lang", "Co_py"),
                                                                 GTK_STOCK_COPY);
  g_signal_connect (panel->priv->copy_button,
                    "clicked", G_CALLBACK (copy_button_clicked_cb), panel);
  gtk_widget_show (panel->priv->copy_button);

  gtk_box_pack_start (GTK_BOX (buttonbox),
                      panel->priv->open_button, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (buttonbox),
                      panel->priv->close_button, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (buttonbox),
                      panel->priv->copy_button, TRUE, TRUE, 0);

  gtk_box_pack_start (GTK_BOX (hbox), buttonbox, FALSE, TRUE, 0);

  /* Status image */
  panel->priv->status = gtk_image_new ();
  gtk_widget_show (panel->priv->status);

  gtk_box_pack_start (GTK_BOX (hbox), panel->priv->status, FALSE, FALSE, 0);

  /* Text view */
  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scroll);

  panel->priv->textview = gtr_view_new ();
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (panel->priv->textview),
                               GTK_WRAP_WORD);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (panel->priv->textview), FALSE);
  gtr_alternate_lang_panel_set_text (panel, _("There isn't any file loaded"));
  gtk_widget_set_sensitive (panel->priv->textview, FALSE);
  gtk_widget_show (panel->priv->textview);

  gtk_container_add (GTK_CONTAINER (scroll), panel->priv->textview);

  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroll),
                                       GTK_SHADOW_IN);

  gtk_box_pack_start (GTK_BOX (panel), scroll, TRUE, TRUE, 0);
}

static void
gtr_alternate_lang_panel_init (GtrAlternateLangPanel * panel)
{
  panel->priv = GTR_ALTERNATE_LANG_PANEL_GET_PRIVATE (panel);

  panel->priv->showed_message_id = 0;
  panel->priv->po = NULL;

  gtk_orientable_set_orientation (GTK_ORIENTABLE (panel),
                                  GTK_ORIENTATION_VERTICAL);

  gtr_alternate_lang_panel_draw (panel);
}

static void
gtr_alternate_lang_panel_dispose (GObject *object)
{
  GtrAlternateLangPanel *panel = GTR_ALTERNATE_LANG_PANEL (object);

  DEBUG_PRINT ("Disposing alternate lang panel");

  if (panel->priv->showed_message_id != 0)
    {
      g_signal_handler_disconnect (panel->priv->tab,
                                   panel->priv->showed_message_id);
      panel->priv->showed_message_id = 0;
    }

  g_clear_object (&panel->priv->po);

  G_OBJECT_CLASS (gtr_alternate_lang_panel_parent_class)->dispose (object);
}

static void
gtr_alternate_lang_panel_class_init (GtrAlternateLangPanelClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrAlternateLangPanelPrivate));

  object_class->dispose = gtr_alternate_lang_panel_dispose;
}

static void
gtr_alternate_lang_panel_class_finalize (GtrAlternateLangPanelClass *klass)
{
}

/***************************** Public funcs ***********************************/

GtkWidget *
gtr_alternate_lang_panel_new (GtrTab *tab)
{
  GtrAlternateLangPanel *panel;
  panel = g_object_new (GTR_TYPE_ALTERNATE_LANG_PANEL, NULL);

  panel->priv->tab = tab;

  return GTK_WIDGET (panel);
}

void
_gtr_alternate_lang_panel_register_type (GTypeModule *type_module)
{
  gtr_alternate_lang_panel_register_type (type_module);
}
