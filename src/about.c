/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gtranslator.org>
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
#include "color-schemes.h"
#include "dialogs.h"

#include <gtk/gtkhbox.h>
#include <gtk/gtklabel.h>
#include <libgnome/gnome-defs.h>
#include <libgnome/gnome-i18n.h>
#include <libgnomeui/gnome-about.h>
#include <libgnomeui/gnome-href.h>
#include <string.h>

/*
 * Creates and shows the about box for gtranslator.
 */ 
void gtranslator_about_dialog(GtkWidget * widget, gpointer useless)
{
	static GtkWidget *about = NULL;
	GtkWidget *hbox;
	GtkWidget *scheme, *author;
	gchar *umfo, *url;
	gchar *bottom_line;
	
	const gchar *authors[] = {
		"Fatih Demir 		<kabalak@gtranslator.org>",
		"Gediminas Paulauskas 	<menesis@gtranslator.org>",
		"Thomas Ziehmer 	<thomas@gtranslator.org>",
		"Peeter Vois 		<peeter@gtranslator.org>",
		"",
		_("Messages table:"),
		"Kevin Vandersloot 	<kfv101@psu.edu>",
		"",
		_("Documentation:"),
		"Emese Kovacs 		<emese@gnome.hu>",
		NULL
	};
	
	gtranslator_raise_dialog(about);


	/* Translators should localize the following string
 	 * which will be displayed at the bottom of the about
	 * box to give credit to the translator(s).
	 * Translate the "Translation:", add your name and an
	 * email address. You can span several lines with a
	 * newline character if needed, but it shouldn't be
	 * too long; vertical space is limited in the about
	 * dialog. 
	 * If you dont translate it nothing will be added 
	 */
	if (strcmp (_("Translation:"), "Translation:") == 0) {
		bottom_line = g_strdup 
			("gtranslator is a po file editing suite with many bells and whistles.");
	} else {
		bottom_line = g_strconcat (
			_("gtranslator is a po file editing suite with many bells and whistles."),
			"\n \n",
			_("Translation:"), NULL);
	}

	/*
	 * Create the about box via gnome_about_new.
	 */ 
	about =
	    gnome_about_new("gtranslator", VERSION,
		_("(C) 1999-2001 The Free Software Foundation"), authors,
		bottom_line,
		NULL);

	g_free(bottom_line);
	
	hbox=gtk_hbox_new(TRUE, 0);
	
	/*
	 * This string is displayed in the about box as an information about
	 *  the currently used colorscheme.
	 */
	umfo=g_strdup_printf(_("Current colorscheme: \"%s\" (version %s) by"), 
		theme->info->name, theme->info->version);
	scheme=gtk_label_new(umfo);

	g_free(umfo);

	/*
	 * Show author name and bind in a GnomeHREF widget for contacting
	 *  the scheme author directly.
	 */
	umfo=g_strdup_printf("%s <%s>",
		theme->info->author, theme->info->author_email);
	url=g_strdup_printf("mailto:%s", theme->info->author_email);

	author=gnome_href_new(url, umfo);

	g_free(umfo);
	g_free(url);

	/*
	 * Add all the new widgets to the about dialog.
	 */
	gtk_box_pack_start(GTK_BOX(hbox), scheme, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), author, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(about)->vbox), hbox,
		TRUE, TRUE, 0);

	gtk_widget_show_all(hbox);
	
	gtranslator_dialog_show(&about, "gtranslator -- about");
}
