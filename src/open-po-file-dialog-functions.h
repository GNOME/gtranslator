#include <gnome.h>


void
file_tree_select_child                 (GtkTree         *tree,
                                        GtkWidget       *widget,
                                        gpointer         user_data);

void
file_tree_unselect_child               (GtkTree         *tree,
                                        GtkWidget       *widget,
                                        gpointer         user_data);

void
file_tree_changed                      (GtkTree         *tree,
                                        gpointer         user_data);

void
open_po_file                           (GtkButton       *button,
                                        gpointer         user_data);

void
reopen_last_po_file                    (GtkButton       *button,
                                        gpointer         user_data);
