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

struct _GtrLangButton
{
  GtrFilterSelection parent_instance;
};

enum
{
  CHANGED,
  LAST_SIGNAL
};

G_DEFINE_TYPE (GtrLangButton, gtr_lang_button, GTR_TYPE_FILTER_SELECTION)

static gpointer
copy_lang (gconstpointer src,
           gpointer      data)
{
  GtrLanguage *lang = (GtrLanguage *)src;
  const gchar *langname = gtr_language_get_name (lang);

  return g_strdup ((char *)langname);
}

static void
gtr_lang_button_class_init (GtrLangButtonClass *klass)
{
}

static void
gtr_lang_button_init (GtrLangButton *self)
{
  const GSList *languages;
  GSList *langs;
  languages = gtr_language_get_languages ();
  langs = g_slist_copy_deep ((GSList *)languages, copy_lang, NULL);
  langs = g_slist_sort (langs, (GCompareFunc)g_utf8_collate);
  gtr_filter_selection_set_options (GTR_FILTER_SELECTION (self), langs);
  gtr_filter_selection_set_text (GTR_FILTER_SELECTION (self), _("Choose Language"));

  if (langs)
    g_slist_free_full (langs, g_free);
  g_printf("ran init of lang button\n");
}

GtrLangButton*
gtr_lang_button_new () {
  GtrLangButton *self = g_object_new (GTR_TYPE_LANG_BUTTON, NULL);
  return self;
}

const gchar *
gtr_lang_button_get_lang (GtrLangButton *self)
{
  const GtrFilterOption *opt = gtr_filter_selection_get_option (GTR_FILTER_SELECTION (self));
  if (!opt)
    return NULL;
  return opt->name;
}

void
gtr_lang_button_set_lang (GtrLangButton *self,
                          const gchar   *name)
{
  gtr_filter_selection_set_option (GTR_FILTER_SELECTION (self), name);
}

