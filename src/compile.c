/*
 * (C) 2000-2004 	Fatih Demir <kabalak@kabalak.net>
 *			Ross Golder <ross@golder.org>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *    
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#include "compile.h"
#include "dialogs.h"
#include "page.h"
#include "prefs.h"
#include "utils_gui.h"

/*
 * Set up our temporary file names for the test compile run.
 */
void gtranslator_utils_get_compile_file_names(gchar **test_file, 
	gchar **output_file, gchar **result_file);

void gtranslator_compile_error_dialog(FILE * fs);

/*
 * Clean up the temporary files for the test compile run -- also free the variables.
 */
void gtranslator_utils_remove_compile_files(gchar **test_file,
	gchar **output_file, gchar **result_file);

/*
 * Set up and assign the test file names for the compile process.
 */
void gtranslator_utils_get_compile_file_names(gchar **test_file, 
	gchar **output_file, gchar **result_file)
{
	const char *project_name;
	
	/* TODO: Determine project name from po_file header */
	*test_file=g_strdup_printf("%s/.gtranslator/files/gtranslator-temp-compile-file",
		g_get_home_dir());

	*result_file=g_strdup_printf("%s/.gtranslator/files/gtranslator-compile-result-file",
		g_get_home_dir());

	*output_file=g_strdup_printf("%s/%s.gmo",
		g_get_current_dir(), project_name);
}

/* 
 * TODO: Jump to the message containing first error. Something strange with
 * line/message numbers, maybe we need to convert between them?
 */
void gtranslator_compile_error_dialog(FILE * fs)
{
	gchar buf[2048];
	gint len;
	GtkWidget *dialog, *textbox;
	GtkWidget *scroll;
	GtkTextBuffer *buffer;
	GtkTextIter *iter = NULL;

	dialog = gtranslator_utils_error_dialog(_("An error occurred while msgfmt was executed:\n"));
	buffer = gtk_text_buffer_new(NULL);
	while (TRUE) {
		len = fread(buf, 1, sizeof(buf), fs);
		if (len == 0)
			break;
		gtk_text_buffer_get_end_iter(buffer, iter);
		gtk_text_buffer_insert(buffer, iter, buf, len);
	}
	textbox = gtk_text_view_new_with_buffer(buffer);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(textbox), FALSE);

	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scroll), textbox);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
			 scroll, TRUE, TRUE, 0);
	gtranslator_dialog_show(&dialog, NULL);
}

/*
 * Clean up after the test compile run.
 */
void gtranslator_utils_remove_compile_files(gchar **test_file,
	gchar **output_file, gchar **result_file)
{
	if(*test_file)
	{
		remove(*test_file);
		g_free(*test_file);
	}

	if(*result_file)
	{
		remove(*result_file);
		g_free(*result_file);
	}
	
	if(*output_file)
	{
		/*
		 * Only cleanup if this is wished by the user.
		 */
		if(GtrPreferences.sweep_compile_file)
		{
			remove(*output_file);
		}
		
		g_free(*output_file);
	}
}


/*
 * The compile function
 */
void compile(GtkWidget * widget, gpointer useless)
{
	gchar	*cmd,
		*status,
		*test_file_name,
		*output_file_name,
		*result_file_name;
	GError	*error;
	gchar 	line[128];
	gint 	res = 1;
	FILE 	*fs;

	g_assert(current_page != NULL);

	/*
	 * Check if msgfmt is available on the system.
	 */
	if(!g_find_program_in_path("msgfmt"))
	{
		gtranslator_utils_error_dialog(_("Sorry, msgfmt isn't available on your system!"));
		return;
	}

	gtranslator_utils_get_compile_file_names(&test_file_name, 
		&output_file_name, &result_file_name);

	if (!gtranslator_save_file(current_page->po, test_file_name, &error)) {
		GtkWidget *dialog;
		g_assert(error != NULL);
		dialog = gtk_message_dialog_new(
			GTK_WINDOW(gtranslator_application),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_WARNING,
			GTK_BUTTONS_OK,
			error->message);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		g_clear_error(&error);
		return;
	}

	cmd = g_strdup_printf("msgfmt -v -c -o '%s' '%s' > '%s' 2>&1",
			    output_file_name, test_file_name, result_file_name);
	
	res = system(cmd);
	fs=fopen(result_file_name,"r");

	/*
	 * If there has been an error show an error-box with some kind of
	 *  cleaned error message.
	 */
	if (res != 0)
	{
		gtranslator_compile_error_dialog(fs);
	}
	else
	{
		fgets(line, sizeof(line), fs);
		g_strchomp(line);
		status=g_strdup_printf(_("Compile successful:\n%s"), line);
		gnome_app_message(GNOME_APP(gtranslator_application), status);
		g_free(status);
	}
	
	fclose(fs);
	g_free(cmd);

	gtranslator_utils_remove_compile_files(&test_file_name, 
		&output_file_name, &result_file_name);
}
