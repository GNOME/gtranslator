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

#include "gtr-translation-memory-tab-activatable.h"
#include "gtr-window.h"
#include "gtr-tab-activatable.h"
#include "gtr-translation-memory.h"
#include "gtr-translation-memory-ui.h"

#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <string.h>

struct _GtrTranslationMemoryTabActivatablePrivate
{
  GtrTab *tab;
  GtrTranslationMemory *translation_memory;
  GtkWidget *tm_ui;
};

enum
{
  PROP_0,
  PROP_TAB
};

static void gtr_tab_activatable_iface_init (GtrTabActivatableInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED (GtrTranslationMemoryTabActivatable,
                                gtr_translation_memory_tab_activatable,
                                PEAS_TYPE_EXTENSION_BASE,
                                0,
                                G_IMPLEMENT_INTERFACE_DYNAMIC (GTR_TYPE_TAB_ACTIVATABLE,
                                                               gtr_tab_activatable_iface_init))

static void
gtr_translation_memory_tab_activatable_init (GtrTranslationMemoryTabActivatable *tab_activatable)
{
  tab_activatable->priv = G_TYPE_INSTANCE_GET_PRIVATE (tab_activatable,
                                                       GTR_TYPE_TRANSLATION_MEMORY_TAB_ACTIVATABLE,
                                                       GtrTranslationMemoryTabActivatablePrivate);
}

static void
gtr_translation_memory_tab_activatable_dispose (GObject *object)
{
  GtrTranslationMemoryTabActivatablePrivate *priv = GTR_TRANSLATION_MEMORY_TAB_ACTIVATABLE (object)->priv;

  g_clear_object (&priv->tab);

  G_OBJECT_CLASS (gtr_translation_memory_tab_activatable_parent_class)->dispose (object);
}

static void
gtr_translation_memory_tab_activatable_set_property (GObject      *object,
                                                     guint         prop_id,
                                                     const GValue *value,
                                                     GParamSpec   *pspec)
{
  GtrTranslationMemoryTabActivatablePrivate *priv = GTR_TRANSLATION_MEMORY_TAB_ACTIVATABLE (object)->priv;

  switch (prop_id)
    {
      case PROP_TAB:
        priv->tab = GTR_TAB (g_value_dup_object (value));
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
gtr_translation_memory_tab_activatable_get_property (GObject    *object,
                                                     guint       prop_id,
                                                     GValue     *value,
                                                     GParamSpec *pspec)
{
  GtrTranslationMemoryTabActivatablePrivate *priv = GTR_TRANSLATION_MEMORY_TAB_ACTIVATABLE (object)->priv;

  switch (prop_id)
    {
      case PROP_TAB:
        g_value_set_object (value, priv->tab);
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
on_message_edition_finished (GtrTab                             *tab,
                             GtrMsg                             *msg,
                             GtrTranslationMemoryTabActivatable *activatable)
{
  GtrHeader *header;
  GtrPo *po;

  po = gtr_tab_get_po (tab);
  header = gtr_po_get_header (po);

  if (gtr_header_get_profile (header) != NULL)
    {
      if (gtr_msg_is_translated (msg) && !gtr_msg_is_fuzzy (msg))
        gtr_translation_memory_store (activatable->priv->translation_memory, msg);
    }
}

static void
gtr_translation_memory_tab_activatable_activate (GtrTabActivatable *activatable)
{
  GtrTranslationMemoryTabActivatablePrivate *priv = GTR_TRANSLATION_MEMORY_TAB_ACTIVATABLE (activatable)->priv;
  GtkWidget *window;

  window = gtk_widget_get_toplevel (GTK_WIDGET (priv->tab));

  priv->translation_memory = GTR_TRANSLATION_MEMORY (g_object_get_data (G_OBJECT (window), "GtrTranslationMemory"));

  /* TM */
  priv->tm_ui = gtr_translation_memory_ui_new (GTK_WIDGET (priv->tab), priv->translation_memory);
  gtk_widget_show (priv->tm_ui);
  gtr_tab_add_widget (priv->tab, priv->tm_ui,
                      "GtrTranslationMemoryUI",
                      _("Translation Memory"),
                      NULL,
                      GTR_TAB_PLACEMENT_RIGHT);

  g_signal_connect (priv->tab, "message-edition-finished",
                    G_CALLBACK (on_message_edition_finished),
                    activatable);
}

static void
gtr_translation_memory_tab_activatable_deactivate (GtrTabActivatable *activatable)
{
  GtrTranslationMemoryTabActivatablePrivate *priv = GTR_TRANSLATION_MEMORY_TAB_ACTIVATABLE (activatable)->priv;

  g_signal_handlers_disconnect_by_func (priv->tab, on_message_edition_finished, activatable);

  gtr_tab_remove_widget (priv->tab, priv->tm_ui);
}

static void
gtr_translation_memory_tab_activatable_class_init (GtrTranslationMemoryTabActivatableClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gtr_translation_memory_tab_activatable_dispose;
  object_class->set_property = gtr_translation_memory_tab_activatable_set_property;
  object_class->get_property = gtr_translation_memory_tab_activatable_get_property;

  g_object_class_override_property (object_class, PROP_TAB, "tab");

  g_type_class_add_private (klass, sizeof (GtrTranslationMemoryTabActivatablePrivate));
}

static void
gtr_translation_memory_tab_activatable_class_finalize (GtrTranslationMemoryTabActivatableClass *klass)
{
}

static void
gtr_tab_activatable_iface_init (GtrTabActivatableInterface *iface)
{
  iface->activate = gtr_translation_memory_tab_activatable_activate;
  iface->deactivate = gtr_translation_memory_tab_activatable_deactivate;
}

void
gtr_tab_activatable_register_peas_type (PeasObjectModule *module)
{
  gtr_translation_memory_tab_activatable_register_type (G_TYPE_MODULE (module));

  peas_object_module_register_extension_type (module,
                                              GTR_TYPE_TAB_ACTIVATABLE,
                                              GTR_TYPE_TRANSLATION_MEMORY_TAB_ACTIVATABLE);
}
