#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>

#include "preferences-functions.h"
#include "preferences-interface.h"
#include "support.h"


gboolean
gtranslator_preferences_quit           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return FALSE;
}


void
index_row_is_selected                  (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

}


void
index_clicked_row                      (GtkCList        *clist,
                                        gint             column,
                                        gpointer         user_data)
{

}


void
index_nothing_selected                 (GtkCList        *clist,
                                        gpointer         user_data)
{

}


void
preferences_apply                      (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
preferences_ok                         (GtkButton       *button,
                                        gpointer         user_data)
{

}

