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

#include <config.h>
#include "nautilus-gtranslator-view.h"

#include <parse.h>
#include <vfs-handle.h>
#include <messages.h>
#include <open-differently.h>
#include <stylistics.h>

#include <gtk/gtk.h>

#include <libnautilus/nautilus-bonobo-ui.h>
#include <libgnomevfs/gnome-vfs.h>
#include <gnome.h>
#include <stdlib.h>
#include <bonobo/bonobo-control.h>
#include <libgnome/gnome-i18n.h>
#include <libgnomeui/gnome-stock.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtklabel.h>
#include <gtk/gtksignal.h>

struct NautilusGtranslatorViewDetails {
	char		*location;
	GtkWidget 	*table;
};

static void nautilus_gtranslator_view_initialize_class (NautilusGtranslatorViewClass *klass);
static void nautilus_gtranslator_view_initialize       (NautilusGtranslatorView      *view);
static void nautilus_gtranslator_view_destroy          (GtkObject                      *object);
static void sample_load_location_callback                 (NautilusView                   *nautilus_view,
							   const char                     *location,
							   gpointer                        user_data);
static void sample_merge_bonobo_items_callback            (BonoboControl                  *control,
							   gboolean                        state,
							   gpointer                        user_data);


void open_po_file(GtkWidget *widget, gpointer filename);
void mail_po_file(GtkWidget *widget, gpointer filename);

static gpointer parent_class;

void open_po_file(GtkWidget *widget, gpointer filename)
{
	gchar *cmd;
	
	g_assert(filename);

	cmd=g_strdup_printf("gtranslator --disable-crash-dialog %s", 
		(gchar *) filename);

	system(cmd);

	g_free(cmd);
}

void mail_po_file(GtkWidget *widget, gpointer filename)
{
	GtkWidget *dialog=NULL;
	GtkWidget *entry=NULL;
	GtkWidget *label=NULL;
	gchar *tuz;
	gchar *mailing;

	dialog=gnome_dialog_new(
		"nautilus-gtranslator-view -- mail the po file",
		"Mail", "Cancel", NULL);

	entry=gnome_entry_new("MAIL_TO");

	label=gtk_label_new("Mail to this address:");

	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), label,
		FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), entry,
		FALSE, FALSE, 0);

	gtk_widget_show(label);
	gtk_widget_show(entry);
	
	switch(gnome_dialog_run(GNOME_DIALOG(dialog)))
	{
		case GNOME_OK:
			
			tuz=gtk_editable_get_chars(
				GTK_EDITABLE(gnome_entry_gtk_entry
					(GNOME_ENTRY(entry))), 0, -1);

			mailing=g_strdup_printf("echo \"%s\" > /tmp/.ZT && cat /tmp/.ZT|mutt '%s' -s '%s' -a \"%s\"",
				"Po file for you -- take it or blame it!",	
				tuz, "Po file for you!", 
				gnome_vfs_get_local_path_from_uri(
					(gchar *) filename));

			system(mailing);

			g_free(mailing);
			break;
		
		default:
			
			break;
	}
	
	gtk_widget_destroy(dialog);

	g_free(tuz);
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
	
	view->details->table=gtk_table_new(6, 2, FALSE);

	gtk_widget_show(view->details->table);
	
	nautilus_view_construct(NAUTILUS_VIEW(view), 
		view->details->table);

	gtk_signal_connect (GTK_OBJECT (view), 
			    "load_location",
			    sample_load_location_callback, 
			    NULL);

        gtk_signal_connect (GTK_OBJECT (nautilus_view_get_bonobo_control (NAUTILUS_VIEW (view))),
                            "activate",
                            sample_merge_bonobo_items_callback,
                            view);
	
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
load_location (NautilusGtranslatorView *view,
	       const char *location)
{
	GtkWidget *name, *version, *translator, *language, *podate, *potdate,
		*encoding;
	
	g_assert(NAUTILUS_IS_GTRANSLATOR_VIEW(view));

	g_assert(location!=NULL);

	g_free(view->details->location);
	view->details->location=g_strdup(location);

	parse(gnome_vfs_get_local_path_from_uri(view->details->location));
	
	#define add_part(x, y, z) \
	gtk_table_attach_defaults(GTK_TABLE(view->details->table), \
		x, z, z+1, y, y+1)

	name=gtk_label_new(g_strdup_printf(_("Project: %s"),
		po->header->prj_name));

	version=gtk_label_new(g_strdup_printf(_("Version: %s"),
		po->header->prj_version));

	translator=gnome_href_new(g_strdup_printf(_("mailto:%s"), po->header->tr_email),
		g_strdup_printf(_("Last translator: %s <%s>"), po->header->translator, 
		po->header->tr_email));

	language=gnome_href_new(g_strdup_printf(_("mailto:%s"), po->header->lg_email),
		g_strdup_printf(_("Language team: %s <%s>"), po->header->language,
		po->header->lg_email));

	potdate=gtk_label_new(g_strdup_printf(_("Last %s.pot update: %s"),
		po->header->prj_name, po->header->pot_date));
	
	podate=gtk_label_new(g_strdup_printf(_("Last po file update: %s"),
		po->header->po_date));
	
	encoding=gtk_label_new(g_strdup_printf(_("Encoding: %s (%s)"),
		po->header->charset, po->header->encoding));

	if(po->header->comment)
	{
		GtkWidget *comment;
		
		comment=gtk_label_new(g_strdup_printf(_("Comments: %s"), 
			prepare_comment_for_view(po->header->comment)));
		
		add_part(comment, 0, 0);
		add_part(encoding, 0, 1);
		add_part(name, 1, 0);
		add_part(version, 1, 1);
		add_part(translator, 2, 0);
		add_part(language, 2, 1);
		add_part(potdate, 3, 0);
		add_part(podate, 3, 1);
	}
	else
	{
		add_part(encoding, 0, 0);
		add_part(name, 1, 0);
		add_part(version, 1, 1);
		add_part(translator, 2, 0);
		add_part(language, 2, 1);
		add_part(potdate, 3, 0);
		add_part(podate, 3, 1);
	}

	gtk_widget_show_all(GTK_WIDGET(GTK_TABLE(view->details->table)));
}

static void
sample_load_location_callback (NautilusView *nautilus_view, 
			       const char *location,
			       gpointer user_data)
{
	NautilusGtranslatorView *view;
	
	g_assert (NAUTILUS_IS_VIEW (nautilus_view));
	g_assert (location != NULL);
	
	view = NAUTILUS_GTRANSLATOR_VIEW (nautilus_view);
	
	nautilus_view_report_load_underway (nautilus_view);

	nautilus_view_report_status(nautilus_view, _("Loading po file..."));

	load_location (view, location);

	nautilus_view_report_load_complete(NAUTILUS_VIEW(view));
	
}

static void
bonobo_sample_callback (BonoboUIComponent *ui, 
			gpointer           user_data, 
			const char        *verb)
{
 	NautilusGtranslatorView *view;
	
	g_assert (BONOBO_IS_UI_COMPONENT (ui));
        g_assert (verb != NULL);

	view = NAUTILUS_GTRANSLATOR_VIEW (user_data);

	if(!strcmp(verb, _("Open Po File")))
	{
		open_po_file(NULL, view->details->location);
	}
	if(!strcmp(verb, _("Edit Header")))
	{
		edit_header(NULL, NULL);
	}
	if(!strcmp(verb, "Mail File"))
	{
		mail_po_file(NULL, view->details->location);
	}
}

/* CHANGE: Do your own menu/toolbar merging here. */
static void
sample_merge_bonobo_items_callback (BonoboControl *control, 
				    gboolean       state, 
				    gpointer       user_data)
{
 	NautilusGtranslatorView *view;
	BonoboUIComponent *ui_component;
	BonoboUIVerb verbs [] = {
		BONOBO_UI_VERB(_("Open Po File"), bonobo_sample_callback),
		BONOBO_UI_VERB(_("Edit Header"), bonobo_sample_callback),
		BONOBO_UI_VERB("Mail File", bonobo_sample_callback),
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

        /* Note that we do nothing if state is FALSE. Nautilus content
         * views are activated when installed, but never explicitly
         * deactivated. When the view changes to another, the content
         * view object is destroyed, which ends up calling
         * bonobo_ui_handler_unset_container, which removes its merged
         * menu & toolbar items.
	 */
}
