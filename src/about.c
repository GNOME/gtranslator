/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
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

/*
 * Creates and shows the about box for gtranslator.
 */ 
void gtranslator_create_about_box(GtkWidget * widget, gpointer useless)
{
	static GtkWidget *about = NULL;
	GtkWidget *hbox;
	GtkWidget *scheme, *author;
	gchar *umfo, *url;
	
	const gchar *authors[] = {
		"Fatih Demir <kabalak@gtranslator.org>",
		"Gediminas Paulauskas <menesis@gtranslator.org>",
		NULL
	};
	
	gtranslator_raise_dialog(about);

	/*
	 * Create the about box via gnome_about_new.
	 */ 
	about =
	    gnome_about_new("gtranslator", VERSION,
		_("(C) 1999-2001 The Free Software Foundation"), authors,
		_("gtranslator is a po file editing suite with many bells and whistles."),
		NULL);
	
	hbox=gtk_hbox_new(TRUE, 0);
	
	/*
	 * Print out some colorscheme informations in a label which will be
	 *  followed by the author name/email address.
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

	/*
	 * Free the used gchars.
	 */
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
