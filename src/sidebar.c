/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
 *		Gediminas Paulauskas <menesis@gtranslator.org>
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

#include "gui.h"
#include "sidebar.h"
#include "syntax.h"
#include "undo.h"
#include "views.h"

#include <libgnome/gnome-util.h>
#include <gal/widgets/e-unicode.h>

#define GetLocalString(x) (e_utf8_from_locale_string(x))

/*
 * The internal icon callback method.
 */ 
GdkPixbuf *get_shortcut_icon(EShortcutBar *bar, const gchar *url,
	gpointer data);

/*
 * Shows the right view for the clicked icon on the sidebar.
 */ 
static void select_icon(EShortcutBar *bar, GdkEvent *event, gint group,
	gint item);	

/*
 * Creates the sidebar (in our case the shortcut-bar).
 */
GtkWidget *gtranslator_sidebar_new()
{
	GtkWidget *sidebar;
	
	/*
	 * Create the EShortcutModel & EShortcutBar.
	 */ 
	model=e_shortcut_model_new();
	sidebar=e_shortcut_bar_new();

	/*
	 * Set the EShortcutModel for our sidebar.
	 */ 
	e_shortcut_bar_set_model(E_SHORTCUT_BAR(sidebar), model);

	/*
	 * This is our general icon callback for any item in the shortcut bar.
	 */
	e_shortcut_bar_set_icon_callback(E_SHORTCUT_BAR(sidebar),
		get_shortcut_icon, NULL);
	
	/*
	 * Our views sidebar.
	 */  
	e_shortcut_model_add_group(E_SHORTCUT_BAR(sidebar)->model,
		-1, GetLocalString(_("Views")));

	e_shortcut_bar_set_view_type(E_SHORTCUT_BAR(sidebar), 0, 
		E_ICON_BAR_SMALL_ICONS);

	gtk_signal_connect(GTK_OBJECT(sidebar), "item_selected",
		GTK_SIGNAL_FUNC(select_icon), NULL);	
	
	return sidebar;
}

/*
 * Adds the given po file stats to the shortcut bar.
 */ 
void gtranslator_sidebar_activate_views()
{
	g_return_if_fail(po->filename!=NULL);

	/*
	 * Add the current view models to the bar.
	 */ 
	e_shortcut_model_add_item(model, 0, -1, 
		"message:",
		GetLocalString(_("Message")));

	e_shortcut_model_add_item(model, 0, -1,
		"comment:",
		GetLocalString(_("Comment")));
	
	e_shortcut_model_add_item(model, 0, -1,
		"number:",
		GetLocalString(_("Number")));

	e_shortcut_model_add_item(model, 0, -1,
		"format:",
		GetLocalString(_("Format")));

	e_shortcut_model_add_item(model, 0, -1,
		"hotkey:",
		GetLocalString(_("Hotkey")));
}

/*
 * Clean the sidebar fro the icons.
 */ 
void gtranslator_sidebar_clear_views()
{
	/*
	 * Remove the view icons/items from the sidebar model.
	 */
	e_shortcut_model_remove_item(model, 0, 4);
	e_shortcut_model_remove_item(model, 0, 3);
	e_shortcut_model_remove_item(model, 0, 2);
	e_shortcut_model_remove_item(model, 0, 1);
	e_shortcut_model_remove_item(model, 0, 0);
}

/*
 * This is the "select" callback for the icons in the shortcut bar --
 *  should call the file/view in the future.
 */
void select_icon(EShortcutBar *bar, GdkEvent *event, gint group,
	gint item)
{
	if(event->button.button==1)
	{
		gtranslator_views_set(item);
	}
}

/*
 * Sets the icons in the shortcut bar for the given url type.
 */
GdkPixbuf *get_shortcut_icon(EShortcutBar *bar, const gchar *url,
	gpointer data)
{
	GdkPixbuf *icon;
	gchar *pixmap_filename;
	
	g_return_val_if_fail(url!=NULL, NULL);

	/*
	 * Switch corresponding to the url prefix the icon in the
	 *  shortcut bar.
	 */  
	switch(url[0])
	{
		/*
		 * The simple and normal message: URI case.
		 */ 
		case 'm':
			pixmap_filename=gnome_pixmap_file(
				"mc/application-x-po.png");
				break;
	
		/*
		 * The C-Format URI type.
		 */
		case 'f':
			pixmap_filename=gnome_pixmap_file(
				"mc/application-x-po.png");
				break;
		
		/*
		 * Number URI:
		 */
		case 'n':
			pixmap_filename=gnome_pixmap_file(
				"mc/application-x-po.png");
				break;

		/*
		 * Hotkey URI:
		 */
		case 'h':
			pixmap_filename=gnome_pixmap_file(
				"mc/application-x-po.png");
				break;
				
		/*
		 * The comment: URI case:
		 */
		case 'c':
			pixmap_filename=gnome_pixmap_file("gtranslator.png");
				break;

		/*
		 * Everything else.
		 */
		default:
			pixmap_filename=gnome_pixmap_file("gtranslator.png");
				break;
	}
	
	if(pixmap_filename)
	{
		icon=gdk_pixbuf_new_from_file(pixmap_filename);
		
		/*
		 * Ref the icon and return it for our shortcut.
		 */ 
		gdk_pixbuf_ref(icon);
		
		g_free(pixmap_filename);
		return icon;
	}
	else
		return NULL;
}
