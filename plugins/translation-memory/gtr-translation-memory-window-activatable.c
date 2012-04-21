/*
 * Copyright (C) 2012 Ignacio Casal Quinteiro <icq@gnome.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-translation-memory-window-activatable.h"
#include "gtr-translation-memory-dialog.h"
#include "gtr-window.h"
#include "gtr-window-activatable.h"
#include "gtr-translation-memory.h"
#include "gda/gtr-gda.h"

#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <string.h>

struct _GtrTranslationMemoryWindowActivatablePrivate
{
  GtrWindow *window;
  GSettings *tm_settings;
  GtrTranslationMemory *translation_memory;

  GtkActionGroup *action_group;
  guint           ui_id;
};

enum
{
  PROP_0,
  PROP_WINDOW
};

static void gtr_window_activatable_iface_init (GtrWindowActivatableInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED (GtrTranslationMemoryWindowActivatable,
                                gtr_translation_memory_window_activatable,
                                PEAS_TYPE_EXTENSION_BASE,
                                0,
                                G_IMPLEMENT_INTERFACE_DYNAMIC (GTR_TYPE_WINDOW_ACTIVATABLE,
                                                               gtr_window_activatable_iface_init))

static void
on_import_translation_memory_activated (GtkToggleAction                       *action,
                                        GtrTranslationMemoryWindowActivatable *window_activatable)
{
  static GtkWidget *dlg = NULL;

  if (dlg == NULL)
    {
      dlg = gtr_translation_memory_dialog_new (window_activatable->priv->translation_memory);
      g_signal_connect (dlg, "destroy",
                        G_CALLBACK (gtk_widget_destroyed), &dlg);
      gtk_widget_show_all (dlg);
    }

  if (GTK_WINDOW (window_activatable->priv->window) != gtk_window_get_transient_for (GTK_WINDOW (dlg)))
    {
      gtk_window_set_transient_for (GTK_WINDOW (dlg), GTK_WINDOW (window_activatable->priv->window));
    }

  gtk_window_present (GTK_WINDOW (dlg));
}

static const GtkActionEntry action_entries[] = {
  {"TranslationMemoryManager", NULL, N_("_Build Translation Memory"), NULL,
   N_("Build translation memory"),
   G_CALLBACK (on_import_translation_memory_activated)},
  {"EditTranslationMemory", NULL, N_("_Translation Memory"), NULL, NULL, NULL}
};

static const gchar ui_data[] =
"<ui>"
  "<menubar name=\"MainMenu\">"
    "<menu name=\"EditMenu\" action=\"Edit\">"
      "<placeholder name=\"EditOps_1\">"
        "<menu name=\"EditTranslationMemoryMenu\" action=\"EditTranslationMemory\"/>"
      "</placeholder>"
    "</menu>"
    "<menu name=\"ToolsMenu\" action=\"Tools\">"
      "<placeholder name=\"ToolsOps_1\">"
        "<menuitem action=\"TranslationMemoryManager\"/>"
      "</placeholder>"
    "</menu>"
  "</menubar>"
"</ui>";

static void
gtr_translation_memory_window_activatable_init (GtrTranslationMemoryWindowActivatable *window_activatable)
{
  GtrTranslationMemoryWindowActivatablePrivate *priv;

  window_activatable->priv = G_TYPE_INSTANCE_GET_PRIVATE (window_activatable,
                                                          GTR_TYPE_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE,
                                                          GtrTranslationMemoryWindowActivatablePrivate);

  priv = window_activatable->priv;

  priv->tm_settings = g_settings_new ("org.gnome.gtranslator.plugins.translation-memory");

  /* Creating translation memory */
  priv->translation_memory = GTR_TRANSLATION_MEMORY (gtr_gda_new ());

  gtr_translation_memory_set_max_omits (priv->translation_memory,
                                        g_settings_get_int (priv->tm_settings,
                                                            "max-missing-words"));
  gtr_translation_memory_set_max_delta (priv->translation_memory,
                                        g_settings_get_int (priv->tm_settings,
                                                            "max-length-diff"));
  gtr_translation_memory_set_max_items (priv->translation_memory, 10);
}

static void
gtr_translation_memory_window_activatable_dispose (GObject *object)
{
  GtrTranslationMemoryWindowActivatablePrivate *priv = GTR_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE (object)->priv;

  g_clear_object (&priv->window);
  g_clear_object (&priv->action_group);
  g_clear_object (&priv->translation_memory);
  g_clear_object (&priv->tm_settings);

  G_OBJECT_CLASS (gtr_translation_memory_window_activatable_parent_class)->dispose (object);
}

static void
gtr_translation_memory_window_activatable_set_property (GObject      *object,
                                    guint         prop_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
  GtrTranslationMemoryWindowActivatablePrivate *priv = GTR_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE (object)->priv;

  switch (prop_id)
    {
      case PROP_WINDOW:
        priv->window = GTR_WINDOW (g_value_dup_object (value));
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
gtr_translation_memory_window_activatable_get_property (GObject    *object,
                                    guint       prop_id,
                                    GValue     *value,
                                    GParamSpec *pspec)
{
  GtrTranslationMemoryWindowActivatablePrivate *priv = GTR_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE (object)->priv;

  switch (prop_id)
    {
      case PROP_WINDOW:
        g_value_set_object (value, priv->window);
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
gtr_translation_memory_window_activatable_activate (GtrWindowActivatable *activatable)
{
  GtrTranslationMemoryWindowActivatablePrivate *priv = GTR_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE (activatable)->priv;
  GtkUIManager *manager;
  GError *error = NULL;

  g_object_set_data (G_OBJECT (priv->window), "GtrTranslationMemory", priv->translation_memory);

  manager = gtr_window_get_ui_manager (priv->window);

  priv->action_group = gtk_action_group_new ("GtrTranslationMemoryWindowActivatableActions");
  gtk_action_group_set_translation_domain (priv->action_group,
                                           GETTEXT_PACKAGE);
  gtk_action_group_add_actions (priv->action_group, action_entries,
                                G_N_ELEMENTS (action_entries),
                                activatable);

  gtk_ui_manager_insert_action_group (manager, priv->action_group, -1);

  priv->ui_id = gtk_ui_manager_add_ui_from_string (manager,
                                                   ui_data,
                                                   -1,
                                                   &error);

  if (error != NULL)
    {
      g_message ("%s", error->message);
      g_error_free (error);
    }
}

static void
gtr_translation_memory_window_activatable_deactivate (GtrWindowActivatable *activatable)
{
  GtrTranslationMemoryWindowActivatablePrivate *priv = GTR_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE (activatable)->priv;
  GtkUIManager *manager;

  g_object_set_data (G_OBJECT (priv->window), "GtrTranslationMemory", NULL);

  manager = gtr_window_get_ui_manager (priv->window);

  gtk_ui_manager_remove_ui (manager, priv->ui_id);
  gtk_ui_manager_remove_action_group (manager, priv->action_group);
}

static void
gtr_translation_memory_window_activatable_class_init (GtrTranslationMemoryWindowActivatableClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gtr_translation_memory_window_activatable_dispose;
  object_class->set_property = gtr_translation_memory_window_activatable_set_property;
  object_class->get_property = gtr_translation_memory_window_activatable_get_property;

  g_object_class_override_property (object_class, PROP_WINDOW, "window");

  g_type_class_add_private (klass, sizeof (GtrTranslationMemoryWindowActivatablePrivate));
}

static void
gtr_translation_memory_window_activatable_class_finalize (GtrTranslationMemoryWindowActivatableClass *klass)
{
}

static void
gtr_window_activatable_iface_init (GtrWindowActivatableInterface *iface)
{
  iface->activate = gtr_translation_memory_window_activatable_activate;
  iface->deactivate = gtr_translation_memory_window_activatable_deactivate;
}

void
gtr_window_activatable_register_peas_type (PeasObjectModule *module)
{
  gtr_translation_memory_window_activatable_register_type (G_TYPE_MODULE (module));

  peas_object_module_register_extension_type (module,
                                              GTR_TYPE_WINDOW_ACTIVATABLE,
                                              GTR_TYPE_TRANSLATION_MEMORY_WINDOW_ACTIVATABLE);
}
