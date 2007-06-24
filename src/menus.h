/*
 * (C) 2001-2007 	Fatih Demir <kabalak@kabalak.net>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 * 			Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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

/*With libglade this is not neccessary anymore*/
/*With libglade we can get menus when we need it*/

#ifndef GTR_MENUS_H
#define GTR_MENUS_H 1

#include <gtk/gtkwidget.h>

/*
 * Menu items
 */
typedef struct _GtrMenuItems
{
	//File
	GtkWidget *compile;
	GtkWidget *refresh;
	GtkWidget *add_bookmark;
	GtkWidget *autotranslate;
	GtkWidget *remove_translations;
	GtkWidget *open;
	GtkWidget *open_uri;
	GtkWidget *save;
	GtkWidget *save_as;
	GtkWidget *revert;
	GtkWidget *close;
	GtkWidget *recent_files;
	GtkWidget *quit;
	
	//Edit
	GtkWidget *undo;
	GtkWidget *cut;
	GtkWidget *copy;
	GtkWidget *paste;
	GtkWidget *clear;
	GtkWidget *find;
	GtkWidget *search_next;
	GtkWidget *replace;
	GtkWidget *header;
	GtkWidget *comment;
	GtkWidget *copy_message;
	GtkWidget *fuzzy;
	GtkWidget *preferences;
	
	//View
	GtkWidget *bookmarks;
	GtkWidget *colorschemes;
	
	//Go
	GtkWidget *first;
	GtkWidget *go_back;
	GtkWidget *go_forward;
	GtkWidget *goto_last;
	GtkWidget *jump_to;
	GtkWidget *next_fuzzy;
	GtkWidget *next_untranslated;
	
	//About
	GtkWidget *contents;
	GtkWidget *website;
	GtkWidget *about;
	
	//Toolbar
	GtkWidget *t_save;
	GtkWidget *t_undo;
    	GtkWidget *t_first;
    	GtkWidget *t_go_back;
    	GtkWidget *t_go_forward;
    	GtkWidget *t_goto_last;
	
}GtrMenuItems;

extern GtrMenuItems *gtranslator_menuitems;

void gtranslator_menuitems_set_up();

void connect_menu_signals();

#endif
