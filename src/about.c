/*
 * (C) 2000-2002 	Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
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

#include <libgnomeui/gnome-about.h>

/*
 * Creates and shows the about box for gtranslator.
 */ 
void gtranslator_about_dialog(GtkWidget * widget, gpointer useless)
{
	static GtkWidget *about = NULL;
	GdkPixbuf *pixbuf = NULL;
	gchar *file;
	
	const gchar *authors[] = {
		"Fatih Demir <kabalak@gtranslator.org>",
		"Gediminas Paulauskas <menesis@delfi.lt>",
		"Thomas Ziehmer <thomas@gtranslator.org>",
		"Peeter Vois <peeter@gtranslator.org>",
		"Kevin Vandersloot <kfv101@psu.edu>",
		NULL
	};	
	static const char *documenters[] =
	{
		"Emese Kovacs <emese@gnome.hu>",
		NULL
	};	
	/* Translator credits */
	const char *translator_credits = _("translator_credits");

	if (about != NULL)
	{
		gtk_window_present(GTK_WINDOW(about));
		return;
	}

	pixbuf = NULL;
	
	file = gnome_program_locate_file (NULL, GNOME_FILE_DOMAIN_PIXMAP,
	                                  "gtranslator.png", TRUE, NULL);
	if (file) {
		pixbuf = gdk_pixbuf_new_from_file (file, NULL);
		g_free (file);
	} else
		g_warning (G_STRLOC ": gtranslator.png cannot be found");

	about =
	    gnome_about_new("gtranslator", VERSION,
		_("(C) 1999-2002 The Free Software Foundation"),
		_("gtranslator is a po file editing suite with many bells and whistles."),
		authors,
		documenters,
		strcmp (translator_credits, "translator_credits") != 0 ? translator_credits : NULL,
		pixbuf);

	if (pixbuf) {
		g_object_unref (pixbuf);
	}
	
	g_signal_connect (G_OBJECT (about), "destroy",
			  G_CALLBACK (gtk_widget_destroyed), &about);
	
	gtk_widget_show (about);
}
