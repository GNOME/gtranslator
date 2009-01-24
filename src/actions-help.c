/*
 * (C) 2000-2007 	Fatih Demir <kabalak@kabalak.net>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Roy-Magne Mo <rmo@sunnmore.net>
 *			Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANMSGILITY or FITNESS FOR A PARTICULAR PURMSGSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "window.h"

#include <glib.h>
#include <glib/gi18n.h>

#include <gtk/gtk.h>

/*
 * Show the user's guide for gtranslator
 */
void
gtranslator_cmd_help_contents (GtkAction   *action,
			       GtranslatorWindow *window)
{
	gtranslator_utils_help_display (GTK_WINDOW (window),
					"gtranslator",
					"gtranslator.xml");
}

/*
 * Creates and shows the about box for gtranslator.
 */ 
void
gtranslator_about_dialog(GtkAction *action,
			 GtranslatorWindow *window)
{
	gchar *license_trans;
	
	const gchar *authors[] = {
		N_("Current Developers"),
		"---------------------------------------------------------",
		"Pablo Sanxiao <psanxiao@gmail.org>",
		"Ignacio Casal Quinteiro <nacho.resa@gmail.com>",
		"Seán de Búrca <leftmostcat@gmail.com>",
		"",
		N_("Previous Developers"),
		"---------------------------------------------------------",
		"Ross Golder <ross@golder.org>",
		"Fatih Demir <kabalak@kabalak.net>",
		"Gediminas Paulauskas <menesis@delfi.lt>",
		"Thomas Ziehmer <thomas@kabalak.net>",
		"Peeter Vois <peeter@kabalak.net>",
		NULL
	};	
	const gchar *documenters[] =
	{
		"Abel Cheung <deaddog@deaddog.org>",
		"Emese Kovacs <emese@gnome.hu>",
		"Pablo Sanxiao <psanxiao@gmail.org>",
		NULL
	};	
	
	
	const gchar *license[] = {
		N_("This program is free software: you can redistribute it and/or modify "
		   "it under the terms of the GNU General Public License as published by "
		   "the Free Software Foundation, either version 3 of the License, or "
		   "(at your option) any later version."),
		N_("This program is distributed in the hope that it will be useful, "
		   "but WITHOUT ANY WARRANTY; without even the implied warranty of "
		   "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
		   "GNU General Public License for more details."),
		N_("You should have received a copy of the GNU General Public License "
		   "along with this program.  If not, see <http://www.gnu.org/licenses/>.")
	};


	license_trans = g_strconcat (_(license[0]), "\n\n",
	                             _(license[1]), "\n\n",
         	                     _(license[2]),
                	             NULL);                          

	
	gtk_show_about_dialog (GTK_WINDOW (window),
		"comments", _("Translation file editing suite for localization of applications and libraries."),
		"authors", authors,
		"copyright", _("Copyright © 1999-2008 Free Software Foundation, Inc."),
		"documenters", documenters,
		"license", license_trans,
		"logo-icon-name", "gtranslator",
		"title", _("About Gtranslator"),
		/*
		 * Note to translators: put here your name and email so it will show
		 * up in the "about" box
		 */
		"translator-credits", _("translator-credits"),
		"version", VERSION,
		"website", "http://gtranslator.sourceforge.net/",
		"wrap-license", TRUE,
		"website-label", _("Gtranslator Web Site"),
		NULL);

	g_free (license_trans);
}
