/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *                     Fatih Demir <kabalak@kabalak.net>
 * 		       Ross Golder <ross@golder.org>
 * 		       Gediminas Paulauskas <menesis@kabalak.net>
 * 		       homas Ziehmer <thomas@kabalak.net>
 *               2008  Igalia
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *   Pablo Sanxiao <psanxiao@gmail.com>
 *   Fatih Demir <kabalak@kabalak.net>
 *   Ross Golder <ross@golder.org>
 *   Gediminas Paulauskas <menesis@kabalak.net>
 *   Thomas Ziehmer <thomas@kabalak.net>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-application.h"
#include "gtr-context.h"
#include "gtr-io-error-info-bar.h"
#include "gtr-message-table.h"
#include "gtr-msg.h"
#include "gtr-tab.h"
#include "gtr-po.h"
#include "gtr-prefs-manager.h"
#include "gtr-prefs-manager-app.h"
#include "gtr-view.h"
#include "gtr-translation-memory.h"
#include "gtr-translation-memory-ui.h"
#include "gtr-window.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#define GTR_TAB_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_TAB,     \
					 GtrTabPrivate))

#define MAX_PLURALS 6

#define GTR_TAB_KEY "GtrTabFromDocument"

G_DEFINE_TYPE (GtrTab, gtr_tab, GTK_TYPE_VBOX)

struct _GtrTabPrivate
{
  GtrPo *po;

  GtkWidget *table_pane;
  GtkWidget *content_pane;
  GtkWidget *panel;
  GtkWidget *message_table;
  GtkWidget *lateral_panel;        //TM, Context, etc.

  GtkWidget *comment_pane;
  GtkWidget *context;
  GtkWidget *translation_memory;

  /*Info bar */
  GtkWidget *infobar;

  /*Original text */
  GtkWidget *msgid_hbox;
  GtkWidget *text_vbox;
  GtkWidget *text_msgid;
  GtkWidget *text_plural_scroll;
  GtkWidget *text_msgid_plural;

  /*Translated text */
  GtkWidget *msgstr_label;
  GtkWidget *msgstr_hbox;
  GtkWidget *trans_notebook;
  GtkWidget *trans_msgstr[MAX_PLURALS];

  /*Status widgets */
  GtkWidget *translated;
  GtkWidget *fuzzy;
  GtkWidget *untranslated;

  /* Autosave */
  GTimer *timer;
  gint autosave_interval;
  guint autosave_timeout;
  guint autosave : 1;

  /*Blocking movement */
  guint blocking : 1;
};

enum
{
  SHOWED_MESSAGE,
  MESSAGE_CHANGED,
  MESSAGE_EDITION_FINISHED,
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_NAME,
  PROP_AUTOSAVE,
  PROP_AUTOSAVE_INTERVAL
};

static guint signals[LAST_SIGNAL];

static gboolean gtr_tab_autosave (GtrTab * tab);

static void
install_autosave_timeout (GtrTab * tab)
{
  gint timeout;

  g_return_if_fail (tab->priv->autosave_timeout <= 0);
  g_return_if_fail (tab->priv->autosave);
  g_return_if_fail (tab->priv->autosave_interval > 0);

  /* Add a new timeout */
  timeout = g_timeout_add (tab->priv->autosave_interval * 1000 * 60,
                           (GSourceFunc) gtr_tab_autosave, tab);

  tab->priv->autosave_timeout = timeout;
}

static gboolean
install_autosave_timeout_if_needed (GtrTab * tab)
{
  g_return_val_if_fail (tab->priv->autosave_timeout <= 0, FALSE);

  if (tab->priv->autosave)
    {
      install_autosave_timeout (tab);

      return TRUE;
    }

  return FALSE;
}

static gboolean
gtr_tab_autosave (GtrTab * tab)
{
  GError *error = NULL;

  if (!gtr_po_get_state (tab->priv->po) == GTR_PO_STATE_MODIFIED)
    return TRUE;

  gtr_po_save_file (tab->priv->po, &error);
  if (error)
    {
      g_warning ("%s", error->message);
      g_error_free (error);
    }

  return TRUE;
}

static void
remove_autosave_timeout (GtrTab * tab)
{
  g_return_if_fail (tab->priv->autosave_timeout > 0);

  g_source_remove (tab->priv->autosave_timeout);
  tab->priv->autosave_timeout = 0;
}

static void
gtr_tab_edition_finished (GtrTab * tab, GtrMsg * msg)
{
  GtrTranslationMemory *tm;
  gchar *message_error;
  GtkWidget *infobar;

  tm =
    GTR_TRANSLATION_MEMORY (gtr_application_get_translation_memory (GTR_APP));

  if (gtr_msg_is_translated (msg) && !gtr_msg_is_fuzzy (msg))
    gtr_translation_memory_store (tm,
                                  gtr_msg_get_msgid (msg),
                                  gtr_msg_get_msgstr (msg));

  /*
   * Checking message
   */
  message_error = gtr_msg_check (msg);

  if (message_error != NULL)
    {
      gtr_tab_block_movement (tab);

      infobar = create_error_info_bar (_("There is an error in the message:"),
                                       message_error);
      gtr_tab_set_info_bar (tab, infobar);
      g_free (message_error);
    }
  else
    {
      gtr_tab_unblock_movement (tab);
      gtr_tab_set_info_bar (tab, NULL);
    }
}

/*
 * Write the change back to the gettext PO instance in memory and
 * mark the page dirty
 */
static void
gtr_message_translation_update (GtkTextBuffer * textbuffer, GtrTab * tab)
{
  GtrHeader *header;
  GtkTextIter start, end;
  GtkTextBuffer *buf;
  GList *msg_aux;
  GtrMsg *msg;
  const gchar *check;
  gchar *translation;
  gint i;

  /* Work out which message this is associated with */

  header = gtr_po_get_header (tab->priv->po);

  msg_aux = gtr_po_get_current_message (tab->priv->po);
  msg = msg_aux->data;
  buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tab->priv->trans_msgstr[0]));

  if (gtr_msg_is_fuzzy (msg)
      && gtr_prefs_manager_get_unmark_fuzzy_when_changed ())
    gtr_msg_set_fuzzy (msg, FALSE);

  if (textbuffer == buf)
    {
      /* Get message as UTF-8 buffer */
      gtk_text_buffer_get_bounds (textbuffer, &start, &end);
      translation = gtk_text_buffer_get_text (textbuffer, &start, &end, TRUE);

      /* TODO: convert to file's own encoding if not UTF-8 */

      /* Write back to PO file in memory */
      if (!(check = gtr_msg_get_msgid_plural (msg)))
        {
          gtr_msg_set_msgstr (msg, translation);
        }
      else
        {
          gtr_msg_set_msgstr_plural (msg, 0, translation);
          //free(check);
        }
      g_free (translation);
      return;
    }
  i = 1;
  while (i < gtr_header_get_nplurals (header))
    {
      /* Know when to break out of the loop */
      if (!tab->priv->trans_msgstr[i])
        {
          break;
        }

      /* Have we reached the one we want yet? */
      buf =
        gtk_text_view_get_buffer (GTK_TEXT_VIEW (tab->priv->trans_msgstr[i]));
      if (textbuffer != buf)
        {
          i++;
          continue;
        }

      /* Get message as UTF-8 buffer */
      gtk_text_buffer_get_bounds (textbuffer, &start, &end);
      translation = gtk_text_buffer_get_text (textbuffer, &start, &end, TRUE);

      /* TODO: convert to file's own encoding if not UTF-8 */

      /* Write back to PO file in memory */
      gtr_msg_set_msgstr_plural (msg, i, translation);
      return;
    }

  /* Shouldn't get here */
  g_return_if_reached ();
}

static GtkWidget *
gtr_tab_append_msgstr_page (const gchar * tab_label,
                            GtkWidget * box, gboolean spellcheck)
{
  GtkWidget *scroll;
  GtkWidget *label;
  GtkWidget *widget;

  label = gtk_label_new (tab_label);

  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scroll);

  widget = gtr_view_new ();
  gtk_widget_show (widget);

  if (spellcheck && gtr_prefs_manager_get_spellcheck ())
    gtr_view_enable_spellcheck (GTR_VIEW (widget), spellcheck);

  gtk_container_add (GTK_CONTAINER (scroll), widget);

  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroll),
                                       GTK_SHADOW_IN);

  gtk_notebook_append_page (GTK_NOTEBOOK (box), scroll, label);

  return widget;
}

static void
gtr_message_plural_forms (GtrTab * tab, GtrMsg * msg)
{
  GtrHeader *header;
  GtkTextBuffer *buf;
  const gchar *msgstr_plural;
  gint i;

  g_return_if_fail (tab != NULL);
  g_return_if_fail (msg != NULL);

  header = gtr_po_get_header (tab->priv->po);

  for (i = 0; i < gtr_header_get_nplurals (header); i++)
    {
      msgstr_plural = gtr_msg_get_msgstr_plural (msg, i);
      if (msgstr_plural)
        {
          buf =
            gtk_text_view_get_buffer (GTK_TEXT_VIEW
                                      (tab->priv->trans_msgstr[i]));
          gtk_source_buffer_begin_not_undoable_action (GTK_SOURCE_BUFFER
                                                       (buf));
          gtk_text_buffer_set_text (buf, (gchar *) msgstr_plural, -1);
          gtk_source_buffer_end_not_undoable_action (GTK_SOURCE_BUFFER (buf));
        }
    }
}

/*
 * gtr_tab_show_message:
 * @tab: a #GtranslationTab
 * @msg: a #GtrMsg
 * 
 * Shows the @msg in the @tab TextViews
 *
 */
static void
gtr_tab_show_message (GtrTab * tab, GtrMsg * msg)
{
  GtrTabPrivate *priv = tab->priv;
  GtrPo *po;
  GtkTextBuffer *buf;
  const gchar *msgid, *msgid_plural;
  const gchar *msgstr;

  g_return_if_fail (GTR_IS_TAB (tab));

  po = priv->po;
  gtr_po_update_current_message (po, msg);
  msgid = gtr_msg_get_msgid (msg);
  if (msgid)
    {
      buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (priv->text_msgid));
      gtk_source_buffer_begin_not_undoable_action (GTK_SOURCE_BUFFER (buf));
      gtk_text_buffer_set_text (buf, (gchar *) msgid, -1);
      gtk_source_buffer_end_not_undoable_action (GTK_SOURCE_BUFFER (buf));
    }
  msgid_plural = gtr_msg_get_msgid_plural (msg);
  if (!msgid_plural)
    {
      msgstr = gtr_msg_get_msgstr (msg);
      /*
       * Disable notebook tabs and hide widgets
       */
      gtk_widget_hide (priv->text_plural_scroll);
      gtk_notebook_set_show_tabs (GTK_NOTEBOOK (priv->trans_notebook), FALSE);
      gtk_notebook_set_current_page (GTK_NOTEBOOK (priv->trans_notebook), 0);
      if (msgstr)
        {
          buf =
            gtk_text_view_get_buffer (GTK_TEXT_VIEW (priv->trans_msgstr[0]));
          gtk_source_buffer_begin_not_undoable_action (GTK_SOURCE_BUFFER
                                                       (buf));
          gtk_text_buffer_set_text (buf, (gchar *) msgstr, -1);
          gtk_source_buffer_end_not_undoable_action (GTK_SOURCE_BUFFER (buf));
          gtk_label_set_mnemonic_widget (GTK_LABEL (priv->msgstr_label),
                                         priv->trans_msgstr[0]);
        }
    }
  else
    {
      gtk_widget_show (priv->text_plural_scroll);
      gtk_notebook_set_show_tabs (GTK_NOTEBOOK (tab->priv->trans_notebook),
                                  TRUE);
      buf =
        gtk_text_view_get_buffer (GTK_TEXT_VIEW
                                  (tab->priv->text_msgid_plural));
      gtk_text_buffer_set_text (buf, (gchar *) msgid_plural, -1);
      gtr_message_plural_forms (tab, msg);
    }
}

static void
emit_message_changed_signal (GtkTextBuffer * buf, GtrTab * tab)
{
  GList *msg;

  msg = gtr_po_get_current_message (tab->priv->po);

  g_signal_emit (G_OBJECT (tab), signals[MESSAGE_CHANGED], 0, msg->data);
}

static void
update_status (GtrTab * tab, GtrMsg * msg, gpointer useless)
{
  GtrMsgStatus status;
  GtrPoState po_state;
  gboolean fuzzy, translated;

  status = gtr_msg_get_status (msg);
  po_state = gtr_po_get_state (tab->priv->po);

  fuzzy = gtr_msg_is_fuzzy (msg);
  translated = gtr_msg_is_translated (msg);

  if ((status == GTR_MSG_STATUS_FUZZY) && !fuzzy)
    {
      _gtr_po_increase_decrease_fuzzy (tab->priv->po, FALSE);
      if (translated)
        {
          status = GTR_MSG_STATUS_TRANSLATED;
          _gtr_po_increase_decrease_translated (tab->priv->po, TRUE);
        }
      else
        {
          status = GTR_MSG_STATUS_UNTRANSLATED;
        }
    }
  else if ((status == GTR_MSG_STATUS_TRANSLATED) && !translated)
    {
      status = GTR_MSG_STATUS_UNTRANSLATED;
      _gtr_po_increase_decrease_translated (tab->priv->po, FALSE);
    }
  else if ((status == GTR_MSG_STATUS_TRANSLATED) && fuzzy)
    {
      status = GTR_MSG_STATUS_FUZZY;
      _gtr_po_increase_decrease_translated (tab->priv->po, FALSE);
      _gtr_po_increase_decrease_fuzzy (tab->priv->po, TRUE);
    }
  else if ((status == GTR_MSG_STATUS_UNTRANSLATED) && translated)
    {
      if (fuzzy)
        {
          status = GTR_MSG_STATUS_FUZZY;
          _gtr_po_increase_decrease_fuzzy (tab->priv->po, TRUE);
        }
      else
        {
          status = GTR_MSG_STATUS_TRANSLATED;
          _gtr_po_increase_decrease_translated (tab->priv->po, TRUE);
        }
    }

  gtr_msg_set_status (msg, status);

  /* We need to update the tab state too if is neccessary */
  if (po_state != GTR_PO_STATE_MODIFIED)
    gtr_po_set_state (tab->priv->po, GTR_PO_STATE_MODIFIED);
}

static void
comment_pane_position_changed (GObject * tab_gobject,
                               GParamSpec * arg1, GtrTab * tab)
{
  gtr_prefs_manager_set_comment_pane_pos (gtk_paned_get_position
                                          (GTK_PANED (tab_gobject)));
}

static void
content_pane_position_changed (GObject * tab_gobject,
                               GParamSpec * arg1, GtrTab * tab)
{
  gtr_prefs_manager_set_content_pane_pos (gtk_paned_get_position
                                          (GTK_PANED (tab_gobject)));
}

static void
gtr_tab_add_msgstr_tabs (GtrTab * tab)
{
  GtrHeader *header;
  GtrTabPrivate *priv = tab->priv;
  gchar *label;
  GtkTextBuffer *buf;
  gint i = 0;

  /*
   * We get the header of the po file
   */
  header = gtr_po_get_header (tab->priv->po);

  do
    {
      label = g_strdup_printf (_("Plural %d"), i + 1);
      priv->trans_msgstr[i] = gtr_tab_append_msgstr_page (label,
                                                          priv->trans_notebook,
                                                          TRUE);
      buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (priv->trans_msgstr[i]));
      g_signal_connect (buf, "end-user-action",
                        G_CALLBACK (gtr_message_translation_update), tab);

      g_signal_connect_after (buf, "end_user_action",
                              G_CALLBACK (emit_message_changed_signal), tab);
      i++;
      g_free (label);
    }
  while (i < gtr_header_get_nplurals (header));
}

static void
on_location_notify (GtrPo      *po,
                    GParamSpec *pspec,
                    GtrTab     *tab)
{
  g_object_notify (G_OBJECT (tab), "name");
}

static void
on_state_notify (GtrPo      *po,
                 GParamSpec *pspec,
                 GtrTab     *tab)
{
  g_object_notify (G_OBJECT (tab), "name");
}

static void
gtr_tab_draw (GtrTab * tab)
{
  GtkWidget *vertical_box;
  GtkWidget *label_widget;
  GtkWidget *msgid_label;
  GtkWidget *scroll;
  GtrTabPrivate *priv = tab->priv;

  /* Panel */
  priv->panel = gtk_notebook_new ();
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK (priv->panel), GTK_POS_BOTTOM);
  gtk_widget_show (priv->panel);

  /* Message table */
  priv->message_table = gtr_message_table_new (GTK_WIDGET (tab));
  gtk_widget_show (priv->message_table);

  label_widget = gtk_label_new (_("Message Table"));

  gtk_notebook_append_page (GTK_NOTEBOOK (priv->panel),
                            priv->message_table, label_widget);

  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (priv->panel), FALSE);

  /* Comment pane */
  priv->comment_pane = gtk_hpaned_new ();
  gtk_paned_set_position (GTK_PANED (priv->comment_pane),
                          gtr_prefs_manager_get_comment_pane_pos ());
  g_signal_connect (priv->comment_pane, "notify::position",
                    G_CALLBACK (comment_pane_position_changed), tab);
  gtk_widget_show (priv->comment_pane);

  /* Lateral panel */
  tab->priv->lateral_panel = gtk_notebook_new ();
  gtk_widget_show (tab->priv->lateral_panel);

  gtk_paned_pack2 (GTK_PANED (priv->comment_pane), tab->priv->lateral_panel,
                   TRUE, TRUE);

  /* Context */
  priv->context = gtr_context_panel_new (GTK_WIDGET (tab));
  gtk_widget_show (priv->context);
  gtr_tab_add_widget_to_lateral_panel (tab, priv->context, _("Context"));

  /* TM */
  priv->translation_memory = gtr_translation_memory_ui_new (GTK_WIDGET (tab));
  gtk_widget_show (priv->translation_memory);
  gtr_tab_add_widget_to_lateral_panel (tab, priv->translation_memory,
                                       _("Translation Memory"));

  /* Content pane; this is where the message table and message area go */
  priv->content_pane = gtk_vpaned_new ();
  gtk_paned_set_position (GTK_PANED (priv->content_pane),
                          gtr_prefs_manager_get_content_pane_pos ());
  g_signal_connect (priv->content_pane,
                    "notify::position",
                    G_CALLBACK (content_pane_position_changed), tab);
  gtk_widget_show (priv->content_pane);

  /* Pack the comments pane and the main content */
  vertical_box = gtk_vbox_new (FALSE, 0);
  gtk_paned_pack1 (GTK_PANED (priv->content_pane), GTK_WIDGET (priv->panel),
                   TRUE, FALSE);
  gtk_paned_pack2 (GTK_PANED (priv->content_pane), priv->comment_pane, FALSE,
                   TRUE);
  gtk_widget_show (vertical_box);

  /* Orignal text widgets */
  priv->msgid_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (priv->msgid_hbox);

  msgid_label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (msgid_label), _("<b>Original Text:</b>"));
  gtk_misc_set_padding (GTK_MISC (msgid_label), 0, 5);
  gtk_widget_show (msgid_label);

  gtk_box_pack_start (GTK_BOX (priv->msgid_hbox), msgid_label, FALSE, FALSE,
                      0);

  priv->text_vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (priv->text_vbox);

  /* Singular */
  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scroll);

  priv->text_msgid = gtr_view_new ();
  gtk_text_view_set_editable (GTK_TEXT_VIEW (priv->text_msgid), FALSE);
  gtk_widget_show (priv->text_msgid);

  gtk_container_add (GTK_CONTAINER (scroll), priv->text_msgid);

  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroll),
                                       GTK_SHADOW_IN);

  gtk_box_pack_start (GTK_BOX (priv->text_vbox), scroll, TRUE, TRUE, 0);

  /* Plural */
  priv->text_plural_scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (priv->text_plural_scroll);

  priv->text_msgid_plural = gtr_view_new ();
  gtk_text_view_set_editable (GTK_TEXT_VIEW (priv->text_msgid_plural), FALSE);
  gtk_widget_show (priv->text_msgid_plural);

  gtk_container_add (GTK_CONTAINER (priv->text_plural_scroll),
                     priv->text_msgid_plural);

  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW
                                  (priv->text_plural_scroll),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW
                                       (priv->text_plural_scroll),
                                       GTK_SHADOW_IN);

  gtk_box_pack_start (GTK_BOX (priv->text_vbox), priv->text_plural_scroll,
                      TRUE, TRUE, 0);

  gtk_box_pack_start (GTK_BOX (vertical_box), priv->msgid_hbox, FALSE, FALSE,
                      0);
  gtk_box_pack_start (GTK_BOX (vertical_box), priv->text_vbox, TRUE, TRUE, 0);


  /* Translation widgets */
  priv->msgstr_label = gtk_label_new (NULL);
  gtk_label_set_markup_with_mnemonic (GTK_LABEL (priv->msgstr_label),
                                      _("<b>Translate_d Text:</b>"));
  gtk_misc_set_padding (GTK_MISC (priv->msgstr_label), 0, 5);
  gtk_misc_set_alignment (GTK_MISC (priv->msgstr_label), 0, 0.5);
  gtk_widget_show (priv->msgstr_label);

  priv->trans_notebook = gtk_notebook_new ();
  gtk_notebook_set_show_border (GTK_NOTEBOOK (priv->trans_notebook), FALSE);
  gtk_widget_show (priv->trans_notebook);

  gtk_box_pack_start (GTK_BOX (vertical_box), priv->msgstr_label, FALSE, FALSE,
                      0);
  gtk_box_pack_start (GTK_BOX (vertical_box), priv->trans_notebook, TRUE,
                      TRUE, 0);

  gtk_paned_pack1 (GTK_PANED (priv->comment_pane), vertical_box, FALSE,
                   FALSE);

  gtk_box_pack_start (GTK_BOX (tab), priv->content_pane, TRUE, TRUE, 0);
}

static void
gtr_tab_init (GtrTab * tab)
{
  tab->priv = GTR_TAB_GET_PRIVATE (tab);

  g_signal_connect (tab, "message-changed", G_CALLBACK (update_status), NULL);

  gtr_tab_draw (tab);

  /* Manage auto save data */
  tab->priv->autosave = gtr_prefs_manager_get_autosave ();
  tab->priv->autosave = (tab->priv->autosave != FALSE);

  tab->priv->autosave_interval = gtr_prefs_manager_get_autosave_interval ();
  if (tab->priv->autosave_interval <= 0)
    tab->priv->autosave_interval = 1;
}

static void
gtr_tab_finalize (GObject * object)
{
  GtrTab *tab = GTR_TAB (object);

  if (tab->priv->po)
    g_object_unref (tab->priv->po);

  if (tab->priv->timer != NULL)
    g_timer_destroy (tab->priv->timer);

  if (tab->priv->autosave_timeout > 0)
    remove_autosave_timeout (tab);

  G_OBJECT_CLASS (gtr_tab_parent_class)->finalize (object);
}

static void
gtr_tab_get_property (GObject * object,
                      guint prop_id, GValue * value, GParamSpec * pspec)
{
  GtrTab *tab = GTR_TAB (object);

  switch (prop_id)
    {
    case PROP_AUTOSAVE:
      g_value_set_boolean (value, gtr_tab_get_autosave_enabled (tab));
      break;
    case PROP_AUTOSAVE_INTERVAL:
      g_value_set_int (value, gtr_tab_get_autosave_interval (tab));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtr_tab_set_property (GObject * object,
                      guint prop_id, const GValue * value, GParamSpec * pspec)
{
  GtrTab *tab = GTR_TAB (object);

  switch (prop_id)
    {
    case PROP_AUTOSAVE:
      gtr_tab_set_autosave_enabled (tab, g_value_get_boolean (value));
      break;
    case PROP_AUTOSAVE_INTERVAL:
      gtr_tab_set_autosave_interval (tab, g_value_get_int (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtr_tab_class_init (GtrTabClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrTabPrivate));

  object_class->finalize = gtr_tab_finalize;
  object_class->set_property = gtr_tab_set_property;
  object_class->get_property = gtr_tab_get_property;
  klass->message_edition_finished = gtr_tab_edition_finished;

  /* Signals */
  signals[SHOWED_MESSAGE] =
    g_signal_new ("showed-message",
                  G_OBJECT_CLASS_TYPE (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GtrTabClass, showed_message),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__POINTER,
                  G_TYPE_NONE, 1, G_TYPE_POINTER);

  signals[MESSAGE_CHANGED] =
    g_signal_new ("message-changed",
                  G_OBJECT_CLASS_TYPE (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GtrTabClass, message_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__POINTER,
                  G_TYPE_NONE, 1, G_TYPE_POINTER);

  signals[MESSAGE_EDITION_FINISHED] =
    g_signal_new ("message-edition-finished",
                  G_OBJECT_CLASS_TYPE (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GtrTabClass,
                                   message_edition_finished), NULL, NULL,
                  g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1,
                  G_TYPE_POINTER);

  /* Properties */
  g_object_class_install_property (object_class,
                                   PROP_NAME,
                                   g_param_spec_string ("name",
                                                        "Name",
                                                        "The tab's name",
                                                        NULL,
                                                        G_PARAM_READABLE |
                                                        G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (object_class,
                                   PROP_AUTOSAVE,
                                   g_param_spec_boolean ("autosave",
                                                         "Autosave",
                                                         "Autosave feature",
                                                         TRUE,
                                                         G_PARAM_READWRITE |
                                                         G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (object_class,
                                   PROP_AUTOSAVE_INTERVAL,
                                   g_param_spec_int ("autosave-interval",
                                                     "AutosaveInterval",
                                                     "Time between two autosaves",
                                                     0,
                                                     G_MAXINT,
                                                     0,
                                                     G_PARAM_READWRITE |
                                                     G_PARAM_STATIC_STRINGS));
}

/***************************** Public funcs ***********************************/

/**
 * gtr_tab_new:
 * @po: a #GtrPo
 * 
 * Creates a new #GtrTab.
 * 
 * Return value: a new #GtrTab object
 **/
GtrTab *
gtr_tab_new (GtrPo * po)
{
  GtrTab *tab;

  g_return_val_if_fail (po != NULL, NULL);

  tab = g_object_new (GTR_TYPE_TAB, NULL);

  /* FIXME: make the po a property */
  tab->priv->po = po;
  g_object_set_data (G_OBJECT (po), GTR_TAB_KEY, tab);

  g_signal_connect (po, "notify::location",
                    G_CALLBACK (on_location_notify), tab);

  g_signal_connect (po, "notify::state",
                    G_CALLBACK (on_state_notify), tab);

  install_autosave_timeout_if_needed (tab);

  /* Now we have to initialize the number of msgstr tabs */
  gtr_tab_add_msgstr_tabs (tab);

  gtr_message_table_populate (GTR_MESSAGE_TABLE (tab->priv->message_table),
                              gtr_po_get_messages (tab->priv->po));

  gtk_widget_show (GTK_WIDGET (tab));
  return tab;
}

/**
 * gtr_tab_get_po:
 * @tab: a #GtrTab
 *
 * Return value: the #GtrPo stored in the #GtrTab
**/
GtrPo *
gtr_tab_get_po (GtrTab * tab)
{
  return tab->priv->po;
}

/**
 * gtr_tab_get_panel:
 * @tab: a #GtranslationTab
 * 
 * Return value: the horizontal notebook of the #GtranslationTab
**/
GtkWidget *
gtr_tab_get_panel (GtrTab * tab)
{
  g_return_val_if_fail (tab != NULL, NULL);

  return tab->priv->panel;
}

/**
 * gtr_tab_get_active_trans_tab:
 * @tab: a #GtranslationTab
 * 
 * Return value: the number of the active translation notebook.
 **/
gint
gtr_tab_get_active_trans_tab (GtrTab * tab)
{
  return
    gtk_notebook_get_current_page (GTK_NOTEBOOK (tab->priv->trans_notebook));
}

/**
 * gtr_tab_get_context_panel:
 * @tab: a #GtrTab
 *
 * Return value: the #GtranslaorContextPanel
 */
GtrContextPanel *
gtr_tab_get_context_panel (GtrTab * tab)
{
  return GTR_CONTEXT_PANEL (tab->priv->context);
}

/**
 * gtr_tab_get_translation_memory_ui:
 * @tab: a #GtrTab
 *
 * Returns: the #GtrTranslationMemoryUi panel.
 */
GtkWidget *
gtr_tab_get_translation_memory_ui (GtrTab * tab)
{
  g_return_val_if_fail (GTR_IS_TAB (tab), NULL);

  return tab->priv->translation_memory;
}

/**
 * gtr_tab_get_active_view:
 * @tab: a #GtranslationTab
 *
 * Return value: the active page of the translation notebook.
**/
GtrView *
gtr_tab_get_active_view (GtrTab * tab)
{
  gint num;

  num =
    gtk_notebook_get_current_page (GTK_NOTEBOOK (tab->priv->trans_notebook));
  return GTR_VIEW (tab->priv->trans_msgstr[num]);
}

/**
 * gtr_tab_get_all_views:
 * @tab: the #GtranslationTab
 * @original: TRUE if you want original TextViews.
 * @translated: TRUE if you want tranlated TextViews.
 *
 * Returns all the views currently present in #GtranslationTab
 *
 * Return value: a newly allocated list of #GtranslationTab objects
 */
GList *
gtr_tab_get_all_views (GtrTab * tab, gboolean original, gboolean translated)
{
  GList *ret = NULL;
  gint i = 0;

  g_return_val_if_fail (GTR_IS_TAB (tab), NULL);

  if (original)
    {
      ret = g_list_append (ret, tab->priv->text_msgid);
      ret = g_list_append (ret, tab->priv->text_msgid_plural);
    }

  if (translated)
    {
      while (i < MAX_PLURALS)
        {
          if (tab->priv->trans_msgstr[i])
            ret = g_list_append (ret, tab->priv->trans_msgstr[i]);
          else
            break;
          i++;
        }
    }

  return ret;
}

/**
 * gtr_tab_message_go_to:
 * @tab: a #GtrTab
 * @to_go: the #GtrMsg you want to jump
 * @searching: TRUE if we are searching in the message list
 *
 * Jumps to the specific @to_go pointer message and show the message
 * in the #GtrView.
**/
void
gtr_tab_message_go_to (GtrTab * tab,
                       GList * to_go, gboolean searching, GtrTabMove move)
{
  GtrPo *po;
  GList *current_msg;
  static gboolean first_msg = TRUE;

  g_return_if_fail (tab != NULL);
  g_return_if_fail (to_go != NULL);
  g_return_if_fail (GTR_IS_MSG (to_go->data));

  po = tab->priv->po;

  current_msg = gtr_po_get_current_message (po);

  /*
   * Emitting message-edition-finished signal
   */
  if (!searching && !first_msg)
    g_signal_emit (G_OBJECT (tab), signals[MESSAGE_EDITION_FINISHED],
                   0, GTR_MSG (current_msg->data));

  if (!tab->priv->blocking || first_msg)
    {
      gboolean plurals;
      gint current_page, n_pages;
      /*
       * If the current message is plural and we press next/prev
       * we have to change to the next/prev plural tab in case is not
       * the last
       * To implement that:
       * if the tabs are showed then we check if we want prev or 
       * next and then if we need to change the tab we change it 
       * in other case we show the message
       * 
       * I don't like too much this implementation so if anybody can
       * rewrite this is a better way would be great.
       */
      plurals =
        gtk_notebook_get_show_tabs (GTK_NOTEBOOK (tab->priv->trans_notebook));
      current_page =
        gtk_notebook_get_current_page (GTK_NOTEBOOK
                                       (tab->priv->trans_notebook));
      n_pages =
        gtk_notebook_get_n_pages (GTK_NOTEBOOK (tab->priv->trans_notebook));
      if ((plurals == TRUE) && (move != GTR_TAB_MOVE_NONE))
        {
          if ((n_pages - 1) == current_page && move == GTR_TAB_MOVE_NEXT)
            {
              gtk_notebook_set_current_page (GTK_NOTEBOOK
                                             (tab->priv->trans_notebook), 0);
              gtr_tab_show_message (tab, to_go->data);
            }
          else if (current_page == 0 && move == GTR_TAB_MOVE_PREV)
            {
              gtk_notebook_set_current_page (GTK_NOTEBOOK
                                             (tab->priv->trans_notebook),
                                             n_pages - 1);
              gtr_tab_show_message (tab, to_go->data);
            }
          else
            {
              if (move == GTR_TAB_MOVE_NEXT)
                gtk_notebook_set_current_page (GTK_NOTEBOOK
                                               (tab->priv->trans_notebook),
                                               current_page + 1);
              else
                gtk_notebook_set_current_page (GTK_NOTEBOOK
                                               (tab->priv->trans_notebook),
                                               current_page - 1);
              return;
            }
        }
      else
        gtr_tab_show_message (tab, to_go->data);
      first_msg = FALSE;
    }
  else
    return;

  /*
   * Emitting showed-message signal
   */
  if (!searching)
    g_signal_emit (G_OBJECT (tab), signals[SHOWED_MESSAGE], 0,
                   GTR_MSG (to_go->data));
}

/**
 * _gtr_tab_get_name:
 * @tab: a #GtrTab 
 * 
 * Return value: a new allocated string with the name of the @tab.
 */
gchar *
_gtr_tab_get_name (GtrTab *tab)
{
  GtrHeader *header;
  GtrPoState state;
  const gchar *str;
  gchar *tab_name;

  header = gtr_po_get_header (tab->priv->po);
  state = gtr_po_get_state (tab->priv->po);

  str = gtr_header_get_prj_id_version (header);

  if (state == GTR_PO_STATE_MODIFIED)
    {
      tab_name = g_strdup_printf ("*%s", str);
      return tab_name;
    }

  return g_strdup (str);
}

gchar *
_gtr_tab_get_tooltips (GtrTab *tab)
{
  GFile *location;
  gchar *tooltips;
  gchar *path;

  location = gtr_po_get_location (tab->priv->po);
  path = g_file_get_path (location);
  g_object_unref (location);

  /* Translators: Path to the document opened */
  tooltips = g_strdup_printf ("<b>%s</b> %s", _("Path:"), path);
  g_free (path);

  return tooltips;
}

/**
 * _gtr_tab_can_close:
 * @tab: a #GtrTab
 *
 * Whether a #GtrTab can be closed.
 *
 * Returns: TRUE if the #GtrPo of the @tab is already saved
 */
gboolean
_gtr_tab_can_close (GtrTab * tab)
{
  return gtr_po_get_state (tab->priv->po) == GTR_PO_STATE_SAVED;
}

/**
 * gtr_tab_get_from_document:
 * @po: a #GtrPo
 *
 * Returns the #GtrTab for a specific #GtrPo.
 *
 * Returns: the #GtrTab for a specific #GtrPo
 */
GtrTab *
gtr_tab_get_from_document (GtrPo * po)
{
  gpointer res;

  g_return_val_if_fail (GTR_IS_PO (po), NULL);

  res = g_object_get_data (G_OBJECT (po), GTR_TAB_KEY);

  return (res != NULL) ? GTR_TAB (res) : NULL;
}

/**
 * gtr_tab_get_autosave_enabled:
 * @tab: a #GtrTab
 * 
 * Gets the current state for the autosave feature
 * 
 * Return value: TRUE if the autosave is enabled, else FALSE
 **/
gboolean
gtr_tab_get_autosave_enabled (GtrTab * tab)
{
  g_return_val_if_fail (GTR_IS_TAB (tab), FALSE);

  return tab->priv->autosave;
}

/**
 * gtr_tab_set_autosave_enabled:
 * @tab: a #GtrTab
 * @enable: enable (TRUE) or disable (FALSE) auto save
 * 
 * Enables or disables the autosave feature. It does not install an
 * autosave timeout if the document is new or is read-only
 **/
void
gtr_tab_set_autosave_enabled (GtrTab * tab, gboolean enable)
{
  g_return_if_fail (GTR_IS_TAB (tab));

  if (tab->priv->autosave == enable)
    return;

  tab->priv->autosave = enable;

  if (enable && (tab->priv->autosave_timeout <= 0))
    {
      install_autosave_timeout (tab);

      return;
    }

  if (!enable && (tab->priv->autosave_timeout > 0))
    {
      remove_autosave_timeout (tab);

      return;
    }

  g_return_if_fail (!enable && (tab->priv->autosave_timeout <= 0));
}

/**
 * gtr_tab_get_autosave_interval:
 * @tab: a #GtrTab
 * 
 * Gets the current interval for the autosaves
 * 
 * Return value: the value of the autosave
 **/
gint
gtr_tab_get_autosave_interval (GtrTab * tab)
{
  g_return_val_if_fail (GTR_IS_TAB (tab), 0);

  return tab->priv->autosave_interval;
}

/**
 * gtr_tab_set_autosave_interval:
 * @tab: a #GtrTab
 * @interval: the new interval
 * 
 * Sets the interval for the autosave feature. It does nothing if the
 * interval is the same as the one already present. It removes the old
 * interval timeout and adds a new one with the autosave passed as
 * argument.
 **/
void
gtr_tab_set_autosave_interval (GtrTab * tab, gint interval)
{
  g_return_if_fail (GTR_IS_TAB (tab));
  g_return_if_fail (interval > 0);

  if (tab->priv->autosave_interval == interval)
    return;

  tab->priv->autosave_interval = interval;

  if (!tab->priv->autosave)
    return;

  if (tab->priv->autosave_timeout > 0)
    {
      remove_autosave_timeout (tab);

      install_autosave_timeout (tab);
    }
}

/**
 * gtr_tab_add_widget_to_lateral_panel:
 * @tab: a #GtrTab
 * @widget: a #GtkWidget
 * @tab_name: the tab name in the notebook
 *
 * Adds a new widget to the laberal panel notebook.
 */
void
gtr_tab_add_widget_to_lateral_panel (GtrTab * tab,
                                     GtkWidget * widget,
                                     const gchar * tab_name)
{
  GtkWidget *label;

  g_return_if_fail (GTR_IS_TAB (tab));
  g_return_if_fail (GTK_IS_WIDGET (widget));

  label = gtk_label_new (tab_name);

  gtk_notebook_append_page (GTK_NOTEBOOK (tab->priv->lateral_panel),
                            widget, label);
}

/**
 * gtr_tab_remove_widget_from_lateral_panel:
 * @tab: a #GtrTab
 * @widget: a #GtkWidget
 *
 * Removes the @widget from the lateral panel notebook of @tab.
 */
void
gtr_tab_remove_widget_from_lateral_panel (GtrTab * tab, GtkWidget * widget)
{
  gint page;

  g_return_if_fail (GTR_IS_TAB (tab));
  g_return_if_fail (GTK_IS_WIDGET (widget));

  page = gtk_notebook_page_num (GTK_NOTEBOOK (tab->priv->lateral_panel),
                                widget);

  gtk_notebook_remove_page (GTK_NOTEBOOK (tab->priv->lateral_panel), page);
}

/**
 * gtr_tab_show_lateral_panel_widget:
 * @tab: a #GtrTab
 * @widget: the widget to be shown.
 *
 * Shows the notebook page of the @widget.
 */
void
gtr_tab_show_lateral_panel_widget (GtrTab * tab, GtkWidget * widget)
{
  gint page;

  page = gtk_notebook_page_num (GTK_NOTEBOOK (tab->priv->lateral_panel),
                                widget);
  gtk_notebook_set_current_page (GTK_NOTEBOOK (tab->priv->lateral_panel),
                                 page);
}

/**
 * gtr_tab_clear_msgstr_views:
 * @tab: a #GtrTab
 * 
 * Clears all text from msgstr text views.
 */
void
gtr_tab_clear_msgstr_views (GtrTab * tab)
{
  gint i = 0;
  GtrHeader *header;
  GtkTextBuffer *buf;

  g_return_if_fail (GTR_IS_TAB (tab));

  header = gtr_po_get_header (tab->priv->po);

  do
    {
      buf =
        gtk_text_view_get_buffer (GTK_TEXT_VIEW (tab->priv->trans_msgstr[i]));
      gtk_text_buffer_begin_user_action (buf);
      gtk_text_buffer_set_text (buf, "", -1);
      gtk_text_buffer_end_user_action (buf);
      i++;
    }
  while (i < gtr_header_get_nplurals (header));
}

/**
 * gtr_tab_copy_to_translation:
 * @tab: a #GtrTab
 *
 * Copies the text from the original text box to the translation text box.
 */
void
gtr_tab_copy_to_translation (GtrTab * tab)
{
  GtkTextBuffer *msgstr, *msgid;
  gint page_index;
  const gchar *text;
  GtkTextIter start, end;

  g_return_if_fail (GTR_IS_TAB (tab));

  page_index = gtr_tab_get_active_trans_tab (tab);

  msgstr =
    gtk_text_view_get_buffer (GTK_TEXT_VIEW
                              (tab->priv->trans_msgstr[page_index]));
  msgid = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tab->priv->text_msgid));

  gtk_text_buffer_begin_user_action (msgstr);
  gtk_text_buffer_get_bounds (msgid, &start, &end);
  text = gtk_text_buffer_get_text (msgid, &start, &end, FALSE);
  gtk_text_buffer_set_text (msgstr, text, -1);
  gtk_text_buffer_end_user_action (msgstr);
}

/**
 * gtr_tab_block_movement:
 * @tab: a #GtrTab
 *
 * Blocks the movement to the next/prev message.
 */
void
gtr_tab_block_movement (GtrTab * tab)
{
  g_return_if_fail (GTR_IS_TAB (tab));

  tab->priv->blocking = TRUE;
}

/**
 * gtr_tab_unblock_movement:
 * @tab: a #GtrTab
 * 
 * Unblocks the movement to the next/prev message.
 */
void
gtr_tab_unblock_movement (GtrTab * tab)
{
  g_return_if_fail (GTR_IS_TAB (tab));

  tab->priv->blocking = FALSE;
}

/**
 * gtr_tab_go_to_next:
 * @tab: a #GtrTab
 *
 * Moves to the next message or plural tab in case the message has plurals.
 */
void
gtr_tab_go_to_next (GtrTab * tab)
{
  GtrPo *po;

  po = gtr_tab_get_po (tab);

  gtr_tab_message_go_to (tab,
                         g_list_next
                         (gtr_po_get_current_message (po)),
                         FALSE, GTR_TAB_MOVE_NEXT);
}

/**
 * gtr_tab_go_to_prev:
 * @tab: a #GtrTab
 *
 * Moves to the previous message or plural tab in case the message has plurals.
 */
void
gtr_tab_go_to_prev (GtrTab * tab)
{
  GtrPo *po;

  po = gtr_tab_get_po (tab);

  gtr_tab_message_go_to (tab,
                         g_list_previous
                         (gtr_po_get_current_message (po)),
                         FALSE, GTR_TAB_MOVE_PREV);
}

/**
 * gtr_tab_go_to_first:
 * @tab: a #GtrTab
 *
 * Jumps to the first message.
 */
void
gtr_tab_go_to_first (GtrTab * tab)
{
  GtrPo *po;

  po = gtr_tab_get_po (tab);

  gtr_tab_message_go_to (tab,
                         g_list_first
                         (gtr_po_get_current_message (po)),
                         FALSE, GTR_TAB_MOVE_NONE);
}

/**
 * gtr_tab_go_to_last:
 * @tab: a #GtrTab 
 *
 * Jumps to the last message.
 */
void
gtr_tab_go_to_last (GtrTab * tab)
{
  GtrPo *po;

  po = gtr_tab_get_po (tab);

  gtr_tab_message_go_to (tab,
                         g_list_last
                         (gtr_po_get_current_message (po)),
                         FALSE, GTR_TAB_MOVE_NONE);
}

/**
 * gtr_tab_go_to_next_fuzzy:
 * @tab: a #GtrTab
 *
 * If there is a next fuzzy message it jumps to it.
 *
 * Returns: TRUE if there is a next fuzzy message.
 */
gboolean
gtr_tab_go_to_next_fuzzy (GtrTab * tab)
{
  GtrPo *po;
  GList *msg;

  po = gtr_tab_get_po (tab);

  msg = gtr_po_get_next_fuzzy (po);
  if (msg != NULL)
    {
      gtr_tab_message_go_to (tab, msg, FALSE, GTR_TAB_MOVE_NONE);
      return TRUE;
    }

  return FALSE;
}

/**
 * gtr_tab_go_to_prev_fuzzy:
 * @tab: a #GtrTab
 *
 * If there is a prev fuzzy message it jumps to it.
 *
 * Returns: TRUE if there is a prev fuzzy message.
 */
gboolean
gtr_tab_go_to_prev_fuzzy (GtrTab * tab)
{
  GtrPo *po;
  GList *msg;

  po = gtr_tab_get_po (tab);

  msg = gtr_po_get_prev_fuzzy (po);
  if (msg != NULL)
    {
      gtr_tab_message_go_to (tab, msg, FALSE, GTR_TAB_MOVE_NONE);
      return TRUE;
    }

  return FALSE;
}

/**
 * gtr_tab_go_to_next_untrans:
 * @tab: a #GtrTab
 *
 * If there is a next untranslated message it jumps to it.
 *
 * Returns: TRUE if there is a next untranslated message.
 */
gboolean
gtr_tab_go_to_next_untrans (GtrTab * tab)
{
  GtrPo *po;
  GList *msg;

  po = gtr_tab_get_po (tab);

  msg = gtr_po_get_next_untrans (po);
  if (msg != NULL)
    {
      gtr_tab_message_go_to (tab, msg, FALSE, GTR_TAB_MOVE_NONE);
      return TRUE;
    }

  return FALSE;
}

/**
 * gtr_tab_go_to_prev_untrans:
 * @tab: a #GtrTab
 *
 * If there is a prev untranslated message it jumps to it.
 *
 * Returns: TRUE if there is a prev untranslated message.
 */
gboolean
gtr_tab_go_to_prev_untrans (GtrTab * tab)
{
  GtrPo *po;
  GList *msg;

  po = gtr_tab_get_po (tab);

  msg = gtr_po_get_prev_untrans (po);
  if (msg != NULL)
    {
      gtr_tab_message_go_to (tab, msg, FALSE, GTR_TAB_MOVE_NONE);
      return TRUE;
    }

  return FALSE;
}

/**
 * gtr_tab_go_to_next_fuzzy_or_untrans:
 * @tab: a #GtrTab
 *
 * If there is a next fuzzy or untranslated message it jumps to it.
 *
 * Returns: TRUE if there is a next fuzzy or untranslated message.
 */
gboolean
gtr_tab_go_to_next_fuzzy_or_untrans (GtrTab * tab)
{
  GtrPo *po;
  GList *msg;

  po = gtr_tab_get_po (tab);

  msg = gtr_po_get_next_fuzzy_or_untrans (po);
  if (msg != NULL)
    {
      gtr_tab_message_go_to (tab, msg, FALSE, GTR_TAB_MOVE_NONE);
      return TRUE;
    }

  return FALSE;
}

/**
 * gtr_tab_go_to_prev_fuzzy_or_untrans:
 * @tab: a #GtrTab
 *
 * If there is a prev fuzzy or untranslated message it jumps to it.
 *
 * Returns: TRUE if there is a prev fuzzy or untranslated message.
 */
gboolean
gtr_tab_go_to_prev_fuzzy_or_untrans (GtrTab * tab)
{
  GtrPo *po;
  GList *msg;

  po = gtr_tab_get_po (tab);

  msg = gtr_po_get_prev_fuzzy_or_untrans (po);
  if (msg != NULL)
    {
      gtr_tab_message_go_to (tab, msg, FALSE, GTR_TAB_MOVE_NONE);
      return TRUE;
    }

  return FALSE;
}

/**
 * gtr_tab_go_to_number:
 * @tab: a #GtrTab
 * @number: the message number you want to jump
 *
 * Jumps to the message with the @number in the list, if the message does not
 * exists it does not jump.
 */
void
gtr_tab_go_to_number (GtrTab * tab, gint number)
{
  GtrPo *po;
  GList *msg;

  po = gtr_tab_get_po (tab);

  msg = gtr_po_get_msg_from_number (po, number);
  if (msg != NULL)
    {
      gtr_tab_message_go_to (tab, msg, FALSE, GTR_TAB_MOVE_NONE);
    }
}

/**
 * gtr_tab_set_info_bar:
 * @tab: a #GtrTab
 * @infobar: a #GtrMessageArea
 *
 * Sets the @infobar to be shown in the @tab.
 */
void
gtr_tab_set_info_bar (GtrTab * tab, GtkWidget * infobar)
{
  g_return_if_fail (GTR_IS_TAB (tab));

  if (tab->priv->infobar == infobar)
    return;

  if (tab->priv->infobar != NULL)
    gtk_widget_destroy (tab->priv->infobar);

  tab->priv->infobar = infobar;

  if (infobar == NULL)
    return;

  gtk_box_pack_start (GTK_BOX (tab), tab->priv->infobar, FALSE, FALSE, 0);

  g_object_add_weak_pointer (G_OBJECT (tab->priv->infobar),
                             (gpointer *) & tab->priv->infobar);
}
