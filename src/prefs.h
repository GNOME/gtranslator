/*
 * (C) 2000 	Fatih Demir <kabalak@gmx.net>
 *		Gediminas Paulauskas <menesis@delfi.lt>
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

#ifndef GTR_PREFS_H
#define GTR_PREFS_H 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <gtk/gtkwidget.h>
#include <gtk/gtksignal.h>

/*
 * Global variables used in the preferences.
 */
gchar *author, *email, *language, *mime, *enc, *lc, *lg;

/*
 * The preferences structure of gtranslator.
 */
struct {
	guint warn_if_fuzzy		: 1;
	guint warn_if_no_change		: 1;
	guint dont_save_unchanged_files : 1;
	guint save_geometry		: 1;
	guint unmark_fuzzy		: 1;
	/*
	 * Options, used NOT in prefs-box.
	 */
	guint match_case		: 1;
	guint find_in			: 2;
	guint fill_header		: 1;
	/*
	 * Specifies the use of the "dot char".
	 */
	guint dot_char			: 1;
	/* 
	 * Whether we'd use popup menus.
	 */
	guint popup_menu		: 1;
	/*
	 * Defines if the update function of gtranslator is enabled.
	 */
	guint update_function		: 1;
	/*
	 * The maximal number of entries in the recent files menu.
	 */
	guint recent_files		: 3;
	/*
	 * Whether to check every recent file for existence.
	 */
	guint check_recent_file		: 1;
	/*
	 * Really unuseful dialogs which can be enabled/disabled.
	 */
	guint uzi_dialogs		: 1;
	/*
	 * Defines if gtranslator should use his own font/color settings.
	 */
	guint use_own_specs		: 1;
	/*
	 * Specifies if the instant spelling check via gtkspell should be
	 *  enabled.
	 */
	guint instant_spell_check	: 1;
	/*
	 * The ispell command to use for the instant spelling check.
	 */
	gchar				*spell_command;
	/*
	 * The special font to be used within gtranslator.
	 */
	gchar				*font;
} wants;

/*
 * Creating the preferences box and the neededbackend-functions.
 */
void prefs_box(GtkWidget * widget, gpointer useless);
void read_prefs(void);
void free_prefs(void);

/*
 * Routines for saving/restoring/setting geometry of the main window.
 */
void save_geometry(void);
void restore_geometry(gchar *gstr);

/*
 * Font/color specific widgets used in the preferences box.
 */
GtkWidget *foreground, *background, *font;

/*
 * The lists for the combo-boxes..
 */
GList *languages_list, *encodings_list,
	*lcodes_list, *group_emails_list, *bits_list;

void create_lists(void);
gboolean destroy_lists(GtkWidget *widget, gpointer useless);

/*
 * Convenience functions for adding items 
 */
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
/*
 * Returns the added GtkTable to the preferences box.
 */ 
GtkWidget *append_page_table(GtkWidget * probox, gint rows, gint cols,
			     const char *label_text);

#endif
