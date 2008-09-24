/*
 * This file is part of gtranslator
 *
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * Copyright (C) 1998, 1999 Alex Roberts, Evan Lawrence
 * Copyright (C) 2000, 2001 Chema Celorio, Paolo Maggi
 * Copyright (C) 2002-2006 Paolo Maggi
 *
 * Based in gedit-commands-search.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <glib/gi18n.h>
#include <gdk/gdkkeysyms.h>

#include "actions.h"
#include "msg.h"
#include "statusbar.h"
#include "utils.h"
#include "view.h"
#include "window.h"
#include "dialogs/search-dialog.h"


#define GTR_SEARCH_DIALOG_KEY		"gtranslator-search-dialog-key"
#define GTR_LAST_SEARCH_DATA_KEY	"gtranslator-last-search-data-key"

/* data stored in the document
 * to persist the last searched
 * words and options
 */
typedef struct _LastSearchData LastSearchData;
struct _LastSearchData
{
	gchar *find_text;
	gchar *replace_text;

	gint original_text:1;
	gint translated_text:1;
	gint fuzzy_messages:1;
	gint match_case :1;
	gint entire_word :1;
	gint backwards :1;
	gint wrap_around :1;
};

static void
last_search_data_free (LastSearchData *data)
{
	g_free (data->find_text);
	g_free (data->replace_text);
	g_free (data);
}

static void
last_search_data_set (LastSearchData    *data,
		      GtranslatorSearchDialog *dialog)
{
	const gchar *str;

	str = gtranslator_search_dialog_get_search_text (dialog);
	if (str != NULL && *str != '\0')
	{
		g_free (data->find_text);
		data->find_text = gtranslator_utils_unescape_search_text (str);
	}

	str = gtranslator_search_dialog_get_replace_text (dialog);
	if (str != NULL && *str != '\0')
	{
		g_free (data->replace_text);
		data->replace_text = gtranslator_utils_unescape_search_text (str);
	}

	data->original_text = gtranslator_search_dialog_get_original_text (dialog);
	data->translated_text = gtranslator_search_dialog_get_translated_text (dialog);
	data->fuzzy_messages = gtranslator_search_dialog_get_fuzzy (dialog);
	data->match_case = gtranslator_search_dialog_get_match_case (dialog);
	data->entire_word = gtranslator_search_dialog_get_entire_word (dialog);
	data->backwards = gtranslator_search_dialog_get_backwards (dialog);
	data->wrap_around = gtranslator_search_dialog_get_wrap_around (dialog);
}

static void
search_dialog_set_last_find_text (GtranslatorSearchDialog *dialog,
				  LastSearchData    *data)
{
	if (data->find_text)
		gtranslator_search_dialog_set_search_text (dialog, data->find_text);
}

static void
search_dialog_set_last_replace_text (GtranslatorSearchDialog *dialog,
				     LastSearchData    *data)
{
	if (data->replace_text)
		gtranslator_search_dialog_set_replace_text (dialog, data->replace_text);
}

static void
search_dialog_set_last_options (GtranslatorSearchDialog *dialog,
			        LastSearchData    *data)
{
	gtranslator_search_dialog_set_original_text (dialog, data->original_text);
	gtranslator_search_dialog_set_translated_text (dialog, data->translated_text);
	gtranslator_search_dialog_set_fuzzy (dialog, data->fuzzy_messages);
	gtranslator_search_dialog_set_match_case (dialog, data->match_case);
	gtranslator_search_dialog_set_entire_word (dialog, data->entire_word);
	gtranslator_search_dialog_set_backwards (dialog, data->backwards);
	gtranslator_search_dialog_set_wrap_around (dialog, data->wrap_around);
}

/*
 * Used to get the old search data and store the new values.
 */
static void
restore_last_searched_data (GtranslatorSearchDialog *dialog,
			    GtranslatorTab *tab)
{
	LastSearchData *data;

	data = g_object_get_data (G_OBJECT (tab), GTR_LAST_SEARCH_DATA_KEY);

	if (data == NULL)
	{
		data = g_new0 (LastSearchData, 1);
		last_search_data_set (data, dialog);

		g_object_set_data (G_OBJECT (tab),
				   GTR_LAST_SEARCH_DATA_KEY,
				   data);

		g_object_weak_ref (G_OBJECT (tab),
				   (GWeakNotify) last_search_data_free,
				   data);
	}
	else
	{
		last_search_data_set (data, dialog);
	}
}

/* Use occurences only for Replace All */
static void
phrase_found (GtranslatorWindow *window,
	      gint         occurrences)
{
	GtranslatorStatusbar *statusbar;
	
	statusbar = GTR_STATUSBAR (gtranslator_window_get_statusbar (window));
	
	if (occurrences > 1)
	{
		gtranslator_statusbar_flash_message (statusbar,
						     0,
						     ngettext("Found and replaced %d occurrence",
							      "Found and replaced %d occurrences",
							      occurrences),
						     occurrences);
	}
	else
	{
		if (occurrences == 1)
			gtranslator_statusbar_flash_message (statusbar,
							     0,
							     _("Found and replaced one occurrence"));
		else
			gtranslator_statusbar_flash_message (statusbar,
							     0,
							     " ");
	}
}

static void
phrase_not_found (GtranslatorWindow *window)
{
	GtranslatorStatusbar *status;
	
	status = GTR_STATUSBAR (gtranslator_window_get_statusbar (window));
	gtranslator_statusbar_flash_message (status, 0,
					     _("Phrase not found"));
}

static gboolean
run_search (GtranslatorView   *view,
	    gboolean follow)
{
	GtkSourceBuffer *doc;
	GtkTextIter start_iter;
	GtkTextIter match_start;
	GtkTextIter match_end;
	gboolean found = FALSE;

	g_return_val_if_fail(GTR_IS_VIEW(view), FALSE);
	
	doc = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)));

	if(!follow)
		gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(doc), &start_iter);
	else
		gtk_text_buffer_get_selection_bounds (GTK_TEXT_BUFFER (doc),
						      NULL,
						      &start_iter);
		
	found = gtranslator_view_search_forward (view,
						 &start_iter,
						 NULL,
						 &match_start,
						 &match_end);
	
	if (found)
	{
		gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER (doc),
					      &match_start);

		gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER (doc),
						   "selection_bound",
						   &match_end);

	}
	else
	{
		gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER (doc),
					      &start_iter);
	}
	
	return found;
}

static gboolean
find_in_list(GtranslatorWindow *window,
	     GList *views,
	     gboolean fuzzy,
	     gboolean wrap_around,
	     gboolean search_backwards)
{
	GtranslatorTab *tab = gtranslator_window_get_active_tab(window);
	GtranslatorPo *po = gtranslator_tab_get_po(tab);
	GList *l = gtranslator_po_get_current_message(po);
	GList *current;
	static GList *viewsaux = NULL;
	
	current = l;

	if(viewsaux == NULL)
		viewsaux = views;
	
	/*
	 * Variable used to know when start search in from the beggining of the view
	 */
	static gboolean found = FALSE;
	
	do{
		if(gtranslator_msg_is_fuzzy(GTR_MSG(l->data)) && !fuzzy)
		{
			if(!search_backwards)
			{
				if(l->next == NULL)
				{
					if(!wrap_around)
						return FALSE;
					l = g_list_first(l);
				}
				else l = l->next;
			}
			else{
				if(l->prev == NULL)
				{
					if(!wrap_around)
						return FALSE;
					l = g_list_last(l);
				}
				else l = l->prev;
			}
			gtranslator_tab_message_go_to(tab, l, TRUE, GTR_TAB_MOVE_NONE);
		}
		else{
			while(viewsaux != NULL)
			{
				gboolean aux = found;
				
				found = run_search(GTR_VIEW(viewsaux->data), found);
				if(found)
				{
					gtranslator_tab_message_go_to (tab, l, FALSE, GTR_TAB_MOVE_NONE);
					run_search (GTR_VIEW (viewsaux->data), aux);
					return TRUE;
				}
				viewsaux = viewsaux->next;
			}
			if(!search_backwards)
			{
				if(l->next == NULL)
				{
					if(!wrap_around)
						return FALSE;
					l = g_list_first(l);
				}
				else l = l->next;
			}
			else{
				if(l->prev == NULL)
				{
					if(!wrap_around)
						return FALSE;
					l = g_list_last(l);
				}
				else l = l->prev;
			}
			gtranslator_tab_message_go_to(tab, l, TRUE, GTR_TAB_MOVE_NONE);
			viewsaux = views;
		}
	}while(l != current);
	
	return FALSE;
}

static void
do_find (GtranslatorSearchDialog *dialog,
	 GtranslatorWindow       *window)
{
	GtranslatorTab *tab;
	GList *views, *list;
	gchar *search_text;
	const gchar *entry_text;
	gboolean original_text;
	gboolean translated_text;
	gboolean fuzzy;
	gboolean match_case;
	gboolean entire_word;
	gboolean wrap_around;
	gboolean search_backwards;
	guint flags = 0;
	guint old_flags = 0;
	gboolean found;

	/* Used to store search options */
	tab = gtranslator_window_get_active_tab(window);
	
	entry_text = gtranslator_search_dialog_get_search_text (dialog);

	/* Views where find */
	original_text = gtranslator_search_dialog_get_original_text (dialog);
	translated_text = gtranslator_search_dialog_get_translated_text (dialog);
	fuzzy = gtranslator_search_dialog_get_fuzzy (dialog);
	
	/* Flags */
	match_case = gtranslator_search_dialog_get_match_case (dialog);
	entire_word = gtranslator_search_dialog_get_entire_word (dialog);
	search_backwards = gtranslator_search_dialog_get_backwards (dialog);
	wrap_around = gtranslator_search_dialog_get_wrap_around (dialog);

	if(!original_text && !translated_text && !fuzzy)
		return;
	
	/* Get textviews */
	views = gtranslator_window_get_all_views(window, original_text,
						 translated_text);
	
	g_return_if_fail(views != NULL);
	
	list = views;
	
	GTR_SEARCH_SET_CASE_SENSITIVE (flags, match_case);
	GTR_SEARCH_SET_ENTIRE_WORD (flags, entire_word);
	
	while(list != NULL)
	{
		search_text = gtranslator_view_get_search_text (GTR_VIEW(list->data), &old_flags);

		if ((search_text == NULL) ||
	    	(strcmp (search_text, entry_text) != 0) ||
	    	(flags != old_flags))
		{
			gtranslator_view_set_search_text (GTR_VIEW(list->data), entry_text, flags);
		}

		g_free (search_text);
		list = list->next;
	}
	
	found = find_in_list (window, views, fuzzy,
			      wrap_around, search_backwards);

	if (found)
		phrase_found (window, 0);
	else
		phrase_not_found (window);

	gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog),
					   GTR_SEARCH_DIALOG_REPLACE_RESPONSE,
					   found);

	restore_last_searched_data (dialog, tab);
}

static void
replace_selected_text (GtkTextBuffer *buffer,
		       const gchar   *replace)
{
	g_return_if_fail (gtk_text_buffer_get_selection_bounds (buffer, NULL, NULL));
	g_return_if_fail (replace != NULL);

	gtk_text_buffer_begin_user_action (buffer);

	gtk_text_buffer_delete_selection (buffer, FALSE, TRUE);

	gtk_text_buffer_insert_at_cursor (buffer, replace, strlen (replace));

	gtk_text_buffer_end_user_action (buffer);
}

static void
do_replace (GtranslatorSearchDialog *dialog,
	    GtranslatorWindow       *window)
{
	GtranslatorView *view;
	const gchar *search_entry_text;
	const gchar *replace_entry_text;	
	gchar *unescaped_search_text;
	gchar *unescaped_replace_text;
	gchar *selected_text = NULL;
	gboolean match_case;
	gboolean search_backwards;

	view = gtranslator_window_get_active_view (window);
	if (view == NULL)
		return;

	search_entry_text = gtranslator_search_dialog_get_search_text (dialog);
	g_return_if_fail ((search_entry_text) != NULL);
	g_return_if_fail ((*search_entry_text) != '\0');

	/* replace text may be "", we just delete */
	replace_entry_text = gtranslator_search_dialog_get_replace_text (dialog);
	g_return_if_fail ((replace_entry_text) != NULL);

	unescaped_search_text = gtranslator_utils_unescape_search_text (search_entry_text);

	gtranslator_view_get_selected_text (view, 
					    &selected_text, 
					    NULL);

	match_case = gtranslator_search_dialog_get_match_case (dialog);
	search_backwards = gtranslator_search_dialog_get_backwards (dialog);

	if ((selected_text == NULL) ||
	    (match_case && (strcmp (selected_text, unescaped_search_text) != 0)) || 
	    (!match_case && !g_utf8_caselessnmatch (selected_text,
						    unescaped_search_text, 
						    strlen (selected_text), 
						    strlen (unescaped_search_text)) != 0))
	{
		do_find (dialog, window);
		g_free (unescaped_search_text);
		g_free (selected_text);	

		return;
	}

	unescaped_replace_text = gtranslator_utils_unescape_search_text (replace_entry_text);	
	replace_selected_text (gtk_text_view_get_buffer(GTK_TEXT_VIEW (view)),
			       unescaped_replace_text);

	g_free (unescaped_search_text);
	g_free (selected_text);
	g_free (unescaped_replace_text);
	
	do_find (dialog, window);
}

static void
do_replace_all (GtranslatorSearchDialog *dialog,
		GtranslatorWindow       *window)
{
	GtranslatorTab *tab;
	GList *views, *l;
	GList *current_msg, *aux;
	const gchar *search_entry_text;
	const gchar *replace_entry_text;	
	gboolean match_case;
	gboolean entire_word;
	guint flags = 0;
	gint count = 0;

	tab = gtranslator_window_get_active_tab(window);
	
	/* Get only translated textviews */
	views = gtranslator_window_get_all_views(window, FALSE,
						 TRUE);
	
	current_msg = gtranslator_po_get_current_message(gtranslator_tab_get_po(tab));
	
	g_return_if_fail(views != NULL);
	g_return_if_fail(current_msg != NULL);
	
	l = views;
	aux = current_msg;
	
	search_entry_text = gtranslator_search_dialog_get_search_text (dialog);
	g_return_if_fail ((search_entry_text) != NULL);
	g_return_if_fail ((*search_entry_text) != '\0');

	/* replace text may be "", we just delete all occurrencies */
	replace_entry_text = gtranslator_search_dialog_get_replace_text (dialog);
	g_return_if_fail ((replace_entry_text) != NULL);

	match_case = gtranslator_search_dialog_get_match_case (dialog);
	entire_word = gtranslator_search_dialog_get_entire_word (dialog);

	GTR_SEARCH_SET_CASE_SENSITIVE (flags, match_case);
	GTR_SEARCH_SET_ENTIRE_WORD (flags, entire_word);
	
	
	do
	{
		while(l)
		{
			count += gtranslator_view_replace_all (GTR_VIEW(l->data), 
							      search_entry_text,
							      replace_entry_text,
							      flags);
			l = l->next;
		}
		
		l = views;
		
		if(aux->next == NULL)
			aux = g_list_first(aux);
		else aux = aux->next;
		gtranslator_tab_message_go_to(tab, aux, TRUE, GTR_TAB_MOVE_NONE);
	}while(current_msg != aux);

	gtranslator_tab_message_go_to(tab, aux, FALSE, GTR_TAB_MOVE_NONE);
	
	if (count > 0)
	{
		phrase_found (window, count);
	}
	else
	{
		phrase_not_found (window);
	}

	gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog),
					   GTR_SEARCH_DIALOG_REPLACE_RESPONSE,
					   FALSE);

	restore_last_searched_data (dialog, tab);
}

static void
search_dialog_response_cb (GtranslatorSearchDialog *dialog,
			   gint               response_id,
			   GtranslatorWindow       *window)
{
	switch (response_id)
	{
		case GTR_SEARCH_DIALOG_FIND_RESPONSE:
			do_find (dialog, window);
			break;
		case GTR_SEARCH_DIALOG_REPLACE_RESPONSE:
			do_replace (dialog, window);
			break;
		case GTR_SEARCH_DIALOG_REPLACE_ALL_RESPONSE:
			do_replace_all (dialog, window);
			break;
		default:
			gtk_widget_destroy (GTK_WIDGET (dialog));
	}
}

static void
search_dialog_destroyed (GtranslatorWindow       *window,
			 GtranslatorSearchDialog *dialog)
{
	g_object_set_data (G_OBJECT (window),
			   GTR_SEARCH_DIALOG_KEY,
			   NULL);
}

void
_gtranslator_actions_search_find (GtkAction   *action,
				  GtranslatorWindow *window)
{
	gpointer data;
	GtkWidget *search_dialog;
	GtranslatorTab *tab;
	GtranslatorView *view;
	gboolean selection_exists;
	gchar *find_text = NULL;
	gint sel_len;
	LastSearchData *last_search_data;

	data = g_object_get_data (G_OBJECT (window), GTR_SEARCH_DIALOG_KEY);

	if (data != NULL)
	{
		g_return_if_fail (GTR_IS_SEARCH_DIALOG (data));

		/* note that we rise a search&replace dialog if
		 * it is already open
		 */

		gtranslator_search_dialog_present_with_time (GTR_SEARCH_DIALOG (data),
							     GDK_CURRENT_TIME);

		return;
	}

	search_dialog = gtranslator_search_dialog_new (GTK_WINDOW (window),
						 FALSE);

	g_object_set_data (G_OBJECT (window),
			   GTR_SEARCH_DIALOG_KEY,
			   search_dialog);

	g_object_weak_ref (G_OBJECT (search_dialog),
			   (GWeakNotify) search_dialog_destroyed,
			   window);

	tab = gtranslator_window_get_active_tab (window);
	view = gtranslator_window_get_active_view(window);
	g_return_if_fail (tab != NULL);
	g_return_if_fail (view != NULL);

	last_search_data = g_object_get_data (G_OBJECT (tab),
					      GTR_LAST_SEARCH_DATA_KEY);

	selection_exists = gtranslator_view_get_selected_text (view,
							       &find_text,
							       &sel_len);

	if (selection_exists && find_text != NULL && sel_len < 80)
	{
		gtranslator_search_dialog_set_search_text (GTR_SEARCH_DIALOG (search_dialog),
							   find_text);
		g_free (find_text);
	}
	else
	{
		g_free (find_text);

		if (last_search_data != NULL)
			search_dialog_set_last_find_text (GTR_SEARCH_DIALOG (search_dialog),
							  last_search_data);
	}

	if (last_search_data != NULL)
		search_dialog_set_last_replace_text (GTR_SEARCH_DIALOG (search_dialog),
						     last_search_data);

	if (last_search_data != NULL)
		search_dialog_set_last_options (GTR_SEARCH_DIALOG (search_dialog),
						last_search_data);

	g_signal_connect (search_dialog,
			  "response",
			  G_CALLBACK (search_dialog_response_cb),
			  window);

	gtk_widget_show (search_dialog);
}

void
_gtranslator_actions_search_replace (GtkAction   *action,
				 GtranslatorWindow *window)
{
	gpointer data;
	GtkWidget *replace_dialog;
	GtranslatorTab *tab;
	GtranslatorView *view;
	gboolean selection_exists;
	gchar *find_text = NULL;
	gint sel_len;
	LastSearchData *last_search_data;

	data = g_object_get_data (G_OBJECT (window), GTR_SEARCH_DIALOG_KEY);

	if (data != NULL)
	{
		g_return_if_fail (GTR_IS_SEARCH_DIALOG (data));

		/* turn the dialog into a replace dialog if needed */
		if (!gtranslator_search_dialog_get_show_replace (GTR_SEARCH_DIALOG (data)))
			gtranslator_search_dialog_set_show_replace (GTR_SEARCH_DIALOG (data),
							      TRUE);

		gtranslator_search_dialog_present_with_time (GTR_SEARCH_DIALOG (data),
						       GDK_CURRENT_TIME);

		return;
	}

	replace_dialog = gtranslator_search_dialog_new (GTK_WINDOW (window),
							TRUE);

	g_object_set_data (G_OBJECT (window),
			   GTR_SEARCH_DIALOG_KEY,
			   replace_dialog);

	g_object_weak_ref (G_OBJECT (replace_dialog),
			   (GWeakNotify) search_dialog_destroyed,
			   window);

	tab = gtranslator_window_get_active_tab (window);
	view = gtranslator_window_get_active_view(window);
	g_return_if_fail (tab != NULL);

	last_search_data = g_object_get_data (G_OBJECT (tab),
					      GTR_LAST_SEARCH_DATA_KEY);


	selection_exists = gtranslator_view_get_selected_text (view,
							       &find_text,
							       &sel_len);

	if (selection_exists && find_text != NULL && sel_len < 80)
	{
		gtranslator_search_dialog_set_search_text (GTR_SEARCH_DIALOG (replace_dialog),
						     find_text);
		g_free (find_text);
	}
	else
	{
		g_free (find_text);

		if (last_search_data != NULL)
			search_dialog_set_last_find_text (GTR_SEARCH_DIALOG (replace_dialog),
							  last_search_data);
	}

	if (last_search_data != NULL)
		search_dialog_set_last_replace_text (GTR_SEARCH_DIALOG (replace_dialog),
						     last_search_data);

	if (last_search_data != NULL)
		search_dialog_set_last_options (GTR_SEARCH_DIALOG (replace_dialog),
						last_search_data);

	g_signal_connect (replace_dialog,
			  "response",
			  G_CALLBACK (search_dialog_response_cb),
			  window);

	gtk_widget_show (replace_dialog);
}

static void
do_find_again (GtranslatorWindow *window,
	       gboolean     backward)
{
	GtranslatorView *active_view;
	GtkTextBuffer *buffer;
	gboolean wrap_around = TRUE;
	LastSearchData *data;
	
	active_view = gtranslator_window_get_active_view (window);
	g_return_if_fail (active_view != NULL);

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (active_view));

	data = g_object_get_data (G_OBJECT (buffer), 
				  GTR_LAST_SEARCH_DATA_KEY);

	if (data != NULL)
		wrap_around = data->wrap_around;					    

	//run_search (active_view);
}

void
_gtranslator_actions_search_find_next (GtkAction   *action,
				       GtranslatorWindow *window)
{
	do_find_again (window, FALSE);
}

void
_gtranslator_actions_search_find_prev (GtkAction   *action,
				       GtranslatorWindow *window)
{
	do_find_again (window, TRUE);
}
