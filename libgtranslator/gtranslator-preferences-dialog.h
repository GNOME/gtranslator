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
**/

#ifndef GTRANSLATOR_PREFERENCES_DIALOG_H
#define GTRANSLATOR_PREFERENCES_DIALOG_H 1

#include <libgtranslator/preferences.h>
#include <gtk/gtk.h>

/**
* This is an own preferences dialog widget which tries to
*  include some kind of intelligent preferences setting.
**/

/**
* The typedef for the structure.
**/
typedef struct GtranslatorPreferencesDialogInternal GtranslatorPreferencesDialog;

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
	* The default preferences page ( we're starting
	*  at '1', not at '0' !!!! )
	**/
	guint		default_page : 1;
	/**
	* The current page's #.
	**/
	guint		current_page : 1;
};

/**
* The general stuff.
**/

/**
* Create a new dialog for us :-)
**/
GtranslatorPreferencesDialog *gtranslator_preferences_dialog_new(
	const gchar *title);

/**
* Hide the preferences dialog.
**/
void gtranslator_preferences_dialog_hide(
	GtranslatorPreferencesDialog *dialog);

/**
* Button stuff.
**/

/**
* Some macros to easify the button actions.
*
* 1) The enable karavane...
**/
#define gtranslator_preferences_dialog_enable_apply_button(dialog) \
	gtranslator_preferences_dialog_enable_buttons( \
		dialog, TRUE, FALSE, FALSE);
#define gtranslator_preferences_dialog_enable_ok_button(dialog) \
	gtranslator_preferences_dialog_enable_buttons( \
		dialog, FALSE, TRUE, FALSE);
#define gtranslator_preferences_dialog_enable_cancel_button(dialog) \
	gtranslator_preferences_dialog_enable_buttons( \
		dialog, FALSE, FALSE, TRUE);
/**
* 2) The disable karavane...
**/
#define gtranslator_preferences_dialog_disable_apply_button(dialog) \
	gtranslator_preferences_dialog_disable_buttons( \
		dialog, TRUE, FALSE, FALSE);
#define gtranslator_preferences_dialog_disable_ok_button(dialog) \
	gtranslator_preferences_dialog_disable_buttons( \
		dialog, FALSE, TRUE, FALSE);
#define gtranslator_preferences_dialog_disable_cancel_button(dialog) \
	gtranslator_preferences_dialog_disable_buttons( \
		dialog, FALSE, FALSE, TRUE);

/**
* Enable the buttons defined by the arguments.
**/
void gtranslator_preferences_dialog_enable_buttons(
	GtranslatorPreferencesDialog *dialog, gboolean apply_button,
	gboolean ok_button, gboolean cancel_button);
/**
* And disable them necessarily.
**/
void gtranslator_preferences_dialog_disable_buttons(
	GtranslatorPreferencesDialog *dialog, gboolean apply_button,
	gboolean ok_button, gboolean cancel_button);
	
/**
* Section stuff.
**/
	
/**
* Add another settings section to the dialog.
**/
void gtranslator_preferences_dialog_append_section(
	GtranslatorPreferencesDialog *dialog, const gchar *name,
	GtkWidget *section_widget);
/**
* Delete the given section by name.
**/
gint gtranslator_preferences_dialog_delete_section_by_name(
	GtranslatorPreferencesDialog *dialog, const gchar *name);
/**
* Or by the # in the list.
**/
gint gtranslator_preferences_dialog_delete_section(
	GtranslatorPreferencesDialog *dialog, gint position);

/**
* Position setting stuff.
**/

/**
* Set the default page.
**/
void gtranslator_preferences_dialog_set_default_page(
	GtranslatorPreferencesDialog *dialog, gint position);
/**
* S|Get the current page number.
**/
guint gtranslator_preferences_dialog_get_current_page(
	GtranslatorPreferencesDialog *dialog);
void gtranslator_preferences_dialog_set_current_page(
	GtranslatorPreferencesDialog *dialog, gint position);

#endif
