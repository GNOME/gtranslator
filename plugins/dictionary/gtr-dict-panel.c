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

#include "gtr-dict-panel.h"
#include "gtr-gdict-sidebar.h"
#include "gtr-plugin.h"
#include "gtr-window.h"
#include "gtr-statusbar.h"
#include "gtr-dirs.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gdict/gdict.h>
#include <string.h>

#define GTR_DICT_PANEL_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),		       \
						 GTR_TYPE_DICT_PANEL,     \
						 GtrDictPanelPrivate))

#define DICTIONARY_DEFAULT_SOURCE_NAME "Default"

#define DICTIONARY_SETTINGS_DATABASE_KEY         "database"
#define DICTIONARY_SETTINGS_STRATEGY_KEY         "strategy"
#define DICTIONARY_SETTINGS_SOURCE_KEY           "source-name"
#define DICTIONARY_SETTINGS_POSITION_KEY         "panel-position"

/* sidebar pages logical ids */
#define GDICT_SIDEBAR_SPELLER_PAGE      "speller"
#define GDICT_SIDEBAR_DATABASES_PAGE    "db-chooser"
#define GDICT_SIDEBAR_STRATEGIES_PAGE   "strat-chooser"
#define GDICT_SIDEBAR_SOURCES_PAGE      "source-chooser"

GTR_PLUGIN_DEFINE_TYPE (GtrDictPanel, gtr_dict_panel, GTK_TYPE_VBOX)

struct _GtrDictPanelPrivate
{
  GSettings *settings;

  GtkPaned *paned;
  GtrStatusbar *status;

  gchar *database;
  gchar *strategy;
  gchar *source_name;

  gchar *word;
  GdictContext *context;

  GdictSourceLoader *loader;

  GtkWidget *speller;
  GtkWidget *db_chooser;
  GtkWidget *strat_chooser;
  GtkWidget *source_chooser;
  GtkWidget *entry;
  GtkWidget *button;
  GtkWidget *defbox;
  GtkWidget *sidebar;
};

static void
gtr_dict_panel_create_warning_dialog (const gchar * primary,
                                      const gchar * secondary)
{
  GtkWidget *dialog;

  if (!primary)
    return;

  dialog = gtk_message_dialog_new (NULL,
                                   GTK_DIALOG_DESTROY_WITH_PARENT,
                                   GTK_MESSAGE_WARNING,
                                   GTK_BUTTONS_CLOSE, "%s", primary);

  if (secondary)
    gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
                                              "%s", secondary);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

static void
gtr_dict_panel_entry_activate_cb (GtkWidget * widget, GtrDictPanel * panel)
{
  GtrDictPanelPrivate *priv = panel->priv;
  const gchar *text;

  text = gtk_entry_get_text (GTK_ENTRY (priv->entry));

  if (!text)
    return;

  g_free (priv->word);
  priv->word = g_strdup (text);

  gdict_defbox_lookup (GDICT_DEFBOX (priv->defbox), priv->word);
}

static void
gtr_dict_panel_set_database (GtrDictPanel * panel, const gchar * database)
{
  GtrDictPanelPrivate *priv = panel->priv;

  g_free (priv->database);

  if (database)
    priv->database = g_strdup (database);
  else
    priv->database = g_settings_get_string (priv->settings,
                                            DICTIONARY_SETTINGS_DATABASE_KEY);

  if (priv->defbox)
    gdict_defbox_set_database (GDICT_DEFBOX (priv->defbox), priv->database);
}

static void
gtr_dict_panel_set_strategy (GtrDictPanel * panel, const gchar * strategy)
{
  GtrDictPanelPrivate *priv = panel->priv;

  g_free (priv->strategy);

  if (strategy)
    priv->strategy = g_strdup (strategy);
  else
    priv->strategy = g_settings_get_string (priv->settings,
                                            DICTIONARY_SETTINGS_STRATEGY_KEY);
}

static GdictContext *
get_context_from_loader (GtrDictPanel * panel)
{
  GtrDictPanelPrivate *priv = panel->priv;
  GdictSource *source;
  GdictContext *retval;

  if (!priv->source_name)
    priv->source_name = g_strdup (DICTIONARY_DEFAULT_SOURCE_NAME);

  source = gdict_source_loader_get_source (priv->loader, priv->source_name);
  if (!source)
    {
      gchar *detail;

      detail =
        g_strdup_printf (_("No dictionary source available with name '%s'"),
                         priv->source_name);

      gtr_dict_panel_create_warning_dialog (_
                                            ("Unable to find dictionary source"),
                                            detail);
      g_free (detail);

      return NULL;
    }

  gtr_dict_panel_set_database (panel, gdict_source_get_database (source));
  gtr_dict_panel_set_strategy (panel, gdict_source_get_strategy (source));

  retval = gdict_source_get_context (source);
  if (!retval)
    {
      gchar *detail;

      detail = g_strdup_printf (_("No context available for source '%s'"),
                                gdict_source_get_description (source));

      gtr_dict_panel_create_warning_dialog (_
                                            ("Unable to create a context"),
                                            detail);

      g_free (detail);
      g_object_unref (source);

      return NULL;
    }

  g_object_unref (source);

  return retval;
}

static void
gtr_dict_panel_set_context (GtrDictPanel * panel, GdictContext * context)
{
  GtrDictPanelPrivate *priv = panel->priv;

  if (priv->context)
    {
      g_object_unref (priv->context);
      priv->context = NULL;
    }

  if (priv->defbox)
    gdict_defbox_set_context (GDICT_DEFBOX (priv->defbox), context);

  if (priv->db_chooser)
    gdict_database_chooser_set_context (GDICT_DATABASE_CHOOSER
                                        (priv->db_chooser), context);

  if (priv->strat_chooser)
    gdict_strategy_chooser_set_context (GDICT_STRATEGY_CHOOSER
                                        (priv->strat_chooser), context);

  if (!context)
    return;

  priv->context = context;
}

static void
gtr_dict_panel_set_source_name (GtrDictPanel * panel,
                                const gchar * source_name)
{
  GtrDictPanelPrivate *priv = panel->priv;
  GdictContext *context;

  if (priv->source_name && source_name &&
      strcmp (priv->source_name, source_name) == 0)
    return;

  g_free (priv->source_name);

  if (source_name)
    priv->source_name = g_strdup (source_name);
  else
    priv->source_name = g_settings_get_string (priv->settings,
                                               DICTIONARY_SETTINGS_SOURCE_KEY);

  context = get_context_from_loader (panel);
  gtr_dict_panel_set_context (panel, context);

  if (priv->source_chooser)
    gdict_source_chooser_set_current_source (GDICT_SOURCE_CHOOSER
                                             (priv->source_chooser),
                                             priv->source_name);
}

static void
source_activated_cb (GdictSourceChooser * chooser,
                     const gchar * source_name,
                     GdictSource * source, GtrDictPanel * panel)
{
  g_signal_handlers_block_by_func (chooser, source_activated_cb, panel);
  gtr_dict_panel_set_source_name (panel, source_name);
  g_signal_handlers_unblock_by_func (chooser, source_activated_cb, panel);

  if (panel->priv->status)
    {
      gchar *message;

      message = g_strdup_printf (_("Dictionary source '%s' selected"),
                                 gdict_source_get_description (source));
      gtr_statusbar_flash_message (panel->priv->status, 0, "%s", message);
      g_free (message);
    }
}

static void
strategy_activated_cb (GdictStrategyChooser * chooser,
                       const gchar * strat_name,
                       const gchar * strat_desc, GtrDictPanel * panel)
{
  GtrDictPanelPrivate *priv = panel->priv;
  gtr_dict_panel_set_strategy (panel, strat_name);

  if (priv->status)
    {
      gchar *message;

      message = g_strdup_printf (_("Strategy '%s' selected"), strat_desc);
      gtr_statusbar_flash_message (panel->priv->status, 0, "%s", message);
      g_free (message);
    }
}

static void
database_activated_cb (GdictDatabaseChooser * chooser,
                       const gchar * db_name,
                       const gchar * db_desc, GtrDictPanel * panel)
{
  GtrDictPanelPrivate *priv = panel->priv;
  gtr_dict_panel_set_database (panel, db_name);

  if (priv->status)
    {
      gchar *message;

      message = g_strdup_printf (_("Database '%s' selected"), db_desc);
      gtr_statusbar_flash_message (panel->priv->status, 0, "%s", message);
      g_free (message);
    }
}

static void
gtr_dict_panel_set_word (GtrDictPanel * panel,
                         const gchar * word, const gchar * database)
{
  GtrDictPanelPrivate *priv = panel->priv;

  g_free (priv->word);
  priv->word = NULL;

  if (word && word[0] != '\0')
    priv->word = g_strdup (word);
  else
    return;

  if (!database || database[0] == '\0')
    database = priv->database;

  if (priv->defbox)
    {
      gdict_defbox_set_database (GDICT_DEFBOX (priv->defbox), database);
      gdict_defbox_lookup (GDICT_DEFBOX (priv->defbox), word);
    }
}

static void
speller_word_activated_cb (GdictSpeller * speller,
                           const gchar * word,
                           const gchar * db_name, GtrDictPanel * panel)
{
  GtrDictPanelPrivate *priv = panel->priv;
  gtk_entry_set_text (GTK_ENTRY (priv->entry), word);

  gtr_dict_panel_set_word (panel, word, db_name);

  if (priv->status)
    {
      gchar *message;

      message = g_strdup_printf (_("Word '%s' selected"), word);
      gtr_statusbar_flash_message (panel->priv->status, 0, "%s", message);
      g_free (message);
    }
}

static void
sidebar_page_changed_cb (GdictSidebar * sidebar, GtrDictPanel * panel)
{
  GtrDictPanelPrivate *priv = panel->priv;
  const gchar *page_id;
  const gchar *message;

  page_id = gdict_sidebar_current_page (sidebar);

  switch (page_id[0])
    {
    case 's':
      {
        switch (page_id[1])
          {
          case 'p':            /* speller */
            message = _("Double-click on the word to look up");
            if (priv->word)
              gdict_speller_match (GDICT_SPELLER (priv->speller), priv->word);
            break;
          case 't':            /* strat-chooser */
            message = _("Double-click on the matching strategy to use");

            gdict_strategy_chooser_refresh (GDICT_STRATEGY_CHOOSER
                                            (priv->strat_chooser));
            break;
          case 'o':            /* source-chooser */
            message = _("Double-click on the source to use");
            gdict_source_chooser_refresh (GDICT_SOURCE_CHOOSER
                                          (priv->source_chooser));
            break;
          default:
            message = NULL;
          }
      }
      break;
    case 'd':                  /* db-chooser */
      message = _("Double-click on the database to use");

      gdict_database_chooser_refresh (GDICT_DATABASE_CHOOSER
                                      (priv->db_chooser));
      break;
    default:
      message = NULL;
      break;
    }

  if (message && priv->status)
    gtr_statusbar_flash_message (panel->priv->status, 0, "%s", message);
}

static void
gtr_dict_panel_link_clicked (GtkWidget * defbox,
                             const gchar * link_text, GtrDictPanel * panel)
{
  if (!link_text)
    return;

  g_free (panel->priv->word);
  panel->priv->word = g_strdup (link_text);

  gtk_entry_set_text (GTK_ENTRY (panel->priv->entry), link_text);

  gdict_defbox_lookup (GDICT_DEFBOX (defbox), panel->priv->word);
}

static void
gtr_dict_panel_draw (GtrDictPanel * panel)
{
  GtkWidget *vbox;
  GtkWidget *hbox;

  vbox = gtk_vbox_new (FALSE, 6);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 6);
  gtk_widget_show (vbox);

  hbox = gtk_hbox_new (FALSE, 12);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  gtk_widget_show (hbox);

  /* Look up Button */
  panel->priv->button = gtk_button_new_with_mnemonic (_("Look _up:"));
  g_signal_connect (panel->priv->button, "clicked",
                    G_CALLBACK (gtr_dict_panel_entry_activate_cb), panel);
  gtk_button_set_relief (GTK_BUTTON (panel->priv->button), GTK_RELIEF_NONE);
  gtk_box_pack_start (GTK_BOX (hbox), panel->priv->button, FALSE, FALSE, 0);
  gtk_widget_show (panel->priv->button);

  /* Entry */
  panel->priv->entry = gtk_entry_new ();
  if (panel->priv->word)
    gtk_entry_set_text (GTK_ENTRY (panel->priv->entry), panel->priv->word);

  g_signal_connect (panel->priv->entry, "activate",
                    G_CALLBACK (gtr_dict_panel_entry_activate_cb), panel);
  gtk_box_pack_start (GTK_BOX (hbox), panel->priv->entry, TRUE, TRUE, 0);
  gtk_widget_show (panel->priv->entry);

  /* Defbox */
  panel->priv->defbox = gdict_defbox_new ();
  if (panel->priv->context)
    gdict_defbox_set_context (GDICT_DEFBOX (panel->priv->defbox),
                              panel->priv->context);
  g_signal_connect (panel->priv->defbox, "link-clicked",
                    G_CALLBACK (gtr_dict_panel_link_clicked), panel);

  gtk_container_add (GTK_CONTAINER (vbox), panel->priv->defbox);
  gtk_widget_show (panel->priv->defbox);

  /* Sidebar */
  panel->priv->sidebar = gdict_sidebar_new ();
  g_signal_connect (panel->priv->sidebar, "page-changed",
                    G_CALLBACK (sidebar_page_changed_cb), panel);

  /* Paned */
  panel->priv->paned = GTK_PANED (gtk_paned_new (GTK_ORIENTATION_VERTICAL));
  gtk_box_pack_start (GTK_BOX (panel), GTK_WIDGET (panel->priv->paned), TRUE,
                      TRUE, 0);
  gtk_paned_pack1 (panel->priv->paned, vbox, FALSE, TRUE);
  gtk_paned_pack2 (panel->priv->paned, panel->priv->sidebar, TRUE, TRUE);
  gtk_widget_show (GTK_WIDGET (panel->priv->paned));

  g_settings_bind (panel->priv->settings,
                   DICTIONARY_SETTINGS_POSITION_KEY,
                   panel->priv->paned,
                   "position",
                   G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);

  /* Speller */
  panel->priv->speller = gdict_speller_new ();
  if (panel->priv->context)
    gdict_speller_set_context (GDICT_SPELLER (panel->priv->speller),
                               panel->priv->context);
  g_signal_connect (panel->priv->speller, "word-activated",
                    G_CALLBACK (speller_word_activated_cb), panel);

  gdict_sidebar_add_page (GDICT_SIDEBAR (panel->priv->sidebar),
                          GDICT_SIDEBAR_SPELLER_PAGE,
                          _("Similar words"), panel->priv->speller);
  gtk_widget_show (panel->priv->speller);

  /* db chooser */
  panel->priv->db_chooser = gdict_database_chooser_new ();
  if (panel->priv->context)
    gdict_database_chooser_set_context (GDICT_DATABASE_CHOOSER
                                        (panel->priv->db_chooser),
                                        panel->priv->context);
  g_signal_connect (panel->priv->db_chooser, "database-activated",
                    G_CALLBACK (database_activated_cb), panel);
  gdict_sidebar_add_page (GDICT_SIDEBAR (panel->priv->sidebar),
                          GDICT_SIDEBAR_DATABASES_PAGE,
                          _("Available dictionaries"),
                          panel->priv->db_chooser);
  gtk_widget_show (panel->priv->db_chooser);

  /* Strategy chooser */
  panel->priv->strat_chooser = gdict_strategy_chooser_new ();
  if (panel->priv->context)
    gdict_strategy_chooser_set_context (GDICT_STRATEGY_CHOOSER
                                        (panel->priv->strat_chooser),
                                        panel->priv->context);
  g_signal_connect (panel->priv->strat_chooser, "strategy-activated",
                    G_CALLBACK (strategy_activated_cb), panel);
  gdict_sidebar_add_page (GDICT_SIDEBAR (panel->priv->sidebar),
                          GDICT_SIDEBAR_STRATEGIES_PAGE,
                          _("Available strategies"),
                          panel->priv->strat_chooser);
  gtk_widget_show (panel->priv->strat_chooser);

  /* Source chooser */
  panel->priv->source_chooser =
    gdict_source_chooser_new_with_loader (panel->priv->loader);
  g_signal_connect (panel->priv->source_chooser, "source-activated",
                    G_CALLBACK (source_activated_cb), panel);
  gdict_sidebar_add_page (GDICT_SIDEBAR (panel->priv->sidebar),
                          GDICT_SIDEBAR_SOURCES_PAGE, _("Dictionary sources"),
                          panel->priv->source_chooser);
  gtk_widget_show (panel->priv->source_chooser);

  gtk_widget_show (panel->priv->sidebar);
}

static void
on_settings_changed (GSettings    *settings,
                     const gchar  *key,
                     GtrDictPanel *panel)
{
  if (strcmp (key, DICTIONARY_SETTINGS_SOURCE_KEY) == 0)
    {
      gchar *source_name;

      source_name = g_settings_get_string (settings, key);
      gtr_dict_panel_set_source_name (panel, source_name);

      g_free (source_name);
    }
  else if (strcmp (key, DICTIONARY_SETTINGS_DATABASE_KEY) == 0)
    {
      gchar *database;

      database = g_settings_get_string (settings, key);
      gtr_dict_panel_set_database (panel, database);

      g_free (database);
    }
  else if (strcmp (key, DICTIONARY_SETTINGS_STRATEGY_KEY) == 0)
    {
      gchar *strategy;

      strategy = g_settings_get_string (settings, key);
      gtr_dict_panel_set_strategy (panel, strategy);

      g_free (strategy);
    }
}

static void
gtr_dict_panel_init (GtrDictPanel * panel)
{
  gchar *data_dir;
  GtrDictPanelPrivate *priv;

  panel->priv = GTR_DICT_PANEL_GET_PRIVATE (panel);
  priv = panel->priv;

  priv->status = NULL;

  if (!priv->loader)
    panel->priv->loader = gdict_source_loader_new ();

  /* add our data dir inside $HOME to the loader's search paths */
  data_dir = gtr_dirs_get_user_config_dir ();
  gdict_source_loader_add_search_path (priv->loader, data_dir);
  g_free (data_dir);

  /* settings */
  priv->settings = g_settings_new ("org.gnome.gtranslator.plugins.dictionary");

  g_signal_connect (priv->settings,
                    "changed",
                    G_CALLBACK (on_settings_changed),
                    panel);

  /* force retrieval of the configuration from gsettings */
  gtr_dict_panel_set_source_name (panel, NULL);

  /* Draw widgets */
  gtr_dict_panel_draw (panel);
}

static void
gtr_dict_panel_finalize (GObject * object)
{
  GtrDictPanel *panel = GTR_DICT_PANEL (object);

  g_free (panel->priv->source_name);
  g_free (panel->priv->word);
  g_free (panel->priv->database);
  g_free (panel->priv->strategy);

  G_OBJECT_CLASS (gtr_dict_panel_parent_class)->finalize (object);
}

static void
gtr_dict_panel_dispose (GObject * object)
{
  GtrDictPanel *panel = GTR_DICT_PANEL (object);

  if (panel->priv->settings != NULL)
    {
      g_object_unref (panel->priv->settings);
      panel->priv->settings = NULL;
    }

  G_OBJECT_CLASS (gtr_dict_panel_parent_class)->dispose (object);
}

static void
gtr_dict_panel_class_init (GtrDictPanelClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrDictPanelPrivate));

  gobject_class->finalize = gtr_dict_panel_finalize;
  gobject_class->dispose = gtr_dict_panel_dispose;
}

GtkWidget *
gtr_dict_panel_new (GtrWindow * window)
{
  GtrDictPanel *panel;

  panel = g_object_new (GTR_TYPE_DICT_PANEL, NULL);
  panel->priv->status = GTR_STATUSBAR (gtr_window_get_statusbar (window));

  return GTK_WIDGET (panel);
}
