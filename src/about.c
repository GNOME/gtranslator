/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gmx.net>
 *			Gediminas Paulauskas <menesis@delfi.lt>
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

#include "about.h"

/*
 * Creates and shows the about box for gtranslator.
 */ 
void about_box(GtkWidget * widget, gpointer useless)
{
	static GtkWidget *about = NULL;
	const gchar *authors[] = {
		"Fatih Demir <kabalak@gmx.net>",
		"Gediminas Paulauskas <menesis@delfi.lt>",
		NULL
	};
	raise_and_return_if_exists(about);

	/*
	 * Create the about box via gnome_about_new.
	 */ 
	about =
	    gnome_about_new("gtranslator", VERSION, "(C) 1999-2001", authors,
			    _("gtranslator will be a Gnome framework for translating these emminent important po-files.\nIt will be based upon a gtranslatord and a gtranslator client. Wish the best for us translators and for me!"),
			    NULL);

	show_nice_dialog(&about, "gtranslator -- about");
}
