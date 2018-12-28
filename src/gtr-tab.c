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
#include "gtr-tab-activatable.h"
#include "gtr-tab.h"
#include "gtr-po.h"
#include "gtr-settings.h"
#include "gtr-view.h"
#include "gtr-dirs.h"
#include "gtr-debug.h"
#include "gtr-window.h"
#include "gtr-progress.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

#define MAX_PLURALS 6

#define GTR_TAB_KEY "GtrTabFromDocument"

typedef struct
{
  GSettings *ui_settings;
  GSettings *files_settings;
  GSettings *editor_settings;
  GSettings *state_settings;

  GtkWidget *progress_eventbox;
  GtkWidget *progress_box;
  GtkWidget *progress_revealer;
  GtkWidget *progress_percentage;
  GtkWidget *progress_trans;
  GtkWidget *progress_fuzzy;
  GtkWidget *progress_untrans;

  GtrProgress *progress;
  gboolean find_replace_flag;

  GtrPo *po;

  GtkWidget *dock;

  GtkWidget *message_table;
  GtkWidget *context;

  /*Info bar */
  GtkWidget *infobar;

  /*Original text */
  GtkWidget *text_msgid;
  GtkWidget *text_plural_scroll;
  GtkWidget *text_msgid_plural;
  GtkWidget *msgid_tags;
  GtkWidget *msgid_ctxt;
  GtkWidget *msgid_ctxt_label;

  /*Translated text */
  GtkWidget *msgstr_label;
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

  guint tab_realized : 1;
  guint dispose_has_run : 1;
} GtrTabPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GtrTab, gtr_tab, GTK_TYPE_BOX)

enum
{
  SHOWED_MESSAGE,
  MESSAGE_CHANGED,
  MESSAGE_EDITION_FINISHED,
  SELECTION_CHANGED,
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

static gboolean
show_hide_revealer (GtkWidget *widget, GdkEvent *ev, GtrTab *tab)
{
  GtrTabPrivate *priv;
  GtkRevealer *rev;

  priv = gtr_tab_get_instance_private (tab);
  rev = GTK_REVEALER (priv->progress_revealer);
  gtk_revealer_set_reveal_child (rev, !gtk_revealer_get_reveal_child (rev));

  return TRUE;
}

static gboolean
msg_grab_focus (GtrTab *tab)
{
  GtrTabPrivate *priv;
  priv = gtr_tab_get_instance_private (tab);
  gtk_widget_grab_focus (priv->trans_msgstr[0]);
  return FALSE;
}

static void
install_autosave_timeout (GtrTab * tab)
{
  GtrTabPrivate *priv;
  gint timeout;

  priv = gtr_tab_get_instance_private (tab);
  g_return_if_fail (priv->autosave_timeout <= 0);
  g_return_if_fail (priv->autosave);
  g_return_if_fail (priv->autosave_interval > 0);

  /* Add a new timeout */
  timeout = g_timeout_add (priv->autosave_interval * 1000 * 60,
                           (GSourceFunc) gtr_tab_autosave, tab);

  priv->autosave_timeout = timeout;
}

static gboolean
install_autosave_timeout_if_needed (GtrTab * tab)
{
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);
  g_return_val_if_fail (priv->autosave_timeout <= 0, FALSE);

  if (priv->autosave)
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
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);
  if (!(gtr_po_get_state (priv->po) == GTR_PO_STATE_MODIFIED))
    return TRUE;

  gtr_po_save_file (priv->po, &error);
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
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);
  g_return_if_fail (priv->autosave_timeout > 0);

  g_source_remove (priv->autosave_timeout);
  priv->autosave_timeout = 0;
}

static void
gtr_tab_edition_finished (GtrTab * tab, GtrMsg * msg)
{
  gchar *message_error;
  GtkWidget *infobar;

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
  GtrTabPrivate *priv;
  const gchar *check;
  gchar *translation;
  gboolean unmark_fuzzy;
  gint i;

  priv = gtr_tab_get_instance_private (tab);

  /* Work out which message this is associated with */
  header = gtr_po_get_header (priv->po);

  msg_aux = gtr_po_get_current_message (priv->po);
  msg = msg_aux->data;
  buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (priv->trans_msgstr[0]));
  unmark_fuzzy = g_settings_get_boolean (priv->editor_settings,
                                         GTR_SETTINGS_UNMARK_FUZZY_WHEN_CHANGED);

  unmark_fuzzy = unmark_fuzzy && !priv->find_replace_flag;
  if (gtr_msg_is_fuzzy (msg) && unmark_fuzzy)
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
      if (!priv->trans_msgstr[i])
        {
          break;
        }

      /* Have we reached the one we want yet? */
      buf =
        gtk_text_view_get_buffer (GTK_TEXT_VIEW (priv->trans_msgstr[i]));
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
                            GtkWidget * box, gboolean spellcheck,
                            GtrTab *tab)
{
  GtkWidget *scroll;
  GtkWidget *label;
  GtkWidget *widget;
  GtrTabPrivate *priv;

  label = gtk_label_new (tab_label);

  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scroll);

  widget = gtr_view_new ();
  gtk_widget_show (widget);

  priv = gtr_tab_get_instance_private (tab);

  if (spellcheck &&
      g_settings_get_boolean (priv->editor_settings,
                              GTR_SETTINGS_SPELLCHECK))
    {
      gtr_view_enable_spellcheck (GTR_VIEW (widget), spellcheck);
    }

  gtk_container_add (GTK_CONTAINER (scroll), widget);

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
  GtrTabPrivate *priv;
  const gchar *msgstr_plural;
  gint i;

  g_return_if_fail (tab != NULL);
  g_return_if_fail (msg != NULL);

  priv = gtr_tab_get_instance_private (tab);

  header = gtr_po_get_header (priv->po);

  for (i = 0; i < gtr_header_get_nplurals (header); i++)
    {
      msgstr_plural = gtr_msg_get_msgstr_plural (msg, i);
      if (msgstr_plural)
        {
          buf =
            gtk_text_view_get_buffer (GTK_TEXT_VIEW
                                      (priv->trans_msgstr[i]));
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
  GtrTabPrivate *priv;
  GtrPo *po;
  GtkTextBuffer *buf;
  const gchar *msgid, *msgid_plural;
  const gchar *msgstr;
  const gchar *msgctxt;

  g_return_if_fail (GTR_IS_TAB (tab));

  priv = gtr_tab_get_instance_private (tab);
  gtk_label_set_text (GTK_LABEL (priv->msgid_tags), "");

  msgctxt = gtr_msg_get_msgctxt (msg);
  if (msgctxt)
   {
    gtk_label_set_text (GTK_LABEL (priv->msgid_ctxt_label), _("Context: "));
    gtk_label_set_text (GTK_LABEL (priv->msgid_ctxt), msgctxt);
   }
  else
   {
    gtk_label_set_text (GTK_LABEL (priv->msgid_ctxt_label), "");
    gtk_label_set_text (GTK_LABEL (priv->msgid_ctxt), "");
   }

  po = priv->po;
  gtr_po_update_current_message (po, msg);
  msgid = gtr_msg_get_msgid (msg);
  if (msgid)
    {
      buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (priv->text_msgid));
      gtk_source_buffer_begin_not_undoable_action (GTK_SOURCE_BUFFER (buf));
      gtk_text_buffer_set_text (buf, (gchar *) msgid, -1);
      gtk_source_buffer_end_not_undoable_action (GTK_SOURCE_BUFFER (buf));

      if (gtr_msg_is_fuzzy (msg))
        gtk_label_set_text (GTK_LABEL (priv->msgid_tags), _("fuzzy"));
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
      gtk_notebook_set_show_tabs (GTK_NOTEBOOK (priv->trans_notebook),
                                  TRUE);
      buf =
        gtk_text_view_get_buffer (GTK_TEXT_VIEW
                                  (priv->text_msgid_plural));
      gtk_text_buffer_set_text (buf, (gchar *) msgid_plural, -1);
      gtr_message_plural_forms (tab, msg);
    }
}

static void
emit_message_changed_signal (GtkTextBuffer * buf, GtrTab * tab)
{
  GList *msg;
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);
  msg = gtr_po_get_current_message (priv->po);

  g_signal_emit (G_OBJECT (tab), signals[MESSAGE_CHANGED], 0, msg->data);
}

static void
emit_selection_changed (GtkTextBuffer * buf, GParamSpec * spec, GtrTab * tab)
{
  g_signal_emit (G_OBJECT (tab), signals[SELECTION_CHANGED], 0);
}

static void
update_status (GtrTab * tab, GtrMsg * msg, gpointer useless)
{
  GtrMsgStatus status;
  GtrPoState po_state;
  GtrTabPrivate *priv;
  gboolean fuzzy, translated;

  priv = gtr_tab_get_instance_private (tab);

  status = gtr_msg_get_status (msg);
  po_state = gtr_po_get_state (priv->po);

  fuzzy = gtr_msg_is_fuzzy (msg);
  translated = gtr_msg_is_translated (msg);

  if ((status == GTR_MSG_STATUS_FUZZY) && !fuzzy && !priv->find_replace_flag)
    {
      _gtr_po_increase_decrease_fuzzy (priv->po, FALSE);
      if (translated)
        {
          status = GTR_MSG_STATUS_TRANSLATED;
          _gtr_po_increase_decrease_translated (priv->po, TRUE);
        }
      else
        {
          status = GTR_MSG_STATUS_UNTRANSLATED;
        }
    }
  else if ((status == GTR_MSG_STATUS_TRANSLATED) && !translated)
    {
      status = GTR_MSG_STATUS_UNTRANSLATED;
      _gtr_po_increase_decrease_translated (priv->po, FALSE);
    }
  else if ((status == GTR_MSG_STATUS_TRANSLATED) && fuzzy)
    {
      status = GTR_MSG_STATUS_FUZZY;
      _gtr_po_increase_decrease_translated (priv->po, FALSE);
      _gtr_po_increase_decrease_fuzzy (priv->po, TRUE);
    }
  else if ((status == GTR_MSG_STATUS_UNTRANSLATED) && translated)
    {
      if (fuzzy)
        {
          status = GTR_MSG_STATUS_FUZZY;
          _gtr_po_increase_decrease_fuzzy (priv->po, TRUE);
        }
      else
        {
          status = GTR_MSG_STATUS_TRANSLATED;
          _gtr_po_increase_decrease_translated (priv->po, TRUE);
        }
    }

  gtr_msg_set_status (msg, status);
  if (gtr_msg_is_fuzzy (msg))
    gtk_label_set_text (GTK_LABEL (priv->msgid_tags), _("fuzzy"));
  else
    gtk_label_set_text (GTK_LABEL (priv->msgid_tags), "");

  /* We need to update the tab state too if is neccessary */
  if (po_state != GTR_PO_STATE_MODIFIED)
    gtr_po_set_state (priv->po, GTR_PO_STATE_MODIFIED);
}

static void
gtr_tab_add_msgstr_tabs (GtrTab * tab)
{
  GtrHeader *header;
  GtrTabPrivate *priv;
  gchar *label;
  GtkTextBuffer *buf;
  gint i = 0;
  gchar *lang_code = NULL;

  priv = gtr_tab_get_instance_private (tab);

  /*
   * We get the header of the po file
   */
  header = gtr_po_get_header (priv->po);
  lang_code = gtr_header_get_language_code (header);

  do
    {

      label = g_strdup_printf (_("Plural %d"), i);
      priv->trans_msgstr[i] = gtr_tab_append_msgstr_page (label,
                                                          priv->trans_notebook,
                                                          TRUE,
                                                          tab);

      gtr_view_set_language (GTR_VIEW (priv->trans_msgstr[i]), lang_code);

      buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (priv->trans_msgstr[i]));
      g_signal_connect (buf, "end-user-action",
                        G_CALLBACK (gtr_message_translation_update), tab);

      g_signal_connect_after (buf, "end_user_action",
                              G_CALLBACK (emit_message_changed_signal), tab);
      g_signal_connect (buf, "notify::has-selection",
                        G_CALLBACK (emit_selection_changed), tab);
      i++;
      g_free (label);
    }
  while (i < gtr_header_get_nplurals (header));
  g_free (lang_code);
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
gtr_tab_init (GtrTab * tab)
{
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);

  gtk_widget_init_template (GTK_WIDGET (tab));

  priv->ui_settings = g_settings_new ("org.gnome.gtranslator.preferences.ui");
  priv->files_settings = g_settings_new ("org.gnome.gtranslator.preferences.files");
  priv->editor_settings = g_settings_new ("org.gnome.gtranslator.preferences.editor");
  priv->state_settings = g_settings_new ("org.gnome.gtranslator.state.window");

  g_signal_connect (tab, "message-changed", G_CALLBACK (update_status), NULL);

  /* Manage auto save data */
  priv->autosave = g_settings_get_boolean (priv->files_settings,
                                           GTR_SETTINGS_AUTO_SAVE);
  priv->autosave = (priv->autosave != FALSE);

  priv->autosave_interval = g_settings_get_int (priv->files_settings,
                                                GTR_SETTINGS_AUTO_SAVE_INTERVAL);
  if (priv->autosave_interval <= 0)
    priv->autosave_interval = 1;

  priv->find_replace_flag = FALSE;
  priv->progress = gtr_progress_new ();
  gtk_widget_show (GTK_WIDGET (priv->progress));
  gtk_container_add (GTK_CONTAINER (priv->progress_box), GTK_WIDGET (priv->progress));

  g_signal_connect (priv->progress_eventbox, "button-press-event",
                    G_CALLBACK (show_hide_revealer), tab);
}

static void
gtr_tab_finalize (GObject * object)
{
  GtrTab *tab = GTR_TAB (object);
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);

  if (priv->timer != NULL)
    g_timer_destroy (priv->timer);

  if (priv->autosave_timeout > 0)
    remove_autosave_timeout (tab);

  G_OBJECT_CLASS (gtr_tab_parent_class)->finalize (object);
}

static void
gtr_tab_dispose (GObject * object)
{
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (GTR_TAB (object));

  g_clear_object (&priv->po);
  g_clear_object (&priv->ui_settings);
  g_clear_object (&priv->files_settings);
  g_clear_object (&priv->editor_settings);
  g_clear_object (&priv->state_settings);

  G_OBJECT_CLASS (gtr_tab_parent_class)->dispose (object);
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
gtr_tab_realize (GtkWidget *widget)
{
  GTK_WIDGET_CLASS (gtr_tab_parent_class)->realize (widget);
}

static void
gtr_tab_class_init (GtrTabClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gtr_tab_finalize;
  object_class->dispose = gtr_tab_dispose;
  object_class->set_property = gtr_tab_set_property;
  object_class->get_property = gtr_tab_get_property;

  widget_class->realize = gtr_tab_realize;

  klass->message_edition_finished = gtr_tab_edition_finished;

  /* Signals */
  signals[SHOWED_MESSAGE] =
    g_signal_new ("showed-message",
                  G_OBJECT_CLASS_TYPE (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GtrTabClass, showed_message),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1, GTR_TYPE_MSG);

  signals[MESSAGE_CHANGED] =
    g_signal_new ("message-changed",
                  G_OBJECT_CLASS_TYPE (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GtrTabClass, message_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1, GTR_TYPE_MSG);

  signals[MESSAGE_EDITION_FINISHED] =
    g_signal_new ("message-edition-finished",
                  G_OBJECT_CLASS_TYPE (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GtrTabClass, message_edition_finished),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1, GTR_TYPE_MSG);
  signals[SELECTION_CHANGED] =
    g_signal_new ("selection-changed",
                  G_OBJECT_CLASS_TYPE (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GtrTabClass, selection_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

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

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gnome/translator/gtr-tab.ui");

  gtk_widget_class_bind_template_child_private (widget_class, GtrTab, message_table);
  gtk_widget_class_bind_template_child_private (widget_class, GtrTab, text_msgid);
  gtk_widget_class_bind_template_child_private (widget_class, GtrTab, msgid_tags);
  gtk_widget_class_bind_template_child_private (widget_class, GtrTab, msgid_ctxt);
  gtk_widget_class_bind_template_child_private (widget_class, GtrTab, msgid_ctxt_label);
  gtk_widget_class_bind_template_child_private (widget_class, GtrTab, text_plural_scroll);
  gtk_widget_class_bind_template_child_private (widget_class, GtrTab, text_msgid_plural);
  gtk_widget_class_bind_template_child_private (widget_class, GtrTab, msgstr_label);
  gtk_widget_class_bind_template_child_private (widget_class, GtrTab, trans_notebook);
  gtk_widget_class_bind_template_child_private (widget_class, GtrTab, context);

  gtk_widget_class_bind_template_child_private (widget_class, GtrTab, progress_eventbox);
  gtk_widget_class_bind_template_child_private (widget_class, GtrTab, progress_box);
  gtk_widget_class_bind_template_child_private (widget_class, GtrTab, progress_revealer);
  gtk_widget_class_bind_template_child_private (widget_class, GtrTab, progress_trans);
  gtk_widget_class_bind_template_child_private (widget_class, GtrTab, progress_fuzzy);
  gtk_widget_class_bind_template_child_private (widget_class, GtrTab, progress_untrans);
  gtk_widget_class_bind_template_child_private (widget_class, GtrTab, progress_percentage);

  g_type_ensure (gtr_view_get_type ());
  g_type_ensure (gtr_context_panel_get_type ());
  g_type_ensure (gtr_message_table_get_type ());
}

/***************************** Public funcs ***********************************/

/**
 * gtr_tab_new:
 * @po: a #GtrPo
 * @window: a #GtkWindow
 * 
 * Creates a new #GtrTab.
 * 
 * Return value: a new #GtrTab object
 **/
GtrTab *
gtr_tab_new (GtrPo * po,
             GtkWindow *window)
{
  GtrTab *tab;
  GtrTabPrivate *priv;

  g_return_val_if_fail (po != NULL, NULL);

  tab = g_object_new (GTR_TYPE_TAB, NULL);

  priv = gtr_tab_get_instance_private (tab);
  gtr_context_init_tm (GTR_CONTEXT_PANEL (priv->context),
                       gtr_window_get_tm (GTR_WINDOW (window)));

  /* FIXME: make the po a property */
  priv->po = po;
  g_object_set_data (G_OBJECT (po), GTR_TAB_KEY, tab);

  g_signal_connect (po, "notify::location",
                    G_CALLBACK (on_location_notify), tab);

  g_signal_connect (po, "notify::state",
                    G_CALLBACK (on_state_notify), tab);

  install_autosave_timeout_if_needed (tab);

  /* Now we have to initialize the number of msgstr tabs */
  gtr_tab_add_msgstr_tabs (tab);

  gtr_message_table_populate (GTR_MESSAGE_TABLE (priv->message_table),
                              GTR_MESSAGE_CONTAINER (priv->po));

  gtk_widget_show (GTK_WIDGET (tab));
  return tab;
}

/**
 * gtr_tab_get_po:
 * @tab: a #GtrTab
 *
 * Return value: (transfer none): the #GtrPo stored in the #GtrTab
**/
GtrPo *
gtr_tab_get_po (GtrTab * tab)
{
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);
  return priv->po;
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
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);
  return
    gtk_notebook_get_current_page (GTK_NOTEBOOK (priv->trans_notebook));
}

/**
 * gtr_tab_get_context_panel:
 * @tab: a #GtrTab
 *
 * Return value: (transfer none): the #GtranslaorContextPanel
 */
GtrContextPanel *
gtr_tab_get_context_panel (GtrTab * tab)
{
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);
  return GTR_CONTEXT_PANEL (priv->context);
}

/**
 * gtr_tab_get_active_view:
 * @tab: a #GtranslationTab
 *
 * Return value: (transfer none): the active page of the translation notebook.
**/
GtrView *
gtr_tab_get_active_view (GtrTab * tab)
{
  GtrTabPrivate *priv;
  gint num;

  priv = gtr_tab_get_instance_private (tab);

  num =
    gtk_notebook_get_current_page (GTK_NOTEBOOK (priv->trans_notebook));
  return GTR_VIEW (priv->trans_msgstr[num]);
}

/**
 * gtr_tab_get_all_views:
 * @tab: the #GtranslationTab
 * @original: TRUE if you want original TextViews.
 * @translated: TRUE if you want tranlated TextViews.
 *
 * Returns all the views currently present in #GtranslationTab
 *
 * Returns: (transfer container) (element-type Gtranslator.View):
 *          a newly allocated list of #GtranslationTab objects
 */
GList *
gtr_tab_get_all_views (GtrTab * tab, gboolean original, gboolean translated)
{
  GList *ret = NULL;
  GtrTabPrivate *priv;
  gint i = 0;

  g_return_val_if_fail (GTR_IS_TAB (tab), NULL);

  priv = gtr_tab_get_instance_private (tab);
  if (original)
    {
      ret = g_list_append (ret, priv->text_msgid);
      ret = g_list_append (ret, priv->text_msgid_plural);
    }

  if (translated)
    {
      while (i < MAX_PLURALS)
        {
          if (priv->trans_msgstr[i])
            ret = g_list_append (ret, priv->trans_msgstr[i]);
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
                       GtrMsg * to_go, gboolean searching, GtrTabMove move)
{
  static gboolean first_msg = TRUE;
  GtrTabPrivate *priv;

  g_return_if_fail (tab != NULL);
  g_return_if_fail (GTR_IS_MSG (to_go));

  priv = gtr_tab_get_instance_private (tab);

  if (!priv->blocking || first_msg)
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
        gtk_notebook_get_show_tabs (GTK_NOTEBOOK (priv->trans_notebook));
      current_page =
        gtk_notebook_get_current_page (GTK_NOTEBOOK
                                       (priv->trans_notebook));
      n_pages =
        gtk_notebook_get_n_pages (GTK_NOTEBOOK (priv->trans_notebook));
      if ((plurals == TRUE) && (move != GTR_TAB_MOVE_NONE))
        {
          if ((n_pages - 1) == current_page && move == GTR_TAB_MOVE_NEXT)
            {
              gtk_notebook_set_current_page (GTK_NOTEBOOK
                                             (priv->trans_notebook), 0);
              gtr_tab_show_message (tab, to_go);
            }
          else if (current_page == 0 && move == GTR_TAB_MOVE_PREV)
            {
              gtk_notebook_set_current_page (GTK_NOTEBOOK
                                             (priv->trans_notebook),
                                             n_pages - 1);
              gtr_tab_show_message (tab, to_go);
            }
          else
            {
              if (move == GTR_TAB_MOVE_NEXT)
                gtk_notebook_set_current_page (GTK_NOTEBOOK
                                               (priv->trans_notebook),
                                               current_page + 1);
              else
                gtk_notebook_set_current_page (GTK_NOTEBOOK
                                               (priv->trans_notebook),
                                               current_page - 1);
              return;
            }
        }
      else
        gtr_tab_show_message (tab, to_go);
      first_msg = FALSE;
    }
  else
    return;

  // Grabbing the focus in the GtrView to edit the message
  // This is done in the idle add to avoid the focus grab from the
  // message-table
  g_idle_add((GSourceFunc)msg_grab_focus, tab);

  /*
   * Emitting showed-message signal
   */
  if (!searching)
    g_signal_emit (G_OBJECT (tab), signals[SHOWED_MESSAGE], 0,
                   GTR_MSG (to_go));
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
  GtrTabPrivate *priv;
  const gchar *str;
  gchar *tab_name;

  priv = gtr_tab_get_instance_private (tab);

  header = gtr_po_get_header (priv->po);
  state = gtr_po_get_state (priv->po);

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
  GtrTabPrivate *priv;
  gchar *tooltips;
  gchar *path;

  priv = gtr_tab_get_instance_private (tab);

  location = gtr_po_get_location (priv->po);
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
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);
  return gtr_po_get_state (priv->po) == GTR_PO_STATE_SAVED;
}

/**
 * gtr_tab_get_from_document:
 * @po: a #GtrPo
 *
 * Returns the #GtrTab for a specific #GtrPo.
 *
 * Returns: (transfer none): the #GtrTab for a specific #GtrPo
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
  GtrTabPrivate *priv;

  g_return_val_if_fail (GTR_IS_TAB (tab), FALSE);

  priv = gtr_tab_get_instance_private (tab);
  return priv->autosave;
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
  GtrTabPrivate *priv;

  g_return_if_fail (GTR_IS_TAB (tab));

  priv = gtr_tab_get_instance_private (tab);

  if (priv->autosave == enable)
    return;

  priv->autosave = enable;

  if (enable && (priv->autosave_timeout <= 0))
    {
      install_autosave_timeout (tab);

      return;
    }

  if (!enable && (priv->autosave_timeout > 0))
    {
      remove_autosave_timeout (tab);

      return;
    }

  g_return_if_fail (!enable && (priv->autosave_timeout <= 0));
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
  GtrTabPrivate *priv;

  g_return_val_if_fail (GTR_IS_TAB (tab), 0);

  priv = gtr_tab_get_instance_private (tab);
  return priv->autosave_interval;
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
  GtrTabPrivate *priv;

  g_return_if_fail (GTR_IS_TAB (tab));
  g_return_if_fail (interval > 0);

  priv = gtr_tab_get_instance_private (tab);

  if (priv->autosave_interval == interval)
    return;

  priv->autosave_interval = interval;

  if (!priv->autosave)
    return;

  if (priv->autosave_timeout > 0)
    {
      remove_autosave_timeout (tab);

      install_autosave_timeout (tab);
    }
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
  GtrTabPrivate *priv;

  g_return_if_fail (GTR_IS_TAB (tab));

  priv = gtr_tab_get_instance_private (tab);

  header = gtr_po_get_header (priv->po);

  do
    {
      buf =
        gtk_text_view_get_buffer (GTK_TEXT_VIEW (priv->trans_msgstr[i]));
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
  gchar *text;
  GtkTextIter start, end;
  GtrTabPrivate *priv;

  g_return_if_fail (GTR_IS_TAB (tab));

  priv = gtr_tab_get_instance_private (tab);

  page_index = gtr_tab_get_active_trans_tab (tab);

  msgstr =
    gtk_text_view_get_buffer (GTK_TEXT_VIEW
                              (priv->trans_msgstr[page_index]));
  msgid = gtk_text_view_get_buffer (GTK_TEXT_VIEW (priv->text_msgid));

  gtk_text_buffer_begin_user_action (msgstr);
  gtk_text_buffer_get_bounds (msgid, &start, &end);
  text = gtk_text_buffer_get_text (msgid, &start, &end, FALSE);
  gtk_text_buffer_set_text (msgstr, text, -1);
  g_free (text);
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
  GtrTabPrivate *priv;

  g_return_if_fail (GTR_IS_TAB (tab));

  priv = gtr_tab_get_instance_private (tab);
  priv->blocking = TRUE;
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
  GtrTabPrivate *priv;

  g_return_if_fail (GTR_IS_TAB (tab));

  priv = gtr_tab_get_instance_private (tab);
  priv->blocking = FALSE;
}

static gboolean
_gtr_tab_finish_edition (GtrTab * tab)
{
  GList *current_msg;
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);

  current_msg = gtr_po_get_current_message (priv->po);

  /* movement is blocked/unblocked within the handler */
  g_signal_emit (G_OBJECT (tab), signals[MESSAGE_EDITION_FINISHED],
		 0, GTR_MSG (current_msg->data));

  return !priv->blocking;
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
  GtrMsg *msg;
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);

  if (_gtr_tab_finish_edition (tab))
    {
      msg = gtr_message_table_navigate (GTR_MESSAGE_TABLE (priv->message_table),
                                        GTR_NAVIGATE_NEXT, NULL);
      if (msg)
        gtr_tab_message_go_to (tab, msg,
                               FALSE, GTR_TAB_MOVE_NEXT);
    }
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
  GtrMsg *msg;
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);

  if (_gtr_tab_finish_edition (tab))
    {
      msg = gtr_message_table_navigate (GTR_MESSAGE_TABLE (priv->message_table),
                                        GTR_NAVIGATE_PREV, NULL);
      if (msg)
        gtr_tab_message_go_to (tab, msg,
                               FALSE, GTR_TAB_MOVE_PREV);
    }
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
  GtrMsg *msg;
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);

  if (_gtr_tab_finish_edition (tab))
    {
      msg = gtr_message_table_navigate (GTR_MESSAGE_TABLE (priv->message_table),
                                        GTR_NAVIGATE_FIRST, NULL);
      if (msg)
        gtr_tab_message_go_to (tab, msg,
                               FALSE, GTR_TAB_MOVE_NONE);
    }
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
  GtrMsg *msg;
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);

  if (_gtr_tab_finish_edition (tab))
    {
      msg = gtr_message_table_navigate (GTR_MESSAGE_TABLE (priv->message_table),
                                        GTR_NAVIGATE_LAST, NULL);
      if (msg)
        gtr_tab_message_go_to (tab, msg,
                               FALSE, GTR_TAB_MOVE_NONE);
    }
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
  GtrMsg *msg;
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);

  if (!_gtr_tab_finish_edition (tab))
    return FALSE;

  msg = gtr_message_table_navigate (GTR_MESSAGE_TABLE (priv->message_table),
                                    GTR_NAVIGATE_NEXT,
                                    gtr_msg_is_fuzzy);
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
  GtrMsg *msg;
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);

  if (!_gtr_tab_finish_edition (tab))
    return FALSE;

  msg = gtr_message_table_navigate (GTR_MESSAGE_TABLE (priv->message_table),
                                    GTR_NAVIGATE_PREV,
                                    gtr_msg_is_fuzzy);
  if (msg != NULL)
    {
      gtr_tab_message_go_to (tab, msg, FALSE, GTR_TAB_MOVE_NONE);
      return TRUE;
    }

  return FALSE;
}

static gboolean
message_is_untranslated (GtrMsg * msg)
{
  return !gtr_msg_is_translated (msg);
}

static gboolean
message_is_fuzzy_or_untranslated (GtrMsg * msg)
{
  return gtr_msg_is_fuzzy (msg) || !gtr_msg_is_translated (msg);
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
  GtrMsg *msg;
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);

  if (!_gtr_tab_finish_edition (tab))
    return FALSE;

  msg = gtr_message_table_navigate (GTR_MESSAGE_TABLE (priv->message_table),
                                    GTR_NAVIGATE_NEXT,
                                    message_is_untranslated);
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
  GtrMsg *msg;
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);

  if (!_gtr_tab_finish_edition (tab))
    return FALSE;

  msg = gtr_message_table_navigate (GTR_MESSAGE_TABLE (priv->message_table),
                                    GTR_NAVIGATE_PREV,
                                    message_is_untranslated);
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
  GtrMsg *msg;
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);

  if (!_gtr_tab_finish_edition (tab))
    return FALSE;

  msg = gtr_message_table_navigate (GTR_MESSAGE_TABLE (priv->message_table),
                                    GTR_NAVIGATE_NEXT,
                                    message_is_fuzzy_or_untranslated);
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
  GtrMsg *msg;
  GtrTabPrivate *priv;

  priv = gtr_tab_get_instance_private (tab);

  if (!_gtr_tab_finish_edition (tab))
    return FALSE;

  msg = gtr_message_table_navigate (GTR_MESSAGE_TABLE (priv->message_table),
                                    GTR_NAVIGATE_PREV,
                                    message_is_fuzzy_or_untranslated);
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

  if (!_gtr_tab_finish_edition (tab))
    return;

  po = gtr_tab_get_po (tab);
  msg = gtr_po_get_msg_from_number (po, number);
  if (msg != NULL)
    {
      gtr_tab_message_go_to (tab, msg->data, FALSE, GTR_TAB_MOVE_NONE);
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
  GtrTabPrivate *priv;

  g_return_if_fail (GTR_IS_TAB (tab));

  priv = gtr_tab_get_instance_private (tab);

  if (priv->infobar == infobar)
    return;

  if (priv->infobar != NULL)
    gtk_widget_destroy (priv->infobar);

  priv->infobar = infobar;

  if (infobar == NULL)
    return;

  gtk_box_pack_start (GTK_BOX (tab), priv->infobar, FALSE, FALSE, 0);

  g_object_add_weak_pointer (G_OBJECT (priv->infobar),
                             (gpointer *) & priv->infobar);
}

GtrMsg *
gtr_tab_get_msg (GtrTab *tab)
{
  GtrTabPrivate *priv;
  GList *msg_aux;
  GtrMsg *msg;

  priv = gtr_tab_get_instance_private (tab);
  msg_aux = gtr_po_get_current_message (priv->po);
  msg = msg_aux->data;

  return msg;
}

void
gtr_tab_set_progress (GtrTab      *tab,
                      gint         trans,
                      gint         untrans,
                      gint         fuzzy)
{
  GtrTabPrivate *priv = gtr_tab_get_instance_private (tab);
  gchar *percentage, *trans_text, *fuzzy_text, *untrans_text;

  gtr_progress_set (priv->progress, trans, untrans, fuzzy);

  percentage = g_strdup_printf (_("Translated: %0.2f%%"), (float)trans * 100 / (float)(trans + untrans + fuzzy));
  trans_text = g_strdup_printf (_("Translated: %d"), trans);
  untrans_text = g_strdup_printf (_("Untranslated: %d"), untrans);
  fuzzy_text = g_strdup_printf (_("Fuzzy: %d"), fuzzy);

  gtk_label_set_text (GTK_LABEL (priv->progress_percentage), percentage);
  gtk_label_set_text (GTK_LABEL (priv->progress_fuzzy), fuzzy_text);
  gtk_label_set_text (GTK_LABEL (priv->progress_untrans), untrans_text);
  gtk_label_set_text (GTK_LABEL (priv->progress_trans), trans_text);

  g_free (percentage);
  g_free (trans_text);
  g_free (fuzzy_text);
  g_free (untrans_text);
}

void
gtr_tab_sort_by (GtrTab *tab,
                 GtrMessageTableSortBy sort)
{
  GtrTabPrivate *priv;
  priv = gtr_tab_get_instance_private (tab);
  gtr_message_table_sort_by (GTR_MESSAGE_TABLE (priv->message_table), sort);
}

void
gtr_tab_find_replace (GtrTab *tab,
                      gboolean set)
{
  GtrTabPrivate *priv;
  priv = gtr_tab_get_instance_private (tab);
  priv->find_replace_flag = set;
}

