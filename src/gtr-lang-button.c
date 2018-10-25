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

#include "gtr-language.h"
#include "gtr-lang-button.h"

typedef struct
{
  GtkWidget *lang;
  GtkWidget *lang_list;
  GtkWidget *popup;
  gchar *lang_name;

} GtrLangButtonPrivate;

struct _GtrLangButton
{
  GtkMenuButton parent_instance;
};

enum
{
  CHANGED,
  LAST_SIGNAL
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrLangButton, gtr_lang_button, GTK_TYPE_MENU_BUTTON)

static guint signals[LAST_SIGNAL] = { 0 };

static gint
compare_languages_name (gconstpointer a,
                        gconstpointer b)
{
  GtrLanguage *lang1, *lang2;
  const gchar *name1, *name2;

  lang1 = (GtrLanguage *) a;
  lang2 = (GtrLanguage *) b;

  name1 = gtr_language_get_name (lang1);
  name2 = gtr_language_get_name (lang2);

  return g_utf8_collate (name1, name2);
}

static void
change_language (GtkListBox    *box,
                 GtkListBoxRow *row,
                 GtrLangButton *self)
{
  GtrLangButtonPrivate *priv = gtr_lang_button_get_instance_private (self);
  GtkWidget *label = gtk_bin_get_child (GTK_BIN (row));
  gtr_lang_button_set_lang (self, gtk_label_get_text (GTK_LABEL (label)));

  gtk_popover_popdown (GTK_POPOVER (priv->popup));
}

static void
filter_language (GtkEditable   *entry,
                 GtrLangButton *self)
{
  GtrLangButtonPrivate *priv = gtr_lang_button_get_instance_private (self);
  const gchar *text = gtk_entry_get_text (GTK_ENTRY (entry));
  gchar *uptext = g_ascii_strup (text, -1);
  const GSList *languages, *l;
  GList *children;

  languages = gtr_language_get_languages ();
  languages = g_slist_sort ((GSList*)languages, compare_languages_name);

  children = gtk_container_get_children (GTK_CONTAINER (priv->lang_list));
  while (children)
    {
      GtkWidget *w = GTK_WIDGET (children->data);
      gtk_container_remove (GTK_CONTAINER (priv->lang_list), w);
      children = g_list_next (children);
    }

  for (l = languages; l != NULL; l = g_slist_next (l))
    {
      GtrLanguage *lang = (GtrLanguage *)l->data;
      const gchar *langname = gtr_language_get_name (lang);
      GtkWidget *child;
      gchar *uplang = g_ascii_strup (langname, -1);

      if (g_strrstr (uplang, uptext) == NULL) {
        g_free (uplang);
        continue;
      }
      g_free (uplang);

      child = gtk_label_new (langname);
      gtk_label_set_xalign (GTK_LABEL (child), 0.0);
      gtk_container_add (GTK_CONTAINER (priv->lang_list), child);
    }
  gtk_widget_show_all (priv->lang_list);

  g_free (uptext);
}

static void
gtr_lang_button_finalize (GObject *object)
{
  GtrLangButtonPrivate *priv = gtr_lang_button_get_instance_private (GTR_LANG_BUTTON (object));

  g_clear_pointer (&priv->lang_name, g_free);

  G_OBJECT_CLASS (gtr_lang_button_parent_class)->finalize (object);
}

static void
gtr_lang_button_class_init (GtrLangButtonClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gtr_lang_button_finalize;

  signals[CHANGED] =
    g_signal_newv ("changed",
                   G_OBJECT_CLASS_TYPE (object_class),
                   G_SIGNAL_RUN_LAST,
                   NULL, NULL, NULL, NULL,
                   G_TYPE_NONE, 0, NULL);

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gnome/translator/gtr-lang-button.ui");

  gtk_widget_class_bind_template_child_private (widget_class, GtrLangButton, lang);
  gtk_widget_class_bind_template_child_private (widget_class, GtrLangButton, lang_list);
  gtk_widget_class_bind_template_child_private (widget_class, GtrLangButton, popup);
}

static void
gtr_lang_button_init (GtrLangButton *self)
{
  const GSList *languages, *l;
  GtrLangButtonPrivate *priv = gtr_lang_button_get_instance_private (self);
  gtk_widget_init_template (GTK_WIDGET (self));

  languages = gtr_language_get_languages ();
  languages = g_slist_sort ((GSList*)languages, compare_languages_name);

  for (l = languages; l != NULL; l = g_slist_next (l))
    {
      GtrLanguage *lang = (GtrLanguage *)l->data;
      GtkWidget *child = gtk_label_new (gtr_language_get_name (lang));
      gtk_label_set_xalign (GTK_LABEL (child), 0.0);
      gtk_container_add (GTK_CONTAINER (priv->lang_list), child);
    }

  gtk_widget_show_all (priv->lang_list);
  priv->lang_name = NULL;

  g_signal_connect (priv->lang_list,
                    "row-activated",
                    G_CALLBACK (change_language),
                    self);

  g_signal_connect (priv->lang,
                    "changed",
                    G_CALLBACK (filter_language),
                    self);
}

GtrLangButton*
gtr_lang_button_new () {
  GtrLangButton *self = g_object_new (GTR_TYPE_LANG_BUTTON, NULL);
  return self;
}

const gchar *
gtr_lang_button_get_lang (GtrLangButton *self)
{
  GtrLangButtonPrivate *priv = gtr_lang_button_get_instance_private (self);
  return priv->lang_name;
}

void
gtr_lang_button_set_lang (GtrLangButton *self,
                          const gchar   *name)
{
  GtrLangButtonPrivate *priv = gtr_lang_button_get_instance_private (self);
  g_clear_pointer (&priv->lang_name, g_free);
  priv->lang_name = g_strdup (name);
  gtk_button_set_label (GTK_BUTTON (self), name);
  g_signal_emit (self, signals[CHANGED], 0, NULL);
}

