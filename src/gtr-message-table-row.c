/*
 * Copyright (C) 2023  Daniel García Moreno <danigm@gnome.org>
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

#include "gtr-message-table-row.h"
#include "gtr-msg.h"
#include "gtr-settings.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>

enum
{
  PROP_0,
  PROP_MSG,
};

typedef struct
{
  GtkWidget *id;
  GtkWidget *original;
  GtkWidget *translated;

  GtrMsg *msg;
  GSettings *ui_settings;
} GtrMessageTableRowPrivate;

struct _GtrMessageTableRow
{
  GtkBox parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrMessageTableRow, gtr_message_table_row, GTK_TYPE_BOX)


static void
update_cell_style (GtkWidget *cell, GtrMsg *msg)
{
  switch (gtr_msg_get_status (msg))
    {
      case GTR_MSG_STATUS_UNTRANSLATED:
        gtk_widget_add_css_class (cell, "untranslated");
        gtk_widget_remove_css_class (cell, "fuzzy");
        break;
      case GTR_MSG_STATUS_FUZZY:
        gtk_widget_add_css_class (cell, "fuzzy");
        gtk_widget_remove_css_class (cell, "untranslated");
        break;
      case GTR_MSG_STATUS_TRANSLATED:
      default:
        gtk_widget_remove_css_class (cell, "fuzzy");
        gtk_widget_remove_css_class (cell, "untranslated");
        break;
    }
}

static void
update_msg (GtrMsg             *msg,
            GParamSpec         *pspec,
            GtrMessageTableRow *row)
{
  GtrMessageTableRowPrivate *priv = NULL;
  g_autofree char *pos = NULL;
  const char *text = NULL;

  priv = gtr_message_table_row_get_instance_private (row);

  text = gtr_msg_get_msgid (msg);
  gtk_label_set_label (GTK_LABEL (priv->original), text);
  text = gtr_msg_get_msgstr (msg);
  gtk_label_set_label (GTK_LABEL (priv->translated), text);
  pos = g_strdup_printf ("%d", gtr_msg_get_po_position (msg));
  gtk_label_set_label (GTK_LABEL (priv->id), pos);

  // Update css classes for all columns
  update_cell_style (priv->original, msg);
  update_cell_style (priv->translated, msg);
  update_cell_style (priv->id, msg);
}

static void
gtr_message_table_row_init (GtrMessageTableRow *row)
{
  GtrMessageTableRowPrivate *priv;
  priv = gtr_message_table_row_get_instance_private (row);
  priv->ui_settings = g_settings_new ("org.gnome.gtranslator.preferences.ui");

  gtk_widget_init_template (GTK_WIDGET (row));

  g_settings_bind (priv->ui_settings, GTR_SETTINGS_SHOW_ID_COLUMN,
                   priv->id, "visible",
                   G_SETTINGS_BIND_DEFAULT);
}

static void
gtr_message_table_row_finalize (GObject *object)
{
  GtrMessageTableRow *row = GTR_MESSAGE_TABLE_ROW (object);
  GtrMessageTableRowPrivate *priv;

  priv = gtr_message_table_row_get_instance_private (row);

  g_clear_object (&priv->ui_settings);
  if (priv->msg)
    {
      g_signal_handlers_disconnect_by_func (priv->msg, update_msg, row);
      g_object_unref (priv->msg);
      priv->msg = NULL;
    }

  G_OBJECT_CLASS (gtr_message_table_row_parent_class)->finalize (object);
}

static void
gtr_message_table_row_set_property (GObject      *object,
                                    guint         prop_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
  GtrMessageTableRow *row = GTR_MESSAGE_TABLE_ROW (object);

  switch (prop_id)
    {
    case PROP_MSG:
      gtr_message_table_row_set_msg (row, GTR_MSG (g_value_get_object (value)));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtr_message_table_row_get_property (GObject    *object,
                                    guint       prop_id,
                                    GValue     *value,
                                    GParamSpec *pspec)
{
  GtrMessageTableRow *row = GTR_MESSAGE_TABLE_ROW (object);
  GtrMessageTableRowPrivate *priv;

  priv = gtr_message_table_row_get_instance_private (row);

  switch (prop_id)
    {
    case PROP_MSG:
      g_value_set_object (value, priv->msg);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
gtr_message_table_row_class_init (GtrMessageTableRowClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gtr_message_table_row_finalize;
  object_class->set_property = gtr_message_table_row_set_property;
  object_class->get_property = gtr_message_table_row_get_property;

  g_object_class_install_property (object_class,
                                   PROP_MSG,
                                   g_param_spec_object ("msg",
                                                        "MSG",
                                                        "The row msg",
                                                        GTR_TYPE_MSG,
                                                        G_PARAM_READWRITE));

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gnome/translator/gtr-message-table-row.ui");

  gtk_widget_class_bind_template_child_private (widget_class, GtrMessageTableRow, id);
  gtk_widget_class_bind_template_child_private (widget_class, GtrMessageTableRow, original);
  gtk_widget_class_bind_template_child_private (widget_class, GtrMessageTableRow, translated);
}

/**
 * gtr_message_table_row_new:
 *
 * Creates a new #GtrMessageTableRow object.
 *
 * Returns: the newly created #GtrMessageTableRow
 */
GtkWidget *
gtr_message_table_row_new (void)
{
  return GTK_WIDGET (g_object_new (GTR_TYPE_MESSAGE_TABLE_ROW, NULL));
}

void
gtr_message_table_row_set_msg (GtrMessageTableRow *row, GtrMsg *msg)
{
  GtrMessageTableRowPrivate *priv = NULL;

  priv = gtr_message_table_row_get_instance_private (row);

  if (priv->msg)
    {
      g_signal_handlers_disconnect_by_func (priv->msg, update_msg, row);
      g_object_unref (priv->msg);
      priv->msg = NULL;
    }

  if (!msg)
    return;

  priv->msg = msg;
  g_object_ref (priv->msg);
  g_signal_connect (priv->msg, "notify", G_CALLBACK (update_msg), row);

  update_msg (priv->msg, NULL, row);
}
