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

#include "shortcutbar.h"
#include "gui.h"

/*
 * The internal icon callback method.
 */ 
GdkPixbuf *get_shortcut_icon(EShortcutBar *bar, const gchar *url,
	gpointer data);

/*
 * Shows the right view for the clicked icon on the sidebar.
 */ 
void select_icon(EShortcutBar *bar, GdkEvent *event, gint group,
	gint item);	

/*
 * Nomen est res naturalae. Shows an editable comment view.
 */ 
void show_comment(GtkWidget *text);

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
		-1, _("Views"));

	gtk_signal_connect(GTK_OBJECT(sidebar), "item_selected",
		GTK_SIGNAL_FUNC(select_icon), NULL);	
	
	return sidebar;
}

/*
 * Adds the given po file stats to the shortcut bar.
 */ 
void gtranslator_sidebar_add_po(GtrPo *po)
{
	g_return_if_fail(po->filename!=NULL);

	/*
	 * Add the current view models to the bar.
	 */ 
	e_shortcut_model_add_item(model, 0, -1, 
		"message:",
		"Message");

	e_shortcut_model_add_item(model, 0, -1,
		"comment:",
		"Comment");
}

/*
 * Clean the sidebar fro the icons.
 */ 
void gtranslator_sidebar_clear()
{
	/*
	 * Remove the view icons/items from the sidebar model.
	 */
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
		/*
		 * Switch the icon numbers for the right view.
		 */ 
		switch(item+1)
		{
			case 1:
				/*
				 * Just show the message.
				 */ 
				display_msg(po->current);
				break;
				
			case 2:
				/*
				 * Display the current comment or
				 *  show a helping message.
				 */  
				 show_comment(text1);
				 break;
				
			default:
				break;
		}
	}
}

/*
 * Sets the icons in the shortcut bar for the given url type.
 */
GdkPixbuf *get_shortcut_icon(EShortcutBar *bar, const gchar *url,
	gpointer data)
{
	GdkPixbuf *icon;
	gchar *pixmap_filename=g_new0(gchar,1);
	
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
		 * The comment: URI case:
		 */
		case 'c':
			pixmap_filename=gnome_pixmap_file("gtranslator.png");
			break;
			
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
		
		return icon;
	}
	else
	{
		return NULL;
	}
}

/*
 * Shows the comment of the current message in the given GtkText.
 */
void show_comment(GtkWidget *text)
{
	g_return_if_fail(text!=NULL);

	if(GTR_MSG(po->current->data)->comment)
	{
		gtk_editable_delete_text(GTK_EDITABLE(text), 0, -1);

		gtk_text_insert(GTK_TEXT(text), NULL, NULL, NULL,
			GTR_MSG(po->current->data)->comment, -1);
	}
	else
	{
		gtk_editable_delete_text(GTK_EDITABLE(text), 0, -1);

		gtk_text_insert(GTK_TEXT(text), NULL, NULL, NULL,
			_("No comment available for this message") , -1);
	}
}
