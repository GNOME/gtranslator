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

#ifndef GTRANSLATOR_PREFERENCES_DIALOG_H
#define GTRANSLATOR_PREFERENCES_DIALOG_H 1

#include <libgtranslator/preferences.h>

/**
* This is an own preferences dialog widget which tries to
*  include some kind of intelligent preferences setting.
**/

/**
* The typedef for the structure.
**/
typedef struct GtranslatorPreferencesDialogInternal *GtranslatorPreferencesDialog;

/**
* The structure itself.
**/
struct GtranslatorPreferencesDialogInternal
{
	/**
	* The main window widget.
	**/
	GtkWidget 	*prefs_window;
	/**
	* The main widget/window title
	**/
	const gchar	*title;
	/**
	* The leftern side of the widget,
	*  named ofter it's purpose :-)
	**/
	GtkWidget	*index;
	/**
	* The rightern container for the
	*  content.
	**/
	GtkWidget	*content;
	/**
	* The buttons.
	**/
	GtkWidget 	*apply_button,
			*ok_button,
			*cancel_button;
	/**
	* The different pages as a GList "union".
	**/
	GList 		*prefs_pages;
	/**
	* The default preferences page.
	**/
	guint		default_page : 1;
};

/**
* Create a new dialog for us :-)
**/
GtranslatorPreferencesDialog *gtranslator_preferences_dialog_new(const gchar *title);

/**
* Hide the preferences dialog.
**/
void gtranslator_preferences_dialog_hide(GtranslatorPreferencesDialog *dialog);

#endif
