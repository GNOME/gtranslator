/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gmx.net>
 *			Gediminas Paulauskas <menesis@delfi.lt>
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

#ifndef GTR_PARSE_H
#define GTR_PARSE_H 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#ifdef HAVE_MY_REGEX_CACHE
  #include <libgnome/gnome-regex.h>
#else
  #include "gnome-regex.h"
#endif

#include <libgtranslator/messages.h>
#include "header_stuff.h"

/*
 * The general po-file structure
 */
typedef struct {
	/*
	 * Absolute file name
	 */
	gchar *filename;
	/*
	 * The header 
	 */
	GtrHeader *header;
	/*
	 * All the po->messages are stored here 
	 */
	GList *messages;
	/*
	 * The length of messages list 
	 */
	guint length;
	/*
	 * A pointer to the currently displayed message 
	 */
	GList *current;
	/*
	 * Marks if the file was changed; 
	 */
	guint file_changed : 1;
	/*
	 * Is the file write-permitted?
	 */
	gboolean no_write_perms;
	/*
	 * The translated entries. 
	 */
	guint translated;
	/*
	 * The fuzzy entries. 
	 */
	guint fuzzy;
} GtrPo;

#define GTR_PO(x) ((GtrPo *)x)

/*
 * The main variable for storing info about file 
 */
GtrPo *po;

/*
 * Marks if any file was opened 
 */
gboolean file_opened;

/*
 * Marks if the current message was changed; 
 */
gboolean message_changed;

/*
 * The variables which store the count-stuff.
 */
gfloat percentage;

/*
 * Changes message fuzzy status
 */
void mark_msg_fuzzy(GtrMsg  * msg, gboolean fuzzy);

/*
 * Copies msgid to msgstr or blanks msgstr 
 */
void mark_msg_sticky(GtrMsg  * msg, gboolean on);

/*
 * The internally used parse-function
 */
void parse(const gchar *po);

/*
 * Callbacks for the widgets
 */
void parse_the_file(GtkWidget  * widget, gpointer of_dlg);
void parse_the_file_from_the_recent_files_list(GtkWidget *widget, gpointer filepointer);
void save_the_file(GtkWidget  * widget, gpointer sfa_dlg);
void save_current_file(GtkWidget  * widget, gpointer useless);
void revert_file(GtkWidget  * widget, gpointer useless);
void close_file(GtkWidget  * widget, gpointer useless);

/*
 * The compile function.
 */
void compile(GtkWidget  * widget, gpointer useless);

/*
 * The update function (now outsources to update.c).
 */
void update(GtkWidget *widget, gpointer useless);

/*
 * Display the recent files.
 */ 
void gtranslator_display_recent(void);

/*
 * Checks the given file for the right permissions for
 *  open/writing the files.
 */
gboolean gtranslator_check_file_perms(GtrPo *po_file);

/*
 * This function gets the count of the translated messages
 *  from the given po-file.
 */
void get_translated_count(void);

/*
 * Set the progress bar status.
 */
void gtranslator_set_progress_bar(void);

/*
 * A cache for saving regexps 
 */
GnomeRegexCache *rxc;

#endif
