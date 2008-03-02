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

#include "notebook.h"
#include "tab.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#define GTR_NOTEBOOK_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_NOTEBOOK,     \
					 GtranslatorNotebookPrivate))

G_DEFINE_TYPE(GtranslatorNotebook, gtranslator_notebook, GTK_TYPE_NOTEBOOK)

struct _GtranslatorNotebookPrivate
{
	GList *pages;
	
};

/* Signals */
enum
{
	TAB_CLOSE_REQUEST,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void
tab_label_style_set_cb (GtkWidget *hbox,
			GtkStyle *previous_style,
			gpointer user_data)
{
	GtkWidget *button;
	gint h, w;

	gtk_icon_size_lookup_for_settings (gtk_widget_get_settings (hbox),
					   GTK_ICON_SIZE_MENU, &w, &h);

	button = g_object_get_data (G_OBJECT (hbox), "close-button");
	gtk_widget_set_size_request (button, w + 2, h + 2);
}

static void
sync_name (GtranslatorTab *tab,
	   GParamSpec *pspec,
	   GtkWidget *hbox)
{
	gchar *str;
	GtkWidget *label;
	GtkWidget *ebox;
	gchar *tooltip;
	GtranslatorPo *po;
	
	label = GTK_WIDGET (g_object_get_data (G_OBJECT (hbox), "label"));
	ebox = GTK_WIDGET (g_object_get_data (G_OBJECT (hbox), "label-ebox"));
	
	po = gtranslator_tab_get_po (tab);
	
	str = gtranslator_tab_get_name (tab);
	g_return_if_fail (str != NULL);
	
	gtk_label_set_text (GTK_LABEL (label), str);
	
	tooltip = g_strdup_printf (_("<b>Path:</b> %s"),
				   gtranslator_po_get_filename (po));
	gtk_widget_set_tooltip_markup (ebox, tooltip);
	
	g_free (tooltip);
	g_free (str);
}

static void
close_button_clicked_cb (GtkWidget *widget, 
			 GtkWidget *tab)
{
	GtranslatorNotebook *notebook;

	notebook = GTR_NOTEBOOK (gtk_widget_get_parent (tab));
	g_signal_emit (notebook, signals[TAB_CLOSE_REQUEST], 0, tab);
}

static GtkWidget *
build_tab_label (GtranslatorNotebook *nb, 
		 GtranslatorTab      *tab)
{
	GtkWidget *hbox, *label_hbox, *label_ebox;
	GtkWidget *label, *dummy_label;
	GtkWidget *close_button;
	GtkRcStyle *rcstyle;
	GtkWidget *image;
	GtkWidget *spinner;
	GtkWidget *icon;

	hbox = gtk_hbox_new (FALSE, 4);

	label_ebox = gtk_event_box_new ();
	gtk_event_box_set_visible_window (GTK_EVENT_BOX (label_ebox), FALSE);
	gtk_box_pack_start (GTK_BOX (hbox), label_ebox, TRUE, TRUE, 0);

	label_hbox = gtk_hbox_new (FALSE, 4);
	gtk_container_add (GTK_CONTAINER (label_ebox), label_hbox);

	/* setup close button */
	close_button = gtk_button_new ();
	gtk_button_set_relief (GTK_BUTTON (close_button),
			       GTK_RELIEF_NONE);
	/* don't allow focus on the close button */
	gtk_button_set_focus_on_click (GTK_BUTTON (close_button), FALSE);

	/* make it as small as possible */
	rcstyle = gtk_rc_style_new ();
	rcstyle->xthickness = rcstyle->ythickness = 0;
	gtk_widget_modify_style (close_button, rcstyle);
	gtk_rc_style_unref (rcstyle),

	image = gtk_image_new_from_stock (GTK_STOCK_CLOSE,
					  GTK_ICON_SIZE_MENU);
	gtk_container_add (GTK_CONTAINER (close_button), image);
	gtk_box_pack_start (GTK_BOX (hbox), close_button, FALSE, FALSE, 0);

	gtk_widget_set_tooltip_text(close_button, _("Close document"));

	g_signal_connect (close_button,
			  "clicked",
			  G_CALLBACK (close_button_clicked_cb),
			  tab);

	/* setup site icon, empty by default */
	icon = gtk_image_new ();
	gtk_box_pack_start (GTK_BOX (label_hbox), icon, FALSE, FALSE, 0);
	
	/* setup label */
	label = gtk_label_new ("");
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_misc_set_padding (GTK_MISC (label), 0, 0);
	gtk_box_pack_start (GTK_BOX (label_hbox), label, FALSE, FALSE, 0);

	dummy_label = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (label_hbox), dummy_label, TRUE, TRUE, 0);
	
	/* Set minimal size */
	g_signal_connect(hbox, "style-set",
			G_CALLBACK (tab_label_style_set_cb), NULL);
	
	gtk_widget_show (hbox);
	gtk_widget_show (label_ebox);
	gtk_widget_show (label_hbox);
	gtk_widget_show (label);
	gtk_widget_show (dummy_label);	
	gtk_widget_show (image);
	gtk_widget_show (close_button);
	gtk_widget_show (icon);
	
	g_object_set_data (G_OBJECT (hbox), "label", label);
	g_object_set_data (G_OBJECT (hbox), "label-ebox", label_ebox);
	g_object_set_data (G_OBJECT (hbox), "close-button", close_button);
	g_object_set_data (G_OBJECT (tab), "close-button", close_button);

	return hbox;
}

static void
gtranslator_notebook_init (GtranslatorNotebook *notebook)
{
	notebook->priv = GTR_NOTEBOOK_GET_PRIVATE (notebook);
	GtranslatorNotebookPrivate *priv = notebook->priv;
	
	priv->pages = NULL;
}

static void
gtranslator_notebook_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_notebook_parent_class)->finalize (object);
}

static void
gtranslator_notebook_class_init (GtranslatorNotebookClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorNotebookPrivate));

	object_class->finalize = gtranslator_notebook_finalize;

	signals[TAB_CLOSE_REQUEST] =
		g_signal_new ("tab-close-request",
			      G_OBJECT_CLASS_TYPE (object_class),
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (GtranslatorNotebookClass, tab_close_request),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__OBJECT,
			      G_TYPE_NONE,
			      1,
			      GTR_TYPE_TAB);
}

/***************************** Public funcs ***********************************/

GtkWidget *
gtranslator_notebook_new()
{
	return GTK_WIDGET(g_object_new(GTR_TYPE_NOTEBOOK, NULL));
}

void
gtranslator_notebook_add_page(GtranslatorNotebook *notebook,
			      GtranslatorTab *tab)
{
	GtranslatorNotebookPrivate *priv = notebook->priv;
	GtkWidget *label;

	g_return_if_fail(GTR_IS_NOTEBOOK(notebook));
	g_return_if_fail(GTR_IS_TAB(tab));
	
	label = build_tab_label(notebook, tab);

	sync_name (tab, NULL, label);
		         
	g_signal_connect_object (tab, 
				 "notify::state",
			         G_CALLBACK (sync_name), 
			         label, 
			         0);

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
				 GTK_WIDGET(tab), label);
	priv->pages = g_list_append(priv->pages, tab);
}

GtranslatorTab *
gtranslator_notebook_get_page(GtranslatorNotebook *notebook)
{
	gint num;
	
	num = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
	
	return GTR_TAB(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), num));
}


