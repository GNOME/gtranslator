#include "dictionary.h"
#include "dict-panel.h"
#include "gui.h"
#include <gdict/gdict.h>

#define GLADE_DICTIONARY_BOX "dictionary_box"

void
gtranslator_dictionary_create()
{
	GtkWidget *dictionary_box;
	GtkWidget *panel;
	
	panel = gtranslator_dict_panel_new();
	dictionary_box = glade_xml_get_widget(glade, GLADE_DICTIONARY_BOX);
	gtk_box_pack_start(GTK_BOX(dictionary_box), panel, TRUE, TRUE, 0);
	gtk_widget_show_all(panel);
}
