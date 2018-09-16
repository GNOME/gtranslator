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

#include "gtr-notebook.h"
#include "gtr-tab.h"
#include "gtr-tab-label.h"
#include "gtr-debug.h"
#include "gtr-progress.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

typedef struct
{
  GtkWidget *titlebar;
  GtkWidget *progress_button;
  GtkWidget *progress_percentage;
  GtkWidget *progress_trans;
  GtkWidget *progress_fuzzy;
  GtkWidget *progress_untrans;
  GtkWidget *save;
  GtrProgress *progress;
} GtrNotebookPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GtrNotebook, gtr_notebook, GTK_TYPE_NOTEBOOK)

/* Signals */
enum
{
  TAB_CLOSE_REQUEST,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void
close_button_clicked_cb (GtrTabLabel *tab_label, GtrNotebook *notebook)
{
  GtrTab *tab;

  tab = gtr_tab_label_get_tab (tab_label);
  g_signal_emit (notebook, signals[TAB_CLOSE_REQUEST], 0, tab);
}

static GtkWidget *
create_tab_label (GtrNotebook *nb,
                  GtrTab      *tab)
{
  GtkWidget *tab_label;

  tab_label = gtr_tab_label_new (tab);

  g_signal_connect (tab_label,
                    "close-clicked",
                    G_CALLBACK (close_button_clicked_cb),
                    nb);

  g_object_set_data (G_OBJECT (tab), "tab-label", tab_label);

  return tab_label;
}

static void
remove_tab_label (GtrNotebook *nb,
                  GtrTab      *tab)
{
  GtkWidget *tab_label;

  tab_label = g_object_get_data (G_OBJECT (tab), "tab-label");
  g_return_if_fail (tab_label != NULL);

  g_signal_handlers_disconnect_by_func (tab_label,
                                        G_CALLBACK (close_button_clicked_cb),
                                        nb);

  g_object_set_data (G_OBJECT (tab), "tab-label", NULL);
}

static void
update_tabs_visibility (GtrNotebook *nb)
{
  gboolean show_tabs;
  guint num;

  num = gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb));

  show_tabs = (num > 1);

  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (nb), show_tabs);
}

static void
gtr_notebook_init (GtrNotebook * notebook)
{
  GtrNotebookPrivate *priv = gtr_notebook_get_instance_private (notebook);
  gtk_widget_init_template (GTK_WIDGET (notebook));

  priv->progress = gtr_progress_new ();
  gtk_widget_show (GTK_WIDGET (priv->progress));
  gtk_container_add (GTK_CONTAINER (priv->progress_button), GTK_WIDGET (priv->progress));
}

static void
gtr_notebook_finalize (GObject * object)
{
  G_OBJECT_CLASS (gtr_notebook_parent_class)->finalize (object);
}

static void
gtr_notebook_class_init (GtrNotebookClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gtr_notebook_finalize;

  signals[TAB_CLOSE_REQUEST] =
    g_signal_new ("tab-close-request",
                  G_OBJECT_CLASS_TYPE (object_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GtrNotebookClass,
                                   tab_close_request), NULL, NULL,
                  g_cclosure_marshal_VOID__OBJECT, G_TYPE_NONE, 1,
                  GTR_TYPE_TAB);

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gnome/translator/gtr-notebook.ui");

  gtk_widget_class_bind_template_child_private (widget_class, GtrNotebook, titlebar);
  gtk_widget_class_bind_template_child_private (widget_class, GtrNotebook, progress_button);
  gtk_widget_class_bind_template_child_private (widget_class, GtrNotebook, progress_trans);
  gtk_widget_class_bind_template_child_private (widget_class, GtrNotebook, progress_fuzzy);
  gtk_widget_class_bind_template_child_private (widget_class, GtrNotebook, progress_untrans);
  gtk_widget_class_bind_template_child_private (widget_class, GtrNotebook, progress_percentage);
  gtk_widget_class_bind_template_child_private (widget_class, GtrNotebook, save);
}

/***************************** Public funcs ***********************************/

/**
 * gtr_notebook_new:
 * 
 * Creates a new #GtrNotebook.
 * 
 * Returns: a new #GtrNotebook object
 */
GtkWidget *
gtr_notebook_new ()
{
  return GTK_WIDGET (g_object_new (GTR_TYPE_NOTEBOOK, NULL));
}

/**
 * gtr_notebook_add_page:
 * @notebook: a #GtrNotebook
 * @tab: a #GtrTab
 * 
 * Adds a new #GtrTab to @notebook.
 */
void
gtr_notebook_add_page (GtrNotebook * notebook, GtrTab * tab)
{
  GtkWidget *label;

  g_return_if_fail (GTR_IS_NOTEBOOK (notebook));
  g_return_if_fail (GTR_IS_TAB (tab));

  label = create_tab_label (notebook, tab);

  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), GTK_WIDGET (tab), label);

  update_tabs_visibility (notebook);
}

static void
remove_tab (GtrTab *tab,
            GtrNotebook *notebook)
{
  remove_tab_label (notebook, tab);

  /* Destroy the tab to break circular refs */
  gtk_widget_destroy (GTK_WIDGET (tab));

  update_tabs_visibility (notebook);
}

/**
 * gtr_notebook_remove_page:
 * @notebook: a #GtrNotebook
 * @page_num: the index of a notebook page, starting from 0.
 *
 * Removes a page from the notebook given its index in the notebook.
 */
void
gtr_notebook_remove_page (GtrNotebook * notebook, gint page_num)
{
  GtrTab *tab;

  g_return_if_fail (GTR_IS_NOTEBOOK (notebook));

  tab = GTR_TAB (gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), page_num));

  remove_tab (tab, notebook);
}

/**
 * gtr_notebook_remove_all_pages:
 * @notebook: a #GtrNotebook
 *
 * Removes all tabs from from @notebook
 */
void
gtr_notebook_remove_all_pages (GtrNotebook *notebook)
{
  g_return_if_fail (GTR_IS_NOTEBOOK (notebook));

  gtk_container_foreach (GTK_CONTAINER (notebook),
                         (GtkCallback)remove_tab,
                         notebook);
}

/**
 * gtr_notebook_get_page:
 * @notebook: a #GtrNotebook
 * 
 * Gets the selected page in the #GtrNotebook.
 * 
 * Returns: (transfer none): the selected page in the @notebook
 */
GtrTab *
gtr_notebook_get_page (GtrNotebook * notebook)
{
  gint num;

  num = gtk_notebook_get_current_page (GTK_NOTEBOOK (notebook));

  return GTR_TAB (gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), num));
}

GtkWidget *
gtr_notebook_get_header (GtrNotebook *notebook)
{
  GtrNotebookPrivate *priv = gtr_notebook_get_instance_private (notebook);
  return priv->titlebar;
}

void
gtr_notebook_set_progress (GtrNotebook *notebook,
                           gint         trans,
                           gint         untrans,
                           gint         fuzzy)
{
  GtrNotebookPrivate *priv = gtr_notebook_get_instance_private (notebook);
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
gtr_notebook_enable_save (GtrNotebook *notebook,
                          gboolean enable)
{
  GtrNotebookPrivate *priv = gtr_notebook_get_instance_private (notebook);
  gtk_widget_set_sensitive (priv->save, enable);
}

