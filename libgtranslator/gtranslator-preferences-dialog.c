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
* Create and return the new widget.
**/
GtranslatorPreferencesDialog *gtranslator_preferences_dialog_new()
{
	/**
	* The GladeXML document.
	**/
	GladeXML	*interface;
	/**
	* The "new" widget.
	**/
	GtranslatorPreferencesDialog	*window=g_new0(
		GtranslatorPreferencesDialog,1);
	/**
	* Now init the libglade for GNOME.
	**/
	glade_gnome_init();
	/**
	* Now get the apps' default preferences widget from glade.
	**/
	interface=glade_xml_new(INTERFACE_DIR "preferences.glade", NULL);
	/**
	* And get the main widget.
	**/
	window->prefs_window=glade_xml_get_widget(interface, "gtranslator_prefs_window");
	/**
	* At least return the new widget.
	**/
	return window;
}
