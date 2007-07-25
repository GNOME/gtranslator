/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dict-panel.h"
#include "gdict-sidebar.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gdict/gdict.h>

#define GTR_DICT_PANEL_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),		       \
						 GTR_TYPE_DICT_PANEL,     \
						 GtranslatorDictPanelPrivate))

/* sidebar pages logical ids */
#define GDICT_SIDEBAR_SPELLER_PAGE      "speller"
#define GDICT_SIDEBAR_DATABASES_PAGE    "db-chooser"
#define GDICT_SIDEBAR_STRATEGIES_PAGE   "strat-chooser"

#define GDICT_DEFAULT_SOURCE_NAME       "Default"

enum
{
	COMPLETION_TEXT_COLUMN,

	COMPLETION_N_COLUMNS
};

enum
{
  PROP_0,

  PROP_ACTION,
  PROP_SOURCE_LOADER,
  PROP_SOURCE_NAME,
  PROP_PRINT_FONT,
  PROP_DEFBOX_FONT,
  PROP_WORD,
  PROP_WINDOW_ID
};

G_DEFINE_TYPE(GtranslatorDictPanel, gtranslator_dict_panel, GTK_TYPE_VBOX)

struct _GtranslatorDictPanelPrivate
{
	GtkWidget *button;
	GtkWidget *entry;
	GtkWidget *defbox;
	GtkWidget *strat_chooser;
	GtkWidget *speller;
	GtkWidget *db_chooser;
	GtkWidget *sidebar;
	
	gchar *source_name;
	GdictSourceLoader *loader;
	
	GdictContext *context;
	guint definition_id;
	guint lookup_start_id;
	guint lookup_end_id;
	guint error_id;
	
	GtkEntryCompletion *completion;
	GtkListStore *completion_model;
	
	gchar *word;
	gint max_definition;
	gint last_definition;
	gint current_definition;
	
	gchar *database;
	gchar *strategy;
};




static void
gdict_window_set_database (GtranslatorDictPanel *panel,
			   const gchar *database)
{
	g_free (panel->priv->database);

	if (database)
		panel->priv->database = g_strdup (database);
	/*else
		panel->priv->database = gdict_gconf_get_string_with_default (window->gconf_client,
							    GDICT_GCONF_DATABASE_KEY,
							    GDICT_DEFAULT_DATABASE);*/

	if (panel->priv->defbox)
	gdict_defbox_set_database (GDICT_DEFBOX (panel->priv->defbox),
				   panel->priv->database);
}

static void
gdict_window_set_strategy (GtranslatorDictPanel *panel,
			   const gchar *strategy)
{
	if (panel->priv->strategy)
		g_free (panel->priv->strategy);

	panel->priv->strategy = g_strdup (strategy);
}


static void
gdict_window_set_word (GtranslatorDictPanel *panel,
		       const gchar *word,
		       const gchar *database)
{
	gchar *title;
  
	g_free (panel->priv->word);
	panel->priv->word = NULL;

	if (word && word[0] != '\0')
		panel->priv->word = g_strdup (word);
	else
		return;

	if (!database || database[0] == '\0')
		database = panel->priv->database;

	/*if (panel->priv->word)
		title = g_strdup_printf (_("%s - Dictionary"), panel->priv->word);
	else
		title = g_strdup (_("Dictionary"));
  
	gtk_window_set_title (GTK_WINDOW (window), title);
	g_free (title);*/

	if (panel->priv->defbox)
	{
		gdict_defbox_set_database (GDICT_DEFBOX (panel->priv->defbox), database);
		gdict_defbox_lookup (GDICT_DEFBOX (panel->priv->defbox), word);
	}
}

static void
gdict_window_definition_cb (GdictContext    *context,
			    GdictDefinition *definition,
			    GtranslatorDictPanel *panel)
{
	gint total, n;
  /*gdouble fraction;

  g_assert (GDICT_IS_WINDOW (window));

  total = gdict_definition_get_total (definition);*/
	n = panel->priv->current_definition + 1;

  /*fraction = CLAMP (((gdouble) n / (gdouble) total), 0.0, 1.0);

  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (window->progress),
		  		 fraction);*/
	while (gtk_events_pending ())
		gtk_main_iteration ();

	panel->priv->current_definition = n;
}

static void
gdict_window_lookup_start_cb (GdictContext *context,
			      GtranslatorDictPanel *panel)
{
	gchar *message;

	if (!panel->priv->word)
		return;

	/*if (!window->busy_cursor)
		window->busy_cursor = gdk_cursor_new (GDK_WATCH);*/

	message = g_strdup_printf (_("Searching for '%s'..."), panel->priv->word);
  
	/*if (window->status)
		gtk_statusbar_push (GTK_STATUSBAR (window->status), 0, message);

  if (window->progress)
    gtk_widget_show (window->progress);*/

	panel->priv->max_definition = -1;
	panel->priv->last_definition = 0;
	panel->priv->current_definition = 0;

	/*gdk_window_set_cursor (GTK_WIDGET (window)->window,
		         window->busy_cursor);*/
	g_free (message);
}

static void
gdict_window_lookup_end_cb (GdictContext *context,
			    GtranslatorDictPanel *panel)
{
	gchar *message;
	gint count;
	GtkTreeIter iter;

	count = panel->priv->current_definition;

	panel->priv->max_definition = count - 1;

	if (count == 0)
		message = g_strdup (_("No definitions found"));
	else 
		message = g_strdup_printf (ngettext("A definition found",
						    "%d definitions found",
						    count),
					   count);

	/*if (window->status)
		gtk_statusbar_push (GTK_STATUSBAR (window->status), 0, message);

	if (window->progress)
		gtk_widget_hide (window->progress);*/

	gtk_list_store_append (panel->priv->completion_model, &iter);
	gtk_list_store_set (panel->priv->completion_model, &iter,
			    COMPLETION_TEXT_COLUMN, panel->priv->word,
			    -1);

  //gdk_window_set_cursor (GTK_WIDGET (window)->window, NULL);
	g_free (message);
}

static void
gdict_window_error_cb (GdictContext *context,
		       const GError *error,
		       GtranslatorDictPanel *panel)
{
	gint count;
  
	//gdk_window_set_cursor (GTK_WIDGET (window)->window, NULL);
  
	/*gtk_statusbar_push (GTK_STATUSBAR (window->status), 0,
		      _("No definitions found"));*/

	//gtk_widget_hide (window->progress);

	/* launch the speller only on NO_MATCH */
	if (error->code == GDICT_CONTEXT_ERROR_NO_MATCH)
	{
		GdictSource *source;
		GdictContext *context;

		//gdict_window_set_sidebar_visible (window, TRUE);
		gdict_sidebar_view_page (GDICT_SIDEBAR (panel->priv->sidebar),
					 GDICT_SIDEBAR_SPELLER_PAGE);

		/* we clone the context, so that the signals that it
		* fires do not get caught by the signal handlers we
		* use for getting the definitions.
		*/
		source = gdict_source_loader_get_source (panel->priv->loader,
							 panel->priv->source_name);
		context = gdict_source_get_context (source);

		gdict_speller_set_context (GDICT_SPELLER (panel->priv->speller),
					   context);
		g_object_unref (context);
		g_object_unref (source);
      
		gdict_speller_set_strategy (GDICT_SPELLER (panel->priv->speller),
					    panel->priv->strategy);
      
		gdict_speller_match (GDICT_SPELLER (panel->priv->speller),
				     panel->priv->word);
    }
}

static void
gdict_window_set_context (GtranslatorDictPanel *panel,
			  GdictContext *context)
{
	if (panel->priv->context)
	{
		g_signal_handler_disconnect (panel->priv->context, panel->priv->definition_id);
		g_signal_handler_disconnect (panel->priv->context, panel->priv->lookup_start_id);
		g_signal_handler_disconnect (panel->priv->context, panel->priv->lookup_end_id);
		g_signal_handler_disconnect (panel->priv->context, panel->priv->error_id);

		panel->priv->definition_id = 0;
		panel->priv->lookup_start_id = 0;
		panel->priv->lookup_end_id = 0;
		panel->priv->error_id = 0;
		
		g_object_unref (panel->priv->context);
		panel->priv->context = NULL;
	}

	if (panel->priv->defbox)
	{
		gdict_defbox_set_context (GDICT_DEFBOX (panel->priv->defbox), context);
	}

	if (!context)
		return;
  
	/* attach our callbacks */
	panel->priv->definition_id   = g_signal_connect (context, "definition-found",
							 G_CALLBACK (gdict_window_definition_cb),
							 panel);
	panel->priv->lookup_start_id = g_signal_connect (context, "lookup-start",
							 G_CALLBACK (gdict_window_lookup_start_cb),
							 panel);
	panel->priv->lookup_end_id   = g_signal_connect (context, "lookup-end",
		  			      G_CALLBACK (gdict_window_lookup_end_cb),
					      panel);
	panel->priv->error_id        = g_signal_connect (context, "error",
		  			      G_CALLBACK (gdict_window_error_cb),
					      panel);
  
	panel->priv->context = context;
}

static GdictContext *
get_context_from_loader (GtranslatorDictPanel *panel)
{
	GdictSource *source;
	GdictContext *retval;

	if (!panel->priv->source_name)
		panel->priv->source_name = g_strdup (GDICT_DEFAULT_SOURCE_NAME);

	source = gdict_source_loader_get_source (panel->priv->loader,
						 panel->priv->source_name);
	if (!source &&
		strcmp (panel->priv->source_name, GDICT_DEFAULT_SOURCE_NAME) != 0)
	{
		g_free (panel->priv->source_name);
		panel->priv->source_name = g_strdup (GDICT_DEFAULT_SOURCE_NAME);

		source = gdict_source_loader_get_source (panel->priv->loader,
							 panel->priv->source_name);
	}
  
	if (!source)
	{
		gchar *detail;
		
		detail = g_strdup_printf (_("No dictionary source available with name '%s'"),
					  panel->priv->source_name);

		/*gdict_show_error_dialog (GTK_WINDOW (window),
                               _("Unable to find dictionary source"),
                               NULL);*/
		g_warning(_("Unable to find dictionary source"));
      
		g_free (detail);

		return NULL;
	}
  
	gdict_window_set_database (panel, gdict_source_get_database (source));
	gdict_window_set_strategy (panel, gdict_source_get_strategy (source));
  
	retval = gdict_source_get_context (source);
	if (!retval)
	{
		gchar *detail;
      
		detail = g_strdup_printf (_("No context available for source '%s'"),
					  gdict_source_get_description (source));
      				
      /*gdict_show_error_dialog (GTK_WINDOW (window),
                               _("Unable to create a context"),
                               detail);*/
		g_warning(_("Unable to create a context"));
      
		g_free (detail);
		g_object_unref (source);
      
		return NULL;
	}
  
	g_object_unref (source);
  
	return retval;
}

static void
gdict_window_set_source_name (GtranslatorDictPanel *panel,
			      const gchar *source_name)
{
	GdictContext *context;

	g_free (panel->priv->source_name);

	if (source_name)
	{
		g_warning("hay sourcename");
		panel->priv->source_name = g_strdup (source_name);
	}
	/*else
	panel->priv->source_name = gdict_gconf_get_string_with_default (window->gconf_client,
							       GDICT_GCONF_SOURCE_KEY,
							       GDICT_DEFAULT_SOURCE_NAME);*/

	context = get_context_from_loader (panel);
	gdict_window_set_context (panel, context);
}


static void
gdict_window_set_property (GObject      *object,
			   guint         prop_id,
			   const GValue *value,
			   GParamSpec   *pspec)
{
  GtranslatorDictPanel *panel = GTR_DICT_PANEL (object);
  
  switch (prop_id)
    {
    /*case PROP_ACTION:
      window->action = g_value_get_enum (value);
      break;*/
    case PROP_SOURCE_LOADER:
      if (panel->priv->loader)
        g_object_unref (panel->priv->loader);
      panel->priv->loader = g_value_get_object (value);
      g_object_ref (panel->priv->loader);
      break;
    case PROP_SOURCE_NAME:
      gdict_window_set_source_name (panel, g_value_get_string (value));
	    g_warning("source-name");
      break;
    case PROP_WORD:
      gdict_window_set_word (panel, g_value_get_string (value), NULL);
      break;
    /*case PROP_PRINT_FONT:
      gdict_window_set_print_font (window, g_value_get_string (value));
      break;*/
    /*case PROP_DEFBOX_FONT:
      gdict_window_set_defbox_font (window, g_value_get_string (value));
      break;*/
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gdict_window_get_property (GObject    *object,
			   guint       prop_id,
			   GValue     *value,
			   GParamSpec *pspec)
{
  GtranslatorDictPanel *panel = GTR_DICT_PANEL(object);
  
  switch (prop_id)
    {
    /*case PROP_ACTION:
      g_value_set_enum (value, window->action);
      break;*/
    case PROP_SOURCE_LOADER:
      g_value_set_object (value, panel->priv->loader);
      break;
    case PROP_SOURCE_NAME:
      g_value_set_string (value, panel->priv->source_name);
      break;
    case PROP_WORD:
      g_value_set_string (value, panel->priv->word);
      break;
    /*case PROP_PRINT_FONT:
      g_value_set_string (value, window->print_font);
      break;
    case PROP_DEFBOX_FONT:
      g_value_set_string (value, window->defbox_font);
      break;
    case PROP_WINDOW_ID:
      g_value_set_uint (value, window->window_id);
      break;*/
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
	g_warning("entra en get");
}

static void
lookup_word (GtranslatorDictPanel *panel,
             gpointer dummy)
{
	const gchar *word;

	g_assert (GTR_IS_DICT_PANEL (panel));
  
	if (!panel->priv->context)
	{
		g_warning("There is no context\n");
		return;
	}
  
	word = gtk_entry_get_text (GTK_ENTRY (panel->priv->entry));
	if (!word || *word == '\0')
	{
		g_warning("There is no word\n");
		return;
	}

	gdict_window_set_word (panel, word, NULL);
}

static void
strategy_activated_cb (GdictStrategyChooser *chooser,
                       const gchar          *strat_name,
                       const gchar          *strat_desc,
                       GtranslatorDictPanel *panel)
{
	gdict_window_set_strategy (panel, strat_name);

  /*if (window->status)
    {
      gchar *message;

      message = g_strdup_printf (_("Strategy `%s' selected"), strat_desc);
      gtk_statusbar_push (GTK_STATUSBAR (window->status), 0, message);
      g_free (message);
    }*/
}

static void
database_activated_cb (GdictDatabaseChooser *chooser,
		       const gchar          *db_name,
		       const gchar          *db_desc,
		       GtranslatorDictPanel *panel)
{
	gdict_window_set_database (panel, db_name);

  /*if (window->status)
    {
      gchar *message;

      message = g_strdup_printf (_("Database `%s' selected"), db_desc);
      gtk_statusbar_push (GTK_STATUSBAR (window->status), 0, message);
      g_free (message);
    }*/
}

static void
speller_word_activated_cb (GdictSpeller *speller,
			   const gchar  *word,
			   const gchar  *db_name,
			   GtranslatorDictPanel *panel)
{
	gtk_entry_set_text (GTK_ENTRY (panel->priv->entry), word);
  
	gdict_window_set_word (panel, word, db_name);

/*  if (window->status)
    {
      gchar *message;

      message = g_strdup_printf (_("Word `%s' selected"), word);
      gtk_statusbar_push (GTK_STATUSBAR (window->status), 0, message);
      g_free (message);
    }*/
}

static void
sidebar_page_changed_cb (GdictSidebar *sidebar,
			 GtranslatorDictPanel *panel)
{
	const gchar *page_id;
	const gchar *message;

	page_id = gdict_sidebar_current_page (sidebar);

	switch (page_id[0])
	{
		case 's':
		{
			switch (page_id[1])
 			{
				case 'p': /* speller */
					message = _("Double-click on the word to look up");
					if (panel->priv->word)
						gdict_speller_match (GDICT_SPELLER (panel->priv->speller),
								     panel->priv->word);
				break;
				case 't': /* strat-chooser */
					message = _("Double-click on the matching strategy to use");

					gdict_strategy_chooser_refresh (GDICT_STRATEGY_CHOOSER (panel->priv->strat_chooser));
				break;
				default:
					message = NULL;
			}
		}
		break;
		case 'd': /* db-chooser */
			message = _("Double-click on the database to use");
			gdict_database_chooser_refresh (GDICT_DATABASE_CHOOSER (panel->priv->db_chooser));
		break;
		default:
			message = NULL;
		break;
	}

	/*if (message && window->status)
		gtk_statusbar_push (GTK_STATUSBAR (window->status), 0, message);*/
}

static void
gtranslator_dict_panel_init (GtranslatorDictPanel *panel)
{
	GtkPaned   *paned;
	GtkWidget  *vbox;
	GtkWidget  *hbox;
	
	panel->priv = GTR_DICT_PANEL_GET_PRIVATE (panel);
	
	vbox = gtk_vbox_new (FALSE, 6);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 6);
	gtk_widget_show (vbox);
 
	hbox = gtk_hbox_new (FALSE, 12);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show (hbox);
	
	/*
	 * Look up Button
	 */
	panel->priv->button = gtk_button_new_with_mnemonic (_("Look _up:"));
	g_signal_connect_swapped(panel->priv->button, "clicked",
				 G_CALLBACK (lookup_word),
				 panel);
	gtk_button_set_relief (GTK_BUTTON (panel->priv->button), GTK_RELIEF_NONE);
	gtk_box_pack_start (GTK_BOX (hbox), panel->priv->button, FALSE, FALSE, 0);
 	gtk_widget_show(panel->priv->button);
	
	/*
	 * Entry
	 */
	panel->priv->completion_model = gtk_list_store_new (COMPLETION_N_COLUMNS,
							    G_TYPE_STRING);
  
	panel->priv->completion = gtk_entry_completion_new ();
	gtk_entry_completion_set_popup_completion (panel->priv->completion, TRUE);
	gtk_entry_completion_set_model (panel->priv->completion,
					GTK_TREE_MODEL (panel->priv->completion_model));
	gtk_entry_completion_set_text_column (panel->priv->completion,
					      COMPLETION_TEXT_COLUMN);
  
	panel->priv->entry = gtk_entry_new ();
	if (panel->priv->word)
		gtk_entry_set_text (GTK_ENTRY (panel->priv->entry), panel->priv->word);
  
	gtk_entry_set_completion (GTK_ENTRY (panel->priv->entry),
				  panel->priv->completion);
	g_signal_connect_swapped (panel->priv->entry, "activate",
				  G_CALLBACK (lookup_word),
				  panel);
	gtk_box_pack_start (GTK_BOX (hbox), panel->priv->entry, TRUE, TRUE, 0);
	gtk_widget_show (panel->priv->entry);


	/*
	 * Defbox
	 */
	panel->priv->defbox = gdict_defbox_new ();
	if (panel->priv->context)
		gdict_defbox_set_context (GDICT_DEFBOX (panel->priv->defbox),
					  panel->priv->context);
	
	gtk_container_add (GTK_CONTAINER (vbox), panel->priv->defbox);
	gtk_widget_show (panel->priv->defbox);
	
	panel->priv->sidebar = gdict_sidebar_new ();
	g_signal_connect (panel->priv->sidebar, "page-changed",
			  G_CALLBACK (sidebar_page_changed_cb),
			  panel);

	/*
	 * Paned
	 */
	paned = GTK_PANED(gtk_vpaned_new());
	gtk_box_pack_start (GTK_BOX (panel), GTK_WIDGET(paned), TRUE, TRUE, 0);
	gtk_paned_pack1 (paned, vbox, FALSE, TRUE);
	gtk_paned_pack2 (paned, panel->priv->sidebar, TRUE, TRUE);
	gtk_widget_show (GTK_WIDGET(paned));
	
	/*
	 * Speller
	 */
	panel->priv->speller = gdict_speller_new ();
	if (panel->priv->context)
		gdict_speller_set_context (GDICT_SPELLER (panel->priv->speller),
					   panel->priv->context);
	g_signal_connect (panel->priv->speller, "word-activated",
			  G_CALLBACK (speller_word_activated_cb),
			  panel);
  
	gdict_sidebar_add_page (GDICT_SIDEBAR (panel->priv->sidebar),
				GDICT_SIDEBAR_SPELLER_PAGE,
				_("Similar words"),
				panel->priv->speller);
	gtk_widget_show (panel->priv->speller);

	/*
	 * db chooser
	 */
	panel->priv->db_chooser = gdict_database_chooser_new ();
	if (panel->priv->context)
		gdict_database_chooser_set_context (GDICT_DATABASE_CHOOSER (panel->priv->db_chooser),
						    panel->priv->context);
	g_signal_connect (panel->priv->db_chooser, "database-activated",
			  G_CALLBACK (database_activated_cb),
			  panel);
	gdict_sidebar_add_page (GDICT_SIDEBAR (panel->priv->sidebar),
				GDICT_SIDEBAR_DATABASES_PAGE,
				_("Available dictionaries"),
				panel->priv->db_chooser);
	gtk_widget_show (panel->priv->db_chooser);

	/*
	 * Strategy chooser
	 */
	panel->priv->strat_chooser = gdict_strategy_chooser_new ();
	if (panel->priv->context)
		gdict_strategy_chooser_set_context (GDICT_STRATEGY_CHOOSER (panel->priv->strat_chooser),
						    panel->priv->context);
	g_signal_connect (panel->priv->strat_chooser, "strategy-activated",
			  G_CALLBACK (strategy_activated_cb),
			  panel);
	gdict_sidebar_add_page (GDICT_SIDEBAR (panel->priv->sidebar),
				GDICT_SIDEBAR_STRATEGIES_PAGE,
				_("Available strategies"),
				panel->priv->strat_chooser);
	gtk_widget_show (panel->priv->strat_chooser);

	gtk_widget_show (panel->priv->sidebar);
	
}

static void
gtranslator_dict_panel_finalize (GObject *object)
{
	GtranslatorDictPanel *panel = GTR_DICT_PANEL (object);

	g_free (panel->priv->source_name);
	/*g_free (panel->priv->print_font);
	g_free (panel->priv->defbox_font);*/
	g_free (panel->priv->word);
	g_free (panel->priv->database);
	g_free (panel->priv->strategy);
	
	G_OBJECT_CLASS (gtranslator_dict_panel_parent_class)->finalize (object);
}

static void
gtranslator_dict_panel_dispose (GObject *gobject)
{
	GtranslatorDictPanel *panel = GTR_DICT_PANEL (gobject);
  
	if (panel->priv->context)
	{
		if (panel->priv->lookup_start_id)
		{
			g_signal_handler_disconnect (panel->priv->context,
						     panel->priv->lookup_start_id);
			g_signal_handler_disconnect (panel->priv->context,
						     panel->priv->definition_id);
			g_signal_handler_disconnect (panel->priv->context,
						     panel->priv->lookup_end_id);
			g_signal_handler_disconnect (panel->priv->context,
						     panel->priv->error_id);
			
			panel->priv->lookup_start_id = 0;
			panel->priv->definition_id = 0;
			panel->priv->lookup_end_id = 0;
			panel->priv->error_id = 0;
		}
		
		g_object_unref (panel->priv->context);
		panel->priv->context = NULL;
	}

	if (panel->priv->loader)
	{
		g_object_unref (panel->priv->loader);
		panel->priv->loader = NULL;
	}

	if (panel->priv->completion)
	{
		g_object_unref (panel->priv->completion);
		panel->priv->completion = NULL;
	}

	if (panel->priv->completion_model)
	{
		g_object_unref (panel->priv->completion_model);
		panel->priv->completion_model = NULL;
	}


	G_OBJECT_CLASS (gtranslator_dict_panel_parent_class)->dispose (gobject);
}

static void
gtranslator_dict_panel_class_init (GtranslatorDictPanelClass *klass)
{
	/*GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorDictPanelPrivate));

	object_class->finalize = gtranslator_dict_panel_finalize;*/
	
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	
	g_type_class_add_private (klass, sizeof (GtranslatorDictPanelPrivate));
	
	gobject_class->finalize = gtranslator_dict_panel_finalize;
	gobject_class->dispose = gtranslator_dict_panel_dispose;
	gobject_class->set_property = gdict_window_set_property;
	gobject_class->get_property = gdict_window_get_property;
	//gobject_class->constructor = gtranslator_dict_panel_constructor;

	/*widget_class->style_set = gdict_window_style_set;
	widget_class->size_allocate = gdict_window_size_allocate;*/
  
	/*g_object_class_install_property (gobject_class,
  				   PROP_ACTION,
				   g_param_spec_enum ("action",
				   		      "Action",
						      "The default action performed by the window",
						      GDICT_TYPE_WINDOW_ACTION,
						      GDICT_WINDOW_ACTION_CLEAR,
						      (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY)));*/
	g_object_class_install_property (gobject_class,
  				   PROP_SOURCE_LOADER,
  				   g_param_spec_object ("source-loader",
  							"Source Loader",
  							"The GdictSourceLoader to be used to load dictionary sources",
  							GDICT_TYPE_SOURCE_LOADER,
  							(G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY)));
	g_object_class_install_property (gobject_class,
		  		   PROP_SOURCE_NAME,
				   g_param_spec_string ("source-name",
					   		"Source Name",
							"The name of the GdictSource to be used",
							GDICT_DEFAULT_SOURCE_NAME,
							(G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT)));
	/*g_object_class_install_property (gobject_class,
  				   PROP_PRINT_FONT,
  				   g_param_spec_string ("print-font",
  				   			"Print Font",
  				   			"The font name to be used when printing",
  				   			GDICT_DEFAULT_PRINT_FONT,
  				   			(G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT)));*/
	/*g_object_class_install_property (gobject_class,
		  		   PROP_DEFBOX_FONT,
				   g_param_spec_string ("defbox-font",
					   		"Defbox Font",
							"The font name to be used by the defbox widget",
							GDICT_DEFAULT_DEFBOX_FONT,
							(G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT)));*/
	g_object_class_install_property (gobject_class,
		  		   PROP_WORD,
				   g_param_spec_string ("word",
					   		"Word",
							"The word to search",
							NULL,
							(G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT)));
	/*g_object_class_install_property (gobject_class,
  				   PROP_WINDOW_ID,
  				   g_param_spec_uint ("window-id",
  				   		      "Window ID",
  				   		      "The unique identifier for this window",
  				   		      0,
  				   		      G_MAXUINT,
  				   		      0,
  				   		      G_PARAM_READABLE));*/
}

GtkWidget *
gtranslator_dict_panel_new (GdictWindowAction  action,
			    GdictSourceLoader *loader,
			    const gchar       *source_name,
			    const gchar       *word)
{
	return GTK_WIDGET (g_object_new (GTR_TYPE_DICT_PANEL,
					 "action", action,
					 "source-loader", loader,
					 "source-name", source_name,
					 NULL));
}


