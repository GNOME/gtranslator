/*
 * (C) 2000-2003 	Fatih Demir <kabalak@kabalak.net>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Peeter Vois <peeter@kabalak.net>
 *			Ross Golder <ross@kabalak.net>
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

#include <gtk/gtkwidget.h>
#include <gtk/gtkeditable.h>

#include <libgnomeui/libgnomeui.h>

/*
 * #include <libgnome/gnome-defs.h>
 */

#include <libgnome/gnome-i18n.h>

/*
 * The globally needed widgets
 */
extern GtkWidget *gtranslator_application;
extern GtkWidget *gtranslator_messages_table;
extern GtkTextView *trans_box;
extern GtkTextView *text_box;
extern GtkWidget *gtranslator_application_bar;

extern GtkWidget *sidebar_pane;
extern GtkWidget *table_pane;
extern GtkWidget *content_pane;

/*
 * This is the "extra" content structure for the comment area 
 *  and the corresponding variable .-)
 */
typedef struct
{
	GtkWidget	*box;

	GtkWidget	*comment;
	GtkWidget	*edit_button;
} GtrExtraContentArea;

extern GtrExtraContentArea *extra_content_view;

/*
 * Creates the main window ans sets up the environment.
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
void gtranslator_text_boxes_clean(void);

/*
 * Callbacks for text operations
 */
void gtranslator_clipboard_cut(GtkWidget  *widget, gpointer useless);
void gtranslator_clipboard_copy(GtkWidget  *widget, gpointer useless);
void gtranslator_clipboard_paste(GtkWidget  *widget, gpointer useless);
void gtranslator_selection_clear(GtkWidget  *widget, gpointer useless);
void gtranslator_selection_set(GtkTextView *text_view, gint start, gint end);

/*
 * A status defining callback -- now generaly defined for instance.
 */
void gtranslator_translation_changed(GtkWidget  *widget, gpointer useless);

/*
 * If TRUE, means that trans_box is being changed by program, not user
 */
extern gboolean nothing_changes;

#endif
