/*
 * (C) 2000-2002 	Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
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

#include <glib.h>
#include <gtk/gtkwidget.h>

#include "preferences.h"

/*
 * The preferences structure of gtranslator.
 */
struct {
	guint warn_if_fuzzy		: 1;
	guint save_geometry		: 1;
	guint unmark_fuzzy		: 1;
	
	guint match_case		: 1;
	guint find_in			: 3;
	guint ignore_hotkeys		: 1;
	guint use_learn_buffer		: 1;
	guint fuzzy_matching		: 1;
	guint auto_learn		: 1;
	guint show_messages_table	: 1;
	guint show_comment		: 1;
	guint collapse_all		: 1;
	guint show_plural_forms		: 1;

	/*
	 * Special options, taken by value in the prefs-box.
	 */
	guint fill_header		: 1;
	guint update_function		: 1;
	guint rambo_function		: 1;
	guint highlight			: 1;
	guint dot_char			: 1;
	guint check_recent_file		: 1;
	guint use_own_fonts		: 1;
	guint use_own_colors		: 1;
	guint use_own_mt_colors		: 1;
	guint instant_spell_check	: 1;
	guint use_own_dict		: 1;
	guint keep_obsolete		: 1;
	guint sweep_compile_file	: 1;
	guint autosave			: 1;
	guint autosave_with_suffix	: 1;
	guint max_history_entries	: 10;

	/*
	 * The autosave timeout in minutes.
	 */
	gfloat autosave_timeout;
	gfloat min_match_percentage;
	
	/*
	 * User-specified strings.
	 */
	gchar *autosave_suffix; 
	gchar *spell_command;
	gchar *dictionary;
	gchar *msgid_font;
	gchar *msgstr_font;
	gchar *scheme;
} GtrPreferences;

/*
 * Creating the preferences box and the needed backend-functions.
 */
void gtranslator_preferences_dialog_create(GtkWidget *widget, gpointer useless);
void gtranslator_preferences_read(void);
void gtranslator_preferences_free(void);

/*
 * Initialize the preferences with useful default values if there hasn't
 *  been any options set up yet.
 */
void gtranslator_preferences_init_default_values(void);

/*
 * Initialize the syntax highlighting default colors.
 */
void gtranslator_colors_init_default_colors(void);

#endif
