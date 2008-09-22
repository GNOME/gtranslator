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

#include "draw-spaces.h"
#include "prefs-manager.h"
#include "utils.h"
#include "view.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourceiter.h>
#include <gtksourceview/gtksourcebuffer.h>

//#undef HAVE_GTKSPELL
#ifdef HAVE_GTKSPELL
#include <gtkspell/gtkspell.h>
#endif

#undef HAVE_SPELL_CHECK
#ifdef HAVE_SPELL_CHECK
#include <gtkspellcheck/client.h>
#include <gtkspellcheck/manager.h>
#include <gtkspellcheck/textviewclient.h>
#endif

#define GTR_VIEW_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 	(object),	\
						 	GTR_TYPE_VIEW,     \
						 	GtranslatorViewPrivate))

G_DEFINE_TYPE(GtranslatorView, gtranslator_view, GTK_TYPE_SOURCE_VIEW)

struct _GtranslatorViewPrivate
{
	GtkSourceBuffer *buffer;
	
	guint        search_flags;
	gchar       *search_text;
	
#ifdef HAVE_GTKSPELL
	GtkSpell *spell;
#endif
	
#ifdef HAVE_SPELL_CHECK
	GtkSpellCheckClient *client;
	GtkSpellCheckManager *manager;
#endif
};


#ifdef HAVE_GTKSPELL
static void
gtranslator_attach_gtkspell(GtranslatorView *view)
{
	gint i;
	GError *error = NULL;
	gchar *errortext = NULL;
	view->priv->spell = NULL;
	
	view->priv->spell = 
		gtkspell_new_attach(GTK_TEXT_VIEW(view), NULL, &error);
	if (view->priv->spell == NULL) 
	{
		g_warning(_("gtkspell error: %s\n"), error->message);
		errortext = g_strdup_printf(_("GtkSpell was unable to initialize.\n %s"),
					    error->message);
		g_warning(errortext);
		
		g_error_free(error);
		g_free(errortext);
	}
}
#endif

#ifdef HAVE_SPELL_CHECK
static void
gtranslator_attach_spellcheck(GtranslatorView *view)
{
	view->priv->client = GTK_SPELL_CHECK_CLIENT(gtk_spell_check_text_view_client_new(GTK_TEXT_VIEW(view)));
	view->priv->manager = gtk_spell_check_manager_new(NULL, TRUE);
	
	gtk_spell_check_manager_attach(view->priv->manager,
				       view->priv->client);
}
#endif

	       
static void
gtranslator_view_init (GtranslatorView *view)
{
	GtkSourceLanguageManager *lm;
	GtkSourceLanguage *lang;
	GPtrArray *dirs;
	gchar **langs;
	const gchar * const *temp;
	gint i;
	
	view->priv = GTR_VIEW_GET_PRIVATE (view);
	
	GtranslatorViewPrivate *priv = view->priv;
	
	lm = gtk_source_language_manager_new();
	dirs = g_ptr_array_new();
	
	for(temp = gtk_source_language_manager_get_search_path(lm);
	    temp != NULL && *temp != NULL;
	    ++temp)
		g_ptr_array_add(dirs, g_strdup(*temp));
		
	g_ptr_array_add(dirs, g_strdup(DATADIR));
	g_ptr_array_add(dirs, NULL);
	langs = (gchar **)g_ptr_array_free(dirs, FALSE);

	
	gtk_source_language_manager_set_search_path(lm, langs);
	lang = gtk_source_language_manager_get_language(lm, "gtranslator");
	g_strfreev(langs);
		
	priv->buffer = gtk_source_buffer_new_with_language(lang);
	
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(view), GTK_TEXT_BUFFER(priv->buffer));
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD);
	
	//Set syntax highlight according to preferences
	gtk_source_buffer_set_highlight_syntax(priv->buffer, gtranslator_prefs_manager_get_highlight());
	
	//Set dot char according to preferences
	
	if(gtranslator_prefs_manager_get_visible_whitespace())
		gtranslator_view_enable_visible_whitespace(view, TRUE);
	
	/*
	 *  Set fonts according to preferences 
	 */
	if (gtranslator_prefs_manager_get_use_custom_font ())
	{
		gchar *editor_font;

		editor_font = g_strdup(gtranslator_prefs_manager_get_editor_font ());

		gtranslator_view_set_font (view, FALSE, editor_font);

		g_free (editor_font);
	}
	else
	{
		gtranslator_view_set_font (view, TRUE, NULL);
	}
}

static void
gtranslator_view_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_view_parent_class)->finalize (object);
}

static void
gtranslator_view_class_init (GtranslatorViewClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorViewPrivate));

	object_class->finalize = gtranslator_view_finalize;
}

/**
 * gtranslator_view_new:
 *
 * Creates a new #GtranslatorView. An empty default buffer will be created for you.
 * 
 * Returns: a new #GtranslatorView
 */
GtkWidget *
gtranslator_view_new (void)
{
	GtkWidget *view;
	
	view = GTK_WIDGET (g_object_new (GTR_TYPE_VIEW, NULL));
	return view;
}

/**
 * gtranslator_view_get_selected_text:
 * @view: a #GtranslatorView
 * @selected_text: it stores the text selected in the #GtranslatorView
 * @len: it stores the length of the @selected_text
 *
 * Gets the selected text region of the #GtranslatorView
 *
 * Returns: TRUE if the @selected_text was got correctly.
 */
gboolean
gtranslator_view_get_selected_text (GtranslatorView *view,
				    gchar         **selected_text,
				    gint           *len)
{
	GtkTextIter start, end;
	GtkTextBuffer *doc;

	g_return_val_if_fail (selected_text != NULL, FALSE);
	g_return_val_if_fail (*selected_text == NULL, FALSE);
	g_return_val_if_fail (GTR_IS_VIEW(view), FALSE);

	doc = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
	
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
 * gtranslator_view_enable_spellcheck:
 * @view: a #GtranslatorView
 * @enable: TRUE if you want enable the spellcheck
 * 
 * Enables the spellcheck
 **/
void
gtranslator_view_enable_spellcheck(GtranslatorView *view,
				    gboolean enable)
{
	if(enable)
	{
#ifdef HAVE_GTKSPELL
		gtranslator_attach_gtkspell(view);
#endif
#ifdef HAVE_SPELL_CHECK
		if(!view->priv->manager)
			gtranslator_attach_spellcheck(view);
		else
			gtk_spell_check_manager_set_active(view->priv->manager,
							   TRUE);
#endif
	}
	else
	{
#ifdef HAVE_GTKSPELL
		if(!view->priv->spell)
			return;
		gtkspell_detach(view->priv->spell);
#endif
#ifdef HAVE_SPELL_CHECK
		if(!view->priv->manager)
			return;
		gtk_spell_check_manager_set_active(view->priv->manager,
						   FALSE);
#endif
	}
}

/**
 * gtranslator_view_enable_visible_whitespace:
 * @view: a #GtranslatorView
 * @enable: TRUE if you want to enable special chars for white spaces
 *
 * Enables special chars for white spaces including \n and \t
**/
void
gtranslator_view_enable_visible_whitespace(GtranslatorView *view,
					   gboolean enable)
{
	g_return_if_fail(GTR_IS_VIEW(view));
	
	if(enable)
		g_signal_connect(view, "event-after",
				 G_CALLBACK(on_event_after), NULL);
	else
		g_signal_handlers_disconnect_by_func(view,
						     G_CALLBACK(on_event_after),
						     NULL);
	
	/*It's neccessary redraw the widget when you connect or disconnect the signal*/
	gtk_widget_queue_draw (GTK_WIDGET (view));
}

/**
 * gtranslator_view_cut_clipboard:
 * @view: a #GtranslatorView
 *
 * Copies the currently-selected text to a clipboard,
 * then deletes said text if it's editable.
 */
void
gtranslator_view_cut_clipboard (GtranslatorView *view)
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
				       gtk_text_view_get_editable(
						GTK_TEXT_VIEW (view)));
  	
	gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (view),
				      gtk_text_buffer_get_insert (buffer),
				      0.0,
				      FALSE,
				      0.0,
				      0.0);
}

/**
 * gtranslator_view_copy_clipboard:
 * @view: a #GtranslatorView
 *
 * Copies the currently-selected text to a clipboard.
 */
void
gtranslator_view_copy_clipboard (GtranslatorView *view)
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
 * gtranslator_view_cut_clipboard:
 * @view: a #GtranslatorView
 *
 * Pastes the contents of a clipboard at the insertion point,
 * or at override_location.
 */
void
gtranslator_view_paste_clipboard (GtranslatorView *view)
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
					 gtk_text_view_get_editable(
						GTK_TEXT_VIEW (view)));

	gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (view),
				      gtk_text_buffer_get_insert (buffer),
				      0.0,
				      FALSE,
				      0.0,
				      0.0);
}

/**
 * gtranslator_view_set_font:
 * @view: a #GtranslatorView
 * @def: TRUE if you want to use the default font
 * @font_name: The name of the font you want to use in the #GtranslatorView
 * 
 * Sets the #GtranslatorView font.
 **/
void
gtranslator_view_set_font (GtranslatorView *view, 
			   gboolean     def, 
			   const gchar *font_name)
{
	PangoFontDescription *font_desc = NULL;

	g_return_if_fail (GTR_IS_VIEW (view));

	if (def)
		font_name = g_strdup(GPM_DEFAULT_EDITOR_FONT);

	g_return_if_fail (font_name != NULL);

	font_desc = pango_font_description_from_string (font_name);
	g_return_if_fail (font_desc != NULL);

	gtk_widget_modify_font (GTK_WIDGET (view), font_desc);

	pango_font_description_free (font_desc);	
}


/**
 * gtranslator_view_set_search_text:
 * @view: a #GtranslatorView
 * @text: the text to set for searching
 * @flags: a #GtranslatorSearchFlags
 *
 * Stores the text to search for in the @view with some specific @flags.
 */
void
gtranslator_view_set_search_text (GtranslatorView *view,
				  const gchar   *text,
				  guint          flags)
{
	GtkSourceBuffer *doc;
	gchar *converted_text;
	gboolean notify = FALSE;
	//gboolean update_to_search_region = FALSE;
	
	g_return_if_fail (GTR_IS_VIEW (view));
	g_return_if_fail ((text == NULL) || (view->priv->search_text != text));
	g_return_if_fail ((text == NULL) || g_utf8_validate (text, -1, NULL));

	//gedit_debug_message (DEBUG_DOCUMENT, "text = %s", text);
	doc = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)));

	if (text != NULL)
	{
		if (*text != '\0')
		{
			converted_text = gtranslator_utils_unescape_search_text (text);
			notify = !gtranslator_view_get_can_search_again (view);
		}
		else
		{
			converted_text = g_strdup("");
			notify = gtranslator_view_get_can_search_again (view);
		}
		
		g_free (view->priv->search_text);
	
		view->priv->search_text = converted_text;
		//view->priv->num_of_lines_search_text = compute_num_of_lines (view->priv->search_text);
		//update_to_search_region = TRUE;
	}
	
	if (!GTR_SEARCH_IS_DONT_SET_FLAGS (flags))
	{
		/*if (view->priv->search_flags != flags)
			update_to_search_region = TRUE;*/
			
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
	}*/
	
	if (notify)
		g_object_notify (G_OBJECT (doc), "can-search-again");
}

/**
 * gtranslator_view_get_search_text:
 * @view: a #GtranslatorView
 * @flags: the #GtranslatorSearchFlags of the stored text.
 * 
 * Returns the text to search for it and the #GtranslatorSearchFlags of that
 * text.
 * 
 * Returns: the text to search for it.
 */
gchar *
gtranslator_view_get_search_text (GtranslatorView *view,
				  guint         *flags)
{
	g_return_val_if_fail (GTR_IS_VIEW (view), NULL);

	if (flags != NULL)
		*flags = view->priv->search_flags;

	return gtranslator_utils_escape_search_text (view->priv->search_text);
}

/**
 * gtranslator_view_get_can_search_again:
 * @view: a #GtranslatorView
 * 
 * Returns: TRUE if it can search again
 */
gboolean
gtranslator_view_get_can_search_again (GtranslatorView *view)
{
	g_return_val_if_fail (GTR_IS_VIEW (view), FALSE);

	return ((view->priv->search_text != NULL) && 
	        (*view->priv->search_text != '\0'));
}

/**
 * gtranslator_view_search_forward:
 * @view: a #GtranslatorView
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
gtranslator_view_search_forward (GtranslatorView   *view,
				 const GtkTextIter *start,
				 const GtkTextIter *end,
				 GtkTextIter       *match_start,
				 GtkTextIter       *match_end)
{
	GtkSourceBuffer *doc;
	GtkTextIter iter;
	GtkSourceSearchFlags search_flags;
	gboolean found = FALSE;
	GtkTextIter m_start;
	GtkTextIter m_end;
	
	g_return_val_if_fail (GTR_IS_VIEW (view), FALSE);
	
	doc = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)));
	
	g_return_val_if_fail ((start == NULL) || 
			      (gtk_text_iter_get_buffer (start) ==  GTK_TEXT_BUFFER (doc)), FALSE);
	g_return_val_if_fail ((end == NULL) || 
			      (gtk_text_iter_get_buffer (end) ==  GTK_TEXT_BUFFER (doc)), FALSE);
		
	if (view->priv->search_text == NULL)
	{
		//gedit_debug_message (DEBUG_DOCUMENT, "doc->priv->search_text == NULL\n");
		return FALSE;
	}
	/*else
		gedit_debug_message (DEBUG_DOCUMENT, "doc->priv->search_text == \"%s\"\n", doc->priv->search_text);*/
				      
	if (start == NULL)
		gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (doc), &iter);
	else
		iter = *start;
		
	search_flags = GTK_SOURCE_SEARCH_VISIBLE_ONLY | GTK_SOURCE_SEARCH_TEXT_ONLY;

	if (!GTR_SEARCH_IS_CASE_SENSITIVE (view->priv->search_flags))
	{
		search_flags = search_flags | GTK_SOURCE_SEARCH_CASE_INSENSITIVE;
	}
		
	while (!found)
	{
		found = gtk_source_iter_forward_search (&iter,
							view->priv->search_text, 
							search_flags,
                        	                	&m_start, 
                        	                	&m_end,
                                	               	end);
      	               	
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
 * gtranslator_view_search_backward:
 * @view: a #GtranslatorView
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
gtranslator_view_search_backward (GtranslatorView   *view,
				  const GtkTextIter *start,
				  const GtkTextIter *end,
				  GtkTextIter       *match_start,
				  GtkTextIter       *match_end)
{
	GtkSourceBuffer *doc;
	GtkTextIter iter;
	GtkSourceSearchFlags search_flags;
	gboolean found = FALSE;
	GtkTextIter m_start;
	GtkTextIter m_end;
	
	g_return_val_if_fail (GTR_IS_VIEW (view), FALSE);
	
	doc = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)));
	
	g_return_val_if_fail ((start == NULL) || 
			      (gtk_text_iter_get_buffer (start) ==  GTK_TEXT_BUFFER (doc)), FALSE);
	g_return_val_if_fail ((end == NULL) || 
			      (gtk_text_iter_get_buffer (end) ==  GTK_TEXT_BUFFER (doc)), FALSE);
	
	if (view->priv->search_text == NULL)
	{
		//gedit_debug_message (DEBUG_DOCUMENT, "doc->priv->search_text == NULL\n");
		return FALSE;
	}
	/*else
		gedit_debug_message (DEBUG_DOCUMENT, "doc->priv->search_text == \"%s\"\n", doc->priv->search_text);*/
				      
	if (end == NULL)
		gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (doc), &iter);
	else
		iter = *end;
		
	search_flags = GTK_SOURCE_SEARCH_VISIBLE_ONLY | GTK_SOURCE_SEARCH_TEXT_ONLY;

	if (!GTR_SEARCH_IS_CASE_SENSITIVE (view->priv->search_flags))
	{
		search_flags = search_flags | GTK_SOURCE_SEARCH_CASE_INSENSITIVE;
	}

	while (!found)
	{
		found = gtk_source_iter_backward_search (&iter,
							 view->priv->search_text, 
							 search_flags,
                        	                	 &m_start, 
                        	                	 &m_end,
                                	               	 start);
      	               	
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
 * gtranslator_view_replace_all:
 * @view: a #GtranslatorView
 * @find: the text to find
 * @replace: the text to replace @find
 * @flags: a #GtranslatorSearchFlags
 * 
 * Replaces all matches of @find with @replace and returns the number of 
 * replacements.
 * 
 * Returns: the number of replacements made it.
 */
gint 
gtranslator_view_replace_all (GtranslatorView     *view,
			      const gchar         *find, 
			      const gchar         *replace, 
			      guint                flags)
{
	GtkTextIter iter;
	GtkTextIter m_start;
	GtkTextIter m_end;
	GtkSourceSearchFlags search_flags = 0;
	gboolean found = TRUE;
	gint cont = 0;
	gchar *search_text;
	gchar *replace_text;
	gint replace_text_len;
	GtkTextBuffer *buffer;

	g_return_val_if_fail (GTR_IS_VIEW (view), 0);
	
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (view));
	
	g_return_val_if_fail (replace != NULL, 0);
	g_return_val_if_fail ((find != NULL) || (view->priv->search_text != NULL), 0);

	if (find == NULL)
		search_text = g_strdup (view->priv->search_text);
	else
		search_text = gtranslator_utils_unescape_search_text (find);

	replace_text = gtranslator_utils_unescape_search_text (replace);

	gtk_text_buffer_get_start_iter (buffer, &iter);

	search_flags = GTK_SOURCE_SEARCH_VISIBLE_ONLY | GTK_SOURCE_SEARCH_TEXT_ONLY;

	if (!GTR_SEARCH_IS_CASE_SENSITIVE (flags))
	{
		search_flags = search_flags | GTK_SOURCE_SEARCH_CASE_INSENSITIVE;
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
		found = gtk_source_iter_forward_search (&iter,
							search_text, 
							search_flags,
                        	                	&m_start, 
                        	                	&m_end,
                                	               	NULL);

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

			gtk_text_buffer_delete (buffer, 
						&m_start,
						&m_end);
			gtk_text_buffer_insert (buffer,
						&m_start,
						replace_text,
						replace_text_len);

			iter = m_start;
		}		

	} while (found);

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
