/*
 * Copyright (C) 2019  Daniel Garcia Moreno <danigm@gnome.org>
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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-filter-selection.h"

typedef struct
{
  GtkWidget *entry;
  GtkWidget *option_list;
  GtkWidget *popup;
  GSList *options;
  // TODO: manage this as a property
  char *option;

} GtrFilterSelectionPrivate;

enum
{
  CHANGED,
  LAST_SIGNAL
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrFilterSelection, gtr_filter_selection, GTK_TYPE_MENU_BUTTON)

static guint signals[LAST_SIGNAL] = { 0 };

static void
change_option (GtkListBox         *box,
               GtkListBoxRow      *row,
               GtrFilterSelection *self)
{
  GtrFilterSelectionPrivate *priv = gtr_filter_selection_get_instance_private (self);
  GtkWidget *label = gtk_bin_get_child (GTK_BIN (row));
  gtr_filter_selection_set_option (self, gtk_label_get_text (GTK_LABEL (label)));

  gtk_popover_popdown (GTK_POPOVER (priv->popup));
}

static void
filter_option (GtkEditable        *entry,
               GtrFilterSelection *self)
{
  GtrFilterSelectionPrivate *priv = gtr_filter_selection_get_instance_private (self);
  const char *text = gtk_entry_get_text (GTK_ENTRY (entry));
  g_autofree char *uptext = g_ascii_strup (text, -1);
  const GSList *o;
  GList *children;

  children = gtk_container_get_children (GTK_CONTAINER (priv->option_list));
  while (children)
    {
      GtkWidget *w = GTK_WIDGET (children->data);
      gtk_container_remove (GTK_CONTAINER (priv->option_list), w);
      children = g_list_next (children);
    }

  for (o = priv->options; o != NULL; o = g_slist_next (o))
    {
      GtkWidget *child;
      const char *opt = (char *)o->data;
      g_autofree char *upopt = g_ascii_strup (opt, -1);

      if (g_strrstr (upopt, uptext) == NULL)
        continue;

      child = gtk_label_new (opt);
      gtk_label_set_xalign (GTK_LABEL (child), 0.0);
      gtk_container_add (GTK_CONTAINER (priv->option_list), child);
    }
  gtk_widget_show_all (priv->option_list);
}

static void
gtr_filter_selection_finalize (GObject *object)
{
  GtrFilterSelectionPrivate *priv = gtr_filter_selection_get_instance_private (GTR_FILTER_SELECTION (object));
  g_clear_pointer (&priv->option, g_free);
  if (priv->options)
    g_slist_free_full (priv->options, g_free);
  G_OBJECT_CLASS (gtr_filter_selection_parent_class)->finalize (object);
}

static void
gtr_filter_selection_class_init (GtrFilterSelectionClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gtr_filter_selection_finalize;

  signals[CHANGED] =
    g_signal_newv ("changed",
                   G_OBJECT_CLASS_TYPE (object_class),
                   G_SIGNAL_RUN_LAST,
                   NULL, NULL, NULL, NULL,
                   G_TYPE_NONE, 0, NULL);

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gnome/translator/gtr-filter-selection.ui");

  gtk_widget_class_bind_template_child_private (widget_class, GtrFilterSelection, entry);
  gtk_widget_class_bind_template_child_private (widget_class, GtrFilterSelection, option_list);
  gtk_widget_class_bind_template_child_private (widget_class, GtrFilterSelection, popup);
}

static void
gtr_filter_selection_init (GtrFilterSelection *self)
{
  GtrFilterSelectionPrivate *priv = gtr_filter_selection_get_instance_private (self);
  priv->option = NULL;
  priv->options = NULL;
  gtk_widget_init_template (GTK_WIDGET (self));
  gtk_widget_show_all (priv->option_list);

  g_signal_connect (priv->option_list,
                    "row-activated",
                    G_CALLBACK (change_option),
                    self);

  g_signal_connect (priv->entry,
                    "changed",
                    G_CALLBACK (filter_option),
                    self);
}

GtrFilterSelection*
gtr_filter_selection_new () {
  GtrFilterSelection *self = g_object_new (GTR_TYPE_FILTER_SELECTION, NULL);
  return self;
}

const char *
gtr_filter_selection_get_option (GtrFilterSelection *self)
{
  GtrFilterSelectionPrivate *priv = gtr_filter_selection_get_instance_private (self);
  return priv->option;
}

void
gtr_filter_selection_set_option (GtrFilterSelection *self,
                                 const char         *option)
{
  GtrFilterSelectionPrivate *priv = gtr_filter_selection_get_instance_private (self);
  g_clear_pointer (&priv->option, g_free);
  priv->option = g_strdup (option);
  gtk_button_set_label (GTK_BUTTON (self), option);
  g_signal_emit (self, signals[CHANGED], 0, NULL);
}

void
gtr_filter_selection_set_options (GtrFilterSelection *self,
                                  GSList *options)
{
  GtrFilterSelectionPrivate *priv = gtr_filter_selection_get_instance_private (self);
  const GSList *o;
  GList *children;

  if (priv->options)
    g_slist_free_full (priv->options, g_free);
  priv->options = options;

  children = gtk_container_get_children (GTK_CONTAINER (priv->option_list));
  while (children)
    {
      GtkWidget *w = GTK_WIDGET (children->data);
      gtk_container_remove (GTK_CONTAINER (priv->option_list), w);
      children = g_list_next (children);
    }

  for (o = priv->options; o != NULL; o = g_slist_next (o))
    {
      const char *opt = (char *)o->data;
      GtkWidget *child = gtk_label_new (opt);
      gtk_label_set_xalign (GTK_LABEL (child), 0.0);
      gtk_container_add (GTK_CONTAINER (priv->option_list), child);
    }

  gtk_widget_show_all (priv->option_list);
}

void
gtr_filter_selection_set_text (GtrFilterSelection *selection,
                               const char *text)
{
  gtk_button_set_label (GTK_BUTTON (selection), text);
}

