/**
* Gediminas Paulauskas <menesis@delfi.lt>
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* The find-routines for gtranslator.
*
* -- the header.
**/

#ifndef GTR_FIND_H
#define GTR_FIND_H 1

#include <gtk/gtkwidget.h>

typedef gboolean (*FEFunc) (gpointer list_item, gpointer user_data);

gboolean for_each_msg(GList * first, FEFunc func, gpointer user_data);

/**
* The search function
**/
void find_do(GtkWidget * widget, gpointer target);

/* Finds and shows next fuzzy message */
void goto_next_fuzzy(GtkWidget * widget, gpointer useless);

/* Finds and shows next untranslated message */
void goto_next_untranslated(GtkWidget * widget, gpointer useless);

void update_flags(void);

#endif
