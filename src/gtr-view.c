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

#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcestyleschememanager.h>

#ifdef HAVE_GTKSPELL
#include <gtkspell/gtkspell.h>
#endif

#define GTR_VIEW_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 	(object),	\
						 	GTR_TYPE_VIEW,     \
						 	GtrViewPrivate))

G_DEFINE_TYPE (GtrView, gtr_view, GTK_TYPE_SOURCE_VIEW)

struct _GtrViewPrivate
{
  GSettings *editor_settings;
  GSettings *ui_settings;

  GtkSourceBuffer *buffer;

  guint search_flags;
  gchar *search_text;

#ifdef HAVE_GTKSPELL
  GtkSpell *spell;
#endif
};

#ifdef HAVE_GTKSPELL
static void
gtr_attach_gtkspell (GtrView * view)
{
  GError *error = NULL;
  gchar *errortext = NULL;
  view->priv->spell = NULL;

  view->priv->spell =
    gtkspell_new_attach (GTK_TEXT_VIEW (view), NULL, &error);
  if (view->priv->spell == NULL)
    {
      g_warning (_("gtkspell error: %s\n"), error->message);
      errortext =
        g_strdup_printf (_("GtkSpell was unable to initialize.\n %s"),
                         error->message);
      g_warning ("%s", errortext);

      g_error_free (error);
      g_free (errortext);
    }
}
#endif

static void
gtr_view_init (GtrView * view)
{
  GtkSourceLanguageManager *lm;
  GtkSourceLanguage *lang;
  GPtrArray *dirs;
  gchar **langs;
  const gchar *const *temp;
  const gchar *datadir;
  GtrViewPrivate *priv;

  view->priv = GTR_VIEW_GET_PRIVATE (view);

  priv = view->priv;

  priv->editor_settings = g_settings_new ("org.gnome.gtranslator.preferences.editor");
  priv->ui_settings = g_settings_new ("org.gnome.gtranslator.preferences.ui");

  lm = gtk_source_language_manager_new ();
  dirs = g_ptr_array_new ();

  for (temp = gtk_source_language_manager_get_search_path (lm);
       temp != NULL && *temp != NULL; ++temp)
    g_ptr_array_add (dirs, g_strdup (*temp));

  datadir = gtr_dirs_get_gtr_data_dir ();
  g_ptr_array_add (dirs, g_strdup (datadir));
  g_ptr_array_add (dirs, NULL);
  langs = (gchar **) g_ptr_array_free (dirs, FALSE);

  gtk_source_language_manager_set_search_path (lm, langs);
  lang = gtk_source_language_manager_get_language (lm, "gtranslator");
  g_strfreev (langs);

  priv->buffer = gtk_source_buffer_new_with_language (lang);

  gtk_text_view_set_buffer (GTK_TEXT_VIEW (view),
                            GTK_TEXT_BUFFER (priv->buffer));
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (view), GTK_WRAP_WORD);

  /* Set syntax highlight according to preferences */
  gtk_source_buffer_set_highlight_syntax (priv->buffer,
                                          g_settings_get_boolean (priv->editor_settings,
                                                                  GTR_SETTINGS_HIGHLIGHT_SYNTAX));

  /* Set dot char according to preferences */
  gtr_view_enable_visible_whitespace (view,
                                      g_settings_get_boolean (priv->editor_settings,
                                                              GTR_SETTINGS_VISIBLE_WHITESPACE));

  /* Set fonts according to preferences */
  if (g_settings_get_boolean (priv->editor_settings, GTR_SETTINGS_USE_CUSTOM_FONT))
    {
      gchar *editor_font;

      editor_font = g_settings_get_string (priv->editor_settings,
                                           GTR_SETTINGS_EDITOR_FONT);

      gtr_view_set_font (view, FALSE, editor_font);

      g_free (editor_font);
    }
  else
    {
      gtr_view_set_font (view, TRUE, NULL);
    }

  /* Set scheme color according to preferences */
  gtr_view_reload_scheme_color (view);
}

static void
gtr_view_dispose (GObject * object)
{
  GtrView *view = GTR_VIEW (object);

  if (view->priv->editor_settings)
    {
      g_object_unref (view->priv->editor_settings);
      view->priv->editor_settings = NULL;
    }

  if (view->priv->ui_settings)
    {
      g_object_unref (view->priv->ui_settings);
      view->priv->ui_settings = NULL;
    }

  G_OBJECT_CLASS (gtr_view_parent_class)->dispose (object);
}

static void
gtr_view_class_init (GtrViewClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrViewPrivate));

  object_class->dispose = gtr_view_dispose;
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
 * gtr_view_enable_spellcheck:
 * @view: a #GtrView
 * @enable: TRUE if you want enable the spellcheck
 * 
 * Enables the spellcheck
 **/
void
gtr_view_enable_spellcheck (GtrView * view, gboolean enable)
{
  if (enable)
    {
#ifdef HAVE_GTKSPELL
      gtr_attach_gtkspell (view);
#endif
    }
  else
    {
#ifdef HAVE_GTKSPELL
      if (!view->priv->spell)
        return;
      gtkspell_detach (view->priv->spell);
#endif
    }
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
  g_return_if_fail (GTR_IS_VIEW (view));

  if (enable)
    gtk_source_view_set_draw_spaces (GTK_SOURCE_VIEW (view),
                                     GTK_SOURCE_DRAW_SPACES_ALL);
  else
    gtk_source_view_set_draw_spaces (GTK_SOURCE_VIEW (view), 0);
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
  GtkClipboard *clipboard;

  g_return_if_fail (GTR_IS_VIEW (view));

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
  g_return_if_fail (buffer != NULL);

  clipboard = gtk_widget_get_clipboard (GTK_WIDGET (view),
                                        GDK_SELECTION_CLIPBOARD);

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
  GtkClipboard *clipboard;

  g_return_if_fail (GTR_IS_VIEW (view));

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
  g_return_if_fail (buffer != NULL);

  clipboard = gtk_widget_get_clipboard (GTK_WIDGET (view),
                                        GDK_SELECTION_CLIPBOARD);

  gtk_text_buffer_copy_clipboard (buffer, clipboard);

  /* on copy do not scroll, we are already on screen */
}

/**
 * gtr_view_cut_clipboard:
 * @view: a #GtrView
 *
 * Pastes the contents of a clipboard at the insertion point,
 * or at override_location.
 */
void
gtr_view_paste_clipboard (GtrView * view)
{
  GtkTextBuffer *buffer;
  GtkClipboard *clipboard;

  g_return_if_fail (GTR_IS_VIEW (view));

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
  g_return_if_fail (buffer != NULL);

  clipboard = gtk_widget_get_clipboard (GTK_WIDGET (view),
                                        GDK_SELECTION_CLIPBOARD);

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
 * gtr_view_set_font:
 * @view: a #GtrView
 * @def: TRUE if you want to use the default font
 * @font_name: The name of the font you want to use in the #GtrView
 * 
 * Sets the #GtrView font.
 **/
void
gtr_view_set_font (GtrView * view, gboolean def, const gchar * font_name)
{
  PangoFontDescription *font_desc = NULL;

  g_return_if_fail (GTR_IS_VIEW (view));

  if (def)
    {
      GSettings *gtr_settings;
      gchar *font;

      gtr_settings = _gtr_application_get_settings (GTR_APP);
      font = gtr_settings_get_system_font (GTR_SETTINGS (gtr_settings));

      font_desc = pango_font_description_from_string (font);
      g_free (font);
    }
  else
    {
      font_desc = pango_font_description_from_string (font_name);
    }

  g_return_if_fail (font_desc != NULL);

  gtk_widget_modify_font (GTK_WIDGET (view), font_desc);

  pango_font_description_free (font_desc);
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
  GtkSourceBuffer *doc;
  gchar *converted_text;
  gboolean notify = FALSE;
  //gboolean update_to_search_region = FALSE;

  g_return_if_fail (GTR_IS_VIEW (view));
  g_return_if_fail ((text == NULL) || (view->priv->search_text != text));
  g_return_if_fail ((text == NULL) || g_utf8_validate (text, -1, NULL));

  //gedit_debug_message (DEBUG_DOCUMENT, "text = %s", text);
  doc = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)));

  if (text != NULL)
    {
      if (*text != '\0')
        {
          converted_text = gtr_utils_unescape_search_text (text);
          notify = !gtr_view_get_can_search_again (view);
        }
      else
        {
          converted_text = g_strdup ("");
          notify = gtr_view_get_can_search_again (view);
        }

      g_free (view->priv->search_text);

      view->priv->search_text = converted_text;
      //view->priv->num_of_lines_search_text = compute_num_of_lines (view->priv->search_text);
      //update_to_search_region = TRUE;
    }

  if (!GTR_SEARCH_IS_DONT_SET_FLAGS (flags))
    {
      /*if (view->priv->search_flags != flags)
         update_to_search_region = TRUE; */

      view->priv->search_flags = flags;

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
  g_return_val_if_fail (GTR_IS_VIEW (view), NULL);

  if (flags != NULL)
    *flags = view->priv->search_flags;

  return gtr_utils_escape_search_text (view->priv->search_text);
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
  g_return_val_if_fail (GTR_IS_VIEW (view), FALSE);

  return ((view->priv->search_text != NULL) &&
          (*view->priv->search_text != '\0'));
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

  g_return_val_if_fail (GTR_IS_VIEW (view), FALSE);

  doc = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)));

  g_return_val_if_fail ((start == NULL) ||
                        (gtk_text_iter_get_buffer (start) ==
                         GTK_TEXT_BUFFER (doc)), FALSE);
  g_return_val_if_fail ((end == NULL)
                        || (gtk_text_iter_get_buffer (end) ==
                            GTK_TEXT_BUFFER (doc)), FALSE);

  if (view->priv->search_text == NULL)
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

  if (!GTR_SEARCH_IS_CASE_SENSITIVE (view->priv->search_flags))
    {
      search_flags = search_flags | GTK_TEXT_SEARCH_CASE_INSENSITIVE;
    }

  while (!found)
    {
      found = gtk_text_iter_forward_search (&iter,
                                            view->priv->search_text,
                                            search_flags,
                                            &m_start, &m_end, end);

      if (found && GTR_SEARCH_IS_ENTIRE_WORD (view->priv->search_flags))
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

  g_return_val_if_fail (GTR_IS_VIEW (view), FALSE);

  doc = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)));

  g_return_val_if_fail ((start == NULL) ||
                        (gtk_text_iter_get_buffer (start) ==
                         GTK_TEXT_BUFFER (doc)), FALSE);
  g_return_val_if_fail ((end == NULL)
                        || (gtk_text_iter_get_buffer (end) ==
                            GTK_TEXT_BUFFER (doc)), FALSE);

  if (view->priv->search_text == NULL)
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

  if (!GTR_SEARCH_IS_CASE_SENSITIVE (view->priv->search_flags))
    {
      search_flags = search_flags | GTK_TEXT_SEARCH_CASE_INSENSITIVE;
    }

  while (!found)
    {
      found = gtk_text_iter_backward_search (&iter,
                                             view->priv->search_text,
                                             search_flags,
                                             &m_start, &m_end, start);

      if (found && GTR_SEARCH_IS_ENTIRE_WORD (view->priv->search_flags))
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
  gboolean found = TRUE;
  gint cont = 0;
  gchar *search_text;
  gchar *replace_text;
  gint replace_text_len;
  GtkTextBuffer *buffer;

  g_return_val_if_fail (GTR_IS_VIEW (view), 0);

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

  g_return_val_if_fail (replace != NULL, 0);
  g_return_val_if_fail ((find != NULL)
                        || (view->priv->search_text != NULL), 0);

  if (find == NULL)
    search_text = g_strdup (view->priv->search_text);
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
  //view->priv->stop_cursor_moved_emission = TRUE;

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
  //view->priv->stop_cursor_moved_emission = FALSE;
  //emit_cursor_moved (GTK_SOURCE_BUFFER(buffer));

  g_free (search_text);
  g_free (replace_text);

  return cont;
}

/**
 * gtr_view_reload_scheme_color:
 * @view: a #GtrView
 *
 * Reloads the gtksourceview scheme color. Neccessary when the scheme color 
 * changes.
 */
void
gtr_view_reload_scheme_color (GtrView * view)
{
  GtkSourceBuffer *buf;
  GtkSourceStyleScheme *scheme;
  GtkSourceStyleSchemeManager *manager;
  gchar *scheme_id;

  buf = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)));
  manager = gtk_source_style_scheme_manager_get_default ();

  scheme_id = g_settings_get_string (view->priv->ui_settings,
                                     GTR_SETTINGS_COLOR_SCHEME);
  scheme = gtk_source_style_scheme_manager_get_scheme (manager, scheme_id);
  g_free (scheme_id);

  gtk_source_buffer_set_style_scheme (buf, scheme);
}
