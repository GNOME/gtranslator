/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *		 1998, 1999 Alex Roberts, Evan Lawrence
 * 		 2000  2002 Chema Celorio, Paolo Maggi 
 * 		 2003  2005 Paolo Maggi  
 * 
 * Some funcs based in gedit-view.c file.
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
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include "gtr-dirs.h"
#include "gtr-settings.h"
#include "gtr-utils.h"
#include "gtr-view.h"
#include "gtr-application.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <gtksourceview/gtksource.h>
#ifdef LIBSPELL
#include <libspelling.h>
#endif

typedef struct
{
  GSettings *editor_settings;
  GSettings *ui_settings;

  GtkSourceBuffer *buffer;
#ifdef LIBSPELL
  SpellingChecker *checker;
#endif

  guint search_flags;
  gchar *search_text;

  GtkCssProvider *provider;
} GtrViewPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GtrView, gtr_view, GTK_SOURCE_TYPE_VIEW)

static void
notify_dark_cb (GtrView *view)
{
  gtr_view_reload_scheme_color (view);
}

static void
gtr_view_init (GtrView * view)
{
  g_autoptr (GtkSourceLanguageManager) lm = NULL;
  GtkSourceLanguage *lang;
  g_autoptr(GStrvBuilder) builder = g_strv_builder_new ();
  g_auto(GStrv) langs = NULL;
  const gchar *const *temp;
  gchar *ui_dir;
  GtrViewPrivate *priv;
  AdwStyleManager *manager;

#ifdef LIBSPELL
  GMenuModel *extra_menu = NULL;
  g_autoptr(SpellingTextBufferAdapter) adapter = NULL;
#endif
  g_autofree char *font = NULL;

  priv = gtr_view_get_instance_private (view);

  priv->provider = gtk_css_provider_new ();
  gtk_style_context_add_provider_for_display (
    gdk_display_get_default (),
    GTK_STYLE_PROVIDER (priv->provider),
    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
  );

  priv->editor_settings = g_settings_new ("org.gnome.gtranslator.preferences.editor");
  priv->ui_settings = g_settings_new ("org.gnome.gtranslator.preferences.ui");

  lm = gtk_source_language_manager_new ();

  for (temp = gtk_source_language_manager_get_search_path (lm);
       temp != NULL && *temp != NULL; ++temp)
    g_strv_builder_add (builder, *temp);

  ui_dir = g_build_filename (gtr_dirs_get_gtr_data_dir (), "ui", NULL);
  g_strv_builder_take (builder, ui_dir);
  langs = g_strv_builder_end (builder);

  gtk_source_language_manager_set_search_path (lm, (const  char * const *) langs);
  lang = gtk_source_language_manager_get_language (lm, "gtranslator");

  priv->buffer = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)));
  gtk_source_buffer_set_language (priv->buffer, lang);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (view), GTK_WRAP_WORD);

  /* Set syntax highlight according to preferences */
  gtk_source_buffer_set_highlight_syntax (priv->buffer,
                                          g_settings_get_boolean (priv->editor_settings,
                                                                  GTR_SETTINGS_HIGHLIGHT_SYNTAX));

  /* Set dot char according to preferences */
  gtr_view_enable_visible_whitespace (view,
                                      g_settings_get_boolean (priv->editor_settings,
                                                              GTR_SETTINGS_VISIBLE_WHITESPACE));

  font = g_settings_get_string (priv->editor_settings, GTR_SETTINGS_FONT);
  gtr_view_set_font (view, font);

  /* Set scheme color according to preferences */
  gtr_view_reload_scheme_color (view);
  manager = adw_style_manager_get_default ();
  g_signal_connect_swapped (manager, "notify::dark",
                            G_CALLBACK (notify_dark_cb), view);
  gtk_text_view_set_monospace (GTK_TEXT_VIEW (view), TRUE);

#ifdef LIBSPELL
  priv->checker = spelling_checker_new (spelling_provider_get_default (), "en_US");
  adapter = spelling_text_buffer_adapter_new (priv->buffer, priv->checker);
  extra_menu = spelling_text_buffer_adapter_get_menu_model (adapter);

  gtk_text_view_set_extra_menu (GTK_TEXT_VIEW (view), extra_menu);
  gtk_widget_insert_action_group (GTK_WIDGET (view), "spelling", G_ACTION_GROUP (adapter));
  g_settings_bind (priv->editor_settings, GTR_SETTINGS_SPELLCHECK, adapter,
                   "enabled", G_SETTINGS_BIND_GET | G_SETTINGS_BIND_SET);
  gtr_view_set_lang (GTR_VIEW (view), "en_US");
#endif
}

static void
gtr_view_dispose (GObject * object)
{
  GtrView *view = GTR_VIEW (object);
  GtrViewPrivate *priv;

  priv = gtr_view_get_instance_private (view);

  g_clear_object (&priv->editor_settings);
  g_clear_object (&priv->ui_settings);
  g_clear_object (&priv->provider);
#ifdef LIBSPELL
  g_clear_object (&priv->checker);
#endif

  G_OBJECT_CLASS (gtr_view_parent_class)->dispose (object);
}

static void
gtr_view_finalize (GObject *object)
{
  GtrView *view = GTR_VIEW (object);
  GtrViewPrivate *priv;

  priv = gtr_view_get_instance_private (view);

  g_clear_pointer (&priv->search_text, g_free);

  G_OBJECT_CLASS (gtr_view_parent_class)->finalize (object);
}

static void
gtr_view_class_init (GtrViewClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gtr_view_dispose;
  object_class->finalize = gtr_view_finalize;
}

/**
 * gtr_view_new:
 *
 * Creates a new #GtrView. An empty default buffer will be created for you.
 * 
 * Returns: a new #GtrView
 */
GtkWidget *
gtr_view_new (void)
{
  GtkWidget *view;

  view = GTK_WIDGET (g_object_new (GTR_TYPE_VIEW, NULL));
  return view;
}

/**
 * gtr_view_get_selected_text:
 * @view: a #GtrView
 * @selected_text: it stores the text selected in the #GtrView
 * @len: it stores the length of the @selected_text
 *
 * Gets the selected text region of the #GtrView
 *
 * Returns: TRUE if the @selected_text was got correctly.
 */
gboolean
gtr_view_get_selected_text (GtrView * view,
                            gchar ** selected_text, gint * len)
{
  GtkTextIter start, end;
  GtkTextBuffer *doc;

  g_return_val_if_fail (selected_text != NULL, FALSE);
  g_return_val_if_fail (*selected_text == NULL, FALSE);
  g_return_val_if_fail (GTR_IS_VIEW (view), FALSE);

  doc = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

  if (!gtk_text_buffer_get_selection_bounds (doc, &start, &end))
    {
      if (len != NULL)
        len = 0;

      return FALSE;
    }

  *selected_text = gtk_text_buffer_get_slice (doc, &start, &end, TRUE);

  if (len != NULL)
    *len = g_utf8_strlen (*selected_text, -1);

  return TRUE;
}

/**
 * gtr_view_enable_visible_whitespace:
 * @view: a #GtrView
 * @enable: TRUE if you want to enable special chars for white spaces
 *
 * Enables special chars for white spaces including \n and \t
**/
void
gtr_view_enable_visible_whitespace (GtrView * view, gboolean enable)
{
  GtkSourceView *source;
  GtkSourceSpaceDrawer *drawer;
  GtkSourceBuffer *buffer;
  g_autoptr (GtkSourceLanguageManager) manager = NULL;
  const gchar * const * deflangs = NULL;
  const gchar *langs[20] = {NULL};
  gint i = 0;

  g_return_if_fail (GTR_IS_VIEW (view));

  manager = gtk_source_language_manager_get_default ();
  deflangs = gtk_source_language_manager_get_search_path (manager);
  langs[1] = deflangs[0];

  while (deflangs[i] && i < 18) {
    langs[i] = deflangs[i];
    i++;
  }
  langs[i] = gtr_dirs_get_gtr_sourceview_dir ();

  manager = gtk_source_language_manager_new ();
  gtk_source_language_manager_set_search_path (manager, (const char * const *)langs);

  source = GTK_SOURCE_VIEW (view);
  drawer = gtk_source_view_get_space_drawer (source);
  gtk_source_space_drawer_set_enable_matrix (drawer, TRUE);
  buffer = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)));
  gtk_source_buffer_set_implicit_trailing_newline (buffer, FALSE);

  gtk_source_buffer_set_highlight_syntax (buffer, TRUE);

  if (enable)
    gtk_source_space_drawer_set_types_for_locations (drawer,
                                                     GTK_SOURCE_SPACE_LOCATION_ALL,
                                                     GTK_SOURCE_SPACE_TYPE_ALL);
  else
    gtk_source_space_drawer_set_types_for_locations (drawer,
                                                     GTK_SOURCE_SPACE_LOCATION_NONE,
                                                     GTK_SOURCE_SPACE_TYPE_NONE);
}

/**
 * gtr_view_cut_clipboard:
 * @view: a #GtrView
 *
 * Copies the currently-selected text to a clipboard,
 * then deletes said text if it's editable.
 */
void
gtr_view_cut_clipboard (GtrView * view)
{
  GtkTextBuffer *buffer;
  GdkClipboard *clipboard;

  g_return_if_fail (GTR_IS_VIEW (view));

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
  g_return_if_fail (buffer != NULL);

  clipboard = gtk_widget_get_clipboard (GTK_WIDGET (view));
                                        //GDK_SELECTION_CLIPBOARD);

  /* FIXME: what is default editability of a buffer? */
  gtk_text_buffer_cut_clipboard (buffer,
                                 clipboard,
                                 gtk_text_view_get_editable (GTK_TEXT_VIEW
                                                             (view)));

  gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (view),
                                gtk_text_buffer_get_insert (buffer),
                                0.0, FALSE, 0.0, 0.0);
}

/**
 * gtr_view_copy_clipboard:
 * @view: a #GtrView
 *
 * Copies the currently-selected text to a clipboard.
 */
void
gtr_view_copy_clipboard (GtrView * view)
{
  GtkTextBuffer *buffer;
  GdkClipboard *clipboard;

  g_return_if_fail (GTR_IS_VIEW (view));

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
  g_return_if_fail (buffer != NULL);

  clipboard = gtk_widget_get_clipboard (GTK_WIDGET (view));
                                        //GDK_SELECTION_CLIPBOARD);

  gtk_text_buffer_copy_clipboard (buffer, clipboard);

  /* on copy do not scroll, we are already on screen */
}

/**
 * gtr_view_paste_clipboard:
 * @view: a #GtrView
 *
 * Pastes the contents of a clipboard at the insertion point,
 * or at override_location.
 */
void
gtr_view_paste_clipboard (GtrView * view)
{
  GtkTextBuffer *buffer;
  GdkClipboard *clipboard;

  g_return_if_fail (GTR_IS_VIEW (view));

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
  g_return_if_fail (buffer != NULL);

  clipboard = gtk_widget_get_clipboard (GTK_WIDGET (view));
                                        //GDK_SELECTION_CLIPBOARD);

  /* FIXME: what is default editability of a buffer? */
  gtk_text_buffer_paste_clipboard (buffer,
                                   clipboard,
                                   NULL,
                                   gtk_text_view_get_editable (GTK_TEXT_VIEW
                                                               (view)));

  gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (view),
                                gtk_text_buffer_get_insert (buffer),
                                0.0, FALSE, 0.0, 0.0);
}


/**
 * gtr_view_set_search_text:
 * @view: a #GtrView
 * @text: the text to set for searching
 * @flags: a #GtrSearchFlags
 *
 * Stores the text to search for in the @view with some specific @flags.
 */
void
gtr_view_set_search_text (GtrView * view, const gchar * text, guint flags)
{
  GtrViewPrivate *priv;
  gchar *converted_text;

  g_return_if_fail (GTR_IS_VIEW (view));

  priv = gtr_view_get_instance_private (view);
  g_return_if_fail ((text == NULL) || (priv->search_text != text));
  g_return_if_fail ((text == NULL) || g_utf8_validate (text, -1, NULL));

  //gedit_debug_message (DEBUG_DOCUMENT, "text = %s", text);

  if (text != NULL)
    {
      if (*text != '\0')
        {
          converted_text = gtr_utils_unescape_search_text (text);
        }
      else
        {
          converted_text = g_strdup ("");
        }

      g_free (priv->search_text);

      priv->search_text = converted_text;
      //priv->num_of_lines_search_text = compute_num_of_lines (priv->search_text);
      //update_to_search_region = TRUE;
    }

  if (!GTR_SEARCH_IS_DONT_SET_FLAGS (flags))
    {
      /*if (priv->search_flags != flags)
         update_to_search_region = TRUE; */

      priv->search_flags = flags;

    }

  /*if (update_to_search_region)
     {
     GtkTextIter begin;
     GtkTextIter end;

     gtk_text_buffer_get_bounds (GTK_TEXT_BUFFER (doc),
     &begin,
     &end);

     to_search_region_range (doc,
     &begin,
     &end);
     } */
}

/**
 * gtr_view_get_search_text:
 * @view: a #GtrView
 * @flags: the #GtrSearchFlags of the stored text.
 * 
 * Returns the text to search for it and the #GtrSearchFlags of that
 * text.
 * 
 * Returns: the text to search for it.
 */
gchar *
gtr_view_get_search_text (GtrView * view, guint * flags)
{
  GtrViewPrivate *priv;

  g_return_val_if_fail (GTR_IS_VIEW (view), NULL);

  priv = gtr_view_get_instance_private (view);

  if (flags != NULL)
    *flags = priv->search_flags;

  return gtr_utils_escape_search_text (priv->search_text);
}

/**
 * gtr_view_get_can_search_again:
 * @view: a #GtrView
 * 
 * Returns: TRUE if it can search again
 */
gboolean
gtr_view_get_can_search_again (GtrView * view)
{
  GtrViewPrivate *priv;

  g_return_val_if_fail (GTR_IS_VIEW (view), FALSE);

  priv = gtr_view_get_instance_private (view);
  return ((priv->search_text != NULL) &&
          (*priv->search_text != '\0'));
}

/**
 * gtr_view_search_forward:
 * @view: a #GtrView
 * @start: start of search 
 * @end: bound for the search, or %NULL for the end of the buffer
 * @match_start: return location for start of match, or %NULL
 * @match_end: return location for end of match, or %NULL
 * 
 * Searches forward for str. Any match is returned by setting match_start to the
 * first character of the match and match_end to the first character after the match.
 * The search will not continue past limit.
 * Note that a search is a linear or O(n) operation, so you may wish to use limit
 * to avoid locking up your UI on large buffers. 
 * 
 * Returns: whether a match was found
 */
gboolean
gtr_view_search_forward (GtrView * view,
                         const GtkTextIter * start,
                         const GtkTextIter * end,
                         GtkTextIter * match_start, GtkTextIter * match_end)
{
  GtkSourceBuffer *doc;
  GtkTextIter iter;
  GtkTextSearchFlags search_flags;
  gboolean found = FALSE;
  GtkTextIter m_start;
  GtkTextIter m_end;
  GtrViewPrivate *priv;

  g_return_val_if_fail (GTR_IS_VIEW (view), FALSE);

  doc = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)));

  g_return_val_if_fail ((start == NULL) ||
                        (gtk_text_iter_get_buffer (start) ==
                         GTK_TEXT_BUFFER (doc)), FALSE);
  g_return_val_if_fail ((end == NULL)
                        || (gtk_text_iter_get_buffer (end) ==
                            GTK_TEXT_BUFFER (doc)), FALSE);

  priv = gtr_view_get_instance_private (view);

  if (priv->search_text == NULL)
    {
      //gedit_debug_message (DEBUG_DOCUMENT, "doc->priv->search_text == NULL\n");
      return FALSE;
    }
  /*else
     gedit_debug_message (DEBUG_DOCUMENT, "doc->priv->search_text == \"%s\"\n", doc->priv->search_text); */

  if (start == NULL)
    gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (doc), &iter);
  else
    iter = *start;

  search_flags = GTK_TEXT_SEARCH_VISIBLE_ONLY | GTK_TEXT_SEARCH_TEXT_ONLY;

  if (!GTR_SEARCH_IS_CASE_SENSITIVE (priv->search_flags))
    {
      search_flags = search_flags | GTK_TEXT_SEARCH_CASE_INSENSITIVE;
    }

  while (!found)
    {
      found = gtk_text_iter_forward_search (&iter,
                                            priv->search_text,
                                            search_flags,
                                            &m_start, &m_end, end);

      if (found && GTR_SEARCH_IS_ENTIRE_WORD (priv->search_flags))
        {
          found = gtk_text_iter_starts_word (&m_start) &&
            gtk_text_iter_ends_word (&m_end);

          if (!found)
            iter = m_end;
        }
      else
        break;
    }

  if (found && (match_start != NULL))
    *match_start = m_start;

  if (found && (match_end != NULL))
    *match_end = m_end;

  return found;
}

/**
 * gtr_view_search_backward:
 * @view: a #GtrView
 * @start: start of search 
 * @end: bound for the search, or %NULL for the end of the buffer
 * @match_start: return location for start of match, or %NULL
 * @match_end: return location for end of match, or %NULL
 * 
 * Searches backward for str. Any match is returned by setting match_start to the
 * first character of the match and match_end to the first character after the match.
 * The search will not continue past limit.
 * Note that a search is a linear or O(n) operation, so you may wish to use limit
 * to avoid locking up your UI on large buffers. 
 * 
 * Returns: whether a match was found
 */
gboolean
gtr_view_search_backward (GtrView * view,
                          const GtkTextIter * start,
                          const GtkTextIter * end,
                          GtkTextIter * match_start, GtkTextIter * match_end)
{
  GtkSourceBuffer *doc;
  GtkTextIter iter;
  GtkTextSearchFlags search_flags;
  gboolean found = FALSE;
  GtkTextIter m_start;
  GtkTextIter m_end;
  GtrViewPrivate *priv;

  g_return_val_if_fail (GTR_IS_VIEW (view), FALSE);

  doc = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)));

  g_return_val_if_fail ((start == NULL) ||
                        (gtk_text_iter_get_buffer (start) ==
                         GTK_TEXT_BUFFER (doc)), FALSE);
  g_return_val_if_fail ((end == NULL)
                        || (gtk_text_iter_get_buffer (end) ==
                            GTK_TEXT_BUFFER (doc)), FALSE);

  priv = gtr_view_get_instance_private (view);

  if (priv->search_text == NULL)
    {
      //gedit_debug_message (DEBUG_DOCUMENT, "doc->priv->search_text == NULL\n");
      return FALSE;
    }
  /*else
     gedit_debug_message (DEBUG_DOCUMENT, "doc->priv->search_text == \"%s\"\n", doc->priv->search_text); */

  if (end == NULL)
    gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (doc), &iter);
  else
    iter = *end;

  search_flags = GTK_TEXT_SEARCH_VISIBLE_ONLY | GTK_TEXT_SEARCH_TEXT_ONLY;

  if (!GTR_SEARCH_IS_CASE_SENSITIVE (priv->search_flags))
    {
      search_flags = search_flags | GTK_TEXT_SEARCH_CASE_INSENSITIVE;
    }

  while (!found)
    {
      found = gtk_text_iter_backward_search (&iter,
                                             priv->search_text,
                                             search_flags,
                                             &m_start, &m_end, start);

      if (found && GTR_SEARCH_IS_ENTIRE_WORD (priv->search_flags))
        {
          found = gtk_text_iter_starts_word (&m_start) &&
            gtk_text_iter_ends_word (&m_end);

          if (!found)
            iter = m_start;
        }
      else
        break;
    }

  if (found && (match_start != NULL))
    *match_start = m_start;

  if (found && (match_end != NULL))
    *match_end = m_end;

  return found;
}

/**
 * gtr_view_replace_all:
 * @view: a #GtrView
 * @find: the text to find
 * @replace: the text to replace @find
 * @flags: a #GtrSearchFlags
 * 
 * Replaces all matches of @find with @replace and returns the number of 
 * replacements.
 * 
 * Returns: the number of replacements made it.
 */
gint
gtr_view_replace_all (GtrView * view,
                      const gchar * find, const gchar * replace, guint flags)
{
  GtkTextIter iter;
  GtkTextIter m_start;
  GtkTextIter m_end;
  GtkTextSearchFlags search_flags = 0;
  GtrViewPrivate *priv;
  gboolean found = TRUE;
  gint cont = 0;
  gchar *search_text;
  gchar *replace_text;
  gint replace_text_len;
  GtkTextBuffer *buffer;

  g_return_val_if_fail (GTR_IS_VIEW (view), 0);

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

  priv = gtr_view_get_instance_private (view);

  g_return_val_if_fail (replace != NULL, 0);
  g_return_val_if_fail ((find != NULL)
                        || (priv->search_text != NULL), 0);

  if (find == NULL)
    search_text = g_strdup (priv->search_text);
  else
    search_text = gtr_utils_unescape_search_text (find);

  replace_text = gtr_utils_unescape_search_text (replace);

  gtk_text_buffer_get_start_iter (buffer, &iter);

  search_flags = GTK_TEXT_SEARCH_VISIBLE_ONLY | GTK_TEXT_SEARCH_TEXT_ONLY;

  if (!GTR_SEARCH_IS_CASE_SENSITIVE (flags))
    {
      search_flags = search_flags | GTK_TEXT_SEARCH_CASE_INSENSITIVE;
    }

  replace_text_len = strlen (replace_text);

  /* disable cursor_moved emission until the end of the
   * replace_all so that we don't spend all the time
   * updating the position in the statusbar
   */
  //priv->stop_cursor_moved_emission = TRUE;

  gtk_text_buffer_begin_user_action (buffer);

  do
    {
      found = gtk_text_iter_forward_search (&iter,
                                            search_text,
                                            search_flags,
                                            &m_start, &m_end, NULL);

      if (found && GTR_SEARCH_IS_ENTIRE_WORD (flags))
        {
          gboolean word;

          word = gtk_text_iter_starts_word (&m_start) &&
            gtk_text_iter_ends_word (&m_end);

          if (!word)
            {
              iter = m_end;
              continue;
            }
        }

      if (found)
        {
          ++cont;

          gtk_text_buffer_delete (buffer, &m_start, &m_end);
          gtk_text_buffer_insert (buffer,
                                  &m_start, replace_text, replace_text_len);

          iter = m_start;
        }

    }
  while (found);

  gtk_text_buffer_end_user_action (buffer);

  /* re-enable cursor_moved emission and notify
   * the current position 
   */
  //priv->stop_cursor_moved_emission = FALSE;
  //emit_cursor_moved (GTK_SOURCE_BUFFER(buffer));

  g_free (search_text);
  g_free (replace_text);

  return cont;
}

/**
 * gtr_view_reload_scheme_color:
 * @view: a #GtrView
 *
 * Reloads the gtksourceview scheme color. Necessary when the scheme color
 * changes.
 */
void
gtr_view_reload_scheme_color (GtrView * view)
{
  GtkSourceBuffer *buf;
  GtkSourceStyleScheme *scheme;
  GtkSourceStyleSchemeManager *manager;
  const gchar *scheme_id;
  AdwStyleManager *style_manager;

  style_manager = adw_style_manager_get_default ();
  buf = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)));
  manager = gtk_source_style_scheme_manager_get_default ();

  if (adw_style_manager_get_dark (style_manager))
    scheme_id = "Adwaita-dark";
  else
    scheme_id = "Adwaita";

  scheme = gtk_source_style_scheme_manager_get_scheme (manager, scheme_id);

  gtk_source_buffer_set_style_scheme (buf, scheme);
}

void
gtr_view_set_font (GtrView *view, char *font)
{
  g_autofree char *str = NULL;
  g_autofree char *css = NULL;
  g_autoptr(GtkFontDialog) dialog = gtk_font_dialog_new ();
  g_autoptr(PangoFontDescription) font_desc = NULL;

  GtrViewPrivate *priv = gtr_view_get_instance_private (view);
  GtkWidget *button = gtk_font_dialog_button_new (dialog);

  font_desc = pango_font_description_from_string (font);
  gtk_font_dialog_button_set_font_desc (GTK_FONT_DIALOG_BUTTON (button), font_desc);

  str = pango_font_description_to_css (font_desc);
  css = g_strdup_printf ("textview  %s", str ?: "");

  gtk_css_provider_load_from_string (priv->provider, css);
}

void
gtr_view_set_lang (GtrView *view, const char *lang)
{
#ifdef LIBSPELL
  GtrViewPrivate *priv;

  priv = gtr_view_get_instance_private (view);
  spelling_checker_set_language (priv->checker, lang);
#endif
}
