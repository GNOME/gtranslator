/**
* Fatih Demir <kabalak@gmx.net>
* Gediminas Paulauskas <menesis@delfi.lt>
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* Here has the preferences box got his own
*  home file ...
*
* -- the header
**/

#ifndef GTR_PREFS_H
#define GTR_PREFS_H 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <gtk/gtkwidget.h>
#include <gtk/gtksignal.h>

/**
* These are the variables set by the prefs-box ( globally ones )
**/
gchar *author, *email, *language, *mime, *enc, *lc, *lg;

/* A compact structure to store all the preferences */
struct {
	guint warn_if_fuzzy		: 1;
	guint warn_if_no_change		: 1;
	guint dont_save_unchanged_files : 1;
	guint save_geometry		: 1;
	guint unmark_fuzzy		: 1;
	/* Options, used NOT in prefs-box. */
	guint match_case		: 1;
	guint find_in			: 2;
	guint fill_header		: 1;
	/**
	* The curious dot_char option.
	**/
	guint dot_char			: 1;
	/**
	* The popup menu enableing/disabling option.
	**/
	guint popup_menu		: 1;
	/**
	* If the update function should be disabled all the time.
	**/
	guint update_function		: 1;
	/**
	* The number of entries in the Recent files menu.
	**/
	guint recent_files		: 3;
	/**
	* If the recent files should be checked.
	**/
	guint check_recent_file		: 1;
	/**
	* If to show the unneeded dialogs within the update function.
	**/
	guint uzi_dialogs		: 1;
	/**
	* If we'd use the own font and color specifications.
	**/
	guint use_own_specs		: 1;
} wants;

/* Preferences-box creation and callbacks */
void prefs_box(GtkWidget * widget, gpointer useless);
void read_prefs(void);
void free_prefs(void);

/* Routines for saving/restoring/setting geometry of the main window */
void save_geometry(void);
void restore_geometry(gchar * gstr);

/**
* Lists for the combo-boxes ..
**/
GList *languages_list, *encodings_list,
	*lcodes_list, *group_emails_list, *bits_list;

void create_lists(void);
gboolean destroy_lists(GtkWidget * widget, gpointer useless);

/**
 * Convenience functions for adding items 
 **/
GtkWidget *attach_combo_with_label(GtkWidget * table, gint row,
				   const char *label_text,
				   GList * list, const char *value,
				   GtkSignalFunc callback,
				   gpointer user_data);
GtkWidget *attach_entry_with_label(GtkWidget * table, gint row,
				   const char *label_text,
				   const char *value,
				   GtkSignalFunc callback);
GtkWidget *attach_toggle_with_label(GtkWidget * table, gint row,
				    const char *label_text,
				    gboolean value,
				    GtkSignalFunc callback);
GtkWidget *attach_text_with_label(GtkWidget * table, gint row,
				  const char *label_text,
				  const char *value,
				  GtkSignalFunc callback);
GtkWidget *append_page_table(GtkWidget * probox, gint rows, gint cols,
			     const char *label_text);

#endif
