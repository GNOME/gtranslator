/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* This creates the main app and defines simple callbacks
*
* -- the header
**/

#ifndef GTR_GUI_H
#define GTR_GUI_H 1

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <gnome.h>

#ifdef USE_WINDOW_ICON 
#include <libgnomeui/gnome-window-icon.h>
#endif

#include "about.h"
#include "dnd.h"
#include "dialogs.h"
#include "prefs.h"
#include "msg_db.h"
#include "parse.h"

void create_app1 (void);

/**
* The globally needed widgets
**/

GtkWidget *app1;
GtkWidget *trans_box;
GtkWidget *text1;
GtkWidget *appbar1; 

/**
* For the status messages
**/
gchar status[128];

/**
* Enable/disable the buttons of the toolbars if a file is opened/
*  closed.
**/
void enable_actions(const gchar *first, ...);
void disable_actions(const gchar *first, ...);
void disable_actions_no_file(void);

// Various functions for displaying messages
void update_msg(GList *li);
void display_msg(GList *list_item);
void clean_text_boxes(void);
void toggle_msg_status(GtkWidget *widget,gpointer which);

/** 
 * General functions -- these do operate on the global lists where they get
 * the first/previous/next/last msgid & msgstr's ...
**/

void goto_first_msg(GtkWidget *widget,gpointer useless);
void goto_prev_msg(GtkWidget *widget,gpointer useless);
void goto_next_msg(GtkWidget *widget,gpointer useless);
void goto_last_msg(GtkWidget *widget,gpointer useless);
void goto_nth_msg(GtkWidget *widget,gpointer adjustment);
void goto_given_msg(GtkWidget *widget,gpointer to_go);

/**
* The text-based callbacks
**/

gboolean nothing_changes;

/**
* The search function
**/
void search_do(GtkWidget *widget,gpointer wherefrom);

#endif
