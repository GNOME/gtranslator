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

#ifndef GTR_MENUS_MESSAGE_STATUS_H
#define GTR_MENUS_MESSAGE_STATUS_H 1

#include "message.h"

#include <gdk/gdkkeysyms.h>
#include <libgnomeui/gnome-app.h>
#include <libgnomeui/gnome-app-helper.h>
#include <libgnomeui/gnome-stock.h>

static GnomeUIInfo the_msg_status_menu[] = {
	{
		GNOME_APP_UI_TOGGLEITEM, N_("_Translated"),
		N_("Toggle translated status of a message"),
		gtranslator_message_change_status,
		GINT_TO_POINTER(GTR_MSG_STATUS_TRANSLATED),
		NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_GREEN,
		GDK_1, GDK_MOD1_MASK, NULL
	},
	{
		GNOME_APP_UI_TOGGLEITEM, N_("_Fuzzy"),
		N_("Toggle fuzzy status of a message"),
		gtranslator_message_change_status,
		GINT_TO_POINTER(GTR_MSG_STATUS_FUZZY),
		NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_RED,
		GDK_2, GDK_MOD1_MASK, NULL
	},
	{
		GNOME_APP_UI_TOGGLEITEM, N_("_Stick"),
		N_("Stick this message"),
		gtranslator_message_change_status,
		GINT_TO_POINTER(GTR_MSG_STATUS_STICK),
		NULL,
		GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_COPY,
		GDK_3, GDK_MOD1_MASK, NULL
	},
	GNOMEUIINFO_END
};

#endif
