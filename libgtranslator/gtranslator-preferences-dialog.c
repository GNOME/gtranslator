/**
*
* (C) 2000 Fatih Demir -- kabalak / kabalak@gmx.net
*
* This is distributed under the GNU GPL V 2.0 or higher which can be
*  found in the file COPYING for further studies.
*
* Enjoy this piece of software, brain-crack and other nice things.
*
* WARNING: Trying to decode the source-code may be hazardous for all your
*	future development in direction to better IQ-Test rankings!
*
* PSC: This has been completely written with vim; the best editor of all.
*
**/

#include <libgtranslator/gtranslator-preferences-dialog.h>
#include <libgnomeui/gnome-stock.h>

/**
* The source for this new GtranslatorPreferencesDialog widget.
**/

/**
* The default widget/window title.
**/
#define GTRANSLATOR_PREFERENCES_DIALOG_DEFAULT_TITLE \
	"libgtranslator -- GtranslatorPreferencesDialog"

/**
* Create and return the new widget.
**/
GtranslatorPreferencesDialog gtranslator_preferences_dialog_new(
	const gchar *title)
{
	/**
	* Partial, private widgets.
	**/
	GtkWidget	*button_split;
	/**
	* Create a new dialog.
	**/
	GtranslatorPreferencesDialog window=NULL;
	/**
	* Check if we'd got a title for the widget and warn the
	*  user that there's no current title to use, so we use
	*   our default title :-)
	**/
	if(!title)
	{
		g_warning(_("No title set for a new GtranslatorPreferencesDialog widget!\n\
		Using the default title `%s'..."),
			GTRANSLATOR_PREFERENCES_DIALOG_DEFAULT_TITLE);
		/**
		* Set the title.
		**/	
		title=GTRANSLATOR_PREFERENCES_DIALOG_DEFAULT_TITLE;
	}
	/**
	* Create the new window.
	**/
	window->prefs_window=GTK_WIDGET(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	/**
	* Now set up the window title.
	**/
	gtk_window_set_title(GTK_WINDOW(window->prefs_window), title);
	/**
	* Set the modal status.
	**/
	gtk_window_set_modal(GTK_WINDOW(window->prefs_window), TRUE);
	/**
	* Now get the three default buttons.
	**/
	window->apply_button=gnome_stock_button(GNOME_STOCK_BUTTON_APPLY);
	window->ok_button=gnome_stock_button(GNOME_STOCK_BUTTON_OK);
	window->cancel_button=gnome_stock_button(GNOME_STOCK_BUTTON_CANCEL);
	/**
	* Create the new vbox.
	**/
	button_split=gtk_vbox_new(FALSE, 1);
	/**
	* Add the buttons to the new vbox.
	**/
	gtk_box_pack_start(GTK_BOX(button_split), window->apply_button,
		FALSE, FALSE, 1);
	gtk_box_pack_start(GTK_BOX(button_split), window->ok_button,
		FALSE, FALSE, 1);
	gtk_box_pack_end(GTK_BOX(button_split), window->cancel_button,
		FALSE, FALSE, 1);
	/**
	* Add the vbox into the main window.
	**/
	gtk_container_add(GTK_CONTAINER(window->prefs_window), button_split);	
	/**
	* Check the widget with an assertion..
	**/
	g_assert(window!=NULL);
	/**
	* At least return the new widget.
	**/
	return window;
}	
