/**
* Fatih Demir [ kabalak@gmx.net ] 
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

#include "messages.h"
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
} GtrPo;

#define GTR_PO(x) ((GtrPo *)x)

/* The main variable for storing info about file */
GtrPo *po;

/* Marks if any file was opened */
gboolean file_opened;

/* Marks if the current message was changed; */
gboolean message_changed;

/* Changes message fuzzy status */
void mark_msg_fuzzy(GtrMsg * msg, gboolean fuzzy);

/**
* The internally used parse-function
**/
void parse(const char *po);

/**
* Callbacks for the widgets
**/
void parse_the_file(GtkWidget * widget, gpointer of_dlg);
void save_the_file(GtkWidget * widget, gpointer sfa_dlg);
void save_current_file(GtkWidget * widget, gpointer useless);
void revert_file(GtkWidget * widget, gpointer useless);
void close_file(GtkWidget * widget, gpointer useless);

void compile(GtkWidget * widget, gpointer useless);

/* A cache for saving regexps */
GnomeRegexCache *rxc;

#endif
