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
#include "preferences.h"
#include "sidebar.h"
#include "syntax.h"
#include "undo.h"
#include "utils.h"
#include "views.h"

#include <libgnome/gnome-util.h>

#include <gal/e-paned/e-paned.h>
#include <gal/shortcut-bar/e-shortcut-bar.h>
#include <gal/widgets/e-unicode.h>

/*
 * The shortcut bar releated widgets/variables/methods.
 */ 
static EShortcutModel	*model;
static GtkWidget	*sidebar;

/*
 * Erh, a small own data type for the shortcut view items.
 */
typedef struct
{
	const gchar	*name;
	const gchar	*uri_prefix;
} GtrShortcutViewItem;

/*
 * The internally used GtrShortcutViewItem array.
 */
static GtrShortcutViewItem gtranslator_shortcut_view_items[] = 
{
	{ N_("Message"), "message:" },
	{ N_("Number"), "number:" },
	{ N_("Format"), "format:" },
	{ N_("Hotkey"), "hotkey:" },
	{ NULL,	 NULL }
};

/*
 * The internal icon callback method.
 */ 
GdkPixbuf *get_shortcut_icon(const gchar *url);

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
	 * Our views/bookmarks sidebar.
	 */  
	e_shortcut_model_add_group(E_SHORTCUT_BAR(sidebar)->model,
		-1, e_utf8_from_locale_string(_("Views")));
	e_shortcut_model_add_group(E_SHORTCUT_BAR(sidebar)->model,
		-1, e_utf8_from_locale_string(_("Bookmarks")));

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
	GdkPixbuf	*pixbuf;
	gint		 count=0;
	
	g_return_if_fail(po->filename!=NULL);

	/*
	 * Cruise through our internally used array.
	 */
	while(gtranslator_shortcut_view_items[count].name!=NULL)
	{
		pixbuf=get_shortcut_icon(gtranslator_shortcut_view_items[count].uri_prefix);

		e_shortcut_model_add_item(model, 0, -1,
			gtranslator_shortcut_view_items[count].uri_prefix,
			e_utf8_from_locale_string(gtranslator_shortcut_view_items[count].name),
			pixbuf);

		gdk_pixbuf_unref(pixbuf);
		
		count++;
	}
}

/*
 * Clean the sidebar fro the icons.
 */ 
void gtranslator_sidebar_clear_views()
{
	/*
	 * Remove the view icons/items from the sidebar model.
	 */
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
GdkPixbuf *get_shortcut_icon(const gchar *url)
{
	GdkPixbuf *icon;
	gchar *pixmap_filename;
	
	g_return_val_if_fail(url!=NULL, NULL);
	g_return_val_if_fail(url[0]!='\0', NULL);

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
		
		GTR_FREE(pixmap_filename);
		return icon;
	}
	else
		return NULL;
}

/*
 * Hide the sidebar if possible -- should be really sane this way.
 */
gboolean gtranslator_sidebar_hide()
{
	gint position=-1;
	
	g_return_val_if_fail(sidebar_pane!=NULL, FALSE);
	
	position=e_paned_get_position(E_PANED(sidebar_pane));
	
	if(position<=0)
	{
		return FALSE;
	}
	else
	{
		gtranslator_config_init();
		gtranslator_config_set_int("interface/sidebar_pane_position", position);
		gtranslator_config_set_bool("toggles/show_sidebar", FALSE);
		gtranslator_config_close();
		
		e_paned_set_position(E_PANED(sidebar_pane), 0);
		return TRUE;
	}
}

/*
 * The show equivalent for the hide function.
 */
gboolean gtranslator_sidebar_show()
{
	gint position=-1;
	
	g_return_val_if_fail(sidebar_pane!=NULL, FALSE);
	
	gtranslator_config_init();
	position=gtranslator_config_get_int("interface/sidebar_pane_position");
	gtranslator_config_close();

	if(position<=0)
	{
		return FALSE;
	}
	else
	{
		e_paned_set_position(E_PANED(sidebar_pane), position);
		
		gtranslator_config_init();
		gtranslator_config_set_bool("toggles/show_sidebar", TRUE);
		gtranslator_config_close();
		
		return TRUE;
	}
}

/*
 * Show or hide the sidebar we're using -- here you can get why
 *  the show/hide functions were returning gboolean values.
 */
void gtranslator_sidebar_toggle()
{
	if(!gtranslator_sidebar_hide())
	{
		gtranslator_sidebar_show();
	}
}
