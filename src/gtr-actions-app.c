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

#include "gtr-actions-app.h"
#include "gtr-dirs.h"
#include "gtr-utils.h"
#include "gtr-window.h"

#include <glib.h>
#include <glib/gi18n.h>

#include <gtk/gtk.h>

/* Show the user's guide for gtranslator */
void
gtr_show_help (GtrWindow *window)
{
  gtr_utils_help_display (GTK_WINDOW (window), "gtranslator", "");
}

/* Creates and shows the about box for gtranslator */
void
gtr_about_dialog (GtrWindow *window)
{
  static const gchar *authors[] = {
    N_("Current Maintainers"),
    "------------------------",
    "Daniel Garcia Moreno <danigm@gnome.org>",
    "",
    N_("Previous Developers"),
    "------------------------",
    "Seán de Búrca <leftmostcat@gmail.com>",
    "Ignacio Casal Quinteiro <icq@gnome.org>",
    "Pablo Sanxiao <psanxiao@gnome.org>",
    "Fatih Demir <kabalak@kabalak.net> (Founder)",
    "Ross Golder <ross@golder.org>",
    "Gediminas Paulauskas <menesis@delfi.lt>",
    "Thomas Ziehmer <thomas@kabalak.net>",
    "Peeter Vois <peeter@kabalak.net>",
    "",
    N_("Contributors"),
    "------------------------",
    "Juan José Sánchez Penas <jjsanchez@igalia.com>",
    "Christian Kirbach <christian.kirbach@googlemail.com>",
    "Luca Ferreti <elle.uca@libero.it>",
    "Jordi Mallach <jordi@sindominio.net>",
    "Jonh Wendell <jwendell@gnome.org>",
    "Baris Cicek <baris@teamforce.name.tr>",
    "Francisco Puga <fran.puga@gmail.com>",
    "Yuri Penkin",
    NULL
  };
  static const gchar *documenters[] = {
    "GNOME Documentation Project",
    "Abel Cheung <deaddog@deaddog.org>",
    "Emese Kovacs <emese@gnome.hu>",
    "Pablo Sanxiao <psanxiao@gnome.org>",
    NULL
  };

  static const gchar copyright[] =
    "Copyright © 1999-2008 Free Software Foundation, Inc.\n"
    "Copyright © 2007-2012 Ignacio Casal Quinteiro";

  static const gchar comments[] =
    N_("Translation file editing suite for localization of applications and libraries.");

  gtk_show_about_dialog (GTK_WINDOW (window),
                         "comments", _(comments),
                         "authors", authors,
                         "copyright", copyright,
                         "license-type", GTK_LICENSE_GPL_3_0,
                         "documenters", documenters,
                         "logo-icon-name", PACKAGE_APPID,
                         /*
                          * Note to translators: put here your name and email so it will show
                          * up in the "about" box
                          */
                         "translator-credits", _("translator-credits"),
                         "version", PACKAGE_VERSION,
                         "website", PACKAGE_URL,
                         "website-label", _("Translation Editor Web Site"),
                         NULL);

}
