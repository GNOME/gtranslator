/*
 * (C) 2001 	Fatih Demir <kabalak@gmx.net>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "nautilus-gtranslator-view.h"

#include "../src/messages.h"
#include "../src/open-differently.h"
#include "../src/nautilus-string.h"
#include "../src/parse.h"
#include "../src/stylistics.h"
#include "../src/vfs-handle.h"

#include <stdlib.h>

#include <gtk/gtkwidget.h>

#include <libgnomevfs/gnome-vfs.h>
#include <libgnomeui/gnome-dialog.h>
#include <libgnomeui/gnome-entry.h>
#include <libgnomeui/gnome-uidefs.h>
#include <libgnomeui/gnome-href.h>

#include <libnautilus/nautilus-bonobo-ui.h>

#include <gtkhtml/gtkhtml.h>

struct NautilusGtranslatorViewDetails {
	char		*location;
	GtkWidget 	*html_wonderland;
};

static gpointer parent_class;

static void nautilus_gtranslator_view_initialize_class (NautilusGtranslatorViewClass *klass);
static void nautilus_gtranslator_view_initialize       (NautilusGtranslatorView      *view);
static void nautilus_gtranslator_view_destroy          (GtkObject                      *object);
static void nautilus_gtranslator_view_load_location                 (NautilusView                   *nautilus_view,
							   const char                     *location,
							   gpointer                        user_data);
static void nautilus_gtranslator_view_merge_bonobo_ui            (BonoboControl                  *control,
							   gboolean                        state,
							   gpointer                        user_data);

/*
 * Own callback methods.
 */
static void 	nautilus_gtranslator_view_open_po_file(GtkWidget *widget, gpointer filename);
static void	nautilus_gtranslator_view_url_clicked(GtkHTML *html, const gchar *url);

gchar 		*nautilus_gtranslator_view_setup_html_header(const gchar *filename);
gchar		*nautilus_gtranslator_view_setup_html_table(GtrPo *Po);
gchar 		*nautilus_gtranslator_view_setup_html_footer(void);

/*
 * Return a finished header/footer part of a HTML page.
 */
gchar *nautilus_gtranslator_view_setup_html_header(const gchar *filename)
{
	gchar *header;
	
	g_return_val_if_fail(filename!=NULL, NULL);

	header=g_strconcat("<html><head><title>", filename, "</title></head><body>", NULL);
	return header;
}

gchar *nautilus_gtranslator_view_setup_html_footer()
{
	return "</body></html>";
}

/*
 * Creates the html table with all the informations from the po file.
 */
gchar *nautilus_gtranslator_view_setup_html_table(GtrPo *Po)
{
	gchar *table;
	
	g_return_val_if_fail(Po!=NULL, NULL);

	if(!Po->header)
	{
		table=g_strdup_printf("<table border=\"1\" align=\"center\">\
			<tr><td>\
			<font color=\"red\">Po file `%s' doesn't contain a valuable & right header .-(</font>\
			</td></tr></table>", Po->filename);
	}
	else
	{
		table=g_strdup_printf("<table border=\"1\" align=\"center\">\
			<tr><td>\
			%s (<a href=\"run:%s\">Run gtranslator</a>):\
			</td></tr></table>", Po->filename, Po->filename);
	}

	return table;
}

/*
 * Opens the po file simply via a system() call.
 */
static void nautilus_gtranslator_view_open_po_file(GtkWidget *widget, gpointer filename)
{
	gchar *cmd;

	g_assert(filename);

	cmd=g_strdup_printf("gtranslator --disable-crash-dialog %s",
		(gchar *) filename);

	system(cmd);
	g_free(cmd);
}

/*
 * The clicked callback formthe html widget.
 */
static void nautilus_gtranslator_view_url_clicked(GtkHTML *html, const gchar *url)
{
	g_return_if_fail(html!=NULL);
	g_return_if_fail(url!=NULL);

	if(nautilus_istr_has_prefix(url, "run:"))
	{
		gchar *cmd;
		gchar *filename=nautilus_str_get_after_prefix(url, "run:");
		g_return_if_fail(filename!=NULL);

		cmd=g_strdup_printf("gtranslator --disable-crash-dialog %s",
			filename);

		system(cmd);

		g_free(cmd);
		g_free(filename);
	}
}

GtkType
nautilus_gtranslator_view_get_type (void)
{
	GtkType parent_type;
	static GtkType type;

	if (type == 0) {
		static GtkTypeInfo info = {
		        "NautilusGtranslatorView",
			sizeof (NautilusGtranslatorView),
			sizeof (NautilusGtranslatorViewClass),
			(GtkClassInitFunc)nautilus_gtranslator_view_initialize_class,
			(GtkObjectInitFunc)nautilus_gtranslator_view_initialize,
			NULL,
			NULL,
			NULL
		};

		parent_type = (NAUTILUS_TYPE_VIEW);
		type = gtk_type_unique (parent_type, &info);
		parent_class = gtk_type_class (parent_type);
	}

	return type;
}


static void
nautilus_gtranslator_view_initialize_class (NautilusGtranslatorViewClass *klass)
{
	GtkObjectClass *object_class;

	g_assert (NAUTILUS_IS_GTRANSLATOR_VIEW_CLASS (klass));

	object_class = GTK_OBJECT_CLASS (klass);

	object_class->destroy = nautilus_gtranslator_view_destroy;
}

static void
nautilus_gtranslator_view_initialize (NautilusGtranslatorView *view)
{
	g_assert (NAUTILUS_IS_GTRANSLATOR_VIEW (view));

	view->details = g_new0 (NautilusGtranslatorViewDetails, 1);

	view->details->html_wonderland=gtk_html_new();
	gtk_html_load_empty(GTK_HTML(view->details->html_wonderland));
	gtk_html_set_editable(GTK_HTML(view->details->html_wonderland), FALSE);

	gtk_widget_show(view->details->html_wonderland);

	nautilus_view_construct(NAUTILUS_VIEW(view),
		view->details->html_wonderland);

	gtk_signal_connect(GTK_OBJECT(view),
		"load_location",
		nautilus_gtranslator_view_load_location,
		NULL);

        gtk_signal_connect(GTK_OBJECT(nautilus_view_get_bonobo_control (NAUTILUS_VIEW (view))),
		"activate",
		nautilus_gtranslator_view_merge_bonobo_ui,
		view);

	gtk_signal_connect(GTK_OBJECT(view->details->html_wonderland),
		"link_clicked",
		nautilus_gtranslator_view_url_clicked,
		NULL);

}

static void
nautilus_gtranslator_view_destroy (GtkObject *object)
{
	NautilusGtranslatorView *view;

	view = NAUTILUS_GTRANSLATOR_VIEW (object);

	g_free(view->details);

	(* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}



static void
nautilus_gtranslator_view_core_load_location (NautilusGtranslatorView *view,
	       const char *location)
{
	GString 	*htmlpage=g_string_new("");

	g_assert(NAUTILUS_IS_GTRANSLATOR_VIEW(view));

	g_assert(location!=NULL);

	g_free(view->details->location);
	view->details->location=g_strdup(location);

	/*
	 * Parse the current location as a po file.
	 */ 
	gtranslator_parse(gnome_vfs_get_local_path_from_uri(
		view->details->location));

	g_return_if_fail(po->filename!=NULL);

	htmlpage=g_string_append(htmlpage, nautilus_gtranslator_view_setup_html_header(po->filename));
	htmlpage=g_string_append(htmlpage, nautilus_gtranslator_view_setup_html_table(po)); 
	htmlpage=g_string_append(htmlpage, nautilus_gtranslator_view_setup_html_footer());

	gtk_html_load_from_string(GTK_HTML(view->details->html_wonderland), 
		htmlpage->str, htmlpage->len);

	g_string_free(htmlpage, FALSE);

	gtk_widget_show(GTK_WIDGET(GTK_TABLE(view->details->html_wonderland)));
}

static void
nautilus_gtranslator_view_load_location (NautilusView *nautilus_view,
			       const char *location,
			       gpointer user_data)
{
	NautilusGtranslatorView *view;

	g_assert (NAUTILUS_IS_VIEW (nautilus_view));
	g_assert (location != NULL);

	view = NAUTILUS_GTRANSLATOR_VIEW (nautilus_view);

	nautilus_view_report_load_underway (nautilus_view);

	nautilus_view_report_status(nautilus_view, _("Loading po file..."));

	nautilus_gtranslator_view_core_load_location (view, location);

	nautilus_view_report_load_complete(NAUTILUS_VIEW(view));

}

static void
nautilus_gtranslator_view_verb_callback (BonoboUIComponent *ui,
			gpointer           user_data,
			const char        *verb)
{
 	NautilusGtranslatorView *view;

	g_assert (BONOBO_IS_UI_COMPONENT (ui));
        g_assert (verb != NULL);

	view = NAUTILUS_GTRANSLATOR_VIEW (user_data);

	if(!strcmp(verb, _("Open Po File")))
	{
		nautilus_gtranslator_view_open_po_file(NULL, view->details->location);
	}
}

static void
nautilus_gtranslator_view_merge_bonobo_ui (BonoboControl *control,
				    gboolean       state,
				    gpointer       user_data)
{
 	NautilusGtranslatorView *view;
	BonoboUIComponent *ui_component;
	BonoboUIVerb verbs [] = {
		BONOBO_UI_VERB(_("Open Po File"), nautilus_gtranslator_view_verb_callback),
		BONOBO_UI_VERB_END
	};

	g_assert (BONOBO_IS_CONTROL (control));

	view = NAUTILUS_GTRANSLATOR_VIEW (user_data);

	if(state)
	{
		ui_component=nautilus_view_set_up_ui(NAUTILUS_VIEW(view),
			DATADIR,
			"nautilus-gtranslator-view-ui.xml",
			"nautilus-gtranslator-view");

		bonobo_ui_component_add_verb_list_with_data(
			ui_component, verbs, view);
	}
}
