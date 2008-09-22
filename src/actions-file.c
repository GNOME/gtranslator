/*
 * Copyright (C) 2007   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * 			Fatih Demir <kabalak@kabalak.net>
 *			Ross Golder <ross@golder.org>
 *			Gediminas Paulauskas <menesis@kabalak.net>
			Pablo Sanxiao <psanxiao@gmail.com>
 *
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANMSGILITY or FITNESS FOR A PARTICULAR PURMSGSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <gio/gio.h>
#include <string.h>
#include <gio/gio.h>

#include "actions.h"
#include "application.h"
#include "dialogs/close-confirmation-dialog.h"
#include "file-dialogs.h"
#include "notebook.h"
#include "po.h"
#include "profile.h"
#include "statusbar.h"
#include "tab.h"
#include "window.h"

#define GTR_TAB_SAVE_AS "gtranslator-tab-save-as"
#define GTR_IS_CLOSING_ALL            "gtranslator-is-closing-all"

static void load_file_list(GtranslatorWindow *window,
			   const GSList *uris);


/*
 * The main file opening function. Checks that the file isn't already open,
 * and if not, opens it in a new tab.
 */
gboolean 
gtranslator_open(const gchar *filename,
		 GtranslatorWindow *window,
		 GError **error)
{
	GtranslatorHeader *header;
	GtranslatorPo	*po;
	GtranslatorTab *tab;
	GList *current;
	GtranslatorView *active_view;
	gchar *project_id;

	/*
	 * If the filename can't be opened, pass the error back to the caller
	 * to handle.
	 */
	po = gtranslator_po_new();
	gtranslator_po_parse(po, filename, error);
	
	if((*error != NULL) && (((GError *)*error)->code != GTR_PO_ERROR_RECOVERY))
		return FALSE;

	header = gtranslator_po_get_header(po);
	project_id = gtranslator_header_get_prj_id_version(header);

	/*
	 * If not a crash/temporary file, add to the history.
	 */
	gtranslator_recent_add(window, filename, project_id);

	/*
	 * Create a page to add to our list of open files
	 */
	tab = gtranslator_window_create_tab(window, po);
	
	/*
	 * Show the current message.
	 */
	current = gtranslator_po_get_current_message(po);
	gtranslator_tab_message_go_to (tab, current, FALSE);
	
	/*
	 * Grab the focus
	 */
	active_view = gtranslator_tab_get_active_view(tab);
	gtk_widget_grab_focus(GTK_WIDGET(active_view));
	
	gtranslator_statusbar_update_progress_bar (GTR_STATUSBAR (gtranslator_window_get_statusbar (window)),
						   (gdouble)gtranslator_po_get_translated_count (po),
						   (gdouble)gtranslator_po_get_messages_count (po));
	
	return TRUE;
}

static void 
gtranslator_po_parse_files_from_dialog (GtkWidget * dialog,
					GtranslatorWindow *window)
{
	GSList *po_files;
	GFile *file, *parent;
	gchar *uri;
	
	po_files = gtk_file_chooser_get_uris (GTK_FILE_CHOOSER (dialog));

	/*
	 * We store latest directory
	 */
	file = g_file_new_for_uri (po_files->data);
	parent = g_file_get_parent (file);
	g_object_unref (file);

	uri = g_file_get_uri (parent);
	g_object_unref (parent);
	_gtranslator_application_set_last_dir (GTR_APP,
					       uri);
	
	g_free (uri);

	/*
	 * Open the file via our centralized opening function.
	 */
	load_file_list (window, (const GSList *)po_files);

	g_slist_free (po_files);
	
	/*
	 * Destroy the dialog 
	 */
	gtk_widget_destroy (dialog);
}


static void
gtranslator_file_chooser_analyse(gpointer dialog,
				 FileselMode mode,
				 GtranslatorWindow *window)
{	
	gint reply;

	reply = gtk_dialog_run(GTK_DIALOG (dialog));
	switch (reply){
		case GTK_RESPONSE_ACCEPT:
			if (mode == FILESEL_OPEN){
				gtranslator_po_parse_files_from_dialog(GTK_WIDGET(dialog),
								       window);
			}
			break;
		case GTK_RESPONSE_CANCEL:
			gtk_widget_hide(GTK_WIDGET(dialog));
			break;
		case GTK_RESPONSE_DELETE_EVENT:
			gtk_widget_hide(GTK_WIDGET(dialog));
			break;
		default:
			break;
	}
}


/*
 * The "Open file" dialog.
 */
void
gtranslator_open_file_dialog(GtkAction * action,
			     GtranslatorWindow *window)
{
	GtkWidget *dialog = NULL;

	if(dialog != NULL) {
		gtk_window_present(GTK_WINDOW(dialog));
		return;
	}
	dialog = gtranslator_file_chooser_new (GTK_WINDOW(window), 
					       FILESEL_OPEN,
					       _("Open file for translation"),
					       _gtranslator_application_get_last_dir (GTR_APP));	
	
	/*
	 * With the gettext parser/writer API, we can't currently read/write
	 * to remote files with gnome-vfs. Eventually, we should intercept
	 * remote requests and use gnome-vfs to retrieve a temporary file to 
	 * work on, and transmit it back when saved.
	 */
	//gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(dialog), TRUE);

	gtranslator_file_chooser_analyse((gpointer) dialog, FILESEL_OPEN, window);
}

static void
save_dialog_response_cb (GtkDialog *dialog,
                         gint                    response_id,
                         GtranslatorWindow      *window)
{
	GError *error = NULL;
	GtranslatorPo *po;
	GtranslatorTab *tab;
	gchar *filename;
	
	tab = GTR_TAB (g_object_get_data (G_OBJECT (dialog),
					  GTR_TAB_SAVE_AS));
				
	g_return_if_fail (GTK_IS_FILE_CHOOSER (dialog));
				     
	po = gtranslator_tab_get_po (tab);

	if (response_id != GTK_RESPONSE_ACCEPT)
	{
		gtk_widget_destroy (GTK_WIDGET (dialog));
		return;
	}
		
	filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
	g_return_if_fail (filename != NULL);
	
	gtk_widget_destroy (GTK_WIDGET (dialog));
	
	if (po != NULL)
	{
		gtranslator_po_set_filename (po, filename);
		
		gtranslator_po_save_file (po, &error);
	
		if (error)
		{
			GtkWidget *dialog;
			dialog = gtk_message_dialog_new (GTK_WINDOW (window),
							 GTK_DIALOG_DESTROY_WITH_PARENT,
							 GTK_MESSAGE_WARNING,
							 GTK_BUTTONS_OK,
							 error->message);
			gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);
			g_clear_error(&error);
			g_free (filename);
			return;
		}
	
		/* We have to change the state of the tab */
		gtranslator_po_set_state(po, GTR_PO_STATE_SAVED);	
			
		g_free (filename);
	}
}

static GtkFileChooserConfirmation
confirm_overwrite_callback (GtkFileChooser *dialog,
			    gpointer        data)
{
	gchar *uri;
	GtkFileChooserConfirmation res;

	uri = gtk_file_chooser_get_uri (dialog);

	/*
	 * FIXME: We have to detect if the file is read-only
	 */

	/* fall back to the default confirmation dialog */
	res = GTK_FILE_CHOOSER_CONFIRMATION_CONFIRM;

	g_free (uri);

	return res;
}

/*
 * "Save as" dialog.
 */
void
gtranslator_save_file_as_dialog (GtkAction * action,
				 GtranslatorWindow *window)
{
	GtkWidget *dialog = NULL;
	GtranslatorTab *current_page;
	GtranslatorPo *po;
	const gchar *filename;
	gchar *uri = NULL;
	gboolean uri_set = FALSE;
	
	if (dialog != NULL) {
		gtk_window_present (GTK_WINDOW (dialog));
		return;
	}

	current_page = gtranslator_window_get_active_tab (window);
	po = gtranslator_tab_get_po (current_page);
	
	dialog = gtranslator_file_chooser_new (GTK_WINDOW (window),
					       FILESEL_SAVE,
					       _("Save file as..."),
					       _gtranslator_application_get_last_dir (GTR_APP));
	
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog),
							TRUE);
	g_signal_connect (dialog,
			  "confirm-overwrite",
			  G_CALLBACK (confirm_overwrite_callback),
			  NULL);
			  
	gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
	
	/*Set the suggested file */
	filename = gtranslator_po_get_filename (po);
	
	uri = g_filename_to_uri (filename, NULL, NULL);
	
	if (uri)
		uri_set = gtk_file_chooser_set_uri (GTK_FILE_CHOOSER (dialog),
						    uri);
	
	g_free (uri);
	
	/*
	 * FIXME: If we can't set the uri we should add a default path and name
	 */
	 
	g_object_set_data (G_OBJECT (dialog),
			   GTR_TAB_SAVE_AS,
			   current_page);
	 
	g_signal_connect (dialog,
			  "response",
			  G_CALLBACK (save_dialog_response_cb),
			  window);

	gtk_widget_show (GTK_WIDGET (dialog));
}

/*
 * A callback for Save
 */
void 
gtranslator_save_current_file_dialog (GtkWidget * widget,
				      GtranslatorWindow *window)
{
	GError *error = NULL;
	GtranslatorTab *current;
	GtranslatorPo *po;
	
	current = gtranslator_window_get_active_tab (window);
	po = gtranslator_tab_get_po (current);
	
	gtranslator_po_save_file (po, &error);
	
	if (error)
	{
		GtkWidget *dialog;
		dialog = gtk_message_dialog_new (GTK_WINDOW (window),
						 GTK_DIALOG_DESTROY_WITH_PARENT,
						 GTK_MESSAGE_WARNING,
						 GTK_BUTTONS_OK,
						 error->message);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		g_clear_error(&error);
		return;
	}
	
	/* We have to change the state of the tab */
	gtranslator_po_set_state(po, GTR_PO_STATE_SAVED);
}

static gboolean
is_duplicated_uri (const GSList *uris, 
		   const gchar  *u)
{
	while (uris != NULL)
	{
		if (strcmp (u, (const gchar*)uris->data) == 0)
			return TRUE;
			
		uris = g_slist_next (uris);
	}
	
	return FALSE;
}

static void
load_file_list(GtranslatorWindow *window,
	       const GSList *uris)
{
	GSList        *uris_to_load = NULL;
	const GSList  *l;
	GError *error = NULL;
	gchar *path;
	GtkWidget *tab;
	
	g_return_if_fail ((uris != NULL) && (uris->data != NULL));

	/* Remove the uris corresponding to documents already open
	 * in "window" and remove duplicates from "uris" list */
	l = uris;
	while (uris != NULL)
	{
		if (!is_duplicated_uri (uris_to_load, uris->data))
		{
			/*We need to now if is already loaded in any tab*/
			tab = gtranslator_window_get_tab_from_uri (window,
								   (const gchar *)uris->data);

			if (tab != NULL)
			{
				if (uris == l)
				{
					gtranslator_window_set_active_tab (window,
									   tab);
				}

			}
			else
			{
				uris_to_load = g_slist_prepend (uris_to_load, 
								uris->data);
			}

		}

		uris = g_slist_next (uris);
	}

	if (uris_to_load == NULL)
		return;
	
	uris_to_load = g_slist_reverse (uris_to_load);
	l = uris_to_load;
	
	while (uris_to_load != NULL)
	{
		g_return_if_fail (uris_to_load->data != NULL);

		path = g_filename_from_uri((const gchar *)uris_to_load->data,
					   NULL, NULL);
		if(!gtranslator_open(path, window, &error))
			break;
		
		g_free(path);
		uris_to_load = g_slist_next (uris_to_load);
	}
	
	/*
	 * Now if there are any error we have to manage it
	 * and free the path
	 */
	if(error != NULL)
	{
		g_free(path);
		
		GtkWidget *dialog;
		/*
		 * We have to show the error in a dialog
		 */
		dialog = gtk_message_dialog_new(GTK_WINDOW(window),
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_ERROR,
						GTK_BUTTONS_CLOSE,
						error->message);
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
		g_error_free(error);
	}
	
	/* Free uris_to_load. Note that l points to the first element of uris_to_load */
	g_slist_free ((GSList *)l);
}


/**
 * gtranslator_actions_load_uris:
 *
 * Ignore non-existing URIs 
 */
void
gtranslator_actions_load_uris (GtranslatorWindow *window,
			       const GSList        *uris)
{	
	g_return_if_fail (GTR_IS_WINDOW (window));
	g_return_if_fail ((uris != NULL) && (uris->data != NULL));
	
	load_file_list (window, uris);
}

static void
save_and_close_document (GtranslatorPo *po,
			 GtranslatorWindow *window)
{
	GtranslatorTab *tab;
	
	gtranslator_save_current_file_dialog (NULL, window);
	
	tab = gtranslator_tab_get_from_document (po);
	
	_gtranslator_window_close_tab (window, tab);
}

static void
close_all_tabs (GtranslatorWindow *window)
{
	GtranslatorNotebook *nb;
	gint pages;
	
	nb = gtranslator_window_get_notebook (window);
	pages = gtk_notebook_get_n_pages (GTK_NOTEBOOK(nb));
	
	while(pages >= 0)
	{		
		gtk_notebook_remove_page (GTK_NOTEBOOK (nb),
					  pages);
		
		pages--;
	}
	
	//FIXME: This has to change once we add the close all documents menuitem
	gtk_widget_destroy (GTK_WIDGET (window));
}

static void
save_and_close_all_documents (GList *unsaved_documents,
			      GtranslatorWindow *window)
{
	GtranslatorTab *tab;
	GList *l;
	GError *error = NULL;
	
	for (l = unsaved_documents; l != NULL; l = g_list_next (l))
	{
		gtranslator_po_save_file (l->data, &error);
		
		if (error)
		{
			GtkWidget *dialog;
			dialog = gtk_message_dialog_new (GTK_WINDOW (window),
							 GTK_DIALOG_DESTROY_WITH_PARENT,
							 GTK_MESSAGE_WARNING,
							 GTK_BUTTONS_OK,
							 error->message);
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
			g_clear_error (&error);
			
			return;
		}
		
		tab = gtranslator_tab_get_from_document (l->data);
		
		_gtranslator_window_close_tab (window, tab);
	}
	
	gtk_widget_destroy (GTK_WIDGET (window));
}

static void
close_confirmation_dialog_response_handler (GtranslatorCloseConfirmationDialog *dlg,
					    gint                          response_id,
					    GtranslatorWindow            *window)
{
	GList *selected_documents;
	gboolean is_closing_all;

	gtk_widget_hide (GTK_WIDGET (dlg));
	
	is_closing_all = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (window),
							     GTR_IS_CLOSING_ALL));

	switch (response_id)
	{
		case GTK_RESPONSE_YES: /* Save and Close */
			selected_documents = gtranslator_close_confirmation_dialog_get_selected_documents (dlg);
			if (selected_documents == NULL)
			{
				if (is_closing_all)
				{
					gtk_widget_destroy (GTK_WIDGET (dlg));
					
					close_all_tabs (window);
					
					return;
				}
				else g_return_if_reached ();
			}
			else
			{
				if (is_closing_all)
				{
					save_and_close_all_documents (selected_documents,
								      window);
				}
				else
				{
					save_and_close_document (selected_documents->data,
								 window);
				}
			}

			g_list_free (selected_documents);

			break;

		case GTK_RESPONSE_NO: /* Close without Saving */
			if (is_closing_all)
			{
				gtk_widget_destroy (GTK_WIDGET (dlg));

				close_all_tabs (window);

				return;
			}
			else
			{
				const GList *unsaved_documents;

				unsaved_documents = gtranslator_close_confirmation_dialog_get_unsaved_documents (dlg);
				g_return_if_fail (unsaved_documents->next == NULL);

				_gtranslator_window_close_tab (window,
							       gtranslator_tab_get_from_document (unsaved_documents->data));
			}

			break;
		default: /* Do not close */
			break;
	}

	gtk_widget_destroy (GTK_WIDGET (dlg));
}

void 
gtranslator_file_close (GtkAction * widget,
			GtranslatorWindow *window)
{
	GtranslatorTab *tab;
	
	tab = gtranslator_window_get_active_tab (window);
	
	g_object_set_data (G_OBJECT (window),
			   GTR_IS_CLOSING_ALL,
			   GINT_TO_POINTER (0));
	
	if (!_gtranslator_tab_can_close (tab))
	{
		GtkWidget     *dlg;

		dlg = gtranslator_close_confirmation_dialog_new_single (GTK_WINDOW (window),
									gtranslator_tab_get_po (tab),
									FALSE);

		g_signal_connect (dlg,
				  "response",
				  G_CALLBACK (close_confirmation_dialog_response_handler),
				  window);

		gtk_widget_show (dlg);
	}
	else 
		_gtranslator_window_close_tab (window, tab);
}

void
gtranslator_file_quit (GtkAction *action,
		       GtranslatorWindow *window)
{	
	GtranslatorNotebook *nb;
	GtranslatorTab *tab;
	GtranslatorPo *po;
	gint pages;
	GList *list = NULL;
	gchar *config_folder;
	gchar *filename;
	GFile *file;
        
        config_folder = gtranslator_utils_get_user_config_dir ();
 	filename = g_build_filename (config_folder,
 				     "profiles.xml",
 				     NULL);
	
	file = g_file_new_for_path (filename);
	
	if (g_file_query_exists (file, NULL)) {
	  g_file_delete (file, NULL, NULL);
	  gtranslator_profile_save_profiles_in_xml (filename);
	}

	nb = gtranslator_window_get_notebook (window);
	pages = gtk_notebook_get_n_pages (GTK_NOTEBOOK(nb));

	while (pages > 0)
	{		
		tab = GTR_TAB (gtk_notebook_get_nth_page (GTK_NOTEBOOK (nb),
							  pages - 1));
		
		po = gtranslator_tab_get_po (tab);
		if (gtranslator_po_get_state (po) == GTR_PO_STATE_MODIFIED)
			list = g_list_prepend (list, po);
		
		pages--;
	}

	g_object_set_data (G_OBJECT (window),
			   GTR_IS_CLOSING_ALL,
			   GINT_TO_POINTER (1));

	if (list != NULL)
	{
		GtkWidget     *dlg;

		dlg = gtranslator_close_confirmation_dialog_new (GTK_WINDOW (window),
								 list, TRUE);

		g_signal_connect (dlg,
				  "response",
				  G_CALLBACK (close_confirmation_dialog_response_handler),
				  window);
		
		g_list_free (list);

		gtk_widget_show (dlg);
	}
	else {
		close_all_tabs (window);
		
		gtk_widget_destroy (GTK_WIDGET (window));
	}
}
