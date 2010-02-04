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

#include "gtr-dirs.h"
#include "gtr-window.h"

#include <glib.h>
#include <glib/gi18n.h>

#include <gtk/gtk.h>

/*
 * Show the user's guide for gtranslator
 */
void
gtranslator_cmd_help_contents (GtkAction * action, GtranslatorWindow * window)
{
  gtranslator_utils_help_display (GTK_WINDOW (window),
				  "gtranslator", "gtranslator.xml");
}

/*
 * Creates and shows the about box for gtranslator.
 */
void
gtranslator_about_dialog (GtkAction * action, GtranslatorWindow * window)
{
  gchar *license_trans;

  const gchar *authors[] = {
    N_("Current Maintainers"),
    "---------------------------------------------------------",
    "Juan José Sánchez Penas <jjsanchez@igalia.com>",
    "Pablo Sanxiao <psanxiao@gnome.org>",
    "",
    N_("Current Developers"),
    "---------------------------------------------------------",
    "Pablo Sanxiao <psanxiao@gnome.org>",
    "Ignacio Casal Quinteiro <nacho.resa@gmail.com>",
    "Seán de Búrca <leftmostcat@gmail.com>",
    "",
    N_("Previous Developers"),
    "---------------------------------------------------------",
    "Fatih Demir <kabalak@kabalak.net> (Founder)",
    "Ross Golder <ross@golder.org>",
    "Gediminas Paulauskas <menesis@delfi.lt>",
    "Thomas Ziehmer <thomas@kabalak.net>",
    "Peeter Vois <peeter@kabalak.net>",
    "",
    N_("Contributors"),
    "---------------------------------------------------------",
    "Christian Kirbach <christian.kirbach@googlemail.com>",
    "Luca Ferreti <elle.uca@libero.it>",
    "Jordi Mallach <jordi@sindominio.net>",
    "Jonh Wendell <jwendell@gnome.org>",
    "Baris Cicek <baris@teamforce.name.tr>",
    "Francisco Puga <fran.puga@gmail.com>",
    "Yuri Penkin",
    NULL
  };
  const gchar *documenters[] = {
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

  GdkPixbuf *logo;
  gchar *pixmaps_dir;
  gchar *logo_file;

  pixmaps_dir = gtranslator_dirs_get_pixmaps_dir ();
  logo_file = g_build_filename (pixmaps_dir,
                                "gtranslator-logo.png",
                                NULL);
  g_free (pixmaps_dir);
  logo = gdk_pixbuf_new_from_file (logo_file, NULL);
  g_free (logo_file);


  license_trans = g_strconcat (_(license[0]), "\n\n",
			       _(license[1]), "\n\n", _(license[2]), NULL);


  gtk_show_about_dialog (GTK_WINDOW (window),
			 "comments",
			 _
			 ("Translation file editing suite for localization of applications and libraries."),
			 "authors", authors, "copyright",
			 _
			 ("Copyright © 1999-2008 Free Software Foundation, Inc."),
			 "documenters", documenters, "license", license_trans,
			 "logo", logo,
			 "title", _("About Gtranslator"),
			 /*
			  * Note to translators: put here your name and email so it will show
			  * up in the "about" box
			  */
			 "translator-credits", _("translator-credits"),
			 "version", VERSION,
			 "website", "http://gtranslator.sourceforge.net/",
			 "wrap-license", TRUE,
			 "website-label", _("Gtranslator Web Site"), NULL);

  if (logo)
    g_object_unref (logo);
  g_free (license_trans);
}
