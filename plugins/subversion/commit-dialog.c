/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *
 *     Based on anjuta subversion plugin.
 *     Copyright (C) James Liggett 2007 <jrliggett@cox.net>
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

#include "commit-dialog.h"
#include "utils.h"
#include "subversion-plugin.h"
#include "svn-status-command.h"
#include "vcs-status-tree-view.h"
#include "svn-status.h"
#include "subversion-utils.h"
#include "statusbar.h"
#include "svn-commit-command.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <gconf/gconf-client.h>
#include <unistd.h>


#define GTR_COMMIT_DIALOG_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 	(object),	\
						 	GTR_TYPE_COMMIT_DIALOG,     \
						 	GtranslatorCommitDialogPrivate))

#define ADD_CHANGE_LOG_KEY SUBVERSION_BASE_KEY "/add_change_log"

G_DEFINE_TYPE(GtranslatorCommitDialog, gtranslator_commit_dialog, GTK_TYPE_DIALOG)

struct _GtranslatorCommitDialogPrivate
{
	GConfClient *gconf_client;
	
	GtkWidget *main_box;
	GtkWidget *changelog_view;
	GtkWidget *status_view;
	GtkWidget *select_all_button;
	GtkWidget *clear_button;
	GtkWidget *status_progress_bar;
	GtkWidget *add_log_checkbutton;
	
	GtranslatorWindow *window;
	gchar *dirname;
};

static void        send_status_command           (GtranslatorCommitDialog *dlg,
						  GtranslatorWindow *window);

static void
select_all_status_items (GtkButton *select_all_button,
			 GtranslatorVcsStatusTreeView *tree_view)
{
	gtranslator_vcs_status_tree_view_select_all (tree_view);
}

static void
clear_all_status_selections (GtkButton *clear_button,
			     GtranslatorVcsStatusTreeView *tree_view)
{
	gtranslator_vcs_status_tree_view_unselect_all (tree_view);
}

static gboolean
pulse_timer (GtkProgressBar *progress_bar)
{
	gtk_progress_bar_pulse (progress_bar);
	return TRUE;
}

static void
stop_pulse_timer (gpointer timer_id, GtkProgressBar *progress_bar)
{
	g_source_remove (GPOINTER_TO_UINT (timer_id));
}

static void
pulse_progress_bar (GtkProgressBar *progress_bar)
{
	guint timer_id;
	
	timer_id = g_timeout_add (100, (GSourceFunc) pulse_timer, 
							  progress_bar);
	g_object_set_data (G_OBJECT (progress_bar), "pulse-timer-id",
					   GUINT_TO_POINTER (timer_id));
	
	g_object_weak_ref (G_OBJECT (progress_bar),
					   (GWeakNotify) stop_pulse_timer,
					   GUINT_TO_POINTER (timer_id));
}

static void
hide_pulse_progress_bar (GtranslatorCommand *command,
			 guint return_code,
			 GtkProgressBar *progress_bar)
{
	guint timer_id;
	
	/* If the progress bar has already been destroyed, the timer should be 
	 * stopped by stop_pulse_timer */
	if (GTK_IS_PROGRESS_BAR (progress_bar))
	{
		timer_id = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (progress_bar),
								"pulse-timer-id")); 
		
		g_source_remove (GPOINTER_TO_UINT (timer_id));
		gtk_widget_hide (GTK_WIDGET (progress_bar));
	}
}

static void
select_all_files (GtranslatorCommand *command,
		  guint return_code, 
		  GtranslatorVcsStatusTreeView *status_view)
{
	gtranslator_vcs_status_tree_view_select_all (status_view);
}

/* This function is normally intended to disconnect stock data-arrived signal
 * handlers in this file. It is assumed that object is the user data for the 
 * callback. If you use any of the stock callbacks defined here, make sure 
 * to weak ref its target with this callback. Make sure to cancel this ref
 * by connecting cancel_data_arrived_signal_disconnect to the command-finished 
 * signal so we don't try to disconnect signals on a destroyed command. */
static void
disconnect_data_arrived_signals (GtranslatorCommand *command,
				 GObject *object)
{
	guint data_arrived_signal;
	
	if (GTR_IS_COMMAND (command))
	{
		data_arrived_signal = g_signal_lookup ("data-arrived",
						       GTR_TYPE_COMMAND);
		
		g_signal_handlers_disconnect_matched (command,
						      G_SIGNAL_MATCH_DATA,
						      data_arrived_signal,
						      0,
						      NULL,
						      NULL,
						      object);
	}
										  
}

static void 
cancel_data_arrived_signal_disconnect (GtranslatorCommand *command, 
				       guint return_code,
				       GObject *signal_target)
{
	g_object_weak_unref (signal_target, 
			     (GWeakNotify) disconnect_data_arrived_signals,
			     command);
}

static void
on_status_command_finished (GtranslatorCommand *command,
			    guint return_code, 
			    GtranslatorWindow *window)
{
	subversion_utils_report_errors (window, command, return_code);
	
	svn_status_command_destroy (SVN_STATUS_COMMAND (command));
}

static void
on_status_command_data_arrived (GtranslatorCommand *command, 
				GtranslatorVcsStatusTreeView *tree_view)
{
	GQueue *status_queue;
	SvnStatus *status;
	gchar *path;

	status_queue = svn_status_command_get_status_queue (SVN_STATUS_COMMAND (command));

	while (g_queue_peek_head (status_queue))
	{
		status = g_queue_pop_head (status_queue);
		path = svn_status_get_path (status);
		
		gtranslator_vcs_status_tree_view_add (tree_view, path, 
						      svn_status_get_vcs_status (status),
						      FALSE);
		
		svn_status_destroy (status);
		g_free (path);
	}
}

static void
on_commit_command_finished (GtranslatorCommand *command,
			    guint return_code,
			    GtranslatorCommitDialog *dlg)
{
	GtranslatorStatusbar *status;
	
	g_return_if_fail (GTR_IS_WINDOW (dlg->priv->window));

	status = GTR_STATUSBAR (gtranslator_window_get_statusbar (dlg->priv->window));
	
	gtranslator_statusbar_flash_message (status, 0,
					     _("Subversion: Commit complete."));
	
	subversion_utils_report_errors (dlg->priv->window,
					command, return_code);
	
	svn_commit_command_destroy (SVN_COMMIT_COMMAND (command));
}

static void
on_command_info_arrived (GtranslatorCommand *command,
			 GtranslatorWindow *window)
{
	GQueue *info;
	gchar *message;
	GtranslatorStatusbar *status;
	
	status = GTR_STATUSBAR (gtranslator_window_get_statusbar (window));
	info = svn_command_get_info_queue (SVN_COMMAND (command));
	
	while (g_queue_peek_head (info))
	{
		message = g_queue_pop_head (info);
		gtranslator_statusbar_flash_message (status, 0,
						     message);
		g_free (message);
	}
}

static void
add_changelog_to_text_view (GtkWidget *textview)
{
	gchar *log;
	GtkTextBuffer *buf;
	
	buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
	
	log = g_strdup_printf (" * Updated %s translation",
	/*
	 * Translators: Please change LANGUAGE for your language name in ENGLISH.
	 * This string is used to fill the ChangeLog entry
	 */
			       C_("ChangeLog entry", "LANGUAGE"));
	
	gtk_text_buffer_set_text (buf, log, -1);
	g_free (log);
}

static void
add_changelog_entry (GtranslatorCommitDialog *dlg)
{
	guint tmpfd;
	GError *error = NULL;
	gchar *tmp_fname;
	GFile *tmp_file;
	gchar *changelog;
	GFile *changelog_file;
	GFileInputStream *istream;
	GFileOutputStream *ostream;
	gchar *changelog_entry;
	
	changelog = g_build_filename (dlg->priv->dirname,
				      "ChangeLog",
				      NULL);
	
	changelog_file = g_file_new_for_path (changelog);
	if (!g_file_query_exists (changelog_file, NULL))
	{
		g_warning (_("The ChangeLog file '%s' does not exists"),
			   changelog);
		g_free (changelog);
		g_object_unref (changelog_file);
		return;
	}
	g_free (changelog);
	
	tmpfd = g_file_open_tmp (".gtranslator-changelog-XXXXXX",
				 &tmp_fname,
				 &error);
	
	if (tmpfd == -1)
	{
		g_warning (error->message);
		g_error_free (error);
		g_object_unref (changelog_file);
		
		/* In this case we don't have to close the file */
		return;
	}
	
	tmp_file = g_file_new_for_path (tmp_fname);
	
	istream = g_file_read (changelog_file, NULL, &error);
	if (error)
		goto free;
	
	ostream = g_file_append_to (tmp_file, G_FILE_CREATE_NONE,
				    NULL, &error);
	if (error)
		goto free;
	
	/*
	 * We move the current ChangeLog file to the temp file
	 */
	subversion_utils_from_file_to_file (G_INPUT_STREAM (istream),
					    G_OUTPUT_STREAM (ostream));
	
	/*
	 * Then we remove the old changelog_file
	 */
	g_file_delete (changelog_file, NULL, &error);
	if (error)
		goto free;
	
	/*
	 * We create the new one
	 */
	ostream = g_file_create (changelog_file, G_FILE_CREATE_NONE,
				 NULL, &error);
	if (error)
		goto free;

	/*
	 * Now, adding the new changelog entry
	 */
	changelog_entry = subversion_utils_get_changelog_entry_from_view (dlg->priv->changelog_view);
	
	g_output_stream_write (G_OUTPUT_STREAM (ostream), changelog_entry,
			       g_utf8_strlen (changelog_entry, -1),
			       NULL, &error);
	if (error)
		goto free;
	
	istream = g_file_read (tmp_file, NULL, &error);
	if (error)
		goto free;
	
	/* Now, appending from tempfile to ChangeLog file */
	subversion_utils_from_file_to_file (G_INPUT_STREAM (istream),
					    G_OUTPUT_STREAM (ostream));

	/*
	 * Updating the dialog
	 */
	gtranslator_vsc_status_tree_view_clear (GTR_VCS_STATUS_TREE_VIEW (dlg->priv->status_view));
	send_status_command (dlg, dlg->priv->window);
	
free:   if (error)
	{
		g_warning (error->message);
		g_error_free (error);
	}
	g_object_unref (tmp_file);
	close (tmpfd);
	g_free (tmp_fname);
	g_object_unref (changelog_file);
}

static void
dialog_response_handler (GtkDialog *dlg, 
			 gint       res_id)
{
	GtranslatorCommitDialog *commit = GTR_COMMIT_DIALOG (dlg);
	
	switch (res_id)
	{
		case GTK_RESPONSE_OK:
		{
			gchar* log = NULL;
			GList *selected_paths;
			SvnCommitCommand *commit_command;
			//guint pulse_timer_id;
			
			log = subversion_utils_get_log_from_textview (commit->priv->changelog_view);
			if (!g_utf8_strlen (log, -1))
			{
				gint result;
				GtkWidget* dialog = gtk_message_dialog_new (GTK_WINDOW (commit->priv->window), 
									    GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO,
									    GTK_BUTTONS_YES_NO, 
									    _("Are you sure that you want to pass an empty log message?"));
				result = gtk_dialog_run (GTK_DIALOG (dialog));
				gtk_widget_destroy (dialog);
				if (result == GTK_RESPONSE_NO)
				{
					g_free (log);
					break;
				}
			}
			
			selected_paths = gtranslator_vcs_status_tree_view_get_selected (GTR_VCS_STATUS_TREE_VIEW (commit->priv->status_view));

			if (selected_paths == NULL)
			{
				GtkWidget* dialog = gtk_message_dialog_new (GTK_WINDOW (commit->priv->window), 
									    GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO,
									    GTK_BUTTONS_CLOSE, 
									    _("You have to select at least one file to send"));
				gtk_dialog_run (GTK_DIALOG (dialog));
				gtk_widget_destroy (dialog);
				g_free (log);
				break;
			}
			
			if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (commit->priv->add_log_checkbutton)))
			{
				gchar *changelog_path;
				
				add_changelog_entry (commit);
				
				changelog_path = g_build_filename (commit->priv->dirname,
								   "ChangeLog",
								   NULL);
				selected_paths = g_list_append (selected_paths,
								changelog_path);
			}
				    
			commit_command = svn_commit_command_new (selected_paths, 
								 log,
								 FALSE);
			g_free (log);
			
			svn_command_free_path_list (selected_paths);
		
			/*pulse_timer_id = status_bar_progress_pulse (data->plugin,
								    _("Subversion: " 
								      "Committing changes "
								      "to the "
								      "repository..."));
			
			g_signal_connect (G_OBJECT (commit_command), "command-finished",
					  G_CALLBACK (stop_status_bar_progress_pulse),
					  GUINT_TO_POINTER (pulse_timer_id));*/
			
			g_signal_connect (G_OBJECT (commit_command), "command-finished",
					  G_CALLBACK (on_commit_command_finished),
					  commit);
			
			g_signal_connect (G_OBJECT (commit_command), "data-arrived",
					  G_CALLBACK (on_command_info_arrived),
					  commit->priv->window);
			
			gtranslator_command_start (GTR_COMMAND (commit_command));
		
			gtk_widget_hide (GTK_WIDGET (dlg));
			break;
		}
		default:
			gtk_widget_destroy (GTK_WIDGET (dlg));
	}
}

static void
send_status_command (GtranslatorCommitDialog *dlg,
		     GtranslatorWindow *window)
{
	SvnStatusCommand *status_command;
	GtranslatorTab *tab;
	GtranslatorPo *po;
	
	/* Setting up */
	dlg->priv->window = window;
	tab = gtranslator_window_get_active_tab (window);
	po = gtranslator_tab_get_po (tab);
	g_free (dlg->priv->dirname);
	dlg->priv->dirname = g_path_get_dirname (gtranslator_po_get_filename (po));
	
	status_command = svn_status_command_new (dlg->priv->dirname,
						 TRUE, TRUE);
	
	g_signal_connect (G_OBJECT (status_command), "command-finished",
			  G_CALLBACK (select_all_files),
			  dlg->priv->status_view);
	
	pulse_progress_bar (GTK_PROGRESS_BAR (dlg->priv->status_progress_bar));
	
	g_signal_connect (G_OBJECT (status_command), "command-finished",
			  G_CALLBACK (cancel_data_arrived_signal_disconnect),
			  dlg->priv->status_view);
	
	g_signal_connect (G_OBJECT (status_command), "command-finished",
			  G_CALLBACK (hide_pulse_progress_bar),
			  dlg->priv->status_progress_bar);
	
	g_signal_connect (G_OBJECT (status_command), "command-finished",
			  G_CALLBACK (on_status_command_finished),
			  dlg->priv->window);
	
	g_signal_connect (G_OBJECT (status_command), "data-arrived",
			  G_CALLBACK (on_status_command_data_arrived),
			  dlg->priv->status_view);
	
	g_object_weak_ref (G_OBJECT (dlg->priv->status_view),
			   (GWeakNotify) disconnect_data_arrived_signals,
			   status_command);
	
	gtranslator_command_start (GTR_COMMAND (status_command));
}

static void
on_add_log_checkbutton_toggled (GtkToggleButton *button,
				GtranslatorCommitDialog *dlg)
{
	g_return_if_fail (GTR_IS_COMMIT_DIALOG (dlg));
	
	if (!gconf_client_key_is_writable (dlg->priv->gconf_client,
					   ADD_CHANGE_LOG_KEY,
					   NULL))
		return;

	gconf_client_set_bool (dlg->priv->gconf_client,
			       ADD_CHANGE_LOG_KEY,
			       gtk_toggle_button_get_active (button),
			       NULL);
}

static void
gtranslator_commit_dialog_init (GtranslatorCommitDialog *dlg)
{
	gboolean ret;
	GtkWidget *error_widget;
	GtkWidget *sw_view;
	gboolean data;
	
	dlg->priv = GTR_COMMIT_DIALOG_GET_PRIVATE (dlg);
	
	dlg->priv->gconf_client = gconf_client_get_default ();
	dlg->priv->dirname = NULL;
	
	gtk_dialog_add_buttons (GTK_DIALOG (dlg),
				GTK_STOCK_CANCEL,
				GTK_RESPONSE_CANCEL,
				GTK_STOCK_OK,
				GTK_RESPONSE_OK,
				NULL);
	
	gtk_window_set_title (GTK_WINDOW (dlg), _("Commit Changes"));
	gtk_window_set_default_size (GTK_WINDOW (dlg), 600, 600);
	gtk_window_set_resizable (GTK_WINDOW (dlg), TRUE);
	gtk_dialog_set_has_separator (GTK_DIALOG (dlg), FALSE);
	gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);
	
	/* HIG defaults */
	gtk_container_set_border_width (GTK_CONTAINER (dlg), 5);
	gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (dlg)->vbox), 2); /* 2 * 5 + 2 = 12 */
	gtk_container_set_border_width (GTK_CONTAINER (GTK_DIALOG (dlg)->action_area), 5);
	gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (dlg)->action_area), 4);
	
	g_signal_connect (dlg,
			  "response",
			  G_CALLBACK (dialog_response_handler),
			  NULL);
	
	/*Glade*/
	ret = gtranslator_utils_get_glade_widgets (GLADE_FILE,
		"commit_main_box",
		&error_widget,
		
		"commit_main_box", &dlg->priv->main_box,
		"changelog_view", &dlg->priv->changelog_view,
		"add_log_checkbutton", &dlg->priv->add_log_checkbutton,
		"select_all_button", &dlg->priv->select_all_button,
		"clear_button", &dlg->priv->clear_button,
		"status_progress_bar", &dlg->priv->status_progress_bar,
		"sw_view", &sw_view,
		
		NULL);
	
	if(!ret)
	{
		gtk_widget_show (error_widget);
		gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dlg)->vbox),
					     error_widget);
		
		return;
	}
	
	/* status view */
	dlg->priv->status_view = gtranslator_vcs_status_tree_view_new ();
	gtk_widget_show (dlg->priv->status_view);
	gtk_container_add (GTK_CONTAINER (sw_view),
			   dlg->priv->status_view);
	
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dlg)->vbox),
			    dlg->priv->main_box, TRUE, TRUE, 0);
	
	gtk_container_set_border_width (GTK_CONTAINER (dlg->priv->main_box), 5);
	
	/*
	 * We add the ChangeLog
	 */
	add_changelog_to_text_view (dlg->priv->changelog_view);
	
	g_signal_connect (dlg->priv->add_log_checkbutton, "toggled",
			  G_CALLBACK (on_add_log_checkbutton_toggled), dlg);
	
	g_signal_connect (G_OBJECT (dlg->priv->select_all_button), "clicked",
			  G_CALLBACK (select_all_status_items),
			  dlg->priv->status_view);
	
	g_signal_connect (G_OBJECT (dlg->priv->clear_button), "clicked",
			  G_CALLBACK (clear_all_status_selections),
			  dlg->priv->status_view);
	
	/*
	 * Set add_log_checkbutton value
	 */
	data = gconf_client_get_bool (dlg->priv->gconf_client,
				      ADD_CHANGE_LOG_KEY,
				      NULL);
	
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dlg->priv->add_log_checkbutton),
				      data);
}

static void
gtranslator_commit_dialog_finalize (GObject *object)
{
	GtranslatorCommitDialog *dlg = GTR_COMMIT_DIALOG (object);
	
	gconf_client_suggest_sync (dlg->priv->gconf_client, NULL);

	g_object_unref (G_OBJECT (dlg->priv->gconf_client));
	
	g_free (dlg->priv->dirname);
	
	G_OBJECT_CLASS (gtranslator_commit_dialog_parent_class)->finalize (object);
}

static void
gtranslator_commit_dialog_class_init (GtranslatorCommitDialogClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorCommitDialogPrivate));

	object_class->finalize = gtranslator_commit_dialog_finalize;
}

void
gtranslator_show_commit_dialog (GtranslatorWindow *window)
{
	static GtranslatorCommitDialog *dlg = NULL;
	
	g_return_if_fail (GTR_IS_WINDOW (window));
	
	if(dlg == NULL)
	{
		dlg = g_object_new (GTR_TYPE_COMMIT_DIALOG, NULL);
		
		gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg),
						    TRUE);

		send_status_command (dlg, window);
		
		g_signal_connect (dlg,
				  "destroy",
				  G_CALLBACK (gtk_widget_destroyed),
				  &dlg);
		gtk_widget_show (GTK_WIDGET (dlg));
	}
	else
	{
		/* We clean the current status treeview and send another
		 status command */
		gtranslator_vsc_status_tree_view_clear (GTR_VCS_STATUS_TREE_VIEW (dlg->priv->status_view));
		send_status_command (dlg, dlg->priv->window);
	}
	
	if (GTK_WINDOW (window) != gtk_window_get_transient_for (GTK_WINDOW (dlg)))
	{
		gtk_window_set_transient_for (GTK_WINDOW (dlg),
					      GTK_WINDOW (window));
	}

	gtk_window_present (GTK_WINDOW (dlg));
}
