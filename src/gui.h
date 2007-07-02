/*
 * (C) 2000-2007 	Fatih Demir <kabalak@kabalak.net>
 *			Ross Golder <ross@golder.org>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Peeter Vois <peeter@kabalak.net>
 *			Ignacio Casal <nacho.resa@gmail.com>
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

#ifndef GTR_GUI_H
#define GTR_GUI_H 1

#include <gtk/gtk.h>
/*#include <gtk/gtkwidget.h>
#include <gtk/gtkeditable.h>*/
#include <glade/glade-xml.h>

#include <glib/gi18n.h>


typedef struct _GtrWindow GtrWindow;

struct _GtrWindow
{
	GtkWidget *app;
	GtkWidget *statusbar;
	GtkWidget *progressbar;
	GtkWidget *toolbar;
	GladeXML *glade;
	guint *context_id;
};
	
/*
 * The globally needed widgets
 */
extern GtkWidget *gtranslator_application;
extern GtkWidget *gtranslator_status_bar;
extern GtkWidget *gtranslator_progress_bar;
extern GtkWidget *gtranslator_toolbar;

extern GladeXML *glade;

extern GtkWidget *sidebar_pane;

/*Statusbar context id*/
extern guint *status_id;

/*
 * A handle on the notebook, for parse.c to add/remove pages
 */
extern GtkWidget *notebook_widget;

/*
 * Pane positions storage variables.
 */
extern gint	table_pane_position;

/*
 * To get warning_hbox widget
 */
GtkWidget *gtranslator_gui_get_warning_hbox();

/*
 * Creates the main window and sets up the environment.
 */
void gtranslator_create_main_window(void);

/* Callback called when the user closes the main window */
gboolean gtranslator_application_delete_event_cb(GtkWidget  * widget,
						 GdkEvent  * event,
						 gpointer user_data);

/*
 * Callback called when the user uses the quit command (^Q or Quit in the menu)
 */
void gtranslator_menu_quit_cb(void  * data);

/*
 * Quits from gtranslator.
 */
void gtranslator_quit();

/*
 * Various functions for displaying messages 
 */
void gtranslator_application_bar_update(gint pos);

/*
 * Callbacks for text operations
 */
void gtranslator_clipboard_cut(GtkWidget  *widget, gpointer useless);
void gtranslator_clipboard_copy(GtkWidget  *widget, gpointer useless);
void gtranslator_clipboard_paste(GtkWidget  *widget, gpointer useless);
void gtranslator_selection_clear(GtkWidget  *widget, gpointer useless);
//void gtranslator_selection_set(GtkTextView *text_view, gint start, gint end);
void
gtranslator_selection_set(GtkTextView *text_view, gint start, gint end);

/*
 * A status defining callback -- now generaly defined for instance.
 */
void gtranslator_translation_changed(GtkWidget  *widget, gpointer useless);

/*
 * Update the progress bar according to how many messages have been
 * translated.
 */
void gtranslator_update_progress_bar(void);

/*
 * Statusbar handling
 */
void push_statusbar_data(GtkWidget *widget, gpointer data);
void pop_statusbar_data(GtkWidget *widget, gpointer useless);

/*
 * If TRUE, means that trans_box is being changed by program, not user
 */
extern gboolean nothing_changes;

#endif
