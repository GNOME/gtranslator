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
	GdictSourceLoader *loader;
	GError *gconf_error;
  GOptionContext *context;
  GOptionGroup *group;
  gchar *loader_path;
  gchar **lookup_words = NULL;
  gchar **match_words = NULL;
  gchar *database = NULL;
  gchar *strategy = NULL;
  gchar *source_name = NULL;
  gboolean no_window = FALSE;
  gboolean list_sources = FALSE;

  const GOptionEntry gdict_app_goptions[] =
  {
    { "look-up", 0, 0, G_OPTION_ARG_STRING_ARRAY, &lookup_words,
       N_("Words to look up"), N_("word") },
    { "match", 0, 0, G_OPTION_ARG_STRING_ARRAY, &match_words,
       N_("Words to match"), N_("word") },
    { "source", 's', 0, G_OPTION_ARG_STRING, &source_name,
       N_("Dictionary source to use"), N_("source") },
    { "list-sources", 'l', 0, G_OPTION_ARG_NONE, &list_sources,
       N_("Show available dictionary sources"), NULL },
    { "no-window", 'n', 0, G_OPTION_ARG_NONE, &no_window,
       N_("Print result to the console"), NULL },
    { "database", 'D', 0, G_OPTION_ARG_STRING, &database,
       N_("Database to use"), N_("db") },
    { "strategy", 'S', 0, G_OPTION_ARG_STRING, &strategy,
       N_("Strategy to use"), N_("strat") },
    { G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_STRING_ARRAY, &lookup_words,
       N_("Words to look up"), N_("word") },
    { NULL },
  };
	loader = gdict_source_loader_new ();
	/*loader_path = gdict_get_data_dir (); 
	gdict_source_loader_add_search_path (loader, loader_path);
	g_free (loader_path);*/
	
	
	
	panel = gtranslator_dict_panel_new(GDICT_WINDOW_ACTION_CLEAR,
      				 loader,
				 source_name,
				 NULL);
	dictionary_box = glade_xml_get_widget(glade, GLADE_DICTIONARY_BOX);
	gtk_box_pack_start(GTK_BOX(dictionary_box), panel, TRUE, TRUE, 0);
	gtk_widget_show_all(panel);
}
