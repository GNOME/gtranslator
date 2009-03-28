/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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

#include "diff-dialog.h"
#include "subversion-plugin.h"
#include "svn-diff-command.h"
#include "svn-cat-command.h"
#include "utils.h"
#include "svn-command.h"
#include "statusbar.h"
#include "subversion-utils.h"
#include "subversion-plugin.h"
#include "tab.h"
#include "po.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <gconf/gconf-client.h>
#include <unistd.h>

#define USE_CONFIGURED_PROGRAM_KEY SUBVERSION_BASE_KEY "/use_configured_program"
#define SAVE_DIFF_KEY SUBVERSION_BASE_KEY "/save_diff"

#define GTR_DIFF_DIALOG_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 	(object),	\
						 	GTR_TYPE_DIFF_DIALOG,     \
						 	GtranslatorDiffDialogPrivate))


G_DEFINE_TYPE(GtranslatorDiffDialog, gtranslator_diff_dialog, GTK_TYPE_DIALOG)

struct _GtranslatorDiffDialogPrivate
{
	GConfClient *gconf_client;
	
	GtkWidget *main_box;
	GtkWidget *use_configured_program;
	GtkWidget *save_diff;
	GtkWidget *diff_filename_entry;
	GtkWidget *diff_find;
	
	gchar *filename;

	GtranslatorWindow *window;
};

static void
on_diff_command_finished (GtranslatorCommand *command,
			  guint return_code,
			  GtranslatorDiffDialog *dlg)
{
	GtranslatorStatusbar *status;
	
	status = GTR_STATUSBAR (gtranslator_window_get_statusbar (dlg->priv->window));
	
	gtranslator_statusbar_flash_message (status, 0,
					     _("Subversion: Diff complete."));
	
	subversion_utils_report_errors (dlg->priv->window, 
					command, return_code);
	
	if (SVN_IS_DIFF_COMMAND (command))
		svn_diff_command_destroy (SVN_DIFF_COMMAND (command));	
	else svn_cat_command_destroy (SVN_CAT_COMMAND (command));
}

static void
show_in_program (const gchar *program_name,
		 const gchar *line_arg,
		 const gchar *path1,
		 const gchar *path2)
{
	gchar *open[5];

	if (g_find_program_in_path (program_name))
	{
		open[0] = g_strdup (program_name);
	}
	else
	{
		GtkWidget *dialog;
		
		dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL,
						 GTK_DIALOG_MODAL,
						 GTK_BUTTONS_CLOSE,
						 _("Please install %s to be able to diff the file"),
						 program_name);
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
	}
	
	open[1] = g_strdup (path1);
	open[2] = g_strdup (path2);
	
	if (strcmp (line_arg, ""))
	{
		open[3] = g_strdup_printf ("%s",line_arg);
		open[4] = NULL;
	}
	else open[3] = NULL;
					
	gdk_spawn_on_screen (gdk_screen_get_default (),
			     NULL,
			     open,
			     NULL,
			     G_SPAWN_SEARCH_PATH,
			     NULL,
			     NULL, NULL, NULL);
	g_free (open[0]);
	g_free (open[1]);
	g_free (open[2]);
	g_free (open[3]);
}

static void
on_cat_command_info_arrived (GtranslatorCommand *command,
			     GtranslatorDiffDialog *dlg)
{
	GQueue *output;
	gchar *line;
	guint tmpfd;
	GError *error = NULL;
	gchar *tmp_fname;
	GFile *tmp_file;
	GFileOutputStream *ostream;
	gboolean check;
	gchar *program_name;
	gchar *program_arg;
	
	output = svn_cat_command_get_output (SVN_CAT_COMMAND (command));
	
	tmpfd = g_file_open_tmp (".gtranslator-changelog-XXXXXX",
				 &tmp_fname,
				 &error);
	
	if (tmpfd == -1)
	{
		g_warning (error->message);
		g_error_free (error);
		
		/* In this case we don't have to close the file */
		return;
	}
	
	tmp_file = g_file_new_for_path (tmp_fname);
		    
	ostream = g_file_append_to (tmp_file, G_FILE_CREATE_NONE,
				    NULL, &error);
	
	while (g_queue_peek_head (output))
	{
		line = g_queue_pop_head (output);
		
		g_output_stream_write (G_OUTPUT_STREAM (ostream), line,
				       strlen (line),
				       NULL, &error);
		
		g_free (line);
		
		if (error)
			break;
	}
	
	if (error)
	{
		g_warning (error->message);
		g_error_free (error);
		error = NULL;
	}
	
	if (!g_output_stream_close (G_OUTPUT_STREAM (ostream), NULL, &error))
	{
		g_warning (error->message);
		g_error_free (error);
	}
	
	/*
	 * Check if we have to show it in an external diff program
	 */
	check = gconf_client_get_bool (dlg->priv->gconf_client,
				       USE_CONFIGURED_PROGRAM_KEY,
				       NULL);
	
	if (check)
	{
		program_name = gconf_client_get_string (dlg->priv->gconf_client,
							PROGRAM_NAME_KEY,
							NULL);
		program_arg = gconf_client_get_string (dlg->priv->gconf_client,
						       LINE_ARGUMENT_KEY,
						       NULL);
		
		show_in_program (program_name, program_arg,
				 tmp_fname, dlg->priv->filename);
		
		g_free (program_name);
		g_free (program_arg);
	}
	
	close (tmpfd);
	g_free (tmp_fname);
	g_object_unref (tmp_file);
}

static void
on_diff_command_info_arrived (GtranslatorCommand *command,
			      GtranslatorDiffDialog *dlg)
{
	GQueue *output;
	gchar *line;
	GError *error = NULL;
	GFile *tmp_file;
	const gchar *tmp_fname;
	GFileOutputStream *ostream;
	
	output = svn_diff_command_get_output (SVN_DIFF_COMMAND (command));
	
	tmp_fname = gtk_entry_get_text (GTK_ENTRY (dlg->priv->diff_filename_entry));
	
	tmp_file = g_file_new_for_path (tmp_fname);
	
	if (g_file_query_exists (tmp_file, NULL))
	{
		if (!g_file_delete (tmp_file, NULL, &error))
		{
			g_warning (error->message);
			g_error_free (error);
			
			g_object_unref (tmp_file);
			return;
		}
	}
	
	ostream = g_file_create (tmp_file, G_FILE_CREATE_NONE,
				 NULL, &error);
	
	while (g_queue_peek_head (output))
	{
		line = g_queue_pop_head (output);
		
		g_output_stream_write (G_OUTPUT_STREAM (ostream), line,
				       strlen (line),
				       NULL, &error);
		
		g_free (line);
		
		if (error)
			break;
	}
	
	if (error)
	{
		g_warning (error->message);
		g_error_free (error);
		error = NULL;
	}
	
	if (!g_output_stream_close (G_OUTPUT_STREAM (ostream), NULL, &error))
	{
		g_warning (error->message);
		g_error_free (error);
	}
	
	g_object_unref (tmp_file);
}

static void
send_diff_command (GtranslatorDiffDialog *dlg)
{
	SvnDiffCommand *diff_command;
	
	diff_command = svn_diff_command_new (dlg->priv->filename,
					     SVN_DIFF_REVISION_NONE,
					     SVN_DIFF_REVISION_NONE,
					     TRUE);
	
	g_signal_connect (G_OBJECT (diff_command), "command-finished",
			  G_CALLBACK (on_diff_command_finished),
			  dlg);
	
	g_signal_connect (G_OBJECT (diff_command), "data-arrived",
			  G_CALLBACK (on_diff_command_info_arrived),
			  dlg);
	
	gtranslator_command_start (GTR_COMMAND (diff_command));
}

static void
send_cat_command (GtranslatorDiffDialog *dlg)
{
	SvnCatCommand *cat_command;
	
	cat_command = svn_cat_command_new (dlg->priv->filename,
					   SVN_DIFF_REVISION_NONE);
	
	g_signal_connect (G_OBJECT (cat_command), "command-finished",
			  G_CALLBACK (on_diff_command_finished),
			  dlg);
	
	g_signal_connect (G_OBJECT (cat_command), "data-arrived",
			  G_CALLBACK (on_cat_command_info_arrived),
			  dlg);
	
	gtranslator_command_start (GTR_COMMAND (cat_command));
}

static void
dialog_response_handler (GtranslatorDiffDialog *dlg, 
			 gint       res_id)
{
	switch (res_id)
	{
		case GTK_RESPONSE_OK:
		{	
			if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dlg->priv->save_diff)))
				send_diff_command (dlg);
			if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dlg->priv->use_configured_program)))
				send_cat_command (dlg);
			gtk_widget_hide (GTK_WIDGET (dlg));
			break;
		}
		default:
			gtk_widget_hide (GTK_WIDGET (dlg));
	}
}

static void
setup_diff_filename_entry (GtranslatorDiffDialog *dlg)
{
	gchar *file;
	
	file = g_strdup_printf ("%s.diff", dlg->priv->filename);
	
	gtk_entry_set_text (GTK_ENTRY (dlg->priv->diff_filename_entry),
			    file);
	g_free (file);
}

static void
use_configured_program_toggled (GtkToggleButton *button,
				GtranslatorDiffDialog *dlg)
{
	g_return_if_fail (GTK_IS_TOGGLE_BUTTON (button));
	
	if (!gconf_client_key_is_writable (dlg->priv->gconf_client,
					   USE_CONFIGURED_PROGRAM_KEY,
					   NULL))
		return;

	gconf_client_set_bool (dlg->priv->gconf_client,
			       USE_CONFIGURED_PROGRAM_KEY,
			       gtk_toggle_button_get_active (button),
			       NULL);
}

static void
save_diff_toggled (GtkToggleButton *button,
		   GtranslatorDiffDialog *dlg)
{
	gboolean active;
	
	g_return_if_fail (GTK_IS_TOGGLE_BUTTON (button));
	
	active = gtk_toggle_button_get_active (button);
	
	if (!gconf_client_key_is_writable (dlg->priv->gconf_client,
					   SAVE_DIFF_KEY,
					   NULL))
		return;

	gconf_client_set_bool (dlg->priv->gconf_client,
			       SAVE_DIFF_KEY,
			       active,
			       NULL);
	

	
	gtk_widget_set_sensitive (dlg->priv->diff_filename_entry, active);
	gtk_widget_set_sensitive (dlg->priv->diff_find, active);
}

static void
on_diff_find_clicked (GtkButton *button,
		      GtranslatorDiffDialog *dlg)
{
	GtkWidget *dialog;
	gint res;
	
	dialog = gtk_file_chooser_dialog_new (_("Diff file"),
					      GTK_WINDOW (dlg->priv->window),
					      GTK_FILE_CHOOSER_ACTION_SAVE,
					      GTK_STOCK_CANCEL,
					      GTK_RESPONSE_CANCEL,
					      GTK_STOCK_OK,
					      GTK_RESPONSE_OK,
					      NULL);
	res = gtk_dialog_run (GTK_DIALOG (dialog));
	switch (res)
	{
		case GTK_RESPONSE_OK: 
		{
			gchar *filename;
			
			filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
			gtk_entry_set_text (GTK_ENTRY (dlg->priv->diff_filename_entry),
					    filename);
			g_free (filename);
			break;
		}
		default:
			break;
	}
	gtk_widget_destroy (dialog);
}

static void
set_values (GtranslatorDiffDialog *dlg)
{
	gboolean data;
	
	/* Use configured program */
	data = gconf_client_get_bool (dlg->priv->gconf_client,
				      USE_CONFIGURED_PROGRAM_KEY,
				      NULL);
	
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dlg->priv->use_configured_program),
				      data);
	
	/* Save diff */
	data = gconf_client_get_bool (dlg->priv->gconf_client,
				      SAVE_DIFF_KEY,
				      NULL);
	
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dlg->priv->save_diff),
				      data);
}

static void
gtranslator_diff_dialog_init (GtranslatorDiffDialog *dlg)
{
	gboolean ret;
	GtkWidget *error_widget;
	gboolean active;
	
	dlg->priv = GTR_DIFF_DIALOG_GET_PRIVATE (dlg);
	
	dlg->priv->gconf_client = gconf_client_get_default ();
	dlg->priv->filename = NULL;
	
	gtk_dialog_add_buttons (GTK_DIALOG (dlg),
				GTK_STOCK_CLOSE,
				GTK_RESPONSE_CLOSE,
				GTK_STOCK_OK,
				GTK_RESPONSE_OK,
				NULL);
	
	gtk_window_set_title (GTK_WINDOW (dlg), _("Diff"));
	gtk_window_set_default_size (GTK_WINDOW (dlg), 500, 300);
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
		"diff_main_box",
		&error_widget,
		
		"diff_main_box", &dlg->priv->main_box,
		"use_configured_program", &dlg->priv->use_configured_program,
		"save_diff", &dlg->priv->save_diff,
		"diff_filename_entry", &dlg->priv->diff_filename_entry,
		"diff_find", &dlg->priv->diff_find,
		
		NULL);
	
	if(!ret)
	{
		gtk_widget_show (error_widget);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dlg)->vbox),
				    error_widget,
				    TRUE,
				    TRUE,
				    0);
		
		return;
	}
	
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dlg)->vbox),
			    dlg->priv->main_box, TRUE, TRUE, 0);
	
	gtk_container_set_border_width (GTK_CONTAINER (dlg->priv->main_box), 5);
	
	g_signal_connect (dlg->priv->use_configured_program, "toggled",
			  G_CALLBACK (use_configured_program_toggled), dlg);
	
	g_signal_connect (dlg->priv->save_diff, "toggled",
			  G_CALLBACK (save_diff_toggled), dlg);
	
	g_signal_connect (dlg->priv->diff_find, "clicked",
			  G_CALLBACK (on_diff_find_clicked), dlg);
	
	/*
	 * Set values
	 */
	set_values (dlg);
	
	active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dlg->priv->save_diff));
	gtk_widget_set_sensitive (dlg->priv->diff_filename_entry, active);
	gtk_widget_set_sensitive (dlg->priv->diff_find, active);
}

static void
gtranslator_diff_dialog_finalize (GObject *object)
{
	GtranslatorDiffDialog *dlg = GTR_DIFF_DIALOG (object);
	
	gconf_client_suggest_sync (dlg->priv->gconf_client, NULL);

	g_object_unref (G_OBJECT (dlg->priv->gconf_client));
	
	g_free (dlg->priv->filename);
	
	G_OBJECT_CLASS (gtranslator_diff_dialog_parent_class)->finalize (object);
}

static void
gtranslator_diff_dialog_class_init (GtranslatorDiffDialogClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorDiffDialogPrivate));

	object_class->finalize = gtranslator_diff_dialog_finalize;
}

void
gtranslator_show_diff_dialog (GtranslatorWindow *window)
{
	static GtranslatorDiffDialog *dlg = NULL;
	GtranslatorTab *tab;
	GtranslatorPo *po;
	GFile *location;
	
	g_return_if_fail (GTR_IS_WINDOW (window));
	
	tab = gtranslator_window_get_active_tab (window);
	po = gtranslator_tab_get_po (tab);
	location = gtranslator_po_get_location (po);
	
	if(dlg == NULL)
	{
		dlg = g_object_new (GTR_TYPE_DIFF_DIALOG, NULL);
		
		gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg),
						    TRUE);
		
		dlg->priv->window = window;
		
		g_signal_connect (dlg,
				  "destroy",
				  G_CALLBACK (gtk_widget_destroyed),
				  &dlg);

		gtk_widget_show (GTK_WIDGET (dlg));
	}
	
	//As we are not destroying the dialog we have to free the filename
	g_free (dlg->priv->filename);
	dlg->priv->filename = g_file_get_path (location);
	setup_diff_filename_entry (dlg);
	
	if (GTK_WINDOW (window) != gtk_window_get_transient_for (GTK_WINDOW (dlg)))
	{
		gtk_window_set_transient_for (GTK_WINDOW (dlg),
					      GTK_WINDOW (window));
	}

	gtk_window_present (GTK_WINDOW (dlg));
}
