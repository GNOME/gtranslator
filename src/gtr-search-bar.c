/*
 * Copyright (C) 2020  Priyanka Saggu <priyankasaggu11929@gmail.com>
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

#include "config.h"

#include <glib/gi18n.h>

#include "gtr-search-bar.h"
#include "gtr-actions.h"
#include "gtr-tab.h"
#include "gtr-window.h"
#include "gtr-utils.h"

struct _GtrSearchBar
{
  AdwBin                  parent_instance;

  GBindingGroup           *search_bindings;

  GObject                 *search_entry_tag;

  GtkButton               *replace_all_button;
  GtkButton               *replace_button;
  GtkButton               *previous_button;
  GtkButton               *next_button;
  GtkEntry                *replace_entry;
  GtkEntry                *search_entry;
  GtkRevealer             *search_options;
  GtkRevealer             *replace_options;
  GtkLabel                *search_text_error;

  GtrWindow               *active_window;

  guint                    show_options : 1;
  guint                    replace_mode : 1;

  gboolean                 case_sensitive;
  gboolean                 at_word_boundaries;
  gboolean                 wrap_around;
  gboolean                 at_original_text;
  gboolean                 at_translated_text;
};

enum {
  PROP_0,
  PROP_REPLACE_MODE,
  PROP_SHOW_OPTIONS,
  PROP_CASE_SENSITIVE,
  PROP_AT_WORD_BOUNDARIES,
  PROP_WRAP_AROUND,
  PROP_AT_ORIGINAL_TEXT,
  PROP_AT_TRANSLATED_TEXT,
  N_PROPS
};

enum {
  STOP_SEARCH,
  N_SIGNALS
};

G_DEFINE_FINAL_TYPE (GtrSearchBar, gtr_search_bar, ADW_TYPE_BIN)

static GParamSpec *properties [N_PROPS];
static guint signals [N_SIGNALS];

/*----------------------------------------NEW RE-WRITTEN METHODS--------------------------------------*/

void
gtr_search_bar_set_search_text (GtrSearchBar *dialog,
                                const gchar  *text)
{
  g_return_if_fail (GTR_IS_SEARCH_BAR (dialog));
  g_return_if_fail (text != NULL);

  //gtk_entry_set_text (GTK_ENTRY (dialog->search_entry), text);
  GtkEntryBuffer *entry_buffer = gtk_entry_get_buffer (GTK_ENTRY(dialog->search_entry));
  gtk_entry_buffer_set_text (entry_buffer, text, -1);
}

/*
 * The text must be unescaped before searching.
 */
const gchar*
gtr_search_bar_get_search_text (GtrSearchBar *dialog)
{
  const char *text;
  GtkEntryBuffer *buffer = gtk_entry_get_buffer (GTK_ENTRY (dialog->search_entry));
  text = gtk_entry_buffer_get_text (buffer);
  return text;
}

void
gtr_search_bar_set_replace_text (GtrSearchBar *dialog,
                                 const gchar  *text)
{
  GtkEntryBuffer *entry_buffer = gtk_entry_get_buffer (GTK_ENTRY (dialog->replace_entry));
  gtk_entry_buffer_set_text (entry_buffer, text, -1);
}

const gchar *
gtr_search_bar_get_replace_text (GtrSearchBar *dialog)
{
  const char *text;
  GtkEntryBuffer *buffer;

  buffer = gtk_entry_get_buffer (GTK_ENTRY (dialog->replace_entry));
  text = gtk_entry_buffer_get_text (buffer);
  return text;
}

void
gtr_search_bar_set_original_text (GtrSearchBar *self,
                                  gboolean      at_original_text)
{
  if (self->at_original_text == at_original_text)
    return;

  self->at_original_text = at_original_text;
  g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_AT_ORIGINAL_TEXT]);

  /* Make sure at least one is activated */
  if (!at_original_text && !self->at_translated_text)
    gtr_search_bar_set_translated_text (self, TRUE);
}

gboolean
gtr_search_bar_get_original_text (GtrSearchBar *self)
{
  g_return_val_if_fail (GTR_IS_SEARCH_BAR (self), FALSE);

  return self->at_original_text;
}

void
gtr_search_bar_set_translated_text (GtrSearchBar *self,
                                    gboolean      at_translated_text)
{
  g_return_if_fail (GTR_IS_SEARCH_BAR (self));

  if (self->at_translated_text == at_translated_text)
    return;

  self->at_translated_text = at_translated_text;
  g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_AT_TRANSLATED_TEXT]);

  /* Make sure at least one is activated */
  if (!self->at_original_text && !at_translated_text)
    gtr_search_bar_set_original_text (self, TRUE);
}

gboolean
gtr_search_bar_get_translated_text (GtrSearchBar *self)
{
  g_return_val_if_fail (GTR_IS_SEARCH_BAR (self), FALSE);

  return self->at_translated_text;
}

void
gtr_search_bar_set_match_case (GtrSearchBar *self,
                               gboolean      case_sensitive)
{
  g_return_if_fail (GTR_IS_SEARCH_BAR (self));

  if (self->case_sensitive == case_sensitive)
    return;

  self->case_sensitive = case_sensitive;
  g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_CASE_SENSITIVE]);
}

gboolean
gtr_search_bar_get_match_case (GtrSearchBar *self)
{
  g_return_val_if_fail (GTR_IS_SEARCH_BAR (self), FALSE);

  return self->case_sensitive;
}

void
gtr_search_bar_set_entire_word (GtrSearchBar *self,
                                gboolean      at_word_boundaries)
{
  g_return_if_fail (GTR_IS_SEARCH_BAR (self));

  if (self->at_word_boundaries == at_word_boundaries)
    return;

  self->at_word_boundaries = at_word_boundaries;
  g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_AT_WORD_BOUNDARIES]);
}

gboolean
gtr_search_bar_get_entire_word (GtrSearchBar *self)
{
  g_return_val_if_fail (GTR_IS_SEARCH_BAR (self), FALSE);

  return self->at_word_boundaries;
}

void
gtr_search_bar_set_wrap_around (GtrSearchBar *self,
                                gboolean wrap_around)
{

  g_return_if_fail (GTR_IS_SEARCH_BAR (self));

  if (self->wrap_around == wrap_around)
    return;

  self->wrap_around = wrap_around;
  g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_WRAP_AROUND]);
}

gboolean
gtr_search_bar_get_wrap_around (GtrSearchBar *self)
{
  g_return_val_if_fail (GTR_IS_SEARCH_BAR (self), FALSE);

  return self->wrap_around;
}

void
gtr_search_bar_find_next (GtrSearchBar *self)
{
  GtkWindow *window = GTK_WINDOW (gtk_widget_get_root (GTK_WIDGET (self)));
  do_find (self, GTR_WINDOW (window), FALSE);
}

void
gtr_search_bar_find_prev (GtrSearchBar *self)
{
  GtkWindow *window = GTK_WINDOW (gtk_widget_get_root (GTK_WIDGET (self)));
  do_find (self, GTR_WINDOW (window), TRUE);
}

static void
gtr_do_replace (GtrSearchBar *self)
{
  GtkWindow *window = GTK_WINDOW (gtk_widget_get_root (GTK_WIDGET (self)));
  do_replace (self, GTR_WINDOW (window));
}

static void
gtr_do_replace_all (GtrSearchBar *self)
{
  GtkWindow *window = GTK_WINDOW (gtk_widget_get_root (GTK_WIDGET (self)));
  do_replace_all (self, GTR_WINDOW (window));
}

static void
insert_text_handler (GtkEditable * editable,
                     const gchar * text,
                     gint length, gint * position, gpointer data)
{
  static gboolean insert_text = FALSE;
  gchar *escaped_text;
  gint new_len;

  /* To avoid recursive behavior */
  if (insert_text)
    return;

  escaped_text = gtr_utils_escape_search_text (text);

  new_len = strlen (escaped_text);

  if (new_len == length)
    {
      g_free (escaped_text);
      return;
    }

  insert_text = TRUE;

  g_signal_stop_emission_by_name (editable, "insert_text");

  gtk_editable_insert_text (editable, escaped_text, new_len, position);

  insert_text = FALSE;

  g_free (escaped_text);
}

/*-----------------------------END OF NEW RE_WRITTEN METHODS-----------------*/

gboolean
gtr_search_bar_get_replace_mode (GtrSearchBar *self)
{
  g_return_val_if_fail (GTR_IS_SEARCH_BAR (self), FALSE);

  return self->replace_mode;
}

void
gtr_search_bar_set_replace_mode (GtrSearchBar *self,
                                 gboolean      replace_mode)
{
    g_return_if_fail (GTR_IS_SEARCH_BAR (self));

  replace_mode = !!replace_mode;

  if (replace_mode != self->replace_mode)
    {
      self->replace_mode = replace_mode;
      gtk_revealer_set_reveal_child (self->replace_options, replace_mode);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_REPLACE_MODE]);
    }
}

static gboolean
maybe_escape_regex (GBinding     *binding,
                    const GValue *from_value,
                    GValue       *to_value,
                    gpointer      user_data)
{
  GtrSearchBar *self = user_data;
  const gchar *entry_text;

  g_assert (GTR_IS_SEARCH_BAR (self));
  g_assert (from_value != NULL);
  g_assert (to_value != NULL);

  entry_text = g_value_get_string (from_value);

  if (entry_text == NULL)
    {
      g_value_set_static_string (to_value, "");
    }
  else
    {
      g_autofree gchar *unescaped = NULL;

      entry_text = unescaped = gtk_source_utils_unescape_search_text (entry_text);

      g_value_set_string (to_value, entry_text);
    }

  return TRUE;
}

static gboolean
pacify_null_text (GBinding     *binding,
                  const GValue *from_value,
                  GValue       *to_value,
                  gpointer      user_data)
{
  g_assert (from_value != NULL);
  g_assert (to_value != NULL);
  g_assert (G_VALUE_HOLDS_STRING (from_value));
  g_assert (G_VALUE_HOLDS_STRING (to_value));

  if (g_value_get_string (from_value) == NULL)
    g_value_set_static_string (to_value, "");
  else
    g_value_copy (from_value, to_value);

  return TRUE;
}

static gboolean
gtr_search_bar_grab_focus (GtkWidget *widget)
{
  GtrSearchBar *self = (GtrSearchBar *)widget;

  g_assert (GTR_IS_SEARCH_BAR (self));

  return gtk_widget_grab_focus (GTK_WIDGET (self->search_entry));
}

static void
gtr_search_bar_dispose (GObject *object)
{
  GtrSearchBar *self = (GtrSearchBar *)object;

  g_clear_object (&self->search_bindings);
  g_clear_object (&self->search_entry_tag);

  G_OBJECT_CLASS (gtr_search_bar_parent_class)->dispose (object);
}

gboolean
gtr_search_bar_get_show_options (GtrSearchBar *self)
{
  g_return_val_if_fail (GTR_IS_SEARCH_BAR (self), FALSE);

  return self->show_options;
}

void
gtr_search_bar_set_show_options (GtrSearchBar *self,
                                 gboolean      show_options)
{
  g_return_if_fail (GTR_IS_SEARCH_BAR (self));

  show_options = !!show_options;

  if (self->show_options != show_options)
    {
      self->show_options = show_options;
      gtk_revealer_set_reveal_child (self->search_options, show_options);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_SHOW_OPTIONS]);
    }
}

static void
add_actions (GtrSearchBar *self)
{
  GSimpleActionGroup *group;
  GPropertyAction *action;

  group = g_simple_action_group_new ();

  action = g_property_action_new ("case-sensitive", self, "case-sensitive");
  g_action_map_add_action (G_ACTION_MAP (group), G_ACTION (action));
  g_object_unref (action);

  action = g_property_action_new ("at-word-boundaries", self, "at-word-boundaries");
  g_action_map_add_action (G_ACTION_MAP (group), G_ACTION (action));
  g_object_unref (action);

  action = g_property_action_new ("wrap-around", self, "wrap-around");
  g_action_map_add_action (G_ACTION_MAP (group), G_ACTION (action));
  g_object_unref (action);

  action = g_property_action_new ("at-original-text", self, "at-original-text");
  g_action_map_add_action (G_ACTION_MAP (group), G_ACTION (action));
  g_object_unref (action);

  action = g_property_action_new ("at-translated-text", self, "at-translated-text");
  g_action_map_add_action (G_ACTION_MAP (group), G_ACTION (action));
  g_object_unref (action);

  gtk_widget_insert_action_group (GTK_WIDGET (self), "search-settings", G_ACTION_GROUP (group));

  g_object_unref (group);
}


static void
gtr_search_bar_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  GtrSearchBar *self = GTR_SEARCH_BAR (object);

  switch (prop_id)
    {
    case PROP_REPLACE_MODE:
      g_value_set_boolean (value, gtr_search_bar_get_replace_mode (self));
      break;

    case PROP_SHOW_OPTIONS:
      g_value_set_boolean (value, gtr_search_bar_get_show_options (self));
      break;

    case PROP_CASE_SENSITIVE:
      g_value_set_boolean (value, gtr_search_bar_get_match_case (self));
      break;

    case PROP_AT_WORD_BOUNDARIES:
      g_value_set_boolean (value, gtr_search_bar_get_entire_word (self));
      break;

    case PROP_WRAP_AROUND:
      g_value_set_boolean (value, gtr_search_bar_get_wrap_around (self));
      break;

    case PROP_AT_ORIGINAL_TEXT:
      g_value_set_boolean (value, gtr_search_bar_get_original_text (self));
      break;

    case PROP_AT_TRANSLATED_TEXT:
      g_value_set_boolean (value, gtr_search_bar_get_translated_text (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gtr_search_bar_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  GtrSearchBar *self = GTR_SEARCH_BAR (object);

  switch (prop_id)
    {
    case PROP_REPLACE_MODE:
      gtr_search_bar_set_replace_mode (self, g_value_get_boolean (value));
      break;

    case PROP_SHOW_OPTIONS:
      gtr_search_bar_set_show_options (self, g_value_get_boolean (value));
      break;

    case PROP_CASE_SENSITIVE:
      gtr_search_bar_set_match_case (self, g_value_get_boolean (value));
      break;

    case PROP_AT_WORD_BOUNDARIES:
      gtr_search_bar_set_entire_word (self, g_value_get_boolean (value));
      break;

    case PROP_WRAP_AROUND:
      gtr_search_bar_set_wrap_around (self, g_value_get_boolean (value));
      break;

    case PROP_AT_ORIGINAL_TEXT:
      gtr_search_bar_set_original_text (self, g_value_get_boolean (value));
      break;

    case PROP_AT_TRANSLATED_TEXT:
      gtr_search_bar_set_translated_text (self, g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gtr_search_bar_class_init (GtrSearchBarClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->get_property = gtr_search_bar_get_property;
  object_class->set_property = gtr_search_bar_set_property;
  object_class->dispose = gtr_search_bar_dispose;

  widget_class->grab_focus = gtr_search_bar_grab_focus;

  properties [PROP_REPLACE_MODE] =
    g_param_spec_boolean ("replace-mode", NULL, NULL, FALSE,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  properties [PROP_SHOW_OPTIONS] =
    g_param_spec_boolean ("show-options", NULL, NULL, FALSE,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  properties [PROP_CASE_SENSITIVE] =
    g_param_spec_boolean ("case-sensitive", NULL, NULL, FALSE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  properties [PROP_AT_WORD_BOUNDARIES] =
    g_param_spec_boolean ("at-word-boundaries", NULL, NULL, FALSE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  properties [PROP_WRAP_AROUND] =
    g_param_spec_boolean ("wrap-around", NULL, NULL, TRUE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  properties [PROP_AT_ORIGINAL_TEXT] =
    g_param_spec_boolean ("at-original-text", NULL, NULL, TRUE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  properties [PROP_AT_TRANSLATED_TEXT] =
    g_param_spec_boolean ("at-translated-text", NULL, NULL, TRUE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (object_class, N_PROPS, properties);

  signals [STOP_SEARCH] =
    g_signal_new ("stop-search",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                  0,
                  NULL, NULL, NULL,
                  G_TYPE_NONE, 0);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/translator/gtr-search-bar.ui");
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, replace_all_button);
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, replace_button);
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, replace_entry);
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, search_entry);
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, previous_button);
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, next_button);
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, search_options);
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, replace_options);
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, search_text_error);

  gtk_widget_class_set_css_name (widget_class, "gtrsearchbar");

  // TODO:
  /* Add gtk_widget_class_add_binding for next-match and previous-match as
   * in gtksearchentry.c, which are already in the app as app.find-next and
   * app.find-prev */
  gtk_widget_class_add_binding_signal (GTK_WIDGET_CLASS (klass), GDK_KEY_Escape, 0, "stop-search", NULL);
}

static void
gtr_search_bar_init (GtrSearchBar *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

  // Sets the Original-text, Translated-text and Wrap-around enabled by default.

  self->case_sensitive = FALSE;
  self->at_word_boundaries = FALSE;
  self->wrap_around = TRUE;
  self->at_original_text = TRUE;
  self->at_translated_text = TRUE;

  self->search_bindings = g_binding_group_new ();

  g_binding_group_bind_full (self->search_bindings, "search-text",
                             self->search_entry, "text",
                             G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL,
                             maybe_escape_regex, pacify_null_text, self, NULL);

  g_binding_group_bind_full (self->search_bindings, "replacement-text",
                             self->replace_entry, "text",
                             G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL,
                             pacify_null_text, pacify_null_text, NULL, NULL);

  g_signal_connect (self->search_entry,
                    "insert_text", G_CALLBACK (insert_text_handler), NULL);

  g_signal_connect (self->replace_entry,
                    "insert_text", G_CALLBACK (insert_text_handler), NULL);

  g_signal_connect_swapped (self->search_entry,
                            "activate",
                            G_CALLBACK (gtr_search_bar_find_next),
                            self);

  g_signal_connect_swapped (self->next_button,
                            "clicked",
                            G_CALLBACK (gtr_search_bar_find_next),
                            self);

  g_signal_connect_swapped (self->previous_button,
                            "clicked",
                            G_CALLBACK (gtr_search_bar_find_prev),
                            self);

  g_signal_connect_swapped (self->replace_button,
                            "clicked",
                            G_CALLBACK (gtr_do_replace),
                            self);

  g_signal_connect_swapped (self->replace_all_button,
                            "clicked",
                            G_CALLBACK (gtr_do_replace_all),
                            self);

  add_actions (self);
}

void
gtr_search_bar_set_found (GtrSearchBar *self,
                          gboolean found)
{
  if (found)
    gtk_widget_remove_css_class (GTK_WIDGET (self->search_entry), "error");
  else
    gtk_widget_add_css_class (GTK_WIDGET (self->search_entry), "error");
}

/* Previously, declared but unimplemented */
GtkEntry *
gtr_search_bar_get_search (GtrSearchBar *self)
{
  return self->search_entry;
}

/**
 * gtr_search_bar_regex:
 * @self: a #GtrSearchBar
 *
 * Return value: (transfer full): A new #GRegex with the current search encoded
 **/
GRegex *
gtr_search_bar_regex (GtrSearchBar *self)
{
  const gchar *entry_text;
  gboolean match_case;
  gboolean entire_word;

  GRegex *regex = NULL;
  GRegexCompileFlags cflags = G_REGEX_DEFAULT | G_REGEX_MULTILINE;

  g_autofree char *pattern = NULL;
  g_autofree char *escaped = NULL;

  entry_text = gtr_search_bar_get_search_text (self);
  escaped = g_regex_escape_string (entry_text, strlen (entry_text));

  /* Flags */
  match_case = gtr_search_bar_get_match_case (self);
  entire_word = gtr_search_bar_get_entire_word (self);

  if (!match_case)
    cflags = cflags | G_REGEX_CASELESS;

  if (entire_word)
    {
      pattern = g_strdup_printf ("^(.+[^\\w]+)?%s([^\\w]+.+)?$", escaped);
    }
  else
    {
      pattern = g_strdup_printf ("^.*%s.*$", escaped);
    }
  regex = g_regex_new (pattern, cflags, G_REGEX_MATCH_DEFAULT, NULL);
  return regex;
}
