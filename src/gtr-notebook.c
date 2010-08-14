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

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

G_DEFINE_TYPE (GtrNotebook, gtr_notebook, GTK_TYPE_NOTEBOOK)

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

  tab_label = gtr_tab_label_new (tab);

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

  object_class->finalize = gtr_notebook_finalize;

  signals[TAB_CLOSE_REQUEST] =
    g_signal_new ("tab-close-request",
                  G_OBJECT_CLASS_TYPE (object_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GtrNotebookClass,
                                   tab_close_request), NULL, NULL,
                  g_cclosure_marshal_VOID__OBJECT, G_TYPE_NONE, 1,
                  GTR_TYPE_TAB);
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
  GtrPo *po;
  GtkWidget *label;

  g_return_if_fail (GTR_IS_NOTEBOOK (notebook));
  g_return_if_fail (GTR_IS_TAB (tab));

  po = gtr_tab_get_po (tab);

  label = create_tab_label (notebook, tab);

  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), GTK_WIDGET (tab), label);

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
  GtkWidget *tab;

  g_return_if_fail (GTR_IS_NOTEBOOK (notebook));

  tab = gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), page_num);

  if (page_num != -1)
    {
      remove_tab_label (notebook, GTR_TAB (tab));
      gtk_notebook_remove_page (GTK_NOTEBOOK (notebook), page_num);
    }

  update_tabs_visibility (notebook);
}

/**
 * gtr_notebook_get_page:
 * @notebook: a #GtrNotebook
 * 
 * Gets the selected page in the #GtrNotebook.
 * 
 * Returns: the selected page in the @notebook
 */
GtrTab *
gtr_notebook_get_page (GtrNotebook * notebook)
{
  gint num;

  num = gtk_notebook_get_current_page (GTK_NOTEBOOK (notebook));

  return GTR_TAB (gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), num));
}
