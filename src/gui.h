/**
* Fatih Demir <kabalak@gmx.net>
* Gediminas Paulauskas <menesis@delfi.lt>
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* This creates the main app and defines simple callbacks
*
* -- the header
**/

#ifndef GTR_GUI_H
#define GTR_GUI_H 1

#include <gnome.h>

#ifdef USE_WINDOW_ICON
#include <libgnomeui/gnome-window-icon.h>
#endif

/**
* The globally needed widgets
**/

GtkWidget *app1;
GtkWidget *trans_box;
GtkWidget *text1;
GtkWidget *appbar1;

void create_app1(void);

/**
* This creates the popup-menu.
**/
void create_popup_menu(GtkWidget *widget, GdkEventButton *event);

/**
* For the status messages
**/
gchar status[128];

/* Actions IDs */
enum {
	/* FIXME: some time this is disabled */
	ACT_NULL,
	ACT_COMPILE,
	ACT_SAVE,
	ACT_UPDATE,
	ACT_SAVE_AS,
	ACT_REVERT,
	ACT_CLOSE,
	/************/
	ACT_UNDO,
	ACT_CUT,
	ACT_COPY,
	ACT_PASTE,
	ACT_CLEAR,
	ACT_FIND,
	ACT_FIND_AGAIN,
	ACT_HEADER,
	ACT_SPELL,
	/************/
	ACT_FIRST,
	ACT_BACK,
	ACT_NEXT,
	ACT_LAST,
	ACT_GOTO,
	ACT_NEXT_FUZZY,
	ACT_NEXT_UNTRANSLATED,
	/************/
	ACT_TRANSLATED,
	ACT_FUZZY,
	ACT_STICK,
	/* This must always be the last, add new entries above */
	ACT_END
};

void change_actions(gboolean state, ...);
#define enable_actions(args...) change_actions(TRUE, args, ACT_END)
#define disable_actions(args...) change_actions(FALSE, args, ACT_END)
void disable_actions_no_file(void);
void enable_actions_just_opened(void);

/* Various functions for displaying messages */
void update_msg(void);
void display_msg(GList * list_item);
void clean_text_boxes(void);
void toggle_msg_status(GtkWidget * widget, gpointer which);

/* Shows the needed message in text boxes */
void goto_given_msg(GList * to_go);
void goto_first_msg(GtkWidget * widget, gpointer useless);
void goto_prev_msg(GtkWidget * widget, gpointer useless);
void goto_next_msg(GtkWidget * widget, gpointer useless);
void goto_last_msg(GtkWidget * widget, gpointer useless);
void goto_nth_msg(GtkWidget * widget, gpointer number);

/* If TRUE, means that trans_box is being changed by program, not user */
gboolean nothing_changes;

#endif
