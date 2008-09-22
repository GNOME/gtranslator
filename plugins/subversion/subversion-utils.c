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

#include "command.h"
#include "subversion-utils.h"
#include "utils.h"
#include "statusbar.h"
#include "svn-command.h"
#include "application.h"
#include "profile.h"

#include <string.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

typedef struct _PulseProgressData
{
	GtranslatorStatusbar *status;
	gchar *text;
}PulseProgressData;

void
subversion_utils_report_errors (GtranslatorWindow *window,
				GtranslatorCommand *command,
				guint error_code)
{
	GtkWidget *dialog;
	
	if (error_code)
	{
		gchar *message;
		
		message = gtranslator_command_get_error_message (command);
		dialog = gtk_message_dialog_new (GTK_WINDOW (window),
						 GTK_DIALOG_DESTROY_WITH_PARENT,
						 GTK_MESSAGE_ERROR,
						 GTK_BUTTONS_CLOSE,
						 message);
		g_free (message);
		
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
	}
}

gchar * 
subversion_utils_get_log_from_textview (GtkWidget* textview)
{
	gchar* log;
	GtkTextBuffer* textbuf;
	GtkTextIter iterbegin, iterend;
	gchar* escaped_log;
	
	textbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
	gtk_text_buffer_get_start_iter(textbuf, &iterbegin);
	gtk_text_buffer_get_end_iter(textbuf, &iterend) ;
	log = gtk_text_buffer_get_text(textbuf, &iterbegin, &iterend, FALSE);

	escaped_log = gtranslator_utils_escape_search_text (log);
	return escaped_log;
}

/*guint
subversion_utils_status_bar_progress_pulse (GtranslatorWindow *window, gchar *text)
{
	PulseProgressData *data;
	
	data = g_new0 (PulseProgressData, 1);
	data->status = gtranslator_window_get_statusbar (window);
	data->text = g_strdup (text);
	
	return g_timeout_add_full (G_PRIORITY_DEFAULT, 100,  
				   (GSourceFunc) status_pulse_timer, data,
				   (GDestroyNotify) on_pulse_timer_destroyed);
}*/

void
subversion_utils_from_file_to_file (GInputStream *istream,
				    GOutputStream *ostream)
{
	gsize bytes = 1;
	GError *error = NULL;
	gchar buffer[4096];
	static gint i = 0;
	i++;
	
	while (bytes != 0 && bytes != -1)
	{
		bytes = g_input_stream_read (istream, buffer,
					     sizeof (buffer),
					     NULL, &error);
		if (error)
			break;
		g_output_stream_write (ostream, buffer,
				       sizeof (buffer),
				       NULL, &error);
		if (error)
			break;
	}
	
	if (error)
	{
		g_warning (error->message);
		g_error_free (error);
		error = NULL;
	}
	
	if (!g_output_stream_close (ostream, NULL, &error))
	{
		g_warning (error->message);
		g_error_free (error);
		error = NULL;
	}
	if (!g_input_stream_close (istream, NULL, &error))
	{
		g_warning (error->message);
		g_error_free (error);
	}
}

gchar *
subversion_utils_get_changelog_entry_from_view (GtkWidget *view)
{
	gchar *log;
	GtranslatorProfile *profile;
	gchar *name;
	gchar *email;
	gchar *changelog_entry;
	gchar *date;
	gchar *code;
	
	log = subversion_utils_get_log_from_textview (view);
	
	profile = gtranslator_application_get_active_profile (GTR_APP);
	name = gtranslator_profile_get_author_name (profile);
	email = gtranslator_profile_get_author_email (profile);
	code = gtranslator_profile_get_language_code (profile);
	date = gtranslator_utils_get_current_date ();
	
	//FIXME: Is missing the date
	changelog_entry = g_strdup_printf ("%s  %s  <%s>\n"
					   "\n\t %s.po: %s\n\n", date, name, email,
					   code, log);
	g_free (log);
	g_free (date);
	//g_free (name);
	//g_free (email);
	
	return changelog_entry;
}