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
#include "gtr-lang-combo-row.h"
#include "gtr-drop-down-option.h"

typedef struct
{
  GListStore *model;
} GtrLangComboRowPrivate;

struct _GtrLangComboRow
{
  AdwComboRow parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrLangComboRow, gtr_lang_combo_row, ADW_TYPE_COMBO_ROW)

static gpointer
copy_lang (gconstpointer src,
           gpointer      data)
{
  GtrLanguage *lang = (GtrLanguage *)src;
  const gchar *langname = gtr_language_get_name (lang);

  return g_strdup ((char *)langname);
}

static void
gtr_lang_combo_row_class_init (GtrLangComboRowClass *klass)
{
}

static void
gtr_lang_combo_row_init (GtrLangComboRow *self)
{
  const GSList *languages;
  const GSList *o;
  GSList *langs;
  GtrLangComboRowPrivate *priv = gtr_lang_combo_row_get_instance_private (self);
  g_autoptr(GtkExpression) expression = NULL;

  languages = gtr_language_get_languages ();
  langs = g_slist_copy_deep ((GSList *)languages, copy_lang, NULL);
  langs = g_slist_sort (langs, (GCompareFunc)g_utf8_collate);

  expression = gtk_property_expression_new (GTR_TYPE_DROP_DOWN_OPTION, NULL, "name");
  priv->model = g_list_store_new (GTR_TYPE_DROP_DOWN_OPTION);
  adw_combo_row_set_model (ADW_COMBO_ROW (self), G_LIST_MODEL (priv->model));
  adw_combo_row_set_expression (ADW_COMBO_ROW (self), expression);
  adw_preferences_row_set_title (ADW_PREFERENCES_ROW (self), _("Languages"));
  adw_combo_row_set_enable_search (ADW_COMBO_ROW (self), TRUE);

  for (o = langs; o != NULL; o = g_slist_next (o))
    {
      const char *lang = (char *)o->data;
      GtrDropDownOption *option = gtr_drop_down_option_new (lang, NULL);
      g_list_store_append (priv->model, option);
      g_object_unref (option);
    }

  if (langs)
    g_slist_free_full (langs, g_free);
}

GtrLangComboRow*
gtr_lang_combo_row_new (void) {
  GtrLangComboRow *self = g_object_new (GTR_TYPE_LANG_COMBO_ROW, NULL);
  return self;
}

const gchar *
gtr_lang_combo_row_get_lang (GtrLangComboRow *self)
{
  GtrDropDownOption *opt = GTR_DROP_DOWN_OPTION (
    adw_combo_row_get_selected_item (ADW_COMBO_ROW (self))
  );
  if (!opt)
    return NULL;

  return gtr_drop_down_option_get_name (opt);
}

void
gtr_lang_combo_row_set_lang (GtrLangComboRow *self,
                          const gchar   *name)
{
  GtrLangComboRowPrivate *priv = gtr_lang_combo_row_get_instance_private (GTR_LANG_COMBO_ROW (self));
  unsigned int lang_pos = 0;
  g_autoptr(GtrDropDownOption) option = gtr_drop_down_option_new (name, NULL);

  g_list_store_find_with_equal_func (
    priv->model,
    option,
    (GEqualFunc)gtr_drop_down_option_equal,
    &lang_pos
  );
  adw_combo_row_set_selected (ADW_COMBO_ROW (self), lang_pos);
}

