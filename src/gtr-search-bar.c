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

#define G_LOG_DOMAIN "gtr-search-bar"

#include "config.h"

#include <glib/gi18n.h>

#include "gtr-search-bar.h"
#include "gtr-actions.h"
#include "gtr-tab.h"
#include "gtr-window.h"
#include "gtr-utils.h"

struct _GtrSearchBar
{
  GtkBin                  parent_instance;

  GBindingGroup           *search_bindings;

  GObject                 *search_entry_tag;

  GtkCheckButton          *case_sensitive;
  GtkButton               *replace_all_button;
  GtkButton               *replace_button;
  GtkButton               *previous_button;
  GtkButton               *next_button;
  GtkEntry                *replace_entry;
  GtkEntry                *search_entry;
  GtkGrid                 *search_options;
  GtkCheckButton          *whole_word;
  GtkCheckButton          *wrap_around_button;
  GtkCheckButton          *original_text_checkbutton;
  GtkCheckButton          *translated_text_checkbutton;
  GtkLabel                *search_text_error;

  GtrWindow               *active_window;

  guint                    show_options : 1;
  guint                    replace_mode : 1;
};

enum {
  PROP_0,
  PROP_REPLACE_MODE,
  PROP_SHOW_OPTIONS,
  N_PROPS
};

enum {
  STOP_SEARCH,
  N_SIGNALS
};

G_DEFINE_FINAL_TYPE (GtrSearchBar, gtr_search_bar, GTK_TYPE_BIN)

static GParamSpec *properties [N_PROPS];
static guint signals [N_SIGNALS];

/*----------------------------------------NEW RE-WRITTEN METHODS--------------------------------------*/

void
gtr_search_bar_set_search_text (GtrSearchBar *dialog,
                                const gchar  *text)
{
  g_return_if_fail (GTR_IS_SEARCH_BAR (dialog));
  g_return_if_fail (text != NULL);

  gtk_entry_set_text (GTK_ENTRY (dialog->search_entry), text);
}

/*
 * The text must be unescaped before searching.
 */
const gchar*
gtr_search_bar_get_search_text (GtrSearchBar *dialog)
{
  //g_assert (GTR_IS_SEARCH_BAR (dialog));
  g_return_val_if_fail (GTR_IS_SEARCH_BAR (dialog), NULL);

  return gtk_entry_get_text (GTK_ENTRY (dialog->search_entry));
}

void
gtr_search_bar_set_replace_text (GtrSearchBar *dialog,
                                 const gchar  *text)
{
  //g_assert (GTR_IS_SEARCH_BAR (dialog));
  g_return_if_fail (GTR_IS_SEARCH_BAR (dialog));
  g_return_if_fail (text != NULL);

  gtk_entry_set_text (GTK_ENTRY (dialog->replace_entry), text);
}

const gchar *
gtr_search_bar_get_replace_text (GtrSearchBar *dialog)
{
  g_return_val_if_fail (GTR_IS_SEARCH_BAR (dialog), NULL);

  return gtk_entry_get_text (GTK_ENTRY (dialog->replace_entry));
}

void
gtr_search_bar_set_original_text (GtrSearchBar *dialog,
                                  gboolean match_case)
{
  g_return_if_fail (GTR_IS_SEARCH_BAR (dialog));

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON
                                (dialog->original_text_checkbutton),
                                match_case);
}

gboolean
gtr_search_bar_get_original_text (GtrSearchBar *dialog)
{
  g_return_val_if_fail (GTR_IS_SEARCH_BAR (dialog), FALSE);

  return
    gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
                                  (dialog->original_text_checkbutton));
}

void
gtr_search_bar_set_translated_text (GtrSearchBar *dialog,
                                    gboolean match_case)
{

  g_return_if_fail (GTR_IS_SEARCH_BAR (dialog));

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON
                                (dialog->translated_text_checkbutton),
                                match_case);
}

gboolean
gtr_search_bar_get_translated_text (GtrSearchBar *dialog)
{

  g_return_val_if_fail (GTR_IS_SEARCH_BAR (dialog), FALSE);

  return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dialog->translated_text_checkbutton));
}

void
gtr_search_bar_set_match_case (GtrSearchBar *dialog,
                               gboolean match_case)
{

  g_return_if_fail (GTR_IS_SEARCH_BAR (dialog));

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dialog->case_sensitive),
                                match_case);
}

gboolean
gtr_search_bar_get_match_case (GtrSearchBar *dialog)
{
  g_return_val_if_fail (GTR_IS_SEARCH_BAR (dialog), FALSE);

  return
    gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
                                  (dialog->case_sensitive));
}

void
gtr_search_bar_set_entire_word (GtrSearchBar *dialog,
                                gboolean entire_word)
{
  g_return_if_fail (GTR_IS_SEARCH_BAR (dialog));

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON
                                (dialog->whole_word),
                                entire_word);
}

gboolean
gtr_search_bar_get_entire_word (GtrSearchBar *dialog)
{

  g_return_val_if_fail (GTR_IS_SEARCH_BAR (dialog), FALSE);

  return
    gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
                                  (dialog->whole_word));
}

void
gtr_search_bar_set_wrap_around (GtrSearchBar *dialog,
                                   gboolean wrap_around)
{

  g_return_if_fail (GTR_IS_SEARCH_BAR (dialog));

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON
                                (dialog->wrap_around_button),
                                wrap_around);
}

gboolean
gtr_search_bar_get_wrap_around (GtrSearchBar *dialog)
{
  g_return_val_if_fail (GTR_IS_SEARCH_BAR (dialog), FALSE);

  return
    gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
                                  (dialog->wrap_around_button));
}

void
gtr_hide_bar (GtrSearchBar *self)
{
  GtkWidget *window = gtk_widget_get_toplevel (GTK_WIDGET (self));

  GtrTab *active_tab = gtr_window_get_active_tab (GTR_WINDOW (window));

  return gtr_tab_show_hide_search_bar (active_tab, FALSE);
}

void
gtr_search_bar_find_next (GtrSearchBar *self)
{
  GtkWidget *window = gtk_widget_get_toplevel (GTK_WIDGET (self));
  do_find (self, GTR_WINDOW (window), FALSE);
}

void
gtr_search_bar_find_prev (GtrSearchBar *self)
{
  GtkWidget *window = gtk_widget_get_toplevel (GTK_WIDGET (self));
  do_find (self, GTR_WINDOW (window), TRUE);
}

void
gtr_do_replace (GtrSearchBar *self)
{
  GtkWidget *window = gtk_widget_get_toplevel (GTK_WIDGET (self));
  do_replace (self, GTR_WINDOW (window));
}

void
gtr_do_replace_all (GtrSearchBar *self)
{
  GtkWidget *window = gtk_widget_get_toplevel (GTK_WIDGET (self));
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

/*
 * Functions to make sure either translated or original checkbutton
 * is selected at any time. Anything else does not make any sense.
 */
static void
translated_checkbutton_toggled (GtkToggleButton * button,
                                         GtrSearchBar * dialog)
{
  gboolean original_text;
  gboolean translated_text;

  original_text =
    gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
                                  (dialog->original_text_checkbutton));
  translated_text =
    gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
                                  (
                                   dialog->translated_text_checkbutton));

  if (!original_text && !translated_text)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON
                                    (dialog->original_text_checkbutton),
                                     TRUE);
    }
}

static void
original_checkbutton_toggled (GtkToggleButton * button,
                              GtrSearchBar * dialog)
{
  gboolean original_text;
  gboolean translated_text;

  original_text =
    gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
                                  (dialog->original_text_checkbutton));
  translated_text =
    gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
                                  (
                                   dialog->translated_text_checkbutton));

  if (!original_text && !translated_text)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON
                                    (dialog->translated_text_checkbutton),
                                     TRUE);
    }
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
      gtk_widget_set_visible (GTK_WIDGET (self->replace_entry), replace_mode);
      gtk_widget_set_visible (GTK_WIDGET (self->replace_button), replace_mode);
      gtk_widget_set_visible (GTK_WIDGET (self->replace_all_button), replace_mode);
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

static void
gtr_search_bar_grab_focus (GtkWidget *widget)
{
  GtrSearchBar *self = (GtrSearchBar *)widget;

  g_assert (GTR_IS_SEARCH_BAR (self));

  gtk_widget_grab_focus (GTK_WIDGET (self->search_entry));
}

static void
gtr_search_bar_real_stop_search (GtrSearchBar *self)
{
  g_assert (GTR_IS_SEARCH_BAR (self));
}

static void
search_entry_stop_search (GtrSearchBar *self,
                          GtkEntry     *entry)
{
  g_assert (GTR_IS_SEARCH_BAR (self));
  g_assert (GTK_IS_ENTRY (entry));

  g_signal_emit (self, signals [STOP_SEARCH], 0);
}

static void
gtr_search_bar_destroy (GtkWidget *widget)
{
  GtrSearchBar *self = (GtrSearchBar *)widget;

  g_clear_object (&self->search_bindings);
  g_clear_object (&self->search_entry_tag);

  GTK_WIDGET_CLASS (gtr_search_bar_parent_class)->destroy (widget);
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
      gtk_widget_set_visible (GTK_WIDGET (self->search_options), show_options);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_SHOW_OPTIONS]);
    }
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

  widget_class->destroy = gtr_search_bar_destroy;
  widget_class->grab_focus = gtr_search_bar_grab_focus;

  properties [PROP_REPLACE_MODE] =
    g_param_spec_boolean ("replace-mode", NULL, NULL, FALSE,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  properties [PROP_SHOW_OPTIONS] =
    g_param_spec_boolean ("show-options", NULL, NULL, FALSE,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPS, properties);

  signals [STOP_SEARCH] =
    g_signal_new_class_handler ("stop-search",
                                G_TYPE_FROM_CLASS (klass),
                                G_SIGNAL_RUN_LAST,
                                G_CALLBACK (gtr_search_bar_real_stop_search),
                                NULL, NULL,
                                g_cclosure_marshal_VOID__VOID,
                                G_TYPE_NONE, 0);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/translator/gtr-search-bar.ui");
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, case_sensitive);
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, replace_all_button);
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, replace_button);
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, replace_entry);
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, search_entry);
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, previous_button);
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, next_button);
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, search_options);
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, search_text_error);
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, whole_word);
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, wrap_around_button );
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, original_text_checkbutton);
  gtk_widget_class_bind_template_child (widget_class, GtrSearchBar, translated_text_checkbutton);

  gtk_widget_class_set_css_name (widget_class, "gtrsearchbar");
}

static void
gtr_search_bar_init (GtrSearchBar *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

  // Sets the Original-text, Translated-text and Wrap-around checkbuttons toggled by default.

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON
                                (self->original_text_checkbutton),
                                TRUE);

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON
                                (self->translated_text_checkbutton),
                                TRUE);

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON
                                (self->wrap_around_button),
                                TRUE);

  g_object_set (G_OBJECT (self->next_button), "can-default", TRUE, NULL);

  self->search_bindings = g_binding_group_new ();

  g_binding_group_bind_full (self->search_bindings, "search-text",
                             self->search_entry, "text",
                             G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL,
                             maybe_escape_regex, pacify_null_text, self, NULL);

  g_binding_group_bind_full (self->search_bindings, "replacement-text",
                             self->replace_entry, "text",
                             G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL,
                             pacify_null_text, pacify_null_text, NULL, NULL);

  g_binding_group_bind (self->search_bindings, "case-sensitive",
                        self->case_sensitive, "active",
                        G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL);

  g_binding_group_bind (self->search_bindings, "at-word-boundaries",
                        self->whole_word, "active",
                        G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL);

  g_binding_group_bind (self->search_bindings, "wrap-around",
                        self->whole_word, "active",
                        G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL);

  g_binding_group_bind (self->search_bindings, "at-original-text",
                        self->whole_word, "active",
                        G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL);

  g_binding_group_bind (self->search_bindings, "at-translated-text",
                        self->whole_word, "active",
                        G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL);

  g_signal_connect (self->search_entry,
                    "insert_text", G_CALLBACK (insert_text_handler), NULL);

  g_signal_connect (self->replace_entry,
                    "insert_text", G_CALLBACK (insert_text_handler), NULL);

  g_signal_connect (self->original_text_checkbutton,
                    "toggled",
                    G_CALLBACK (original_checkbutton_toggled),
                    self);

  g_signal_connect (self->translated_text_checkbutton,
                    "toggled",
                    G_CALLBACK (translated_checkbutton_toggled),
                    self);

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

  g_signal_connect_swapped (self->search_entry,
                            "stop-search",
                            G_CALLBACK (search_entry_stop_search),
                            self);
}

void
gtr_search_bar_set_found (GtrSearchBar *self,
                          gboolean found)
{
  GtkStyleContext *context = gtk_widget_get_style_context (
    GTK_WIDGET (self->search_entry));
  if (found)
    gtk_style_context_remove_class (context, "error");
  else
    gtk_style_context_add_class (context, "error");
}

