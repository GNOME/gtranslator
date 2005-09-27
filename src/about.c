/*
 * (C) 2000-2003 	Fatih Demir <kabalak@kabalak.net>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Roy-Magne Mo <rmo@sunnmore.net>
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

#include "about.h"

#include <string.h>

#include <libgnome/gnome-i18n.h>
#include <libgnome/gnome-program.h>

#include <gtk/gtk.h>

/*
 * Creates and shows the about box for gtranslator.
 */ 
void gtranslator_about_dialog(GtkWidget * widget, gpointer useless)
{
	
	const gchar *authors[] = {
		"Ross Golder <ross@golder.org>",
		"Fatih Demir <kabalak@kabalak.net>",
		" -- ",
		"Gediminas Paulauskas <menesis@delfi.lt>",
		"Thomas Ziehmer <thomas@kabalak.net>",
		"Peeter Vois <peeter@kabalak.net>",
		NULL
	};	
	static const char *documenters[] =
	{
		"Abel Cheung <deaddog@deaddog.org>",
		"Emese Kovacs <emese@gnome.hu>",
		NULL
	};	

	GdkPixbuf *pixbuf = NULL;
	gchar *image = gnome_program_locate_file(NULL, GNOME_FILE_DOMAIN_PIXMAP, "gtranslator.png",TRUE, NULL);
	GdkPixbuf *file = gdk_pixbuf_new_from_file (image, NULL);
	GtkWidget *about = g_object_new (GTK_TYPE_ABOUT_DIALOG,
                                     "name", PACKAGE_NAME, 
                                     "version", VERSION,
			             "copyright", _("(C) 1999-2005 The Free Software Foundation, Inc."),
				     "comments", _("gtranslator is a po file editing suite with many bells and whistles."),
			             "authors", authors,
			             "documenters", documenters,
			             "translator-credits", _("translator-credits"),
				     "logo", file,
                                     NULL);
	if (about != NULL)
	{
		gtk_window_present(GTK_WINDOW(about));
		return;
	}

	if (file) {
		pixbuf = gdk_pixbuf_new_from_file (image, NULL);
		g_free (file);
	} else
		g_warning (G_STRLOC ": gtranslator.png cannot be found");

	if (pixbuf) {
		g_object_unref (pixbuf);
	}
	
	g_signal_connect (G_OBJECT (about), "destroy",
			  G_CALLBACK (gtk_widget_destroyed), &about);
	
	gtk_widget_show (about);
}
