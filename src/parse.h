/**
* Fatih Demir <kabalak@gmx.net>
* Gediminas Paulauskas <menesis@delfi.lt>
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* Routines, which work with files and raw messages, or change them, are here
*
* -- the header
**/

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

/**
* The general po-file structure
**/
typedef struct {
	/* Absolute file name */
	gchar *filename;
	/* The header */
	GtrHeader *header;
	/* All the po->messages are stored here */
	GList *messages;
	/* The length of messages list */
	guint length;
	/* A pointer to the currently displayed message */
	GList *current;
	/* Marks if the file was changed; */
	guint file_changed : 1;
	/* The translated entries. */
	guint translated;
	/* The fuzzy entries. */
	guint fuzzy;
} GtrPo;

#define GTR_PO(x) ((GtrPo *)x)

/* The main variable for storing info about file */
GtrPo *po;

/* Marks if any file was opened */
gboolean file_opened;

/* Marks if the current message was changed; */
gboolean message_changed;

/**
* The variables which store the count-stuff.
**/
gfloat percentage;

/* Changes message fuzzy status */
void mark_msg_fuzzy(GtrMsg * msg, gboolean fuzzy);

/* Copies msgid to msgstr or blanks msgstr */
void mark_msg_sticky(GtrMsg * msg, gboolean on);

/**
* The internally used parse-function
**/
void parse(const gchar *po);

/**
* Callbacks for the widgets
**/
void parse_the_file(GtkWidget * widget, gpointer of_dlg);
void parse_the_file_from_the_recent_files_list(GtkWidget *widget, gpointer filepointer);
void save_the_file(GtkWidget * widget, gpointer sfa_dlg);
void save_current_file(GtkWidget * widget, gpointer useless);
void revert_file(GtkWidget * widget, gpointer useless);
void close_file(GtkWidget * widget, gpointer useless);

/**
* The compile function.
**/
void compile(GtkWidget * widget, gpointer useless);

/**
* Opens a given .mo/.gmo-file
**/ 
void gtranslator_open_mo_file(gchar *file);

/**
* This opens up a gzipped po-file or tries it at least..
**/
void gtranslator_open_gzipped_po_file(gchar *file);

/**
* The update function.
**/
void update(GtkWidget *widget, gpointer useless);

/**
* This shows you the most recent menus.
**/
void gtranslator_display_recent(void);

/**
* This function gets the count of the translated messages
*  from the given po-file.
**/
void get_translated_count(void);

/**
* Set the progress bar status.
**/
void gtranslator_set_progress_bar(void);

/* A cache for saving regexps */
GnomeRegexCache *rxc;

#endif
