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
#include <libgnomeui/gnome-dialog.h>

/**
* The source for this new GtranslatorPreferencesDialog widget.
**/

/**
* The default widget/window title.
**/
#define GTRANSLATOR_PREFERENCES_DIALOG_DEFAULT_TITLE \
	_("libgtranslator -- GtranslatorPreferencesDialog")

/**
* The pane splitting up the index and the
*  content window.
**/
GtkWidget			*pane;

/**
* The "new" widget.
**/
GtranslatorPreferencesDialog	*window;

/**
* Create and return the new widget.
**/
GtranslatorPreferencesDialog *gtranslator_preferences_dialog_new(
	gchar *title)
{
	/**
	* Partial, private widgets.
	**/
	GtkWidget	*frame;
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
		sprintf(window->title, "%s", GTRANSLATOR_PREFERENCES_DIALOG_DEFAULT_TITLE);
	}
	else
	{
		sprintf(window->title, "%s", title);
	}
	/**
	* Create the new window.
	**/
	window->prefs_window=gnome_dialog_new(window->title, 
		GNOME_STOCK_BUTTON_APPLY,
		GNOME_STOCK_BUTTON_OK,
		GNOME_STOCK_BUTTON_CANCEL,
		NULL);
	/**
	* Create the (for now) GtkPane.
	**/
	pane=gtk_vpaned_new();	
	/**
	* Create the clist, set the options, create the frame and
	*  insert it into the pane.
	**/
	window->index=gtk_clist_new(1);
	frame=gtk_frame_new(_("Default"));
	gtk_paned_add1(GTK_PANED(pane), window->index);
	gtk_paned_add2(GTK_PANED(pane), frame);
	/**
	* Add the pane to the main window.
	**/
	gtk_container_add(GTK_CONTAINER(GNOME_DIALOG(window->prefs_window)->vbox), pane);
	/**
	* Check the widget with an assertion..
	**/
	g_assert(window!=NULL);
	/**
	* At least return the new widget.
	**/
	return window;
}
