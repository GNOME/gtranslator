/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
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

#ifndef GTR_MENUS_MISC_H
#define GTR_MENUS_MISC_H 1

#include "about.h"
#include "prefs.h"
#include "utils.h"

#include <libgnomeui/gnome-app.h>
#include <libgnomeui/gnome-app-helper.h>
#include <libgnomeui/gnome-stock.h>

static GnomeUIInfo the_settings_menu[] = {
	GNOMEUIINFO_MENU_PREFERENCES_ITEM(gtranslator_preferences_dialog_create, NULL),
	GNOMEUIINFO_END
};

static GnomeUIInfo the_help_menu[] = {
	GNOMEUIINFO_HELP("gtranslator"),
	GNOMEUIINFO_MENU_ABOUT_ITEM(gtranslator_create_about_box, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(N_("gtranslator _website"),
			       N_("gtranslator's homepage on the web"),
			       gtranslator_utils_show_home_page,
			       GNOME_STOCK_MENU_HOME),
	GNOMEUIINFO_END
};

#endif
