/*
 * Copyright (C) 2018  Daniel Garcia Moreno <danigm@gnome.org>
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
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib/gi18n.h>

#include "gtr-language.h"
#include "gtr-lang-button.h"
#include "gtr-drop-down-option.h"

typedef struct
{
  GListStore *model;
  GtkWidget *widget;
} GtrLangButtonPrivate;

struct _GtrLangButton
{
  AdwBin parent_instance;
};

enum
{
  CHANGED,
  LAST_SIGNAL
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrLangButton, gtr_lang_button, ADW_TYPE_BIN)
static guint signals[LAST_SIGNAL] = { 0 };

static gpointer
copy_lang (gconstpointer src,
           gpointer      data)
{
  GtrLanguage *lang = (GtrLanguage *)src;
  const gchar *langname = gtr_language_get_name (lang);

  return g_strdup ((char *)langname);
}

static void
gtr_lang_button_changed (GtkWidget  *widget,
                         GParamSpec *spec,
                         GtrLangButton *self)
{
  g_signal_emit (self, signals[CHANGED], 0, NULL);
}

static void
gtr_lang_button_dispose (GObject *object)
{
  G_OBJECT_CLASS (gtr_lang_button_parent_class)->dispose (object);
}

static void
gtr_lang_button_class_init (GtrLangButtonClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->dispose = gtr_lang_button_dispose;

  signals[CHANGED] =
    g_signal_newv ("changed",
                   G_OBJECT_CLASS_TYPE (object_class),
                   G_SIGNAL_RUN_LAST,
                   NULL, NULL, NULL, NULL,
                   G_TYPE_NONE, 0, NULL);
}

static void
gtr_lang_button_init (GtrLangButton *self)
{
  const GSList *languages;
  const GSList *o;
  GSList *langs;
  GtrLangButtonPrivate *priv = gtr_lang_button_get_instance_private (self);
  GtkExpression *expression = NULL;

  languages = gtr_language_get_languages ();
  langs = g_slist_copy_deep ((GSList *)languages, copy_lang, NULL);
  langs = g_slist_sort (langs, (GCompareFunc)g_utf8_collate);

  expression = gtk_property_expression_new (GTR_TYPE_DROP_DOWN_OPTION, NULL, "name");
  priv->model = g_list_store_new (GTR_TYPE_DROP_DOWN_OPTION);
  priv->widget = GTK_WIDGET (
    gtk_drop_down_new (G_LIST_MODEL (priv->model), expression)
  );
  gtk_drop_down_set_enable_search (GTK_DROP_DOWN (priv->widget), TRUE);

  for (o = langs; o != NULL; o = g_slist_next (o))
    {
      const char *lang = (char *)o->data;
      GtrDropDownOption *option = gtr_drop_down_option_new (lang, NULL);
      g_list_store_append (priv->model, option);
      g_object_unref (option);
    }

  adw_bin_set_child (ADW_BIN (self), priv->widget);

  if (langs)
    g_slist_free_full (langs, g_free);

  g_signal_connect (priv->widget,
                    "notify::selected",
                    G_CALLBACK (gtr_lang_button_changed),
                    self);
}

GtrLangButton*
gtr_lang_button_new (void) {
  GtrLangButton *self = g_object_new (GTR_TYPE_LANG_BUTTON, NULL);
  return self;
}

const gchar *
gtr_lang_button_get_lang (GtrLangButton *self)
{
  GtrLangButtonPrivate *priv = gtr_lang_button_get_instance_private (GTR_LANG_BUTTON (self));
  GtrDropDownOption *opt = GTR_DROP_DOWN_OPTION (
    gtk_drop_down_get_selected_item (GTK_DROP_DOWN (priv->widget))
  );
  if (!opt)
    return NULL;

  return gtr_drop_down_option_get_name (opt);
}

void
gtr_lang_button_set_lang (GtrLangButton *self,
                          const gchar   *name)
{
  GtrLangButtonPrivate *priv = gtr_lang_button_get_instance_private (GTR_LANG_BUTTON (self));
  unsigned int lang_pos = 0;
  GtrDropDownOption *option = gtr_drop_down_option_new (name, NULL);

  g_list_store_find_with_equal_func (
    priv->model,
    option,
    (GEqualFunc)gtr_drop_down_option_equal,
    &lang_pos
  );
  gtk_drop_down_set_selected (GTK_DROP_DOWN (priv->widget), lang_pos);
}

