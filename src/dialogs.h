/**
* Fatih Demir <kabalak@gmx.net>
* Gediminas Paulauskas <menesis@delfi.lt>
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* This is the general place for all dialogs
* used in gtranslator ...
*
* the header ...
**/

#ifndef GTR_GTR_DIALOGS_H
#define GTR_GTR_DIALOGS_H 1

#include "prefs.h"
#include "gui.h"
#include "parse.h"

#define raise_and_return_if_exists(dlg)\
	if (dlg) {\
		gdk_window_show(dlg->window);\
		gdk_window_raise(dlg->window);\
		return;\
	}

/* Sets window name/class, icon for it, connects the destroy signal, */
/* and shows it. dlg should be static or global */
void show_nice_dialog(GtkWidget ** dlg, const gchar * wmname);

/**
* The file open/save_as file selections
**/
void save_file_as(GtkWidget * widget, gpointer useless);
void open_file(GtkWidget * widget, gpointer useless);
gboolean ask_to_save_file(void);

/**
* The GoTo message dialog
**/
void goto_dlg(GtkWidget * widget, gpointer useless);

/**
* The basical search dialog
**/
void find_dialog(GtkWidget * widget, gpointer useless);

void compile_error_dialog(FILE * fs);
void no_changed_dialog(GtkWidget * widget, gpointer useless);

#endif
